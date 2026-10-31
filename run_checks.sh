#!/bin/bash

# Цвета для вывода
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Параметры тестов: ./codexion coders burnout compile debug refactor required cooldown policy
TESTS=(
    "4 800 200 200 200 5 100 fifo"
    "2 400 150 150 150 3 50 edf"
    "1 800 200 200 200 5 100 fifo"   # Граничный случай с 1 кодером
    "5 500 100 100 100 2 50 edf"     # Интенсивный режим
)

echo "=== Сборка проекта с флагом -g ==="
make re CFLAGS="-Wall -Wextra -Werror -g3"

if [ $? -ne 0 ]; then
    echo -e "${RED}Ошибка компиляции!${NC}"
    exit 1
fi

echo -e "\n${GREEN}=== ЗАПУСК VALGRIND (Memory Leaks) ===${NC}"
for i in "${!TESTS[@]}"; do
    echo "Тест $((i+1)): ./codexion ${TESTS[$i]}"
    valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=42 ./codexion ${TESTS[$i]} > /dev/null 2>&1
    RET=$?
    if [ $RET -eq 42 ] || [ $RET -ne 0 ]; then
        echo -e "${RED}[FAIL] Обнаружены утечки памяти или ошибки в Тесте $((i+1))${NC}"
        valgrind --leak-check=full --show-leak-kinds=all ./codexion ${TESTS[$i]}
        exit 1
    else
        echo -e "${GREEN}[PASS] Утечек нет${NC}"
    fi
done

echo -e "\n${GREEN}=== ЗАПУСК HELGRIND (Data Races) ===${NC}"
for i in "${!TESTS[@]}"; do
    echo "Тест $((i+1)): ./codexion ${TESTS[$i]}"
    valgrind --tool=helgrind --error-exitcode=42 ./codexion ${TESTS[$i]} > /dev/null 2>&1
    RET=$?
    if [ $RET -eq 42 ]; then
        echo -e "${RED}[FAIL] Обнаружены гонки данных в Тесте $((i+1))${NC}"
        exit 1
    else
        echo -e "${GREEN}[PASS] Потоки безопасны${NC}"
    fi
done

echo -e "\n${GREEN}Все тесты успешно пройдены! Проект готов к эвалюации.${NC}"