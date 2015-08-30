/*********************************************************************

  Name:         v1742.c
  Created by:   Clemens Sauerzopf
  Based on:     v1740.c by Pierre-A. Amaudruz / K.Olchanski

  Contents:     V1742 32 ch, 5 MS
 
*********************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "v1742drv.h"
#include "v1742.h"
#include "mvmestd.h"

// Buffer organization map for number of samples
uint32_t V1742_NSAMPLES_MODE[11] = { (1024*128), (512*128), (256*128), (128*128), (64*128), (32*128),(16*128), (8*128), (4*128), (2*128), (128)};

/*****************************************************************/
/*
Read V1742 register value
*/
static uint32_t regRead(MVME_INTERFACE *mvme, uint32_t base, int offset)
{
  mvme_set_am(mvme, MVME_AM_A32);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  return mvme_read_value(mvme, base + offset);
}

/*****************************************************************/
/*
Write V1742 register value
*/
static void regWrite(MVME_INTERFACE *mvme, uint32_t base, int offset, uint32_t value)
{
  mvme_set_am(mvme, MVME_AM_A32);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  mvme_write_value(mvme, base + offset, value);
}

/*****************************************************************/
uint32_t v1742_RegisterRead(MVME_INTERFACE *mvme, uint32_t base, int offset)
{
  return regRead(mvme, base, offset);
}

/*****************************************************************/
void v1742_RegisterWrite(MVME_INTERFACE *mvme, uint32_t base, int offset, uint32_t value)
{
  regWrite(mvme, base, offset, value);
}

/*****************************************************************/
void v1742_Reset(MVME_INTERFACE *mvme, uint32_t base)
{
  regWrite(mvme, base, V1742_SW_RESET, 0);
}

/*****************************************************************/
void v1742_TrgCtl(MVME_INTERFACE *mvme, uint32_t base, uint32_t reg, uint32_t mask)
{
  regWrite(mvme, base, reg, mask);
}

/*****************************************************************/
void v1742_GroupCtl(MVME_INTERFACE *mvme, uint32_t base, uint32_t reg, uint32_t mask)
{
  regWrite(mvme, base, reg, mask);
}

/*****************************************************************/
void v1742_GroupSet(MVME_INTERFACE *mvme, uint32_t base, uint32_t channel, uint32_t what, uint32_t that)
{
  uint32_t reg, mask;

  if (what == V1742_GROUP_THRESHOLD)   mask = 0x0FFF;
  if (what == V1742_GROUP_DAC)         mask = 0xFFFF;
  reg = what | (channel << 8);
  //  printf("base:0x%x reg:0x%x, this:%x\n", base, reg, that);
  regWrite(mvme, base, reg, (that & mask));
  that = regRead(mvme, base, (V1742_GROUP_STATUS | (channel << 8)));
  //  printf("Group %d  status:0x%x\n", channel, that);
}

/*****************************************************************/
uint32_t v1742_GroupGet(MVME_INTERFACE *mvme, uint32_t base, uint32_t channel, uint32_t what)
{
  uint32_t reg, mask;

  if (what == V1742_GROUP_THRESHOLD)   mask = 0x0FFF;
  if (what == V1742_GROUP_DAC)         mask = 0xFFFF;
  reg = what | (channel << 8);
  return regRead(mvme, base, reg);
}

/*****************************************************************/
void v1742_AcqCtl(MVME_INTERFACE *mvme, uint32_t base, uint32_t operation)
{
  uint32_t reg;
  
  reg = regRead(mvme, base, V1742_ACQUISITION_CONTROL);  
  switch (operation) {
  case V1742_RUN_START:
    regWrite(mvme, base, V1742_ACQUISITION_CONTROL, (reg | 0x4));
    break;
  case V1742_RUN_STOP:
    regWrite(mvme, base, V1742_ACQUISITION_CONTROL, (reg & ~(0x4)));
    break;
  case V1742_COUNT_ALL_TRIGGER:
    regWrite(mvme, base, V1742_ACQUISITION_CONTROL, (reg | 0x08));
    break;
  case V1742_COUNT_ACCEPTED_TRIGGER:
    regWrite(mvme, base, V1742_ACQUISITION_CONTROL, (reg & ~(0x08)));
    break;
  case V1742_ONE_BUFFER_FREE:
    regWrite(mvme, base, V1742_ACQUISITION_CONTROL, (reg | 0x20));
    break;
  case V1742_NORMAL_MODE:
    regWrite(mvme, base, V1742_ACQUISITION_CONTROL, (reg & ~(0x20)));
    break;
  default:
    break;
  }
}

void v1742_SetSampling(MVME_INTERFACE *mvme, uint32_t base, uint32_t operation){
  uint32_t reg;
  
  reg = regRead(mvme, base, V1742_SAMPING_FREQUENCY);  
  switch (operation) {
  case V1742_SAMPLING_5GS:
    regWrite(mvme, base, V1742_SAMPING_FREQUENCY, (reg & ~(0x3)));
    break;
  case V1742_SAMPLING_2_5GS:
    regWrite(mvme, base, V1742_SAMPING_FREQUENCY, (reg & ~(0x3)));
    regWrite(mvme, base, V1742_SAMPING_FREQUENCY, (reg | 0x1));
    break;
  case V1742_SAMPLING_1GS:
    regWrite(mvme, base, V1742_SAMPING_FREQUENCY, (reg & ~(0x3)));
    regWrite(mvme, base, V1742_SAMPING_FREQUENCY, (reg | 0x2));
    break;
  default:
    break;
  }
}

/*****************************************************************/
void v1742_GroupConfig(MVME_INTERFACE *mvme, uint32_t base, uint32_t operation)
{
  uint32_t reg;
  
  reg = regRead(mvme, base, V1742_GROUP_CONFIG);  

  switch (operation) {
  case V1742_TRIGGER_FALL:
    regWrite(mvme, base, V1742_GROUP_CONFIG, (reg | 0x40));
    break;
  case V1742_TRIGGER_RISE:
    regWrite(mvme, base, V1742_GROUP_CONFIG, (reg & ~(0x40)));
    break;
  case V1742_TEST_MODE_ON:
    regWrite(mvme, base, V1742_GROUP_CONFIG, (reg | 0x08));
    break;
  case V1742_TEST_MODE_OFF:
    regWrite(mvme, base, V1742_GROUP_CONFIG, (reg & ~(0x08)));
    break;
  case V1742_TRIGGER_READOUT_ON:
    regWrite(mvme, base, V1742_GROUP_CONFIG, (reg | 0x800));
    break;
  case V1742_TRIGGER_READOUT_OFF:
    regWrite(mvme, base, V1742_GROUP_CONFIG, (reg & ~(0x800)));
    break;
  case V1742_LOCAL_TRIGGER_ON:
    regWrite(mvme, base, V1742_GROUP_CONFIG, (reg | 0x1000));
    break;
  case V1742_LOCAL_TRIGGER_OFF:
    regWrite(mvme, base, V1742_GROUP_CONFIG, (reg & ~(0x1000)));
    break;
  default:
    break;
  }
  //printf("Channel_config2: 0x%x\n", regRead(mvme, base, V1742_GROUP_CONFIG));  
}

/*****************************************************************/
void v1742_info(MVME_INTERFACE *mvme, uint32_t base, int *ngroups, uint32_t *n32word)
{
  int i, grpmask;

  // Evaluate the event size
  // Number of samples per group (8 channels)
  *n32word = 8 * V1742_NSAMPLES_MODE[regRead(mvme, base, V1742_BUFFER_ORGANIZATION)];
  printf("n32words per group = %i\n", *n32word);
  // times the number of active group
  grpmask = 0xff & regRead(mvme, base, V1742_GROUP_EN_MASK); 
  *ngroups = 0;
  for (i=0;i<8;i++) {
    if (grpmask & (1<<i))
      *ngroups += 1;
  }
  //printf("groups: %i\n", *ngroups);
  *n32word = *n32word * *ngroups * 12 / 32;  // 12bit/Sample , 32bits/DW
  *n32word += 4;    // Headers
  printf("n32words total = %i\n", *n32word);
}

/*****************************************************************/
uint32_t v1742_BufferOccupancy(MVME_INTERFACE *mvme, uint32_t base, uint32_t group)
{
  uint32_t reg;
  reg = V1742_GROUP_BUFFER_OCCUPANCY + (group<<8);
  //printf("GROUP_BUFFER_OCCUPANCY  = %i\n",regRead(mvme, base, reg));
  return regRead(mvme, base, reg);
}

uint32_t v1742_DataRead(MVME_INTERFACE *mvme, uint32_t base, uint32_t *pdata, uint32_t n32w)
{
  uint32_t i;

  mvme_set_am(  mvme, MVME_AM_A32);
  mvme_set_dmode(  mvme, MVME_DMODE_D32);
  for (i=0;i<n32w;i++) {
    *pdata = regRead(mvme, base, V1742_EVENT_READOUT_BUFFER);
    //printf("DataRead base:0x%x add:0x%x idx:%d 0x%x\n", base, V1742_EVENT_READOUT_BUFFER, i, *pdata);
    if (*pdata != 0xffffffff)
      pdata++;
    else
      break;
  }
  return i;
}

/********************************************************************/
/** v1742_DataBlockRead
Read N entries (32bit) 
@param mvme vme structure
@param base  base address
@param pdest Destination pointer
@return nentry
*/
uint32_t v1742_DataBlockRead(MVME_INTERFACE *mvme, uint32_t base, uint32_t *pdest, uint32_t nentry)
{
  int status, ngroups;
  //printf("ASDASD\n");
  mvme_set_am(  mvme, MVME_AM_A32);
  mvme_set_dmode(  mvme, MVME_DMODE_D32);
  //printf("DataBlockRead n32w:%d 0x%x\n", *nentry, *nentry);
  mvme_set_blt(  mvme, MVME_BLT_MBLT64);
  //mvme_set_blt(  mvme, MVME_BLT_2EVME);
  //mvme_set_blt(  mvme, MVME_BLT_2EVME);
  //
  // Transfer in MBLT64 (8bytes), nentry is in 32bits
  //printf("*nentry<<2 = %i, 0x%x", *nentry<<2, *nentry<<2);
  //status = mvme_read(mvme, pdest, base+V1742_EVENT_READOUT_BUFFER, *nentry<<2);
  //status = mvme_read(mvme, pdest, base+V1742_EVENT_READOUT_BUFFER, *nentry*4);
  int n32w = 1024, readw32=0;
  while(nentry >1024){
    status = mvme_read(mvme, pdest, base+V1742_EVENT_READOUT_BUFFER,  n32w*4);
    nentry -= n32w;
    pdest += n32w;
    readw32 += n32w;
    }
  status = mvme_read(mvme, pdest, base+V1742_EVENT_READOUT_BUFFER,  nentry*4);
  nentry=readw32+nentry;
  mvme_set_blt(  mvme, 0);
  if (status != MVME_SUCCESS)
    return 0;

  return nentry;
}

static int read_flash_page(MVME_INTERFACE *mvme, uint32_t base, 
			   uint8_t gr, int8_t* page, uint32_t pagenum) { 
  uint32_t flash_addr;
  uint16_t	   dd;
  uint32_t i,tmp[528];
  uint32_t   fl_a[528];
  uint8_t addr0,addr1,addr2;
  int ret;
  
  flash_addr = pagenum<<9;
  addr0 = (uint8_t)flash_addr;
  addr1 = (uint8_t)(flash_addr>>8);
  addr2 = (uint8_t)(flash_addr>>16);

  dd=0xffff;
  while ((dd>>2)& 0x1)  
    dd = (int16_t)regRead(mvme, base, V1742_STATUS(gr));

  // enable flash (NCS = 0)
  regWrite(mvme, base, V1742_SEL_FLASH(gr),(int16_t)1);

  // write opcode
  regWrite(mvme, base, V1742_FLASH(gr),(int16_t)V1742_MAIN_MEM_PAGE_READ);

  // write address
  dd=0xffff;
  regWrite(mvme, base, V1742_FLASH(gr),(int16_t)addr2);

  dd=0xffff;
  regWrite(mvme, base, V1742_FLASH(gr),(int16_t)addr1);

  dd=0xffff;
  regWrite(mvme, base, V1742_FLASH(gr),(int16_t)addr0);

  // additional don't care bytes
  for (i=0; i<4; i++) {
    dd=0xffff;
    regWrite(mvme, base, V1742_FLASH(gr),(int16_t)0);

  }
  
  for (i=0; i<528; i+=2) {
    fl_a[i] = V1742_FLASH(gr);
    fl_a[i+1] = V1742_STATUS(gr);
  }	  

  for (i=0; i<528; i+=2) {
    while((v1742_check_SPI(mvme, base,gr>>8))) usleep(100);
    page[(int)(i/2)] = (int8_t) regRead(mvme, base, fl_a[i]);
  }

  // disable flash (NCS = 1)
  regWrite(mvme, base, V1742_SEL_FLASH(gr),(int16_t)0);

  return 0;
}


void v1742_LoadCorrectionTables(MVME_INTERFACE *mvme, uint32_t base, V1742_DataCorrection_t *Table, uint8_t group, uint32_t frequency){
  uint32_t pagenum = 0,i,n,j,start;
  int8_t TempCell[264]; // 
  int8_t *p;
  int ret;
  int8_t tmp[0x1000]; // 256byte * 16 pagine
  for (n=0;n<V1742_MAX_CHANNELS+1;n++) {
    pagenum = 0;
    pagenum = (group %2) ? 0xC00: 0x800;
    pagenum |= frequency << 8;
    pagenum |= n << 2;
    // load the Offset Cell Correction
    p = TempCell;
    start = 0;
    for (i=0;i<4;i++) {
      int endidx = 256;
      if ((ret =read_flash_page(mvme,base,group,p,pagenum)) != 0) 
	return ret;
      // peak correction
      for (j=start;j<(start+256);j++) {
	if (p[j-start] != 0x7f) {
	  Table->cell[n][j] = p[j-start];
	}
	else {
	  short cel = (short)((((unsigned char)(p[endidx+1])) << 0x08) |((unsigned char) p[endidx]));
	  if (cel == 0) Table->cell[n][j] = p[j-start]; else Table->cell[n][j] = cel;
	  endidx+=2;
	  if (endidx > 263) endidx = 256;
	}
      }
      start +=256;
      pagenum++;
    }
    start = 0;
    // load the Offset Num Samples Correction
    p = TempCell;
    pagenum &= 0xF00;
    pagenum |= 0x40;
    pagenum |= n << 2;
		
    for (i=0;i<4;i++) {
      if ((ret =read_flash_page(mvme,base,group,p,pagenum)) != 0) // FIX
	return ret;
      for (j=start;j<start+256;j++) Table->nsample[n][j] = p[j-start];
      start +=256;
      pagenum++;
    }
    if (n == V1742_MAX_CHANNELS) {
      // load the Time Correction
      p = TempCell;
      pagenum &= 0xF00;
      pagenum |= 0xA0;
      start = 0;
      for (i=0;i<16;i++) {
	if ((ret =read_flash_page(mvme,base,group,p,pagenum)) != 0)  // FIX
	  return ret;
	for (j=start;j<start+256;j++) tmp[j] = p[j-start];
	start +=256;
	pagenum++;
      }
      for (i=0;i<1024;i++) {
	p = (int8_t *) &(Table->time[i]);
	p[0] = tmp[i*4];
	p[1] = tmp[(i*4)+1];
	p[2] = tmp[(i*4)+2];
	p[3] = tmp[(i*4)+3];
      }
    }
  }
  return 0;
}


/*****************************************************************/
void  v1742_Status(MVME_INTERFACE *mvme, uint32_t base)
{
  printf("================================================\n");
  printf("V1742 at A32 0x%x\n", (int)base);
  printf("Board ID           (0xEF08)  : 0x%x\n", regRead(mvme, base, V1742_BOARD_ID));
  printf("Board Info         (0x8140)  : 0x%x\n", regRead(mvme, base, V1742_BOARD_INFO));
  printf("ROC FPGA FW Rev    (0x8124)  : 0x%x\n", regRead(mvme, base, V1742_ROC_FPGA_FW_REV));
  printf("Group Config       (0x8000)  : 0x%8.8x\n", regRead(mvme, base, V1742_GROUP_CONFIG));
  printf("Group0 Threshold   (0x1080)  : 0x%8.8x\n", regRead(mvme, base, V1742_GROUP_THRESHOLD));
  printf("Group0 DAC         (0x1098)  : 0x%8.8x\n", regRead(mvme, base, V1742_GROUP_DAC));
  printf("Group0 Ch Trg Mask (0x10A8)  : 0x%8.8x\n", regRead(mvme, base, V1742_GROUP_CH_TRG_MASK));
  printf("Acquisition control(0x8100)  : 0x%8.8x\n", regRead(mvme, base, V1742_ACQUISITION_CONTROL));
  printf("Acquisition status (0x800C)  : 0x%8.8x\n", regRead(mvme, base, V1742_ACQUISITION_STATUS));
  printf("Trg Src En Mask    (0x810C)  : 0x%8.8x\n", regRead(mvme, base, V1742_TRIG_SRCE_EN_MASK));
  printf("FP Trg Out En Mask (0x8110)  : 0x%8.8x\n", regRead(mvme, base, V1742_FP_TRIGGER_OUT_EN_MASK));
  printf("FP IO Control      (0x811C)  : 0x%8.8x\n", regRead(mvme, base, V1742_FP_IO_CONTROL));
  printf("Group Enable Mask  (0x8120)  : 0x%8.8x\n", regRead(mvme, base, V1742_GROUP_EN_MASK));
  printf("VME Control        (0xEF00)  : 0x%8.8x\n", regRead(mvme, base, V1742_VME_CONTROL));
  printf("VME Status         (0xEF04)  : 0x%8.8x\n", regRead(mvme, base, V1742_VME_STATUS));
  printf("================================================\n");
}


void v1742_VME_Control(MVME_INTERFACE *mvme, uint32_t base, int operation){
  uint32_t reg;
  reg = regRead(mvme, base, V1742_VME_CONTROL);  
  switch (operation){
  case V1742_OPTICAL_INTERRUPT_ON:
    regWrite(mvme, base, V1742_VME_CONTROL, (reg | 0x8));
    break;
  case V1742_OPTICAL_INTERRUPT_OFF:
    regWrite(mvme, base, V1742_VME_CONTROL, (reg & ~(0x8)));
    break;
  default:
    break;
  }
}

void v1742_VME_Interrupt_Level(MVME_INTERFACE *mvme, uint32_t base, int level){
  uint32_t reg;
  reg = regRead(mvme, base, V1742_VME_CONTROL); 
  if(level < 8 && level > 0){
    regWrite(mvme, base, V1742_VME_CONTROL, (reg & ~(0x7)));
    regWrite(mvme, base, V1742_VME_CONTROL, (reg | level));
  }
}

int v1742_VME_Get_Status(MVME_INTERFACE *mvme, uint32_t base){
  return regRead(mvme, base, V1742_VME_STATUS);
}

void v1742_Set_Interrupt_Event_Num(MVME_INTERFACE *mvme, uint32_t base, int num){
  if (num < 1024 && num >0){
    regWrite(mvme, base,V1742_INTERRUPT_EVT_NB, num);
  }
}

int v1742_Get_Interrupt_Event_Num(MVME_INTERFACE *mvme, uint32_t base, int num){
  return regRead(mvme, base,V1742_INTERRUPT_EVT_NB);
}

int  v1742_Setup(MVME_INTERFACE *mvme, uint32_t base, int mode)
{
  switch (mode) {
  case 0x0:
    printf("--------------------------------------------\n");
    printf("Setup Skip\n");
    printf("--------------------------------------------\n");
  case 0x1:
    printf("--------------------------------------------\n");
    printf("Basesetup, 4 groups, low latency TRn Triggers\n");
    printf("--------------------------------------------\n");

    /*Setting group config: rising edge trigger, individual trigger, local trigger
     all fast trigger, trigger not in readout included*/
    v1742_RegisterWrite(mvme, base,V1742_GROUP_CONFIG,0x10001110);

    /*Count accepted triggers only*/
    //v1742_RegisterWrite(mvme, base,V1742_ACQUISITION_CONTROL,0x4);

    /*Accept software and external triggers*/
    v1742_RegisterWrite(mvme, base,V1742_TRIG_SRCE_EN_MASK,0xC0000000);

    /*Accept local triggers from frontpanel input*/
    v1742_RegisterWrite(mvme, base,V1742_FP_TRIGGER_OUT_EN_MASK,0x0000000F);

    /*Enable all groups*/
    v1742_RegisterWrite(mvme, base,V1742_GROUP_EN_MASK,0x0000000F);

    /*Set DC trigger offset to 0V (measuremnt +/- 0.5V)*/
    v1742_RegisterWrite(mvme, base,V1742_GROUP_TR_DC_OFFSET,0x8000);

    /*Set trigger treshold to -200mV*/
    v1742_RegisterWrite(mvme, base,V1742_GROUP_TR_THRESHOLD,0x613E);

    v1742_RegisterWrite(mvme, base,0xEF1C,0);

    printf("\n");
    break;
  default:
    printf("Unknown setup mode\n");
    return -1;
  }
  //v1742_Status(mvme, base);
  return 0;
}

/* check_SPI returns 0 for good and 1 for spi busy */
uint32_t v1742_check_SPI(MVME_INTERFACE *mvme, uint32_t base, int group){
  if(group<0 || group>3) return -1;

  int status = v1742_RegisterRead(mvme, base,V1742_GROUP_STATUS + (group<<8));
  return (status >> 2) & 0x1;
}

/*Sets the offset for the low latency triggers TR1 and TR2 offset is in 
  arbitraty units, the TRn inputs ATTENUATE the signal by a factor of 2 !!!
  center is at 0x8000*/
uint32_t v1742_trigger_offset(MVME_INTERFACE *mvme, uint32_t base, int trigger, 
			      double offset){
printf("\n\n\n --------------------TRIGGER--------------------\n\n\n");
  if(trigger != 0 && trigger != 1) return -1;
  //if(offset < -1 || offset > 1) return -2;

  /************* WARNING *************/
  /***   DATA SHEET NOT ACCUTATE!  ***/
  
  /* We are assuming that the whole range  of the 16 bit DAC covers
     -1 to +1 Volt*/
  /*const int k = 0x8000;
    int value = offset*k + k;*/
  int value = offset;
  if(value < 0 || value > 0xFFFF) return -3;

  /* check_SPI returns 0 for good and 1 for busy */
  if(trigger == 0){
    while(!((!v1742_check_SPI(mvme, base,0)) && 
	    (!v1742_check_SPI(mvme, base,1))    )) usleep(1000); /* 1 ms */
    v1742_RegisterWrite(mvme, base,V1742_GROUP_TR_DC_OFFSET,value);}
  else if(trigger == 1){
    while(!((!v1742_check_SPI(mvme, base,2)) && 
	    (!v1742_check_SPI(mvme, base,3))    )) usleep(1000); /* 1 ms */
    v1742_RegisterWrite(mvme, base,V1742_GROUP_TR_DC_OFFSET+0x200,value);}
  else return -4;

  return 0;
}

/* Sets the threshold for the low latency triggers TR1 and TR2 offset is 
   in arbitraty units */
uint32_t v1742_trigger_threshold(MVME_INTERFACE *mvme, uint32_t base, int trigger, 
				 double threshold){
  if(trigger != 0 && trigger != 1) return -1;
  /*if(threshold < -1 || threshold > 1) return -2;*/

  /************* WARNING *************/
  /***   DATA SHEET NOT ACCUTATE!  ***/

  /*We assume that the whole range of the 16 bit DAC covers -2 to +2 Volt*/
  /*const int k = 0x8000/2;
    int value = threshold*k + k;*/
  int value = threshold;
  if(value < 0 || value > 0xFFFF) return -3;
  
  /* check_SPI returns 0 for good and 1 for busy */
  if(trigger == 0){
    while(!((!v1742_check_SPI(mvme, base,0)) && 
	    (!v1742_check_SPI(mvme, base,1))    )) usleep(1000); /* 1 ms */
    v1742_RegisterWrite(mvme, base, V1742_GROUP_TR_THRESHOLD,value);}
  else if(trigger == 1){
    while(!((!v1742_check_SPI(mvme, base,2)) && 
	    (!v1742_check_SPI(mvme, base,3))    )) usleep(1000); /* 1 ms */
    v1742_RegisterWrite(mvme, base, V1742_GROUP_TR_THRESHOLD+0x200,
			value);}
  else return -4;

  return 0;
}

/*Sets the channel offset in arbitrary units*/
uint32_t v1742_set_channel_offset(MVME_INTERFACE *mvme, uint32_t base, int ch, 
			      uint32_t offset){
  int g;
  if(ch >= 0 && ch < 8)
    g = 0;
  else if(ch >= 8 && ch < 16) 
    g = 0x100;
  else if(ch >= 16 && ch < 24)
    g = 0x200;
  else if(ch >= 24 && ch < 32)
    g = 0x300;
  else return -1;

  if(offset > 0xFFFF) return -2;

  int val = ((ch%8) << 16) + offset;
  //printf("\n\n------------ 0x%x 0x%x ---------------- ch = %u\n\n", V1742_GROUP_DAC+g, g>>8,ch);

  /* check_SPI returns 0 for good and 1 for busy */
  while((v1742_check_SPI(mvme, base,g>>8))) usleep(1000); /* 1 ms */ 
  v1742_RegisterWrite(mvme, base,V1742_GROUP_DAC+g, val);
  //printf("-------------------------------\nasdasd 0x%x 0x%x asdasd\n----------------------\n",offset, val);
  return 0;  
}

/* returns the channel offset */
uint32_t v1742_get_channel_offset(MVME_INTERFACE *mvme, uint32_t base, int ch){
  int g;
  if(ch >= 0 && ch < 8)
    g = 0;
  else if(ch >= 8 && ch < 16) 
    g = 0x100;
  else if(ch >= 16 && ch < 24)
    g = 0x200;
  else if(ch >= 24 && ch < 32)
    g = 0x300;
  else return -1;

  v1742_RegisterWrite(mvme, base,V1742_GROUP_DAC_SEL+g, ch%8);
  return v1742_RegisterRead(mvme, base, V1742_GROUP_DAC+g);
}

/* sets the digitalisation window, valid values are from 0 to 127 
   where each step is about 8.5 ns to the future, returnvalue is new value */
uint32_t v1742_set_window(MVME_INTERFACE *mvme, uint32_t base, int val){
  if(val < 0 || val > 0x7F) return -1;
  
  v1742_RegisterWrite(mvme, base, V1742_POST_TRIGGER_SETTING, val);
  return val;
}

/*****************************************************************/
/*For test purpose only */
#ifdef MAIN_ENABLE
int main (int argc, char* argv[]) {

  uint32_t V1742_BASE  = 0x11110000;

  MVME_INTERFACE *myvme;

  uint32_t data[100000], n32word, n32read, nevt;
  int status, group, i, ngroups, chanmask;

  if (argc>1) {
    sscanf(argv[1],"%x", &V1742_BASE);
    printf("Base:0x%x\n", V1742_BASE);
  }

  // Test under vmic
  status = mvme_open(&myvme, 0);
  v1742_Reset(myvme, V1742_BASE);
  v1742_Setup(myvme, V1742_BASE, 1);
  // Run control by register
  //v1742_AcqCtl(myvme, V1742_BASE, V1742_REGISTER_RUN_MODE);
  // Soft or External trigger
  v1742_TrgCtl(myvme, V1742_BASE, V1742_TRIG_SRCE_EN_MASK     , V1742_EXTERNAL_TRIGGER|V1742_SOFT_TRIGGER);
  // Soft and External trigger output
  v1742_TrgCtl(myvme, V1742_BASE, V1742_FP_TRIGGER_OUT_EN_MASK, V1742_EXTERNAL_TRIGGER|V1742_SOFT_TRIGGER);
  // Enabled groups
  v1742_GroupCtl(myvme, V1742_BASE, V1742_GROUP_EN_MASK, 0xF);
  //  sleep(1);

  group = 0;
  // Start run then wait for trigger
  v1742_AcqCtl(myvme, V1742_BASE, V1742_RUN_START);
  sleep(1);
  regWrite(myvme, V1742_BASE, V1742_SOFTWARE_TRIGGER, 1); 
  //sleep(1);
  regWrite(myvme, V1742_BASE, V1742_SOFTWARE_TRIGGER, 2); 
  //sleep(1);
  regWrite(myvme, V1742_BASE, V1742_SOFTWARE_TRIGGER, 3); 
  //sleep(1);
  // Evaluate the event size
  // Number of samples per groups
  n32word  =  1<<regRead(myvme, V1742_BASE, V1742_BUFFER_ORGANIZATION);
  // times the number of active groups
  chanmask = 0xff & regRead(myvme, V1742_BASE, V1742_GROUP_EN_MASK); 
  for (ngroups=0, i=0;i<8;i++) 
    if (chanmask & (1<<i))
      ngroups++;
  printf("chanmask : %x translante in %d groups of 8 channels each\n", chanmask, ngroups);
  n32word *= ngroups;
  n32word /= 2;   // 2 samples per 32bit word
  n32word += 4;   // Headers
  printf("Expected n32word:%d\n", n32word);

  printf("Occupancy for group %d = %d\n", group, v1742_BufferOccupancy(myvme, V1742_BASE, group)); 

#if 0
  do {
    status = regRead(myvme, V1742_BASE, V1742_ACQUISITION_STATUS);
    printf("Acq Status1:0x%x\n", status);
  } while ((status & 0x8)==0);
  
  n32read = v1742_DataRead(myvme, V1742_BASE, &(data[0]), n32word);
  printf("n32read:%d\n", n32read);
  
  for (i=0; i<n32read;i+=4) {
    printf("[%d]:0x%x - [%d]:0x%x - [%d]:0x%x - [%d]:0x%x\n"
	   , i, data[i], i+1, data[i+1], i+2, data[i+2], i+3, data[i+3]);
  }
  
  v1742_AcqCtl(myvme, V1742_BASE, V1742_RUN_STOP);
#endif

  // Read Data
  nevt = regRead(myvme, V1742_BASE, V1742_EVENT_STORED);
  n32word = regRead(myvme, V1742_BASE, V1742_EVENT_SIZE);
  printf("nevt:%d n32word:%d\n", nevt, n32word);
      
#if 1
  int j;
  for(j=0;j<nevt; j++)
    n32read = v1742_DataRead(myvme, V1742_BASE, &(data[0]), n32word);
#endif
#if 0
  for(int j=0,j<nevt, j++)
  n32read = v1742_DataBlockRead(myvme, V1742_BASE, &(data[j]), &n32word);
#endif
  printf("n32read:%d  n32requested:%d\n", n32read, n32word);
  
  for (i=0; i<32;i+=4) { // n32read;i+=4) {
    printf("[%d]:0x%x - [%d]:0x%x - [%d]:0x%x - [%d]:0x%x\n"
	   , i, data[i], i+1, data[i+1], i+2, data[i+2], i+3, data[i+3]);
  }
  
  v1742_AcqCtl(myvme, V1742_BASE, V1742_RUN_STOP);
  
  regRead(myvme, V1742_BASE, V1742_ACQUISITION_CONTROL);
  status = mvme_close(myvme);

  return 1;

}
#endif

