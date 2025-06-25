import { InventoryItem, LookResult, GameState } from "../network/types";

export interface AIConfig {
    host: string;
    port: number;
    teamName: string;
    maxRetries: number;
    retryDelay: number;
}

export enum AIState {
    SURVIVAL = "survival",
    EXPLORATION = "exploration",
    GATHERING = "gathering",
    REPRODUCTION = "reproduction",
    ELEVATION = "elevation",
    COORDINATION = "coordination",
}

export interface GameContext {
    inventory: InventoryItem;
    vision: LookResult;
    gameState: GameState;
    currentState: AIState;
    timeInState: number;
}
