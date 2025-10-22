/* * * * * * * * * * *
 *    Libraries
 * * * * * * * * * * */
//#include "DHT.h"
#include <dht11.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h> 

// Sensor Pins
#define DHT11_PIN 4          // Pin of Temperature and Humidity Sensor
#define AIR_Q_PIN 11        // Pin of Air Quality Sensor

// OLED Screen
#define SCREEN_ADD 0x3C     // Address of OLED Display
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels

// Pages
#define PAGE_MAIN '0'
#define PAGE_TEMPERATURE '1'
#define PAGE_HUMIDITY '2'
#define PAGE_AIR_Q '3'

#define PAGE_FORWARD '#'
#define PAGE_BACKWARD '*'

// User Input Pins
// ....

// Global Variables
dht11 DHT11;
int temperature;
int humidity;
char selected_page = PAGE_HUMIDITY;

// -------------------------- Keypad Setup ---------------------------------------
const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
// --------------------------- End of Keypad Setup -------------------------------------



// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 oled_screen(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/* * * * * * * * * * *
 *        Tasks
 * * * * * * * * * * */

// Temperature and Humidity Sensor Reading
void read_dht11(void) {
  int chk = DHT11.read(DHT11_PIN);

  temperature = DHT11.temperature;
  humidity = DHT11.humidity;
}

void read_air_q_sensor(void) {

}

void show_main_page(void) {
  oled_screen.clearDisplay();
  oled_screen.setTextSize(2);
  oled_screen.setCursor(0,0);
  oled_screen.print("Temperature Page: 1");
  oled_screen.setTextSize(2);
  oled_screen.setCursor(0,10);
  oled_screen.print("Humidity Page: 2");
  oled_screen.display();
}

// Displaing Sensor Information
void show_temp_page(void) {
  oled_screen.clearDisplay();
  oled_screen.setTextSize(1);
  oled_screen.setCursor(0,0);
  oled_screen.print("Temperature: ");
  oled_screen.setTextSize(2);
  oled_screen.setCursor(0,10);
  oled_screen.print(temperature);
  oled_screen.print(" ");
  oled_screen.setTextSize(1);
  oled_screen.cp437(true);
  oled_screen.write(167);
  oled_screen.setTextSize(2);
  oled_screen.print("C");
  oled_screen.display();
}

void show_hum_page(void) {
  oled_screen.clearDisplay();
  oled_screen.setTextSize(1);
  oled_screen.setCursor(0, 0);
  oled_screen.print("Humidity: ");
  oled_screen.setTextSize(2);
  oled_screen.setCursor(0, 10);
  oled_screen.print(humidity);
  oled_screen.print(" %");
  oled_screen.display();
}

void show_air_q_page(void) {
  oled_screen.clearDisplay();
  oled_screen.setTextSize(1);
  oled_screen.setCursor(0, 35);
  oled_screen.print("Air Quality: ");
  oled_screen.setTextSize(2);
  oled_screen.setCursor(0, 45);
  oled_screen.print(0);
  oled_screen.print(" metric unit");
  oled_screen.display();
}

// Get Keypad Char from user to choose page
void read_keypad(void) {
  char read_key = customKeypad.getKey();

  if (read_key) {
    selected_page = read_key;
  }
}

void show_selected_page(void) {
  switch (selected_page) {
    case PAGE_MAIN:
      show_main_page();
      break;
    case PAGE_TEMPERATURE:
      show_temp_page();
      break;
    case PAGE_HUMIDITY:
      show_hum_page();
      break;
    case PAGE_AIR_Q:
      show_air_q_page();
    default:
      show_main_page();
      break;
  }
}

/* * * * * * * * * * *
 *      Scheduler
 * * * * * * * * * * */
typedef struct {
  /* period in ticks */
  int period;
  /* ticks until next activation */
  int delay;
  /* function pointer */
  void (*func)(void);
  /* activation counter */
  int exec;
} Sched_Task_t;

#define NT 20
Sched_Task_t Tasks[NT];

int Sched_Init(void) {
  for(int x=0; x<NT; x++)
    Tasks[x].func = 0;
  /* Also configures interrupt that periodically calls Sched_Schedule(). */
  noInterrupts(); // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
 
//OCR1A = 6250; // compare match register 16MHz/256/10Hz
//OCR1A = 31250; // compare match register 16MHz/256/2Hz
  OCR1A = 63;    // compare match register 16MHz/256/2kHz
  TCCR1B |= (1 << WGM12); // CTC mode
  TCCR1B |= (1 << CS12); // 256 prescaler
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  interrupts(); // enable all interrupts  
}

int Sched_AddT(void (*f)(void), int d, int p) {
  for(int x=0; x<NT; x++)
    if (!Tasks[x].func) {
      Tasks[x].period = p;
      Tasks[x].delay = d;
      Tasks[x].exec = 0;
      Tasks[x].func = f;
      return x;
    }
  return -1;
}

void Sched_Schedule(void) {
  for(int x=0; x<NT; x++) {
    if(Tasks[x].func) {
      if(Tasks[x].delay) {
        Tasks[x].delay--;
      }
      else {
        /* Schedule Task */
        Tasks[x].exec++;
        Tasks[x].delay = Tasks[x].period-1;
      }
    }
  }
}

void Sched_Dispatch(void) {
  for(int x=0; x<NT; x++) {
    if((Tasks[x].func)&& (Tasks[x].exec)) {
      Tasks[x].exec=0;
      Tasks[x].func();
      /* Delete task if one-shot */
      if(!Tasks[x].period) Tasks[x].func = 0;
        return; // fixed priority version!
    }
  }
}

// the setup function runs once when you press reset or power the board
void setup() {
  // For debbuging with Serial Monitor
  Serial.begin(9600);

  // Initialize OLED Display
  if(!oled_screen.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(1000);
  oled_screen.clearDisplay();
  oled_screen.setTextColor(WHITE);

  // Set Input Pins
  pinMode(DHT11_PIN, INPUT);

  // Initialize Scheduler
  Sched_Init();

  // Add tasks to scheduler
  Sched_AddT(read_dht11, 50, 500);
  Sched_AddT(show_selected_page, 100, 500);
  Sched_AddT(read_keypad, 150, 50);
}

//timer1 interrupt
ISR(TIMER1_COMPA_vect) {
  Sched_Schedule();
}

// the loop function runs over and over again forever
void loop() {
  Sched_Dispatch();
}



