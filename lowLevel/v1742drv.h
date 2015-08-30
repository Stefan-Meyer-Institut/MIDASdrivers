/*********************************************************************

  Name:         v1742drv.h
  Created by:   Clemens Sauerzopf
  Based on:     v1740drv.h by K.Olchanski

  Contents:     v1742 32-channel 5 GS 12-bit ADC

*********************************************************************/
#ifndef  V1742DRV_INCLUDE_H
#define  V1742DRV_INCLUDE_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mvmestd.h"

#include "v1742.h"

uint32_t v1742_RegisterRead           (MVME_INTERFACE *mvme, uint32_t a32base, int offset);
uint32_t v1742_BufferOccupancy        (MVME_INTERFACE *mvme, uint32_t a32base, uint32_t channel);

void     v1742_AcqCtl                 (MVME_INTERFACE *mvme, uint32_t a32base, uint32_t operation);
void     v1742_SetSampling            (MVME_INTERFACE *mvme, uint32_t base, uint32_t operation);
void     v1742_GroupCtl               (MVME_INTERFACE *mvme, uint32_t a32base, uint32_t reg, uint32_t mask);
void     v1742_TrgCtl                 (MVME_INTERFACE *mvme, uint32_t a32base, uint32_t reg, uint32_t mask);

void     v1742_RegisterWrite          (MVME_INTERFACE *mvme, uint32_t a32base, int offset, uint32_t value);
void     v1742_Reset                  (MVME_INTERFACE *mvme, uint32_t a32base);

void     v1742_Status                 (MVME_INTERFACE *mvme, uint32_t a32base);
int      v1742_Setup                  (MVME_INTERFACE *mvme, uint32_t a32base, int mode);
void     v1742_info                   (MVME_INTERFACE *mvme, uint32_t a32base, int *nch, uint32_t *n32w);
uint32_t v1742_DataRead               (MVME_INTERFACE *mvme,uint32_t a32base, uint32_t *pdata, uint32_t n32w);
uint32_t v1742_DataBlockRead          (MVME_INTERFACE *mvme, uint32_t a32base, uint32_t *pdest, uint32_t nentry);
void     v1742_GroupThresholdSet      (MVME_INTERFACE *mvme, uint32_t base, uint32_t channel, uint32_t threshold);
void     v1742_GroupOUThresholdSet    (MVME_INTERFACE *mvme, uint32_t base, uint32_t channel, uint32_t threshold);
void     v1742_GroupDACSet            (MVME_INTERFACE *mvme, uint32_t base, uint32_t channel, uint32_t dac);
int      v1742_GroupDACGet            (MVME_INTERFACE *mvme, uint32_t base, uint32_t channel, uint32_t *dac);
void     v1742_GroupSet               (MVME_INTERFACE *mvme, uint32_t base, uint32_t channel, uint32_t what, uint32_t that);
uint32_t v1742_GroupGet               (MVME_INTERFACE *mvme, uint32_t base, uint32_t channel, uint32_t what);
void     v1742_GroupConfig            (MVME_INTERFACE *mvme, uint32_t base, uint32_t operation);
void     v1742_VME_Control            (MVME_INTERFACE *mvme, uint32_t base, int operation);

uint32_t v1742_set_channel_offset     (MVME_INTERFACE *mvme, uint32_t base, int ch, 
				       uint32_t offset);
uint32_t v1742_trigger_threshold      (MVME_INTERFACE *mvme, uint32_t base, int trigger, 
				       double threshold);
uint32_t v1742_trigger_offset         (MVME_INTERFACE *mvme, uint32_t base, int trigger, 
				       double offset);
uint32_t v1742_check_SPI              (MVME_INTERFACE *mvme, uint32_t base, int group);
uint32_t v1742_get_channel_offset     (MVME_INTERFACE *mvme, uint32_t base, int ch);
uint32_t v1742_set_window             (MVME_INTERFACE *mvme, uint32_t base, int val);
void     v1742_VME_Interrupt_Level    (MVME_INTERFACE *mvme, uint32_t base, int level);
int      v1742_VME_Get_Status         (MVME_INTERFACE *mvme, uint32_t base);
void     v1742_Set_Interrupt_Event_Num(MVME_INTERFACE *mvme, uint32_t base, int num);
int      v1742_Get_Interrupt_Event_Num(MVME_INTERFACE *mvme, uint32_t base, int num);
void     v1742_LoadCorrectionTables   (MVME_INTERFACE *mvme, uint32_t base, V1742_DataCorrection_t *Table,
				       uint8_t group, uint32_t frequency);
#endif // v1742DRV_INCLUDE_H
