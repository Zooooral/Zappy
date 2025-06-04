import { NetworkClient } from "../network/client";
import { BroadcastMessage, InventoryItem, LookResult } from "../network/types";
import { logger } from "../logger";
import { AIState, GameContext } from "./types";

interface Position {
  x: number;
  y: number;
}

interface MovementHistory {
  moves: string[];
  positions: Position[];
  maxHistory: number;
}

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

export class GameLogic {
  private client: NetworkClient;
  private currentState: AIState = AIState.SURVIVAL;
  private timeInState: number = 0;
  private lastTick: number = Date.now();

  private lastInventory?: InventoryItem;
  private lastVision?: LookResult;
  private commandCooldown: boolean = false;

  private movementHistory: MovementHistory = {
    moves: [],
    positions: [],
    maxHistory: 10
  };
  private stuckCounter: number = 0;
  private lastDirection: number = 0; // 0=North, 1=East, 2=South, 3=West

  private estimatedPosition: Position = { x: 0, y: 0 };

  constructor(client: NetworkClient) {
    this.client = client;
  }

  public async tick(): Promise<void> {
    if (this.commandCooldown) {
      return;
    }

    const now = Date.now();
    this.timeInState += now - this.lastTick;
    this.lastTick = now;

    try {
      await this.updateContext();
      const context = this.getContext();

      if (context.inventory.food < 50) {
        this.currentState = AIState.SURVIVAL;
      }

      await this.updateAIState(context);
      await this.executeStrategy(context);
    } catch (error) {
      logger.error("Error in game tick:", error);
    }
  }

  private elevationAttempts: number = 0;
  private lastReproductionAttempt: number = 0;
  private elevationRequirements: ElevationRequirements = {
    1: { players: 1, linemate: 1, deraumere: 0, sibur: 0, mendiane: 0, phiras: 0, thystame: 0 },
    2: { players: 2, linemate: 1, deraumere: 1, sibur: 1, mendiane: 0, phiras: 0, thystame: 0 },
    3: { players: 2, linemate: 2, deraumere: 0, sibur: 1, mendiane: 0, phiras: 2, thystame: 0 },
    4: { players: 4, linemate: 1, deraumere: 1, sibur: 2, mendiane: 0, phiras: 1, thystame: 0 },
    5: { players: 4, linemate: 1, deraumere: 2, sibur: 1, mendiane: 3, phiras: 0, thystame: 0 },
    6: { players: 6, linemate: 1, deraumere: 2, sibur: 3, mendiane: 0, phiras: 1, thystame: 0 },
    7: { players: 6, linemate: 2, deraumere: 2, sibur: 2, mendiane: 2, phiras: 2, thystame: 1 }
  };

  private async updateContext(): Promise<void> {
    try {
      const needInventoryUpdate = !this.lastInventory || (this.lastInventory.food < 50 && this.timeInState % 500 < 100) || this.timeInState % 2000 < 100;

      if (needInventoryUpdate) {
        this.lastInventory = await this.client.getInventory();
      }

      if (!this.lastVision || this.timeInState % 1000 < 100) {
        this.lastVision = await this.client.look();
      }
    } catch (error) {
      logger.warn("Failed to update context:", error);
    }
  }

  private async updateAIState(context: GameContext): Promise<void> {
    const food = context.inventory.food;
    const level = context.gameState.playerLevel;

    if (food < 5) {
      this.currentState = AIState.SURVIVAL;
      logger.warn(`Critical survival mode - food: ${food}`);
    // TODO: Intégrer logique de reproduction et élévation
    } else {
      // Exploration par défaut
      this.currentState = AIState.EXPLORATION;
    }
  }

  private getContext(): GameContext {
    return {
      inventory: this.lastInventory || {
        food: 0,
        linemate: 0,
        deraumere: 0,
        sibur: 0,
        mendiane: 0,
        phiras: 0,
        thystame: 0,
      },
      vision: this.lastVision || { tiles: [] },
      gameState: this.client.getGameState(),
      currentState: this.currentState,
      timeInState: this.timeInState,
    };
  }

  private async executeStrategy(context: GameContext): Promise<void> {
    this.commandCooldown = true;

    try {
      switch (context.currentState) {
        case AIState.SURVIVAL:
          await this.executeSurvivalStrategy(context);
          break;
        case AIState.COORDINATION:
          await this.executeReproductionStrategy(context);
          break;
        case AIState.ELEVATION:
          await this.executeElevationStrategy(context);
          break;
        case AIState.EXPLORATION:
        default:
          await this.executeExplorationStrategy(context);
          break;
      }
    } finally {
      this.commandCooldown = false;
    }
  }

  private async executeSurvivalStrategy(context: GameContext): Promise<void> {
    // PRIORITÉ 1: Nourriture sur la case actuelle
    if (await this.collectFoodOnCurrentTile(context)) {
      return;
    }

    // PRIORITÉ 2: Chercher de la nourriture dans la vision
    if (await this.moveTowardsFood(context)) {
      return;
    }
  }

  private async executeReproductionStrategy(context: GameContext): Promise<void> {
    logger.info("Attempting reproduction (fork)...");

    try {

      await this.client.broadcast("REPRODUCTION_READY");

      const success = await this.client.fork();
      if (success) {
        logger.info("Successfully forked! New team member incoming.");
        this.lastReproductionAttempt = Date.now();
        this.currentState = AIState.SURVIVAL;
      } else {
        logger.warn("Fork failed, continuing with current strategy");
        this.currentState = AIState.GATHERING;
      }
    } catch (error) {
      logger.error("Error during reproduction:", error);
      this.currentState = AIState.GATHERING;
    }
  }

  private async executeElevationStrategy(context: GameContext): Promise<void> {
    logger.info(`Attempting elevation from level ${context.gameState.playerLevel}`);
    try {
      await this.client.broadcast(`ELEVATION_${context.gameState.playerLevel}_READY`);
      if (this.hasElevationResources(context)) {
        const success = await this.client.incantation();
        if (success) {
          logger.info("Incantation started successfully!");
          this.elevationAttempts++;
        } else {
          logger.warn("Incantation failed");
          this.currentState = AIState.GATHERING;
        }
      } else {
        logger.info("Missing resources for elevation, switching to gathering");
        this.currentState = AIState.GATHERING;
      }
    } catch (error) {
      logger.error("Error during elevation:", error);
      this.currentState = AIState.GATHERING;
    }
  }

  private async executeExplorationStrategy(context: GameContext): Promise<void> {
    if (await this.collectFoodOnCurrentTile(context)) {
      return;
    }
    if (await this.collectAnyResourceOnCurrentTile(context)) {
      return;
    }
    await this.exploreIntelligently(context);
  }

  private async collectFoodOnCurrentTile(context: GameContext): Promise<boolean> {
    if (!context.vision.tiles || context.vision.tiles.length === 0) {
      return false;
    }

    const currentTile = context.vision.tiles[0];
    const hasFood = currentTile.includes("food");

    if (hasFood) {
      logger.info("Food found on current tile! Collecting...");
      try {
        const success = await this.client.take("food");
        if (success) {
          logger.info("Successfully collected food");
          this.stuckCounter = 0;
          return true;
        }
      } catch (error) {
        logger.warn("Failed to collect food:", error);
      }
    }

    return false;
  }

  private async moveTowardsFood(context: GameContext): Promise<boolean> {
    if (!context.vision.tiles || context.vision.tiles.length === 0) {
      return false;
    }

    const foodTileIndex = this.findClosestFood(context.vision.tiles);

    if (foodTileIndex === -1) {
      return false;
    }

    const targetDirection = this.getTileDirection(foodTileIndex, context.gameState.playerLevel);

    if (targetDirection === -1) {
      return false;
    }

    const currentDirection = this.lastDirection;
    const turnDirection = this.calculateTurnDirection(currentDirection, targetDirection);

    if (turnDirection === "right") {
      logger.debug("Turning right towards food");
      await this.client.turnRight();
      this.lastDirection = (this.lastDirection + 1) % 4;
      this.addToMovementHistory("turnRight");
      return true;
    } else if (turnDirection === "left") {
      logger.debug("Turning left towards food");
      await this.client.turnLeft();
      this.lastDirection = (this.lastDirection + 3) % 4;
      this.addToMovementHistory("turnLeft");
      return true;
    } else {
      logger.debug("Moving forward towards food");
      const moved = await this.client.moveForward();
      if (moved) {
        this.updateEstimatedPosition();
        this.addToMovementHistory("moveForward");
        this.stuckCounter = 0;
      } else {
        this.stuckCounter++;
      }
      return true;
    }
  }

  private async collectAnyResourceOnCurrentTile(context: GameContext): Promise<boolean> {
    if (!context.vision.tiles || context.vision.tiles.length === 0) {
      return false;
    }

    const currentTile = context.vision.tiles[0];
    const resources = ['linemate', 'deraumere', 'sibur', 'mendiane', 'phiras', 'thystame'];
    for (const resource of resources) {
      if (currentTile.includes(resource)) {
        logger.info(`${resource} found on current tile! Collecting...`);
        try {
          const success = await this.client.take(resource);
          if (success) {
            logger.info(`Successfully collected ${resource}`);
            this.stuckCounter = 0;
            return true;
          }
        } catch (error) {
          logger.warn(`Failed to collect ${resource}:`, error);
        }
      }
    }

    return false;
  }

  private hasElevationResources(context: GameContext): boolean {
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

  private findClosestFood(tiles: string[][]): number {
    for (let i = 1; i < tiles.length; i++) {
      if (tiles[i].includes("food")) {
        logger.debug(`Food found at tile index ${i}`);
        return i;
      }
    }
    return -1;
  }

  private getTileDirection(tileIndex: number, playerLevel: number): number {
    const visionPattern = this.generateVisionPattern(playerLevel);

    if (tileIndex >= visionPattern.length) {
      return -1;
    }

    const relativePos = visionPattern[tileIndex];
    if (!relativePos) {
      return -1;
    }

    if (relativePos.x === 0 && relativePos.y > 0) return 0; // North
    if (relativePos.x > 0 && relativePos.y === 0) return 1; // East
    if (relativePos.x === 0 && relativePos.y < 0) return 2; // South
    if (relativePos.x < 0 && relativePos.y === 0) return 3; // West

    if (relativePos.x > 0 && relativePos.y > 0) return 1; // Northeast -> East
    if (relativePos.x > 0 && relativePos.y < 0) return 1; // Southeast -> East
    if (relativePos.x < 0 && relativePos.y > 0) return 3; // Northwest -> West
    if (relativePos.x < 0 && relativePos.y < 0) return 3; // Southwest -> West

    return -1;
  }

  private generateVisionPattern(level: number): Array<{x: number, y: number}> {
    const pattern: Array<{x: number, y: number}> = [];

    pattern.push({x: 0, y: 0});

    for (let distance = 1; distance <= level; distance++) {
      for (let x = -distance; x <= distance; x++) {
        pattern.push({x, y: distance});
      }
      for (let y = distance - 1; y >= -distance + 1; y--) {
        pattern.push({x: -distance, y});
        pattern.push({x: distance, y});
      }
      if (distance > 1) {
        for (let x = -distance + 1; x < distance; x++) {
          pattern.push({x, y: -distance});
        }
      }
    }

    return pattern;
  }

  private calculateTurnDirection(currentDir: number, targetDir: number): "left" | "right" | "none" {
    if (currentDir === targetDir) {
      return "none";
    }

    const diff = (targetDir - currentDir + 4) % 4;
    return diff <= 2 ? "right" : "left";
  }

  private async exploreIntelligently(context: GameContext): Promise<boolean> {
    if (this.isStuckInLoop() || this.stuckCounter > 3) {
      logger.debug("Detected loop or stuck, trying random direction");
      await this.breakOutOfLoop();
      return true;
    }

    if (Math.random() < 0.7) {
      logger.debug("Exploring: moving forward");
      const moved = await this.client.moveForward();
      if (moved) {
        this.updateEstimatedPosition();
        this.addToMovementHistory("moveForward");
        this.stuckCounter = 0;
      } else {
        this.stuckCounter++;
        if (Math.random() < 0.5) {
          await this.client.turnRight();
          this.lastDirection = (this.lastDirection + 1) % 4;
          this.addToMovementHistory("turnRight");
        } else {
          await this.client.turnLeft();
          this.lastDirection = (this.lastDirection + 3) % 4;
          this.addToMovementHistory("turnLeft");
        }
      }
    } else {
      if (Math.random() < 0.5) {
        logger.debug("Exploring: turning right");
        await this.client.turnRight();
        this.lastDirection = (this.lastDirection + 1) % 4;
        this.addToMovementHistory("turnRight");
      } else {
        logger.debug("Exploring: turning left");
        await this.client.turnLeft();
        this.lastDirection = (this.lastDirection + 3) % 4;
        this.addToMovementHistory("turnLeft");
      }
    }

    return true;
  }

  private isStuckInLoop(): boolean {
    if (this.movementHistory.moves.length < 6) {
      return false;
    }

    const recentMoves = this.movementHistory.moves.slice(-6);
    const pattern = recentMoves.slice(0, 3).join(",");
    const nextPattern = recentMoves.slice(3, 6).join(",");

    return pattern === nextPattern;
  }

  private async breakOutOfLoop(): Promise<void> {
    logger.info("Breaking out of loop with random actions");

    this.movementHistory.moves = [];
    this.stuckCounter = 0;

    const actions = ["moveForward", "turnRight", "turnLeft"];
    const randomAction = actions[Math.floor(Math.random() * actions.length)];

    switch (randomAction) {
      case "moveForward":
        const moved = await this.client.moveForward();
        if (moved) {
          this.updateEstimatedPosition();
        }
        break;
      case "turnRight":
        await this.client.turnRight();
        this.lastDirection = (this.lastDirection + 1) % 4;
        break;
      case "turnLeft":
        await this.client.turnLeft();
        this.lastDirection = (this.lastDirection + 3) % 4;
        break;
    }

    this.addToMovementHistory(randomAction);
  }

  private addToMovementHistory(move: string): void {
    this.movementHistory.moves.push(move);
    if (this.movementHistory.moves.length > this.movementHistory.maxHistory) {
      this.movementHistory.moves.shift();
    }

    this.movementHistory.positions.push({ ...this.estimatedPosition });
    if (this.movementHistory.positions.length > this.movementHistory.maxHistory) {
      this.movementHistory.positions.shift();
    }
  }

  private updateEstimatedPosition(): void {
    switch (this.lastDirection) {
      case 0: // North
        this.estimatedPosition.y--;
        break;
      case 1: // East
        this.estimatedPosition.x++;
        break;
      case 2: // South
        this.estimatedPosition.y++;
        break;
      case 3: // West
        this.estimatedPosition.x--;
        break;
    }
  }

  public handleBroadcast(message: BroadcastMessage): void {
    // TODO: recuperation de message broadcast
    logger.debug(
      `Received broadcast from direction ${message.direction}: ${message.message}`
    );
  }

  public handleEjection(direction: number): void {
    logger.warn(`Handling ejection from direction ${direction}`);
    // Reset movement history and force exploration
    this.movementHistory.moves = [];
    this.stuckCounter = 0;
    this.currentState = AIState.EXPLORATION;
    this.timeInState = 0;
  }
}
