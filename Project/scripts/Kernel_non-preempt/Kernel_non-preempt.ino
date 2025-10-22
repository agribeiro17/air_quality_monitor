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
#define DHT11_PIN 4         // Pin of Temperature and Humidity Sensor Data
#define AIR_Q_PIN A0        // Pin of Air Quality Sensor Analog Data
#define METH_PIN A1         // Pin of Methane Sensor Analog Data

// OLED Screen
#define SCREEN_ADD 0x3C     // Address of OLED Display
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels

// Pages
#define PAGE_MAIN '0'
#define PAGE_DHT11 '1'
#define PAGE_AIR_Q '2'

// Logic Analyser Pins
#define LA_TASK1 13
#define LA_TASK2 12
#define LA_TASK3 11
#define LA_TASK4 10
#define LA_TASK5 A2


// Global Variables
dht11 DHT11;

int temperature = 0;
int humidity = 0;
int air_quality = 0;
int methane = 0;

char selected_page = PAGE_MAIN;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 oled_screen(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

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

/* * * * * * * * * * *
 *        Tasks
 * * * * * * * * * * */

// Keypad Reading
void read_keypad(void) {
  digitalWrite(LA_TASK4, HIGH);
  char read_key = customKeypad.getKey();

  if (read_key) {
    //Serial.print("Keypad Char Read: ");
    //Serial.print(read_key);
    //Serial.print("\n");
    selected_page = read_key;
  }
  digitalWrite(LA_TASK4, LOW);
}

// DHT11 Sensor Reading
void read_dht11(void) {
  digitalWrite(LA_TASK1, HIGH);

  int chk = DHT11.read(DHT11_PIN);

  temperature = DHT11.temperature;
  humidity = DHT11.humidity;
/*
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("\n");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("\n\n");
*/
  digitalWrite(LA_TASK1, LOW);
}

// Air Quality Sensor Reading
void read_air_q_sensor(void) {
  digitalWrite(LA_TASK2, HIGH);

  air_quality = analogRead(AIR_Q_PIN);

  digitalWrite(LA_TASK2, LOW);
}

// Methane Sensor Reading
void read_meth_sensor(void) {
  digitalWrite(LA_TASK3, HIGH);

  methane = analogRead(METH_PIN);

  digitalWrite(LA_TASK3, LOW);
}

//////////////////////////
// Displaing Information
/////////////////////////
// Main Page
void show_main_page(void) {
  oled_screen.clearDisplay();

  oled_screen.setTextSize(1);
  oled_screen.setCursor(0, 0); // 46 limit
  oled_screen.print("Main Page:");

  oled_screen.setTextSize(1);
  oled_screen.setCursor(0, 15);
  oled_screen.print("1 - Temperature and  Humidity");

  oled_screen.setTextSize(1);
  oled_screen.setCursor(0, 45);
  oled_screen.print("2 - Air Quality and  Methane");

  oled_screen.display();
}

// Page to show Temperature and Humidity from DHT11 sensor
void show_dht11_page(void) {
  oled_screen.clearDisplay();
  oled_screen.setTextSize(1);
  oled_screen.setCursor(0,0);
  oled_screen.print("Temperature: ");
  oled_screen.setTextSize(1);
  oled_screen.setCursor(0,10);
  oled_screen.print(temperature);
  oled_screen.print(" ");
  oled_screen.setTextSize(1);
  oled_screen.cp437(true);
  oled_screen.write(167);
  oled_screen.setTextSize(1);
  oled_screen.print("C");
  oled_screen.setTextSize(1);
  oled_screen.setCursor(0, 30);
  oled_screen.print("Humidity:");
  oled_screen.setTextSize(1);
  oled_screen.setCursor(0, 40);
  oled_screen.print(humidity);
  oled_screen.print(" %");
  oled_screen.display();
}

// Page to show Air Quality and Methane from the respective sensors
void show_air_q_page(void) {
  oled_screen.clearDisplay();
  oled_screen.setTextSize(1);
  oled_screen.setCursor(0, 0);
  oled_screen.print("Air Quality:");
  oled_screen.setTextSize(1);
  oled_screen.setCursor(0, 10);
  oled_screen.print(air_quality);
  oled_screen.print(" ppm");
  oled_screen.setTextSize(1);
  oled_screen.setCursor(0, 30);
  oled_screen.print("Methane:");
  oled_screen.setTextSize(1);
  oled_screen.setCursor(0, 40);
  oled_screen.print(methane);
  oled_screen.print(" ppm");
  oled_screen.display();
}

// Task to display the selected page
void show_page(void) {
  digitalWrite(LA_TASK5, HIGH);
  switch (selected_page) {
    case PAGE_DHT11:
      show_dht11_page();
      break;
    case PAGE_AIR_Q:
      show_air_q_page();
      break;
    default:
      show_main_page();
      break;
  }
  digitalWrite(LA_TASK5, LOW);
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

#define NT 5
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
  OCR1A = 630;    // compare match register configure for a tick of 10 ms
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
  pinMode(AIR_Q_PIN, INPUT);
  pinMode(METH_PIN, INPUT);
  pinMode(LA_TASK1, OUTPUT);
  pinMode(LA_TASK2, OUTPUT);
  pinMode(LA_TASK3, OUTPUT);
  pinMode(LA_TASK4, OUTPUT);
  pinMode(LA_TASK5, OUTPUT);

  digitalWrite(LA_TASK1, LOW);
  digitalWrite(LA_TASK2, LOW);
  digitalWrite(LA_TASK3, LOW);
  digitalWrite(LA_TASK4, LOW);
  digitalWrite(LA_TASK5, LOW);

  Sched_Init();

  // Add tasks to scheduler
  Sched_AddT(read_dht11, 0, 500);
  Sched_AddT(read_air_q_sensor, 0, 200); // 50
  Sched_AddT(read_meth_sensor, 0, 200); // 50
  Sched_AddT(read_keypad, 0, 6); // 2
  Sched_AddT(show_page, 0, 25); // 25
}

//timer1 interrupt
ISR(TIMER1_COMPA_vect) {
  Sched_Schedule();
}

// the loop function runs over and over again forever
void loop() {
  Sched_Dispatch();
}
