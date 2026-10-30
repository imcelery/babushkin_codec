# Babushkin Codec

Экспериментальный кодек на C++20: ищет два простых числа `p < q`, у которых первые `n` бит дроби `p / q` совпадают с входными данными. Подходящая пара находится не для каждого файла.

## Структура

```text
babushkin_codec/
├── src/           # Исходники: main, Codec, Sequence, Generator32, Number
├── tests/         # Тесты
├── examples/      # Примеры входных файлов
├── CMakeLists.txt
├── .gitignore
└── README.md
```

`build/` и скомпилированные файлы не отслеживаются Git. После сборки `primecodec.exe` появляется в корне проекта.

## Сборка

Из корня проекта (PowerShell):

```powershell
cmake -S . -B build
cmake --build build --config Release
```

Если команда `cmake` недоступна, открой проект через **CLion** и нажми **Build**, либо добавь CMake в `PATH`.

## Примеры

```powershell
.\primecodec.exe encode --string "A"
# 11 43 8 (p q n)

.\primecodec.exe encode --file examples/sample.txt
.\primecodec.exe decode 11 43 8 --file restored.txt
.\primecodec.exe decode 11 43 8 --string
```

`n` — число бит исходных данных. `examples/sample.txt` содержит один символ `A` без перевода строки.
