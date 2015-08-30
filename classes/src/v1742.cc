/*! 
  \file v1742.cc
  \brief CAEN V1742 source code
  \author Clemens Sauerzopf <clemens.sauerzopf@oeaw.ac.at>  
  \date 06.08.2014
  \version 1.0
*/ 


#include"v1742.hh"
#include<iostream>
#include <time.h>

bool v1742::init(){
  if(!checkModuleResponse()) return false;

  bool retVal = true;
  retVal &= setupV1742();
  retVal &= setupV1742ODB();
  haveTables = loadCorrectionTables();
  retVal &= haveTables;
  
  return retVal;
}

bool v1742::setupV1742(){

  bool retVal = true;

  // perform board reset
  v1742_Reset          (vme, vmeBaseAdress                          );

  // load standard setup
  v1742_Setup(vme, vmeBaseAdress, 1                                 );

  // set fast trigger readout to ON
  v1742_GroupConfig    (vme, vmeBaseAdress, V1742_TRIGGER_READOUT_ON);

  // trigger on falling edge
  v1742_GroupConfig    (vme, vmeBaseAdress, V1742_TRIGGER_FALL      );

  // clear buffers
  v1742_RegisterWrite  (vme, vmeBaseAdress, V1742_SW_CLEAR, 1       );

  v1742_set_window(vme, vmeBaseAdress, 4 ); // approx 60 ns  

  return retVal;
}

bool v1742::setupV1742ODB(){
  char str[1024];
  HNDLE hkey;

  adcNum = (vmeBaseAdress>>16) & 0xF;

  FADC_SETTINGS_STR(fadc_settings_str);
  
  sprintf(str, "/Equipment/V1742/Module-%i-0x%X", adcNum, vmeBaseAdress);
  db_create_record(ODB, 0, str, strcomb(fadc_settings_str));
  db_find_key(ODB, 0, str, &hkey);


  if (db_open_record(ODB, hkey, &settings, sizeof(settings), MODE_READ,
		     &v1742::update, (void *)(&mess)) != DB_SUCCESS) {
    cm_msg(MERROR, "frontend_init",
	   "Cannot open Trigger Settings in ODB");
    
    return false;}

  else{
    update(ODB, hkey, (void *)(&mess));
    return true;
  }
}

bool v1742::loadCorrectionTables(){

  // load correction tables for current settings
  unsigned int frequency;

  if     (settings.sampling ==   5) frequency = 0;
  else if(settings.sampling == 2.5) frequency = 1;
  else if(settings.sampling ==   1) frequency = 2;
  else return false;

  v1742_LoadCorrectionTables(vme, vmeBaseAdress, &corrTable[0], 0, frequency);
  v1742_LoadCorrectionTables(vme, vmeBaseAdress, &corrTable[1], 1, frequency);
  v1742_LoadCorrectionTables(vme, vmeBaseAdress, &corrTable[2], 2, frequency);
  v1742_LoadCorrectionTables(vme, vmeBaseAdress, &corrTable[3], 3, frequency);

  cm_msg(MINFO, "V1742",
	 "correction tables for module at VME adress 0x%X loaded", 
	 vmeBaseAdress);

  return true;
}

bool v1742::checkModuleResponse(){
  unsigned int testval = 0x1010;
  v1742_RegisterWrite  (vme, vmeBaseAdress, V1742_SCRATCH,    testval);
  if(v1742_RegisterRead(vme, vmeBaseAdress, V1742_SCRATCH) != testval)
    return false;
  else
    return true;
}

bool v1742::read(void *pevent){
  //v1742_Status(vme,vmeBaseAdress);
  //std::cout << "EVENTSIZE in DWORDs: " <<getEventSize() << std::endl;

  // poll for 181 microseconds (module deadtime)
  struct timespec sleeptime;
  sleeptime.tv_sec = 0;
  sleeptime.tv_nsec = 1000; // unit is nano seconds
  
  for(int counter = 0; counter < 200; counter ++){
    if(checkEventStatus()) break;
    else nanosleep(&sleeptime, NULL); 
  }
  if(!checkEventStatus()){
    std::cerr << "SOMETHING STRANGE HAPPENED!!!!!" << std::endl;
    return false;

  }
  DWORD  data[1000000]; // just a very big buffer...
  char   bname[5];
  DWORD *pdata;

  sprintf(bname,"A%XDA",adcNum);
    
  // create bank for ADC data
  bk_create(pevent, bname, TID_DWORD, &pdata);
  sleeptime.tv_sec = 0;
  sleeptime.tv_nsec = 200000; // unit is nano seconds
  nanosleep(&sleeptime, NULL); 
  uint32_t entries = getEventSize();
  if(entries>0){
    v1742_DataBlockRead(vme, vmeBaseAdress, data, (uint32_t)entries);
    for(unsigned int i=0; i<entries; i++){
      //std::cout << data[i] << std::endl;
      *pdata++ = data[i];      
    }
  } else {
    return false;
  }

  /*while(checkEventStatus() && entries > 0){ 
    entries = getEventSize();
    std::cout << "entries= " <<entries<< std::endl;
    if(entries == 0) break;
    v1742_DataBlockRead(vme, vmeBaseAdress, data, (uint32_t)entries);
    for(unsigned int i=0; i<entries; i++){
      //std::cout << data[i] << std::endl;
      *pdata++ = data[i];      
    }
    }*/

  bk_close(pevent,pdata);

  // read DRS4 temperature
  sprintf(bname,"A%XTP",adcNum);
  WORD *temp = NULL;
  bk_create(pevent, bname, TID_WORD, &temp);

  for(int i=0; i<4; i++){

    int base = V1742_GROUP_TEMP +(i<<8);
    *temp++ = v1742_RegisterRead(vme, vmeBaseAdress, base) & 0xFF;
  }
  bk_close(pevent,temp);
  v1742_RegisterWrite(vme, vmeBaseAdress, V1742_SW_CLEAR, 0x1);
  return true;
}

bool v1742::storeCorrectionTables(char *pevent){
  bool retVal = true;
  if(!haveTables) retVal &= loadCorrectionTables();
  if(!retVal) return false;

  char bname[5];
  short *cell,*nsample;
  float *time;
  for(int i=0; i<4; i++){ /* groups loop begin */
    /* cell correction */
    sprintf(bname,"A%X%iC",adcNum,i);
    bk_create(pevent, bname, TID_SHORT, &cell);
    for(int j=0; j<=V1742_MAX_CHANNELS; j++){
      for(int c=0; c<1024; c++){
	*cell = (short)corrTable[i].cell[j][c]; 
	cell++; 
      }
    }
    bk_close(pevent,cell);
    
    /*NSample correction*/
    sprintf(bname,"A%X%iN",adcNum,i);
    bk_create(pevent, bname, TID_SHORT, &nsample);
    for(int j=0; j<=V1742_MAX_CHANNELS; j++){
      for(int c=0; c<1024; c++)
	*nsample++ = (short)corrTable[i].nsample[j][c];
    }
    bk_close(pevent,nsample);
    
    /*Time correction*/
    sprintf(bname,"A%X%iM",adcNum,i); // Time, do not confuse with trigger
    bk_create(pevent, bname, TID_FLOAT, &time);
    for(int c=0; c<1024; c++)
      *time++ = corrTable[i].time[c];
    bk_close(pevent,time);
  } /* groups loop end */

  return true;
}

void v1742::update(INT hDB, INT hkey, void *dump){
  if(dump != NULL){

    messenger *mess = (messenger *)dump;

    for(int i=0; i<32; i++)
      v1742_set_channel_offset(mess->vme, mess->vmeBaseAdress, i, mess->settings->choffset[i]);
    /*v1742_trigger_threshold(myvme, VADC_BASE[num],0,fadc_settings[num].tr0level);
      v1742_trigger_threshold(myvme, VADC_BASE[num],1,fadc_settings[num].tr1level);
      v1742_trigger_offset(myvme, VADC_BASE[num],0,fadc_settings[num].tr0offset);
      v1742_trigger_offset(myvme, VADC_BASE[num],1,fadc_settings[num].tr1offset);*/

    if(mess->settings->sampling == 5){
      v1742_SetSampling(mess->vme, mess->vmeBaseAdress, V1742_SAMPLING_5GS);
      *(mess->haveTables) = false;
    }
    else if (mess->settings->sampling == 2.5){
      v1742_SetSampling(mess->vme, mess->vmeBaseAdress, V1742_SAMPLING_2_5GS);
      *(mess->haveTables) = false;
    }
    else if (mess->settings->sampling == 1) {
      v1742_SetSampling(mess->vme, mess->vmeBaseAdress, V1742_SAMPLING_1GS);
      *(mess->haveTables) = false;
      }
  
      /*if(fadc_settings[num].trigger_readout != 0) 
      v1742_GroupConfig(myvme, VADC_BASE[num],V1742_TRIGGER_READOUT_ON);
      else v1742_GroupConfig(myvme, VADC_BASE[num],V1742_TRIGGER_READOUT_OFF);
  
      if(fadc_settings[num].local_trigger != 0) 
      v1742_GroupConfig(myvme, VADC_BASE[num],V1742_LOCAL_TRIGGER_ON);
      else  v1742_GroupConfig(myvme, VADC_BASE[num],V1742_LOCAL_TRIGGER_OFF);

      if (fadc_settings[num].trigger_edge != 0) 
      v1742_GroupConfig(myvme, VADC_BASE[num],V1742_TRIGGER_FALL);
      else  v1742_GroupConfig(myvme, VADC_BASE[num],V1742_TRIGGER_RISE);

      if (fadc_settings[num].buffer_mode != 0) 
      v1742_AcqCtl(myvme, VADC_BASE[num],V1742_NORMAL_MODE);
      else  v1742_AcqCtl(myvme, VADC_BASE[num],V1742_ONE_BUFFER_FREE);

      if (fadc_settings[num].count_trigger != 0) 
      v1742_AcqCtl(myvme, VADC_BASE[num],V1742_COUNT_ACCEPTED_TRIGGER);
      else v1742_AcqCtl(myvme, VADC_BASE[num],V1742_COUNT_ALL_TRIGGER);*/
  
  }
}
