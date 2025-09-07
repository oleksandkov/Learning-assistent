## C++ <chrono> and <thread> Libraries Usage Guide

### <chrono> Library

The `<chrono>` library provides facilities for manipulating dates, times, and durations in C++.

#### Main Types and Functions
- `std::chrono::duration` — Represents a time duration (e.g., seconds, milliseconds).
- `std::chrono::time_point` — Represents a point in time.
- `std::chrono::system_clock` — System-wide real time wall clock.
- `std::chrono::steady_clock` — Monotonic clock, never goes backward.
- `std::chrono::high_resolution_clock` — Highest precision clock available.
- `std::chrono::seconds`, `std::chrono::milliseconds`, `std::chrono::microseconds`, etc. — Predefined duration types.
- `std::chrono::now()` — Gets the current time from a clock (see below for usage).
- `std::chrono::duration_cast` — Converts between duration types.
- `start`, `end` — Common variable names for storing time points (see example below).

##### now(), start, end
- `now()` is a static method of a clock (e.g., `std::chrono::high_resolution_clock::now()`) that returns the current time as a `time_point`.
- `start` and `end` are typical variable names used to store the time before and after an operation, so you can measure elapsed time.

##### Український переклад
- `std::chrono::duration` — представляє тривалість часу (наприклад, секунди, мілісекунди).
- `std::chrono::time_point` — представляє певний момент часу.
- `std::chrono::system_clock` — системний годинник реального часу.
- `std::chrono::steady_clock` — монотонний годинник, не повертається назад.
- `std::chrono::high_resolution_clock` — годинник з найвищою доступною точністю.
- `std::chrono::seconds`, `std::chrono::milliseconds`, `std::chrono::microseconds` тощо — заздалегідь визначені типи тривалості.
- `std::chrono::now()` — отримує поточний час з годинника (див. приклад нижче).
- `std::chrono::duration_cast` — перетворює між різними типами тривалості.
- `start`, `end` — типові імена змінних для збереження моментів часу до і після операції (див. приклад нижче).


#### Examples / Приклади
```cpp
#include <chrono>
#include <iostream>
using namespace std;

// Get current time / Отримати поточний час
auto start = std::chrono::high_resolution_clock::now();

// Do some work... / Виконати якусь роботу...

auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
cout << "Elapsed time: " << duration.count() << " ms" << endl; // Вивести час виконання

// Using durations / Використання тривалостей
std::chrono::seconds sec(2); // 2 seconds / 2 секунди
std::chrono::milliseconds ms(1500); // 1500 milliseconds / 1500 мілісекунд
```

### <thread> Library

The `<thread>` library provides facilities for managing threads and sleeping.

#### Main Types and Functions
- `std::thread` — Represents a thread of execution.
- `std::this_thread::sleep_for(duration)` — Sleep for a specified duration.
- `std::this_thread::sleep_until(time_point)` — Sleep until a specific time point.
- `std::this_thread::get_id()` — Get the current thread's ID.

#### Examples
```cpp
#include <thread>
#include <chrono>
#include <iostream>
using namespace std;

// Sleep for 1 second
std::this_thread::sleep_for(std::chrono::seconds(1));

// Sleep for 500 milliseconds
std::this_thread::sleep_for(std::chrono::milliseconds(500));

// Launch a thread
void worker() {
	cout << "Worker thread running..." << endl;
}
std::thread t(worker);
t.join(); // Wait for thread to finish
```

### Important Aspects
- `<chrono>` and `<thread>` are part of the C++11 standard and later.
- Use `std::chrono::duration_cast` to convert between different duration types.
- Always join threads (`t.join()`) to avoid program termination before threads finish.
- For high-precision timing, prefer `high_resolution_clock` or `steady_clock`.
- `sleep_for` and `sleep_until` are useful for delays and timers.

---
This guide covers the most common functions and usage patterns for `<chrono>` and `<thread>` in C++.
