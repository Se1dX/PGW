#!/bin/bash
# Скрипт для запуска клиента PGW
# Использование: ./run_client.sh <IMSI>

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
BUILD_DIR="$SCRIPT_DIR/out/build/GCC 13.3.0 x86_64-linux-gnu"
CONFIG_FILE="$SCRIPT_DIR/config/client_config.json"


if [ $# -eq 0 ]; then
    echo "Использование: $0 <IMSI>"
    exit 1
fi

if [ ! -f "$BUILD_DIR/client/pgw_client" ]; then
    echo "Ошибка: клиентский бинарник не найден. Соберите проект сначала."
    exit 1
fi

if [ ! -f "$CONFIG_FILE" ]; then
    echo "Ошибка: конфигурационный файл клиента не найден: $CONFIG_FILE"
    exit 1
fi

"$BUILD_DIR/client/pgw_client" "$CONFIG_FILE" "$1"