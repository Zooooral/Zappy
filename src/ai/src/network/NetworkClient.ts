import { EventEmitter } from "events";
import { logger } from "../logger";
import {
    GameState,
    InventoryItem,
    BroadcastMessage,
    LookResult,
    ElevationResult,
} from "./types";
import { ConnectionManager } from "./ConnectionManager";
import { CommandQueue } from "./CommandQueue";
import { MessageParser, ParsedMessage } from "./MessageParser";

export class NetworkClient extends EventEmitter {
    private connectionManager: ConnectionManager;
    private commandQueue: CommandQueue;
    private messageParser: MessageParser;

    constructor(host: string, port: number) {
        super();
        this.connectionManager = new ConnectionManager(host, port);
        this.commandQueue = new CommandQueue();
        this.messageParser = new MessageParser();
        this.setupEventHandlers();
    }

    private setupEventHandlers(): void {
        this.connectionManager.on("connected", () => {
            this.emit("connected");
        });

        this.connectionManager.on("authenticated", (gameState: GameState) => {
            this.emit("authenticated", gameState);
        });

        this.connectionManager.on("disconnected", () => {
            this.commandQueue.rejectAll("Connection closed");
            this.emit("disconnected");
        });

        this.connectionManager.on("error", (error: Error) => {
            this.commandQueue.rejectAll(`Socket error: ${error.message}`);
            this.emit("error", error);
        });

        this.connectionManager.on("message", (message: string) => {
            this.handleMessage(message);
        });

        this.connectionManager.on("welcome", () => {
            this.emit("welcome");
        });

        this.connectionManager.on("clientsNumber", (availableSlots: number) => {
            this.emit("clientsNumber", availableSlots);
        });
    }

    private handleMessage(message: string): void {
        const parsed = this.messageParser.parseMessage(message);

        switch (parsed.type) {
            case "broadcast":
                if (parsed.data) {
                    logger.info(
                        `[BROADCAST] Direction ${parsed.data.direction}: ${parsed.data.message}`
                    );
                    this.emit("broadcast", parsed.data);
                }
                break;

            case "ejection":
                if (parsed.data !== null) {
                    logger.warn(`[EJECTED] From direction: ${parsed.data}`);
                    this.emit("ejected", parsed.data);
                }
                break;

            case "elevation_start":
                break;

            case "death":
                this.emit("dead");
                this.commandQueue.resolveNext("dead");
                break;

            case "command_response":
                try {
                    if (parsed.data && parsed.data.newLevel) {
                        this.connectionManager.updatePlayerLevel(parsed.data.newLevel);
                    }
                    this.commandQueue.resolveNext(parsed.data);
                } catch (error) {
                    this.commandQueue.rejectNext(error as Error);
                }
                break;
        }
    }

    private async sendCommand(
        command: string,
        timeoutMs: number = 15000
    ): Promise<any> {
        return new Promise(async (resolve, reject) => {
            if (!this.connectionManager.isConnected()) {
                reject(new Error("Not connected or authenticated"));
                return;
            }

            if (!this.commandQueue.canAddCommand()) {
                await this.commandQueue.waitForSlot();
            }

            this.commandQueue.addCommand(command, resolve, reject, timeoutMs);

            const dataToSend = command + "\n";
            this.connectionManager.writeToSocket(dataToSend);
        });
    }

    public connect(): Promise<void> {
        return this.connectionManager.connect();
    }

    public authenticate(teamName: string): Promise<void> {
        return this.connectionManager.authenticate(teamName);
    }

    public async connectWithRetry(
        teamName: string,
        maxRetries: number = 10,
        retryDelay: number = 5000
    ): Promise<void> {
        return this.connectionManager.connectWithRetry(
            teamName,
            maxRetries,
            retryDelay
        );
    }

    public disconnect(): void {
        this.connectionManager.disconnect();
    }

    public async moveForward(): Promise<boolean> {
        const result = await this.sendCommand("Forward");
        if (result) {
            this.emit("moved");
        }
        return result;
    }

    public async turnRight(): Promise<boolean> {
        const result = await this.sendCommand("Right");
        if (result) {
            const currentDirection =
                this.connectionManager.getGameState().playerDirection;
            const newDirection = (currentDirection + 1) % 4;
            this.connectionManager.updatePlayerDirection(newDirection);
            this.emit("turned", "right");
        }
        return result;
    }

    public async turnLeft(): Promise<boolean> {
        const result = await this.sendCommand("Left");
        if (result) {
            const currentDirection =
                this.connectionManager.getGameState().playerDirection;
            const newDirection = (currentDirection + 3) % 4;
            this.connectionManager.updatePlayerDirection(newDirection);
            this.emit("turned", "left");
        }
        return result;
    }

    public async look(): Promise<LookResult> {
        return await this.sendCommand("Look");
    }

    public async getInventory(): Promise<InventoryItem> {
        return await this.sendCommand("Inventory");
    }

    public async getAvailableConnections(): Promise<number> {
        return await this.sendCommand("Connect_nbr");
    }

    public async broadcast(message: string): Promise<boolean> {
        return await this.sendCommand(`Broadcast ${message}`);
    }

    public async fork(): Promise<boolean> {
        return await this.sendCommand("Fork");
    }

    public async eject(): Promise<boolean> {
        return await this.sendCommand("Eject");
    }

    public async take(object: string): Promise<boolean> {
        return await this.sendCommand(`Take ${object}`);
    }

    public async set(object: string): Promise<boolean> {
        return await this.sendCommand(`Set ${object}`);
    }

    public async incantation(): Promise<ElevationResult> {
        return await this.sendCommand("Incantation");
    }

    public isConnected(): boolean {
        return this.connectionManager.isConnected();
    }

    public getGameState(): GameState {
        return this.connectionManager.getGameState();
    }

    public getNetworkStats(): any {
        return {
            connected: this.connectionManager.isConnected(),
            ...this.commandQueue.getStats(),
        };
    }
}