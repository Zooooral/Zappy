import * as net from 'net';
import { logger } from '../logger';

export class NetworkClient {
  private socket: net.Socket;
  private host: string;
  private port: number;
  private connected: boolean = false;
   
  constructor(host: string, port: number) {
    this.host = host;
    this.port = port;
    this.socket = net.createConnection({ host: this.host, port: this.port }, () => {
      this.connected = true;
      logger.info(`Connected to server at ${this.host}:${this.port}`);
    });


  }
}
