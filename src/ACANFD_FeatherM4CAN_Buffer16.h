//--------------------------------------------------------------------------------------------------

#pragma once

//--------------------------------------------------------------------------------------------------

#include <CANFDMessage.h>

//--------------------------------------------------------------------------------------------------

class ACANFD_FeatherM4CAN_Buffer16 {

  //································································································
  // Default constructor
  //································································································

  public: ACANFD_FeatherM4CAN_Buffer16 (void)  :
  mBuffer (NULL),
  mSize (0),
  mReadIndex (0),
  mCount (0),
  mPeakCount (0) {
  }

  //································································································
  // Destructor
  //································································································

  public: ~ ACANFD_FeatherM4CAN_Buffer16 (void) {
    delete [] mBuffer ;
  }

  //································································································
  // Private properties
  //································································································

  private: CANFDMessage * mBuffer ;
  private: uint16_t mSize ;
  private: uint16_t mReadIndex ;
  private: uint16_t mCount ;
  private: uint16_t mPeakCount ; // > mSize if overflow did occur

  //································································································
  // Accessors
  //································································································

  public: inline uint16_t size (void) const { return mSize ; }
  public: inline uint16_t count (void) const { return mCount ; }
  public: inline uint16_t peakCount (void) const { return mPeakCount ; }

  //································································································
  // initWithSize
  //································································································

  public: bool initWithSize (const uint16_t inSize) {
    delete [] mBuffer ;
    mBuffer = new CANFDMessage [inSize] ;
    const bool ok = mBuffer != NULL ;
    mSize = ok ? inSize : 0 ;
    mReadIndex = 0 ;
    mCount = 0 ;
    mPeakCount = 0 ;
    return ok ;
  }

  //································································································
  // append
  //································································································

  public: bool append (const CANFDMessage & inMessage) {
    const bool ok = mCount < mSize ;
    if (ok) {
      uint16_t writeIndex = mReadIndex + mCount ;
      if (writeIndex >= mSize) {
        writeIndex -= mSize ;
      }
      mBuffer [writeIndex] = inMessage ;
      mCount ++ ;
      if (mPeakCount < mCount) {
        mPeakCount = mCount ;
      }
    }
    return ok ;
  }

  //································································································
  // Remove
  //································································································

  public: bool remove (CANFDMessage & outMessage) {
    const bool ok = mCount > 0 ;
    if (ok) {
      outMessage = mBuffer [mReadIndex] ;
      mCount -= 1 ;
      mReadIndex += 1 ;
      if (mReadIndex == mSize) {
        mReadIndex = 0 ;
      }
    }
    return ok ;
  }

  //································································································
  // Free
  //································································································

  public: void free (void) {
    delete [] mBuffer ; mBuffer = nullptr ;
    mSize = 0 ;
    mReadIndex = 0 ;
    mCount = 0 ;
    mPeakCount = 0 ;
  }

  //································································································
  // Reset Peak Count
  //································································································

  public: inline void resetPeakCount (void) { mPeakCount = mCount ; }

  //································································································
  // No copy
  //································································································

  private: ACANFD_FeatherM4CAN_Buffer16 (const ACANFD_FeatherM4CAN_Buffer16 &) ;
  private: ACANFD_FeatherM4CAN_Buffer16 & operator = (const ACANFD_FeatherM4CAN_Buffer16 &) ;
} ;

//--------------------------------------------------------------------------------------------------
