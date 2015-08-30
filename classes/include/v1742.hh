/*! 
  \file v1742.hh
  \brief interface class for easy readout and control of CAEN V1742 waveform digitisers
  \author Clemens Sauerzopf <clemens.sauerzopf@oeaw.ac.at>  
  \date 06.08.2014
  \version 1.0
*/ 

#ifndef __V1742_CC__
#define __V1742_CC__

#include "midas.h"

#include "mvmestd.h"
extern "C" {
#include "v1742drv.h"
}

class v1742 {
public:
  v1742(unsigned int vmeBaseAdr,MVME_INTERFACE *vmeInt, HNDLE hODB) 
    : vmeBaseAdress(vmeBaseAdr), vme(vmeInt), ODB(hODB), haveTables(false),
      mess(vme,&settings,vmeBaseAdr, &haveTables) {}

  // initialise everything
  bool init();
  
  // setup device
  bool setupV1742();
  
  // setup midas ODB
  bool setupV1742ODB();
  
  // load correction tables from module
  bool loadCorrectionTables();
  inline bool haveCorrectionTables() const {return haveTables;};

  //! check if the module is still responding
  bool checkModuleResponse();

  //! read data into midas banks
  bool read(void *pevent);
  
  //! store correction tables in modas banls
  bool storeCorrectionTables(char *pevent);

  //! start acquistion
  inline void start()  const { v1742_AcqCtl(vme, vmeBaseAdress, V1742_RUN_START);              }
  //! stop acquistion
  inline void stop()   const { v1742_AcqCtl(vme, vmeBaseAdress, V1742_RUN_STOP );              }
  //! pause acquistion
  inline void pause()  const { v1742_TrgCtl(vme, vmeBaseAdress, V1742_TRIG_SRCE_EN_MASK, 0);   }
  //! resume acquistion
  inline void resume() const { v1742_TrgCtl(vme, vmeBaseAdress, V1742_TRIG_SRCE_EN_MASK,
					    V1742_SOFT_TRIGGER | V1742_EXTERNAL_TRIGGER   );   }
private:
  v1742(){}

  inline int getEventSize() const {return v1742_RegisterRead(vme, vmeBaseAdress, V1742_EVENT_SIZE);}
  inline bool checkEventStatus() const {return (v1742_RegisterRead(vme, vmeBaseAdress, V1742_ACQUISITION_STATUS)&(0x4))>>2;}

  static void update(INT hDB, INT hkey, void *dump);
    
  // VME base adress A32
  unsigned int vmeBaseAdress;
  
  // VME handle
  MVME_INTERFACE *vme;
  
  // MIDAS ODB handle
  HNDLE ODB;

  // DRS4 correction table
  V1742_DataCorrection_t corrTable[4];
  bool haveTables;

  // number of the ADC (range 0-15)
  int adcNum;


  // MIDAS ODB settings
  typedef struct {
    INT       tr0offset;
    INT       tr1offset;
    INT       tr0level;
    INT       tr1level;
    double    sampling;
    INT       trigger_readout;
    INT       local_trigger;
    INT       trigger_edge;
    INT       buffer_mode;
    INT       count_trigger;
    INT       temperatur;
    INT       choffset[32];
  } FADC_SETTINGS;
  FADC_SETTINGS settings;
  class messenger {
  public:
    messenger(MVME_INTERFACE *mvme, FADC_SETTINGS *set, unsigned int vmeBaseAdr, bool *tables) 
      : vme(mvme), settings(set), vmeBaseAdress(vmeBaseAdr), haveTables(tables) {}
    ~messenger(){vme = NULL; settings = NULL;}
    MVME_INTERFACE *vme;
    FADC_SETTINGS *settings;
    unsigned int vmeBaseAdress;
    bool *haveTables;
    messenger(){}    
  } mess;
  
};

#define FADC_SETTINGS_STR(_name) const char *_name[] = {	\
    "[.]",							\
    "tr0offset = INT : 32768",					\
    "tr1offset = INT : 32768",					\
    "tr0level = INT : 24894",					\
    "tr1level = INT : 24894",					\
    "sampling = DOUBLE : 5",					\
    "trigger_readout = INT : 1",				\
    "local_trigger = INT : 1",					\
    "trigger_edge = INT : 1",					\
    "buffer_mode = INT : 0",					\
    "count_trigger = INT : 0",					\
    "temperatur = INT : 0",					\
    "choffset = INT[32] :",					\
    "[0] 49000",						\
    "[1] 49000",						\
    "[2] 49000",						\
    "[3] 49000",						\
    "[4] 49000",						\
    "[5] 49000",						\
    "[6] 49000",						\
    "[7] 49000",						\
    "[8] 49000",						\
    "[9] 49000",						\
    "[10] 49000",						\
    "[11] 49000",						\
    "[12] 49000",						\
    "[13] 49000",						\
    "[14] 49000",						\
    "[15] 49000",						\
    "[16] 49000",						\
    "[17] 49000",						\
    "[18] 49000",						\
    "[19] 49000",						\
    "[20] 49000",						\
    "[21] 49000",						\
    "[22] 49000",						\
    "[23] 49000",						\
    "[24] 49000",						\
    "[25] 49000",						\
    "[26] 49000",						\
    "[27] 49000",						\
    "[28] 49000",						\
    "[29] 49000",						\
    "[30] 49000",						\
    "[31] 49000",						\
    "",								\
    NULL }

#endif
