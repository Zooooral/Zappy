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

interface PendingCommand {
  command: string;
  resolve: (value: any) => void;
  reject: (error: Error) => void;
  timeout: NodeJS.Timeout;
  timestamp: number;
}

export class NetworkClient extends EventEmitter {
  private socket: net.Socket;
  private host: string;
  private port: number;
  private connected: boolean = false;
  private authenticated: boolean = false;
  private messageBuffer: string = "";

  private pendingCommands: PendingCommand[] = [];
  private maxPendingCommands: number = 10;
  private commandIdCounter: number = 0;
  private waitingForElevationResult: boolean = false;

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
    
    setInterval(() => this.cleanupExpiredCommands(), 1000);
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
      this.rejectAllPendingCommands("Connection closed");
      logger.info("Disconnected from server");
      this.emit("disconnected");
    });

    this.socket.on("error", (error: Error) => {
      logger.error("Socket error:", error);
      this.rejectAllPendingCommands(`Socket error: ${error.message}`);
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
    logger.debug(`[RAW RECEIVED]: ${message}`);

    if (!this.authenticated) {
      this.handleHandshakeMessage(message);
      return;
    }

    if (message.startsWith("message ")) {
      this.handleBroadcastMessage(message);
      return;
    }

    if (message.startsWith("eject: ")) {
      this.handleEjectionMessage(message);
      return;
    }

    if (message === "Elevation underway") {
      this.waitingForElevationResult = true;
      logger.info("Elevation started, waiting for result...");
      return;
    }

    if (this.waitingForElevationResult && message.startsWith("Current level:")) {
      this.waitingForElevationResult = false;
      const levelMatch = message.match(/Current level: (\d+)/);
      if (levelMatch) {
        const newLevel = parseInt(levelMatch[1]);
        this.gameState.playerLevel = newLevel;
        logger.info(`Level up! New level: ${newLevel}`);
        this.resolveNextCommand({ success: true, newLevel });
        return;
      }
    }

    if (message === "dead") {
      this.emit("dead");
      this.resolveNextCommand("dead");
      return;
    }

    this.handleCommandResponse(message);
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

  private handleBroadcastMessage(message: string): void {
    const match = message.match(/^message (\d+),\s*(.*)$/);
    if (match) {
      const broadcast: BroadcastMessage = {
        direction: parseInt(match[1]),
        message: match[2],
      };
      logger.info(`[BROADCAST] Direction ${broadcast.direction}: ${broadcast.message}`);
      this.emit("broadcast", broadcast);
    }
  }

  private handleEjectionMessage(message: string): void {
    const match = message.match(/^eject: (\d+)$/);
    if (match) {
      const direction = parseInt(match[1]);
      logger.warn(`[EJECTED] From direction: ${direction}`);
      this.emit("ejected", direction);
    }
  }

  private handleCommandResponse(message: string): void {
    if (this.pendingCommands.length === 0) {
      logger.warn(`Received response but no pending commands: ${message}`);
      return;
    }

    try {
      const result = this.parseCommandResponse(message);
      this.resolveNextCommand(result);
    } catch (error) {
      this.rejectNextCommand(error as Error);
    }
  }

  private parseCommandResponse(response: string): any {
    if (response === "ko") {
      throw new Error("Command failed (ko)");
    }

    if (response === "ok") {
      return true;
    }

    if (response.startsWith("[") && response.includes(",")) {
      if (response.includes("player") || response.includes("food") || response.includes("linemate")) {
        return this.parseLookResponse(response);
      } else {
        return this.parseInventoryResponse(response);
      }
    }

    if (/^\d+$/.test(response)) {
      return parseInt(response);
    }

    logger.warn(`Unknown response format: ${response}`);
    return response;
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

    const content = response.replace(/^\[|\]$/g, '').trim();
    if (!content) return inventory;

    const items = content.split(',');
    for (const item of items) {
      const cleaned = item.trim();
      if (!cleaned) continue;
      const lastSpaceIndex = cleaned.lastIndexOf(' ');
      if (lastSpaceIndex === -1) continue;
      const itemName = cleaned.substring(0, lastSpaceIndex).trim();
      const quantityStr = cleaned.substring(lastSpaceIndex + 1).trim();
      const quantity = parseInt(quantityStr);
      if (isNaN(quantity)) continue;
      if (itemName in inventory) {
        (inventory as any)[itemName] = quantity;
      }
    }

    return inventory;
  }

  private resolveNextCommand(result: any): void {
    if (this.pendingCommands.length > 0) {
      const command = this.pendingCommands.shift()!;
      clearTimeout(command.timeout);
      const duration = Date.now() - command.timestamp;
      logger.info(`[COMMAND COMPLETED]: ${command.command} -> resolved in ${duration}ms`);
      command.resolve(result);
    }
  }

  private rejectNextCommand(error: Error): void {
    if (this.pendingCommands.length > 0) {
      const command = this.pendingCommands.shift()!;
      clearTimeout(command.timeout);
      logger.error(`[COMMAND FAILED]: ${command.command} -> ${error.message}`);
      command.reject(error);
    }
  }

  private rejectAllPendingCommands(reason: string): void {
    while (this.pendingCommands.length > 0) {
      const command = this.pendingCommands.shift()!;
      clearTimeout(command.timeout);
      command.reject(new Error(reason));
    }
  }

  private cleanupExpiredCommands(): void {
    const now = Date.now();
    const expired = this.pendingCommands.filter(cmd => now - cmd.timestamp > 30000);
    for (const cmd of expired) {
      const index = this.pendingCommands.indexOf(cmd);
      if (index !== -1) {
        this.pendingCommands.splice(index, 1);
        clearTimeout(cmd.timeout);
        cmd.reject(new Error("Command expired"));
        logger.warn(`[EXPIRED]: ${cmd.command}`);
      }
    }
  }

  private async sendCommand(command: string, timeoutMs: number = 15000): Promise<any> {
    return new Promise((resolve, reject) => {
      if (!this.connected || !this.authenticated) {
        reject(new Error("Not connected or authenticated"));
        return;
      }

      const trySend = () => {
        if (this.pendingCommands.length >= this.maxPendingCommands) {
          setTimeout(trySend, 10);
          return;
        }

        const timeout = setTimeout(() => {
          const index = this.pendingCommands.findIndex(cmd => cmd.command === command);
          if (index !== -1) {
            this.pendingCommands.splice(index, 1);
            reject(new Error(`Command timeout: ${command}`));
          }
        }, timeoutMs);

        const pendingCommand: PendingCommand = {
          command,
          resolve,
          reject,
          timeout,
          timestamp: Date.now()
        };

        this.pendingCommands.push(pendingCommand);

        const dataToSend = command + "\n";
        this.socket.write(dataToSend);
        logger.info(`[SENT]: ${command} (pending: ${this.pendingCommands.length}/${this.maxPendingCommands})`);
      };

      trySend();
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

  public getNetworkStats(): any {
    return {
      connected: this.connected,
      authenticated: this.authenticated,
      pendingCommands: this.pendingCommands.length,
      maxPendingCommands: this.maxPendingCommands,
      oldestPendingCommand: this.pendingCommands.length > 0 ? Date.now() - this.pendingCommands[0].timestamp : 0
    };
  }
}