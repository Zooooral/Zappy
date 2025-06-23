import { logger } from "../logger";

interface Position {
    x: number;
    y: number;
}

export class VisionCalculator {
    public findClosestFood(tiles: string[][]): number {
        for (let i = 1; i < tiles.length; i++) {
            if (tiles[i].includes("food")) {
                logger.debug(`Food found at tile index ${i}`);
                return i;
            }
        }
        return -1;
    }

    public findClosestResource(tiles: string[][], resourceType: string): number {
        for (let i = 1; i < tiles.length; i++) {
            if (tiles[i].includes(resourceType)) {
                logger.debug(`${resourceType} found at tile index ${i}`);
                return i;
            }
        }
        return -1;
    }

    public getTileDirection(tileIndex: number, playerLevel: number): number {
        const visionPattern = this.generateVisionPattern(playerLevel);

        if (tileIndex >= visionPattern.length) {
            return -1;
        }

        const relativePos = visionPattern[tileIndex];
        if (!relativePos) {
            return -1;
        }

        if (relativePos.x === 0 && relativePos.y > 0) return 0; // North
        if (relativePos.x > 0 && relativePos.y === 0) return 1; // East
        if (relativePos.x === 0 && relativePos.y < 0) return 2; // South
        if (relativePos.x < 0 && relativePos.y === 0) return 3; // West

        if (relativePos.x > 0 && relativePos.y > 0) return 1; // Northeast -> East
        if (relativePos.x > 0 && relativePos.y < 0) return 1; // Southeast -> East
        if (relativePos.x < 0 && relativePos.y > 0) return 3; // Northwest -> West
        if (relativePos.x < 0 && relativePos.y < 0) return 3; // Southwest -> West

        return -1;
    }

    public generateVisionPattern(level: number): Array<Position> {
        const pattern: Array<Position> = [];

        pattern.push({ x: 0, y: 0 });

        for (let distance = 1; distance <= level; distance++) {
            for (let x = -distance; x <= distance; x++) {
                pattern.push({ x, y: distance });
            }
            for (let y = distance - 1; y >= -distance + 1; y--) {
                pattern.push({ x: -distance, y });
                pattern.push({ x: distance, y });
            }
            if (distance > 1) {
                for (let x = -distance + 1; x < distance; x++) {
                    pattern.push({ x, y: -distance });
                }
            }
        }

        return pattern;
    }

    public calculateTurnDirection(
        currentDir: number,
        targetDir: number
    ): "left" | "right" | "none" {
        if (currentDir === targetDir) {
            return "none";
        }

        const diff = (targetDir - currentDir + 4) % 4;
        return diff <= 2 ? "right" : "left";
    }

    public countResourcesOnTile(tile: string[], resourceType: string): number {
        return tile.filter((item) => item === resourceType).length;
    }

    public hasResourceOnTile(tile: string[], resourceType: string): boolean {
        return tile.includes(resourceType);
    }

    public getAvailableResources(tile: string[]): string[] {
        const resources = [
            "food",
            "linemate",
            "deraumere",
            "sibur",
            "mendiane",
            "phiras",
            "thystame",
        ];
        return tile.filter((item) => resources.includes(item));
    }
}
