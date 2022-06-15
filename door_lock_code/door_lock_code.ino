#include <SPI.h>
#include <RFID.h>
#include <Servo.h>
#include <LiquidCrystal.h>

LiquidCrystal screen(8, 7, 6, 5, 4, 2);
RFID scanner(10, 9);
Servo lockSystem;

unsigned char status;
unsigned char str[MAX_LEN]; //MAX_LEN is 16: size of the array. took these 2 lines from RFID example code online but assume its the only way to do it?

String accessGranted [2] = {"1", "1138713418"};  //only one of my keys has got access for presenatation purposes
int accessGrantedSize = 2;

int lockedPos = 70;
int unlockedPos = 160;
bool locked = true;

void setup() {
  Serial.begin(9600);
  screen.begin(16, 2); //screen is 16 wide 2 high
  SPI.begin();
  screen.clear();
  scanner.init();
  lockSystem.attach(3);
  lockSystem.write(lockedPos);
  startDisplay();
}

void loop() {
  checkForCard();
}

void checkForCard() { //used basic formula from example on how to read cards
  if (scanner.findCard(PICC_REQIDL, str) == MI_OK)
  {
    Serial.println("Reading Card:");
    String id = "";
    if (scanner.anticoll(str) == MI_OK)
    {
      Serial.print("The cards ID is : ");
      for (int i = 0; i < 4; i++)
      {
        id = id + (0x0F & (str[i] >> 4));
        id = id + (0x0F & str[i]);
      }
      Serial.println (id);
      checkForAccess(id);
    }
    scanner.selectTag(str);
  }
  scanner.halt();
}

void checkForAccess(String id) {
  bool accepted = false;
  for (int i = 0; i <= (accessGrantedSize - 1); i++) {
    if (accessGranted[i] == id) {
      Serial.println ("Access Granted");
      screen.clear();
      screen.setCursor(0, 0);
      screen.print("Access Granted");
      delay(200);
      accepted = true;
      if (locked == true) { //locked so open
        lockSystem.write(unlockedPos);
        locked = false;
      }
      else if (locked == false) //unlocked so close
      {
        lockSystem.write(lockedPos);
        locked = true;
      }
    }
    if (accepted == false)     //If ID is not in system
    {
      Serial.println ("Access Denied");
      screen.clear();
      screen.setCursor(0, 0);
      screen.print("Access Denied");
    }
  }
}

void startDisplay() {
  Serial.println("Waiting for card to be placed on sensor");
  screen.setCursor(0, 0);
  screen.print("Place Card");
  screen.setCursor(0, 1);
  screen.print("On Reader");
}
