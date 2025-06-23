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
    const newState = this.determineOptimalState(food);

    if (newState !== this.currentState) {
      this.transitionToState(newState, food);
    }
  }

  private determineOptimalState(food: number): AIState {
    if (food < AIStateManager.FOOD_THRESHOLDS.LOW) {
      return AIState.SURVIVAL;
    }
    return AIState.EXPLORATION;
  }

  private transitionToState(newState: AIState, food: number): void {
    const previousState = this.currentState;

    if (food < AIStateManager.FOOD_THRESHOLDS.CRITICAL) {
      logger.warn(`Mode survie critique - nourriture: ${food}`);
    }
    logger.info(`Transition d'état: ${previousState} -> ${newState}`);
    this.currentState = newState;
    this.timeInState = 0;
  }

  public setState(newState: AIState): void {
    if (this.currentState !== newState) {
      logger.info(`Changement d'état forcé: ${this.currentState} -> ${newState}`);
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
    logger.info("AIStateManager réinitialisé");
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
