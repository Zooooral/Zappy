import { NetworkClient } from "../network";
import { logger } from "../logger";

interface Position {
    x: number;
    y: number;
}

interface MovementHistory {
    moves: string[];
    positions: Position[];
    maxHistory: number;
}

export class MovementController {
    private movementHistory: MovementHistory = {
        moves: [],
        positions: [],
        maxHistory: 10
    };

    private stuckCounter: number = 0;
    private lastDirection: number = 0; // 0=North, 1=East, 2=South, 3=West
    private estimatedPosition: Position = { x: 0, y: 0 };

    public getLastDirection(): number {
        return this.lastDirection;
    }

    public getStuckCounter(): number {
        return this.stuckCounter;
    }

    public resetStuckCounter(): void {
        this.stuckCounter = 0;
    }

    public incrementStuckCounter(): void {
        this.stuckCounter++;
    }

    public async moveForward(client: NetworkClient): Promise<boolean> {
        try {
            const moved = await client.moveForward();
            if (moved) {
                this.updateEstimatedPosition();
                this.addToMovementHistory("moveForward");
                this.resetStuckCounter();
                return true;
            } else {
                this.incrementStuckCounter();
                return false;
            }
        } catch (error) {
            logger.error("Error moving forward:", error);
            this.incrementStuckCounter();
            return false;
        }
    }

    public async turnRight(client: NetworkClient): Promise<boolean> {
        try {
            const turned = await client.turnRight();
            if (turned) {
                this.lastDirection = (this.lastDirection + 1) % 4;
                this.addToMovementHistory("turnRight");
                return true;
            }
            return false;
        } catch (error) {
            logger.error("Error turning right:", error);
            return false;
        }
    }

    public async turnLeft(client: NetworkClient): Promise<boolean> {
        try {
            const turned = await client.turnLeft();
            if (turned) {
                this.lastDirection = (this.lastDirection + 3) % 4;
                this.addToMovementHistory("turnLeft");
                return true;
            }
            return false;
        } catch (error) {
            logger.error("Error turning left:", error);
            return false;
        }
    }

    public async turnToDirection(client: NetworkClient, targetDirection: number): Promise<boolean> {
        const currentDirection = this.lastDirection;
        const turnDirection = this.calculateTurnDirection(currentDirection, targetDirection);

        if (turnDirection === "right") {
            logger.debug("Turning right to target direction");
            return await this.turnRight(client);
        } else if (turnDirection === "left") {
            logger.debug("Turning left to target direction");
            return await this.turnLeft(client);
        }

        return true; // Already facing the right direction
    }

    public async moveTowardsDirection(
        client: NetworkClient,
        targetDirection: number
    ): Promise<boolean> {
        await this.turnToDirection(client, targetDirection);
        return await this.moveForward(client);
    }

    public isStuckInLoop(): boolean {
        if (this.movementHistory.moves.length < 6) {
            return false;
        }

        const recentMoves = this.movementHistory.moves.slice(-6);
        const pattern = recentMoves.slice(0, 3).join(",");
        const nextPattern = recentMoves.slice(3, 6).join(",");

        return pattern === nextPattern;
    }

    public async breakOutOfLoop(client: NetworkClient): Promise<void> {
        logger.info("Breaking out of loop with random actions");

        this.movementHistory.moves = [];
        this.resetStuckCounter();

        const actions = ["moveForward", "turnRight", "turnLeft"];
        const randomAction = actions[Math.floor(Math.random() * actions.length)];

        try {
            switch (randomAction) {
                case "moveForward":
                    await this.moveForward(client);
                    break;
                case "turnRight":
                    await this.turnRight(client);
                    break;
                case "turnLeft":
                    await this.turnLeft(client);
                    break;
            }

            this.addToMovementHistory(randomAction);
        } catch (error) {
            logger.error("Error while breaking out of loop:", error);
        }
    }

    public async exploreRandomly(client: NetworkClient): Promise<boolean> {
        if (this.isStuckInLoop() || this.stuckCounter > 3) {
            await this.breakOutOfLoop(client);
            return true;
        }

        try {
            if (Math.random() < 0.7) {
                logger.debug("Exploring: moving forward");
                await this.moveForward(client);
            } else {
                if (Math.random() < 0.5) {
                    logger.debug("Exploring: turning right");
                    await this.turnRight(client);
                } else {
                    logger.debug("Exploring: turning left");
                    await this.turnLeft(client);
                }
            }
        } catch (error) {
            logger.error("Error during random exploration:", error);
        }

        return true;
    }

    private calculateTurnDirection(currentDir: number, targetDir: number): "left" | "right" | "none" {
        if (currentDir === targetDir) {
            return "none";
        }

        const diff = (targetDir - currentDir + 4) % 4;
        return diff <= 2 ? "right" : "left";
    }

    private addToMovementHistory(move: string): void {
        this.movementHistory.moves.push(move);
        if (this.movementHistory.moves.length > this.movementHistory.maxHistory) {
            this.movementHistory.moves.shift();
        }

        this.movementHistory.positions.push({ ...this.estimatedPosition });
        if (this.movementHistory.positions.length > this.movementHistory.maxHistory) {
            this.movementHistory.positions.shift();
        }
    }

    private updateEstimatedPosition(): void {
        switch (this.lastDirection) {
            case 0: // North
                this.estimatedPosition.y--;
                break;
            case 1: // East
                this.estimatedPosition.x++;
                break;
            case 2: // South
                this.estimatedPosition.y++;
                break;
            case 3: // West
                this.estimatedPosition.x--;
                break;
        }
    }

    public handleEjection(): void {
        logger.warn("Handling ejection - resetting movement state");
        this.movementHistory.moves = [];
        this.resetStuckCounter();
    }
}