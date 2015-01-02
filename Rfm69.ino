#include <WildFire_RFM69.h>
#define GATEWAYID    1  // this is the address the send sketch sends to
#define RX_NODEID    1  // this is the same address as the GATEWAYID
#define TX_NODEID    99
#define NETWORKID   100
#define FREQUENCY   RF69_433MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define KEY         "thisIsEncryptKey" //has to be same 16 characters/bytes on all nodes, not more not less!
#define LED         6
#define SERIAL_BAUD 115200
#define ACK_TIME    30  // # of ms to wait for an ack

byte sendSize=0;
boolean requestACK = true;
boolean promiscuousMode = false; //set to 'true' to sniff all packets on the same network
WildFire_RFM69 radio;
byte NODEID = 0;
typedef struct {		
  int           nodeId; //store this nodeId
  unsigned long uptime; //uptime in ms
  float         temp;   //temperature maybe?
} Payload;
Payload theData;

void setupRfm69(void){
  wf.begin();  
  Serial.print(F("RFM69 initializion..."));
  char buff[50];
  sprintf(buff, "Listening at %d Mhz, ", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.print(buff);
  Serial.print(F("Node ID = "));
  Serial.print(NODEID);
  Serial.print(F(", Network ID = "));
  Serial.print(NETWORKID);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.setHighPower(); //uncomment only for RFM69HW!
  radio.encrypt(KEY);
  radio.promiscuous(promiscuousMode);
  Serial.println(F("Complete."));
}

void testRfm69transmit(void){
  static uint8_t led_state = 0;
  static boolean setup_complete = false;
  
  if(testRfm69transmit_enabled){
    NODEID = TX_NODEID;
    if(!setup_complete){
      setupRfm69();
      setup_complete = true;
    }
    
    //check for any received packets
    if (radio.receiveDone()){
      Serial.print(F("["));Serial.print(radio.SENDERID, DEC);Serial.print(F("] "));
      for (byte ii = 0; ii < radio.DATALEN; ii++){
        Serial.print((char)radio.DATA[ii]);
      }
      Serial.print(F("   [RX_RSSI:"));Serial.print(radio.readRSSI());Serial.print(F("]"));
  
      if (radio.ACKRequested()){
        radio.sendACK();
        Serial.print(F(" - ACK sent"));
        delay(10);
      }
      Serial.println();
    }    
    
    //fill in the struct with new values
    theData.nodeId = NODEID;
    theData.uptime = millis();
    theData.temp = 91.23; //it's hot!
    
    Serial.print(F("Sending struct ("));
    Serial.print(sizeof(theData));
    Serial.print(F(" bytes) ... "));
    if (radio.sendWithRetry(GATEWAYID, (const void*)(&theData), sizeof(theData))){
      Serial.print(F(" ok!"));
    }
    else{
      Serial.print(F(" nothing..."));
    }
    Serial.println();    
    
    led_state ^= 1;
    digitalWrite(LED, led_state);
  }
  else{
    setup_complete = false; 
  }
}

void testRfm69receive(void){
  static boolean setup_complete = false;
  static byte ackCount = 0;
  
  if(testRfm69receive_enabled){    
    NODEID = RX_NODEID;
    if(!setup_complete){
      setupRfm69();
      setup_complete = true;
    }    
    
    if (radio.receiveDone()){     
      Serial.print(F("["));Serial.print(radio.SENDERID, DEC);Serial.print(F("] "));
      Serial.print(F(" [RX_RSSI:"));Serial.print(radio.readRSSI());Serial.print(F("]"));
      if (promiscuousMode){
        Serial.print(F("to ["));Serial.print(radio.TARGETID, DEC);Serial.print(F("] "));
      }
  	
      if (radio.DATALEN != sizeof(Payload)){
        Serial.print(F("Invalid payload received, not matching Payload struct!"));
      }
      else{
        theData = *(Payload*)radio.DATA; //assume radio.DATA actually contains our struct and not something else
        Serial.print(F(" nodeId="));
        Serial.print(theData.nodeId);
        Serial.print(F(" uptime="));
        Serial.print(theData.uptime);
        Serial.print(F(" temp="));
        Serial.print(theData.temp);
      }
      
      if (radio.ACKRequested())
      {
        byte theNodeID = radio.SENDERID;
        radio.sendACK();
        Serial.print(F(" - ACK sent."));
  
        // When a node requests an ACK, respond to the ACK
        // and also send a packet requesting an ACK (every 3rd one only)
        // This way both TX/RX NODE functions are tested on 1 end at the GATEWAY
        if (ackCount++%3==0)
        {
          Serial.print(F(" Pinging node "));
          Serial.print(theNodeID);
          Serial.print(F(" - ACK..."));
          delay(3); //need this when sending right after reception .. ?
          if (radio.sendWithRetry(theNodeID, "ACK TEST", 8, 0)){  // 0 = only 1 attempt, no retries
            Serial.print(F("ok!"));
          }
          else{
            Serial.print(F("nothing"));
          }
        }
      }
      Serial.println();    
    }
  }
  else{
    setup_complete = false;
  }
}
