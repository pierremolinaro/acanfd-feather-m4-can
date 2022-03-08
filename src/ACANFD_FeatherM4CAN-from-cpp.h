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
                               uint32_t * inMessageRAMPtr,
                               const uint32_t inMessageRamWordSize) ;

//--- begin; returns a result code :
//  0 : Ok
//  other: every bit denotes an error
   public: static const uint32_t kMessageRamTooSmall                    = 1 << 20 ;
   public: static const uint32_t kMessageRamNotInFirst64kio             = 1 << 21 ;
   public: static const uint32_t kHardwareRxFIFO0SizeGreaterThan64      = 1 << 22 ;
   public: static const uint32_t kHardwareTransmitFIFOSizeGreaterThan32 = 1 << 23 ;
   public: static const uint32_t kDedicacedTransmitTxBufferCountGreaterThan32 = 1 << 24 ;
   public: static const uint32_t kTxBufferCountGreaterThan32                  = 1 << 25 ;

  public: uint32_t beginFD (const ACANFD_FeatherM4CAN_Settings & inSettings) ;

//--- Testing send buffer
  public: uint32_t messageRamRequiredSize (void) ;

//--- Testing send buffer
  public: bool sendBufferNotFullForIndex (const uint32_t inTxBufferIndex) ;

//--- Transmitting messages and return status (returns 0 if ok)
  public: uint32_t tryToSendReturnStatusFD (const CANFDMessage & inMessage) ;
  public: static const uint32_t kInvalidMessage              = 1 ;
  public: static const uint32_t kTransmitBufferIndexTooLarge = 2 ;
  public: static const uint32_t kTransmitBufferOverflow      = 3 ;

  public: inline uint32_t transmitFIFOSize (void) const { return mDriverTransmitFIFO.size () ; }
  public: inline uint32_t transmitFIFOCount (void) const { return mDriverTransmitFIFO.count () ; }
  public: inline uint32_t transmitFIFOPeakCount (void) const { return mDriverTransmitFIFO.peakCount () ; }

//--- Receiving messages
   public: bool availableFD0 (void) ;
   public: bool receiveFD0 (CANFDMessage & outMessage) ;

//--- Driver Transmit buffer
  private: ACANFD_FeatherM4CAN_FIFO mDriverTransmitFIFO ;

//--- Driver receive FIFO
  private: ACANFD_FeatherM4CAN_FIFO mDriverReceiveFIFO0 ;
  public: uint32_t driverReceiveFIFO0Size (void) { return mDriverReceiveFIFO0.size () ; }
  public: uint32_t driverReceiveFIFO0Count (void) { return mDriverReceiveFIFO0.count () ; }
  public: uint32_t driverReceiveFIFO0PeakCount (void) { return mDriverReceiveFIFO0.peakCount () ; }
  public: void resetDriverReceiveFIFO0PeakCount (void) { mDriverReceiveFIFO0.resetPeakCount () ; }

//--- Private properties
  private: Can * mModulePtr ;
  private: uint32_t * mMessageRAMPtr ;
  public: const uint32_t mMessageRamWordSize ;
  private: uint32_t * mStandardFiltersPointer = nullptr ;
  private: uint32_t * mRxFIFO0Pointer = nullptr ;
  private: uint32_t * mTxBuffersPointer = nullptr ;
  private: uint32_t * mEndOfMessageRamPointer = nullptr ;
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
