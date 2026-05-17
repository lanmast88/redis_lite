# Redis Lite

Облегчённый аналог Redis — сетевой In-Memory Key-Value сервер с поддержкой TTL и транзакций, реализованный на C++17.

## Поддерживаемые команды

| Команда | Описание |
|---------|----------|
| `SET key value [EX seconds]` | Установить значение ключа, опционально с TTL |
| `GET key` | Получить значение ключа |
| `DEL key [key ...]` | Удалить один или несколько ключей |
| `EXISTS key` | Проверить существование ключа |
| `KEYS` | Вернуть все активные ключи |
| `EXPIRE key seconds` | Установить TTL для существующего ключа |
| `BEGIN` | Начать транзакцию |
| `COMMIT` | Применить транзакцию |
| `ROLLBACK` | Отменить транзакцию |
| `PING` | Проверить соединение |
| `QUIT` | Завершить соединение |

## Архитектура

```
include/
├── server.h          — TCP-сервер, принимает подключения
├── client_handler.h  — обработчик клиентского соединения
├── storage.h         — потокобезопасное хранилище ключей
├── transaction.h     — буферизация команд в транзакции
└── command_parser.h  — разбор текстовых команд

src/
├── main.cpp
├── server.cpp
├── client_handler.cpp
├── storage.cpp
├── transaction.cpp
└── command_parser.cpp
```

## Сборка

```bash
cmake -S . -B build
cmake --build build
```

## Запуск

```bash
cd build
./redis_lite          # порт по умолчанию 6379
./redis_lite 7000     # кастомный порт
```

## Пример работы через netcat

```bash
nc localhost 6379

SET name Alice
+OK
GET name
$5
Alice
SET counter 1 EX 10
+OK
EXISTS counter
:1
BEGIN
+OK
SET x 100
+QUEUED
SET y 200
+QUEUED
COMMIT
*2
+OK
+OK
GET x
$3
100
KEYS
*3
$4
name
$7
counter
$1
x
```

## Сторонние библиотеки

- **Threads (pthread)** — стандартная POSIX-библиотека потоков, подключается через `find_package(Threads REQUIRED)` в CMake.
