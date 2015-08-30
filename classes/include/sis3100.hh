/*! 
  \file sis3100.hh
  \brief provides an interface for interrupt handling with the Struck SIS3100 VME interface
  \author Clemens Sauerzopf <clemens.sauerzopf@oeaw.ac.at>  
  \date 06.08.2014
  \version 1.0
*/ 


#ifndef __SIS3100_HH__
#define __SIS3100_HH__

#include<stdint.h>
#include<sys/types.h>

 #if HAVE_MIDAS__
 extern "C" {
 #include "midas.h"
 #include "sis3100/linux/sis3100_vme_calls.h"
 #include "sis3100/linux/sis1100_var.h"
 }
 #else
 #include <pthread.h>
 #include "sis3100/linux/sis3100_vme_calls.h"
 #include "sis3100/linux/sis1100_var.h"
 #endif

//! interrupt frontend handling class for SIS 3100 VME interface with veto gate and trigger generation
/*!
  This class provides an easy to use interface for handling interrupts with the Stuck SIS3100 VME 
  interface. When using this class create an instance and pass an unused device number to the 
  constructor and a function pointer to handle the interrupt. 

  The interrupt handling is done in a separate thread that waits with a timeout of 500 ms for a 
  SIGUSR1 signal from the SIS kernel driver. In case of a timeout the loop checks if a thread stop 
  signal from midas is present and then waits again for an interrupt. If an interrupt occurs the
  interrupt vector is loaded and a veto gate is produced by the @ref startVeto function on the 
  NIM port 1. Afterwards a user supplied function that takes the interrupt bitask, the highest 
  interrupt level and its vector as arguments is executed. After that the interrupts get acknowledged
  and the veto gate is closed.

  This class uses a separate device to avoid race conditions and for the low level interface to the
  irq setup.
 */
class sis3100 {
public:
  sis3100(int sisDeviceNum,  bool (*eventFunction)(int,int,int));
  bool enableFrontInterrupts();
  bool disbaleFrontInterrupts();
  bool enableVMEInterrupts();
  bool disbaleVMEInterrupts();

  bool enableOutputs();
  bool disableOutputs();

  static inline void startVeto(int p)       {s3100_control_write(p, 0x80, 0x00000001);}
  static inline void stopVeto(int p)        {s3100_control_write(p, 0x80, 0x00010000);}
  static inline void startVeto2(int p)      {s3100_control_write(p, 0x80, 0x00000002);}
  static inline void stopVeto2(int p)       {s3100_control_write(p, 0x80, 0x00020000);}
  static inline void generateTrigger(int p) {s3100_control_write(p, 0x80, 0x02000000);}

  inline bool isOpen() const {return (p==-1 ? false : true);}

  inline void setIsRunning(bool isRun){isRunning = isRun;}
private:
  int p; // SIS3100 device handler

  u_int32_t iobits;
  struct sis1100_irq_ctl irqctl;
  struct sis1100_irq_get irqget;
  struct sis1100_irq_ack irqack; 
  sigset_t mask, old_mask;

  bool isRunning;

  // for testing
#if HAVE_MIDAS__
  midas_thread_t thread;
#else
  pthread_t thread;
#endif

  static 
#if HAVE_MIDAS__
  INT
#else
  void*
#endif
 interruptThread(void *param);

  struct threadInit {
    bool (*eventFunction)(int, int, int);
    bool *isRunning;
    int p; // SIS3100 device handler
    u_int32_t *iobits;
    struct sis1100_irq_ctl *irqctl;
    struct sis1100_irq_get *irqget;
    struct sis1100_irq_ack *irqack; 
    sigset_t mask;
    struct timespec waitTime;
  };

  threadInit threadData;
};

#endif // compile guard
