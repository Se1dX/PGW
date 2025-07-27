#!/bin/bash
# Скрипт нагрузочного тестирования системы 
# Генерирует N параллельных запросов к серверу

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
RUN_CLIENT="$SCRIPT_DIR/run_client.sh"
CONCURRENCY=200  # количество параллельных запросов
TOTAL_REQUESTS=2000  # общее количество запросов
BLACKLIST_RATIO=20 # процент запросов из черного списка (целое число 0-100)

# Генератор IMSI
generate_imsi() {
    prefix="001010"
    num=$(printf "%010d" $1)
    echo "${prefix}${num}"
}

# запуск группы клиентов
run_test_group() {
    for i in $(seq 1 $1); do
        # случайно выбираем, будет ли IMSI в черном списке
        if (( RANDOM % 100 < BLACKLIST_RATIO )); then
            imsi="001010123456789"  # IMSI из черного списка
        else
            imsi=$(generate_imsi $((RANDOM % 1000000)))
        fi
        
        "$RUN_CLIENT" "$imsi" > /dev/null &
    done
    wait
}

echo "Начало нагрузочного тестирования"
echo "Параметры:"
echo "  Параллельных запросов: $CONCURRENCY"
echo "  Всего запросов: $TOTAL_REQUESTS"
echo "  Процент черного списка: ${BLACKLIST_RATIO}%"

# основной цикл тестирования
groups=$((TOTAL_REQUESTS / CONCURRENCY))
for group in $(seq 1 $groups); do
    echo "Запуск группы $group/$groups ($CONCURRENCY запросов)..."
    run_test_group $CONCURRENCY
    sleep 0.5  
done

echo "Нагрузочное тестирование завершено"
echo "Проверьте логи сервера и CDR для анализа производительности"
