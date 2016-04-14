/**
 * Nanode RF BBSB online controller simulator
 * This allows you to use tools that can talk to thte Bye bye standbye contoller, without having to buy the online controller.
 * Note this code is designed to work with a nanode RF, with a seperate 433 transmitter (not the transmitter on-board the device).
 * This code is based around the EtherCard TFTP sample (https://github.com/thiseldo/EtherCardExamples/blob/master/EtherCard_TFTPServer/EtherCard_TFTPServer.ino)
 * and the following home easy controller software: http://arduino.cc/playground/Code/HomeEasy?action=sourceblock&num=3
 * My nanode is running the default bootloader. The TX pin of the 433 transmitter should be plugged into pin A0.
 * Please let me know if you find this code useful (first published open source code!)
 */

//#undef DEBUG
#define DEBUG

/*
 * Arduino ENC28J60 Ethernet shield UDP broadcast client
 */
#include "EtherCard.h"




// 
// Please modify the following lines. mac and ip have to be unique
// in your local area network. You can not have the same numbers in
// two devices:
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
static byte hisip[] = { 192,168,1,14 };
const char website[] PROGMEM = "192.168.1.14";
// TFTP port
#define LISTEN_UDP 53008 //The required UDP port for BBSB

#define UDP_DATA_START 0x2c //Indicates where the start of the data in the udp packet is

// Packet buffer, must be big enough for packet and 512b data plus IP/UDP headers
#define BUFFER_SIZE 650

byte Ethernet::buffer[BUFFER_SIZE];


//Used by the BBSB transmit function
int txPin = A0;
int onPin = 6;
int offPin = 5;

boolean houseCode1 = false;
boolean houseCode2 = false;
boolean houseCode3 = false;
boolean houseCode4 = false;

boolean unitCode1 = false;
boolean unitCode2 = false;
boolean unitCode3 = false;
boolean unitCode4 = false;

bool previousActive = false;
bool currentActive = false;

void setup() {
  //This is used to disable all devices not required - see here: https://sites.google.com/site/slangey/misc/nanode-rf under NanodeRF and some sketches running slowly
  DDRB |=0x7; PORTB |= 0x7;
  
   pinMode(txPin, OUTPUT);
   pinMode(onPin, OUTPUT);
   pinMode(offPin, OUTPUT);
   
#ifdef DEBUG
  Serial.begin(57600);
  Serial.println("BBSB Controller");
#endif

  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
    Serial.println( "Failed to access Ethernet controller");
 if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));
//   ether.staticSetup(myip, gwip);
     ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  
  //ether.copyIp(ether.hisip, hisip);
      if (!ether.dnsLookup(website))
    Serial.println("DNS failed");
  ether.printIp("SRV: ", ether.hisip);
  
   ether.hisport = 8080;

  Serial.print("Port ");
 Serial.println(ether.hisport);

 while (ether.clientWaitingGw())
    ether.packetLoop(ether.packetReceive());
  Serial.println("Gateway found");
 /* 
   I found this just locked the device up in certain cases
   while (ether.clientWaitingGw())
    ether.packetLoop(ether.packetReceive());
  
  Serial.println("Gateway found");
*/
#ifdef DEBUG
  Serial.println("Waiting for request");
#endif
}
static uint32_t timer;

int sensorValue = 0;
int count =0;
int readings = 500;
// Main loop, waits for tftp request and allows file to be uploaded to SRAM
void loop(){
  int plen = ether.packetReceive();

  if( plen > 0 ) {
    if(isBBSBCommand(Ethernet::buffer))
    {
    
      boolean turn_on = Ethernet::buffer[UDP_DATA_START] &1; 
      char channel = char(Ethernet::buffer[UDP_DATA_START+1]);
      byte number = ((Ethernet::buffer[UDP_DATA_START+2]-48)*10) + Ethernet::buffer[UDP_DATA_START+3] - 48 ;//This converts a char number into a byte. e.g. 16 as a byte == 49,54 (49=='1',54=='6') - dum I know.
      sendBBSBCommand(turn_on, channel, number);
    }
 
  }
  //clear the buffer
  ether.packetLoop(plen);

 
  
  
    int value = analogRead(A1);    
    if(value>700)
    {
      
      sensorValue += 1;
     
    }
     count++;    
    if(sensorValue>150)
    {
    Serial.print(sensorValue);
    Serial.print(": ");
    Serial.println(value);
    }
    if(count==readings)
    {
          CheckDoorbell();
          sensorValue=0;
          count=0;
    }

     

  //  if (millis() > timer) {
    //timer = millis() + 5000;
    //Serial.println();
    //Serial.print("<<< REQ ");
    //ether.browseUrl(PSTR("/CMD"), "?Door_Bell=ON", website, my_callback);
//  }
}
void my_callback (byte status, word off, word len) {
  Serial.println(">>>");
  Ethernet::buffer[off+300] = 0;
  Serial.print((const char*) Ethernet::buffer + off);
  Serial.println("...");
}
void CheckDoorbell()
{

   
  // print out the value you read:
  currentActive = sensorValue == readings;
  
  if(previousActive != currentActive)
  {
    if(currentActive)
    {      
  Serial.println("Ding Dong!");    
    ether.browseUrl(PSTR("/CMD"), "?Door_Bell=ON", website, my_callback);
      Serial.println("sent");
    }
    else
    {
      Serial.println("Off");
    ether.browseUrl(PSTR("/CMD"), "?Door_Bell=OFF", website, my_callback);
    }

    previousActive = currentActive;
  }
}

//Verifies that the udp port passed in is actually a BBSB command
boolean isBBSBCommand( uint8_t *buf ) {
  //Combine the high part of the UDP port with the low part to get the full port number
  word destinationPort =  word(Ethernet::buffer[UDP_DST_PORT_H_P],Ethernet::buffer[UDP_DST_PORT_L_P]);
  //Now check if it's the correct udp protocol (I think) and check the ports match.
  return ( buf[IP_PROTO_P] == IP_PROTO_UDP_V 
  && destinationPort == LISTEN_UDP);
}

void sendBBSBCommand(boolean turn_on, char channel, byte number)
{
   
  #ifdef DEBUG
  if(turn_on)
  {
     Serial.print("On: ");
  }
  else
  {
     Serial.print("Off: "); 
  }
  Serial.print (channel);
  Serial.print(",");
  Serial.println(number);
  #endif
  
  SetCode(channel,number);
  
  transmit(turn_on);
  
  if(turn_on)
  {
   digitalWrite(onPin,HIGH); 
   digitalWrite(offPin,LOW); 
  }
  else
  {
    digitalWrite(onPin,LOW); 
   digitalWrite(offPin,HIGH); 
  }
}






//Taken from: 

void sendBit(boolean b) {
  if (b) {
    digitalWrite(txPin, HIGH);
    delayMicroseconds(1125);
    digitalWrite(txPin, LOW);
    delayMicroseconds(375);
  }
  else {
    digitalWrite(txPin, HIGH);
    delayMicroseconds(375);
    digitalWrite(txPin, LOW);
    delayMicroseconds(1125);
  }
}

void sendPair(boolean b) {
  sendBit(false);
  sendBit(b);
}

void switchcode(boolean b)
{

  // house code 1 = B
  sendPair(houseCode1);
  sendPair(houseCode2);
  sendPair(houseCode3);
  sendPair(houseCode4);

  // unit code 2
  sendPair(unitCode1);
  sendPair(unitCode2);
  sendPair(unitCode3);
  sendPair(unitCode4);

  // on = 14
  sendPair(false);
  sendPair(true);
  sendPair(true);
  sendPair(b);

  sendBit(false);  
}

void transmit(boolean b) {
  switchcode(b);
  delayMicroseconds(10000);
  switchcode(b);
  delayMicroseconds(10000);
  switchcode(b);
}

void SetCode(char houseCode, int unitCode)
{
    byte bhouseCode= byte(houseCode-65);
    int bitMask = 1;
    houseCode1 = bitMask & bhouseCode;
    unitCode1 = bitMask & unitCode;
    bitMask = bitMask<<1;
    houseCode2 = bitMask & bhouseCode;
    unitCode2 = bitMask & unitCode;
    bitMask = bitMask<<1;
    houseCode3 = bitMask & bhouseCode;
    unitCode3 = bitMask & unitCode;
    bitMask = bitMask<<1;
    houseCode4 = bitMask & bhouseCode;
    unitCode4 = bitMask & unitCode;
}



