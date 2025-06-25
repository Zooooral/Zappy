import { AIState, GameContext } from "./types";
import { logger } from "../logger";

export class AIStateManager {
  private currentState: AIState = AIState.SURVIVAL;
  private timeInState: number = 0;
  private lastTick: number = Date.now();

  private static readonly FOOD_THRESHOLDS = {
    CRITICAL: 5,
    LOW: 50,
  } as const;

  private static readonly UPDATE_INTERVALS = {
    INVENTORY: 2000,
    VISION: 1000,
    INVENTORY_URGENT: 500,
    TOLERANCE: 100,
  } as const;

  public updateTime(): void {
    const now = Date.now();
    this.timeInState += now - this.lastTick;
    this.lastTick = now;
  }

  public getCurrentState(): AIState {
    return this.currentState;
  }

  public getTimeInState(): number {
    return this.timeInState;
  }

  public updateState(context: GameContext): void {
    const { food } = context.inventory;

    if (this.currentState === AIState.REPRODUCTION) {
      return;
    }

    const newState = this.determineOptimalState(food);

    if (newState !== this.currentState) {
      this.transitionToState(newState, food);
    }
  }

  private determineOptimalState(food: number): AIState {
    if (this.currentState === AIState.REPRODUCTION) {
      return AIState.REPRODUCTION;
    }

    if (food < AIStateManager.FOOD_THRESHOLDS.LOW) {
      return AIState.SURVIVAL;
    }
    return AIState.EXPLORATION;
  }

  private transitionToState(newState: AIState, food: number): void {
    const previousState = this.currentState;

    if (food < AIStateManager.FOOD_THRESHOLDS.CRITICAL) {
      logger.warn(`Critical Survival Mode - food: ${food}`);
    }

    if (newState === AIState.REPRODUCTION) {
      logger.info(`Entering REPRODUCTION mode`);
    } else if (previousState === AIState.REPRODUCTION) {
      logger.info(`Exiting REPRODUCTION mode -> ${newState}`);
    } else {
      logger.info(`State transition: ${previousState} -> ${newState}`);
    }

    this.currentState = newState;
    this.timeInState = 0;
  }

  public setState(newState: AIState): void {
    if (this.currentState !== newState) {
      if (newState === AIState.REPRODUCTION) {
        logger.info(`Force change to REPRODUCTION mode`);
      } else if (this.currentState === AIState.REPRODUCTION) {
        logger.info(`Force exit from REPRODUCTION mode -> ${newState}`);
      } else {
        logger.info(`State force change: ${this.currentState} -> ${newState}`);
      }

      this.currentState = newState;
      this.timeInState = 0;
    }
  }

  public shouldUpdateInventory(): boolean {
    return this.isUpdateDue(AIStateManager.UPDATE_INTERVALS.INVENTORY);
  }

  public shouldUpdateVision(): boolean {
    return this.isUpdateDue(AIStateManager.UPDATE_INTERVALS.VISION);
  }

  public shouldUpdateInventoryUrgent(food: number): boolean {
    const isLowFood = food < AIStateManager.FOOD_THRESHOLDS.LOW;
    const shouldUpdate = this.isUpdateDue(AIStateManager.UPDATE_INTERVALS.INVENTORY_URGENT);
    return isLowFood && shouldUpdate;
  }

  private isUpdateDue(interval: number): boolean {
    return this.timeInState % interval < AIStateManager.UPDATE_INTERVALS.TOLERANCE;
  }

  public reset(): void {
    this.currentState = AIState.SURVIVAL;
    this.timeInState = 0;
    this.lastTick = Date.now();
    logger.info("AIStateManager reinitialized");
  }

  public getDebugInfo(): {
    currentState: AIState;
    timeInState: number;
    lastTick: number;
  } {
    return {
      currentState: this.currentState,
      timeInState: this.timeInState,
      lastTick: this.lastTick,
    };
  }
}
