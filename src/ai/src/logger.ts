import pino, { Logger } from "pino";
import * as fs from "fs";
import * as path from "path";

const logDir = path.resolve(__dirname, "../log");
if (!fs.existsSync(logDir)) {
    fs.mkdirSync(logDir, { recursive: true });
}

function getLogFileName() {
    const now = new Date();
    const pad = (n: number) => n.toString().padStart(2, "0");
    const timestamp = `${now.getFullYear()}${pad(now.getMonth() + 1)}${pad(
        now.getDate()
    )}_${pad(now.getHours())}${pad(now.getMinutes())}${pad(now.getSeconds())}`;
    return `app_${timestamp}.log`;
}

const logFilePath = path.join(logDir, getLogFileName());

let logger: Logger;

const logMode =
    process.argv.find((arg) => arg.startsWith("--log-mode="))?.split("=")[1] ||
    "console";

if (logMode === "none") {
    logger = pino({ level: "silent" });
} else if (logMode === "file") {
    const stream = fs.createWriteStream(logFilePath, { flags: "a" });
    logger = pino(
        {
            level: "info",
            timestamp: pino.stdTimeFunctions.isoTime,
        },
        stream
    );
} else if (logMode === "both") {
    const stream = fs.createWriteStream(logFilePath, { flags: "a" });

    const consoleLogger = pino({
        level: "info",
        transport: {
            target: "pino-pretty",
            options: {
                colorize: true,
                translateTime: "HH:MM:ss",
                ignore: "pid,hostname",
            },
        },
    });

    const fileLogger = pino(
        {
            level: "info",
            timestamp: pino.stdTimeFunctions.isoTime,
        },
        stream
    );

    logger = {
        trace: (msg: any, ...args: any[]) => {
            consoleLogger.trace(msg, ...args);
            fileLogger.trace(msg, ...args);
        },
        debug: (msg: any, ...args: any[]) => {
            consoleLogger.debug(msg, ...args);
            fileLogger.debug(msg, ...args);
        },
        info: (msg: any, ...args: any[]) => {
            consoleLogger.info(msg, ...args);
            fileLogger.info(msg, ...args);
        },
        warn: (msg: any, ...args: any[]) => {
            consoleLogger.warn(msg, ...args);
            fileLogger.warn(msg, ...args);
        },
        error: (msg: any, ...args: any[]) => {
            consoleLogger.error(msg, ...args);
            fileLogger.error(msg, ...args);
        },
        fatal: (msg: any, ...args: any[]) => {
            consoleLogger.fatal(msg, ...args);
            fileLogger.fatal(msg, ...args);
        },
        child: (bindings: any) => {
            return {
                ...logger,
                trace: (msg: any, ...args: any[]) => {
                    consoleLogger.child(bindings).trace(msg, ...args);
                    fileLogger.child(bindings).trace(msg, ...args);
                },
                debug: (msg: any, ...args: any[]) => {
                    consoleLogger.child(bindings).debug(msg, ...args);
                    fileLogger.child(bindings).debug(msg, ...args);
                },
                info: (msg: any, ...args: any[]) => {
                    consoleLogger.child(bindings).info(msg, ...args);
                    fileLogger.child(bindings).info(msg, ...args);
                },
                warn: (msg: any, ...args: any[]) => {
                    consoleLogger.child(bindings).warn(msg, ...args);
                    fileLogger.child(bindings).warn(msg, ...args);
                },
                error: (msg: any, ...args: any[]) => {
                    consoleLogger.child(bindings).error(msg, ...args);
                    fileLogger.child(bindings).error(msg, ...args);
                },
                fatal: (msg: any, ...args: any[]) => {
                    consoleLogger.child(bindings).fatal(msg, ...args);
                    fileLogger.child(bindings).fatal(msg, ...args);
                },
            };
        },
    } as unknown as Logger;
} else {
    logger = pino({
        level: "info",
        transport: {
            target: "pino-pretty",
            options: {
                colorize: true,
                translateTime: "HH:MM:ss",
                ignore: "pid,hostname",
            },
        },
    });
}

export { logger };
