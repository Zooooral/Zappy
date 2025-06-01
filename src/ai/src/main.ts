import { logger } from "./logger";

if (process.argv.includes("--help") || process.argv.includes("-h")) {
  console.log("USAGE: ./zappy_gui -p port -h machine");
  process.exit(0);
}

logger.info("Starting application...");

