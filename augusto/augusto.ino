#include <IRremote.h>

#define AI1 6
#define AI2 5
#define pwmA 10

#define BI1 8
#define BI2 7
#define pwmB 9

#define leftLineSensor A0
#define rightLineSensor A1
int klinija;
int dlinija;

#define kaireStatu 2
#define vidurys 3
#define desineStatu 4

int atstumasKaireStatu;
int atstumasVidurys;
int atstumasDesineStatu;
int a, b; // pulsam analogu 

#define StartModule 11

constexpr uint8_t dataFromStartModule {12};
int code = 0; // taktikos kodas

// konfiguraciniai stuff
int greitis = 100;



void drive(int speedA, int speedB);
void sensors();

void drive(int speedA, int speedB)//speed from -255 to 255; if both speeds = to 0 - stall
{
  if(digitalRead(StartModule))
  {
    if(speedA == 0 && speedB == 0)
    {
      analogWrite(pwmA, 255);//motor A speed
      analogWrite(pwmB, 255);//motor B speed
      digitalWrite(AI1, HIGH);//front pin
      digitalWrite(AI2, HIGH);//back pin
      digitalWrite(BI1, HIGH);//front pin
      digitalWrite(BI2, HIGH);//back pin
    }
    else
    {
      if(speedA>0)
      {
        analogWrite(pwmA, speedA); // KAZKURIAM REIK UZDET SPEED CORRECT
        digitalWrite(AI1, HIGH);
        digitalWrite(AI2, LOW);
      }
      else
      { 
        analogWrite(pwmA, -speedA);
        digitalWrite(AI1, LOW);
        digitalWrite(AI2, HIGH);
      }
      if(speedB>0)
      {
        analogWrite(pwmB, speedB);
        digitalWrite(BI1, HIGH);
        digitalWrite(BI2, LOW);
      }
      else
      { 
        analogWrite(pwmB, -speedB);
        digitalWrite(BI1, LOW);
        digitalWrite(BI2, HIGH);
      }
    }
  }
  else
  {
    //if star module LOW shoot off motors
    analogWrite(pwmA, 0);
    analogWrite(pwmB, 0);

  }
}
  

void sensors()
{
  a = pulseIn(kaireStatu, HIGH);
  atstumasKaireStatu = (a-1000)*2;
  if(atstumasKaireStatu<0){ atstumasKaireStatu = 0; }

  atstumasVidurys = digitalRead(vidurys);

  b = pulseIn(desineStatu, HIGH);
  atstumasDesineStatu = (b-1000)*2;
  if(atstumasDesineStatu<0){ atstumasDesineStatu = 0; }

  

  klinija = analogRead(leftLineSensor);
  dlinija = analogRead(rightLineSensor);
}

void setup() {
  IrReceiver.begin(dataFromStartModule);
  Serial.begin(9600);
  pinMode(leftLineSensor, INPUT);
  pinMode(rightLineSensor, INPUT);
  pinMode(StartModule, INPUT);
  //motorai
  pinMode(pwmA, OUTPUT);
  pinMode(AI1, OUTPUT);
  pinMode(AI2, OUTPUT);

  pinMode(pwmB, OUTPUT);
  pinMode(BI1, OUTPUT);
  pinMode(BI2, OUTPUT);

/*  while(code==0){
    IrReceiver.resume();
    if(IrReceiver.decode()){
      if(IrReceiver.decodedIRData.protocol == NEC){
        code = (IrReceiver.decodedIRData.command);
        break;
      }
    }
  } */
  while(digitalRead(StartModule)==LOW){
    Serial.println("erroras");
    if(digitalRead(StartModule)==HIGH){
      break;
    }
  }
}
int state;
void loop() {
  Serial.print(klinija);
  Serial.print(" ");
  Serial.println(dlinija);
  sensors();
  state=0;
  
    if(klinija<50){
      state=1;
      Serial.println("error1");
    }
    if(dlinija<50){
      state=2;
      Serial.println("error2");
    }
    if(klinija<50 && dlinija<50){
      state=3;
      Serial.println("error3");
    }
    if(atstumasVidurys==0){
      state=4;
      Serial.println("error4");
    }
    if(atstumasKaireStatu<400){
      state=7;
      Serial.println("error5");
    }
    if(atstumasDesineStatu<400){
      state=8;
      Serial.println("error6");
    }

    switch(state){
      case 0:
        drive(70,70);
        break;
      case 1:
        drive(-greitis, -greitis);
        delay(150);
        drive(greitis,-greitis);
        delay(150);
        break;
      case 2:
        drive(-greitis, -greitis);
        delay(150);
        drive(-greitis,greitis);
        delay(150);
        break;
      case 3:
        drive(-greitis,-greitis);
        delay(150);
        drive(greitis,-greitis);
        delay(150);
        break;
      case 4:
        drive(greitis,greitis);
        break;
      case 7:
        while(atstumasVidurys==1){
          sensors();
          drive(greitis/2,-greitis/2);
        }
        drive(0,0);
        delay(20);
        break;
      case 8:
        while(atstumasVidurys==1){
          sensors();
          drive(-greitis/2,greitis/2);
        }
        drive(50,50);
        delay(20);
        break;
    }
  
  
}
