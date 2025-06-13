import { NetworkClient } from "../network";
import { GameContext } from "./types";
import { logger } from "../logger";
import { VisionCalculator } from "./VisionCalculator";
import { MovementController } from "./MovementController";

export class ResourceCollector {
    private visionCalculator: VisionCalculator;
    private movementController: MovementController;

    constructor(
        visionCalculator: VisionCalculator,
        movementController: MovementController
    ) {
        this.visionCalculator = visionCalculator;
        this.movementController = movementController;
    }

    public async collectFoodOnCurrentTile(
        client: NetworkClient,
        context: GameContext
    ): Promise<boolean> {
        if (!context.vision.tiles || context.vision.tiles.length === 0) {
            return false;
        }

        const currentTile = context.vision.tiles[0];
        const hasFood = this.visionCalculator.hasResourceOnTile(currentTile, "food");

        if (hasFood) {
            logger.info("Food found on current tile! Collecting...");
            try {
                const success = await client.take("food");
                if (success) {
                    logger.info("Successfully collected food");
                    this.movementController.resetStuckCounter();
                    return true;
                }
            } catch (error) {
                logger.warn("Failed to collect food:", error);
            }
        }

        return false;
    }

    public async collectAnyResourceOnCurrentTile(
        client: NetworkClient,
        context: GameContext
    ): Promise<boolean> {
        if (!context.vision.tiles || context.vision.tiles.length === 0) {
            return false;
        }

        const currentTile = context.vision.tiles[0];
        const resources = [
            "linemate",
            "deraumere",
            "sibur",
            "mendiane",
            "phiras",
            "thystame",
        ];

        for (const resource of resources) {
            if (this.visionCalculator.hasResourceOnTile(currentTile, resource)) {
                logger.info(`${resource} found on current tile! Collecting...`);
                try {
                    const success = await client.take(resource);
                    if (success) {
                        logger.info(`Successfully collected ${resource}`);
                        this.movementController.resetStuckCounter();
                        return true;
                    }
                } catch (error) {
                    logger.warn(`Failed to collect ${resource}:`, error);
                }
            }
        }

        return false;
    }

    public async collectSpecificResource(
        client: NetworkClient,
        context: GameContext,
        resourceType: string
    ): Promise<boolean> {
        if (!context.vision.tiles || context.vision.tiles.length === 0) {
            return false;
        }

        const currentTile = context.vision.tiles[0];
        if (this.visionCalculator.hasResourceOnTile(currentTile, resourceType)) {
            logger.info(`${resourceType} found on current tile! Collecting...`);
            try {
                const success = await client.take(resourceType);
                if (success) {
                    logger.info(`Successfully collected ${resourceType}`);
                    this.movementController.resetStuckCounter();
                    return true;
                }
            } catch (error) {
                logger.warn(`Failed to collect ${resourceType}:`, error);
            }
        }

        return false;
    }

    public async moveTowardsFood(
        client: NetworkClient,
        context: GameContext
    ): Promise<boolean> {
        if (!context.vision.tiles || context.vision.tiles.length === 0) {
            return false;
        }

        const foodTileIndex = this.visionCalculator.findClosestFood(
            context.vision.tiles
        );

        if (foodTileIndex === -1) {
            return false;
        }

        const targetDirection = this.visionCalculator.getTileDirection(
            foodTileIndex,
            context.gameState.playerLevel
        );

        if (targetDirection === -1) {
            return false;
        }

        try {
            const moved = await this.movementController.moveTowardsDirection(
                client,
                targetDirection
            );
            if (moved) {
                logger.debug("Moving towards food");
                return true;
            } else {
                this.movementController.incrementStuckCounter();
                return false;
            }
        } catch (error) {
            logger.error("Error while moving towards food:", error);
            return false;
        }
    }

    public async moveTowardsResource(
        client: NetworkClient,
        context: GameContext,
        resourceType: string
    ): Promise<boolean> {
        if (!context.vision.tiles || context.vision.tiles.length === 0) {
            return false;
        }

        const resourceTileIndex = this.visionCalculator.findClosestResource(
            context.vision.tiles,
            resourceType
        );

        if (resourceTileIndex === -1) {
            return false;
        }

        const targetDirection = this.visionCalculator.getTileDirection(
            resourceTileIndex,
            context.gameState.playerLevel
        );

        if (targetDirection === -1) {
            return false;
        }

        try {
            const moved = await this.movementController.moveTowardsDirection(
                client,
                targetDirection
            );
            if (moved) {
                logger.debug(`Moving towards ${resourceType}`);
                return true;
            } else {
                this.movementController.incrementStuckCounter();
                return false;
            }
        } catch (error) {
            logger.error(`Error while moving towards ${resourceType}:`, error);
            return false;
        }
    }

    public getResourcePriority(context: GameContext): string[] {
        const level = context.gameState.playerLevel;

        switch (level) {
            case 1:
                return ["linemate"];
            case 2:
                return ["linemate", "deraumere", "sibur"];
            case 3:
                return ["linemate", "sibur", "phiras"];
            case 4:
                return ["linemate", "deraumere", "sibur", "phiras"];
            case 5:
                return ["linemate", "deraumere", "sibur", "mendiane"];
            case 6:
                return ["linemate", "deraumere", "sibur", "phiras"];
            case 7:
                return [
                    "linemate",
                    "deraumere",
                    "sibur",
                    "mendiane",
                    "phiras",
                    "thystame",
                ];
            default:
                return [
                    "linemate",
                    "deraumere",
                    "sibur",
                    "mendiane",
                    "phiras",
                    "thystame",
                ];
        }
    }

    public async searchAndCollectResources(
        client: NetworkClient,
        context: GameContext
    ): Promise<boolean> {
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

        return await this.moveTowardsFood(client, context);
    }
}