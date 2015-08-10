#include <pt.h>
String reciveMessage;
String MODE = "";
int temp;
int state;
int val;


#define PT_DELAY(pt, ms, ts) \
  ts = millis(); \
  PT_WAIT_WHILE(pt, millis()-ts < (ms));

long duration, distance;

#define A 9
#define B 10
#define WET 11
#define SW1 12
#define SW2 13
#define LDR A1
#define TEM A2

struct pt pt_taskMOTOR;
struct pt pt_taskDATA;
struct pt pt_taskSendSerial;



PT_THREAD(taskMOTOR(struct pt* pt))
{
  static uint32_t ts;
  PT_BEGIN(pt);
  while (1)
  {
    if(reciveMessage[0]=='A') {
      /*if(digitalRead(WET)==0 || analogRead(LDR)<200) {
        if(val!=1) {
          val=1;
          motor(val);
        }
        
      }*/
      motor(1);
    }

   
    if(reciveMessage=="M_0") {
      /*if(val!=1) {
          val=1;
          motor(val);
        }*/
        motor(1);
    }
    if(reciveMessage=="M_1") {
      /*if(val!=0) {
          val=0;
          motor(val);
        }*/
        motor(0);
    }

    if(reciveMessage[0]=='A' && digitalRead(WET)==1 && analogRead(LDR)>200) {
      motor(0);
    }
    PT_DELAY(pt, 300, ts);
  }
  PT_END(pt);
}
PT_THREAD(taskDATA(struct pt* pt))
{
  static uint32_t ts;
  PT_BEGIN(pt);
  while (1)
  {
    if (Serial1.available() > 0) {
      reciveMessage = Serial1.readStringUntil('\r');
      Serial.print("value Recieve : ");
      Serial.println(reciveMessage);
      Serial1.flush();
    }
    PT_DELAY(pt, 300, ts);
  }
  PT_END(pt);
}

PT_THREAD(taskSendSerial(struct pt* pt))
{
  static uint32_t ts;

  PT_BEGIN(pt);

  while (1)
  {
    if(digitalRead(SW2)==1) {
      state=1;
    }
    if(digitalRead(SW1)==1) {
      state=2;
    }
    if(digitalRead(SW1)==0 && digitalRead(SW2)==0) {
      state=0;
    }
    temp=(25*analogRead(TEM) - 2050)/100;
    Serial1.print(String(temp) + "_" + String(digitalRead(WET)) + "_" + reciveMessage[0] + "_" + String(digitalRead(SW1)) + "_" + String(analogRead(LDR)) + "_" + String(state));
    Serial1.print("\r");
    PT_DELAY(pt, 600, ts);
  }

  PT_END(pt);
}

void motor(int value) {
  if(value==0) {
    
    if(digitalRead(SW1)==0) {
      digitalWrite(A,0);
      digitalWrite(B,1);
    }
    if(digitalRead(SW1)==1) {
      digitalWrite(A,1);
      digitalWrite(B,1);
      digitalWrite(A,0);
      digitalWrite(B,0);
    }
  }
  if(value==1) {
    
    if(digitalRead(SW2)==0) {
      digitalWrite(A,1);
      digitalWrite(B,0);
    }
    if(digitalRead(SW2)==1) {
      digitalWrite(A,1);
      digitalWrite(B,1);
      digitalWrite(A,0);
      digitalWrite(B,0);
    }
  }
}

    
void setup() {
  
  Serial.begin(115200); // usb
  Serial1.begin(115200);
  pinMode(12,INPUT);
  pinMode(13,INPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  PT_INIT(&pt_taskMOTOR);
  PT_INIT(&pt_taskDATA);
  PT_INIT(&pt_taskSendSerial);
}

void loop() {
  taskMOTOR(&pt_taskMOTOR);
  taskDATA(&pt_taskDATA);
  taskSendSerial(&pt_taskSendSerial);
}
