#include <Homie.h>

int redLed = D5;
int greenLed = D6;
int blueLed = D7;

HomieNode rgbNode("rgbNode", "rgb");
bool Red(String value) {
  Homie.setNodeProperty(rgbNode, "Red", value); 
  return handleColour(redLed,value);
}
bool Green(String value) {
  Homie.setNodeProperty(rgbNode, "Green", value); 
  return handleColour(greenLed,value);
}
bool Blue(String value) {
    Homie.setNodeProperty(rgbNode, "Blue", value); 
  return handleColour(blueLed,value);
}

bool handleColour(int pin, String value)
{
  int iValue = (value.toFloat() / 100 )*255;
  Serial.print("value ");//
  Serial.print(iValue);//+ pin.toString() + " updated to " + iValue.toString());
  analogWrite(pin, iValue);
  return true;
}

void setup() {  
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  Homie.setFirmware("homie-rgb", "1.0.0");
  Homie.registerNode(rgbNode);
  rgbNode.subscribe("Red",Red);
  rgbNode.subscribe("Green",Green);
  rgbNode.subscribe("Blue",Blue);  

  Homie.setup();
}

void loop() {
  Homie.loop(); 
}




