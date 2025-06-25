import { NetworkClient } from "../network";
import { BroadcastMessage } from "../network/types";
import { logger } from "../logger";
import { GameContext } from "./types";
import { MovementController } from "./MovementController";
import { VisionCalculator } from "./VisionCalculator";

interface ElevationRequest {
    level: number;
    direction: number;
    timestamp: number;
    responded: boolean;
}

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
    private elevationRequests: Map<number, ElevationRequest> = new Map();
    private currentMeeting?: ElevationMeeting;
    private movementController: MovementController;
    private visionCalculator: VisionCalculator;
    private lastBroadcastTime: number = 0;

    private static readonly BROADCAST_COOLDOWN = 2000;
    private static readonly MEETING_TIMEOUT = 45000;
    private static readonly MOVEMENT_INTERVAL = 1000;
    private static readonly REQUEST_EXPIRY = 20000;
    private static readonly RESPONSE_COOLDOWN = 1000;

    constructor(movementController: MovementController, visionCalculator: VisionCalculator) {
        this.movementController = movementController;
        this.visionCalculator = visionCalculator;
    }

    public async handleElevationBroadcast(message: BroadcastMessage, context: GameContext, client: NetworkClient): Promise<void> {
        if (this.shouldIgnoreBroadcast(message, context)) {
            return;
        }

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
        if (now - this.lastBroadcastTime < CoordinationManager.BROADCAST_COOLDOWN) {
            return;
        }
        const level = context.gameState.playerLevel;
        if (this.elevationRequests.has(level)) {
            logger.debug(`Already have pending request for level ${level}`);
            return;
        }
        const message = `ELEVATION_REQUEST_LVL_${level}`;
        try {
            await client.broadcast(message);
            this.lastBroadcastTime = now;
            logger.info(`Broadcasting elevation request for level ${level}`);
            this.elevationRequests.set(level, {
                level,
                direction: 0,
                timestamp: now,
                responded: false
            });
        } catch (error) {
            logger.error("Failed to broadcast elevation request:", error);
        }
    }

    public async respondToElevationRequest(client: NetworkClient, requestDirection: number, level: number): Promise<void> {
        const now = Date.now();
        if (now - this.lastBroadcastTime < CoordinationManager.RESPONSE_COOLDOWN) {
            return;
        }
        const message = `ELEVATION_RESPONSE_LVL_${level}_FROM_${requestDirection}`;
        try {
            await client.broadcast(message);
            this.lastBroadcastTime = now;
            logger.info(`Responding to elevation request for level ${level} from direction ${requestDirection}`);
        } catch (error) {
            logger.error("Failed to respond to elevation request:", error);
        }
    }

    public async coordinateElevationMeeting(client: NetworkClient, context: GameContext): Promise<boolean> {
        if (!this.currentMeeting) {
            return false;
        }
        const now = Date.now();
        if (now - this.currentMeeting.waitingStartTime > CoordinationManager.MEETING_TIMEOUT) {
            logger.warn("Elevation meeting timed out, ending meeting");
            this.endElevationMeeting();
            return false;
        }
        const currentTile = this.getCurrentTile(context);
        if (currentTile && this.hasEnoughPlayersForElevation(currentTile, this.currentMeeting.level)) {
            logger.info(`Found ${this.countPlayersOnTile(currentTile)} players on current tile for level ${this.currentMeeting.level} elevation!`);
            return true;
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
            logger.debug("Already in a meeting, ignoring new meeting request");
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

        logger.info(`Started elevation meeting for level ${level} as ${isInitiator ? 'initiator' : 'responder'}${partnerDirection ? ` (partner from direction ${partnerDirection})` : ''}`);
    }

    public isInElevationMeeting(): boolean {
        return this.currentMeeting?.meetingInProgress ?? false;
    }

    public getCurrentMeetingLevel(): number | undefined {
        return this.currentMeeting?.level;
    }

    public endElevationMeeting(): void {
        if (this.currentMeeting) {
            logger.info(`Ending elevation meeting for level ${this.currentMeeting.level}`);
            this.currentMeeting = undefined;
        }
        this.elevationRequests.clear();
    }

    public cleanupExpiredRequests(): void {
        const now = Date.now();
        for (const [key, request] of this.elevationRequests.entries()) {
            if (now - request.timestamp > CoordinationManager.REQUEST_EXPIRY) {
                this.elevationRequests.delete(key);
                logger.debug(`Cleaned up expired elevation request for level ${request.level}`);
            }
        }
    }

    public reset(): void {
        this.currentMeeting = undefined;
        this.elevationRequests.clear();
        this.lastBroadcastTime = 0;
        logger.info("CoordinationManager reset");
    }

    public hasActiveRequests(): boolean {
        return this.elevationRequests.size > 0;
    }

    public getPendingRequestsCount(): number {
        return this.elevationRequests.size;
    }

    public getMeetingStatus(): string {
        if (!this.currentMeeting) {
            return "No active meeting";
        }
        const duration = Date.now() - this.currentMeeting.waitingStartTime;
        return `${this.currentMeeting.isInitiator ? 'Initiator' : 'Responder'} meeting for level ${this.currentMeeting.level} (${Math.round(duration/1000)}s)`;
    }

    private shouldIgnoreBroadcast(message: BroadcastMessage, context: GameContext): boolean {
        if (message.direction === 0) {
            return true;
        }
        return false;
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
            logger.debug(`Ignoring elevation request for level ${requestedLevel} (I'm level ${myLevel})`);
            return;
        }
        if (this.currentMeeting?.meetingInProgress) {
            logger.debug("Already in a meeting, ignoring elevation request");
            return;
        }
        logger.info(`Received elevation request for level ${requestedLevel} from direction ${message.direction}`);
        await this.respondToElevationRequest(client, message.direction, requestedLevel);
        this.startElevationMeeting(requestedLevel, false, message.direction);
    }

    private handleElevationResponseMessage(message: BroadcastMessage, context: GameContext): void {
        const match = message.message.match(/ELEVATION_RESPONSE_LVL_(\d+)_FROM_(\d+)/);
        if (!match) return;

        const level = parseInt(match[1]);
        const fromDirection = parseInt(match[2]);
        if (level !== context.gameState.playerLevel) {
            return;
        }
        if (!this.elevationRequests.has(level) && !this.currentMeeting?.isInitiator) {
            logger.debug("Received response but not expecting one");
            return;
        }
        logger.info(`Received positive response for level ${level} elevation from direction ${message.direction}`);
        this.startElevationMeeting(level, true, message.direction);
    }

    private handleElevationWaitingMessage(message: BroadcastMessage, context: GameContext): void {
        const levelMatch = message.message.match(/ELEVATION_WAITING_LVL_(\d+)/);
        if (!levelMatch) return;

        const level = parseInt(levelMatch[1]);
        if (level === context.gameState.playerLevel && !this.currentMeeting?.meetingInProgress) {
            logger.info(`Found waiting player for level ${level} at direction ${message.direction}`);
            this.startElevationMeeting(level, false, message.direction);
        }
    }

    private async handleInitiatorBehavior(client: NetworkClient, context: GameContext, now: number): Promise<void> {
        if (!this.currentMeeting) return;

        if (now - this.currentMeeting.lastBroadcastTime > CoordinationManager.BROADCAST_COOLDOWN) {
            try {
                await client.broadcast(`ELEVATION_WAITING_LVL_${this.currentMeeting.level}`);
                this.currentMeeting.lastBroadcastTime = now;
                logger.debug(`Broadcasting waiting message for level ${this.currentMeeting.level}`);
            } catch (error) {
                logger.error("Failed to broadcast waiting message:", error);
            }
        }
    }

    private async handleResponderBehavior(client: NetworkClient, context: GameContext, now: number): Promise<void> {
        if (!this.currentMeeting?.partnerDirection) return;

        if (now - this.currentMeeting.lastMovementTime > CoordinationManager.MOVEMENT_INTERVAL) {
            try {
                const targetDirection = this.calculateMovementDirection(this.currentMeeting.partnerDirection);
                logger.debug(`Moving towards elevation partner (broadcast dir: ${this.currentMeeting.partnerDirection}, target dir: ${targetDirection})`);
                const moved = await this.movementController.moveTowardsDirection(client, targetDirection);
                this.currentMeeting.lastMovementTime = now;
                if (!moved) {
                    await this.movementController.exploreRandomly(client);
                    logger.debug("Direct movement failed, exploring randomly");
                }
            } catch (error) {
                logger.error("Error moving towards elevation partner:", error);
            }
        }
    }

    private getCurrentTile(context: GameContext): string[] | null {
        return context.vision.tiles?.[0] || null;
    }

    private hasEnoughPlayersForElevation(tile: string[], level: number): boolean {
        const playerCount = this.countPlayersOnTile(tile);
        const required = this.getRequiredPlayersForLevel(level);
        return playerCount >= required;
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
            case 1: return 0;
            case 2: return 1;
            case 3: return 1;
            case 4: return 2;
            case 5: return 2;
            case 6: return 3;
            case 7: return 3;
            case 8: return 0;
            default: return 0;
        }
    }

    public getDebugInfo(): any {
        return {
            elevationRequests: Array.from(this.elevationRequests.entries()),
            currentMeeting: this.currentMeeting,
            lastBroadcastTime: this.lastBroadcastTime,
            isInMeeting: this.isInElevationMeeting(),
            meetingStatus: this.getMeetingStatus(),
            activeRequests: this.hasActiveRequests(),
            pendingRequestsCount: this.getPendingRequestsCount()
        };
    }
}
