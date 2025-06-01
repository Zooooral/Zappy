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

      await this.executeStrategy(context);
    } catch (error) {
      logger.error("Error in game tick:", error);
    }
  }

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
      // PRIORITY 1: Always check and collect food on current tile first
      if (await this.collectFoodOnCurrentTile(context)) {
        return;
      }

      // PRIORITY 2: Look for food in vision and move towards it
      if (await this.moveTowardsFood(context)) {
        return;
      }

      await this.exploreIntelligently(context);

    } finally {
      this.commandCooldown = false;
    }
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
