#include <LiquidCrystal.h>
#include <LCD_I2C.h>
#include <AccelStepper.h>
#include <HCSR04.h>
#define TRIGGER_PIN 11
#define ECHO_PIN 12
#define MOTOR_INTERFACE_TYPE 4
#define IN_1 2
#define IN_2 3
#define IN_3 4
#define IN_4 5

LCD_I2C lcd(0x27, 16, 2);
HCSR04 hc(TRIGGER_PIN, ECHO_PIN);
AccelStepper motor(MOTOR_INTERFACE_TYPE, IN_1, IN_3, IN_2, IN_4);

unsigned long currentTime = 0;

int distance;
int degre;
int aimingAt;
const int zero = 0;
const int maxNear = 30;
const int maxFar = 60;
const int motorLeft = 0;
const int motorRight = 1024;
const int left = 10;
const int right = 170;


void setup() {                                                                                  //setup
  Serial.begin(115200);

  lcd.begin(115200);
  lcd.backlight();

  motor.setMaxSpeed(500);
  motor.setAcceleration(150);
	motor.setSpeed(150);
  motor.moveTo(0);

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.println("Setup completed!");


  lcd.print("2486739");
  lcd.setCursor(0, 2);
  lcd.print("Labo 4B");
  Serial.println("The screen has displayed my student's and lab number already!");
  delay(2000);

  screen();
}

void loop() {                                                                                   //loop
  currentTime = millis();

  getDistance(currentTime);
  refreshScreenPlusStats(currentTime);
  aimingStates();
}

void refreshScreenPlusStats(unsigned long ct) {                                          //refreshScreenPlusStats
  unsigned static long timer;
  static bool timerIsDone = false;

  if (!timerIsDone) {
    timer = ct + 100;
    timerIsDone = true;
  }

  if (ct >= timer) {
    Serial.print("etd:2486739,dist:");
    Serial.print(distance);
    Serial.print(",deg:");
    Serial.println(degre);

    screen();

    timerIsDone = false;
  }
}

void getDistance(unsigned long ct) {                                                                            //getDistance
  unsigned static long timer;
  static bool isTimerDone = false;
  static int lastDistance = 0;
  static int tempDistance;

  if (!isTimerDone) {
    timer = ct + 50;
    isTimerDone = true;
  }

  if (ct >= timer) {
    tempDistance = hc.dist();
    if (tempDistance != 0) {
      distance = tempDistance;
      lastDistance = distance;
    }
    else {
      distance = lastDistance;
    }
    isTimerDone = false;
  }
}

void aimingStates() {                                                                           //aimingStates
  enum AimingStates {MOTOR_OFF, AIMING};
  static AimingStates currentState;

  currentState = (distance < 30 || distance > 60) ? MOTOR_OFF : AIMING;

  motor.run();
  switch (currentState) {
    case AIMING:
      aiming();
      break;
    case MOTOR_OFF:
      motorOff();
      break;
  }
}

void aiming() {                                                                                 //aiming
  degre = map(distance, maxNear, maxFar, left, right);
  aimingAt = map(degre, left, right, motorLeft, motorRight);
  motor.moveTo(aimingAt);
}

void motorOff() {                                                                               //motorOff
  if (motor.distanceToGo() == 0) {
    motor.disableOutputs();
  }
}


void screen() {                                                                                 //Screen
  lcd.clear();

  lcd.print("Dist: ");
  lcd.print(distance);
  lcd.print(" cm");

  lcd.setCursor(0, 2);
  lcd.print("Obj: ");
  if (!(distance < maxNear) && !(distance > maxFar)) {
    lcd.print(degre);
    lcd.print(" deg");
  }
  else if (distance < maxNear) {
    lcd.print("Trop pret");
  }
  else {
    lcd.print("Trop loin");
  }
}