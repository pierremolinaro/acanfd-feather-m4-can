//--------------------------------------------------------------------------------------------------

#pragma once

//--------------------------------------------------------------------------------------------------

#ifndef ARDUINO_FEATHER_M4_CAN
  #error "This sketch should be compiled for Arduino Feather M4 CAN (SAME51)"
#endif

//--------------------------------------------------------------------------------------------------

#include <ACANFD_FeatherM4CAN_Settings.h>
#include <ACANFD_FeatherM4CAN_FIFO.h>

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

//--- Testing send buffer not full
   public: bool sendBufferNotFullForIndex (const uint32_t inTxBufferIndex) ;

//--- Transmitting messages and return status (returns 0 if ok)
  public: uint32_t tryToSendReturnStatusFD (const CANFDMessage & inMessage) ;
  public: static const uint32_t kTransmitBufferOverflow = 1 << 0 ;
  public: static const uint32_t kTransmitBufferIndexTooLarge = 1 << 1 ;

  public: inline uint32_t transmitFIFOSize (void) const { return mDriverTransmitFIFO.size () ; }
  public: inline uint32_t transmitFIFOCount (void) const { return mDriverTransmitFIFO.count () ; }
  public: inline uint32_t transmitFIFOPeakCount (void) const { return mDriverTransmitFIFO.peakCount () ; }

//--- Receiving messages
   public: bool receiveFD0 (CANFDMessage & outMessage) ;

//--- Driver Transmit buffer
  private: ACANFD_FeatherM4CAN_FIFO mDriverTransmitFIFO ;

//--- Driver receive FIFO
  private: ACANFD_FeatherM4CAN_FIFO mDriverReceiveFIFO0 ;

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
