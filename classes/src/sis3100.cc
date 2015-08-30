/*! 
  \file sis3100.cc
  \brief Struck SIS3100 VME interface interrupt source code
  \author Clemens Sauerzopf <clemens.sauerzopf@oeaw.ac.at>  
  \date 06.08.2014
  \version 1.0
*/ 

#include"sis3100.hh"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <errno.h>
#include<iostream>
#if HAVE_MIDAS__
#ifndef __MSYSTEM_H__
#define __MSYSTEM_H_
#include "msystem.h"
#endif

#define ERROR(msg,status) cm_msg(MERROR, "sis3100 interrupt error",msg ,status)
#define RET(x) return x
#else
#define ERROR(msg,status) printf(msg ,status)
#define RET(x) 
#endif

sis3100::sis3100(int sisDeviceNum, bool (*eventFunction)(int, int, int)){
  isRunning = false;
  // open SIS3100 device
  char tmp[128];
  sprintf(tmp,"/dev/sis1100_0%iremote",sisDeviceNum);
  p=open(tmp, O_RDWR, 0);


  // setup interrupt handling
  sigemptyset(&mask);
  sigaddset(&mask, SIGUSR1);

  threadData.eventFunction = eventFunction;
  threadData.isRunning = &isRunning;
  threadData.p = p; // SIS3100 device handler
  threadData.iobits = &iobits;
  threadData.irqctl = &irqctl;
  threadData.irqget = &irqget;
  threadData.irqack = &irqack; 
  threadData.mask = mask;
  threadData.waitTime.tv_sec = 0;
  threadData.waitTime.tv_nsec = 1000000*500; // 500 ms

  // Blocked signal is inherited by thread
  pthread_sigmask(SIG_BLOCK, &mask, &old_mask);
  enableOutputs();

#if HAVE_MIDAS__
  create_event_rb(0);
  thread = ss_thread_create(&interruptThread, (void *) &threadData);
#else
  pthread_create(&thread, NULL, &interruptThread, (void *) &threadData);
#endif

  irqctl.irq_mask=0;
}


bool sis3100::enableFrontInterrupts(){
  /* enable SIS3100 FRONT IRQS */
  irqctl.irq_mask |= SIS3100_FRONT_IRQS;
  irqctl.signal=SIGUSR1;
  if (ioctl(p, SIS1100_IRQ_CTL, &irqctl)<0) {
    ERROR("ioctl(SIS1100_IRQ_CTL): %s\n", strerror(errno));
    return false;    
  }
  return true;
}

bool sis3100::disbaleFrontInterrupts(){
  /* disable SIS3100 FRONT IRQS */
  irqctl.irq_mask &= ~SIS3100_FRONT_IRQS;
  irqctl.signal=0;
  if (ioctl(p, SIS1100_IRQ_CTL, &irqctl)<0) {
    ERROR("ioctl(SIS1100_IRQ_CTL): %s\n", strerror(errno));
    return false;    
  }
  return true;
}

bool sis3100::enableVMEInterrupts(){
  /* enable SIS3100 VME IRQS */
  irqctl.irq_mask |= SIS3100_VME_IRQS;
  irqctl.signal=SIGUSR1;
  if (ioctl(p, SIS1100_IRQ_CTL, &irqctl)<0) {
    ERROR("ioctl(SIS1100_IRQ_CTL): %s\n", strerror(errno));
    return false;    
  }
  return true;
}

bool sis3100::disbaleVMEInterrupts(){
  /* disable SIS3100 VME IRQS */
  irqctl.irq_mask &= ~SIS3100_VME_IRQS;
  irqctl.signal=0;
  if (ioctl(p, SIS1100_IRQ_CTL, &irqctl)<0) {
    ERROR("ioctl(SIS1100_IRQ_CTL): %s\n", strerror(errno));
    return false;    
  }
  return true;
}


bool sis3100::enableOutputs(){
  /* activate all outputs */
  iobits=0xff; /* see sis3100rem_front_io.c */
  if (ioctl(p, SIS1100_FRONT_IO, &iobits)<0) {
    ERROR("ioctl(SIS1100_FRONT_IO): %s\n", strerror(errno));
    return false;    
  }
  return true;
}

bool sis3100::disableOutputs(){
  /* deactivate all outputs */
  iobits=0xff<<16;
  if (ioctl(p, SIS1100_FRONT_IO, &iobits)<0) {
    ERROR("ioctl(SIS1100_FRONT_IO): %s\n", strerror(errno));
    return false;    
  }
  return true;
}

// static function!
#if HAVE_MIDAS__
  INT
#else
  void*
#endif
sis3100::interruptThread(void *param){
  threadInit *data = (threadInit*)param;
  
  siginfo_t sigInfo;
  

#if HAVE_MIDAS__
  signal_readout_thread_active(0,TRUE);
  while (is_readout_thread_enabled()) {
    if (!is_readout_thread_enabled())
      break;
#else
  while (true) {
#endif
    
    // waiting for SIGUSR1
    sigtimedwait(&(data->mask), &sigInfo, &(data->waitTime));
    if(sigInfo.si_signo == 0) continue; // timeout, used to read stop_readout_threads() in midas
    if(*(data->isRunning) == false) continue;
    //printf("1 SigNum: %i \n", sigInfo.si_signo);
    data->irqget->irq_mask=SIS3100_FRONT_IRQS;
    if (ioctl(data->p, SIS1100_IRQ_GET, data->irqget)<0) {
      ERROR("ioctl(SIS1100_IRQ_GET): %s\n", strerror(errno));
      RET(1);
    }

    //printf("2 remote status 0x%X\n",data->irqget->remote_status);
    if (data->irqget->remote_status!=0) {
      ERROR("o%sline\n", data->irqget->remote_status>0?"n":"ff");
      RET(1);  
    }
    /* read actual status */
    *(data->iobits)=0;
    if (ioctl(data->p, SIS1100_FRONT_IO, data->iobits)<0) {
      ERROR("ioctl(SIS1100_FRONT_IO): %s\n", strerror(errno));
      RET(1);    
    }

    // check if interrupt vector is present
    if(data->irqget->irqs == 0) continue;

    // start the VETO signal
    startVeto(data->p);
    //printf("execute event function\n");
    // execute user function and pass irq bits
    // interrupt bits, highest interrupt level , vector of highest interupt
    data->eventFunction(data->irqget->irqs, data->irqget->level, data->irqget->vector);

    // ack of ALL pending IRQs
    data->irqack->irq_mask=data->irqget->irqs/*&SIS3100_FRONT_IRQS*/;
    if (ioctl(data->p, SIS1100_IRQ_ACK, data->irqack)<0) {
      ERROR("ioctl(SIS1100_IRQ_ACK): %s\n", strerror(errno));
      RET(1);
    }

    // start the VETO signal
    stopVeto(data->p);
  }

  /* tell framework that we are finished */
#if HAVE_MIDAS__
  signal_readout_thread_active(0,FALSE);
  RET(0);
#else
  return NULL;
#endif
}
