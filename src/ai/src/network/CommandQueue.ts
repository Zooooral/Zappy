import { logger } from "../logger";

export interface PendingCommand {
    command: string;
    resolve: (value: any) => void;
    reject: (error: Error) => void;
    timestamp: number;
}

export class CommandQueue {
    private pendingCommands: PendingCommand[] = [];
    private readonly maxPendingCommands: number = 10;

    constructor() {
    }

    public canAddCommand(): boolean {
        return this.pendingCommands.length < this.maxPendingCommands;
    }

    public addCommand(
        command: string,
        resolve: (value: any) => void,
        reject: (error: Error) => void
    ): void {
        const pendingCommand: PendingCommand = {
            command,
            resolve,
            reject,
            timestamp: Date.now()
        };

        this.pendingCommands.push(pendingCommand);
        logger.info(
            `[SENT]: ${command} (pending: ${this.pendingCommands.length}/${this.maxPendingCommands})`
        );
    }

    public resolveNext(result: any): void {
        if (this.pendingCommands.length > 0) {
            const command = this.pendingCommands.shift()!;

            const duration = Date.now() - command.timestamp;
            logger.info(
                `[COMMAND COMPLETED]: ${command.command} -> resolved in ${duration}ms`
            );

            command.resolve(result);
        }
    }

    public rejectNext(error: Error): void {
        if (this.pendingCommands.length > 0) {
            const command = this.pendingCommands.shift()!;

            logger.error(
                `[COMMAND FAILED]: ${command.command} -> ${error.message}`
            );
            command.reject(error);
        }
    }

    public rejectAll(reason: string): void {
        while (this.pendingCommands.length > 0) {
            const command = this.pendingCommands.shift()!;
            command.reject(new Error(reason));
        }
    }

    public isEmpty(): boolean {
        return this.pendingCommands.length === 0;
    }

    public size(): number {
        return this.pendingCommands.length;
    }

    public getStats(): any {
        return {
            pendingCommands: this.pendingCommands.length,
            maxPendingCommands: this.maxPendingCommands,
            oldestPendingCommand:
                this.pendingCommands.length > 0
                    ? Date.now() - this.pendingCommands[0].timestamp
                    : 0
        };
    }

    public waitForSlot(): Promise<void> {
        return new Promise((resolve) => {
            const checkSlot = () => {
                if (this.canAddCommand()) {
                    resolve();
                } else {
                    setTimeout(checkSlot, 10);
                }
            };
            checkSlot();
        });
    }
}
