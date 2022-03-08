## CANFD Library for Adafruit Feather M4 CAN


It handles *Controller Area Network with Flexible Data* (CANFD) for CAN0 and CAN1. The board contains a CANFD transceiver for CAN1 and the CANH / CANL signals are exposed. For CAN0, TxCAN is D12 and RxCAN is D13.

### Compatibility with the ACAN2517FD library

This library is fully compatible with the MCP2517FD, MCP2518FD CAN Controllers ACAN2517FD library [https://github.com/pierremolinaro/acan2517FD](https://github.com/pierremolinaro/acan2517FD), it uses a very similar API and the same `CANFDMessage` class for handling messages.

### ACANFD_FeatherM4CAN library description
ACANFD_FeatherM4CAN is a driver for the two CAN modules of the Adafruit Feather M4 CAN microcontroller.

The driver supports many bit rates, as standard 62.5 kbit/s, 125 kbit/s, 250 kbit/s, 500 kbit/s, and 1 Mbit/s. An efficient CAN bit timing calculator finds settings for them, but also for exotic bit rates as 833 kbit/s. If the wished bit rate cannot be achieved, the `begin` method does not configure the hardware and returns an error code.

> Driver API is fully described by the PDF file in the `extras` directory.

### Demo Sketches

> The demo sketches are in the `examples` directory.

The `LoopBackDemoCAN20B_CAN1` demo sketch shows how configure the `CAN1`module, and how to send and receive frames.

```cpp
#define CAN0_MESSAGE_RAM_SIZE (0)
#define CAN1_MESSAGE_RAM_SIZE (1728)

#include <ACANFD_FeatherM4CAN.h>
```

`<ACANFD_FeatherM4CAN.h>` should be included only from the `.ino` file. From an other file, include `<ACANFD_FeatherM4CAN-from-cpp.h>`.  Before including `<ACANFD_FeatherM4CAN.h>`, you should define Message RAM size for CAN0 and Message RAM size for CAN1.   Maximum size is 4,352 (4,352 32-bit words). A 0 size means the CAN module is not configured; its TxCAN and RxCAN pins can be freely used for an other function. The `begin` method checks if actual size is greater or equal to required size.

Configuration is a four-step operation.

1. Instanciation of the `settings` object : the constructor has two parameters: the desired CAN arbitration bit rate, and the data bit rate factor. The `settings` is fully initialized.
2. You can override default settings. Here, we set the `mModuleMode` property to `EXTERNAL_LOOP_BACK`, enabling to run demo code without any additional hardware and observe emitted frames. We can also for example change the driver receive FIFO 0 size by setting the `mDriverReceiveFIFO0Size` property.
3. Calling the `begin` method configures the driver and starts CAN bus participation. Any message can be sent, any frame on the bus is received. No default filter to provide.
4. You check the `errorCode` value to detect configuration error(s).

```cpp
void setup () {
  pinMode (LED_BUILTIN, OUTPUT) ;
  Serial.begin (115200) ;
  while (!Serial) {
    delay (50) ;
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
  }
  Serial.println ("CAN1 CANFD loopback test") ;
  ACANFD_FeatherM4CAN_Settings settings (1000 * 1000, DataBitRateFactor::x2) ;

  settings.mModuleMode = ACANFD_FeatherM4CAN_Settings::EXTERNAL_LOOP_BACK ;

  const uint32_t errorCode = can1.beginFD (settings) ;
  Serial.print ("Message RAM Minimum required size: ") ;
  Serial.print (can1.messageRamRequiredSize ()) ;
  Serial.println (" words") ;
  if (0 == errorCode) {
    Serial.println ("can configuration ok") ;
  }else{
    Serial.print ("Error can configuration: 0x") ;
    Serial.println (errorCode, HEX) ;
  }
}
```

Now, an example of the `loop` function. As we have selected loop back mode, every sent frame is received.

```cpp
static const uint32_t PERIOD = 1000 ;
static uint32_t gBlinkDate = PERIOD ;
static uint32_t gSentCount = 0 ;
static uint32_t gReceiveCount = 0 ;
static CANFDMessage gSentFrame ;
static bool gOk = true ;

void loop () {
  if (gBlinkDate <= millis ()) {
    gBlinkDate += PERIOD ;
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
    if (gOk) {
      ... build a random CANFD frame ...
      const uint32_t sendStatus = can1.tryToSendReturnStatusFD (gSentFrame) ;
      if (sendStatus == 0) {
        gSentCount += 1 ;
        Serial.print ("Sent ") ;
        Serial.println (gSentCount) ;
      }else{
        Serial.print ("Sent error 0x") ;
        Serial.println (sendStatus) ;
      }
    }
  }
  //--- Receive frame
  CANFDMessage frame ;
  if (gOk && can1.receiveFD0 (frame)) {
    bool sameFrames = ... compare gSentFrame and frame ... ;
    if (sameFrames) {
      gReceiveCount += 1 ;
      Serial.print ("Received ") ;
      Serial.println (gReceiveCount) ;
    }else{
      gOk = false ;
      ... display error ...
    }
  }
}
```

`CANFDMessage` is the class that defines a CANFD message. The `message` object is fully initialized by the default constructor. 
The `can0.tryToSendReturnStatusFD` tries to send the message. It returns `0` if the message has been sucessfully added to the driver transmit buffer, and an error code otherwise.

The `gSendDate` variable handles sending a CANFD message every 1000 ms.

`can1.receiveFD0 (frame)` returns `true` if a message has been received, and assigned to the `message` argument. Then the receive message is compared with the sent message. In case of error, no more message is sent.
