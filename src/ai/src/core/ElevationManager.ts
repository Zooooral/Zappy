import { NetworkClient } from "../network";
import { GameContext } from "./types";
import { logger } from "../logger";

interface ElevationRequirements {
    [key: number]: {
        players: number;
        linemate: number;
        deraumere: number;
        sibur: number;
        mendiane: number;
        phiras: number;
        thystame: number;
    };
}

export class ElevationManager {
    private elevationAttempts: number = 0;
    private lastReproductionAttempt: number = 0;

    private readonly elevationRequirements: ElevationRequirements = {
        1: { players: 1, linemate: 1, deraumere: 0, sibur: 0, mendiane: 0, phiras: 0, thystame: 0 },
        2: { players: 2, linemate: 1, deraumere: 1, sibur: 1, mendiane: 0, phiras: 0, thystame: 0 },
        3: { players: 2, linemate: 2, deraumere: 0, sibur: 1, mendiane: 0, phiras: 2, thystame: 0 },
        4: { players: 4, linemate: 1, deraumere: 1, sibur: 2, mendiane: 0, phiras: 1, thystame: 0 },
        5: { players: 4, linemate: 1, deraumere: 2, sibur: 1, mendiane: 3, phiras: 0, thystame: 0 },
        6: { players: 6, linemate: 1, deraumere: 2, sibur: 3, mendiane: 0, phiras: 1, thystame: 0 },
        7: { players: 6, linemate: 2, deraumere: 2, sibur: 2, mendiane: 2, phiras: 2, thystame: 1 }
    };

    public getElevationRequirements(level: number): any {
        return this.elevationRequirements[level] || null;
    }

    public hasElevationResources(context: GameContext): boolean {
        const level = context.gameState.playerLevel;
        const requirements = this.elevationRequirements[level];

        if (!requirements) return false;

        const inv = context.inventory;
        return (
            inv.linemate >= requirements.linemate &&
            inv.deraumere >= requirements.deraumere &&
            inv.sibur >= requirements.sibur &&
            inv.mendiane >= requirements.mendiane &&
            inv.phiras >= requirements.phiras &&
            inv.thystame >= requirements.thystame
        );
    }

    public hasElevationResourcesOnGround(context: GameContext): boolean {
        if (!context.vision.tiles || context.vision.tiles.length === 0) {
            return false;
        }

        const level = context.gameState.playerLevel;
        const requirements = this.elevationRequirements[level];

        if (!requirements) return false;

        const currentTile = context.vision.tiles[0];
        const groundResources = this.countResourcesOnGround(currentTile);

        return (
            groundResources.linemate >= requirements.linemate &&
            groundResources.deraumere >= requirements.deraumere &&
            groundResources.sibur >= requirements.sibur &&
            groundResources.mendiane >= requirements.mendiane &&
            groundResources.phiras >= requirements.phiras &&
            groundResources.thystame >= requirements.thystame
        );
    }

    public hasEnoughPlayers(context: GameContext): boolean {
        if (!context.vision.tiles || context.vision.tiles.length === 0) {
            return false;
        }

        const level = context.gameState.playerLevel;
        const requirements = this.elevationRequirements[level];

        if (!requirements) return false;

        const currentTile = context.vision.tiles[0];
        const playerCount = currentTile.filter(item => item === "player").length;

        return playerCount >= requirements.players;
    }

    public canElevate(context: GameContext): boolean {
        return (
            (this.hasElevationResources(context) ||
                this.hasElevationResourcesOnGround(context)) &&
            this.hasEnoughPlayers(context)
        );
    }

    public async attemptElevation(client: NetworkClient, context: GameContext): Promise<boolean> {
        if (!this.canElevate(context)) {
            logger.info("Cannot elevate - missing requirements");
            return false;
        }

        logger.info(`Attempting elevation from level ${context.gameState.playerLevel}`);

        try {
            await client.broadcast(`ELEVATION_${context.gameState.playerLevel}_READY`);

            const result = await client.incantation();
            if (result.success) {
                logger.info("Incantation started successfully!");
                this.elevationAttempts++;
                return true;
            } else {
                logger.warn("Incantation failed");
                return false;
            }
        } catch (error) {
            logger.error("Error during elevation:", error);
            return false;
        }
    }

    public async requestElevationHelp(client: NetworkClient, context: GameContext): Promise<void> {
        const level = context.gameState.playerLevel;
        const requirements = this.elevationRequirements[level];

        if (!requirements) return;

        const currentTile = context.vision.tiles?.[0] || [];
        const playerCount = currentTile.filter(item => item === "player").length;
        const playersNeeded = requirements.players - playerCount;

        if (playersNeeded > 0) {
            const message = `NEED_${playersNeeded}_PLAYERS_LVL_${level}`;
            await client.broadcast(message);
            logger.info(`Requesting ${playersNeeded} more players for level ${level} elevation`);
        }
    }

    public async attemptReproduction(client: NetworkClient): Promise<boolean> {
        logger.info("Attempting reproduction (fork)...");

        try {
            await client.broadcast("REPRODUCTION_READY");

            const success = await client.fork();
            if (success) {
                logger.info("Successfully forked! New team member incoming.");
                this.lastReproductionAttempt = Date.now();
                return true;
            } else {
                logger.warn("Fork failed");
                return false;
            }
        } catch (error) {
            logger.error("Error during reproduction:", error);
            return false;
        }
    }

    public shouldAttemptReproduction(): boolean {
        const timeSinceLastAttempt = Date.now() - this.lastReproductionAttempt;
        return timeSinceLastAttempt > 30000; // 30 seconds cooldown
    }

    public getMissingResources(context: GameContext): string[] {
        const level = context.gameState.playerLevel;
        const requirements = this.elevationRequirements[level];

        if (!requirements) return [];

        const inv = context.inventory;
        const missing: string[] = [];

        if (inv.linemate < requirements.linemate) missing.push("linemate");
        if (inv.deraumere < requirements.deraumere) missing.push("deraumere");
        if (inv.sibur < requirements.sibur) missing.push("sibur");
        if (inv.mendiane < requirements.mendiane) missing.push("mendiane");
        if (inv.phiras < requirements.phiras) missing.push("phiras");
        if (inv.thystame < requirements.thystame) missing.push("thystame");

        return missing;
    }

    public getElevationProgress(context: GameContext): number {
        const level = context.gameState.playerLevel;
        const requirements = this.elevationRequirements[level];

        if (!requirements) return 0;

        const inv = context.inventory;
        const totalRequired =
            requirements.linemate +
            requirements.deraumere +
            requirements.sibur +
            requirements.mendiane +
            requirements.phiras +
            requirements.thystame;

        if (totalRequired === 0) return 1;

        const currentHave =
            Math.min(inv.linemate, requirements.linemate) +
            Math.min(inv.deraumere, requirements.deraumere) +
            Math.min(inv.sibur, requirements.sibur) +
            Math.min(inv.mendiane, requirements.mendiane) +
            Math.min(inv.phiras, requirements.phiras) +
            Math.min(inv.thystame, requirements.thystame);

        return currentHave / totalRequired;
    }

    private countResourcesOnGround(tile: string[]): any {
        return {
            linemate: tile.filter(item => item === "linemate").length,
            deraumere: tile.filter(item => item === "deraumere").length,
            sibur: tile.filter(item => item === "sibur").length,
            mendiane: tile.filter(item => item === "mendiane").length,
            phiras: tile.filter(item => item === "phiras").length,
            thystame: tile.filter(item => item === "thystame").length,
        };
    }
}