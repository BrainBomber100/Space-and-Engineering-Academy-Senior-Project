#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates
#define SENSORPIN 9

uint8_t servonum = 0;
int sensorState = 0, lastState = 0;
bool x = true;
bool y = true;
int a = 0;
int b = 0;
int list[7] = {0, 0, 0, 0, 0, 0, 0};
const int selectPins[3] = {2, 3, 4}; // S0~2, S1~3, S2~4
const int zOutput = 5;
const int zInput = A0;
int count = 0;
int flag = 0;

void setup() {
  Serial.begin(9600);

  Serial.println("8 channel Servo test!"); 
  pinMode(SENSORPIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  randomSeed(analogRead(1));
  digitalWrite(SENSORPIN, HIGH);
  for (int i = 0; i < 3; i++)
  {
    pinMode(selectPins[i], OUTPUT);
    digitalWrite(selectPins[i], HIGH);
  }
  pinMode(zInput, INPUT);
  Serial.println("Y0\tY1\tY2\tY3\tY4\tY5\tY6");
  Serial.println("---\t---\t---\t---\t---\t---\t---"); //This will print the input of each force-sensitive resistor.


  pwm.begin();

  pwm.setOscillatorFrequency(27000000);  // The int.osc. is closer to 27MHz
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  delay(10);
}


void OpenMouth()
{
  //pwm.setPWM(3, 0, 370); //370
  //delay(100);
  //pwm.setPWM(2, 0, 230); //230

  for (int i = 285; i <= 370; i++) //This For loop helps make the servos run slower when using PWM.
  {
    pwm.setPWM(3, 0, i);
    pwm.setPWM(2, 0, i-160);
    delay(2);
  }

}

void CloseMouth()
{
  //pwm.setPWM(3, 0, 285); //285
  //delay(100);
  //pwm.setPWM(2, 0, 125); //125

  for (int x = 370; x >= 285; x--) 
  {
    pwm.setPWM(3, 0, x);
    pwm.setPWM(2, 0, x-140);
    delay(2);
  }
}

void OpenEyes()
{
  pwm.setPWM(0, 0, 235); //235
  pwm.setPWM(1, 0, 65); //65
}

void CloseEyes()
{
  pwm.setPWM(0, 0, 310); //310
  pwm.setPWM(1, 0, 140); //140
}

void DoorOff()
{
  pwm.setPWM(4, 0, 4096);
  pwm.setPWM(5, 0, 4096);
}

void OpenDoor() 
{
  pwm.setPWM(4, 0, 100); //310
  pwm.setPWM(5, 0, 410); //140
  delay(2300);//2450
  DoorOff();
}

void CloseDoor() 
{
  pwm.setPWM(4, 0, 410); //310
  pwm.setPWM(5, 0, 100); //140
  delay(2500);//2450
  DoorOff();
}

void Off()
{
  pwm.setPWM(0, 0, 4096);
  pwm.setPWM(1, 0, 4096);
  pwm.setPWM(2, 0, 4096);
  pwm.setPWM(3, 0, 4096);
}



void selectMuxPin(byte pin) //This function is for the Multiplexer Breakout Board.
{
  for (int i = 0; i < 3; i++)
  {
    if (pin & (1 << i))
      digitalWrite(selectPins[i], HIGH);
    else
      digitalWrite(selectPins[i], LOW);
  }
}

// 0 & 1 are eyes
//2 & 3 are mouth
void loop() {
  sensorState = digitalRead(SENSORPIN);
 
  if (sensorState && !lastState) {
    Serial.println("Unbroken");
    count++;
    if (count % 2 != 0)
    {
      x = true;
      flag = 1;
      Serial.print("Count: ");
      Serial.println(count);
    }
    else
    {
      x = false;
      flag = 1;
      Serial.print("Count: ");
      Serial.println(count);
    }
    Serial.print("x ");
    Serial.println(x);
  }
  if (!sensorState && lastState) {
    Serial.println("Broken");
  }
  lastState = sensorState;


  // check if the sensor beam is broken
  // if it is, the sensorState is LOW:
  if (sensorState == HIGH && x == true && flag == 1) {
    Serial.println("open");
 
    OpenDoor();
 
    flag = 0;
  }

  if (sensorState == HIGH && x == false && flag == 1)
  {
    Serial.println("close");
    
    CloseDoor();
   
    flag = 0;
  }
  delay (100);

  //reads sensors and wiggles eyes random amount of times when book is taken
  for (byte pin = 0; pin < 7; pin++)
  {
    selectMuxPin(pin); // Select one at a time
    int inputValue = analogRead(A0); // and read Z
    if (inputValue <= 50 && list[pin] > 50)
    {
      int num = random(2, 5);
      for (int i = 1; i <= num; i++) 
      {
        Serial.println("running");
        Serial.println(num);
        OpenMouth();
        delay(250);
        CloseMouth();
        delay(250);
        Off();
      }
      list[pin] = 0;
    }
    if ( inputValue > 50 && list[pin] <= 50)
    {
      list[pin] = inputValue;
    }
    //Off();
    Serial.print(String(inputValue) + "\t");
  }
  Serial.println();
  delay(150);

}
