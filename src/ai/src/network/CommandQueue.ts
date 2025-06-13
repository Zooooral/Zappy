import { logger } from "../logger";

export interface PendingCommand {
    command: string;
    resolve: (value: any) => void;
    reject: (error: Error) => void;
    timeout: NodeJS.Timeout;
    timestamp: number;
}

export class CommandQueue {
    private pendingCommands: PendingCommand[] = [];
    private readonly maxPendingCommands: number = 10;

    constructor() {
        setInterval(() => this.cleanupExpiredCommands(), 1000);
    }

    public canAddCommand(): boolean {
        return this.pendingCommands.length < this.maxPendingCommands;
    }

    public addCommand(
        command: string,
        resolve: (value: any) => void,
        reject: (error: Error) => void,
        timeoutMs: number = 15000
    ): void {
        const timeout = setTimeout(() => {
            this.removeCommand(command);
            reject(new Error(`Command timeout: ${command}`));
        }, timeoutMs);

        const pendingCommand: PendingCommand = {
            command,
            resolve,
            reject,
            timeout,
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
            clearTimeout(command.timeout);

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
            clearTimeout(command.timeout);

            logger.error(
                `[COMMAND FAILED]: ${command.command} -> ${error.message}`
            );
            command.reject(error);
        }
    }

    public rejectAll(reason: string): void {
        while (this.pendingCommands.length > 0) {
            const command = this.pendingCommands.shift()!;
            clearTimeout(command.timeout);
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

    private removeCommand(commandToRemove: string): void {
        const index = this.pendingCommands.findIndex(
            cmd => cmd.command === commandToRemove
        );
        if (index !== -1) {
            const command = this.pendingCommands.splice(index, 1)[0];
            clearTimeout(command.timeout);
        }
    }

    private cleanupExpiredCommands(): void {
        const now = Date.now();
        const expired = this.pendingCommands.filter(
            cmd => now - cmd.timestamp > 30000
        );

        for (const cmd of expired) {
            const index = this.pendingCommands.indexOf(cmd);
            if (index !== -1) {
                this.pendingCommands.splice(index, 1);
                clearTimeout(cmd.timeout);
                cmd.reject(new Error("Command expired"));
                logger.warn(`[EXPIRED]: ${cmd.command}`);
            }
        }
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