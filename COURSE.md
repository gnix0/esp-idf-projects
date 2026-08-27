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

> **IMPORTANT:** _the materials in this file that contain SoC-specific details will do so according to the ESP32-C6 SoC. For other SoCs/modules, check their respective documentations as described below._

- **IMPORTANT: ALL PROJECTS MUST HAVE THE FOLLOWING '.clangd' FILE AT ROOT**

```bash
CompileFlags:
    Remove: [-f*, -m*]
```


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

## ESP-IDF Basics

- Activate env:

```bash
get_idf
```

- Set this in a `.dir-locals.el` file at the root of an ESP-IDF project:

```elisp
# Work laptop
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
			 
# Personal laptop
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

> IMPORTATNT: after creating a new component (or more), it is necessary to reconfigure the project using `idf.py reconfigure` for properly applying the new changes on the project's structure. Otherwise, the build will fail with errors.


## PWM - Pulse Width Modulation

- PWM is a power-control technique that regulates the effective output of an electrical signal by rapidly switching it on and off at a fixed frequency. By adjusting the ratio on the "on" time to the total cycle period, a digital source can emulate varying analog voltage levels, thereby controlling the average energy delivered to a load. More broadly, modulation here refers to altering or encoding information onto an electrical waveform to influence the behavior of a circuit or system. In practical electronics, this means shaping a signal so it can transmit data or manage how much voltage or current reaches a device. This is widely applied in motor drives, dimmable lighting, audio systems, and power-conversion or battery-charging circuits.

- PWM, Amplitude Modulation (AM), and Frequency Modulation (FM) are the primary strategies for manipulating a signal's perceived magnitude or frequency.

- PWM shapes a waveform by adjusting the effective voltage and current delivered to a load. This is achieved by rapidly driving a switching device, typically a transistor, between its fully-on and fully-off states. By varying how long the switch remains in each state, the system encodes information through the relative duration of the high and low intervals. Practically speaking, PWM limits the net electrical power supplied to a device by altering how long it receives its full supply voltage during each switching cycle. Increase the "on-time" raises the average output voltage, while decreasing it lowers the effective level seen by the load. Two primary parameters characterize this behavior: the duty cycle, and the switching frequency.
  1. **duty cycle:** represents the proportion of a complete waveform period during which a signal is in its active, or high, state. It is typically given as a percentage (%) and indicates how much of each cycle the output remains on. For instance, if a digital waveform stays high for 3ms and low for 1ms, the total period is 4ms, resulting in a 75% **duty cycle** and a corresponding **switching frequency** of 250Hz. Because the duty cycle defines the duration of the energized portion of each pulse, modifying it allows control over the effective power delivered to a load by altering the ratio of high to low time, without changing the actual supply voltage. In many systems, voltage and frequency are fixed parameters, leaving duty cycle as the primary adjustable control variable. In applications such as _PWM-driven heatin elements_, monitoring the duty cycle can also serve as a reliable indicator that the system is delivering the intended power level.
  2. **switching frequency:** describes how many times an event repeats during a given time period, in this context, the number of on-off transitions per second made by the switching device that drives the PWM signal. This rate is measured in hertz (Hz), indicating how quickly the power stage cycles through its full operating period. If the frequency is set excessively high for a given application, mechanical components such as relays or certain types of actuators may be unable to keep up with the rapid transitions and can prematurely fail. Conversely, a switching frequency that is too low may cause undesirable effects such as audible noise, vibration, or instability in the controlled device. For example, while relatively low frequencies are acceptable for driven electric motors, solid-state loads such as **LEDs** often require _significantly higher switching rates_ to achieve smooth, flicker-free operation.
  
> Advantages of PWM: the primary benefit is its exceptional efficiency, largely because switching devices dissipate very little power. When the switch is off, virtually no current flows, and when it is fully on, the voltage drop across the device is minimal. This results in significantly reduced conduction and switching losses compared to linear control methods. Additional advantages of PWM include: lower thermal dissipation than linear regulators, thanks to its fully-on/fully-off operation; seamless integration with digital logic, since the control signal is inherently binary; higher overall energy efficiency when regulating motors, lighting, or power converters; precise control of effective voltage or current simply by adjusting the duty cycle; simplified circuitry, often requiring fewer analog components or feedback loops; and borad applicability accross many electronic systems, from power supplies to actuators.
> Key drawbacks of PWM include: increased switching losses at very high operating frequencies; potential for voltage overshoot or transients; generation of _electromagnetic interference (EMI)_ and harmonics that may require filtering; and greater design complexity in high-power systems, where switching elements and layout considerations become more demanding.

- The _ESP32 LED Control (LEDC)_ peripheral was primarily designed for controlling LED intensity, although it can also be used to generate PWM signals for other purposes. It provides hardware timers and PWM waveform generators. The PWM controller has the following resources:
  1. **4 independent timers** supporting fractional clock division.
  2. **8 independent channels** capable of generating eight PWM signals.
  3. **Hardware PWM fading**, with an interrupt that can be generated when a fade operation is complete.
  4. **Adjustable PWM output phase.**
  5. **PWM output in low-power mode.**

- Configuring a LEDC channel:
  1. Configure the timer, specifying the PWM signal frequency and duty-cycle resolution.
  2. Configure the channel, associating it with the timer and the GPIO used for PWM signal output.
  3. Change the PWM signal that controls the output to adjust the LED's intensity. This can be done entirely under software control, or using hardware fading functions.
  4. (Optional) Set up an interrupt on fase end.


## ADC - Analog to Digital Converters

- Switches, relays, and encoders are inherently digital themselves, and therefore interfacing them with gate circuits is straightforward due to the on/off nature of their signals. However, when analog devices are involved, interfacing becomes more complex.

- To translate analog signals into digital (binary) quantities, an **analog-to-digital converter (ADC)** is used. An ADC inputs an analog _electrical signal_ as voltage or current and outputs a _binary number_. These ADC circuits can be found as _individual ADC ICs_ by themselves or embedded into a microcontroller.

![ADC Diagram](./assets/adc_diagram.png)


- How does and ADC work?:
  1. **Reference voltage:** the voltage mapped to the maximum binary value is called the **reference voltage**. For example, in a 10-bit converter with 5V as the reference voltage, 1111111111 corresponds to 5V and 0000000000 corresponds to 0V. So each binary step up represents around 4.9mV, since there are 1024 possible digits in 10 bits. _This measure of 'volts per bit' is called the resolution of the ADC._ If the voltage changes are below the 4.9mV per step, however, the ADC will be put in a dead zone. The conversion result therefore always has a small error. This can be prevented by using an ADC with a higher resolution. ADCs up to 24 bits are available, though conversion frequencies are low (in the order of a few hertz).
  2. **Sample speed:** the number of analog-to-digital conversions the converter can make every second is called the **sample speed**. For example, a really good ADC can have a sample rate of 300Ms/s, to be read as _megasamples per second_, meaning a million samples per second. Sample speed depends completely on the type of converter and the needed accuracy. If a very accurate reading is needed, the ADC usually spends more time looking at the input signal (usually a sample-and-hold or integrating type input), and if accuracy is not a concern they can be quick with the reading. As a general rule of thumb, _speed and accuracy are more or less inversely proportional._

- Types of ADCs:
  1. **Flash ADC:** the simplest type of ADC and the fastest. It consists of a series of comparators with the non-inverting inputs connected to the signal input and the inverting pings connected to a voltage divider ladder. If the voltage is above one of the levels of the ladder, however, all the output bits belot the level are set to one, since the voltage is above the threshold for the bottom comparators. To circumvent this problem, outputs are fed through a priority encoder that converts the output to binary. The speed is limited only by the propagation delays of the comparator and the priority encoder. Accuracy is moderate.
  2. **Counting/Slope Integration ADCs:** a ramp generating circuit is started at the time of conversion and a binary counter is started at the same time. A comparator detects when the ramp goes above the input voltage and stops the binary counter. The binary count obtained is proportional to the input voltage level. The absolute accuracy of this converter is questionable, however, it gives good resolution and even spacing between the binary steps while being simple to implement. If no chips are available, this circuit can even be made discretely.
  3. **Successive Approximation ADCs:** among the most accurate types. They consist of a comparator, a simple flash DAC, and a memory register. The device initially assumes all the bits in the register except for the highest significant bit (which is a one) to be zeroes. The register then sends this to the DAC which converts it to an analog voltage, which is compared with the input through the comparator. If the input voltage is higher than the DAC voltage, then the MSB remains one. This process is repeated until all the bits have been set either to zero or one, i.e., the register value exactly equals the input voltage. This ADC is one of the most commonly used where accuracy is needed and speed is not much of a limitation, such as in microcontrollers. SA type ADCs can easily achieve conversion times of a few microseconds.

- **Applications of ADCs include:** _digital osciloscopes and multimeters, microcontrollers, and digital power supplies._


## DAC - Digital to Analog Converters

- For scenarios where the inverse task is necessary, that is, digital quantities have to be translated to analog signals, a **digital-to-analog converter (DAC)** is used, that is, _a DAC inputs a binary number and outputs an analog voltage or current._

- An example use case: a computer stores audio in the form of binary values of the sound wave, so in order to play these back as sound on a speaker, we need analog signals, since the speaker's diaphragm vibrates based on the intensity of the analog signal to produce sound/music.

![DAC Diagram](./assets/dac_diagram.png)

- How does a DAC work?:

- Since the binary system is positional, the digital-to-analog conversion process can be thought of as a **scaling operation**, where the _binary count is mapped to a certain voltage range, with 0V being the minimum and the maximum voltage being the maximum input binary value._

- Types of DACs:
  1. **Summing Amplifier:** since digital-to-analog conversion is simply a weighted sum of the binary input, a circuit called a **summing amplifier** is used. This is basically an op-amp amplifier with multiple resistors connecter ot one input. The junction where the resistors meet is called the summing junction or the virtual ground. The binary input goes into the resistors and the analog output is obtained on the outpute of the op-amp. Each resistor has to be carefully chosen and matched in order to obtain an accurate analog output. The more bits there are, the more different values of resistores will be needed, which is not always practical.
  2. **R-2R Ladder:** the simplest type of DAC. Needs only two resistor values arranged in a ladder, and can be thought of as a somewhat complex voltage divider, in simple terms. The binary input goes into the 2R resistors and the output is obtained at the bottom of the ladder.
  3. **PWM DAC:** a PWM signal looks like a binary waveform with only high and low peaks with a variable duty cycle (ratio of on time to time period). Here, however, this is used with a RC filter to convert the PWM signal into a voltage value by filtering out the AC component and leaving behind the DC component. The voltage output is proportional to the duty cycle of the input, so the higher the duty cycle the greater the output voltage of the filter.

- **Applications of DACs include:** _digital signal processing, and digital power supplies._

## Built-in Temperature Sensor

- The ESP32-C6 (and other models) has a built-in temperature sensor used to measure the chip's internal temperature. The temperature sensor module contains _an 8-bit Sigma-Delta analog-to-digital converter (ADC), and a digital-to-analog converter (DAC) to compensate for the temperature measurement._

> The temperature sensor is designed primarily to measure the temperature **inside** the silicon. The sensor can reflect the temperature changes very well but it can't give a precise measurement value. Hence it is not recommended to use it for ambient temperature measurement.

- Due to restrictions of hardware, the sensor has predefined measurement ranges with specific measurement errors:

| predefined range (C) | error (C) |
|----------------------|-----------|
| 50 ~ 125             | < 3       |
| 20 ~ 100             | < 2       |
| -10 ~ 80             | < 1       |
| -30 ~ 50             | < 2       |
| -40 ~ 20             | < 3       |
|----------------------|-----------|

## FreeRTOS

#### Super Loops

- Also referred to as **Foreground-Background architecture**. Classic way used in low complexity systems. An application consists of an infinite loop that calls one or more functions in succession to perform the desired operations _(background)._ **Interrupt service routines (ISRs)** are used to handle the asynchronous, real-time parts of the application _(foreground)._ In this architecture, the functions implementing the various functionalities are inherently, even if not formally declared as such, some sort of _finite state machines_, spinning around and switching states based on inputs provided by the ISRs.

![Superloop overview](./assets/superloop_architecture.png)

> This architecture requires only one stack and sometimes may result in simpler applications, especially when the entire functionality is performed on ISRs, and the background logic is reduced to an empty loop witing for interrupts. However, for slightly more complex applications, expressing the entire logic as a set of state machines can become a problem as the program grouws. The overall reaction speed may also be a problem, since the delay between the moment when the ISR makes available the input and the moment when the background routine can use it is not deterministic, depending on many other actions that can happen at the same time in the superloop. To ensure that urgent actions are performed in a timely manner, they must be moved on the ISRs, lengthening them and causing the reaction speed of the application to worsen.

- **Advantages & Disadvantages on the Super Loop architecture:*
  1. Advantages: ease of development; simple and efficient; great for smaller MCUs; doesn't require additional resources for processing.
  2. Disadvantages: doesn't ensure time constraints; a function or interruption influences in the task execution time; hard to maintain and to expand to new functionalities.

- **Important resources that an Operating System brings:** offer a **hardware abstraction layer (HAL)**; memory & processes management; intermediate communication between peripherals and processes; code portability.

#### RTOS Fundamentals

- A **Real-Time Operating System (RTOS)** is a type of operating system designed to be small and deterministic. RTOSes are commonly used in embedded systems such as medical devices and automotive ECUs that need to react to external events withing strict time contraints. Typically this class of embedded system only has one or two requirements demanding such level of deterministic timing, and using an RTOS has benefits even when the embedded system has no hard real-time requirement at all.

- The _kernel_ is the core component withing an OS. General purpose operating systems, such as Linux, employ kernels that allow multiple users to access the computer's processor seemingly simultaneously. These multiple users can each execute multiple programs apparently concurrently. Each execution program is implemented by one or more _threads_ under control of the operating system. If an OS can execute multiple threads in this manner it is said to be _multitasking_. Small RTOSes, like FreeRTOS, normally call threads **tasks** because they don't support virtual memory, so there is no distinction between processes and threads. The use of a multitasking OS can simplify the design of what would otherwise be a complex software applications: the multitasking and inter-task communication features of the OS allow the complex application to be partitioned into a set of smaller and more manageable tasks, the partitioning can result in easier software testing by dividing work within teams and through code reuse, and complex timing and sequencing details become responsibility of the RTOS kernel hence removing the burden from the application code.

- Multitasking vs Concurrency: most OSes appear to allow multiple programs to execute at the same time (multitasking). In reality, each processor core can only be running a single thread of execution at any given point in time.

![Task execution](./assets/task_execution.png)

- Scheduling: the **scheduler** is the part of the kernel responsible for deciding which task should be executing at any particular time. The kernel can pause and later resume a task many times during the task's lifetime. The _scheduling policy_ is the algorithm used by the scheduler to decide which task to execute at any point in time. The policy of a (non real-time) multi-user system will most likely allow each task a "fair" proportion of processor time.

> A task will only be swapped out if the scheduling algorithm decides to execute a different task. This can happen without the currently executing task being aware of it, such as when the scheduling algorithm responds to an external event or timer expiration. It can also happen if the executing task explicitly calls an API function that results in it yielding, sleeping (also called delaying), or blocking. If a task yields, the scheduling algorithm could select the same task to execute again. If a task sleeps, it becomes unavailable for selection until the specified delay period elapses. Similarly, if a task blocks, it becomes unavailable for selection until either a specific event occurs (e.g., data arrives on a UART) or a timeout period expires.

> The operating system kernel is responsible for managing these task states and transitions, ensuring that the appropriate task is selected for execution at any given time according to the scheduling algorithm and the current state of each task

![Scheduling](./assets/suspending.png)

- Real-Time Scheduling: Real-time operating systems (RTOSes) achieve multitasking using these same principles, but their objectives differ greatly to those of general purpose (non real-time) systems. Real-time embedded systems are designed to provide a timely response to real world events. Events occurring in the real world can have deadlines before which the real-time embedded system must respond and the RTOS scheduling policy must ensure these deadlines are met.

> To achieve this objective using a small RTOS, such as FreeRTOS, the developer must assign a priority to each task. The scheduling policy of the RTOS is then to simply ensure that the highest priority task that is able to execute is the task given processing time. This may optionally include sharing processing time "fairly" between tasks of equal priority if there is more than one task at the same highest priority that are able to run (i.e., are not delayed, and are not blocked).

![RTOS Task Scheduling](./assets/rtos_task_scheduling.png)
