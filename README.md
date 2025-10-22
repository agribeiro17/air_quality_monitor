# Air Quality Monitoring System

This project was developed as part of the **SEMB** course within the Master’s in Electrical and Computer Engineering at [FEUP](https://sigarra.up.pt/feup/en/ucurr_geral.ficha_uc_view?pv_ocorrencia_id=540609)

The main objective of the course was to provide students with the skills to develop **embedded systems under real-time constraints**, using real-time operating systems when appropriate, and to understand the design, modeling, and implementation of such systems.

For this project, an **air quality monitoring system** was implemented using the **Arduino UNO**, with a **non-preemptive microkernel** and a **First Come First Served (FCFS)** scheduling approach to satisfy strict timing requirements. The work involved reading and processing sensor data in real time, ensuring timely responses, and performing comparative analysis to validate the correct operation and performance of the system.

Through this project, I gained hands-on experience in **real-time system modeling, microcontroller programming, timing analysis, and embedded system design**—key skills for developing robust and responsive real-time applications. [For a full overview of the project, see the PDF here](https://drive.google.com/file/d/1iPkmDNjr2r15kFsSrqE4MUIi9v9FBpf7/view?usp=sharing) or in the pdf folder.


## 🧰 Technologies Used

This air quality monitoring system integrates hardware, software, and embedded real-time scheduling techniques. The main technologies and libraries used include:

### **Hardware**

- **Arduino UNO** – main microcontroller platform
- **DHT11 Sensor** – temperature and humidity measurement
- **Air Quality Sensor** – analog input for detecting air pollutants
- **4x4 Keypad** – user input for page navigation
- **OLED Display (SSD1306)** – visual feedback for sensor readings

### **Arduino Libraries**

- `dht11.h` – to read temperature and humidity data
- `Wire.h` – I2C communication for OLED display
- `Adafruit_GFX.h` – graphics library for OLED
- `Adafruit_SSD1306.h` – OLED display driver
- `Keypad.h` – handle user input via keypad

### **Software / Programming Concepts**

- **C++ / Arduino IDE** – primary programming language for embedded code
- **Scheduler / Real-Time Tasks** – non-preemptive FCFS scheduling with periodic tasks
  - `Sched_Task_t` structure defines period, delay, and function pointer for tasks
  - Timer1 interrupts trigger the scheduler (`ISR(TIMER1_COMPA_vect)`)
  - Tasks include reading sensors, updating display, and reading keypad
- **Real-Time Event Handling** – timing control using delays, task periods, and interrupts
- **OLED Display Control** – updating multiple pages for temperature, humidity, and air quality
- **Keypad Handling** – reading user input to switch between display pages
- **Serial Debugging** – monitoring sensor values and system state for testing

### **Python Support Script**

- **Python 3** – script to read analog sensor data from Arduino (via serial)
- **Matplotlib** – used to generate **bar graphs** showing measured task periods
- **PySerial** – for communication between Arduino and Python on the host PC

This combination of embedded hardware, real-time scheduling, and visualization tools enabled **precise timing analysis and performance validation** of the air quality monitoring system.

## Libraries to install within Arduino
### Adafruit:

    - https://makeabilitylab.github.io/physcomp/advancedio/oled-libraries.html

### Keypad:

	- Sketch -> Include Library -> Manage Libraries and search for “keypad”.
	- Click on the library, then click Install.

