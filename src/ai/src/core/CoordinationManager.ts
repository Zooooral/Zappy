import { NetworkClient } from "../network";
import { BroadcastMessage } from "../network/types";
import { logger } from "../logger";
import { GameContext } from "./types";
import { MovementController } from "./MovementController";
import { VisionCalculator } from "./VisionCalculator";

interface ElevationMeeting {
    level: number;
    isInitiator: boolean;
    partnerDirection?: number;
    meetingInProgress: boolean;
    waitingStartTime: number;
    lastMovementTime: number;
    lastBroadcastTime: number;
}

export class CoordinationManager {
    private currentMeeting?: ElevationMeeting;
    private movementController: MovementController;
    private visionCalculator: VisionCalculator;
    private lastBroadcastTime: number = 0;

    private static readonly BROADCAST_INTERVAL = 3000;
    private static readonly MEETING_TIMEOUT = 60000;
    private static readonly MOVEMENT_INTERVAL = 1500;

    constructor(movementController: MovementController, visionCalculator: VisionCalculator) {
        this.movementController = movementController;
        this.visionCalculator = visionCalculator;
    }

    public async handleElevationBroadcast(message: BroadcastMessage, context: GameContext, client: NetworkClient): Promise<void> {
        if (message.direction === 0) {
            return;
        }
        logger.info(`[COORDINATION] Processing broadcast: "${message.message}" from direction ${message.direction}`);
        if (this.isElevationRequestMessage(message)) {
            await this.handleElevationRequestMessage(message, context, client);
        } else if (this.isElevationResponseMessage(message)) {
            this.handleElevationResponseMessage(message, context);
        } else if (this.isElevationWaitingMessage(message)) {
            this.handleElevationWaitingMessage(message, context);
        }
    }

    public async requestElevationPartner(client: NetworkClient, context: GameContext): Promise<void> {
        const now = Date.now();
        if (now - this.lastBroadcastTime < CoordinationManager.BROADCAST_INTERVAL) {
            return;
        }

        const level = context.gameState.playerLevel;
        const message = `ELEVATION_REQUEST_LVL_${level}`;

        try {
            await client.broadcast(message);
            this.lastBroadcastTime = now;
        } catch (error) {
            logger.error("[COORDINATION] Failed to broadcast elevation request:", error);
        }
    }

    public async coordinateElevationMeeting(client: NetworkClient, context: GameContext): Promise<boolean> {
        if (!this.currentMeeting) {
            return false;
        }

        const now = Date.now();
        const timeInMeeting = now - this.currentMeeting.waitingStartTime;

        if (timeInMeeting > CoordinationManager.MEETING_TIMEOUT) {
            this.endElevationMeeting();
            return false;
        }

        const currentTile = this.getCurrentTile(context);
        if (currentTile) {
            const playerCount = this.countPlayersOnTile(currentTile);
            const requiredPlayers = this.getRequiredPlayersForLevel(this.currentMeeting.level);

            if (playerCount >= requiredPlayers) {
                return true;
            }
        }
        if (this.currentMeeting.isInitiator) {
            await this.handleInitiatorBehavior(client, context, now);
        } else {
            await this.handleResponderBehavior(client, context, now);
        }
        return false;
    }

    public startElevationMeeting(level: number, isInitiator: boolean, partnerDirection?: number): void {
        if (this.currentMeeting?.meetingInProgress) {
            logger.debug(`[COORDINATION] Already in meeting for level ${this.currentMeeting.level}, ignoring new request`);
            return;
        }
        this.currentMeeting = {
            level,
            isInitiator,
            partnerDirection,
            meetingInProgress: true,
            waitingStartTime: Date.now(),
            lastMovementTime: 0,
            lastBroadcastTime: 0
        };
    }

    public isInElevationMeeting(): boolean {
        return this.currentMeeting?.meetingInProgress ?? false;
    }

    public getCurrentMeetingLevel(): number | undefined {
        return this.currentMeeting?.level;
    }

    public endElevationMeeting(): void {
        if (this.currentMeeting) {
            this.currentMeeting = undefined;
        }
    }

    public cleanupExpiredRequests(): void {}

    public reset(): void {
        this.currentMeeting = undefined;
        this.lastBroadcastTime = 0;
    }

    public getMeetingStatus(): string {
        if (!this.currentMeeting) {
            return "No active meeting";
        }
        const duration = Date.now() - this.currentMeeting.waitingStartTime;
        const role = this.currentMeeting.isInitiator ? 'Initiator' : 'Responder';
        return `${role} meeting for level ${this.currentMeeting.level} (${Math.round(duration/1000)}s)`;
    }

    private isElevationRequestMessage(message: BroadcastMessage): boolean {
        return message.message.includes("ELEVATION_REQUEST_LVL_");
    }

    private isElevationResponseMessage(message: BroadcastMessage): boolean {
        return message.message.includes("ELEVATION_RESPONSE_LVL_");
    }

    private isElevationWaitingMessage(message: BroadcastMessage): boolean {
        return message.message.includes("ELEVATION_WAITING_LVL_");
    }

    private async handleElevationRequestMessage(message: BroadcastMessage, context: GameContext, client: NetworkClient): Promise<void> {
        const levelMatch = message.message.match(/ELEVATION_REQUEST_LVL_(\d+)/);
        if (!levelMatch) return;
        const requestedLevel = parseInt(levelMatch[1]);
        const myLevel = context.gameState.playerLevel;
        if (requestedLevel !== myLevel) {
            logger.info(`[COORDINATION] Ignoring request - level mismatch (${requestedLevel} vs ${myLevel})`);
            return;
        }
        if (this.currentMeeting?.meetingInProgress) {
            logger.info(`[COORDINATION] Ignoring request - already in meeting for level ${this.currentMeeting.level}`);
            return;
        }
        if (!this.hasElevationResourcesForLevel(context, requestedLevel)) {
            logger.info(`[COORDINATION] Ignoring request - don't have elevation resources for level ${requestedLevel}`);
            return;
        }
        const responseMessage = `ELEVATION_RESPONSE_LVL_${requestedLevel}`;
        try {
            await client.broadcast(responseMessage);
            this.startElevationMeeting(requestedLevel, false, message.direction);
        } catch (error) {
            logger.error("[COORDINATION] Failed to respond to elevation request:", error);
        }
    }

    private handleElevationResponseMessage(message: BroadcastMessage, context: GameContext): void {
        const match = message.message.match(/ELEVATION_RESPONSE_LVL_(\d+)/);
        if (!match) return;
        const level = parseInt(match[1]);
        if (level !== context.gameState.playerLevel) {
            logger.info(`[COORDINATION] Ignoring response - level mismatch (${level} vs ${context.gameState.playerLevel})`);
            return;
        }
        if (this.currentMeeting?.meetingInProgress) {
            logger.info(`[COORDINATION] Ignoring response - already in meeting`);
            return;
        }
        logger.info(`[COORDINATION] Starting meeting as initiator with responder from direction ${message.direction}`);
        this.startElevationMeeting(level, true, message.direction);
    }

    private handleElevationWaitingMessage(message: BroadcastMessage, context: GameContext): void {
        const levelMatch = message.message.match(/ELEVATION_WAITING_LVL_(\d+)/);
        if (!levelMatch) return;

        const level = parseInt(levelMatch[1]);
        if (level === context.gameState.playerLevel && !this.currentMeeting?.meetingInProgress) {
            if (this.hasElevationResourcesForLevel(context, level)) {
                this.startElevationMeeting(level, false, message.direction);
            } else {
                logger.info(`[COORDINATION] Can't join - missing elevation resources for level ${level}`);
            }
        }
    }

    private async handleInitiatorBehavior(client: NetworkClient, context: GameContext, now: number): Promise<void> {
        if (!this.currentMeeting) return;

        if (now - this.currentMeeting.lastBroadcastTime > CoordinationManager.BROADCAST_INTERVAL) {
            try {
                const waitingMessage = `ELEVATION_WAITING_LVL_${this.currentMeeting.level}`;
                await client.broadcast(waitingMessage);
                this.currentMeeting.lastBroadcastTime = now;
            } catch (error) {
                logger.error("[COORDINATION] Failed to broadcast waiting message:", error);
            }
        }
    }

    private async handleResponderBehavior(client: NetworkClient, context: GameContext, now: number): Promise<void> {
        if (!this.currentMeeting?.partnerDirection) return;

        if (now - this.currentMeeting.lastMovementTime > CoordinationManager.MOVEMENT_INTERVAL) {
            try {
                const targetDirection = this.calculateMovementDirection(this.currentMeeting.partnerDirection);
                const moved = await this.movementController.moveTowardsDirection(client, targetDirection);
                this.currentMeeting.lastMovementTime = now;
                if (!moved) {
                    await this.movementController.exploreRandomly(client);
                }
            } catch (error) {
                logger.error("[COORDINATION] Error moving towards elevation partner:", error);
            }
        }
    }

    private hasElevationResourcesForLevel(context: GameContext, level: number): boolean {
        const requirements = this.getElevationRequirements(level);
        if (!requirements) return false;
        const hasResources = Object.entries(requirements).every(([resource, required]) => {
            if (resource === 'players') return true;
            const available = (context.inventory as any)[resource] || 0;
            return available >= required;
        });
        logger.debug(`[COORDINATION] Elevation resources check for level ${level}: ${hasResources ? 'PASS' : 'FAIL'}`);
        return hasResources;
    }

    private getElevationRequirements(level: number): Record<string, number> | null {
        const requirements: Record<number, Record<string, number>> = {
            1: { players: 1, linemate: 1, deraumere: 0, sibur: 0, mendiane: 0, phiras: 0, thystame: 0 },
            2: { players: 2, linemate: 1, deraumere: 1, sibur: 1, mendiane: 0, phiras: 0, thystame: 0 },
            3: { players: 2, linemate: 2, deraumere: 0, sibur: 1, mendiane: 0, phiras: 2, thystame: 0 },
            4: { players: 4, linemate: 1, deraumere: 1, sibur: 2, mendiane: 0, phiras: 1, thystame: 0 },
            5: { players: 4, linemate: 1, deraumere: 2, sibur: 1, mendiane: 3, phiras: 0, thystame: 0 },
            6: { players: 6, linemate: 1, deraumere: 2, sibur: 3, mendiane: 0, phiras: 1, thystame: 0 },
            7: { players: 6, linemate: 2, deraumere: 2, sibur: 2, mendiane: 2, phiras: 2, thystame: 1 }
        };
        return requirements[level] || null;
    }

    private getCurrentTile(context: GameContext): string[] | null {
        return context.vision.tiles?.[0] || null;
    }

    private countPlayersOnTile(tile: string[]): number {
        return tile.filter(item => item === "player").length;
    }

    private getRequiredPlayersForLevel(level: number): number {
        const requirements: Record<number, number> = {
            1: 1, 2: 2, 3: 2, 4: 4, 5: 4, 6: 6, 7: 6
        };
        return requirements[level] || 1;
    }

    private calculateMovementDirection(broadcastDirection: number): number {
        switch (broadcastDirection) {
            case 1: return 0; // Nord
            case 2: case 3: return 1; // Est
            case 4: case 5: return 2; // Sud
            case 6: case 7: return 3; // Ouest
            case 8: return 0; // Nord
            default: return 0;
        }
    }

    public getDebugInfo(): any {
        return {
            currentMeeting: this.currentMeeting,
            lastBroadcastTime: this.lastBroadcastTime,
            isInMeeting: this.isInElevationMeeting(),
            meetingStatus: this.getMeetingStatus(),
        };
    }
}
