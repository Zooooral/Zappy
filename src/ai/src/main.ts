import { AIPlayer } from "./core/AiPlayer";
import { logger } from "./logger";

async function main() {
  const args = process.argv.slice(2);
  const config = parseArguments(args);

  if (!config) {
    printUsage();
    process.exit(1);
  }

  logger.info("Starting Zappy AI...");
  logger.info(`Config: ${JSON.stringify(config)}`);

  const ai = new AIPlayer(config);

  try {
    await ai.start();
  } catch (error) {
    logger.error("AI failed to start:", error);
    process.exit(1);
  }

  // vu qu'on est des bg
  process.on("SIGINT", async () => {
    logger.info("Shutting down AI...");
    await ai.stop();
    process.exit(0);
  });
}

function parseArguments(args: string[]): AIConfig | null {
  const config: Partial<AIConfig> = {
    host: "localhost",
    maxRetries: 10,
    retryDelay: 5000,
  };

  for (let i = 0; i < args.length; i += 2) {
    const flag = args[i];
    const value = args[i + 1];

    switch (flag) {
      case "-p":
        config.port = parseInt(value);
        break;
      case "-n":
        config.teamName = value;
        break;
      case "-h":
        config.host = value;
        break;
      case "help":
        return null;
      default:
        logger.warn(`Unknown flag: ${flag}`);
    }
  }

  if (!config.port || !config.teamName) {
    return null;
  }

  return config as AIConfig;
}

function printUsage() {
  console.log("USAGE: ./zappy_ai -p port -n name -h machine");
  console.log("option description");
  console.log("-p port        port number");
  console.log("-n name        name of the team");
  console.log("-h machine     name of the machine; localhost by default");
}

main().catch((error) => {
  logger.error("Unhandled error:", error);
  process.exit(1);
});
