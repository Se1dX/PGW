#!/bin/bash
# Скрипт для запуска сервера PGW
# Использование: ./run_server.sh

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
BUILD_DIR="$SCRIPT_DIR/out/build/GCC 13.3.0 x86_64-linux-gnu"
"$BUILD_DIR/server/pgw_server" "$SCRIPT_DIR/config/server_config.json"