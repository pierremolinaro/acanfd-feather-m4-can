// This sketch tests Transceiver Delay Compensation Value with CAN1
// No external hardware required.
// Testing the Transceiver Delay Compensation Value
// Do not connect CAN network.
//
// Setting the Transceiver Delay Compensation Value is required when
// all the following conditions are met:
//   - transmit CANFD frames
//   - with bit rate swith ;
//   - and data bit rate is > 1 Mbit/s
//
//  The mTransceiverDelayCompensation property of the ACANFD_FeatherM4CAN_Settings class
//  has a default value equal to 4. It seems this value is ok for all bit rates
//  until an 1 Mbit/s nominal bit rate, and a data bit rate factor x8 (8 Mbit/s).
//
//  Correct behaviour expected: when you run this sketch, the Arduino monitor displays:
//            can configuration ok
//            Error count: Tx 0, Rx 0, Transceiver Delay Compensation Value 0
//            Sent 1
//            Error count: Tx 128, Rx 0, Transceiver Delay Compensation Value 8
//            Buffer Full
//            Error count: Tx 128, Rx 0, Transceiver Delay Compensation Value 8
//            Buffer Full
//            Error count: Tx 128, Rx 0, Transceiver Delay Compensation Value 8
//  and both TxCAN and RxCAN are ON.
//  The first time a CANFD frame is submitted, it is immediately emitted. But
//  as the the controller is alone on the bus (no CAN connection), an ACK SLOT
//  occurs. This error increments the Tx Error Count by 8 with the controller is
//  error active, and by 0 when it is error passive. Thus the frame is re-emitted indefinitely.
//  The transmit buffer is never freed.
//
//  Incorrect Transceiver Delay Compensation Value Setting. For exemple,
//  set mTransceiverDelayCompensation to 0, and use fastest bit rate (1 MBit/s, x8).
//  The Arduino monitor displays:
//            can configuration ok
//            Error count: Tx 0, Rx 0, Transceiver Delay Compensation Value 0
//            Sent 1
//            Error count: Tx 256, Rx 0, Transceiver Delay Compensation Value 0
//            Buffer Full
//            Error count: Tx 256, Rx 0, Transceiver Delay Compensation Value 0
//            Buffer Full
//  and both TxCAN and RxCAN are OFF. Note a Tx Error Count equal to 256 means
//  the controller is Bus-Off.
//  What's happening? As the mTransceiverDelayCompensation has an incorrect value
//  the frame cannot be emitted successfully. This error always increments 
//  Tx Error Count by 8 until it reaches 256, at which point the controller
//  becomes bus-off and stops all transmission.
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
//   Maximum size is 4,352 (4,352 32-bit words).
//   A 0 size means the CAN module is not configured; its TxCAN and RxCAN pins
//   can be freely used for an other function.
//   The begin method checks if actual size is greater or equal to required size.

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
  Serial.println ("CANFD loopback test") ;
  ACANFD_FeatherM4CAN_Settings settings (1000 * 1000, DataBitRateFactor::x8) ;

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
  Serial.print ("Transceiver Delay Compensation default value ") ;
  Serial.println (settings.mTransceiverDelayCompensation) ;

  settings.mTransceiverDelayCompensation = 6 ;
  
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

//-----------------------------------------------------------------

void loop () {
  if (gBlinkDate <= millis ()) {
    gBlinkDate += PERIOD ;
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
    CANFDMessage frame ;
    const auto status = can1.getStatus () ;
    Serial.print ("Error count: Tx ") ;
    Serial.print (status.txErrorCount ()) ;
    Serial.print (", Rx ") ;
    Serial.print (status.rxErrorCount ()) ;
    Serial.print (", Transceiver Delay Compensation Value ") ;
    Serial.println (status.transceiverDelayCompensationValue ()) ;
    
    frame.ext = true ;
    frame.type = CANFDMessage::CANFD_WITH_BIT_RATE_SWITCH ;
    frame.id = 0x12345678 ;
    frame.len = 64 ;
    for (uint8_t i=0 ; i<frame.len ; i++) {
      frame.data [i] = i ;
    }
    const uint32_t sendStatus = can1.tryToSendReturnStatusFD (frame) ;
    if (sendStatus == 0) {
      gSentCount += 1 ;
      Serial.print ("Sent ") ;
      Serial.println (gSentCount) ;
    }else{
      Serial.println ("Buffer Full") ;
    }
  }
}

//-----------------------------------------------------------------
