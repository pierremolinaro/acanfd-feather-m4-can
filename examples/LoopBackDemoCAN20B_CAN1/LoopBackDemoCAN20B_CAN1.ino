// CAN1 external LoopBackDemo for Adafruit Feather M4 CAN Express
// No external hardware required.
// You can observe emitted CAN 2.0B frames on CANH / CANL pins.
//-----------------------------------------------------------------

#ifndef ARDUINO_FEATHER_M4_CAN
  #error "This sketch should be compiled for Arduino Feather M4 CAN (SAME51)"
#endif

//-----------------------------------------------------------------

#include <ACANFD_FeatherM4CAN.h>

//-----------------------------------------------------------------

void setup () {
  pinMode (LED_BUILTIN, OUTPUT) ;
  Serial.begin (115200) ;
  while (!Serial) {
    delay (50) ;
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
  }
  Serial.println ("CAN1 loopback test") ;
  ACANFD_FeatherM4CAN_Settings settings (125 * 1000, DataBitRateFactor::x1) ;

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
  if (0 == errorCode) {
    Serial.println ("can configuration ok") ;
  }else{
    Serial.print ("Error can configuration: 0x") ;
    Serial.println (errorCode, HEX) ;
  }
}

//-----------------------------------------------------------------

static const uint32_t PERIOD = 1000 ;
static uint32_t gBlinkDate = PERIOD ;
static uint32_t gSentCount = 0 ;
static uint32_t gReceiveCount = 0 ;

//-----------------------------------------------------------------

void loop () {
  if (gBlinkDate <= millis ()) {
    gBlinkDate += PERIOD ;
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
    CANMessage frame ;
    frame.id = 0x7FF ;
 //   frame.ext = true ;
//    frame.rtr = true ;
    frame.len = 8 ;
    frame.data [0] = 0x11 ;
    frame.data [1] = 0x22 ;
    frame.data [2] = 0x33 ;
    frame.data [3] = 0x44 ;
    frame.data [4] = 0x55 ;
    frame.data [5] = 0x66 ;
    frame.data [6] = 0x77 ;
    frame.data [7] = 0x88 ;
    const uint32_t sendStatus = can1.tryToSendReturnStatusFD (frame) ;
    if (sendStatus == 0) {
      gSentCount += 1 ;
      Serial.print ("Sent ") ;
      Serial.println (gSentCount) ;
    }else{
      Serial.print ("Sent error 0x") ;
      Serial.println (sendStatus) ;    
    }
  }
//--- Receive frame
  CANFDMessage frame ;
  if (can1.receiveFD0 (frame)) {
    gReceiveCount += 1 ;
    Serial.print ("Received ") ;
    Serial.println (gReceiveCount) ;
  }
}

//-----------------------------------------------------------------