#include <Servo.h>

Servo finger1;
Servo finger2;
Servo finger345;

int amin = 180;
int amax = 0;

void setup() {
  Serial.begin(9600);
  finger1.attach(6);
  finger2.attach(9);
  finger345.attach(3);

  finger1.write(amax);
  finger2.write(amin);
  finger345.write(amin);
  // put your setup code here, to run once:

}

void loop() {
//if(Serial.available()){
  int inputbyte = Serial.read();
  

  if (inputbyte == 'r')
  {
    finger1.write(amin);
    finger2.write(amax);
    finger345.write(amax);
  }
  
  if (inputbyte == 's')
  {
    finger1.write(amax);
    finger2.write(amin);
    finger345.write(amax
    );
  }
  
  if (inputbyte == 'p')
  {
    finger1.write(amax);
    finger2.write(amin);
    finger345.write(amin);
  }
  
}
