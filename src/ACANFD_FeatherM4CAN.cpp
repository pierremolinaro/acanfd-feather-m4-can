//--------------------------------------------------------------------------------------------------
// Page numbers refer to DS60001507G data sheet
//--------------------------------------------------------------------------------------------------

#include <ACANFD_FeatherM4CAN.h>

//--------------------------------------------------------------------------------------------------
//    Constructor
//--------------------------------------------------------------------------------------------------

ACANFD_FeatherM4CAN::ACANFD_FeatherM4CAN (const ACANFD_FeatherM4CAN_Module inModule,
                                          uint32_t * inMessageRAMPtr) :
mModulePtr ((inModule == ACANFD_FeatherM4CAN_Module::can0) ? CAN0 : CAN1),
mMessageRAMPtr (inMessageRAMPtr),
mModule (inModule) {
}

//--------------------------------------------------------------------------------------------------
//    beginFD method
//--------------------------------------------------------------------------------------------------

uint32_t ACANFD_FeatherM4CAN::beginFD (const ACANFD_FeatherM4CAN_Settings & inSettings) {
//                                    const ACANFDFilter inFilters [],
//                                    const uint32_t inFilterCount) {
  uint32_t errorCode = inSettings.CANFDBitSettingConsistency () ; // No error code
//--- No configuration if CAN bit settings are incorrect
  if (!inSettings.mBitSettingOk) {
    errorCode |= kCANBitConfiguration ;
  }
//--- Configure if no error
  if (0 == errorCode) {
  //------------------------------------------------------ Configure Drive Receive buffer 0
    mReceiveBuffer0.initWithSize (inSettings.mDriverReceiveBuffer0) ;
  //------------------------------------------------------ Enable CAN Clock (48 MHz)
    switch (mModule) {
    case ACANFD_FeatherM4CAN_Module::can0 :
      GCLK->PCHCTRL [CAN0_GCLK_ID].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK1 ;
      MCLK->AHBMASK.reg |= MCLK_AHBMASK_CAN0 ;
      break ;
    case ACANFD_FeatherM4CAN_Module::can1 :
      GCLK->PCHCTRL [CAN1_GCLK_ID].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK1 ;
      MCLK->AHBMASK.reg |= MCLK_AHBMASK_CAN1 ;
      break ;
    }
  //------------------------------------------------------ Start configuring CAN module
    mModulePtr->CCCR.reg = CAN_CCCR_INIT ; // Page 1123
    while ((mModulePtr->CCCR.reg & CAN_CCCR_INIT) == 0) {
//      mModulePtr->CCCR.reg = CAN_CCCR_INIT ;
    }
  //------------------------------------------------------ Enable configuration change
    mModulePtr->CCCR.reg = CAN_CCCR_INIT | CAN_CCCR_CCE ;
    mModulePtr->CCCR.reg = CAN_CCCR_INIT | CAN_CCCR_CCE | CAN_CCCR_TEST ;
    uint32_t cccr  = CAN_CCCR_BRSE | CAN_CCCR_FDOE ; // | CAN_CCCR_PXHD ;
  //------------------------------------------------------ Select mode
    mModulePtr->TEST.reg = 0 ;
    switch (inSettings.mModuleMode) {
    case ACANFD_FeatherM4CAN_Settings::NORMAL :
      break ;
    case ACANFD_FeatherM4CAN_Settings::INTERNAL_LOOP_BACK :
      mModulePtr->TEST.reg = CAN_TEST_LBCK ;
      cccr |= CAN_CCCR_MON | CAN_CCCR_TEST ;
      break ;
    case ACANFD_FeatherM4CAN_Settings::EXTERNAL_LOOP_BACK :
      mModulePtr->TEST.reg = CAN_TEST_LBCK ;
      cccr |= CAN_CCCR_TEST ;
      break ;
    }
    if (!inSettings.mEnableRetransmission) {
      cccr |= CAN_CCCR_DAR ; // Page 1123
    }
  //------------------------------------------------------ Set nominal Bit Timing and Prescaler (page 1125)
    mModulePtr->NBTP.reg =
      (uint32_t (inSettings.mArbitrationSJW - 1) << 25)
    |
      (uint32_t (inSettings.mBitRatePrescaler - 1) << 16)
    |
      (uint32_t (inSettings.mArbitrationPhaseSegment1 - 1) << 8)
    |
      (uint32_t (inSettings.mArbitrationPhaseSegment2 - 1) << 0)
    ;
//     Serial.print ("NBTP 0x") ;
//     Serial.println (mModulePtr->NBTP.reg, HEX) ;
  //------------------------------------------------------ Set data Bit Timing and Prescaler (page 1119)
    mModulePtr->DBTP.reg =
 //     ((inSettings.mTransceiverDelayCompensation != 0) ? CAN_DBTP_TDC : 0) // Enable Transceiver Delay Compensation ?
       CAN_DBTP_TDC // Enable Transceiver Delay Compensation ?
    |
      (uint32_t (inSettings.mBitRatePrescaler - 1) << 16)
    |
      (uint32_t (inSettings.mDataPhaseSegment1 - 1) << 8)
    |
      (uint32_t (inSettings.mDataPhaseSegment2 - 1) << 4)
    |
      (uint32_t (inSettings.mDataSJW - 1) << 0)
    ;
  //------------------------------------------------------ Transmitter Delay Compensation
    mModulePtr->TDCR.reg = uint32_t (inSettings.mTransceiverDelayCompensation) << 8 ; // Page 1134
  //------------------------------------------------------ Global Filter Configuration (page 1148)
    mModulePtr->GFC.reg =
      (uint32_t (inSettings.mDiscardReceivedStandardRemoteFrames) << 1)
    |
      (uint32_t (inSettings.mDiscardReceivedExtendedRemoteFrames) << 0)
    ;
  //------------------------------------------------------ Configure message RAM
//    mModulePtr->MRCFG.reg = 3 ; // Page 1118
    uint32_t * ptr = mMessageRAMPtr ;
  //--- Allocate Standard ID Filters (0 ... 128 elements -> 0 ... 128 words)
     mModulePtr->SIDFC.reg = 0 ;
//       (uint32_t (ptr) & 0xFFFFU) // Standard ID Filter Configuration, page 1269
//     |
//       (1U << 16) // One standard filter
//     ;
//     *ptr = (2U << 30) | (1U << 27) ; // Page 1149
//     ptr += 1 ;
  //--- Allocate Extended ID Filters (0 ... 64 elements -> 0 ... 128 words)
    mModulePtr->XIDFC.reg = 0 ; // Page 1150
  //--- Allocate Rx FIFO 0 (0 ... 64 elements -> 0 ... 1152 words)
    mRxFIFO0Pointer = ptr ;
//     Serial.print ("RxFIFO0 ptr 0x") ; Serial.println (uint32_t (ptr), HEX) ;
    mHardwareRxFIFO0Payload = inSettings.mHardwareRxFIFO0Payload ;
    mModulePtr->RXF0C.reg = // Page 1155
      (uint32_t (ptr) & 0xFFFFU) // FOSA
    |
      (uint32_t (inSettings.mHardwareRxFIFO0Size) << 16) // F0S
    ;
    mModulePtr->RXESC.reg |= uint32_t (inSettings.mHardwareRxFIFO0Payload) ; // Rx FIFO 0 element size (page 1285)
    ptr += inSettings.mHardwareRxFIFO0Size * ACANFD_FeatherM4CAN_Settings::wordCountForPayload (inSettings.mHardwareRxFIFO0Payload) ;
  //--- Allocate Rx FIFO 1 (0 ... 64 elements -> 0 ... 1152 words)
    mModulePtr->RXF1C.reg = 0 ; // Page 1159
    ptr += 1152 ;
  //--- Allocate Rx Buffers (0 ... 64 elements -> 0 ... 1152 words)
    mModulePtr->RXESC.reg |= 7 << 8 ; // Reserve memory for 64 byte data field (page 1162)
    mModulePtr->RXBC.reg = uint32_t (ptr) ; // Rx Buffer start address, page 1158
//     Serial.print ("RxBuffer ptr 0x") ; Serial.println (uint32_t (ptr), HEX) ;
    ptr += 18 ; // Advance for 18 words
  //--- Allocate Tx Event / FIFO (0 ... 32 elements -> 0 ... 64 words)
  //--- Allocate Tx Buffers (0 ... 32 elements -> 0 ... 576 words)
   mModulePtr->TXESC.reg = 7 ; // Reserve memory for 64 byte data field (page 1166)
   mTxBuffersPointer = ptr ;
   mModulePtr->TXBC.reg = // Page 1164
      (uint32_t (ptr) & 0xFFFFU) // Tx Buffer start address
    |
      (1 << 16) // Number of Transmit buffers
    ;
//     Serial.print ("TxBuffer ptr 0x") ; Serial.println (uint32_t (ptr), HEX) ;
    ptr += 18 ; // Advance for 18 words
  //------------------------------------------------------ Display Message RAM Allocation
    Serial.print ("Message RAM start 0x") ;
    Serial.println (uint32_t (mMessageRAMPtr), HEX) ;
    Serial.print ("  RxFIFO 0x") ;
    Serial.println (uint32_t (mRxFIFO0Pointer), HEX) ;
    Serial.print ("  TxBuffers 0x") ;
    Serial.println (uint32_t (mTxBuffersPointer), HEX) ;
    const uint32_t minRequiredSize = ptr - mMessageRAMPtr ;
    Serial.print ("  Message RAM Minimum required size ") ;
    Serial.print (minRequiredSize) ;
    Serial.println (" words") ;
  //------------------------------------------------------ Interrupts
    mModulePtr->IE.reg = CAN_IE_RF0NE ; // Receive FIFO 0 Non Empty
    mModulePtr->ILS.reg = 0 ; // All interrupt on EINT0
    switch (mModule) {
    case ACANFD_FeatherM4CAN_Module::can0 :
      NVIC_EnableIRQ (CAN0_IRQn) ;
      break ;
    case ACANFD_FeatherM4CAN_Module::can1 :
      NVIC_EnableIRQ (CAN1_IRQn) ;
      break ;
    }
    mModulePtr->ILE.reg = CAN_ILE_EINT0 ; // Enable Interrupt 0
  //------------------------------------------------------ Select TX ad RX pins
  //  CAN0: PA22 is Tx_CAN, PA23 is Rx_CAN
  //  CAN1: PB14 is Tx_CAN, PB15 is Rx_CAN
    switch (mModule) {
    case ACANFD_FeatherM4CAN_Module::can0 :
      { const uint32_t canTxPin = 22 ;
        const uint32_t canRxPin = 23 ;
        const uint32_t GROUP = 0 ; // PA
        PORT->Group [GROUP].DIRSET.reg = (1 << canTxPin) ;
        PORT->Group [GROUP].DIRCLR.reg = (1 << canRxPin) ;
        PORT->Group [GROUP].PINCFG [canTxPin].reg = PORT_PINCFG_INEN | PORT_PINCFG_PMUXEN ;
        PORT->Group [GROUP].PINCFG [canRxPin].reg = PORT_PINCFG_INEN | PORT_PINCFG_PMUXEN ;
        PORT->Group [GROUP].PMUX [canTxPin / 2].reg = PORT_PMUX_PMUXE (8) | PORT_PMUX_PMUXO (8) ;
      }
      break ;
    case ACANFD_FeatherM4CAN_Module::can1 :
      { const uint32_t canTxPin = 14 ;
        const uint32_t canRxPin = 15 ;
        const uint32_t GROUP = 1 ; // PB
        PORT->Group [GROUP].DIRSET.reg = (1 << canTxPin) ;
        PORT->Group [GROUP].DIRCLR.reg = (1 << canRxPin) ;
        PORT->Group [GROUP].PINCFG [canTxPin].reg = PORT_PINCFG_INEN | PORT_PINCFG_PMUXEN ;
        PORT->Group [GROUP].PINCFG [canRxPin].reg = PORT_PINCFG_INEN | PORT_PINCFG_PMUXEN ;
        PORT->Group [GROUP].PMUX [canTxPin / 2].reg = PORT_PMUX_PMUXE (7) | PORT_PMUX_PMUXO (7) ;
      //--- Activate PB12, for enabling transceiver
        const uint32_t canSilentPin = 12 ;
        PORT->Group [GROUP].DIRSET.reg = (1 << canSilentPin) ;
        PORT->Group [GROUP].OUTCLR.reg = (1 << canSilentPin) ;
        PORT->Group [GROUP].PINCFG [canSilentPin].reg = PORT_PINCFG_INEN ;
      //--- Activate PB13, for enabling BOOST_EN
        const uint32_t boostEnablePin = 13 ;
        PORT->Group [GROUP].DIRSET.reg = (1 << boostEnablePin) ;
        PORT->Group [GROUP].OUTSET.reg = (1 << boostEnablePin) ;
        PORT->Group [GROUP].PINCFG [boostEnablePin].reg = PORT_PINCFG_INEN ;
      }
      break ;
    }
  //------------------------------------------------------ Activate CAN controller
//     mModulePtr->NDAT1.reg = 0xFFFFFFFF;   /* clear new (rx) data flags */
//     mModulePtr->NDAT2.reg = 0xFFFFFFFF;   /* clear new (rx) data flags */
    mModulePtr->CCCR.reg = CAN_CCCR_INIT | CAN_CCCR_CCE | cccr ; // Page 1123
    mModulePtr->CCCR.reg = CAN_CCCR_INIT | cccr ; // Page 1123, reset CCE bit
    mModulePtr->CCCR.reg = cccr ; // Page 1123, reset INIT bit
    while ((mModulePtr->CCCR.reg & CAN_CCCR_INIT) != 0) {
//      mModulePtr->CCCR.reg = cccr ; // Page 1123, reset INIT bit
    }
  }
//--- Return error code (0 --> no error)
  return errorCode ;
}

//--------------------------------------------------------------------------------------------------
//   RECEPTION
//--------------------------------------------------------------------------------------------------

bool ACANFD_FeatherM4CAN::receiveFD (CANFDMessage & outMessage) {
  noInterrupts () ;
    const bool hasMessage = mReceiveBuffer0.remove (outMessage) ;
  interrupts () ;
  return hasMessage ;
}

//--------------------------------------------------------------------------------------------------
//   EMISSION
//--------------------------------------------------------------------------------------------------

uint32_t ACANFD_FeatherM4CAN::tryToSendReturnStatus (const CANMessage & inMessage) {
  uint32_t sendStatus = 0 ;
  const bool txBufferIsFull = (mModulePtr->TXBRP.reg & 1) != 0 ;
  if (txBufferIsFull) {
    sendStatus = kTransmitBufferOverflow ;
  }else{
    if (inMessage.ext) {
      mTxBuffersPointer [0] = (inMessage.id & 0x1FFFFFFFU) | (1U << 30) ; // Page 1179
    }else{
      mTxBuffersPointer [0] = (inMessage.id & 0x7FFU) << 18 ;
    }
    mTxBuffersPointer [0] |= uint32_t (inMessage.rtr) << 29 ;
    const uint32_t length = min (uint32_t (inMessage.len), 8U) ;
    mTxBuffersPointer [1] = length << 16 ;
    mTxBuffersPointer [2] = inMessage.data32 [0] ;
    mTxBuffersPointer [3] = inMessage.data32 [1] ;
//    __DMB () ;
//    Serial.print (" Buff 0x") ; Serial.println (uint32_t (&mTxBuffersPointer [0]), HEX) ;
    mModulePtr->TXBAR.reg = 1 ; // Page 1168
  }
  return sendStatus ;
}

//--------------------------------------------------------------------------------------------------

uint32_t ACANFD_FeatherM4CAN::tryToSendReturnStatusFD (const CANFDMessage & inMessage) {
  uint32_t sendStatus = 0 ;
  const bool txBufferIsFull = (mModulePtr->TXBRP.reg & 1) != 0 ;
  if (txBufferIsFull) {
    sendStatus = kTransmitBufferOverflow ;
  }else{
  //--- Identifier and extended bit
    if (inMessage.ext) {
      mTxBuffersPointer [0] = (inMessage.id & 0x1FFFFFFFU) | (1U << 30) ;
    }else{
      mTxBuffersPointer [0] = (inMessage.id & 0x7FFU) << 18 ;
    }
  //---Control
    uint32_t lengthCode ;
    if (inMessage.len > 48) {
      lengthCode = 15 ;
    }else if (inMessage.len > 32) {
      lengthCode = 14 ;
    }else if (inMessage.len > 24) {
      lengthCode = 13 ;
    }else if (inMessage.len > 20) {
      lengthCode = 12 ;
    }else if (inMessage.len > 16) {
      lengthCode = 11 ;
    }else if (inMessage.len > 12) {
      lengthCode = 10 ;
    }else if (inMessage.len > 8) {
      lengthCode = 9 ;
    }else{
      lengthCode = inMessage.len ;
    }
    mTxBuffersPointer [1] = uint32_t (lengthCode) << 16 ;
  //---
    switch (inMessage.type) {
    case CANFDMessage::CAN_REMOTE :
      mTxBuffersPointer [0] |= 1 << 29 ; // Set RTR bit
      break ;
    case CANFDMessage::CAN_DATA :
      mTxBuffersPointer [2] = inMessage.data32 [0] ;
      mTxBuffersPointer [3] = inMessage.data32 [1] ;
      break ;
    case CANFDMessage::CANFD_NO_BIT_RATE_SWITCH :
      { mTxBuffersPointer [1] |= 1 << 21 ; // Set FDF bit
        const uint32_t wc = (inMessage.len + 3) / 4 ;
        for (uint32_t i=0 ; i<wc ; i++) {
          mTxBuffersPointer [i+2] = inMessage.data32 [i] ;
        }
      }
      break ;
    case CANFDMessage::CANFD_WITH_BIT_RATE_SWITCH :
      { mTxBuffersPointer [1] |= (1 << 21) | (1 << 20) ; // Set FDF and BRS bits
        const uint32_t wc = (inMessage.len + 3) / 4 ;
        for (uint32_t i=0 ; i<wc ; i++) {
          mTxBuffersPointer [i+2] = inMessage.data32 [i] ;
        }
      }
      break ;
    }
//    __DMB () ;
  //---Request transmit
    mModulePtr->TXBAR.reg = 1 ; // Page 1293
  }
  return sendStatus ;
}

//--------------------------------------------------------------------------------------------------
//   INTERRUPT SERVICE ROUTINES
//--------------------------------------------------------------------------------------------------

static void getMessageFrom (uint32_t * inMessageRamAddress,
                            const ACANFD_FeatherM4CAN_Settings::Payload /* inPayLoad */,
                            CANFDMessage & outMessage) {
  const uint32_t w0 = inMessageRamAddress [0] ;
  outMessage.id = w0 & 0x1FFFFFFF ;
  const bool remote = (w0 & (1 << 29)) != 0 ;
  outMessage.ext = (w0 & (1 << 30)) != 0 ;
//   const bool esi = (w0 & (1 << 31)) != 0 ;
  if (!outMessage.ext) {
    outMessage.id >>= 18 ;
  }
  const uint32_t w1 = inMessageRamAddress [1] ;
  const uint32_t dlc = (w1 >> 16) &  0xF ;
  static const uint8_t CANFD_LENGTH_FROM_CODE [16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64} ;
  outMessage.len = CANFD_LENGTH_FROM_CODE [dlc] ;
  const bool fdf = (w1 & (1 << 21)) != 0 ;
  const bool brs = (w1 & (1 << 20)) != 0 ;
  if (fdf) { // CANFD frame
    outMessage.type = brs ? CANFDMessage::CANFD_WITH_BIT_RATE_SWITCH : CANFDMessage::CANFD_NO_BIT_RATE_SWITCH ;
  }else if (remote) {
    outMessage.type = CANFDMessage::CAN_REMOTE ;
  }else{
    outMessage.type = CANFDMessage::CAN_DATA ;
  }
//--- Get data
  if (outMessage.type != CANFDMessage::CAN_REMOTE) {
    const uint32_t wc = (outMessage.len + 3) / 4 ;
    for (uint32_t i=0 ; i<wc ; i++) {
      outMessage.data32 [i] = inMessageRamAddress [i+2] ;
    }
  }
}

//--------------------------------------------------------------------------------------------------

void ACANFD_FeatherM4CAN::interruptServiceRoutine (void) {
  bool loop = true ;
  while (loop) {
    const uint32_t it = mModulePtr->IR.reg ;
    loop = it != 0 ;
    if ((it & CAN_IR_RF0N) != 0) { // Receive FIFO 0 Non Empty
      CANFDMessage message ;
    //--- Get read index
      const uint32_t readIndex = (mModulePtr->RXF0S.reg >> 8) & 0x3F ;
    //--- Compute message RAM address
      uint32_t * address = mRxFIFO0Pointer ;
      address += readIndex * ACANFD_FeatherM4CAN_Settings::wordCountForPayload (mHardwareRxFIFO0Payload) ;
    //--- Get message
      getMessageFrom (address, mHardwareRxFIFO0Payload, message) ;
    //--- Clear receive flag
      mModulePtr->RXF0A.reg = readIndex ;
    //--- Interrupt Acknowledge
      mModulePtr->IR.reg = CAN_IR_RF0N ;
    //--- Enter message into driver receive buffer 0
      mReceiveBuffer0.append (message) ;
    }
  }
}

//--------------------------------------------------------------------------------------------------

ACANFD_FeatherM4CAN::Status::Status (Can * inModulePtr) :
mErrorCount (uint16_t (inModulePtr->ECR.reg)),
mProtocolStatus (inModulePtr->PSR.reg) {
}

//--------------------------------------------------------------------------------------------------
//  CAN0
//--------------------------------------------------------------------------------------------------

static uint32_t gMessageRam0 [4352] ;

ACANFD_FeatherM4CAN can0 (ACANFD_FeatherM4CAN_Module::can0, gMessageRam0) ;

//--------------------------------------------------------------------------------------------------

extern "C" void CAN0_Handler (void) ; // SHOULD HAVE C LINKAGE

void CAN0_Handler (void) {
  can0.interruptServiceRoutine () ;
}

//--------------------------------------------------------------------------------------------------
//  CAN1
//--------------------------------------------------------------------------------------------------

static uint32_t gMessageRam1 [4352] ;

ACANFD_FeatherM4CAN can1 (ACANFD_FeatherM4CAN_Module::can1, gMessageRam1) ;

//--------------------------------------------------------------------------------------------------

extern "C" void CAN1_Handler (void) ; // SHOULD HAVE C LINKAGE

void CAN1_Handler (void) {
  can1.interruptServiceRoutine () ;
}

//--------------------------------------------------------------------------------------------------
