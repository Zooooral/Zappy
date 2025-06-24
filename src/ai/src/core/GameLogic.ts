import { NetworkClient } from "../network";
import { BroadcastMessage, InventoryItem, LookResult } from "../network/types";
import { logger } from "../logger";
import { AIState, GameContext } from "./types";
import { AIStateManager } from "./AIStateManager";
import { VisionCalculator } from "./VisionCalculator";
import { MovementController } from "./MovementController";
import { ResourceCollector } from "./ResourceCollector";
import { ElevationManager } from "./ElevationManager";

export class GameLogic {
    private client: NetworkClient;
    private stateManager: AIStateManager;
    private visionCalculator: VisionCalculator;
    private movementController: MovementController;
    private resourceCollector: ResourceCollector;
    private elevationManager: ElevationManager;

    private lastInventory?: InventoryItem;
    private lastVision?: LookResult;
    private lastInventoryUpdate: number = 0;
    private lastVisionUpdate: number = 0;

    private readonly INVENTORY_UPDATE_INTERVAL = 2000;
    private readonly VISION_UPDATE_INTERVAL = 1000;

    constructor(client: NetworkClient) {
        this.client = client;
        this.stateManager = new AIStateManager();
        this.visionCalculator = new VisionCalculator();
        this.movementController = new MovementController();
        this.elevationManager = new ElevationManager();
        this.resourceCollector = new ResourceCollector(
            this.visionCalculator,
            this.movementController
        );
    }

    public async tick(): Promise<void> {
        try {
            await this.updateContextSequentially();

            const context = this.getContext();
            if (!context) return;

            this.stateManager.updateTime();
            this.stateManager.updateState(context);

            await this.executeStrategySequentially(context);
        } catch (error) {
            logger.error("Error in game tick:", error);
        }
    }

    private async updateContextSequentially(): Promise<void> {
        const now = Date.now();

        try {
            if (!this.lastInventory || (now - this.lastInventoryUpdate) > this.INVENTORY_UPDATE_INTERVAL || (this.lastInventory.food < 50 && (now - this.lastInventoryUpdate) > 500)) {
                this.lastInventory = await this.client.getInventory();
                this.lastInventoryUpdate = now;
            }

            if (!this.lastVision || (now - this.lastVisionUpdate) > this.VISION_UPDATE_INTERVAL) {
                this.lastVision = await this.client.look();
                this.lastVisionUpdate = now;
            }
        } catch (error) {
            logger.warn("Failed to update context:", error);
        }
    }

    private getContext(): GameContext | null {
        if (!this.lastInventory || !this.lastVision) {
            return null;
        }

        return {
            inventory: this.lastInventory,
            vision: this.lastVision,
            gameState: this.client.getGameState(),
            currentState: this.stateManager.getCurrentState(),
            timeInState: this.stateManager.getTimeInState(),
        };
    }

    private async executeStrategySequentially(context: GameContext): Promise<void> {
        try {
            switch (context.currentState) {
                case AIState.SURVIVAL:
                    await this.executeSurvivalStrategy(context);
                    break;
                case AIState.COORDINATION:
                    await this.executeCoordinationStrategy(context);
                    break;
                case AIState.ELEVATION:
                    await this.executeElevationStrategy(context);
                    break;
                case AIState.GATHERING:
                    await this.executeGatheringStrategy(context);
                    break;
                case AIState.EXPLORATION:
                default:
                    await this.executeExplorationStrategy(context);
                    break;
            }
        } catch (error) {
            logger.error("Error in strategy execution:", error);
        }
    }

    private async executeSurvivalStrategy(context: GameContext): Promise<void> {
        if (await this.resourceCollector.collectFoodOnCurrentTile(this.client, context)) {
            return;
        }

        if (await this.resourceCollector.moveTowardsFood(this.client, context)) {
            return;
        }

        await this.movementController.exploreRandomly(this.client);
    }

    private async executeCoordinationStrategy(context: GameContext): Promise<void> {
        if (this.elevationManager.shouldAttemptReproduction()) {
            const success = await this.elevationManager.attemptReproduction(this.client);
            if (success) {
                this.stateManager.setState(AIState.SURVIVAL);
                return;
            }
        }

        await this.elevationManager.requestElevationHelp(this.client, context);
        this.stateManager.setState(AIState.GATHERING);
    }

    private async executeElevationStrategy(context: GameContext): Promise<void> {
        if (this.elevationManager.canElevate(context)) {
            const success = await this.elevationManager.attemptElevation(
                this.client,
                context
            );
            if (success) {
                logger.info("Elevation initiated successfully!");
                return;
            }
        }

        const missingResources = this.elevationManager.getMissingResources(context);
        if (missingResources.length > 0) {
            logger.info(`Missing resources for elevation: ${missingResources.join(", ")}`);
            this.stateManager.setState(AIState.GATHERING);
        } else if (!this.elevationManager.hasEnoughPlayers(context)) {
            logger.info("Not enough players for elevation");
            this.stateManager.setState(AIState.COORDINATION);
        }
    }

    private async executeGatheringStrategy(context: GameContext): Promise<void> {
        if (context.inventory.food < 20) {
            this.stateManager.setState(AIState.SURVIVAL);
            return;
        }

        if (await this.resourceCollector.searchAndCollectResources(this.client, context)) {
            return;
        }

        if (this.elevationManager.getElevationProgress(context) > 0.8) {
            this.stateManager.setState(AIState.ELEVATION);
            return;
        }

        await this.movementController.exploreRandomly(this.client);
    }

    private async executeExplorationStrategy(context: GameContext): Promise<void> {
        if (await this.resourceCollector.collectFoodOnCurrentTile(this.client, context)) {
            return;
        }

        if (await this.resourceCollector.collectAnyResourceOnCurrentTile(this.client, context)) {
            return;
        }

        if (this.elevationManager.canElevate(context)) {
            this.stateManager.setState(AIState.ELEVATION);
            return;
        }

        const missingResources = this.elevationManager.getMissingResources(context);
        if (missingResources.length > 0 && context.inventory.food > 30) {
            this.stateManager.setState(AIState.GATHERING);
            return;
        }

        await this.movementController.exploreRandomly(this.client);
    }

    public handleBroadcast(message: BroadcastMessage): void {
        logger.debug(`Received broadcast from direction ${message.direction}: ${message.message}`);

        if (message.message.includes("ELEVATION") && message.message.includes("READY")) {
            logger.info("Other player ready for elevation, checking if we can help");
            this.stateManager.setState(AIState.COORDINATION);
        }

        if (message.message.includes("NEED") && message.message.includes("PLAYERS")) {
            logger.info("Other player needs help with elevation");
            this.stateManager.setState(AIState.COORDINATION);
        }
    }

    public handleEjection(direction: number): void {
        logger.warn(`Handling ejection from direction ${direction}`);
        this.movementController.handleEjection();
        this.stateManager.setState(AIState.EXPLORATION);
    }

    public getDebugInfo(): any {
        const context = this.getContext();
        if (!context) return { error: "No context available" };

        return {
            currentState: this.stateManager.getCurrentState(),
            timeInState: this.stateManager.getTimeInState(),
            stuckCounter: this.movementController.getStuckCounter(),
            lastDirection: this.movementController.getLastDirection(),
            canElevate: this.elevationManager.canElevate(context),
            elevationProgress: this.elevationManager.getElevationProgress(context),
            missingResources: this.elevationManager.getMissingResources(context),
            inventory: context.inventory,
            networkStats: this.client.getNetworkStats(),
        };
    }
}
