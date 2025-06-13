import { AIState, GameContext } from "./types";
import { logger } from "../logger";

export class AIStateManager {
    private currentState: AIState = AIState.SURVIVAL;
    private timeInState: number = 0;
    private lastTick: number = Date.now();

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
        const food = context.inventory.food;
        const level = context.gameState.playerLevel;

        if (food < 5) {
            this.setState(AIState.SURVIVAL);
            logger.warn(`Critical survival mode - food: ${food}`);
        } else if (food < 50) {
            this.setState(AIState.SURVIVAL);
        } else {
            this.setState(AIState.EXPLORATION);
        }
    }

    public setState(newState: AIState): void {
        if (this.currentState !== newState) {
            logger.info(`State transition: ${this.currentState} -> ${newState}`);
            this.currentState = newState;
            this.timeInState = 0;
        }
    }

    public shouldUpdateInventory(): boolean {
        return this.timeInState % 2000 < 100;
    }

    public shouldUpdateVision(): boolean {
        return this.timeInState % 1000 < 100;
    }

    public shouldUpdateInventoryUrgent(food: number): boolean {
        return food < 50 && this.timeInState % 500 < 100;
    }
}