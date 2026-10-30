# Prime Ratio Codec

Небольшой экспериментальный кодек на **C++20**: пробует представить заданную **битовую последовательность** первыми `n` битами после двоичной точки отношения **двух простых чисел** `p / q` (где `p < q`). Для восстановления достаточно `p`, `q` и `n`.

> **Эксперимент, не универсальное сжатие.** Для произвольного файла пара простых может не существовать в поддерживаемом диапазоне. В таком случае `encode` завершится с ошибкой, не выдавая поддельный результат. Длинные файлы обычно не закодируются. Это также не криптография.

## Структура проекта

```text
prime-ratio-codec/
├── src/
│   ├── main.cpp
│   ├── Codec.cpp
│   ├── Codec.h
│   ├── Generator32.h
│   ├── PrimeGenerator.h
│   ├── Number.h
│   ├── Number32.h
│   └── Sequence.h
├── tests/
│   └── smoke.cpp
├── examples/
│   └── sample.txt
├── CMakeLists.txt
├── .gitignore
├── primecodec.exe        # Windows-сборка (не отслеживается Git)
└── README.md
```

Все исходники перенесены в `src/`. Тесты и примеры остаются отдельно.

## Сборка

Нужны CMake >= 3.20 и компилятор C++20 (MSVC, GCC или Clang). В CLion откройте папку репозитория как CMake-проект.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Исполняемый файл: **`./primecodec`** (Linux/macOS) или **`./primecodec.exe`** (Windows) прямо в корне проекта. Папка `build/` содержит только промежуточные файлы сборки и тесты. Сам `.exe` и `build/` игнорируются Git. При сборке Debug и Release исполняемый файл в корне перезаписывается.

## Примеры

Закодировать строку `A` (один байт `01000001`):

```bash
primecodec encode --string "A"
```

Один из допустимых результатов (в текущем генераторе):

```text
11 43 8
```

Это `p q n`. Проверка: первые 8 бит после двоичной точки у `11/43` — `01000001`.

Закодировать тестовый файл `examples/sample.txt` (ровно **один байт `A`**, без перевода строки):

```bash
primecodec encode --file examples/sample.txt
```

Декодировать обратно в файл:

```bash
primecodec decode 11 43 8 --file restored.txt
```

Проверить идентичность (Linux/macOS):

```bash
cmp examples/sample.txt restored.txt
```

Проверить идентичность в Windows PowerShell:

```powershell
(Get-FileHash examples/sample.txt).Hash -eq (Get-FileHash restored.txt).Hash
```

Декодировать в стандартный вывод (подходит для строк; выводятся **сырые байты**):

```bash
primecodec decode 11 43 8 --string
```

Другой пример: `2/3` начинается в двоичной системе с `0.10101010...`; поэтому:

```bash
primecodec decode 2 3 8 --file alternating.bin
```

Создаст один байт `AA` (hex). Это может быть непечатный символ при выводе как текст.

> **Windows / CLion:** аргументы запуска можно задать в **Run → Edit Configurations → Program arguments**. Для примера: `encode --file examples/sample.txt`. Рабочая директория должна быть корнем проекта.

## Интерфейс

```cpp
std::optional<std::pair<uint32_t, uint32_t>>
encode(const std::vector<uint32_t>& data, size_t n);

std::vector<uint32_t>
decode(uint32_t p, uint32_t q, size_t n);
```

- `encode` перебирает два простых из `Generator32`, пока первые `n` битов отношения не совпадут с `data`; возвращает пару или `std::nullopt`.
- `decode` извлекает первые `n` битов `p/q` в `uint32_t`-блоки.
- В каждом **полном** блоке хранится 32 бита, старший бит идёт первым. **Неполный последний** блок (до 31 бита) хранится **в младших битах** (выравнивание вправо).
- `n` — количество **бит**, не байтов; `src/main.cpp` считывает байты и использует `n = 8 * size`.

### Ограничения текущего прототипа

- `Generator32` сейчас содержит `LIMIT = 100000` бит, каждый индекс `i` соответствует нечётному числу `2*i+1`; генерация ограничена простыми до **199999**, а **не** всем диапазоном `uint32_t`.
- У генератора два монотонных указателя, общее лениво расширяемое решето, но подходящая пара **не гарантирована**.
- Использование отношения простых вместо содержимого файла в общем случае не уменьшает информационную сложность. `p q n` могут описывать лишь ограниченное множество битовых строк.
- `decode` получает `p`, `q` как `uint32_t`; корректность того, что они простые, не проверяет.
- Если нужны все 32-битные простые, потребуется отдельно расширить/оптимизировать `Generator32`; для 64-битной или длинной арифметики понадобится новый генератор.

## Структура

```text
.
├── CMakeLists.txt
├── .gitignore
├── README.md
├── src/
│   ├── main.cpp            # CLI: строка/файл → биты и обратно
│   ├── Codec.h / Codec.cpp # чистые функции encode / decode
│   ├── Sequence.h          # сравнение битовых префиксов отношения
│   ├── Generator32.h       # генератор с двумя указателями
│   ├── PrimeGenerator.h    # абстрактный интерфейс
│   └── Number.h / Number32.h
├── examples/
│   └── sample.txt       # 1 байт: 'A'
└── tests/
    └── smoke.cpp        # простой тест кодирования и декодирования
```

## Тесты

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

## Подготовка к публикации на GitHub

Репозиторий уже содержит начальный коммит. Для публикации в **созданный вами пустой** удалённый репозиторий:

```bash
git remote add origin git@github.com:YOUR_ACCOUNT/prime-ratio-codec.git
git branch -M main
git push -u origin main
```

Подставьте свой адрес репозитория вместо `YOUR_ACCOUNT`.
