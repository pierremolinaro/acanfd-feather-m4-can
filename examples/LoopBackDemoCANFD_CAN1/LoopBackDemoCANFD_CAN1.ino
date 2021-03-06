// CAN1 external LoopBackDemo for Adafruit Feather M4 CAN Express
// No external hardware required.
// You can observe emitted CANFD frames on CANH / CANL pins.
//-----------------------------------------------------------------

#ifndef ARDUINO_FEATHER_M4_CAN
  #error "This sketch should be compiled for Arduino Feather M4 CAN (SAME51)"
#endif

//-----------------------------------------------------------------
// IMPORTANT:
//   <ACANFD_FeatherM4CAN.h> should be included only from the .ino file
//   From an other file, include <ACANFD_FeatherM4CAN-from-cpp.h>
//   Before including <ACANFD_FeatherM4CAN.h>, you should define 
//   Message RAM size for CAN0 and Message RAM size for CAN1.
//   Maximum required size is 4,352 (4,352 32-bit words).
//   A 0 size means the CAN module is not configured; its TxCAN and RxCAN pins
//   can be freely used for an other function.
//   The begin method checks if actual size is greater or equal to required size.
//   Hint: if you do not want to compute required size, print
//   can1.messageRamRequiredMinimumSize () for getting it.

#define CAN0_MESSAGE_RAM_SIZE (0)
#define CAN1_MESSAGE_RAM_SIZE (1728)

#include <ACANFD_FeatherM4CAN.h>

//-----------------------------------------------------------------

void setup () {
  pinMode (LED_BUILTIN, OUTPUT) ;
  Serial.begin (115200) ;
  while (!Serial) {
    delay (50) ;
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
  }
  Serial.println ("CAN1 CANFD loopback test") ;
  ACANFD_FeatherM4CAN_Settings settings (500 * 1000, DataBitRateFactor::x4) ;

  Serial.print ("Bit Rate prescaler: ") ;
  Serial.println (settings.mBitRatePrescaler) ;
  Serial.print ("Arbitration Phase segment 1: ") ;
  Serial.println (settings.mArbitrationPhaseSegment1) ;
  Serial.print ("Arbitration Phase segment 2: ") ;
  Serial.println (settings.mArbitrationPhaseSegment2) ;
  Serial.print ("Arbitration SJW: ") ;
  Serial.println (settings.mArbitrationSJW) ;
  Serial.print ("Actual Arbitration Bit Rate: ") ;
  Serial.print (settings.actualArbitrationBitRate ()) ;
  Serial.println (" bit/s") ;
  Serial.print ("Arbitration Sample point: ") ;
  Serial.print (settings.arbitrationSamplePointFromBitStart ()) ;
  Serial.println ("%") ;
  Serial.print ("Exact Arbitration Bit Rate ? ") ;
  Serial.println (settings.exactArbitrationBitRate () ? "yes" : "no") ;
  Serial.print ("Data Phase segment 1: ") ;
  Serial.println (settings.mDataPhaseSegment1) ;
  Serial.print ("Data Phase segment 2: ") ;
  Serial.println (settings.mDataPhaseSegment2) ;
  Serial.print ("Data SJW: ") ;
  Serial.println (settings.mDataSJW) ;
  Serial.print ("Actual Data Bit Rate: ") ;
  Serial.print (settings.actualDataBitRate ()) ;
  Serial.println (" bit/s") ;
  Serial.print ("Data Sample point: ") ;
  Serial.print (settings.dataSamplePointFromBitStart ()) ;
  Serial.println ("%") ;
  Serial.print ("Exact Data Bit Rate ? ") ;
  Serial.println (settings.exactDataBitRate () ? "yes" : "no") ;

  settings.mModuleMode = ACANFD_FeatherM4CAN_Settings::EXTERNAL_LOOP_BACK ;

  const uint32_t errorCode = can1.beginFD (settings) ;

  Serial.print ("Message RAM required minimum size: ") ;
  Serial.print (can1.messageRamRequiredMinimumSize ()) ;
  Serial.println (" words") ;

  if (0 == errorCode) {
    Serial.println ("can configuration ok") ;
  }else{
    Serial.print ("Error can configuration: 0x") ;
    Serial.println (errorCode, HEX) ;
  }
}

//-----------------------------------------------------------------

static uint32_t pseudoRandomValue (void) {
  static uint32_t gSeed = 0 ;
  gSeed = 8253729U * gSeed + 2396403U ;
  return gSeed ;
}

//-----------------------------------------------------------------

static const uint32_t PERIOD = 1000 ;
static uint32_t gBlinkDate = PERIOD ;
static uint32_t gSentCount = 0 ;
static uint32_t gReceiveCount = 0 ;
static CANFDMessage gSentFrame ;
static bool gOk = true ;

//-----------------------------------------------------------------

void loop () {
  if (gBlinkDate <= millis ()) {
    gBlinkDate += PERIOD ;
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
    if (gOk) {
      const uint32_t r = pseudoRandomValue () ;
      gSentFrame.idx = 0 ;
      gSentFrame.ext = (r & (1 << 29))  != 0 ;
      gSentFrame.type = CANFDMessage::Type (r >> 30) ;
      gSentFrame.id = r & 0x1FFFFFFF ;
      if (!gSentFrame.ext) {
        gSentFrame.id &= 0x7FF ;
      }
      switch (gSentFrame.type) {
        case CANFDMessage::CAN_REMOTE :
          gSentFrame.len = pseudoRandomValue () % 9 ;
          break ;
        case CANFDMessage::CAN_DATA :
          gSentFrame.len = pseudoRandomValue () % 9 ;
          for (uint32_t i = 0 ; i < gSentFrame.len ; i++) {
            gSentFrame.data [i] = uint8_t (pseudoRandomValue ()) ;
          }
          break ;
        case CANFDMessage::CANFD_NO_BIT_RATE_SWITCH :
        case CANFDMessage::CANFD_WITH_BIT_RATE_SWITCH :
          gSentFrame.len = pseudoRandomValue () % 9 ;
          for (uint32_t i = 0 ; i < gSentFrame.len ; i++) {
            gSentFrame.data [i] = uint8_t (pseudoRandomValue ()) ;
          }
          break ;
      }
      const uint32_t sendStatus = can1.tryToSendReturnStatusFD (gSentFrame) ;
      if (sendStatus == 0) {
        gSentCount += 1 ;
        Serial.print ("Sent ") ;
        Serial.print (gSentCount) ;
        Serial.print (", idf 0x") ;
        Serial.print (gSentFrame.id, HEX) ;
        Serial.print (", type ") ;
        Serial.print (gSentFrame.type) ;
        Serial.print (" (") ;
        switch (gSentFrame.type) {
        case CANFDMessage::CAN_REMOTE :
          Serial.print ("remote") ;
          break ;
        case CANFDMessage::CAN_DATA :
          Serial.print ("CAN 2.0B data") ;
          break ;
        case CANFDMessage::CANFD_NO_BIT_RATE_SWITCH :
          Serial.print ("CANFD, no BRS") ;
          break ;
        case CANFDMessage::CANFD_WITH_BIT_RATE_SWITCH :
          Serial.print ("CANFD, BRS") ;
          break ;
        }
        Serial.print ("), length ") ;
        Serial.println (gSentFrame.len) ;
      }else{
        Serial.print ("Sent error 0x") ;
        Serial.println (gSentFrame.type) ;
      }
    }
  }
  //--- Receive frame
  CANFDMessage frame ;
  if (gOk && can1.receiveFD0 (frame)) {
    bool sameFrames = gSentFrame.id == frame.id ;
    if (sameFrames) {
      sameFrames = gSentFrame.type == frame.type ;
    }
    if (sameFrames) {
      sameFrames = gSentFrame.len == frame.len ;
    }
    if (gSentFrame.type != CANFDMessage::CAN_REMOTE) {
      for (uint32_t i = 0 ; (i < frame.len) && sameFrames ; i++) {
        sameFrames = gSentFrame.data [i] == frame.data [i] ;
      }
    }
    if (sameFrames) {
      gReceiveCount += 1 ;
      Serial.print ("Received ") ;
      Serial.println (gReceiveCount) ;
    }else{
      gOk = false ;
      Serial.println ("Receive error") ;
      Serial.print ("  IDF: 0x") ;
      Serial.print (gSentFrame.id, HEX) ;
      Serial.print (" :: 0x") ;
      Serial.println (frame.id, HEX) ;
      Serial.print ("  TYPE: ") ;
      Serial.print (gSentFrame.type) ;
      Serial.print (" :: ") ;
      Serial.println (frame.type) ;
      Serial.print ("  LENGTH: ") ;
      Serial.print (gSentFrame.len) ;
      Serial.print (" :: ") ;
      Serial.println (frame.len) ;
    }
  }
}

//-----------------------------------------------------------------
