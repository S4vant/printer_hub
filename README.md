# print-agent

Агент сбора статистики печати для Linux-серверов с CUPS.

Программа получает информацию о заданиях печати напрямую из `systemd-journal`, извлекает необходимые данные о заданиях CUPS, сохраняет их в формате JSON и может отправлять на центральный сервер по HTTP.

---

## Назначение

Проект является частью системы учёта печати и предназначен для автоматизированного сбора информации о печатных заданиях на серверах Linux.

Источником данных выступает `systemd-journal`, что позволяет отказаться от анализа лог-файлов CUPS и получать информацию непосредственно из журнала системы.

---

## Возможности

Текущая версия поддерживает:

- чтение сообщений `cupsd` из `systemd-journal`;
- парсинг событий печати CUPS;
- выделение отдельных заданий печати;
- формирование структуры задания печати (`PrintJob`);
- сохранение данных в JSON;
- инкрементальное обновление JSON-файла;
- отправку данных на сервер по HTTP POST;
- настройку через `.env`;
- несколько режимов работы через аргументы командной строки.

---

## Извлекаемые данные

Для каждого задания печати сохраняются:

| Поле | Описание |
|--------|----------|
| job_id | Идентификатор задания |
| printer | Имя принтера |
| user | Пользователь |
| file_name | Имя документа |
| copies | Количество копий |
| created_at | Время создания задания (Unix Timestamp) |

---

## Структура проекта

```text
src
├── agent
│   ├── Agent.cpp
│   └── Agent.h
│
├── cli
│   ├── CommandLine.cpp
│   └── CommandLine.h
│
├── config
│   ├── Config.cpp
│   └── Config.h
│
├── output
│   ├── JsonWriter.cpp
│   └── JsonWriter.h
│
├── parser
│   ├── JournalReader.cpp
│   ├── JournalReader.h
│   ├── JobParser.cpp
│   └── JobParser.h
│
├── transport
│   ├── HttpSender.cpp
│   └── HttpSender.h
│
├── model
│   └── PrintJob.h
│
└── main.cpp
```

---

## Конфигурация

Настройки задаются через файл `.env`.

Пример:

```env
SERVER_URL=http://192.168.1.10:8000/api/printers/report/
ZABBIX_HOST=192.168.1.10
ZABBIX_PORT=10051

ZABBIX_ITEM_HOST=printer-server
ZABBIX_ITEM_KEY=printer.jobs
```

---

## Формат данных

Пример содержимого файла `jobs.json`:

```json
[
    {
        "job_id": 35,
        "printer": "HP-LaserJet",
        "user": "admin",
        "file_name": "report.pdf",
        "copies": 1,
        "created_at": 1778848593
    }
]
```

---

## Режимы работы

### Полная перестройка базы

Создаёт новый `jobs.json` на основе всех найденных заданий.

```bash
print-agent rebuild
```

---

### Инкрементальное обновление

Добавляет в `jobs.json` только новые задания.

```bash
print-agent update
```

---

### Отправка данных

Отправляет содержимое JSON-файла на сервер.

```bash
print-agent send
```

---

### Полная синхронизация

Выполняет:

```text
update
↓
send
```

Запускается командой:

```bash
print-agent sync
```

или просто:

```bash
print-agent
```

---

## Сборка

### Debian / Ubuntu

Установка зависимостей:

```bash
sudo apt install \
    build-essential \
    cmake \
    pkg-config \
    libsystemd-dev \
    libcurl4-openssl-dev \
    nlohmann-json3-dev
```

---

Сборка проекта:

```bash
mkdir build
cd build

cmake ..
make
```

---

## Используемые библиотеки

- libsystemd
- libcurl
- nlohmann/json

---

## Планируемое развитие

Планируется реализовать:

- хранение состояния последней синхронизации;
- отправку только новых заданий;
- поддержку очереди неотправленных данных;
- настройку сохраняемых полей через `.env`;
- интеграцию с Zabbix;
- работу в режиме системного сервиса (`systemd service`);
- централизованную систему учёта принтеров и печати.

## как вариант разрешения проблем с nlohmann:
mkdir -p third_party/nlohmann
curl -L https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp \
-o third_party/nlohmann/json.hpp
Тогда в CMakeLists.txt можно убрать:

find_package(nlohmann_json REQUIRED)

и вместо этого добавить:

target_include_directories(
    print-agent PRIVATE
    src
    third_party
    ${SYSTEMD_INCLUDE_DIRS}
)