import { NetworkClient } from "../network/client";
import { BroadcastMessage, InventoryItem, LookResult } from "../network/types";
import { logger } from "../logger";
import { AIState, GameContext } from "./types";

export class GameLogic {
  private client: NetworkClient;
  private currentState: AIState = AIState.SURVIVAL;
  private timeInState: number = 0;
  private lastTick: number = Date.now();

  private lastInventory?: InventoryItem;
  private lastVision?: LookResult;
  private commandCooldown: boolean = false;

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

      const newState = this.determineNextState(context);

      if (newState !== this.currentState) {
        logger.info(`State transition: ${this.currentState} -> ${newState}`);
        this.currentState = newState;
        this.timeInState = 0;
      }

      await this.executeStrategy(context);
    } catch (error) {
      logger.error("Error in game tick:", error);
    }
  }

  private async updateContext(): Promise<void> {
    try {
      if (!this.lastInventory || this.timeInState % 1000 < 100) {
        this.lastInventory = await this.client.getInventory();
      }

      if (!this.lastVision || this.timeInState % 2000 < 100) {
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

  private determineNextState(context: GameContext): AIState {
    const { inventory } = context;

    // prio 1: survie (low food) ?? i guess
    if (inventory.food < 50) {
      return AIState.SURVIVAL;
    }

    // TODO: maybe voir pour un algo de detrmination plus poussé

    // prio 2: elevation ?
    // prio 3: ressources pour levelup
    // prio 4: exploration

    return AIState.EXPLORATION;
  }

  private async executeStrategy(context: GameContext): Promise<void> {
    this.commandCooldown = true;

    try {
      let actionTaken = false;

      switch (this.currentState) {
        case AIState.SURVIVAL:
          actionTaken = await this.executeSurvival(context);
          break;

        case AIState.EXPLORATION:
          actionTaken = await this.executeExploration(context);
          break;

        case AIState.GATHERING:
          actionTaken = await this.executeGathering(context);
          break;

        case AIState.ELEVATION:
          actionTaken = await this.executeElevation(context);
          break;

        case AIState.COORDINATION:
          actionTaken = await this.executeCoordination(context);
          break;
      }

      if (!actionTaken) {
        await new Promise((resolve) => setTimeout(resolve, 200));
      }
    } finally {
      this.commandCooldown = false;
    }
  }

  private async executeSurvival(context: GameContext): Promise<boolean> {
    // TODO: logique survie
    logger.debug(`Survival mode: ${context.inventory.food} food remaining`);

    await this.client.moveForward();
    return true;
  }

  private async executeExploration(context: GameContext): Promise<boolean> {
    // TODO: logique explo
    logger.debug("Exploration mode");

    // TODO: logique de mouvement
    const actions = ["moveForward", "turnRight", "turnLeft"];
    const randomAction = actions[Math.floor(Math.random() * actions.length)];

    switch (randomAction) {
      case "moveForward":
        await this.client.moveForward();
        break;
      case "turnRight":
        await this.client.turnRight();
        break;
      case "turnLeft":
        await this.client.turnLeft();
        break;
    }

    return true;
  }

  private async executeGathering(context: GameContext): Promise<boolean> {
    // TODO: collecte de ressources
    logger.debug("Gathering mode");

    // TODO: delete ça
    return await this.executeExploration(context);
  }

  private async executeElevation(context: GameContext): Promise<boolean> {
    // TODO: logique d'élévation
    logger.info(
      `Attempting elevation from level ${context.gameState.playerLevel}`
    );

    try {
      const result = await this.client.incantation();
      return result.success;
    } catch (error) {
      logger.error("Elevation error:", error);
      return false;
    }
  }

  private async executeCoordination(context: GameContext): Promise<boolean> {
    // TODO: coordination avec autres joueurs ?
    logger.debug("Coordination mode");

    return false;
  }

  public handleBroadcast(message: BroadcastMessage): void {
    // TODO: recuperation de message broadcast
    logger.debug(
      `Received broadcast from direction ${message.direction}: ${message.message}`
    );
  }

  public handleEjection(direction: number): void {
    logger.warn(`Handling ejection from direction ${direction}`);
    // forcer exploration pour trouver nouvelle position
    this.currentState = AIState.EXPLORATION;
    this.timeInState = 0;
  }
}
