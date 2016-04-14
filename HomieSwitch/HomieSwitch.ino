#include <Homie.h>
#include <RCSwitch.h>
#define MAX_STRING_LEN 25

RCSwitch mySwitch = RCSwitch();
String statusPreamble = "0011001111101011";




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


HomieNode rcSwitchNode("rcSwitch", "switch", rcSwitchOnHandler);
bool rcSwitchOnHandler1(String value) {
  return rcSwitchOnHandler("1",value);
}
bool rcSwitchOnHandler2(String value) {
  return rcSwitchOnHandler("2",value);
}
bool rcSwitchOnHandler3(String value) {
  return rcSwitchOnHandler("3",value);
}
bool rcSwitchOnHandler4(String value) {
  return rcSwitchOnHandler("4",value);
}
bool rcSwitchOnHandler5(String value) {
  return rcSwitchOnHandler("5",value);
}

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
  mySwitch.enableTransmit(5);

  Homie.setFirmware("homie-switch", "1.0.0");
  Homie.registerNode(rcSwitchNode);
  rcSwitchNode.subscribe("1",rcSwitchOnHandler1);
  rcSwitchNode.subscribe("2",rcSwitchOnHandler2);
  rcSwitchNode.subscribe("3",rcSwitchOnHandler3);
  rcSwitchNode.subscribe("4",rcSwitchOnHandler4);
  rcSwitchNode.subscribe("5",rcSwitchOnHandler5);

  Homie.setup();
}

void loop() {
  Homie.loop(); 
}




