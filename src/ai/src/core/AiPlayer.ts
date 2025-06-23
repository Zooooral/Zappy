import { GameLogic } from "./GameLogic";
import { logger } from "../logger";
import { NetworkClient } from "../network";
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
            logger.info(`Authenticated! World: ${gameState.worldWidth}x${gameState.worldHeight}, Available slots: ${gameState.clientsAvailable}`);
            logger.info(`Starting AI with team: ${this.config.teamName}`);
            this.startGameLoop();
        });

        this.client.on("broadcast", (message: BroadcastMessage) => {
            logger.info(`Broadcast received from direction ${message.direction}: ${message.message}`);
            this.gameLogic.handleBroadcast(message);
        });

        this.client.on("ejected", (direction: number) => {
            logger.warn(`Ejected from direction ${direction}`);
            this.gameLogic.handleEjection(direction);
        });

        this.client.on("moved", () => {
            logger.debug("Player moved");
        });

        this.client.on("turned", (direction: string) => {
            logger.debug(`Player turned ${direction}`);
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
                logger.info(`Attempting reconnection in ${this.config.retryDelay}ms...`);
                setTimeout(() => this.reconnect(), this.config.retryDelay);
            }
        });
    }

    public async start(): Promise<void> {
        this.running = true;
        logger.info(`Starting AI for team "${this.config.teamName}" on ${this.config.host}:${this.config.port}`);

        try {
            await this.client.connectWithRetry(
                this.config.teamName,
                this.config.maxRetries,
                this.config.retryDelay
            );
        } catch (error) {
            this.running = false;
            logger.error("Failed to start AI:", error);
            throw error;
        }
    }

    public async stop(): Promise<void> {
        this.running = false;
        logger.info("Stopping AI...");

        if (this.gameLoopInterval) {
            clearInterval(this.gameLoopInterval);
            this.gameLoopInterval = undefined;
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

        const gameState = this.client.getGameState();
        logger.info(`Initial state - Level: ${gameState.playerLevel}, Direction: ${gameState.playerDirection}`);

        this.gameLoopInterval = setInterval(async () => {
            if (!this.running || !this.client.isConnected()) {
                logger.warn("Game loop stopped - not running or not connected");
                return;
            }

            try {
                const stats = this.client.getNetworkStats();
                if (stats.pendingCommands > 8) {
                    logger.debug(`Queue nearly full: ${stats.pendingCommands}/10, skipping tick`);
                    return;
                }

                await this.gameLogic.tick();
            } catch (error) {
                logger.error("Error in game logic:", error);

                if (error instanceof Error && error.message.includes("Not connected")) {
                    logger.warn("Connection lost, attempting reconnect...");
                    this.reconnect();
                }
            }
        }, 100);
    }
}
