import { GameLogic } from "./GameLogic";
import { logger } from "../logger";
import { NetworkClient } from "../network/client";
import { GameState, BroadcastMessage } from "../network/types";
import { AIConfig } from "./types";

export class AIPlayer {
  private client: NetworkClient;
  private gameLogic: GameLogic;
  private config: AIConfig;
  private running: boolean = false;
  private gameLoopInterval?: NodeJS.Timeout;

  constructor(config: AIConfig) {
    this.config = config;
    this.client = new NetworkClient(config.host, config.port);
    this.gameLogic = new GameLogic(this.client);
    this.setupEventHandlers();
  }

  private setupEventHandlers(): void {
    this.client.on("connected", () => {
      logger.info("Connected to server");
    });

    this.client.on("authenticated", (gameState: GameState) => {
      logger.info(
        `Authenticated! World: ${gameState.worldWidth}x${gameState.worldHeight}`
      );
      this.startGameLoop();
    });

    this.client.on("broadcast", (message: BroadcastMessage) => {
      this.gameLogic.handleBroadcast(message);
    });

    this.client.on("ejected", (direction: number) => {
      logger.warn(`Ejected from direction ${direction}`);
      this.gameLogic.handleEjection(direction);
    });

    this.client.on("dead", () => {
      logger.error("Player died!");
      this.stop();
    });

    this.client.on("error", (error: Error) => {
      logger.error("Network error:", error);
    });

    this.client.on("disconnected", () => {
      logger.warn("Disconnected from server");
      if (this.running) {
        setTimeout(() => this.reconnect(), this.config.retryDelay);
      }
    });
  }

  public async start(): Promise<void> {
    this.running = true;

    try {
      await this.client.connectWithRetry(
        this.config.teamName,
        this.config.maxRetries,
        this.config.retryDelay
      );
    } catch (error) {
      this.running = false;
      throw error;
    }
  }

  public async stop(): Promise<void> {
    this.running = false;

    if (this.gameLoopInterval) {
      clearInterval(this.gameLoopInterval);
    }

    this.client.disconnect();
    logger.info("AI stopped");
  }

  private async reconnect(): Promise<void> {
    if (!this.running) return;

    try {
      logger.info("Attempting to reconnect...");
      await this.client.connectWithRetry(
        this.config.teamName,
        this.config.maxRetries,
        this.config.retryDelay
      );
    } catch (error) {
      logger.error("Failed to reconnect:", error);
      this.stop();
    }
  }

  private startGameLoop(): void {
    logger.info("Starting game loop...");

    // TODO: main game loop execute AI logic every 100ms (à check)
    this.gameLoopInterval = setInterval(async () => {
      if (!this.running || !this.client.isConnected()) {
        return;
      }
      try {
        await this.gameLogic.tick();
      } catch (error) {
        logger.error("Error in game logic:", error);
      }
    }, 100);
  }
}
