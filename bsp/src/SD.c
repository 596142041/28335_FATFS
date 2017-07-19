#include "SD.h"
uint16 response, ocr_contents[5], csd_contents[16], cid_contents[16];
uint16 data_manipulation = TRUE, high_capacity = FALSE, crc_enabled = FALSE;
SD_CardInfo  cardinfo;
void SD_Init(void)
{
	sd_card_insertion();
	sd_initialization();
	sd_read_register(SEND_CSD);		//Read CSD register
	sd_read_register(READ_OCR);		//Read OCR register
	sd_read_register(SEND_CID);		//Read CID register
}
void SPI_ENABLE()
{ 
	GpioDataRegs.GPBDAT.bit.GPIO57 = 0;
} 
void SPI_DISABLE()
{ 
	GpioDataRegs.GPBDAT.bit.GPIO57 = 1;
} 
// SPI_INITIALIZATION 
void spi_initialization()
{    
	EALLOW;
	GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 0;
	GpioCtrlRegs.GPBPUD.bit.GPIO57  = 0;
	GpioCtrlRegs.GPBDIR.bit.GPIO57 = 1;

	GpioCtrlRegs.GPBPUD.bit.GPIO54 = 0;   // Enable pull-up on GPIO54 (SPISIMOA)
	GpioCtrlRegs.GPBPUD.bit.GPIO55 = 0;   // Enable pull-up on GPIO55 (SPISOMIA)
	GpioCtrlRegs.GPBPUD.bit.GPIO56 = 0;   // Enable pull-up on GPIO56 (SPICLKA)

	GpioCtrlRegs.GPBQSEL2.bit.GPIO54 = 3; // Asynch input GPIO16 (SPISIMOA)
	GpioCtrlRegs.GPBQSEL2.bit.GPIO55 = 3; // Asynch input GPIO17 (SPISOMIA)
	GpioCtrlRegs.GPBQSEL2.bit.GPIO56 = 3; // Asynch input GPIO18 (SPICLKA)

	GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 1; // Configure GPIO54 as SPISIMOA
	GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 1; // Configure GPIO55 as SPISOMIA
	GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 1; // Configure GPIO56 as SPICLKA

	EDIS;
	GpioDataRegs.GPBDAT.bit.GPIO57 = 1;
	SpiaRegs.SPICCR.all =0x0007;	   
	SpiaRegs.SPICTL.all =0x000E;         
	SpiaRegs.SPIBRR =0x0063;									
    SpiaRegs.SPICCR.all =0x0087;		
    SpiaRegs.SPIPRI.bit.FREE = 1; 		
}
void sd_card_insertion()
{
	uint16 i;
   	SPI_DISABLE();							
	for(i=0;i<10;i++)					//Transmit 0xFF for 80 clock cycles
   		spi_xmit_byte(DUMMY_DATA); 
}
// SD_INITIALIZATION 
void sd_initialization()
{
	 sd_card_insertion();
	SPI_ENABLE();						 
	data_manipulation = FALSE; 
   	spi_xmit_command(GO_IDLE_STATE, STUFF_BITS, INITIAL_CRC);	
   	
   	RESET_RESPONSE;							 
	while(response != IN_IDLE_STATE)		 
		sd_command_response();  
	RESET_RESPONSE;							 
	EIGHT_CLOCK_CYCLE_DELAY;
	crc_enabled = TRUE;						 
	spi_xmit_command(SEND_IF_COND, INTERFACE_COND, DUMMY_CRC);	 
	crc_enabled = FALSE;					 
	while((response != IN_IDLE_STATE) && (response != ILLEGAL_COMMAND_IDLE_STATE))	
		response = spi_xmit_byte(DUMMY_DATA); 
	if(response == ILLEGAL_COMMAND_IDLE_STATE)
		sd_version1_initialization();	
	else if(response == IN_IDLE_STATE)
		sd_version2_initialization();
	SPI_DISABLE();								 
	EIGHT_CLOCK_CYCLE_DELAY;
	SpiaRegs.SPIBRR = 0x0004;				 
}
// SD_VERSION1_INITIALIZATION 
void sd_version1_initialization()
{

	RESET_RESPONSE;						 
	EIGHT_CLOCK_CYCLE_DELAY; 
	spi_xmit_command(READ_OCR, STUFF_BITS, DUMMY_CRC);	 
	while(response != IN_IDLE_STATE)		
		sd_command_response(); 
	sd_ocr_response(); 
	if(ocr_contents[1] != SUPPORTED_VOLTAGE)		
		sd_error(); 
	RESET_RESPONSE; 
	EIGHT_CLOCK_CYCLE_DELAY; 
	while(response != SUCCESS)
	{
		RESET_RESPONSE;				 
		EIGHT_CLOCK_CYCLE_DELAY; 
		spi_xmit_command(APP_CMD, STUFF_BITS, DUMMY_CRC); 
		while(response != IN_IDLE_STATE)	 
			sd_command_response(); 
		RESET_RESPONSE;					 
		EIGHT_CLOCK_CYCLE_DELAY;	   								
 		spi_xmit_command(SD_SEND_OP_COND, STUFF_BITS, DUMMY_CRC);				  
		sd_command_response();			 
	}
	
	RESET_RESPONSE;				 
	EIGHT_CLOCK_CYCLE_DELAY;			
}
// SD_VERSION2_INITIALIZATION 
void sd_version2_initialization()
{
	uint16 i, send_if_cond_response[4], ccs_check;
	
	 
	for(i=0;i<3;i++)
		send_if_cond_response[i] = spi_xmit_byte(DUMMY_DATA); 
	if((send_if_cond_response[2] != SUPPLY_VOLTAGE) || (send_if_cond_response[3] != CHECK_PATTERN))
		sd_error();

	RESET_RESPONSE;						 
	EIGHT_CLOCK_CYCLE_DELAY; 
	while(response != SUCCESS)
	{
		RESET_RESPONSE;	 
		EIGHT_CLOCK_CYCLE_DELAY; 
		spi_xmit_command(APP_CMD, STUFF_BITS, DUMMY_CRC);
		while(response != IN_IDLE_STATE) 
			sd_command_response(); 
		RESET_RESPONSE;	 
		EIGHT_CLOCK_CYCLE_DELAY;  								
 		spi_xmit_command(SD_SEND_OP_COND, VER2_OP_COND, DUMMY_CRC);				  
		sd_command_response();		 
	} 
	RESET_RESPONSE;						 
	EIGHT_CLOCK_CYCLE_DELAY; 
	spi_xmit_command(READ_OCR, STUFF_BITS, DUMMY_CRC);	 
	while(response != SUCCESS)		
		sd_command_response(); 
	sd_ocr_response();					 
	ccs_check = ocr_contents[0] & HIGH_CAPACITY;  
	if(ccs_check == HIGH_CAPACITY)		 
		high_capacity = TRUE;				  
	RESET_RESPONSE;	 
	EIGHT_CLOCK_CYCLE_DELAY;			
}
// SD_DATA_RESPONSE 
void sd_data_response(uint16 *pBuffer, uint16 total_sectors)
{
	uint16 i, j;
	data_manipulation = TRUE; 
	for(i=0;i<total_sectors;i++)	
	{
		RESET_RESPONSE; 
		while(response != START_BLOCK)
		{
			response = spi_xmit_byte(DUMMY_DATA);
				
			if((response == ERROR) || (response == CC_ERROR) || \
			(response == CARD_ECC_FAILED) || (response == OUT_OF_RANGE))
				sd_error();
		}
			
		for(j=0;j<512;j++) 
		{
			*pBuffer++ = (spi_xmit_byte(DUMMY_DATA)>>8);
		}
                
		spi_xmit_byte(DUMMY_DATA);
	} 
	EIGHT_CLOCK_CYCLE_DELAY;			
}
// SD_READ_REGISTER 
void sd_read_register(uint16 command)
{
	SPI_ENABLE();							 
	data_manipulation = FALSE;		 
	spi_xmit_command(command, STUFF_BITS, DUMMY_CRC);				
	
	RESET_RESPONSE;					 
	while(response != SUCCESS)		 
		sd_command_response(); 
	if(command == READ_OCR)			 					
		sd_ocr_response();			 
	else 
		sd_cid_csd_response(command);
	SPI_DISABLE();

	EIGHT_CLOCK_CYCLE_DELAY;
} 
// SD_OCR_RESPONSE 
void sd_ocr_response()
{
	uint16 i; 
	for(i=0;i<4;i++)						 
		ocr_contents[i] = spi_xmit_byte(DUMMY_DATA); 
	EIGHT_CLOCK_CYCLE_DELAY;
} 
// SD_CID_CSD_RESPONSE 
void sd_cid_csd_response(uint16 command)
{
	uint16 i;
	uint16 temp[16];
	RESET_RESPONSE;					 
	while(response != START_BLOCK)
	{
		response = spi_xmit_byte(DUMMY_DATA);
	}
	for(i=0;i<16;i++)
	{
		if(command == SEND_CSD)
			temp[i] = csd_contents[i] = spi_xmit_byte(DUMMY_DATA);
		else
			temp[i] = cid_contents[i] =  spi_xmit_byte(DUMMY_DATA);
	} 
	spi_xmit_byte(DUMMY_DATA);					
	spi_xmit_byte(DUMMY_DATA); 
	switch (command)
	{
		case SEND_CSD:
			  /*!< Byte 0 */
			  cardinfo.SD_csd.CSDStruct = ((temp[0]>>8) & 0xC0) >> 6;
			  cardinfo.SD_csd.SysSpecVersion = ((temp[0]>>8) & 0x3C) >> 2;
			  cardinfo.SD_csd.Reserved1 = (temp[0]>>8) & 0x03;

			  /*!< Byte 1 */
			  cardinfo.SD_csd.TAAC = (temp[1]>>8);

			  /*!< Byte 2 */
			  cardinfo.SD_csd.NSAC = (temp[2]>>8);

			  /*!< Byte 3 */
			  cardinfo.SD_csd.MaxBusClkFrec = (temp[3]>>8);

			  /*!< Byte 4 */
			  cardinfo.SD_csd.CardComdClasses = (temp[4]>>8) << 4;

			  /*!< Byte 5 */
			  cardinfo.SD_csd.CardComdClasses |= ((temp[5]>>8) & 0xF0) >> 4;
			  cardinfo.SD_csd.RdBlockLen = (temp[5]>>8) & 0x0F;

			  /*!< Byte 6 */
			  cardinfo.SD_csd.PartBlockRead = ((temp[6]>>8) & 0x80) >> 7;
			  cardinfo.SD_csd.WrBlockMisalign = ((temp[6]>>8) & 0x40) >> 6;
			  cardinfo.SD_csd.RdBlockMisalign = ((temp[6]>>8) & 0x20) >> 5;
			  cardinfo.SD_csd.DSRImpl = ((temp[6]>>8) & 0x10) >> 4;
			  cardinfo.SD_csd.Reserved2 = 0; /*!< Reserved */

			  cardinfo.SD_csd.DeviceSize = ((temp[6]>>8) & 0x03) << 10;

			  /*!< Byte 7 */
			  cardinfo.SD_csd.DeviceSize |= ((temp[7]>>8)) << 2;

			  /*!< Byte 8 */
			  cardinfo.SD_csd.DeviceSize |= ((temp[8]>>8) & 0xC0) >> 6;

			  cardinfo.SD_csd.MaxRdCurrentVDDMin = ((temp[8]>>8) & 0x38) >> 3;
			  cardinfo.SD_csd.MaxRdCurrentVDDMax = ((temp[8]>>8) & 0x07);

			  /*!< Byte 9 */
			  cardinfo.SD_csd.MaxWrCurrentVDDMin = ((temp[9]>>8) & 0xE0) >> 5;
			  cardinfo.SD_csd.MaxWrCurrentVDDMax = ((temp[9]>>8) & 0x1C) >> 2;
			  cardinfo.SD_csd.DeviceSizeMul = ((temp[9]>>8) & 0x03) << 1;
			  /*!< Byte 10 */
			  cardinfo.SD_csd.DeviceSizeMul |= ((temp[10]>>8) & 0x80) >> 7;

			  cardinfo.SD_csd.EraseGrSize = ((temp[10]>>8) & 0x40) >> 6;
			  cardinfo.SD_csd.EraseGrMul = ((temp[10]>>8) & 0x3F) << 1;

			  /*!< Byte 11 */
			  cardinfo.SD_csd.EraseGrMul |= ((temp[11]>>8) & 0x80) >> 7;
			  cardinfo.SD_csd.WrProtectGrSize = ((temp[11]>>8) & 0x7F);

			  /*!< Byte 12 */
			  cardinfo.SD_csd.WrProtectGrEnable = ((temp[12]>>8) & 0x80) >> 7;
			  cardinfo.SD_csd.ManDeflECC = ((temp[12]>>8) & 0x60) >> 5;
			  cardinfo.SD_csd.WrSpeedFact = ((temp[12]>>8) & 0x1C) >> 2;
			  cardinfo.SD_csd.MaxWrBlockLen = ((temp[12]>>8) & 0x03) << 2;

			  /*!< Byte 13 */
			  cardinfo.SD_csd.MaxWrBlockLen |= ((temp[13]>>8) & 0xC0) >> 6;
			  cardinfo.SD_csd.WriteBlockPaPartial = ((temp[13]>>8) & 0x20) >> 5;
			  cardinfo.SD_csd.Reserved3 = 0;
			  cardinfo.SD_csd.ContentProtectAppli = ((temp[13]>>8) & 0x01);

			  /*!< Byte 14 */
			  cardinfo.SD_csd.FileFormatGrouop = ((temp[14]>>8) & 0x80) >> 7;
			  cardinfo.SD_csd.CopyFlag = ((temp[14]>>8) & 0x40) >> 6;
			  cardinfo.SD_csd.PermWrProtect = ((temp[14]>>8) & 0x20) >> 5;
			  cardinfo.SD_csd.TempWrProtect = ((temp[14]>>8) & 0x10) >> 4;
			  cardinfo.SD_csd.FileFormat = ((temp[14]>>8) & 0x0C) >> 2;
			  cardinfo.SD_csd.ECC = ((temp[14]>>8) & 0x03);

			  /*!< Byte 15 */
			  cardinfo.SD_csd.CSD_CRC = ((temp[15]>>8) & 0xFE) >> 1;
			  cardinfo.SD_csd.Reserved4 = 1;
			break;
		case SEND_CID:
			  /*!< Byte 0 */
			  cardinfo.SD_cid.ManufacturerID = temp[0]>>8;

			  /*!< Byte 1 */
			  cardinfo.SD_cid.OEM_AppliID = temp[1];

			  /*!< Byte 2 */
			  cardinfo.SD_cid.OEM_AppliID |= temp[2]>>8;

			  /*!< Byte 3 */
			  cardinfo.SD_cid.ProdName1 = temp[3] << 8;

			  /*!< Byte 4 */
			  cardinfo.SD_cid.ProdName1 |= temp[4] << 8;

			  /*!< Byte 5 */
			  cardinfo.SD_cid.ProdName1 |= temp[5];

			  /*!< Byte 6 */
			  cardinfo.SD_cid.ProdName1 |= temp[6]>>8;

			  /*!< Byte 7 */
			  cardinfo.SD_cid.ProdName2 = temp[7]>>8;

			  /*!< Byte 8 */
			  cardinfo.SD_cid.ProdRev = temp[8]>>8;

			  /*!< Byte 9 */
			  cardinfo.SD_cid.ProdSN = temp[9]<<15;//(temp[9]>>8) << 24;

			  /*!< Byte 10 */
			  cardinfo.SD_cid.ProdSN |= temp[10] << 8;

			  /*!< Byte 11 */
			  cardinfo.SD_cid.ProdSN |= temp[11];

			  /*!< Byte 12 */
			  cardinfo.SD_cid.ProdSN |= temp[12]>>8;

			  /*!< Byte 13 */
			  cardinfo.SD_cid.Reserved1 |= ((temp[13]>>8) & 0xF0) >> 4;
			  cardinfo.SD_cid.ManufactDate = ((temp[13]>>8) & 0x0F) << 8;

			  /*!< Byte 14 */
			  cardinfo.SD_cid.ManufactDate |= temp[14]>>8;

			  /*!< Byte 15 */
			  cardinfo.SD_cid.CID_CRC = ((temp[15]>>8) & 0xFE) >> 1;
			  cardinfo.SD_cid.Reserved2 = 1;
			break;
		default:
			break;
	}
	cardinfo.CardCapacity = (cardinfo.SD_csd.DeviceSize + 1) ;
	cardinfo.CardCapacity *= (1 << (cardinfo.SD_csd.DeviceSizeMul + 2));
	cardinfo.CardBlockSize = 1 << (cardinfo.SD_csd.RdBlockLen);
	cardinfo.CardCapacity *= cardinfo.CardBlockSize;
	cardinfo.CardCapacity = cardinfo.CardCapacity>>20;
	EIGHT_CLOCK_CYCLE_DELAY;				
} 
// SD_SEND_STATUS 
void sd_send_status()
{
	data_manipulation = FALSE;	 
	spi_xmit_command(SEND_STATUS, STUFF_BITS, DUMMY_CRC); 
	RESET_RESPONSE;					 
	while(response == DUMMY_DATA)	 
		response = spi_xmit_byte(DUMMY_DATA);
	/*
	card_status[0] = response;					 
	card_status[1] = spi_xmit_byte(DUMMY_DATA); 
	*/
	spi_xmit_byte(DUMMY_DATA);
		sd_error();								 
	EIGHT_CLOCK_CYCLE_DELAY;
}
// SD_SEND_STATUS 
uint16 spi_xmit_byte(uint16 byte)
{
  	SpiaRegs.SPITXBUF = byte;         			 
	while(SpiaRegs.SPISTS.bit.INT_FLAG != 1); 	 
  	return (SpiaRegs.SPIRXBUF << 8);			 
} 
// SPI_XMIT_COMMAND 
void spi_xmit_command(uint16 command, Uint32 data, uint16 crc)
{
	uint16 i, full_command[6];			 
	if((data_manipulation == TRUE) && (high_capacity == FALSE))
		data *= BLOCK_LENGTH;					  
	full_command[0] = command;	 
	for(i=4;i>0;i--)
	{
		full_command[i] = ((uint16)(Uint32)(data & 0x000000FF) << 8);
		data = data >> 8;
	} 
	if(crc_enabled == TRUE)
    	full_command[5] = sd_crc7(full_command);
	else
		full_command[5] = crc; 
    for(i=0;i<6;i++)		
	{
		spi_xmit_byte(full_command[i]);	
	}		
    			
}  
// SD_CRC7 
uint16 sd_crc7(uint16 full_command[6])
{
	uint16 i,j,command,crc = 0x0000;
   	                                           
    for (i=0;i<5;i++)
    {
		command = full_command[i];		
       	for (j=0;j<8;j++)
      	{
         	crc = crc << 1;					 			
         	if ((command ^ crc) & 0x8000)	 
         		crc = crc ^ 0x0900;  		           
         	command = command << 1;			 
      	}

       	crc = crc & 0x7F00;					 
   	}
   
   	crc = crc << 1;							 
   	crc |= 0x0100;							 
   
  	return(crc);							 
} 
// SD_COMMAND_RESPONSE 
void sd_command_response()
{
	uint16 i; 
	RESET_RESPONSE;  
	for(i=0;i<8;i++)
	{
		response = spi_xmit_byte(DUMMY_DATA); 		
		if ((response == IN_IDLE_STATE) || (response == SUCCESS))
			break; 
		else if (response != DUMMY_DATA)
			sd_error();
	}
}
// SD_ERROR 
void sd_error()
{	 
	SPI_DISABLE();								 
	EIGHT_CLOCK_CYCLE_DELAY; 
	asm(" ESTOP0");							 
   	for(;;){};								 
} 
// SD_READ_BLOCK 
void sd_read_block(uint16 sector, uint16 *pBuffer)
{
	SPI_ENABLE();							 
	data_manipulation = TRUE;		 
	spi_xmit_command(READ_SINGLE_BLOCK, sector, DUMMY_CRC);	
	
	RESET_RESPONSE;					 
	while(response != SUCCESS)		 
		sd_command_response(); 
	EIGHT_CLOCK_CYCLE_DELAY; 
	sd_data_response(pBuffer, SINGLE_SECTOR);	

	SPI_DISABLE();					 
	EIGHT_CLOCK_CYCLE_DELAY;
} 
// SD_READ_MULTIPLE_BLOCK 
void sd_read_multiple_block(uint16 sector, uint16 *pBuffer, uint16 total_sectors)
{
	uint16 i;

	SPI_ENABLE();							 
	data_manipulation = TRUE;		 
	spi_xmit_command(READ_MULTIPLE_BLOCK, sector, DUMMY_CRC);	
	
	RESET_RESPONSE;					 
	while(response != SUCCESS)		 
		sd_command_response(); 
	EIGHT_CLOCK_CYCLE_DELAY; 
	sd_data_response(pBuffer, total_sectors); 		
	spi_xmit_command(STOP_TRANSMISSION, STUFF_BITS, DUMMY_CRC);
	
	RESET_RESPONSE;	 
	for(i=0;i<8;i++)
	{
		response = spi_xmit_byte(DUMMY_DATA);
		if (response == SUCCESS)
		 	break;
	}
	SPI_DISABLE();					 
	EIGHT_CLOCK_CYCLE_DELAY;
} 

// SD_WRITE_BLOCK 
void sd_write_block(uint16 sector, uint16 *pBuffer)
{
	SPI_ENABLE();							
    data_manipulation = TRUE;		 
	spi_xmit_command(WRITE_BLOCK, sector, DUMMY_CRC);
	RESET_RESPONSE;					 
	while(response != SUCCESS)		
	{
		sd_command_response(); 
	} 
	EIGHT_CLOCK_CYCLE_DELAY; 
	sd_write_data(pBuffer, SINGLE_SECTOR, 0);	 
	EIGHT_CLOCK_CYCLE_DELAY; 
	sd_send_status();				

	SPI_DISABLE();						 
	EIGHT_CLOCK_CYCLE_DELAY;
}
// SD_WRITE_MULTIPLE_BLOCK 
void sd_write_multiple_block(uint16 sector, uint16 *pBuffer, uint16 total_sectors)
{
	SPI_ENABLE();							 
	data_manipulation = TRUE; 
	spi_xmit_command(WRITE_MULTIPLE_BLOCK, sector, DUMMY_CRC);

	RESET_RESPONSE;
	while(response != SUCCESS)	
	{
		sd_command_response();
	} 
	EIGHT_CLOCK_CYCLE_DELAY; 
	sd_write_data(pBuffer, total_sectors, MULTIPLE_WRITE); 
	response = BUSY; 
	while(response != DUMMY_DATA)
	{
		response = spi_xmit_byte(DUMMY_DATA);
	}		 
	EIGHT_CLOCK_CYCLE_DELAY; 
	sd_send_status();

	SPI_DISABLE(); 
	EIGHT_CLOCK_CYCLE_DELAY;
} 
// SD_WRITE_DATA 
void sd_write_data(uint16 *pBuffer, uint16 total_sectors, uint16 multiple_write)
{
	uint16 i, j; 
	RESET_RESPONSE; 
	for(i=0;i<total_sectors;i++)
	{ 
		if(multiple_write == MULTIPLE_WRITE)
			spi_xmit_byte(MULTIPLE_START_BLOCK); 
		else
			spi_xmit_byte(START_BLOCK);	 
		for(j=0;j<512;j++) 
	        spi_xmit_byte((*pBuffer++)<<8);  
		spi_xmit_byte(DUMMY_DATA);					
		spi_xmit_byte(DUMMY_DATA); 
		while(response != DATA_ACCEPTED)
		{
			response = spi_xmit_byte(DUMMY_DATA);	
			response &= 0x0F00;		
			if((response == CRC_ERROR) || (response == WRITE_ERROR))
				sd_error();	 
		}
		while(response != DUMMY_DATA)			 
			response = spi_xmit_byte(DUMMY_DATA);
	}
	if(multiple_write == MULTIPLE_WRITE)
		spi_xmit_byte(MULTIPLE_STOP_TRANSMISSION);
}
