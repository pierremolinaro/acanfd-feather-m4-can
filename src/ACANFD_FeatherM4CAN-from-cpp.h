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

//--------------------------------------------------------------------------------------------------
//    Private Dynamic Array
//--------------------------------------------------------------------------------------------------

  private: template <typename T> class DynamicArray {
  //--- Default constructor
    public: DynamicArray (void) { }

  //--- Destructor
    public: ~ DynamicArray (void) { delete [] mArray ; }

  //--- Append
    public: void append (const T & inObject) {
      if (mCapacity == mCount) {
        mCapacity += 64 ;
        T * newArray = new T [mCapacity] ;
        for (uint32_t i=0 ; i<mCount ; i++) {
          newArray [i] = mArray [i] ;
        }
        delete [] mArray ;
        mArray = newArray ;
      }
      mArray [mCount] = inObject ;
      mCount += 1 ;
    }

  //--- Methods
    public: void release (void) {
      delete [] mArray ;
      mArray = nullptr ;
      mCount = 0 ;
      mCapacity = 0 ;
    }

    public: void setCapacity (const uint32_t inNewCapacity) {
      if (mCapacity < inNewCapacity) {
        mCapacity = inNewCapacity ;
        T * newArray = new T [mCapacity] ;
        for (uint32_t i=0 ; i<mCount ; i++) {
          newArray [i] = mArray [i] ;
        }
        delete [] mArray ;
        mArray = newArray ;
      }
    }

  //--- Access
    public: uint32_t count () const { return mCount ; }
    public: T operator [] (const uint32_t inIndex) const { return mArray [inIndex] ; }

  //--- Private properties
    private: uint8_t mCapacity = 0 ;
    private: uint8_t mCount = 0 ;
    private: T * mArray = nullptr ;

  //--- No copy
    private : DynamicArray (const DynamicArray &) = delete ;
    private : DynamicArray & operator = (const DynamicArray &) = delete ;
  } ;

//--------------------------------------------------------------------------------------------------
//    Standard filters
//--------------------------------------------------------------------------------------------------

  public: class StandardFilters {
  //--- Default constructor
    public: StandardFilters (void) { }

  //--- Append filter
    public: bool addSingle (const uint16_t inIdentifier,
                            const ACANFD_FeatherM4CAN_FilterAction inAction,
                            const ACANFDCallBackRoutine inCallBack = nullptr) ;

    public: bool addDual (const uint16_t inIdentifier1,
                          const uint16_t inIdentifier2,
                          const ACANFD_FeatherM4CAN_FilterAction inAction,
                          const ACANFDCallBackRoutine inCallBack = nullptr) ;

    public: bool addRange (const uint16_t inIdentifier1,
                           const uint16_t inIdentifier2,
                           const ACANFD_FeatherM4CAN_FilterAction inAction,
                           const ACANFDCallBackRoutine inCallBack = nullptr) ;

    public: bool addClassic (const uint16_t inIdentifier,
                             const uint16_t inMask,
                             const ACANFD_FeatherM4CAN_FilterAction inAction,
                             const ACANFDCallBackRoutine inCallBack = nullptr) ;

  //--- Access
    public: uint32_t count () const { return mFilterArray.count () ; }
    public: uint32_t filterAtIndex (const uint32_t inIndex) const { return mFilterArray [inIndex] ; }
    public: ACANFDCallBackRoutine callBackAtIndex (const uint32_t inIndex) const {
      return mCallBackArray [inIndex] ;
    }

  //--- Private properties
    private: DynamicArray <uint32_t> mFilterArray ;
    private: DynamicArray < ACANFDCallBackRoutine > mCallBackArray ;

  //--- No copy
    private : StandardFilters (const StandardFilters &) = delete ;
    private : StandardFilters & operator = (const StandardFilters &) = delete ;
  } ;

//--------------------------------------------------------------------------------------------------
//    Extended filters
//--------------------------------------------------------------------------------------------------

  public: class ExtendedFilters {
  //--- Default constructor
    public: ExtendedFilters (void) { }

  //--- Append filter
    public: bool addSingle (const uint32_t inExtendedIdentifier,
                            const ACANFD_FeatherM4CAN_FilterAction inAction,
                            const ACANFDCallBackRoutine inCallBack = nullptr) ;

    public: bool addDual (const uint32_t inExtendedIdentifier1,
                          const uint32_t inExtendedIdentifier2,
                          const ACANFD_FeatherM4CAN_FilterAction inAction,
                          const ACANFDCallBackRoutine inCallBack = nullptr) ;

    public: bool addRange (const uint32_t inExtendedIdentifier1,
                           const uint32_t inExtendedIdentifier2,
                           const ACANFD_FeatherM4CAN_FilterAction inAction,
                           const ACANFDCallBackRoutine inCallBack = nullptr) ;

    public: bool addClassic (const uint32_t inExtendedIdentifier,
                             const uint32_t inExtendedMask,
                             const ACANFD_FeatherM4CAN_FilterAction inAction,
                             const ACANFDCallBackRoutine inCallBack = nullptr) ;

  //--- Access
    public: uint32_t count () const { return mCallBackArray.count () ; }
    public: uint32_t firstWordAtIndex (const uint32_t inIndex) const { return mFilterArray [inIndex * 2] ; }
    public: uint32_t secondWordAtIndex (const uint32_t inIndex) const { return mFilterArray [inIndex * 2 + 1] ; }
    public: ACANFDCallBackRoutine callBackAtIndex (const uint32_t inIndex) const {
      return mCallBackArray [inIndex] ;
    }

  //--- Private properties
    private: DynamicArray <uint32_t> mFilterArray ;
    private: DynamicArray < void (*) (const CANFDMessage & inMessage) > mCallBackArray ;

  //--- No copy
    private : ExtendedFilters (const ExtendedFilters &) = delete ;
    private : ExtendedFilters & operator = (const ExtendedFilters &) = delete ;
  } ;

//--------------------------------------------------------------------------------------------------
//  ACANFD_FeatherM4CAN class
//--------------------------------------------------------------------------------------------------

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
  public: static const uint32_t kDedicacedTransmitTxBufferCountGreaterThan30 = 1 << 24 ;
  public: static const uint32_t kTxBufferCountGreaterThan32         = 1 << 25 ;
  public: static const uint32_t kHardwareTransmitFIFOSizeLowerThan2 = 1 << 26 ;
  public: static const uint32_t kHardwareRxFIFO1SizeGreaterThan64      = 1 << 27 ;
  public: static const uint32_t kStandardFilterCountGreaterThan128     = 1 << 28 ;
  public: static const uint32_t kExtendedFilterCountGreaterThan128     = 1 << 29 ;

  public: uint32_t beginFD (const ACANFD_FeatherM4CAN_Settings & inSettings,
                            const StandardFilters & inStandardFilters = StandardFilters (),
                            const ExtendedFilters & inExtendedFilters = ExtendedFilters ()) ;

  public: uint32_t beginFD (const ACANFD_FeatherM4CAN_Settings & inSettings,
                            const ExtendedFilters & inExtendedFilters) ;

//--- Getting Message RAM required minimum size
  public: uint32_t messageRamRequiredMinimumSize (void) ;

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
  public: inline ACANFD_FeatherM4CAN_Settings::Payload hardwareTxBufferPayload (void) const {
    return mHardwareTxBufferPayload ;
  }

//--- Receiving messages
  public: bool availableFD0 (void) ;
  public: bool receiveFD0 (CANFDMessage & outMessage) ;
  public: bool availableFD1 (void) ;
  public: bool receiveFD1 (CANFDMessage & outMessage) ;
  public: bool dispatchReceivedMessage (void) ;
  public: bool dispatchReceivedMessageFIFO0 (void) ;
  public: bool dispatchReceivedMessageFIFO1 (void) ;

//--- Driver Transmit buffer
  private: ACANFD_FeatherM4CAN_FIFO mDriverTransmitFIFO ;

//--- Driver receive FIFO 0
  private: ACANFD_FeatherM4CAN_FIFO mDriverReceiveFIFO0 ;
  public: uint32_t driverReceiveFIFO0Size (void) { return mDriverReceiveFIFO0.size () ; }
  public: uint32_t driverReceiveFIFO0Count (void) { return mDriverReceiveFIFO0.count () ; }
  public: uint32_t driverReceiveFIFO0PeakCount (void) { return mDriverReceiveFIFO0.peakCount () ; }
  public: void resetDriverReceiveFIFO0PeakCount (void) { mDriverReceiveFIFO0.resetPeakCount () ; }
  public: inline ACANFD_FeatherM4CAN_Settings::Payload hardwareRxFIFO0Payload (void) const {
    return mHardwareRxFIFO0Payload ;
  }

//--- Driver receive FIFO 1
  private: ACANFD_FeatherM4CAN_FIFO mDriverReceiveFIFO1 ;
  public: uint32_t driverReceiveFIFO1Size (void) { return mDriverReceiveFIFO1.size () ; }
  public: uint32_t driverReceiveFIFO1Count (void) { return mDriverReceiveFIFO1.count () ; }
  public: uint32_t driverReceiveFIFO1PeakCount (void) { return mDriverReceiveFIFO1.peakCount () ; }
  public: void resetDriverReceiveFIFO1PeakCount (void) { mDriverReceiveFIFO1.resetPeakCount () ; }
  public: inline ACANFD_FeatherM4CAN_Settings::Payload hardwareRxFIFO1Payload (void) const {
    return mHardwareRxFIFO1Payload ;
  }

//--- Private properties
  private: Can * mModulePtr ;
  private: uint32_t * mMessageRAMPtr ;
  public: const uint32_t mMessageRamWordSize ;
  private: uint32_t * mRxFIFO0Pointer = nullptr ;
  private: uint32_t * mRxFIFO1Pointer = nullptr ;
  private: uint32_t * mTxBuffersPointer = nullptr ;
  private: uint32_t * mEndOfMessageRamPointer = nullptr ;
  private: DynamicArray < ACANFDCallBackRoutine > mStandardFilterCallBackArray ;
  private: DynamicArray < ACANFDCallBackRoutine > mExtendedFilterCallBackArray ;
  private: ACANFDCallBackRoutine mNonMatchingStandardMessageCallBack = nullptr ;
  private: ACANFDCallBackRoutine mNonMatchingExtendedMessageCallBack = nullptr ;
  private: ACANFD_FeatherM4CAN_Settings::Payload mHardwareRxFIFO0Payload = ACANFD_FeatherM4CAN_Settings::PAYLOAD_64_BYTES ;
  private: ACANFD_FeatherM4CAN_Settings::Payload mHardwareRxFIFO1Payload = ACANFD_FeatherM4CAN_Settings::PAYLOAD_64_BYTES ;
  private: ACANFD_FeatherM4CAN_Settings::Payload mHardwareTxBufferPayload = ACANFD_FeatherM4CAN_Settings::PAYLOAD_64_BYTES ;
  private: ACANFD_FeatherM4CAN_Module mModule ;

//--- Private methods
  public: void interruptServiceRoutine (void) ;
  private: void writeTxBuffer (const CANFDMessage & inMessage, const uint32_t inTxBufferIndex) ;
  private: void internalDispatchReceivedMessage (const CANFDMessage & inMessage) ;

//--- Status class
  public: class Status {
    public: Status (Can * inModulePtr) ;
    public: const uint16_t mErrorCount ; // Copy of ECR register (page 1130)
    public: const uint32_t mProtocolStatus ; // Copy of PSR register (page 1131)
    public: inline uint16_t txErrorCount (void) const { return isBusOff () ? 256 : uint8_t (mErrorCount) ; }
    public: inline uint8_t rxErrorCount (void) const { return uint8_t (mErrorCount >> 8) ; }
    public: inline bool isBusOff (void) const { return (mProtocolStatus & (1 << 7)) != 0 ; }
    public: inline uint8_t transceiverDelayCompensationOffset (void) const { return uint8_t (mProtocolStatus >> 16) & 0x7F ; }
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
