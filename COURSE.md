# Professional ESP32 Academy

## Introduction

- The ESP32 is a family of **SoC** _(System on a chip)_ developed by Espressif Systems.
A SoC is defined as an integrated circuit that combines multiple cores, such as CPUs, GPUs,
and other functional units, along with memory, I/O ports, and sometimes modems on a single
substrate. This integration has the goal of enhancing performance, reducing power consumption,
and optimizing semiconductor die area compared to traditional motherboard-based architectures.

- 32-bit Architectures: _Xtensa LX6, Xtensa 32-bit LX7, and RISC-V 32-bit_.
- Connectivity: _Wi-Fi, Bluetooth Classic/Low-Energy (LE)/Mesh, IEEE 802.15.4 (Thread, Matter,
and Zigbee)_.


## Documentations

1. [ESP-IDF](https://developer.espressif.com/tags/esp-idf/)
2. [SoCs](https://www.espressif.com/en/products/socs) - Use to look for:
   - Technical Reference Manuals
   - Datasheets
   - Hardware Design Guidelines
3. Other worthwhile readings:
   - [ESP-IDF SDK](https://www.espressif.com/en/products/sdks/esp-idf)
   - [Espressif's Blog](https://developer.espressif.com/blog/)
   - [ESP32 Forums](https://esp32.com/)


## Franzininho WiFi LAB01

#### Resources:

1. MCU ESP32-S2:
   - Xtensa single-core 32-bit LX7, operating up to 240MHz.
   - 128KB ROM, 320KB SRAM, 16KB SRAM in the RTC, 4MB Flash memory.
   - WiFi 802.11 b/g/n.
   - GPIO, SPI, LCD, UART, I2C, I2S, Camera, IR, pulse counter, PWM LED, TWAI(CAN),
   USB 1.1 OTG, ADC, DAC, touch, internal temperature sensor.

2. Pinout:
   - Buses: 40 pins divided into 2x20 buses of 2.54mm.
   - 35 GPIOS.
   - Breadboard compatibility.
   
3. 2 general use-case LEDs.

4. 1 reset and 1 boot buttons.

5. USB: micro USB - OTG 1.1 connector.

6. Power Supply:
   - 5V through USB connection.
   - 5V and GND through bus.
   - 3V3 and GND through bus.

7. Programming: ESP-IDF, Arduino, CircuitPython, MicroPython, Zephyr, NuttX.


## ESP-IDF Basics

- Activate env:

```bash
source "/home/gustavo/.espressif/tools/activate_idf_v6.0.2.sh"
```

- Set this in a `.dir-locals.el` file at the root of an ESP-IDF project:

```elisp
((nil
  . ((eglot-server-programs
      . ((c-ts-mode
          . ("/home/gustavo/.espressif/tools/esp-clang/esp-19.1.2_20250312/esp-clang/bin/clangd"
             "--query-driver=/home/gustavo/.espressif/tools/xtensa-esp-elf/esp-14.2.0_20260121/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc"))
         (c-mode
          . ("/home/gustavo/.espressif/tools/esp-clang/esp-19.1.2_20250312/esp-clang/bin/clangd"
             "--query-driver=/home/gustavo/.espressif/tools/xtensa-esp-elf/esp-14.2.0_20260121/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc"))
         (c++-ts-mode
          . ("/home/gustavo/.espressif/tools/esp-clang/esp-19.1.2_20250312/esp-clang/bin/clangd"
             "--query-driver=/home/gustavo/.espressif/tools/xtensa-esp-elf/esp-14.2.0_20260121/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc"))
         (c++-mode
          . ("/home/gustavo/.espressif/tools/esp-clang/esp-19.1.2_20250312/esp-clang/bin/clangd"
             "--query-driver=/home/gustavo/.espressif/tools/xtensa-esp-elf/esp-14.2.0_20260121/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc"))))))))
```

1. Main CLI Commands

```bash
idf.py create-project <project_name> # generates a basic initial structure in the project_name directory

idf.py set-target <target> # defines the target MCU for which the project will be compiled for
	
idf.py menuconfig # opens an interactive config UI for adjusting compile options and others for the project

idf.py clean # removes the files generated during the previous compilation, project ready for a new build

idf.py fullclean # removes all generated files, including local project configs, directory returns to initial state

idf.py build # compiles the project and generates the binaries (into build/) that will be loaded into the device

idf.py flash # flashed the complete binary to the ESP (will compile the project if needed, then upload to the connected board)
idf.py -p PORT flash # specifies the serial port to flash the binary
idf.py app-flash # uploads only the application's binary, without flashing other memory segments, e.g., the bootloader
idf.py encrypted-flash # flashes the firmware cryptographed, if cryptography is enabled in the project

idf.py monitor # connects to the ESP serial port and shows the debugging logs in real-time

idf.py erase_flash # removes completely the flash memory of the ESP device, useful to clean all data
```

2. Additional Useful Commands

```bash
idf.py size # shows the flash memory usage of the compiled application, including binary sizes and partitions sizes

idf.py size-components # shows a detailed table with memory usage per component

idf.py reconfigure # generates the configurations of the project again without recompiling

idf.py bootloader # compiles only the project's bootloader

idf.py gdb # initializes GDB for project-debugging

idf.py gdbgui # initializes a GUI for debugging with GDB

idf.py -C components create-component <my_component> # creates a new component inside an existing ESP-IDF project, creating the basic components/my_component structure
```

3. Logging

- ESP-IDF provides a set of macros for logging messages throughout the program's execution, through the header file `"esp_log.h"`.

- 5 levels of logging are defined. Below they are listed, in order of increasing verbosity:
	1. ESP_LOGE - Error
	2. ESP_LOGW - Warning
	3. ESP_LOGI - Info
	4. ESP_LOGD - Debug
	5. ESP_LOGV - Verbose
  
> In each C file that needs logging functionalities, define the **TAG** variable as:

```c
static const char TAG[] = "MyModule";
```

> Then use one of logging macros to produce output, e.g.:

```c
ESP_LOGW(TAG, "Baud rate error %.1f%%. Requested: %d baud, actual: %d baud", error * 100, baud_req, baud_real);

ESP_EARLY_LOGW(TAG, "Early log message %d", i++);

ESP_DRAM_LOGE(DRAM_STR("TAG_IN_DRAM"), "DRAM log message %d", i++);
```

> Result:
```bash
I (112500) MyModule: Baud rate error 1.5%. Requested: 115200 baud, actual: 116928 baud
W (112500) MyModule: Early log message 1
E : TAG_IN_DRAM: DRAM log message 2
```

- Enabling higher verbosity logging levels will automatically enable the lower levels. E.g.:
```c
esp_log_level_set(TAG_ ESP_LOG_INFO); // Enables LOGI, but also LOGW and LOGE since they are below INFO in the verbosity levels
```

- Disable all logs:
```c
esp_log_level_set(TAG, ESP_LOG_NONE);
```


## General Purpose Input Output - GPIO in Microcontrollers

- Allows direct, software-controlled, interaction with external devices. GPIO pins can be configured as input to read signals from sensors, buttons, or other peripherals, or as output to control components like LEDs, motors, and displays. Provides a more flexible and simple way to interface with hardware.

- Each GPIO pin can be configured in one of the following modes:
	1. Input Mode: the pin reads signals from external devices, such as sensors or push buttons. The voltage level (high or low) is interpreted as a binary signal (1 or 0).
	2. Output Mode: the pin sends signals to control devices like LEDs, buzzers, or relays. The microcontroller sets the voltage level (high or low) to activate or deactivate the device.

- To ensure a stable signal when a GPIO pin is set as an input but is not actively driven by an external circuit, pull-up and pull-down resistors are used:
	1. Pull-up resistor: keeps the pin at a HIGH state when no signal is applied.
	2. Pull-down resistor: keeps the pin at a LOW state when no signal is applied.

> Pull-up and pull-down resistors are used to correctly bias the inputs of digital gates.
> These resistors prevent floating states when there is no input condition, which can cause unpredictable behavior in digital circuits. Therefore, if the microcontroller don't utilize them through the GPIO pins, and nothing is connected to the pins, the program will read a "floating" impedance state.

> A pull-up resistor is used to establish an additional loop over the critical components while making sure that the voltage is well-defined even when the switch is open. It is used to ensure that a wire is pulled to a high logical level in the absence of an input signal. It is not a special kind of resistor. They are simple fixed-value resistors connected between the coltage supply and the appropriate pin that defines the input or output voltage in the absence of a driving signal.
> When the switch is open, the voltage of the gate input is pulled up to the level of the input voltage.
> When the switch is closed, the input voltage at the gate goes directly to the ground.
> You need to use a pull-up resistor when you have a low default impedance state and wish to pull the signal to `high`.

(TODO) PUT FIGURE HERE

> In the figure above, a pull-up resistor with a fixed value is used to connect the voltage supply and a particular pin in the digital logic circuit. The pull-up resistor is paired with a switch to ensure that the voltage between GND and VCC is actively controlled when the switch is open. At the same time, it will not affect the state of the circuit. If we do not use a pull-up resistor, it will result in a short circuit. This is because the pin cannot be directly shorted to the ground or VCC as this will eventually damage the circuit. Following Ohm's Law principle, if the is a pull-up resistance, a small amount of current will flow from the source to the resistors and to the switch before reaching the ground.

> A pull-down resistor is used to ensure that inputs to logic systems settle at expected logic levels whenever external devices are disconnected or of high impedance. It ensures that the wire is at a defined low logic level even when there are no active connections with other devices. The pull-down resistor holds the logic signal near to zero volts (0V) when no other active device is connected. It pulls the input voltage down to the ground to prevent an undefined state at the input. It should have a larger resistance than the impedance of the logic circuit. Otherwise, it will make the input voltage at the pin on constant logical low value no matter the position of the switch. When the switch is open, the voltage of the gate input is pulled down to the level of the ground. When the switch is closed, the input voltage at the gate goes to Vin. Without the resistor, the voltage leves would virtually float between the two voltages.

(TODO) PUT FIGURE HERE

> In the figure above, the pull-down resistors in the circuit also ensure that the voltage between VCC and a microcontroller pin is actively controlled when the switch is open. Unlike the pull-up resistor, the pull-down resistor pulls the pin to a low value instead of high value. The pull-down resistor which is connected to the ground or 0V sets the digital logic level pin to default or 0 until the switch is pressed and the logical level pin becomes high. Therefore, the small amount of current flows from the 5V source to the ground using the closed switch and pull-down resistor preventing the logic level pin from getting shorted with the 5V source.

---

#### Aside: Ideal Resistance Values for Pull-up and Pull-down Resistors

- When the button is pressed, the input pin is pulled down. The value of the resistor near the supply controls how much current you want to flow from VCC through the button, and then to ground. High current will flow through the pull-up resistor if the resistance value is too low. It may cause unnecessary usage of power even when the switchis closed since the device will heat up. This condition is called a **strong pull-up** and should always be avoided when low power consumption is a requirement.

- When the button is not pressed, the input pint is pulled high. The value of the pull-up resistor controls the voltage on the input pin. When the switch is open and a high pull-up resistance value is combined with a large leakage current from the input pin, the input voltage can become insufficient. This is called having a **weak pull-up**. The pull-up resistor's actual value depends on the impedance of the input pin that is closely related to the pin's leakage current.

> Based on the two conditions above, for **pull-up resistors**, you need to use a resistor that is at least 10 times smaller than the value of the input pin impedance. For logic devices that operate at 5V, the typical pull-up resistor value should be between 1-5 kΩ. On the other hand, for switch and resistive sensor applications, the typical pull-up resistor value should be between 1-10 kΩ.

> For **pull-down resistors**, it should always have a larger resistance than the impedance of the logic circuit. Or else, it will pull the voltage down by too much and the input voltage at the pin would remain at a constant logical low value regardless of whether the switch is on or off.

|--------------------------------------------------------------------------|
| Pull-down Resistors               | Pull-up Resistors                    |
|-----------------------------------|--------------------------------------|
| Less commonly used                | More commonly used                   |
|-----------------------------------|--------------------------------------|
| Keeps the input _"Low"_           | Keeps the input _"High"_             |
|-----------------------------------|--------------------------------------|
| Connect between an I/O pin and    | Connect between I/O pin and          |
| GND, with an open switch          | +supply voltage, with an open        |
| connected between I/O and +supply | switch connected between I/O and GND |
|--------------------------------------------------------------------------|

---

- GPIO pins can generate interrupts, allowing microcontrollers to respond to external events in real time. Interrupts can be triggered on:
	- Rising Edge: when the signal transitions from LOW to HIGH.
	- Falling Edge: when the signal transitions from HIGH to LOW.
	- Both Edges: when a signal changes in either direction.

> Interrupt-driven GPIO significantly improves system efficiency by reducing the need for continuous polling.

- Many GPIO pins support **PWM (Pulse Width Modulation)**, which allows analog-like control over digital outputs. PWM is useful for:
	- Controlling the brightness of LEDs.
	- Adjusting the speed of motors.
	- Generating sound signals in audio applications.

> PWM works by rapidly switching between HIGH and LOW states, adjusting the duty cycle to control the average output voltage.

- Some GPIO pins serve multiple purposes and can be configured for specialized functions such as:
	- **SPI (Serial Peripheral Interface)**
	- **I2C (Inter-Integrated Circuit)**
	- **UART (Universal Asynchronous Receiver-Transmitter)**

> This feature allows microcontrollers to optimize the number of available pins while maintaining versatile functionality.

- GPIO functionalities are available through the header file `"driver/gpio.h"`.

- Example program for handling GPIO as digital outputs:

```c
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h" // needed to access task delay function

#define LED_PIN_1 21
#define LED_PIN_2 33

void app_main(void)
{
	gpio_reset_pin(LED_PIN_1); // resets GPIO to default state, enabling the pin as GPIO
	gpio_set_direction(LED_PIN_1, GPIO_MODE_DEF_OUTPUT); // defines GPIO pin as output
	
	gpio_reset_pin(LED_PIN_2);
	gpio_set_direction(LED_PIN_2, GPIO_MODE_DEF_OUTPUT);
	
	for (;;) {
		gpio_set_level(LED_PIN_1, 1); // sets to HIGH (on)
		vTaskDelay(1000/portTICK_PERIOD_MS); // delays for 1 second
	    gpio_set_level(LED_PIN_2, 0); // sets to LOW (off)
		vTaskDelay(1000/portTICK_PERIOD_MS); // delays for 1 second
		
		gpio_set_level(LED_PIN_1, 0); // sets to LOW (off)
		vTaskDelay(1000/portTICK_PERIOD_MS); // delays for 1 second
		gpio_set_level(LED_PIN_2, 1); // sets to HIGH (on)
		vTaskDelay(1000/portTICK_PERIOD_MS); // delays for 1 second
	}
}
```

- An interrupt is an asynchronous signal from hardware or software indicating an event that needs immediate attention. In the context of microcontrollers like the ESP32, we primarily deal with **hardware interrupts** generated by peripherals:
  1. **GPIO:** a change in the input level (rising edge, falling edge, both edges, high level, low level) on a configured pin. A pull-down resistor will be configured to use Rising Edge, and a pull-up resistor will be configured to use Falling Edge.
  2. **Timers:** a timer reaching a specific count or overflowing.
  3. **Communication Peripherals (UART, SPI, I2C):** data received, transmission complete, error conditions.
  4. **ADC/DAC:** conversion complete.
  5. **WiFi/Bluetooth Controllers:** network events, connection status changes.

#### Interrupt Controller: a piece of hardware that manages interrupt requests from various peripherals. Its main tasks include: detecting interrupt requests from peripherals; prioritizing simultaneous interrupt requests; forwarding the highest-priority, enabled interrupt request to the CPU core; and providing the CPU with information to identify the source of the interrupt (e.g., an _interrupt vector address_).

#### Interrupt Service Routines (ISRs): also known as an **interrupt handler** is the function executed by the CPU in direct response to an interrupt. ISRs have strict constraints due to their asynchronous nature and execution context:

1. **Execution Speed**: ISRs must execute **as quickly as possible**. While an ISR runs, other interrupts (of the same or lower priority) might be blocked, and normal task sheduling is paused. Long ISRs increase system latency and can lead to missed events.
2. **Non-Blocking**: ISRs **must never block**. They cannot wait for semaphores, queues (with a timeout), or call functions like `vTaskDelay()`. Blocking in an ISR can easily lead to system deadlock.
3. **Limited Functionality**: because they interrupt arbitrary code, ISRs typically run with limited context. They usually cannot perform complex computations, floating-point operations (unless specifically configured), or extensive I/O.
4. **Stack Usage**: ISRs use the stack of the task they interrupted (or sometimes a dedicated interrupt stack, depending on configuration). Keep stack usage within ISRs minimal.

- The primary role of an ISR is usually to: intentify the exact cause of the interrupt (if multiple sources share one ISR); cleear the interrupt flag in the peripheral hardware to prevent immediate re-triggering; perform the absolute minimum, time-critical processing; and, optionally, signal a regular FreeRTOS task to perform longer processing (_Deferred Interrupt Processing_).

#### Interrupt Priorities and Leves: interrupt controllers allow assigning priorities or levels to different interrupt sources. This determines the order in which simultaneous interrupts are serviced and whether one ISR can preempt another.

> Higher priority interrupts are serviced before lower priority ones.

> On systems like Xtensa, a higher-priority ISR can interrupt (preempt) a currently running lower-priority ISR.

- Careful priority asisgnment is crucial for real-time performance, ensuring that the most critical events are handled with the lowest latency. ESP-IDF's `esp_intr_alloc` function allows specifying an interrupt level (1-3 are generally recommended for application use on Xtensa, avoiding levels used by the OS).

#### Critical Sections: sometimes, an ISR needs to access data that is also accessed by regular tasks. If the ISR preempts a task _while_ the task is modifying that shared data, a **race condition** can occur, leading to data corruption.

- To prevent this, we use **critical sections**. A critical session is a piece of code that must execute atomically, without being interrupted by other tasks or relevant ISRs. FreeRTOS provides macros for this:
  1. `portENTER_CRITICAL(&spinlock)` / `portENTER_CRITICAL_ISR(&spinlock)`: enters a critical section. Disables interrupts up to `configMAX_SYSCALL_INTERRUPT_PRIORITY`. Requires initializing a `portMUX_TYPE spinlock = portMUX_INITIALIZED_UNLOCKED`.
  2. `portEXIT_CRITICAL(&spinlock)` / `portEXIT_CRITICAL_ISR(&spinlock`: exits the critical section, re-enabling interrupts.

#### Deferred Interrupt Processing: the strict constraints on ISR execution time and complexity often makes it impractical to perform all event handling within the ISR. The standard practice is **deferred interrupt processing**, where the ISR does the bare minimum (clear flag, maybe capture quick data) and then signals (defers work to) a regular FreeRTOS task to handle the bulk of the processing.

- Common techniques for singaling a task from an ISR:
  1. **Semaphores (`xSemaphoreGiveFromISR`):** the ISR "gives" a binary semaphore. A dedicated task wats (`xSemaphoreTake`) on that semaphore. Simple and effective for signaling an event occurence.
  2. **Queues (`xQueuesSendFromISR`):** the ISR sends data (e.g., a sensor reading, event type) to a queue. A task waits (`xQueueReceived`) to process the data from the queue. Useful when data needs to be passed from the ISR to the task.
  3. **Task Notifications (`xTaskNotifyFromISR`, `xTaskNotifyGiveFromISR`):** a direct-to-task signaling mechanism, often faster and more memory-efficient than queues or semaphores for simple signals or passing single values. A task waits using `ulTaskNotifyTake` or `xTaskNotifyWait`.
  4. **Event Groups (`xEventGroupSetBitsFromISR`):** useful for signaling multiple events or conditions. A task waits using `xEventGroupWaitBits`.

---

#### Aside: configuring and using GPIO with ESP-IDF

```c
gpio_reset_pin(gpio_num_t gpio_num); // resets a gpio to default state
gpio_set_direction(gpio_num_t gpio_num, gpio_mode_t mode); // configures gpio mode (overwrites all current modes that have been applied on the IO pin
```

---

#### Aside: bitmasks in C



---

## Components - Part I
