import * as net from "net";
import { EventEmitter } from "events";
import { logger } from "../logger";
import {
  GameState,
  InventoryItem,
  BroadcastMessage,
  LookResult,
  ElevationResult,
} from "./types";

export class NetworkClient extends EventEmitter {
  private socket: net.Socket;
  private host: string;
  private port: number;
  private connected: boolean = false;
  private authenticated: boolean = false;
  private messageBuffer: string = "";
  private commandQueue: Array<{
    command: string;
    resolve: (value: any) => void;
    reject: (error: Error) => void;
  }> = [];
  private waitingForResponse: boolean = false;

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
        this.handleMessage(message.trim());
      }
    }
  }

  private handleMessage(message: string): void {
    logger.debug("Received message:", message);

    if (!this.authenticated) {
      this.handleHandshakeMessage(message);
      return;
    }

    if (message.startsWith("message ")) {
      const match = message.match(/^message (\d+),\s*(.*)$/);
      if (match) {
        const broadcast: BroadcastMessage = {
          direction: parseInt(match[1]),
          message: match[2],
        };
        this.emit("broadcast", broadcast);
        return;
      }
    }

    if (message.startsWith("eject: ")) {
      const direction = parseInt(message.split(": ")[1]);
      this.emit("ejected", direction);
      return;
    }

    if (message === "dead") {
      this.emit("dead");
      return;
    }

    if (this.waitingForResponse && this.commandQueue.length > 0) {
      const currentCommand = this.commandQueue.shift()!;
      this.waitingForResponse = false;

      try {
        const result = this.parseCommandResponse(
          currentCommand.command,
          message
        );
        currentCommand.resolve(result);
      } catch (error) {
        currentCommand.reject(error as Error);
      }

      this.processNextCommand();
    }
  }

  private handleHandshakeMessage(message: string): void {
    if (message === "WELCOME") {
      this.emit("welcome");
    } else if (message.match(/^\d+$/)) {
      this.gameState.clientsAvailable = parseInt(message);
      this.emit("clientsNumber", this.gameState.clientsAvailable);
    } else if (message.match(/^\d+ \d+$/)) {
      const [width, height] = message.split(" ").map(Number);
      this.gameState.worldWidth = width;
      this.gameState.worldHeight = height;
      this.authenticated = true;
      this.emit("authenticated", this.gameState);
      logger.info(
        `Authenticated! World size: ${width}x${height}, Available slots: ${this.gameState.clientsAvailable}`
      );
    }
  }

  private parseCommandResponse(command: string, response: string): any {
    const cmd = command.toLowerCase();

    if (response === "ko") {
      throw new Error(`Command failed: ${command}`);
    }

    if (cmd === "look") {
      return this.parseLookResponse(response);
    }

    if (cmd === "inventory") {
      return this.parseInventoryResponse(response);
    }

    if (cmd === "connect_nbr") {
      return parseInt(response);
    }

    if (cmd.startsWith("incantation")) {
      if (response.includes("Elevation underway")) {
        const levelMatch = response.match(/Current level: (\d+)/);
        if (levelMatch) {
          this.gameState.playerLevel = parseInt(levelMatch[1]);
          return { success: true, newLevel: this.gameState.playerLevel };
        }
      }
      return { success: false };
    }

    return response === "ok";
  }

  private parseLookResponse(response: string): LookResult {
    const content = response.slice(1, -1);
    const tiles = content.split(",").map((tile) =>
      tile
        .trim()
        .split(" ")
        .filter((item) => item.length > 0)
    );

    return { tiles };
  }

  private parseInventoryResponse(response: string): InventoryItem {
    const inventory: InventoryItem = {
      food: 0,
      linemate: 0,
      deraumere: 0,
      sibur: 0,
      mendiane: 0,
      phiras: 0,
      thystame: 0,
    };

    const content = response.slice(1, -1);
    const items = content.split(",").map((item) => item.trim());

    for (const item of items) {
      const parts = item.split(" ").filter((p) => p.length > 0);
      if (parts.length >= 2) {
        const itemName = parts[0] as keyof InventoryItem;
        const quantity = parseInt(parts[1]);
        if (itemName in inventory) {
          inventory[itemName] = quantity;
        }
      }
    }

    return inventory;
  }

  private processNextCommand(): void {
    if (this.commandQueue.length > 0 && !this.waitingForResponse) {
      this.waitingForResponse = true;
      const command = this.commandQueue[0].command;
      this.socket.write(command + "\n");
      logger.debug("Sent command:", command);
    }
  }

  private sendCommand(command: string): Promise<any> {
    return new Promise((resolve, reject) => {
      if (!this.connected || !this.authenticated) {
        reject(new Error("Not connected or authenticated"));
        return;
      }

      if (this.commandQueue.length >= 10) {
        reject(new Error("Command queue is full (max 10 commands)"));
        return;
      }

      this.commandQueue.push({ command, resolve, reject });

      if (!this.waitingForResponse) {
        this.processNextCommand();
      }
    });
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
      this.gameState.playerDirection = (this.gameState.playerDirection + 1) % 4;
      this.emit("turned", "right");
    }
    return result;
  }

  public async turnLeft(): Promise<boolean> {
    const result = await this.sendCommand("Left");
    if (result) {
      this.gameState.playerDirection = (this.gameState.playerDirection + 3) % 4;
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
    return this.connected && this.authenticated;
  }

  public getGameState(): GameState {
    return { ...this.gameState };
  }
}
