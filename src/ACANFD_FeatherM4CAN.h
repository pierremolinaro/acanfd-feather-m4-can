//--------------------------------------------------------------------------------------------------
// THIS FILE SHOULD ONLY BE INCLUDED FROM .ino FILE
// From other C++ files, include ACANFD_FeatherM4CAN-from-cpp.h
//--------------------------------------------------------------------------------------------------

#pragma once

//--------------------------------------------------------------------------------------------------

#include <ACANFD_FeatherM4CAN-from-cpp.h>

//--------------------------------------------------------------------------------------------------

#ifndef CAN0_MESSAGE_RAM_SIZE
  #error "The CAN0_MESSAGE_RAM_SIZE compile time symbol should be defined in the .ino file, before including <ACANFD_FeatherM4CAN.h>"
#endif

//--------------------------------------------------------------------------------------------------

#ifndef CAN1_MESSAGE_RAM_SIZE
  #error "The CAN1_MESSAGE_RAM_SIZE compile time symbol should be defined in the .ino file, before including <ACANFD_FeatherM4CAN.h>"
#endif

//--------------------------------------------------------------------------------------------------
//  CAN0
//--------------------------------------------------------------------------------------------------

#if CAN0_MESSAGE_RAM_SIZE > 0
  static uint32_t gMessageRam0 [CAN0_MESSAGE_RAM_SIZE] ;

  ACANFD_FeatherM4CAN can0 (ACANFD_FeatherM4CAN_Module::can0, gMessageRam0, CAN0_MESSAGE_RAM_SIZE) ;

  extern "C" void CAN0_Handler (void) ; // SHOULD HAVE C LINKAGE

  void CAN0_Handler (void) {
    can0.interruptServiceRoutine () ;
  }
#endif

//--------------------------------------------------------------------------------------------------
//  CAN1
//--------------------------------------------------------------------------------------------------

#if CAN1_MESSAGE_RAM_SIZE > 0
  uint32_t gMessageRam1 [CAN1_MESSAGE_RAM_SIZE] ;

  ACANFD_FeatherM4CAN can1 (ACANFD_FeatherM4CAN_Module::can1, gMessageRam1, CAN1_MESSAGE_RAM_SIZE) ;

  extern "C" void CAN1_Handler (void) ; // SHOULD HAVE C LINKAGE

  void CAN1_Handler (void) {
    can1.interruptServiceRoutine () ;
  }
#endif

//--------------------------------------------------------------------------------------------------
