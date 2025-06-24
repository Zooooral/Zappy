import { AIPlayer } from "./core/AiPlayer";
import { AIConfig } from "./core/types";
import { logger } from "./logger";

interface ParseResult {
  success: boolean;
  config?: AIConfig;
  showHelp?: boolean;
}

class ZappyAI {
  private static readonly DEFAULT_CONFIG: Partial<AIConfig> = {
    host: "localhost",
    maxRetries: 10,
    retryDelay: 5000,
  } as const;

  private static readonly VALID_FLAGS = ["-p", "-n", "-h", "help"] as const;

  private aiPlayer: AIPlayer | null = null;

  public async run(): Promise<void> {
    const args = process.argv.slice(2);
    const parseResult = this.parseArguments(args);

    if (!parseResult.success || parseResult.showHelp) {
      this.printUsage();
      process.exit(parseResult.showHelp ? 0 : 1);
    }

    if (!parseResult.config) {
      logger.error("Failed to parse configuration");
      process.exit(1);
    }

    await this.startAI(parseResult.config);
    this.setupGracefulShutdown();
  }

  private parseArguments(args: string[]): ParseResult {
    if (args.length === 0 || args.includes("help")) {
      return { success: false, showHelp: true };
    }

    const config: Partial<AIConfig> = { ...ZappyAI.DEFAULT_CONFIG };

    for (let i = 0; i < args.length; i += 2) {
      const flag = args[i];
      const value = args[i + 1];

      if (!this.isValidFlag(flag)) {
        logger.warn(`Unknown flag: ${flag}`);
        continue;
      }

      if (!value) {
        logger.error(`Missing value for flag: ${flag}`);
        return { success: false };
      }

      const parseSuccess = this.parseFlag(config, flag, value);
      if (!parseSuccess) {
        return { success: false };
      }
    }

    if (!this.isValidConfig(config)) {
      logger.error("Missing required configuration: port and team name are required");
      return { success: false };
    }

    return { success: true, config: config as AIConfig };
  }

  private isValidFlag(flag: string): boolean {
    return ZappyAI.VALID_FLAGS.includes(flag as any);
  }

  private parseFlag(config: Partial<AIConfig>, flag: string, value: string): boolean {
    switch (flag) {
      case "-p":
        const port = parseInt(value, 10);
        if (isNaN(port) || port <= 0 || port > 65535) {
          logger.error(`Invalid port number: ${value}. Must be between 1 and 65535`);
          return false;
        }
        config.port = port;
        break;

      case "-n":
        if (!value.trim()) {
          logger.error("Team name cannot be empty");
          return false;
        }
        config.teamName = value.trim();
        break;

      case "-h":
        if (!value.trim()) {
          logger.error("Host cannot be empty");
          return false;
        }
        config.host = value.trim();
        break;

      default:
        return false;
    }

    return true;
  }

  private isValidConfig(config: Partial<AIConfig>): config is AIConfig {
    return !!(config.port && config.teamName);
  }

  private async startAI(config: AIConfig): Promise<void> {
    logger.info("Starting Zappy AI...");
    logger.info(`Configuration: ${JSON.stringify(config, null, 2)}`);

    this.aiPlayer = new AIPlayer(config);

    try {
      await this.aiPlayer.start();
    } catch (error) {
      logger.error("AI failed to start:", error);
      throw error;
    }
  }

  private setupGracefulShutdown(): void {
    const shutdownHandler = async (signal: string) => {
      logger.info(`Received ${signal}, shutting down AI ...`);
      if (this.aiPlayer) {
        try {
          await this.aiPlayer.stop();
          logger.info("AI stopped successfully");
        } catch (error) {
          logger.error("Error during AI shutdown:", error);
        }
      }

      process.exit(0);
    };

    process.on("SIGINT", () => shutdownHandler("SIGINT"));
    process.on("SIGTERM", () => shutdownHandler("SIGTERM"));
  }

  private printUsage(): void {
    console.log("USAGE: ./zappy_ai -p port -n name -h machine");
    console.log("");
    console.log("Options:");
    console.log("  -p port        Port number (required)");
    console.log("  -n name        Name of the team (required)");
    console.log("  -h machine     Name of the machine (default: localhost)");
    console.log("  help           Show this help message");
    console.log("");
    console.log("Example:");
    console.log("  ./zappy_ai -p 4242 -n myteam -h localhost");
  }
}

async function main(): Promise<void> {
  const zappyAI = new ZappyAI();
  await zappyAI.run();
}

main().catch((error) => {
  logger.error("Unhandled error in main:", error);
  process.exit(1);
});
