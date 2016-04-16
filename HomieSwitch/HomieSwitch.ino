#include <Homie.h>
#include <RCSwitch.h>
#define MAX_STRING_LEN 25

RCSwitch mySwitch = RCSwitch();
String statusPreamble = "0011001111101011";
int transmitPort = D1;



String pad(String input, int len)
{
  String output = input;
  while(output.length()<len)
  {
    output = "0" + output;
  }
  return output;
}

void controlStatusSwitch(int houseCode, int socketCode, bool turnOn)
{
  String houseCodeString = String(houseCode, BIN);  
  String socketCodeString = String(socketCode, BIN);
  String message = statusPreamble + pad(houseCodeString,4) + turnOn + pad(socketCodeString,3);
  char messageArray[24];
  message.toCharArray(messageArray, 25);
  mySwitch.send(messageArray);
}


HomieNode rcSwitchNode("rcSwitch", "switch", rcSwitchOnHandler, true);

bool rcSwitchOnHandler(String property, String value) {
   int houseCode = 1;
  int socketCode = property.toInt();

  controlStatusSwitch(houseCode,socketCode,value=="true");
  if (value == "true") {    
    Homie.setNodeProperty(rcSwitchNode, property, "true"); // Update the state of the light
    Serial.println("Switch is on");
  } else if (value == "false") {    
   Homie.setNodeProperty(rcSwitchNode, property, "false");
    Serial.println("Switch is off");
  } else {
    return false;
  }
  return true;
}



void setup() {  
  mySwitch.setProtocol(1);
  mySwitch.setRepeatTransmit(10);
  mySwitch.setPulseLength(307); // Bye Bye Standby
  mySwitch.enableTransmit(transmitPort);

  Homie.setFirmware("homie-switch", "1.0.0");
  Homie.registerNode(rcSwitchNode);

  Homie.setup();
}

void loop() {
  Homie.loop(); 
}




