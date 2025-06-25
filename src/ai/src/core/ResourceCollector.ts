import { NetworkClient } from "../network";
import { GameContext } from "./types";
import { logger } from "../logger";
import { VisionCalculator } from "./VisionCalculator";
import { MovementController } from "./MovementController";

type ResourceType = "food" | "linemate" | "deraumere" | "sibur" | "mendiane" | "phiras" | "thystame";

interface CollectionResult {
    success: boolean;
    resourceType?: ResourceType;
    error?: string;
}

interface MovementResult {
    success: boolean;
    direction?: number;
    error?: string;
}

interface ResourcePriorities {
    readonly [level: number]: readonly ResourceType[];
}

export class ResourceCollector {
    private readonly visionCalculator: VisionCalculator;
    private readonly movementController: MovementController;

    private static readonly STONE_RESOURCES: readonly ResourceType[] = [
        "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"
    ] as const;

    private static readonly RESOURCE_PRIORITIES: ResourcePriorities = {
        1: ["linemate"],
        2: ["linemate", "deraumere", "sibur"],
        3: ["linemate", "sibur", "phiras"],
        4: ["linemate", "deraumere", "sibur", "phiras"],
        5: ["linemate", "deraumere", "sibur", "mendiane"],
        6: ["linemate", "deraumere", "sibur", "phiras"],
        7: ["linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"]
    } as const;

    private static readonly DEFAULT_PRIORITIES: readonly ResourceType[] = [
        "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"
    ] as const;

    constructor(visionCalculator: VisionCalculator, movementController: MovementController) {
        this.visionCalculator = visionCalculator;
        this.movementController = movementController;
    }

    public async collectFoodOnCurrentTile(client: NetworkClient, context: GameContext): Promise<boolean> {
        const result = await this.collectSpecificResourceOnCurrentTile(client, context, "food");
        return result.success;
    }

    public async collectAnyResourceOnCurrentTile(client: NetworkClient, context: GameContext): Promise<boolean> {
        const currentTile = this.getCurrentTile(context);
        if (!currentTile) return false;

        for (const resource of ResourceCollector.STONE_RESOURCES) {
            const result = await this.attemptResourceCollection(client, currentTile, resource);
            if (result.success) return true;
        }

        return false;
    }

    public async collectSpecificResource(client: NetworkClient, context: GameContext, resourceType: ResourceType): Promise<boolean> {
        const result = await this.collectSpecificResourceOnCurrentTile(client, context, resourceType);
        return result.success;
    }

    public async moveTowardsFood(client: NetworkClient, context: GameContext): Promise<boolean> {
        return this.moveTowardsSpecificResource(client, context, "food");
    }

    public async moveTowardsResource(client: NetworkClient, context: GameContext, resourceType: ResourceType): Promise<boolean> {
        return this.moveTowardsSpecificResource(client, context, resourceType);
    }

    public getResourcePriority(context: GameContext): readonly ResourceType[] {
        const level = context.gameState.playerLevel;
        return ResourceCollector.RESOURCE_PRIORITIES[level] || ResourceCollector.DEFAULT_PRIORITIES;
    }

    public async searchAndCollectResources(client: NetworkClient, context: GameContext): Promise<boolean> {
        if (await this.collectFoodOnCurrentTile(client, context)) {
            return true;
        }

        if (await this.collectAnyResourceOnCurrentTile(client, context)) {
            return true;
        }

        const priorities = this.getResourcePriority(context);
        for (const resource of priorities) {
            if (await this.moveTowardsResource(client, context, resource)) {
                return true;
            }
        }

        return this.moveTowardsFood(client, context);
    }

    public hasResourcesOnCurrentTile(context: GameContext): ResourceType[] {
        const currentTile = this.getCurrentTile(context);
        if (!currentTile) return [];

        return ResourceCollector.STONE_RESOURCES.filter(resource =>
            this.visionCalculator.hasResourceOnTile(currentTile, resource)
        );
    }

    public findNearestResource(context: GameContext, resourceType: ResourceType): number {
        if (!context.vision.tiles?.length) return -1;

        return resourceType === "food"
            ? this.visionCalculator.findClosestFood(context.vision.tiles)
            : this.visionCalculator.findClosestResource(context.vision.tiles, resourceType);
    }

    public canReachResource(context: GameContext, resourceType: ResourceType): boolean {
        const tileIndex = this.findNearestResource(context, resourceType);
        if (tileIndex === -1) return false;

        const direction = this.visionCalculator.getTileDirection(tileIndex, context.gameState.playerLevel);
        return direction !== -1;
    }

    private async collectSpecificResourceOnCurrentTile(client: NetworkClient, context: GameContext, resourceType: ResourceType): Promise<CollectionResult> {
        const currentTile = this.getCurrentTile(context);
        if (!currentTile) {
            return { success: false, error: "No vision data available" };
        }

        return this.attemptResourceCollection(client, currentTile, resourceType);
    }

    private async attemptResourceCollection(client: NetworkClient, tile: string[], resourceType: ResourceType): Promise<CollectionResult> {
        if (!this.visionCalculator.hasResourceOnTile(tile, resourceType)) {
            return { success: false, error: `${resourceType} not found on tile` };
        }

        logger.info(`${resourceType} found on current tile! Collecting...`);

        try {
            const success = await client.take(resourceType);
            if (success) {
                logger.info(`Successfully collected ressource ${resourceType}`);
                this.movementController.resetStuckCounter();
                return { success: true, resourceType };
            }

            return { success: false, error: `Failed to take ${resourceType}` };
        } catch (error) {
            const errorMessage = `Failed to collect ${resourceType}: ${error}`;
            logger.warn(errorMessage);
            return { success: false, error: errorMessage };
        }
    }

    private async moveTowardsSpecificResource(client: NetworkClient, context: GameContext, resourceType: ResourceType): Promise<boolean> {
        const tiles = context.vision.tiles;
        if (!tiles?.length) return false;

        const tileIndex = this.findNearestResource(context, resourceType);
        if (tileIndex === -1) return false;

        const direction = this.visionCalculator.getTileDirection(tileIndex, context.gameState.playerLevel);
        if (direction === -1) return false;

        const result = await this.attemptMovement(client, direction, resourceType);
        return result.success;
    }

    private async attemptMovement(client: NetworkClient, direction: number, resourceType: ResourceType): Promise<MovementResult> {
        try {
            const moved = await this.movementController.moveTowardsDirection(client, direction);
            if (moved) {
                logger.debug(`Moving to ressource ${resourceType}`);
                return { success: true, direction };
            }

            this.movementController.incrementStuckCounter();
            return { success: false, error: "Movement failed" };
        } catch (error) {
            const errorMessage = `Error while moving to ressource ${resourceType}: ${error}`;
            logger.error(errorMessage);
            return { success: false, error: errorMessage };
        }
    }

    private getCurrentTile(context: GameContext): string[] | null {
        return context.vision.tiles?.[0] || null;
    }
}
