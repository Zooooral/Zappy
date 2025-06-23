import { NetworkClient } from "../network";
import { GameContext } from "./types";
import { logger } from "../logger";

interface ResourceRequirement {
  players: number;
  linemate: number;
  deraumere: number;
  sibur: number;
  mendiane: number;
  phiras: number;
  thystame: number;
}

interface ResourceCount {
  linemate: number;
  deraumere: number;
  sibur: number;
  mendiane: number;
  phiras: number;
  thystame: number;
}

type ResourceType = keyof ResourceCount;

export class ElevationManager {
  private elevationAttempts: number = 0;
  private lastReproductionAttempt: number = 0;

  private static readonly ELEVATION_REQUIREMENTS: Record<number, ResourceRequirement> = {
    1: { players: 1, linemate: 1, deraumere: 0, sibur: 0, mendiane: 0, phiras: 0, thystame: 0 },
    2: { players: 2, linemate: 1, deraumere: 1, sibur: 1, mendiane: 0, phiras: 0, thystame: 0 },
    3: { players: 2, linemate: 2, deraumere: 0, sibur: 1, mendiane: 0, phiras: 2, thystame: 0 },
    4: { players: 4, linemate: 1, deraumere: 1, sibur: 2, mendiane: 0, phiras: 1, thystame: 0 },
    5: { players: 4, linemate: 1, deraumere: 2, sibur: 1, mendiane: 3, phiras: 0, thystame: 0 },
    6: { players: 6, linemate: 1, deraumere: 2, sibur: 3, mendiane: 0, phiras: 1, thystame: 0 },
    7: { players: 6, linemate: 2, deraumere: 2, sibur: 2, mendiane: 2, phiras: 2, thystame: 1 }
  } as const;

  private static readonly REPRODUCTION_COOLDOWN = 30000;
  private static readonly RESOURCE_TYPES: readonly ResourceType[] = [
    'linemate', 'deraumere', 'sibur', 'mendiane', 'phiras', 'thystame'
  ] as const;

  public getElevationRequirements(level: number): ResourceRequirement | null {
    return ElevationManager.ELEVATION_REQUIREMENTS[level] || null;
  }

  public hasElevationResources(context: GameContext): boolean {
    const requirements = this.getRequirementsForLevel(context.gameState.playerLevel);
    if (!requirements) return false;

    return this.checkResourceRequirements(context.inventory, requirements);
  }

  public hasElevationResourcesOnGround(context: GameContext): boolean {
    const currentTile = this.getCurrentTile(context);
    if (!currentTile) return false;

    const requirements = this.getRequirementsForLevel(context.gameState.playerLevel);
    if (!requirements) return false;

    const groundResources = this.countResourcesOnGround(currentTile);
    return this.checkResourceRequirements(groundResources, requirements);
  }

  public hasEnoughPlayers(context: GameContext): boolean {
    const currentTile = this.getCurrentTile(context);
    if (!currentTile) return false;

    const requirements = this.getRequirementsForLevel(context.gameState.playerLevel);
    if (!requirements) return false;

    const playerCount = this.countPlayersOnTile(currentTile);
    return playerCount >= requirements.players;
  }

  public canElevate(context: GameContext): boolean {
    const hasResources = this.hasElevationResources(context) ||
                        this.hasElevationResourcesOnGround(context);
    return hasResources && this.hasEnoughPlayers(context);
  }

  public async attemptElevation(client: NetworkClient, context: GameContext): Promise<boolean> {
    if (!this.canElevate(context)) {
      logger.info("Cannot elevate - missing requirements");
      return false;
    }

    const level = context.gameState.playerLevel;
    logger.info(`Attempting elevation from level ${level}`);

    try {
      await this.broadcastElevationReady(client, level);
      const result = await client.incantation();

      if (result.success) {
        logger.info("Incantation started successfully!");
        this.elevationAttempts++;
        return true;
      }
      logger.warn("Incantation failed");
      return false;
    } catch (error) {
      logger.error("Error during elevation:", error);
      return false;
    }
  }

  public async requestElevationHelp(client: NetworkClient, context: GameContext): Promise<void> {
    const level = context.gameState.playerLevel;
    const requirements = this.getRequirementsForLevel(level);
    if (!requirements) return;

    const playersNeeded = this.calculatePlayersNeeded(context, requirements);
    if (playersNeeded > 0) {
      await this.broadcastPlayerRequest(client, playersNeeded, level);
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
      }
      logger.warn("Fork failed");
      return false;
    } catch (error) {
      logger.error("Error during reproduction:", error);
      return false;
    }
  }

  public shouldAttemptReproduction(): boolean {
    const timeSinceLastAttempt = Date.now() - this.lastReproductionAttempt;
    return timeSinceLastAttempt > ElevationManager.REPRODUCTION_COOLDOWN;
  }

  public getMissingResources(context: GameContext): ResourceType[] {
    const requirements = this.getRequirementsForLevel(context.gameState.playerLevel);
    if (!requirements) return [];

    return ElevationManager.RESOURCE_TYPES.filter(resource =>
      context.inventory[resource] < requirements[resource]
    );
  }

  public getElevationProgress(context: GameContext): number {
    const requirements = this.getRequirementsForLevel(context.gameState.playerLevel);
    if (!requirements) return 0;

    const totalRequired = this.calculateTotalRequiredResources(requirements);
    if (totalRequired === 0) return 1;

    const currentHave = this.calculateCurrentResources(context.inventory, requirements);
    return Math.min(currentHave / totalRequired, 1);
  }

  public getElevationAttempts(): number {
    return this.elevationAttempts;
  }

  public getLastReproductionAttempt(): number {
    return this.lastReproductionAttempt;
  }

  public reset(): void {
    this.elevationAttempts = 0;
    this.lastReproductionAttempt = 0;
  }

  private getRequirementsForLevel(level: number): ResourceRequirement | null {
    return ElevationManager.ELEVATION_REQUIREMENTS[level] || null;
  }

  private getCurrentTile(context: GameContext): string[] | null {
    return context.vision.tiles?.[0] || null;
  }

  private checkResourceRequirements(available: ResourceCount, required: ResourceRequirement): boolean {
    return ElevationManager.RESOURCE_TYPES.every(resource =>
      available[resource] >= required[resource]
    );
  }

  private countResourcesOnGround(tile: string[]): ResourceCount {
    return ElevationManager.RESOURCE_TYPES.reduce((acc, resource) => {
      acc[resource] = tile.filter(item => item === resource).length;
      return acc;
    }, {} as ResourceCount);
  }

  private countPlayersOnTile(tile: string[]): number {
    return tile.filter(item => item === "player").length;
  }

  private calculatePlayersNeeded(context: GameContext, requirements: ResourceRequirement): number {
    const currentTile = this.getCurrentTile(context);
    if (!currentTile) return requirements.players;

    const currentPlayerCount = this.countPlayersOnTile(currentTile);
    return Math.max(0, requirements.players - currentPlayerCount);
  }

  private calculateTotalRequiredResources(requirements: ResourceRequirement): number {
    return ElevationManager.RESOURCE_TYPES.reduce((total, resource) =>
      total + requirements[resource], 0
    );
  }

  private calculateCurrentResources(inventory: ResourceCount, requirements: ResourceRequirement): number {
    return ElevationManager.RESOURCE_TYPES.reduce((total, resource) =>
      total + Math.min(inventory[resource], requirements[resource]), 0
    );
  }

  private async broadcastElevationReady(client: NetworkClient, level: number): Promise<void> {
    await client.broadcast(`ELEVATION_${level}_READY`);
  }

  private async broadcastPlayerRequest(client: NetworkClient, playersNeeded: number, level: number): Promise<void> {
    const message = `NEED_${playersNeeded}_PLAYERS_LVL_${level}`;
    await client.broadcast(message);
    logger.info(`Requesting ${playersNeeded} more players for level ${level} elevation`);
  }
}
