// This sketch will send out a Nikon D50 trigger signal (probably works with most Nikons)
// See the full tutorial at http://www.ladyada.net/learn/sensors/ir.html
// this code is public domain, please enjoy!
 
int IRledPin =  13;    // LED connected to digital pin 13
long lowDelay = 530;
long highDelay = 1660;
// The setup() method runs once, when the sketch starts
 
void setup()   {                
  // initialize the IR digital pin as an output:
  pinMode(IRledPin, OUTPUT);      
 
  Serial.begin(9600);
}
 
void loop()                     
{
  if (Serial.available() > 0) {
                // read the incoming byte:
                byte incomingByte = Serial.read();

    Serial.println("Sending IR signal");
    Serial.println(incomingByte,BIN);
    SendPacManCode(incomingByte);
  }
  //delay(60*1000);  // wait one minute (60 seconds * 1000 milliseconds)
  
}
 
// This procedure sends a 38KHz pulse to the IRledPin 
// for a certain # of microseconds. We'll use this whenever we need to send codes
void pulseIR(long microsecs) {
  // we'll count down from the number of microseconds we are told to wait
 
  cli();  // this turns off any background interrupts
 
  while (microsecs > 0) {
    // 38 kHz is about 13 microseconds high and 13 microseconds low
   digitalWrite(IRledPin, HIGH);  // this takes about 3 microseconds to happen
   delayMicroseconds(10);         // hang out for 10 microseconds, you can also change this to 9 if its not working
   digitalWrite(IRledPin, LOW);   // this also takes about 3 microseconds
   delayMicroseconds(10);         // hang out for 10 microseconds, you can also change this to 9 if its not working
 
   // so 26 microseconds altogether
   microsecs -= 26;
  }
 
  sei();  // this turns them back on
}
void sendCode(boolean code)
{
  pulseIR(lowDelay);
  if(code)
  {
    delayMicroseconds(highDelay);
 //Serial.print(1);
  }
  else
  {
    delayMicroseconds(lowDelay);
   //  Serial.print(0);
  }
  
  
}
 
void SendPacManCode(byte b) {
  

  // This is the code for my particular Nikon, for others use the tutorial
  // to 'grab' the proper code from the remote
   int highConst = 530;
  pulseIR(8740);
  delayMicroseconds(4380);
  
  for(int x=0;x<8;x++)
  {
    sendCode(0);
  }
  for(int x=0;x<4;x++)
  {
    sendCode(1);
  }
  sendCode(0);
  for(int x=0;x<3;x++)
  {
    sendCode(1);
  }
  
  for(int x=4;x>=0;x--)
  {
    
    sendCode(1&(b>>x));
  }


  for(int x=0;x<3;x++)
  {
    sendCode(0);
  }  
  
    for(int x=4;x>=0;x--)
  {
    sendCode(!(1&b>>x));
  }
  
  for(int x=0;x<3;x++)
  {
    sendCode(1);
  } 
   pulseIR(lowDelay);
 
}
