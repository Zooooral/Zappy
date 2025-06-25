import { logger } from "../logger";
import { InventoryItem, LookResult, BroadcastMessage } from "./types";

export class MessageParser {
    private waitingForElevationResult: boolean = false;

    public parseMessage(message: string): ParsedMessage {
        logger.debug(`[RAW RECEIVED]: ${message}`);

        if (message.startsWith("message ")) {
            return {
                type: "broadcast",
                data: this.parseBroadcast(message)
            };
        }

        if (message.startsWith("eject: ")) {
            return {
                type: "ejection",
                data: this.parseEjection(message)
            };
        }

        if (message === "Elevation underway") {
            this.waitingForElevationResult = true;
            logger.info("Elevation started, waiting for result...");
            return {
                type: "elevation_start",
                data: null
            };
        }

        if (this.waitingForElevationResult && message.startsWith("Current level:")) {
            this.waitingForElevationResult = false;
            const levelMatch = message.match(/Current level: (\d+)/);
            if (levelMatch) {
                const newLevel = parseInt(levelMatch[1]);
                logger.info(`Level up! New level: ${newLevel}`);
                return {
                    type: "command_response",
                    data: { success: true, newLevel }
                };
            }
        }

        if (message === "dead") {
            return {
                type: "death",
                data: "dead"
            };
        }

        return {
            type: "command_response",
            data: this.parseCommandResponse(message)
        };
    }

    public parseHandshake(message: string): HandshakeMessage {
        if (message === "WELCOME") {
            return { type: "welcome" };
        }

        if (message.match(/^\d+$/)) {
            return {
                type: "clients_available",
                data: parseInt(message)
            };
        }

        if (message.match(/^\d+ \d+$/)) {
            const [width, height] = message.split(" ").map(Number);
            return {
                type: "world_size",
                data: { width, height }
            };
        }

        return { type: "unknown", data: message };
    }

    private parseBroadcast(message: string): BroadcastMessage | null {
        const match = message.match(/^message (\d+),\s*(.*)$/);
        if (match) {
            return {
                direction: parseInt(match[1]),
                message: match[2],
            };
        }
        return null;
    }

    private parseEjection(message: string): number | null {
        const match = message.match(/^eject: (\d+)$/);
        if (match) {
            return parseInt(match[1]);
        }
        return null;
    }

    private parseCommandResponse(response: string): any {
        if (response === "ko") {
            return false;
        }

        if (response === "ok") {
            return true;
        }

        if (response === "[]") {
            return { tiles: [[]] };
        }

        if (response.startsWith("[") && response.endsWith("]")) {
            if (this.isLookResponse(response)) {
                return this.parseLookResponse(response);
            } else if (this.isInventoryResponse(response)) {
                return this.parseInventoryResponse(response);
            }
        }

        if (/^\d+$/.test(response)) {
            return parseInt(response);
        }

        logger.warn(`Unknown response format: ${response}`);
        return response;
    }

    private isLookResponse(response: string): boolean {
        return response.includes("player") || response.includes("food") || response.includes("linemate") || response.includes("deraumere") || response.includes("sibur") || response.includes("mendiane") || response.includes("phiras") || response.includes("thystame") || (!response.includes(" ") && response.length > 2) || response.includes(",");
    }

    private isInventoryResponse(response: string): boolean {
        const content = response.slice(1, -1);
        return content.includes("food ") || content.includes("linemate ") || content.includes("deraumere ") || content.includes("sibur ") || content.includes("mendiane ") || content.includes("phiras ") || content.includes("thystame ");
    }

    private parseLookResponse(response: string): LookResult {
        const content = response.slice(1, -1);

        if (!content || content.trim() === "") {
            return { tiles: [[]] };
        }

        const tiles = content.split(",").map((tile) => {
            const trimmed = tile.trim();
            if (!trimmed) {
                return [];
            }
            return trimmed.split(" ").filter((item) => item.length > 0);
        });

        logger.debug(`Parsed look result: ${JSON.stringify(tiles)}`);
        return { tiles };
    }

    private parseInventoryResponse(response: string): InventoryItem {
        const inventory: InventoryItem = {
            food: 0,
            linemate: 0,
            deraumere: 0,
            sibur: 0,
            mendiane: 0,
            phiras: 0,
            thystame: 0,
        };

        const content = response.replace(/^\[|\]$/g, '').trim();
        if (!content) return inventory;

        const items = content.split(',');

        for (const item of items) {
            const cleaned = item.trim();
            if (!cleaned) continue;

            const lastSpaceIndex = cleaned.lastIndexOf(' ');
            if (lastSpaceIndex === -1) continue;

            const itemName = cleaned.substring(0, lastSpaceIndex).trim();
            const quantityStr = cleaned.substring(lastSpaceIndex + 1).trim();
            const quantity = parseInt(quantityStr);

            if (isNaN(quantity)) continue;

            if (itemName in inventory) {
                (inventory as any)[itemName] = quantity;
            }
        }

        return inventory;
    }
}

export interface ParsedMessage {
    type: "broadcast" | "ejection" | "elevation_start" | "command_response" | "death";
    data: any;
}

export interface HandshakeMessage {
    type: "welcome" | "clients_available" | "world_size" | "unknown";
    data?: any;
}
