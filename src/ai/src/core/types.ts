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

export interface ElevationRequirement {
  players: number;
  linemate: number;
  deraumere: number;
  sibur: number;
  mendiane: number;
  phiras: number;
  thystame: number;
}

export interface AIMetrics {
  reproductionAttempts: number;
  elevationAttempts: number;
  resourcesCollected: { [key: string]: number };
  survivalTime: number;
  deathCount: number;
}
