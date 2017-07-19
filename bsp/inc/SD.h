#ifndef __SD_H__
#define __SD_H__
#include "DSP2833x_Device.h" 
#define FALSE 0
#define TRUE 1
typedef unsigned short int          uint16;
typedef unsigned char               uint8;
typedef unsigned long int           uint32;
//SD Standard Commands
#define GO_IDLE_STATE 				0x4000
#define SEND_OP_COND  				0x4100
#define SWITCH_FUNC					0x4600
#define SEND_IF_COND				0x4800
#define SEND_CSD	  				0x4900
#define SEND_CID	  				0x4A00
#define	STOP_TRANSMISSION			0x4C00
#define SEND_STATUS 				0x4D00
#define	SET_BLOCKLEN				0x5000
#define READ_SINGLE_BLOCK 			0x5100
#define READ_MULTIPLE_BLOCK			0x5200
#define WRITE_BLOCK 				0x5800
#define	WRITE_MULTIPLE_BLOCK		0x5900
#define PROGRAM_CSD					0x5B00
#define SET_WRITE_PROT				0x5C00
#define CLR_WRITE_PROT				0x5D00
#define SEND_WRITE_PROT				0x5E00
#define ERASE_WR_BLK_START_ADDR 	0x6000
#define ERASE_WR_BLK_END_ADDR 		0x6100
#define ERASE 						0x6600
#define	LOCK_UNLOCK					0x6A00
#define APP_CMD						0x7700
#define GEN_CMD						0x7800
#define READ_OCR	 				0x7A00
#define CRC_ON_OFF					0x7B00
//SD Application Specific Commands
#define	SD_STATUS					0x4D00
#define SEND_NUM_WR_BLOCKS			0x5600
#define SET_WR_BLK_ERASE_COUNT		0x5700
#define SD_SEND_OP_COND				0x6900
#define SET_CLR_CARD_DETECT			0x6A00
#define SEND_SCR					0x7300
//Data Tokens
#define INITIAL_CRC 				0x9500
#define DUMMY_CRC 					0xFF00
#define DUMMY_DATA 					0xFF00
#define START_BLOCK					0xFE00
#define MULTIPLE_START_BLOCK 		0xFC00
#define MULTIPLE_STOP_TRANSMISSION	0xFD00 
#define SECTOR_ZERO 		    	0x0000
#define STUFF_BITS 					0x0000
#define BUSY						0x0000
#define SINGLE_SECTOR				0x0001
#define MULTIPLE_WRITE				0x0001
#define CRC_ON						0x0001
#define CRC_OFF						0x0000
#define BLOCK_LENGTH				0x0200
#define CHECK_PATTERN				0xAA00
#define SUPPLY_VOLTAGE				0x0100
#define HIGH_CAPACITY				0x4000
#define SUPPORTED_VOLTAGE			0xFF00
#define INTERFACE_COND				426
#define VER2_OP_COND				1073741824
//Status and Error Codes
#define IN_IDLE_STATE 				0x0100
#define SUCCESS 					0x0000
#define DATA_ACCEPTED 				0x0500
#define CRC_ERROR 					0x0B00
#define WRITE_ERROR 				0x0D00
#define ERROR						0x0100
#define CC_ERROR					0x0200
#define CARD_ECC_FAILED				0x0400
#define OUT_OF_RANGE				0x0800  
#define ILLEGAL_COMMAND_IDLE_STATE  0x0500 
#define RESET_RESPONSE              response = 0xFF00;
#define EIGHT_CLOCK_CYCLE_DELAY     spi_xmit_byte(DUMMY_DATA);
//-------------------------------------------------------------
typedef struct
{
  volatile uint8  CSDStruct;            /*!< CSD structure */
  volatile uint8  SysSpecVersion;       /*!< System specification version */
  volatile uint8  Reserved1;            /*!< Reserved */
  volatile uint8  TAAC;                 /*!< Data read access-time 1 */
  volatile uint8  NSAC;                 /*!< Data read access-time 2 in CLK cycles */
  volatile uint8  MaxBusClkFrec;        /*!< Max. bus clock frequency */
  volatile uint16 CardComdClasses;      /*!< Card command classes */
  volatile uint8  RdBlockLen;           /*!< Max. read data block length */
  volatile uint8  PartBlockRead;        /*!< Partial blocks for read allowed */
  volatile uint8  WrBlockMisalign;      /*!< Write block misalignment */
  volatile uint8  RdBlockMisalign;      /*!< Read block misalignment */
  volatile uint8  DSRImpl;              /*!< DSR implemented */
  volatile uint8  Reserved2;            /*!< Reserved */
  volatile uint32 DeviceSize;           /*!< Device Size */
  volatile uint8  MaxRdCurrentVDDMin;   /*!< Max. read current @ VDD min */
  volatile uint8  MaxRdCurrentVDDMax;   /*!< Max. read current @ VDD max */
  volatile uint8  MaxWrCurrentVDDMin;   /*!< Max. write current @ VDD min */
  volatile uint8  MaxWrCurrentVDDMax;   /*!< Max. write current @ VDD max */
  volatile uint8  DeviceSizeMul;        /*!< Device size multiplier */
  volatile uint8  EraseGrSize;          /*!< Erase group size */
  volatile uint8  EraseGrMul;           /*!< Erase group size multiplier */
  volatile uint8  WrProtectGrSize;      /*!< Write protect group size */
  volatile uint8  WrProtectGrEnable;    /*!< Write protect group enable */
  volatile uint8  ManDeflECC;           /*!< Manufacturer default ECC */
  volatile uint8  WrSpeedFact;          /*!< Write speed factor */
  volatile uint8  MaxWrBlockLen;        /*!< Max. write data block length */
  volatile uint8  WriteBlockPaPartial;  /*!< Partial blocks for write allowed */
  volatile uint8  Reserved3;            /*!< Reserded */
  volatile uint8  ContentProtectAppli;  /*!< Content protection application */
  volatile uint8  FileFormatGrouop;     /*!< File format group */
  volatile uint8  CopyFlag;             /*!< Copy flag (OTP) */
  volatile uint8  PermWrProtect;        /*!< Permanent write protection */
  volatile uint8  TempWrProtect;        /*!< Temporary write protection */
  volatile uint8  FileFormat;           /*!< File Format */
  volatile uint8  ECC;                  /*!< ECC code */
  volatile uint8  CSD_CRC;              /*!< CSD CRC */
  volatile uint8  Reserved4;            /*!< always 1*/
} SD_CSD;

/**
  * @brief  Card Identification Data: CID Register
  */
typedef struct
{
  volatile uint8  ManufacturerID;       /*!< ManufacturerID */
  volatile uint16 OEM_AppliID;          /*!< OEM/Application ID */
  volatile uint32 ProdName1;            /*!< Product Name part1 */
  volatile uint8  ProdName2;            /*!< Product Name part2*/
  volatile uint8  ProdRev;              /*!< Product Revision */
  volatile uint32 ProdSN;               /*!< Product Serial Number */
  volatile uint8  Reserved1;            /*!< Reserved1 */
  volatile uint16 ManufactDate;         /*!< Manufacturing Date */
  volatile uint8  CID_CRC;              /*!< CID CRC */
  volatile uint8  Reserved2;            /*!< always 1 */
} SD_CID;

/**
  * @brief SD Card information
  */
typedef struct
{
  SD_CSD SD_csd;
  SD_CID SD_cid;
  uint32 CardCapacity;  /*!< Card Capacity */
  uint32 CardBlockSize; /*!< Card Block Size */
} SD_CardInfo;
extern void sd_erase_block(uint16 starting_sector, uint16 total_sectors);
extern void spi_initialization(void);
extern void sd_card_insertion(void);
extern void sd_initialization(void);
extern void sd_version1_initialization(void);
extern void sd_version2_initialization(void);
extern void sd_read_block(uint16 sector, uint16 *pBuffer);
extern void sd_read_multiple_block(uint16 sector, uint16 *pBuffer, uint16 total_sectors);
extern void sd_data_response(uint16 *pBuffer, uint16 total_sectors);
extern void sd_read_register(uint16 command);
extern void sd_ocr_response(void);
extern void sd_cid_csd_response(uint16 command);
extern void sd_send_status(void);
extern void sd_switch_function(uint16 *pBuffer);
extern uint16 spi_xmit_byte(uint16 byte);
extern void spi_xmit_command(uint16 command, Uint32 data, uint16 crc);
extern uint16 sd_crc7(uint16 full_command[6]);
extern void sd_command_response(void);
extern void sd_error(void);
extern void sd_write_block(uint16 sector, uint16 *pBuffer);
extern void sd_write_multiple_block(uint16 sector, uint16 *pBuffer, uint16 total_sectors);
extern void sd_write_data(uint16 *pBuffer, uint16 total_sectors, uint16 multiple_write);
extern void SPI_ENABLE(void);
extern void SPI_DISABLE(void);
extern void SD_Init(void);
//Global Variables
extern uint16 response, ocr_contents[5], csd_contents[16], cid_contents[16];
extern uint16 card_status[2], data_manipulation, high_capacity, crc_enabled;
extern SD_CardInfo  cardinfo;
#endif
