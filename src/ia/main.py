##
## EPITECH PROJECT, 2025
## src/ai/main.py
## File description:
## main.py
##

import sys
import argparse
import socket

def print_help():
    print("USAGE: ./zappy_ai -p port -n name --host machine")
    print("  -p port       : port number")
    print("  -n name       : name of the team")
    print("  -h machine: name of the machine; localhost by default")

def parse_arguments():
    parser = argparse.ArgumentParser(description='Zappy AI client', add_help=False)
    parser.add_argument('-p', '--port', type=int, default=4242,
                        help='port number')
    parser.add_argument('-n', '--name', type=str, required=True,
                        help='name of the team')
    parser.add_argument('-h', '--host', type=str, default='localhost',
                        help='name of the machine; localhost by default')
    parser.add_argument('--help', action='store_true',
                        help='show this help message and exit')

    return parser.parse_args()

def main():
    if len(sys.argv) == 1 or (len(sys.argv) == 2 and (sys.argv[1] == "help" or sys.argv[1] == "--help")):
        print_help()
        return 0

    try:
        args = parse_arguments()
        if args.help:
            print_help()
            return 0
    except Exception as e:
        print(f"Error parsing arguments: {e}", file=sys.stderr)
        print_help()
        return 84
    return 0

if __name__ == "__main__":
    sys.exit(main())
