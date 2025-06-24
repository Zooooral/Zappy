import * as net from "net";
import { EventEmitter } from "events";
import { logger } from "../logger";
import { GameState } from "./types";
import { MessageParser, HandshakeMessage } from "./MessageParser";

export class ConnectionManager extends EventEmitter {
    private socket: net.Socket;
    private host: string;
    private port: number;
    private connected: boolean = false;
    private authenticated: boolean = false;
    private messageBuffer: string = "";
    private parser: MessageParser;

    public gameState: GameState = {
        worldWidth: 0,
        worldHeight: 0,
        clientsAvailable: 0,
        playerLevel: 1,
        playerDirection: 0,
    };

    constructor(host: string, port: number) {
        super();
        this.host = host;
        this.port = port;
        this.socket = new net.Socket();
        this.parser = new MessageParser();
        this.setupSocketHandlers();
    }

    private setupSocketHandlers(): void {
        this.socket.on("connect", () => {
            this.connected = true;
            logger.info(`Connected to server at ${this.host}:${this.port}`);
            this.emit("connected");
        });

        this.socket.on("data", (data: Buffer) => {
            this.messageBuffer += data.toString();
            this.processMessages();
        });

        this.socket.on("close", () => {
            this.connected = false;
            this.authenticated = false;
            logger.info("Disconnected from server");
            this.emit("disconnected");
        });

        this.socket.on("error", (error: Error) => {
            logger.error("Socket error:", error);
            this.emit("error", error);
        });
    }

    private processMessages(): void {
        const messages = this.messageBuffer.split("\n");
        this.messageBuffer = messages.pop() || "";

        for (const message of messages) {
            if (message.trim()) {
                if (!this.authenticated) {
                    this.handleHandshakeMessage(message.trim());
                } else {
                    this.emit("message", message.trim());
                }
            }
        }
    }

    private handleHandshakeMessage(message: string): void {
        const parsed = this.parser.parseHandshake(message);

        switch (parsed.type) {
            case "welcome":
                this.emit("welcome");
                break;
            case "clients_available":
                this.gameState.clientsAvailable = parsed.data;
                this.emit("clientsNumber", this.gameState.clientsAvailable);
                break;
            case "world_size":
                this.gameState.worldWidth = parsed.data.width;
                this.gameState.worldHeight = parsed.data.height;
                this.authenticated = true;
                this.emit("authenticated", this.gameState);
                logger.info(
                    `Authenticated! World size: ${parsed.data.width}x${parsed.data.height}, Available slots: ${this.gameState.clientsAvailable}`
                );
                break;
        }
    }

    public connect(): Promise<void> {
        return new Promise((resolve, reject) => {
            this.socket.connect(this.port, this.host, () => resolve());
            this.socket.on("error", reject);
        });
    }

    public authenticate(teamName: string): Promise<void> {
        return new Promise((resolve, reject) => {
            this.once("welcome", () => {
                logger.info(`[SENDING TEAM NAME]: ${teamName}`);
                this.socket.write(teamName + "\n");
            });

            this.once("clientsNumber", (availableSlots: number) => {
                if (availableSlots === 0) {
                    reject(new Error("No available slots for this team"));
                    return;
                }
            });

            this.once("authenticated", () => resolve());
            this.once("error", reject);
        });
    }

    public async connectWithRetry(
        teamName: string,
        maxRetries: number = 10,
        retryDelay: number = 5000
    ): Promise<void> {
        for (let attempt = 1; attempt <= maxRetries; attempt++) {
            try {
                await this.connect();
                await this.authenticate(teamName);
                logger.info(`Successfully connected on attempt ${attempt}`);
                return;
            } catch (error) {
                if (
                    error instanceof Error &&
                    error.message.includes("No available slots")
                ) {
                    logger.warn(
                        `No slots available, attempt ${attempt}/${maxRetries}. Retrying in ${retryDelay}ms...`
                    );

                    if (attempt === maxRetries) {
                        throw new Error(
                            `Failed to connect after ${maxRetries} attempts: No slots available`
                        );
                    }

                    this.disconnect();
                    await new Promise((resolve) => setTimeout(resolve, retryDelay));
                } else {
                    throw error;
                }
            }
        }
    }

    public disconnect(): void {
        this.socket.end();
    }

    public writeToSocket(data: string): void {
        this.socket.write(data);
    }

    public isConnected(): boolean {
        return this.connected && this.authenticated;
    }

    public getGameState(): GameState {
        return { ...this.gameState };
    }

    public updatePlayerLevel(level: number): void {
        this.gameState.playerLevel = level;
    }

    public updatePlayerDirection(direction: number): void {
        this.gameState.playerDirection = direction;
    }
}
