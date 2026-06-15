// Rotary Encoder Inputs
#define CLK 9
#define DT 10
#define SW 11

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <AccelStepper.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir = "";
unsigned long lastButtonPress = 0;
int x = 0;
int y = 0;
int time = 0;
int finalEncoder = 0;
bool setX = false;
bool setY = false;
bool setTime = false;
bool startIntegral = false;
bool stopIntegral = false;
float finalVal = 0.0;

enum MachineState {
  X,
  Y,
  TIME,
  READY,
  INTEGRAL,
  STOP,
};

MachineState currentState = x;

void setup() {
  // Set encoder pins as inputs
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);

  // Setup Serial Monitor
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(2.5);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Hello!");
  display.display(); 

  // Read the initial state of CLK
  lastStateCLK = digitalRead(CLK);
}

void loop() {
  if (setX == false) {
    counter = 0;
    findX();
  }
  if (setX == true && setY == false) {
    counter = 0;
    findY();
  }
  if (setX == true && setY == true && setTime == false) {
    counter = 0;
    findTime();
  }
  if (setX == true && setY == true && setTime == true && startIntegral == false) {
    counter = 0;
    int btnState = digitalRead(SW);
    display.clearDisplay();
    display.setCursor(50, 25);
    display.println("Ready to Start");
    display.display();

    if (btnState == LOW) {
      if (millis() - lastButtonPress > 50) {
        Serial.println("Button pressed!");
        display.clearDisplay();
        display.setCursor(50, 25);
        display.println("Starting");
        display.display();
        currentState = INTEGRAL;
        startIntegral = true;
      }
      lastButtonPress = millis();
    }
  }
  if (startIntegral == true) {
    doIntegral();
  }
  if (startIntegral == true && stopIntegral == true) {
    //finalVal = (finalEncoder * enc_per_sec) / b   
    display.clearDisplay();
    display.setCursor(50, 25);
    display.println(finalVal);
    display.display();
    int btnState = digitalRead(SW);
    if (btnState == LOW) {
      if (millis() - lastButtonPress > 50) {
        Serial.println("Button pressed!");
        display.clearDisplay();
        display.setCursor(50, 25);
        display.println("Restarting");
        display.display();
        x = 0;
        y = 0;
        time = 0;
        finalEncoder = 0;
        setX = false;
        setY = false;
        setTime = false;
        startIntegral = false;
        stopIntegral = false;
        finalVal = 0.0;
      }
      lastButtonPress = millis();
    }
  }
}

void doIntegral() {
  // Read the current state of CLK
  currentStateCLK = digitalRead(CLK);

  // If last and current state of CLK are different, then pulse occurred
  // React to only 1 state change to avoid double count
  if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {

    // If the DT state is different than the CLK state then
    // the encoder is rotating CCW so decrement
    if (digitalRead(DT) != currentStateCLK) {
      counter--;
      currentDir = "CCW";
    } else {
      // Encoder is rotating CW so increment
      counter++;
      currentDir = "CW";
    }

    Serial.print("Direction: ");
    Serial.print(currentDir);
    Serial.print(" | Counter: ");
    Serial.println(counter);
    display.clearDisplay();
    display.setCursor(50, 25);
    display.println(counter);
    display.display();
  }

  // Remember last CLK state
  lastStateCLK = currentStateCLK;

  // Read the button state
  int btnState = digitalRead(SW);

  //If we detect LOW signal, button is pressed
  if (btnState == LOW) {
    //if 50ms have passed since last LOW pulse, it means that the
    //button has been pressed, released and pressed again
    if (millis() - lastButtonPress > 50) {
      Serial.println("Button pressed!");
      display.clearDisplay();
      display.setCursor(0, 10);
      display.println("Done");
      display.display(); 
      finalEncoder = counter;
      currentState = STOP;
      stopIntegral = true;
    }

    // Remember last button press event
    lastButtonPress = millis();
  }
}

void findX() {
      display.clearDisplay();
      display.setCursor(0, 10);
      display.println("X: ");
      display.println(counter);
      display.display(); 

      currentStateCLK = digitalRead(CLK);

      // If last and current state of CLK are different, then pulse occurred
      // React to only 1 state change to avoid double count
      if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {

        // If the DT state is different than the CLK state then
        // the encoder is rotating CCW so decrement
        if (digitalRead(DT) != currentStateCLK) {
          counter--;
          currentDir = "CCW";
        } else {
          // Encoder is rotating CW so increment
          counter++;
          currentDir = "CW";
        }

        Serial.print("Direction: ");
        Serial.print(currentDir);
        Serial.print(" | Counter: ");
        Serial.println(counter);
      }

      // Remember last CLK state
      lastStateCLK = currentStateCLK;

      // Read the button state
      int btnState = digitalRead(SW);

      //If we detect LOW signal, button is pressed
      if (btnState == LOW) {
        //if 50ms have passed since last LOW pulse, it means that the
        //button has been pressed, released and pressed again
        if (millis() - lastButtonPress > 50) {
          Serial.println("Button pressed!");
          x = counter;
          display.clearDisplay();
          display.setCursor(0, 10);
          display.println("X Set");
          display.display(); 
          currentState = Y;
          setX = true;
        }

        // Remember last button press event
        lastButtonPress = millis();
      }
}

void findY() {
      display.clearDisplay();
      display.setCursor(0, 10);
      display.println("y: ");
      display.println(counter);
      display.display(); 

      currentStateCLK = digitalRead(CLK);

      // If last and current state of CLK are different, then pulse occurred
      // React to only 1 state change to avoid double count
      if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {

        // If the DT state is different than the CLK state then
        // the encoder is rotating CCW so decrement
        if (digitalRead(DT) != currentStateCLK) {
          counter--;
          currentDir = "CCW";
        } else {
          // Encoder is rotating CW so increment
          counter++;
          currentDir = "CW";
        }

        Serial.print("Direction: ");
        Serial.print(currentDir);
        Serial.print(" | Counter: ");
        Serial.println(counter);
      }

      // Remember last CLK state
      lastStateCLK = currentStateCLK;

      // Read the button state
      int btnState = digitalRead(SW);

      //If we detect LOW signal, button is pressed
      if (btnState == LOW) {
        //if 50ms have passed since last LOW pulse, it means that the
        //button has been pressed, released and pressed again
        if (millis() - lastButtonPress > 50) {
          Serial.println("Button pressed!");
          y = counter;
          display.clearDisplay();
          display.setCursor(0, 10);
          display.println("Y Set");
          display.display(); 
          currentState = TIME;
          setY = true;
        }

        // Remember last button press event
        lastButtonPress = millis();
      }
}

void findTime() {
      display.clearDisplay();
      display.setCursor(0, 10);
      display.println("Time: ");
      display.println(counter);
      display.display(); 

      currentStateCLK = digitalRead(CLK);

      // If last and current state of CLK are different, then pulse occurred
      // React to only 1 state change to avoid double count
      if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {

        // If the DT state is different than the CLK state then
        // the encoder is rotating CCW so decrement
        if (digitalRead(DT) != currentStateCLK) {
          counter--;
          currentDir = "CCW";
        } else {
          // Encoder is rotating CW so increment
          counter++;
          currentDir = "CW";
        }

        Serial.print("Direction: ");
        Serial.print(currentDir);
        Serial.print(" | Counter: ");
        Serial.println(counter);
      }

      // Remember last CLK state
      lastStateCLK = currentStateCLK;

      // Read the button state
      int btnState = digitalRead(SW);

      //If we detect LOW signal, button is pressed
      if (btnState == LOW) {
        //if 50ms have passed since last LOW pulse, it means that the
        //button has been pressed, released and pressed again
        if (millis() - lastButtonPress > 50) {
          Serial.println("Button pressed!");
          time = counter;
          display.clearDisplay();
          display.setCursor(0, 10);
          display.println("Time Set");
          display.display(); 
          currentState = READY;
          setTime = true;
        }

        // Remember last button press event
        lastButtonPress = millis();
      }
}
