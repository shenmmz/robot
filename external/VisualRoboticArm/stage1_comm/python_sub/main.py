from __future__ import annotations

import argparse

from .client import ClientConfig, Stage1Client


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Stage1 Python communication client")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", default=5001, type=int)
    parser.add_argument("--name", default="stage1-python")
    parser.add_argument("--reconnect-delay", default=1.0, type=float)
    parser.add_argument("--heartbeat-interval", default=1.0, type=float)
    parser.add_argument("--data-interval", default=2.0, type=float)
    return parser.parse_args()


def main():
    args = parse_args()
    client = Stage1Client(
        ClientConfig(
            host=args.host,
            port=args.port,
            name=args.name,
            reconnect_delay=args.reconnect_delay,
            heartbeat_interval=args.heartbeat_interval,
            data_interval=args.data_interval,
        )
    )
    client.run()


if __name__ == "__main__":
    main()
