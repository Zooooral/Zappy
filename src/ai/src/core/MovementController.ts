import { NetworkClient } from "../network";
import { logger } from "../logger";

enum Direction {
  NORTH = 0,
  EAST = 1,
  SOUTH = 2,
  WEST = 3
}

enum TurnDirection {
  LEFT = "left",
  RIGHT = "right",
  NONE = "none"
}

enum MovementAction {
  MOVE_FORWARD = "moveForward",
  TURN_RIGHT = "turnRight",
  TURN_LEFT = "turnLeft"
}

interface Position {
  x: number;
  y: number;
}

interface MovementHistory {
  readonly moves: string[];
  readonly positions: Position[];
  readonly maxHistory: number;
}

interface MovementStats {
  readonly successfulMoves: number;
  readonly failedMoves: number;
  readonly totalTurns: number;
}

export class MovementController {
  private readonly movementHistory: MovementHistory;
  private stuckCounter: number = 0;
  private currentDirection: Direction = Direction.NORTH;
  private estimatedPosition: Position = { x: 0, y: 0 };
  private stats: MovementStats = { successfulMoves: 0, failedMoves: 0, totalTurns: 0 };

  private static readonly MAX_HISTORY = 10;
  private static readonly STUCK_THRESHOLD = 3;
  private static readonly LOOP_DETECTION_SIZE = 6;
  private static readonly FORWARD_PROBABILITY = 0.7;

  private static readonly DIRECTION_VECTORS: Record<Direction, Position> = {
    [Direction.NORTH]: { x: 0, y: -1 },
    [Direction.EAST]: { x: 1, y: 0 },
    [Direction.SOUTH]: { x: 0, y: 1 },
    [Direction.WEST]: { x: -1, y: 0 }
  };

  constructor() {
    this.movementHistory = {
      moves: [],
      positions: [],
      maxHistory: MovementController.MAX_HISTORY
    };
  }

  public getCurrentDirection(): Direction {
    return this.currentDirection;
  }

  public getLastDirection(): number {
    return this.currentDirection;
  }

  public getStuckCounter(): number {
    return this.stuckCounter;
  }

  public getEstimatedPosition(): Readonly<Position> {
    return { ...this.estimatedPosition };
  }

  public getMovementStats(): Readonly<MovementStats> {
    return { ...this.stats };
  }

  public getMovementHistory(): Readonly<MovementHistory> {
    return {
      moves: [...this.movementHistory.moves],
      positions: [...this.movementHistory.positions],
      maxHistory: this.movementHistory.maxHistory
    };
  }

  public resetStuckCounter(): void {
    this.stuckCounter = 0;
  }

  public incrementStuckCounter(): void {
    this.stuckCounter++;
  }

  public async moveForward(client: NetworkClient): Promise<boolean> {
    try {
      const success = await client.moveForward();

      if (success) {
        this.handleSuccessfulMove();
        return true;
      }
      this.handleFailedMove();
      return false;
    } catch (error) {
      logger.error("Error moving forward:", error);
      this.handleFailedMove();
      return false;
    }
  }

  public async turnRight(client: NetworkClient): Promise<boolean> {
    return this.performTurn(client, MovementAction.TURN_RIGHT, 1);
  }

  public async turnLeft(client: NetworkClient): Promise<boolean> {
    return this.performTurn(client, MovementAction.TURN_LEFT, 3);
  }

  public async turnToDirection(client: NetworkClient, targetDirection: Direction): Promise<boolean> {
    const turnDirection = this.calculateOptimalTurn(this.currentDirection, targetDirection);

    switch (turnDirection) {
      case TurnDirection.RIGHT:
        logger.debug(`Turning right to face ${Direction[targetDirection]}`);
        return this.turnRight(client);
      case TurnDirection.LEFT:
        logger.debug(`Turning left to face ${Direction[targetDirection]}`);
        return this.turnLeft(client);
      case TurnDirection.NONE:
        return true;
    }
  }

  public async moveTowardsDirection(client: NetworkClient, targetDirection: Direction): Promise<boolean> {
    const turnSuccess = await this.turnToDirection(client, targetDirection);
    if (!turnSuccess) return false;

    return this.moveForward(client);
  }

  public isStuckInLoop(): boolean {
    const recentMoves = this.getRecentMoves(MovementController.LOOP_DETECTION_SIZE);
    if (recentMoves.length < MovementController.LOOP_DETECTION_SIZE) {
      return false;
    }

    return this.detectRepeatingPattern(recentMoves);
  }

  public isStuck(): boolean {
    return this.stuckCounter > MovementController.STUCK_THRESHOLD || this.isStuckInLoop();
  }

  public async breakOutOfLoop(client: NetworkClient): Promise<void> {
    logger.info("Breaking out of movement loop with random actions");
    this.clearMovementHistory();
    this.resetStuckCounter();

    const randomAction = this.selectRandomAction();
    await this.executeAction(client, randomAction);
  }

  public async exploreRandomly(client: NetworkClient): Promise<boolean> {
    if (this.isStuck()) {
      await this.breakOutOfLoop(client);
      return true;
    }

    const action = Math.random() < MovementController.FORWARD_PROBABILITY
      ? MovementAction.MOVE_FORWARD
      : this.selectRandomTurn();

    logger.debug(`Random exploration: ${action}`);
    await this.executeAction(client, action);
    return true;
  }

  public handleEjection(): void {
    logger.warn("Handling ejection - resetting movement state");
    this.clearMovementHistory();
    this.resetStuckCounter();
    this.estimatedPosition = { x: 0, y: 0 };
  }

  public reset(): void {
    this.clearMovementHistory();
    this.resetStuckCounter();
    this.currentDirection = Direction.NORTH;
    this.estimatedPosition = { x: 0, y: 0 };
    this.stats = { successfulMoves: 0, failedMoves: 0, totalTurns: 0 };
  }

  private async performTurn(client: NetworkClient, action: MovementAction, directionChange: number): Promise<boolean> {
    try {
      const turnMethod = action === MovementAction.TURN_RIGHT ? client.turnRight : client.turnLeft;
      const success = await turnMethod.call(client);

      if (success) {
        this.updateDirection(directionChange);
        this.addToHistory(action);
        this.stats = { ...this.stats, totalTurns: this.stats.totalTurns + 1 };
        return true;
      }
      return false;
    } catch (error) {
      logger.error(`Error during ${action}:`, error);
      return false;
    }
  }

  private handleSuccessfulMove(): void {
    this.updateEstimatedPosition();
    this.addToHistory(MovementAction.MOVE_FORWARD);
    this.resetStuckCounter();
    this.stats = { ...this.stats, successfulMoves: this.stats.successfulMoves + 1 };
  }

  private handleFailedMove(): void {
    this.stuckCounter++;
    this.stats = { ...this.stats, failedMoves: this.stats.failedMoves + 1 };
  }

  private updateDirection(change: number): void {
    this.currentDirection = (this.currentDirection + change) % 4 as Direction;
  }

  private updateEstimatedPosition(): void {
    const vector = MovementController.DIRECTION_VECTORS[this.currentDirection];
    this.estimatedPosition.x += vector.x;
    this.estimatedPosition.y += vector.y;
  }

  private calculateOptimalTurn(current: Direction, target: Direction): TurnDirection {
    if (current === target) return TurnDirection.NONE;

    const clockwiseDistance = (target - current + 4) % 4;
    return clockwiseDistance <= 2 ? TurnDirection.RIGHT : TurnDirection.LEFT;
  }

  private addToHistory(action: string): void {
    this.movementHistory.moves.push(action);
    this.movementHistory.positions.push({ ...this.estimatedPosition });
    this.trimHistory();
  }

  private trimHistory(): void {
    const maxHistory = this.movementHistory.maxHistory;

    if (this.movementHistory.moves.length > maxHistory) {
      this.movementHistory.moves.shift();
    }
    if (this.movementHistory.positions.length > maxHistory) {
      this.movementHistory.positions.shift();
    }
  }

  private clearMovementHistory(): void {
    this.movementHistory.moves.length = 0;
    this.movementHistory.positions.length = 0;
  }

  private getRecentMoves(count: number): string[] {
    return this.movementHistory.moves.slice(-count);
  }

  private detectRepeatingPattern(moves: string[]): boolean {
    const halfLength = moves.length / 2;
    const firstHalf = moves.slice(0, halfLength).join(",");
    const secondHalf = moves.slice(halfLength).join(",");

    return firstHalf === secondHalf;
  }

  private selectRandomAction(): MovementAction {
    const actions = [MovementAction.MOVE_FORWARD, MovementAction.TURN_RIGHT, MovementAction.TURN_LEFT];
    return actions[Math.floor(Math.random() * actions.length)];
  }

  private selectRandomTurn(): MovementAction {
    return Math.random() < 0.5 ? MovementAction.TURN_RIGHT : MovementAction.TURN_LEFT;
  }

  private async executeAction(client: NetworkClient, action: MovementAction): Promise<void> {
    try {
      switch (action) {
        case MovementAction.MOVE_FORWARD:
          await this.moveForward(client);
          break;
        case MovementAction.TURN_RIGHT:
          await this.turnRight(client);
          break;
        case MovementAction.TURN_LEFT:
          await this.turnLeft(client);
          break;
      }
    } catch (error) {
      logger.error(`Error executing action ${action}:`, error);
    }
  }
}
