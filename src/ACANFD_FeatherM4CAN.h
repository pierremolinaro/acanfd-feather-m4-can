//--------------------------------------------------------------------------------------------------

#pragma once

//--------------------------------------------------------------------------------------------------

#ifndef ARDUINO_FEATHER_M4_CAN
  #error "This sketch should be compiled for Arduino Feather M4 CAN (SAME51)"
#endif

//--------------------------------------------------------------------------------------------------

#include <ACANFD_FeatherM4CAN_Settings.h>
#include <ACANFD_FeatherM4CAN_Buffer16.h>

//--------------------------------------------------------------------------------------------------

enum class ACANFD_FeatherM4CAN_Module { can0, can1 } ;

//--------------------------------------------------------------------------------------------------

class ACANFD_FeatherM4CAN {
//--- Constructor
  public: ACANFD_FeatherM4CAN (const ACANFD_FeatherM4CAN_Module inModule,
                               uint32_t * inMessageRAMPtr) ;

//--- begin; returns a result code :
//  0 : Ok
//  other: every bit denotes an error
//   public: static const uint32_t kTooMuchPrimaryFilters     = 1 << 31 ;
//   public: static const uint32_t kNotConformPrimaryFilter   = 1 << 30 ;
//   public: static const uint32_t kTooMuchSecondaryFilters   = 1 << 29 ;
//   public: static const uint32_t kNotConformSecondaryFilter = 1 << 28 ;
//   public: static const uint32_t kInvalidTxPin              = 1 << 27 ;
//   public: static const uint32_t kInvalidRxPin              = 1 << 26 ;
   public: static const uint32_t kCANBitConfiguration       = 1 << 25 ;
//
// //--- CANFD configuration errors
//   public: static const uint32_t kCANFDNotAvailableOnCAN1AndCAN2 = 1 << 24 ;
//   public: static const uint32_t kTooMuchCANFDFilters       = 1 << 23 ;
//   public: static const uint32_t kCANFDInvalidRxMBCountVersusPayload = 1 << 22 ;

  public: uint32_t beginFD (const ACANFD_FeatherM4CAN_Settings & inSettings) ;
//                             const ACANFDFilter inFilters [] = nullptr,
//                             const uint32_t inFilterCount = 0) ;


// //--- end: stop CAN controller
//   public: void end (void) ;
//
// //--- Transmitting messages
//   public: inline uint32_t transmitBufferSize (void) const { return mTransmitBufferSize ; }
//   public: inline uint32_t transmitBufferCount (void) const { return mTransmitBufferCount ; }
//   public: inline uint32_t transmitBufferPeakCount (void) const { return mTransmitBufferPeakCount ; }
//
// //--- Transmitting messages and return status (returns 0 if ok)
//    public: uint32_t tryToSendReturnStatus (const CANMessage & inMessage) ;
   public: bool sendBufferNotFullForIndex (const uint32_t inTxBufferIndex) ;
   public: uint32_t tryToSendReturnStatusFD (const CANFDMessage & inMessage) ;
   public: static const uint32_t kTransmitBufferOverflow = 1 << 0 ;
   public: static const uint32_t kTransmitBufferIndexTooLarge = 1 << 1 ;
//   public: static const uint32_t kNoAvailableMBForSendingRemoteFrame = 1 << 1 ;
//   public: static const uint32_t kNoReservedMBForSendingRemoteFrame = 1 << 2 ;
//   public: static const uint32_t kMessageLengthExceedsPayload = 1 << 3 ;
//   public: static const uint32_t kFlexCANinCAN20BMode = 1 << 4 ;
//   public: static const uint32_t kFlexCANinCANFDBMode = 1 << 5 ;

//--- Receiving messages
   public: bool receiveFD0 (CANFDMessage & outMessage) ;

// //--- FlexCAN controller state
//   public: tControllerState controllerState (void) const ;
//   public: uint32_t receiveErrorCounter (void) const ;
//   public: uint32_t transmitErrorCounter (void) const ;
//
// //--- Call back function array
//   private: ACANCallBackRoutine * mCallBackFunctionArray = nullptr ;
//   private: ACANFDCallBackRoutine * mCallBackFunctionArrayFD = nullptr ; // null, or size is mRxCANFDMBCount
//   private: uint32_t mCallBackFunctionArraySize = 0 ;
//
// //--- Base address
//   private: const uint32_t mFlexcanBaseAddress ;
//   private: const ACAN_T4_Module mModule ; // Initialized in constructor
//
// //--- CANFD properties
//   private : bool mCANFD = false ;
//   private : ACAN_T4FD_Settings::Payload mPayload = ACAN_T4FD_Settings::PAYLOAD_64_BYTES ;
//   private : uint8_t mRxCANFDMBCount = 12 ;
//   public : uint32_t RxCANFDMBCount (void) const { return mRxCANFDMBCount ; }
//
// //--- Filters
//   private : uint8_t mActualPrimaryFilterCount = 0 ;
//   private : uint8_t mMaxPrimaryFilterCount = 0 ;
//   private: uint32_t * mCANFDAcceptanceFilterArray = nullptr ; //

//--- Driver Transmit buffer
  private: ACANFD_FeatherM4CAN_Buffer16 mDriverTransmitBuffer ;

//--- Driver receive buffer
  private: ACANFD_FeatherM4CAN_Buffer16 mDriverReceiveBuffer0 ;

//--- Driver transmit buffer
//   private: CANFDMessage * mTransmitBuffer = nullptr ;
//   private: volatile uint32_t mTransmitBufferSize = 0 ;
//   private: volatile uint32_t mTransmitBufferReadIndex = 0 ;
//   private: volatile uint32_t mTransmitBufferCount = 0 ;
//   private: volatile uint32_t mTransmitBufferPeakCount = 0 ; // == mTransmitBufferSize + 1 if tentative overflow did occur

// //--- Global status
//   private : volatile uint32_t mGlobalStatus = 0 ; // Returns 0 if all is ok
//   public : uint32_t globalStatus (void) const { return mGlobalStatus ; }
//   public : void resetGlobalStatus (const uint32_t inReset) ;
// //--- Global status bit names
//   public: static const uint32_t kGlobalStatusInitError     = 1 <<  0 ;
//   public: static const uint32_t kGlobalStatusRxFIFOWarning = 1 <<  1 ; // Occurs when the number of messages goes from 4 to 5
//   public: static const uint32_t kGlobalStatusRxFIFOOverflow = 1 <<  2 ; // Occurs when RxFIFO overflows
//   public: static const uint32_t kGlobalStatusReceiveBufferOverflow = 1 <<  3 ; // Occurs when driver receive buffer overflows
//
// //--- Message interrupt service routine
//   public: void message_isr (void) ;

//--- Private properties
  private: Can * mModulePtr ;
  private: uint32_t * mMessageRAMPtr ;
  private: uint32_t * mStandardFiltersPointer = nullptr ;
  private: uint32_t * mRxFIFO0Pointer = nullptr ;
  private: uint32_t * mTxBuffersPointer = nullptr ;
  private: ACANFD_FeatherM4CAN_Settings::Payload mHardwareRxFIFO0Payload = ACANFD_FeatherM4CAN_Settings::PAYLOAD_64_BYTES ;
  private: ACANFD_FeatherM4CAN_Settings::Payload mHardwareTxBufferPayload = ACANFD_FeatherM4CAN_Settings::PAYLOAD_64_BYTES ;
  private: ACANFD_FeatherM4CAN_Module mModule ;

//--- Private methods
  public: void interruptServiceRoutine (void) ;
  private: void writeTxBuffer (const CANFDMessage & inMessage, const uint32_t inTxBufferIndex) ;

//   private : uint32_t tryToSendRemoteFrame (const CANMessage & inMessage) ;
//   private : uint32_t tryToSendDataFrame (const CANMessage & inMessage) ;
//   private : void writeTxRegisters (const CANMessage & inMessage, const uint32_t inMBIndex) ;
//   private : uint32_t tryToSendDataFrameFD (const CANFDMessage & inMessage) ;
//   private : uint32_t tryToSendRemoteFrameFD (const CANFDMessage & inMessage) ;
//   private : void writeTxRegistersFD (const CANFDMessage & inMessage, volatile uint32_t * inMBAddress) ;
//   private : void message_isr_receive (void) ;
//   private : void message_isr_receiveFD (const uint32_t inReceiveMailboxIndex) ;
//   private : void message_isr_FD (void) ;
//   private: void readRxRegisters (CANMessage & outMessage) ;
//   private : void readRxRegistersFD (CANFDMessage & outMessage, const uint32_t inReceiveMailboxIndex) ;

//--- Status class
  public: class Status {
    public: Status (Can * inModulePtr) ;
    public: const uint16_t mErrorCount ; // Copy of ECR register (page 1130)
    public: const uint32_t mProtocolStatus ; // Copy of PSR register (page 1131)
    public: inline uint16_t txErrorCount (void) const { return isBusOff () ? 256 : uint8_t (mErrorCount) ; }
    public: inline uint8_t rxErrorCount (void) const { return uint8_t (mErrorCount >> 8) ; }
    public: inline uint8_t isBusOff (void) const { return (mProtocolStatus & (1 << 7)) != 0 ; }
    public: inline uint8_t transceiverDelayCompensationValue (void) const { return uint8_t (mProtocolStatus >> 16) & 0x7F ; }
  } ;

  public: inline Status getStatus (void) const { return Status (mModulePtr) ; }

//--- No copy
  private : ACANFD_FeatherM4CAN (const ACANFD_FeatherM4CAN &) = delete ;
  private : ACANFD_FeatherM4CAN & operator = (const ACANFD_FeatherM4CAN &) = delete ;
} ;

//--------------------------------------------------------------------------------------------------

extern ACANFD_FeatherM4CAN can0 ;
extern ACANFD_FeatherM4CAN can1 ;

//--------------------------------------------------------------------------------------------------
