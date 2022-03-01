//--------------------------------------------------------------------------------------------------

#include <ACANFD_FeatherM4CAN_Buffer16.h>

//--------------------------------------------------------------------------------------------------
// Default constructor
//--------------------------------------------------------------------------------------------------

ACANFD_FeatherM4CAN_Buffer16::ACANFD_FeatherM4CAN_Buffer16 (void) :
mBuffer (NULL),
mSize (0),
mReadIndex (0),
mCount (0),
mPeakCount (0) {
}

//--------------------------------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------------------------------

ACANFD_FeatherM4CAN_Buffer16:: ~ ACANFD_FeatherM4CAN_Buffer16 (void) {
  delete [] mBuffer ;
}

//--------------------------------------------------------------------------------------------------
// initWithSize
//--------------------------------------------------------------------------------------------------

void ACANFD_FeatherM4CAN_Buffer16::initWithSize (const uint16_t inSize) {
  delete [] mBuffer ;
  mBuffer = new CANFDMessage [inSize] ;
  mSize = inSize ;
  mReadIndex = 0 ;
  mCount = 0 ;
  mPeakCount = 0 ;
}

//--------------------------------------------------------------------------------------------------
// append
//--------------------------------------------------------------------------------------------------

bool ACANFD_FeatherM4CAN_Buffer16::append (const CANFDMessage & inMessage) {
  const bool ok = mCount < mSize ;
  if (ok) {
    uint16_t writeIndex = mReadIndex + mCount ;
    if (writeIndex >= mSize) {
      writeIndex -= mSize ;
    }
    mBuffer [writeIndex] = inMessage ;
    mCount += 1 ;
    if (mPeakCount < mCount) {
      mPeakCount = mCount ;
    }
  }
  return ok ;
}

//--------------------------------------------------------------------------------------------------
// Remove
//--------------------------------------------------------------------------------------------------

bool ACANFD_FeatherM4CAN_Buffer16::remove (CANFDMessage & outMessage) {
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

//--------------------------------------------------------------------------------------------------
// Free
//--------------------------------------------------------------------------------------------------

void ACANFD_FeatherM4CAN_Buffer16::free (void) {
  delete [] mBuffer ; mBuffer = nullptr ;
  mSize = 0 ;
  mReadIndex = 0 ;
  mCount = 0 ;
  mPeakCount = 0 ;
}

//--------------------------------------------------------------------------------------------------
