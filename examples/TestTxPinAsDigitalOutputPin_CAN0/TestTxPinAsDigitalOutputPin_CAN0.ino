//----------------------------------------------------------------------------------
// This sketch configures CAN0 TxCAN pin (aka PA22, D12) as a digital output port
// and blinks it.
// Note this disturbs the CAN bus, if connected.
//----------------------------------------------------------------------------------

#ifndef ARDUINO_FEATHER_M4_CAN
  #error "This sketch should be compiled for Arduino Feather M4 CAN (SAME51)"
#endif

//----------------------------------------------------------------------------------

static const uint32_t CAN_TX_PIN = 22 ;
static const uint32_t GROUP_PA = 0 ;

//----------------------------------------------------------------------------------

void setup () {
  pinMode (LED_BUILTIN, OUTPUT);

  PORT->Group [GROUP_PA].DIRSET.reg = (1 << CAN_TX_PIN) ;
  PORT->Group [GROUP_PA].PINCFG[CAN_TX_PIN].reg = PORT_PINCFG_INEN ;
}

//----------------------------------------------------------------------------------

static uint32_t gDeadline = 0 ;
static const uint32_t PERIOD = 1000 ;
static bool gPhase = false ;

//----------------------------------------------------------------------------------

void loop () {
  if ((millis () - gDeadline) >= PERIOD) {
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
    gDeadline += PERIOD ;
    gPhase ^= true ;
    if (gPhase) {
      PORT->Group [GROUP_PA].OUTSET.reg = (1 << CAN_TX_PIN) ;
    }else{
      PORT->Group [GROUP_PA].OUTCLR.reg = (1 << CAN_TX_PIN) ; 
    }
  }
}

//----------------------------------------------------------------------------------
