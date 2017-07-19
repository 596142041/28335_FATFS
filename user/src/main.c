/*
 * main.c
 *
 *  Created on: 2016年9月11日
 *      Author: lpr
 */
#include "main.h"
u8 res = FR_OK;
int main(void)
{
	int  i;
	InitSysCtrl();
	CAN_GPIO_Config();
	CANB_GPIO_Config();
	DINT;
	DRTM;
	InitPieCtrl();
		IER = 0x00;
	IFR = 0x00;
	InitPieVectTable();
#if DEBUG_FLASH
	MemCopy(&RamfuncsLoadStart,&RamfuncsLoadEnd,&RamfuncsRunStart);
	InitFlash();
#endif
	EALLOW;
		PieVectTable.ECAN1INTA = &Ecana_isr1;
	EDIS;
	spi_initialization();
	CAN_Config();
	CANB_Config();
	CANB_Rx_Config();
	CAN_Rx_Config();
	CAN_Rx_IT_Concig();
	SD_Init();
	PieCtrlRegs.PIEIER9.bit.INTx6 = 1;//CANA的中断1
	IER |= M_INT9;
	EINT;
	ERTM;
	for(i = 0; i  <8;i++)
	{
		can_tx_msg.CAN_Tx_msg_data.msg_byte.data[i] = 0;
		can_rx_msg.CAN_Rx_msg_data.msg_byte.data[i] = 0;
		canb_tx_msg.CAN_Tx_msg_data.msg_byte.data[i] = 0;
		canb_rx_msg.CAN_Rx_msg_data.msg_byte.data[i] = 0;
	}
	can_tx_msg.DLC = 1;
	can_tx_msg.ExtId.bit.ExtId = 0x00;
	can_tx_msg.IDE = CAN_ID_EXT;
	can_tx_msg.MBox_num = 12;
	can_tx_msg.SAE_J1939_Flag = 0;
	can_tx_msg.Tx_timeout_cnt = 0;
	can_tx_msg.StdId.bit.StdId = 0x00;
	//----------------------------
	canb_tx_msg.DLC = 8;
	canb_tx_msg.ExtId.all = 0x00;
	canb_tx_msg.IDE = CAN_ID_STD;
	canb_tx_msg.MBox_num = 12;
	canb_tx_msg.SAE_J1939_Flag = 0;
	canb_tx_msg.StdId.bit.resved = 0;
	canb_tx_msg.StdId.bit.StdId  = isp_cmd_list.Erase;
	canb_tx_msg.Tx_timeout_cnt = 0;
	//res = CAN_ISP();
	res = 0x00;
	delay_ms(60);
	res = CAN_IAP();
	while(1)
	{

	}
}
