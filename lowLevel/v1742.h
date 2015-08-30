/***************************************************************************/
/*                                                                         */
/*  Filename: V1742.h                                                      */
/*                                                                         */
/*  Function: headerfile for V1742                                         */
/*  Register numbers are from V1742 datasheet                              */
/* ----------------------------------------------------------------------- */
/*                                                                         */
/***************************************************************************/

#ifndef  V1742_INCLUDE_H
#define  V1742_INCLUDE_H

#define V1742_EVENT_READOUT_BUFFER          0x0000

#define V1742_GROUP_CONFIG                  0x8000      /* R/W       ; D32 */ 
#define V1742_GROUP_CFG_BIT_SET             0x8004      /* write only  D32 */ 
#define V1742_GROUP_CFG_BIT_CLR             0x8008      /* write only; D32 */ 
#define V1742_BUFFER_ORGANIZATION           0x800C      /* R/W       ; D32 */ 
#define V1742_CUSTOM_SIZE                   0x8020      /* R/W       ; D32 */
#define V1742_INITIAL_TESTWAVE              0x807C      /* R/W       ; D32 */
#define V1742_SAMPING_FREQUENCY             0x80D8      /* R/W       ; D32 */

#define V1742_ACQUISITION_CONTROL           0x8100      /* R/W       ; D32 */ 
#define V1742_ACQUISITION_STATUS            0x8104      /* read  only; D32 */ 
#define V1742_SOFTWARE_TRIGGER              0x8108      /* write only; D32 */ 
#define V1742_TRIG_SRCE_EN_MASK             0x810C      /* R/W       ; D32 */ 
#define V1742_FP_TRIGGER_OUT_EN_MASK        0x8110      /* R/W       ; D32 */ 
#define V1742_POST_TRIGGER_SETTING          0x8114      /* R/W       ; D32 */ 
#define V1742_FP_IO_DATA                    0x8118      /* R/W       ; D32 */ 
#define V1742_FP_IO_CONTROL                 0x811C      /* R/W       ; D32 */  
#define V1742_GROUP_EN_MASK                 0x8120      /* R/W       ; D32 */ 
#define V1742_ROC_FPGA_FW_REV               0x8124      /* read  only; D32 */ 
#define V1742_EVENT_STORED                  0x812C      /* read  only; D32 */ 
#define V1742_SET_MONITOR_DAC               0x8138      /* R/W       ; D32 */ 
#define V1742_BOARD_INFO                    0x8140      /* read  only; D32 */ 
#define V1742_MONITOR_MODE                  0x8144      /* R/W       ; D32 */ 
#define V1742_EVENT_SIZE                    0x814C      /* read  only; D32 */ 

#define V1742_VME_CONTROL                   0xEF00      /* R/W       ; D32 */ 
#define V1742_VME_STATUS                    0xEF04      /* read  only; D32 */ 
#define V1742_BOARD_ID                      0xEF08      /* R/W       ; D32 */ 
#define V1742_MULTICAST_BASE_ADDCTL         0xEF0C      /* R/W       ; D32 */ 
#define V1742_RELOC_ADDRESS                 0xEF10      /* R/W       ; D32 */ 
#define V1742_INTERRUPT_STATUS_ID           0xEF14      /* R/W       ; D32 */ 
#define V1742_INTERRUPT_EVT_NB              0xEF18      /* R/W       ; D32 */ 
#define V1742_BLT_EVENT_NB                  0xEF1C      /* R/W       ; D32 */ 
#define V1742_SCRATCH                       0xEF20      /* R/W       ; D32 */ 
#define V1742_SW_RESET                      0xEF24      /* write only; D32 */ 
#define V1742_SW_CLEAR                      0xEF28      /* write only; D32 */ 
#define V1742_FLASH_ENABLE                  0xEF2C      /* R/W       ; D32 */ 
#define V1742_FLASH_DATA                    0xEF30      /* R/W       ; D32 */ 
#define V1742_CONFIG_RELOAD                 0xEF34      /* write only; D32 */ 
#define V1742_CONFIG_ROM                    0xF000      /* read  only; D32 */ 

#define V1742_GROUP_THRESHOLD               0x1080      /* For group n 0x1n80 */
#define V1742_GROUP_STATUS                  0x1088      /* For group 0 */
#define V1742_GROUP_FW_REV                  0x108C      /* For group 0 */
#define V1742_GROUP_BUFFER_OCCUPANCY        0x1094      /* For group 0 */
#define V1742_GROUP_DAC                     0x1098      /* For group 0 */
#define V1742_GROUP_DAC_SEL                 0x10A4      /* For group 0 */
#define V1742_GROUP_TEMP                    0x10A0      /* For group 0 */
#define V1742_GROUP_CH_TRG_MASK             0x10A8      /* For group 0 */
#define V1742_GROUP_MEM_TAB                 0x10CC      /* For group 0 */
#define V1742_GROUP_MEM_TAB_DATA            0x10D0      /* For group 0 */
#define V1742_GROUP_TR_THRESHOLD            0x10D4      /* For group 0 */
#define V1742_GROUP_TR_DC_OFFSET            0x10DC      /* For group 0 */

#define V1742_RUN_START                           1
#define V1742_RUN_STOP                            2
#define V1742_REGISTER_RUN_MODE                   3
#define V1742_SIN_RUN_MODE                        4
#define V1742_SIN_GATE_RUN_MODE                   5
#define V1742_TRIGGER_RISE                        6
#define V1742_TRIGGER_FALL                        7
#define V1742_MULTI_BOARD_SYNC_MODE               8
#define V1742_COUNT_ACCEPTED_TRIGGER              9
#define V1742_COUNT_ALL_TRIGGER                  10
#define V1742_DOWNSAMPLE_ENABLE                  11
#define V1742_DOWNSAMPLE_DISABLE                 12
#define V1742_NO_ZERO_SUPPRESSION                13
#define V1742_ONE_BUFFER_FREE                    14
#define V1742_NORMAL_MODE                        15
#define V1742_TEST_MODE_ON                       16
#define V1742_TEST_MODE_OFF                      17
#define V1742_TRIGGER_READOUT_ON                 18
#define V1742_TRIGGER_READOUT_OFF                19
#define V1742_LOCAL_TRIGGER_ON                   20
#define V1742_LOCAL_TRIGGER_OFF                  21
#define V1742_SAMPLING_5GS                       22
#define V1742_SAMPLING_2_5GS                     23
#define V1742_SAMPLING_1GS                       24
#define V1742_OPTICAL_INTERRUPT_ON               25
#define V1742_OPTICAL_INTERRUPT_OFF              26  
   
#define V1742_EVENT_CONFIG_ALL_ADC        0x01000000
#define V1742_SOFT_TRIGGER                0x80000000
#define V1742_EXTERNAL_TRIGGER            0x40000000
#define V1742_ALIGN64                           0x20

#define V1742_DONE                          0

// Data correction -- taken from the CAEN library

#define V1742_MAX_CHANNELS		0x08
#define V1742_MAX_CHANNEL_SIZE				9
#define V1742_FLASH(n)          (0x10D0 | ( n << 8))  // base address of the flash memory (first byte)
#define V1742_SEL_FLASH(n)      (0x10CC | ( n << 8))  // flash enable/disable 
#define V1742_STATUS(n)         (0x1088 | ( n << 8))  // status register
#define V1742_MAIN_MEM_PAGE_READ          0x00D2      
#define V1742_MAIN_MEM_PAGE_PROG_TH_BUF1  0x0082   

typedef struct {
	int16_t	 	cell[V1742_MAX_CHANNELS+1][1024];
	int8_t	 	nsample[V1742_MAX_CHANNELS+1][1024];
	float		time[1024];
} V1742_DataCorrection_t;


#endif  //  V1742_INCLUDE_H


