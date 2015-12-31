#define analogPin A0 // RF data pin = Analog pin 0
 
byte dataBuffer[40];
 
int dataCounter = 0;
int lowDataCounter = 0;
int highDataCounter = 0;
int maxSignalLength = 30;
 
const unsigned int upperThreshold = 100;
const unsigned int lowerThreshold = 80;

int ding[] = {2,2,2,1,3,4,1,3,4,1,4,1,3,4,1,4,2,2,1,4};

void setup() {
 
  Serial.begin(115200);
  
}
 
void loop() { 
    lowDataCounter = 0;
    while (analogRead(analogPin) > upperThreshold)
      lowDataCounter++;
   
   
    // HIGH signal
    highDataCounter = 0;
    while(analogRead(analogPin) < lowerThreshold)
      highDataCounter++;
    

    if(lowDataCounter >=2 && lowDataCounter <=5)
    {
      if(highDataCounter >= 25 && highDataCounter <= 27)
      {
         //This could be it, now to capture the rest of the signal.

         capture();
         
      }
    }

    
  
  }

void capture()
{
  
  byte found=1;
  int mistakes =0;
 Serial.println("in here!"); 
  // got HIGH; read the rest of the data into dataBuffer
  for (int i = 0; i < sizeof(dataBuffer); i = i+2) {
   
    // LOW signals
    lowDataCounter = 0;
    while (analogRead(analogPin) > upperThreshold && lowDataCounter < maxSignalLength)
      lowDataCounter++;
   
    dataBuffer[i] = dataCounter;
   
    // HIGH signal
    highDataCounter = 0;
    while(analogRead(analogPin) < lowerThreshold && highDataCounter < maxSignalLength)
      highDataCounter++;
     
      int number = getNumber(lowDataCounter,highDataCounter);
    Serial.print(number);    
     if(ding[i/2]!=number)
     {
        if(i==0 && number==4)
        {
          //this can pass
        }
        else
        {
       //   Serial.println("WRONG");
          found=0;
          //break;     
          mistakes ++;
        }
        
     }     
  }
  
  
  if(found==1)
     {
      Serial.println("YAY!");    
     }
     else
     {
      Serial.print("Found mistakes: ");
      Serial.println(mistakes);
     }
}

int getNumber(byte low, byte high)
{
  if(translateValue(low)==0)
  {
    if(translateValue(high)==0)
    {
      //0,0 == 1
      return 1;
    }
    else if(translateValue(high)==1)
    {
      //0,1==2
      return 2;
    }
  }
  else
  {
    if(translateValue(high)==0)
    {
      //1,0 == 3
      return 3;
    }
    else if(translateValue(high)==1)
    {
      //1,1==4
      return 4;
    }
  }
  return 9;
}

int translateValue(byte in)
{
  if(in  >2 && in < 5)
  {
    return 0;
  }
  if(in > 6 && in < 10)
  {
  return 1;
  }
  return -1;
}


