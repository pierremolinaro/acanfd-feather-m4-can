// CAN1 external LoopBackDemo for Adafruit Feather M4 CAN Express
// No external hardware required.
// You can observe emitted CAN 2.0B and CANFD frames on CANH / CANL pins.
//-----------------------------------------------------------------

#ifndef ARDUINO_FEATHER_M4_CAN
  #error "This sketch should be compiled for Arduino Feather M4 CAN (SAME51)"
#endif

//-----------------------------------------------------------------

#include <ACANFD_FeatherM4CAN.h>

//-----------------------------------------------------------------

static ACANFD_FeatherM4CAN_Buffer16 gBuffer ;

//-----------------------------------------------------------------

void setup () {
  gBuffer.initWithSize (100) ;
  pinMode (LED_BUILTIN, OUTPUT) ;
  Serial.begin (115200) ;
  while (!Serial) {
    delay (50) ;
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
  }
  Serial.println ("CAN1 CANFD loopback test") ;
  ACANFD_FeatherM4CAN_Settings settings (1000 * 1000, DataBitRateFactor::x8) ;

  settings.mModuleMode = ACANFD_FeatherM4CAN_Settings::EXTERNAL_LOOP_BACK ;

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

  const uint32_t errorCode = can1.beginFD (settings) ;
  if (0 == errorCode) {
    Serial.println ("can configuration ok") ;
  }else{
    Serial.print ("Error can configuration: 0x") ;
    Serial.println (errorCode, HEX) ;
  }
}

//-----------------------------------------------------------------

static uint32_t gSeed = 0 ;

static uint32_t pseudoRandomValue (void) {
  gSeed = 8253729U * gSeed + 2396403U ;
  return gSeed ;
}

//-----------------------------------------------------------------

static const uint32_t PERIOD = 1000 ;
static uint32_t gBlinkDate = PERIOD ;
static uint32_t gSentCount = 0 ;
static uint32_t gReceiveCount = 0 ;
static bool gOk = true ;
static uint32_t gCANRemoteFrameCount = 0 ;
static uint32_t gCanDataFrameCount = 0 ;
static uint32_t gCanFDNoBRSDataFrameCount = 0 ;
static uint32_t gCanFDWithBRSDataFrameCount = 0 ;
static uint32_t gStandardFrameCount = 0 ;
static uint32_t gExtendedFrameCount = 0 ;

//-----------------------------------------------------------------

static const uint8_t CANFD_LENGTH_FROM_CODE [16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64} ;

//-----------------------------------------------------------------

void loop () {
  if (gBlinkDate <= millis ()) {
    gBlinkDate += PERIOD ;
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
    Serial.print ("Sent ") ;
    Serial.print (gSentCount) ;
    Serial.print (", received ") ;
    Serial.print (gReceiveCount) ;
    Serial.print (" (") ;
    Serial.print (gCANRemoteFrameCount) ;
    Serial.print (", ") ;
    Serial.print (gCanDataFrameCount) ;
    Serial.print (", ") ;
    Serial.print (gCanFDNoBRSDataFrameCount) ;
    Serial.print (", ") ;
    Serial.print (gCanFDWithBRSDataFrameCount) ;
    Serial.print (", ") ;
    Serial.print (gStandardFrameCount) ;
    Serial.print (", ") ;
    Serial.print (gExtendedFrameCount) ;
    Serial.println (")") ;
  }
//--- Send buffer index
//    0: fifo
//    1 ... settings.mHardwareDedicacedTxBufferCount: dedicaced buffer
  const uint8_t sendBufferIndex = 0 ;
//--- Send frame
  if (gOk && !gBuffer.isFull () && can1.sendBufferNotFullForIndex (sendBufferIndex)) {
    CANFDMessage frame ;
    frame.idx = sendBufferIndex ;
    const uint32_t r = pseudoRandomValue () ;
    frame.ext = (r & (1 << 29))  != 0 ;
    frame.type = CANFDMessage::Type (r >> 30) ;
    frame.id = r & 0x1FFFFFFF ;
    if (frame.ext) {
      gExtendedFrameCount += 1 ;
    }else{
      gStandardFrameCount += 1 ;
      frame.id &= 0x7FF ;
    }
    switch (frame.type) {
    case CANFDMessage::CAN_REMOTE :
      gCANRemoteFrameCount += 1 ;
      frame.len = pseudoRandomValue () % 9 ;
      break ;
    case CANFDMessage::CAN_DATA :
      gCanDataFrameCount += 1 ;
      frame.len = pseudoRandomValue () % 9 ;
      for (uint32_t i=0 ; i<frame.len ; i++) {
        frame.data [i] = uint8_t (pseudoRandomValue ()) ;
      }
      break ;
    case CANFDMessage::CANFD_NO_BIT_RATE_SWITCH :
      gCanFDNoBRSDataFrameCount += 1 ;
      frame.len = CANFD_LENGTH_FROM_CODE [pseudoRandomValue () & 0xF] ;
      for (uint32_t i=0 ; i<frame.len ; i++) {
        frame.data [i] = uint8_t (pseudoRandomValue ()) ;
      }
      break ;  
    case CANFDMessage::CANFD_WITH_BIT_RATE_SWITCH :
      gCanFDWithBRSDataFrameCount += 1 ;
      frame.len = CANFD_LENGTH_FROM_CODE [pseudoRandomValue () & 0xF] ;
      for (uint32_t i=0 ; i<frame.len ; i++) {
        frame.data [i] = uint8_t (pseudoRandomValue ()) ;
      }
      break ;  
    }
    gBuffer.append (frame) ;
    const uint32_t sendStatus = can1.tryToSendReturnStatusFD (frame) ;
    if (sendStatus == 0) {
      gSentCount += 1 ;
    }else{
      gOk = false ;
      Serial.print ("Send status error 0x") ;
      Serial.println (sendStatus, HEX) ;
    }
  }
//--- Receive frame
  CANFDMessage receivedFrame ;
  if (gOk && can1.receiveFD0 (receivedFrame)) {
    CANFDMessage storedFrame ;
    gBuffer.remove (storedFrame) ;
    gReceiveCount += 1 ;
    bool sameFrames = storedFrame.id == receivedFrame.id ;
    if (sameFrames) {
      sameFrames = storedFrame.type == receivedFrame.type ;
    }
    if (sameFrames) {
      sameFrames = storedFrame.len == receivedFrame.len ;
    }
    if (storedFrame.type != CANFDMessage::CAN_REMOTE) {
      for (uint32_t i=0 ; (i<receivedFrame.len) && sameFrames ; i++) {
        sameFrames = storedFrame.data [i] == receivedFrame.data [i] ;
      }
    }
    if (!sameFrames) {
      gOk = false ;
      Serial.println ("Receive error") ;
      Serial.print ("  IDF: 0x") ;
      Serial.print (storedFrame.id, HEX) ;
      Serial.print (" :: 0x") ;
      Serial.println (receivedFrame.id, HEX) ;
      Serial.print ("  TYPE: ") ;
      Serial.print (storedFrame.type) ;
      Serial.print (" :: ") ;
      Serial.println (receivedFrame.type) ;
      Serial.print ("  LENGTH: ") ;
      Serial.print (storedFrame.len) ;
      Serial.print (" :: ") ;
      Serial.println (receivedFrame.len) ;     
    }
  }
}

//-----------------------------------------------------------------
