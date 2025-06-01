export interface GameState {
  worldWidth: number;
  worldHeight: number;
  clientsAvailable: number;
  playerX?: number;
  playerY?: number;
  playerLevel: number;
  playerDirection: number; // 0=North, 1=East, 2=South, 3=West
}

export interface InventoryItem {
  food: number;
  linemate: number;
  deraumere: number;
  sibur: number;
  mendiane: number;
  phiras: number;
  thystame: number;
}

export interface BroadcastMessage {
  direction: number;
  message: string;
}

export interface LookResult {
  tiles: string[][];
}

export interface ElevationResult {
  success: boolean;
  newLevel?: number;
}
