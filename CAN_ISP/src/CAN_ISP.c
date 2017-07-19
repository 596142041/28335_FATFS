/*
 * CAN_ISP.c
 *
 *  Created on: 2017��1��5��
 *      Author: lpr
 */
#include "CAN_ISP.h"
unsigned char file_temp[66];
volatile unsigned long int app_start = 0x8000000;
extern FIL fp;
extern FATFS fatfs;
extern UINT read_cnt;
extern FSIZE_t file_size;
ISP_CMD_LIST isp_cmd_list =
{
	.Erase     = 0x43,
	.GID       = 0x02,
	.GV        = 0x01,
	.Get       = 0x00,
	.Go        = 0x21,
	.RD        = 0x11,
	.WR_cmd    = 0x31,
	.WR_dara   = 0x04,
	.Speed     = CAN_125kbps,
	.ack       = 0x79
};
CanTxMsg canb_tx_msg;
CanRxMsg canb_rx_msg =
{
	.upflag = 0,
};
ISP_data isp_data=
{
		.DLC = 0,
		.IDE = CAN_ID_STD,
};
void CANB_GPIO_Config(void)
{
	EALLOW;
	GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0;   // Enable pull-up for GPIO12 (CANTXB)
	GpioCtrlRegs.GPAPUD.bit.GPIO13 = 0;   // Enable pull-up for GPIO13 (CANRXB)
	GpioCtrlRegs.GPAQSEL1.bit.GPIO13 = 3; // Asynch qual for GPIO13 (CANRXB)
	GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 2;  // Configure GPIO12 for CANTXB operation
	GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 2;  // Configure GPIO13 for CANRXB operation
	EDIS;
}
void CANB_Config(void)
{
	struct ECAN_REGS ECanbShadow;
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.ECANBENCLK = 1;
	EDIS;
	EALLOW;		// EALLOW enables access to protected bits

/* Configure eCAN RX and TX pins for CAN operation using eCAN regs*/

    ECanbShadow.CANTIOC.all = ECanbRegs.CANTIOC.all;
    ECanbShadow.CANTIOC.bit.TXFUNC = 1;
    ECanbRegs.CANTIOC.all = ECanbShadow.CANTIOC.all;

    ECanbShadow.CANRIOC.all = ECanbRegs.CANRIOC.all;
    ECanbShadow.CANRIOC.bit.RXFUNC = 1;
    ECanbRegs.CANRIOC.all = ECanbShadow.CANRIOC.all;

/* Configure eCAN for HECC mode - (reqd to access mailboxes 16 thru 31) */
// HECC mode also enables time-stamping feature
    //ѡ��CAN����ģʽ������eCANģʽ,32�����䶼������
	ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
	ECanbShadow.CANMC.bit.SCB = 1;
	ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;

/* Initialize all bits of 'Master Control Field' to zero */
// Some bits of MSGCTRL register come up in an unknown state. For proper operation,
// all bits (including reserved bits) of MSGCTRL must be initialized to zero

	ECanbMboxes.MBOX0.MSGCTRL.all  = 0x00000000;
    ECanbMboxes.MBOX1.MSGCTRL.all  = 0x00000000;
    ECanbMboxes.MBOX2.MSGCTRL.all  = 0x00000000;
    ECanbMboxes.MBOX3.MSGCTRL.all  = 0x00000000;
    ECanbMboxes.MBOX4.MSGCTRL.all  = 0x00000000;
    ECanbMboxes.MBOX5.MSGCTRL.all  = 0x00000000;
    ECanbMboxes.MBOX6.MSGCTRL.all  = 0x00000000;
    ECanbMboxes.MBOX7.MSGCTRL.all  = 0x00000000;
    ECanbMboxes.MBOX8.MSGCTRL.all  = 0x00000000;
    ECanbMboxes.MBOX9.MSGCTRL.all  = 0x00000000;
    ECanbMboxes.MBOX10.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX11.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX12.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX13.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX14.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX15.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX16.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX17.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX18.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX19.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX20.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX21.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX22.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX23.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX24.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX25.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX26.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX27.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX28.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX29.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX30.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX31.MSGCTRL.all = 0x00000000;

// TAn, RMPn, GIFn bits are all zero upon reset and are cleared again
//	as a matter of precaution.
    /* Clear all TAn bits */
	ECanbRegs.CANTA.all	= 0xFFFFFFFF;
	/* Clear all RMPn bits */
	ECanbRegs.CANRMP.all = 0xFFFFFFFF;
	/* Clear all interrupt flag bits */
	ECanbRegs.CANGIF0.all = 0xFFFFFFFF;
	ECanbRegs.CANGIF1.all = 0xFFFFFFFF;


/* Configure bit timing parameters for eCANA*/
//��ʼ����CAN���߲�����
	ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
	ECanbShadow.CANMC.bit.CCR = 1 ;            // Set CCR = 1
    ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;

    ECanbShadow.CANES.all = ECanbRegs.CANES.all;

    do
	{
	    ECanbShadow.CANES.all = ECanbRegs.CANES.all;
    } while(ECanbShadow.CANES.bit.CCE != 1 );  		// Wait for CCE bit to be set..

    ECanbShadow.CANBTC.all = 0;

    #if (CPU_FRQ_150MHZ)
/********************************************************
*CANͨѶ����������Ϊ125kbps
*******************************************************/
	ECanbShadow.CANBTC.bit.BRPREG = 39;
	ECanbShadow.CANBTC.bit.TSEG2REG = 2;
	ECanbShadow.CANBTC.bit.TSEG1REG = 10;
    #endif
    ECanbShadow.CANBTC.bit.SAM = 1;
    ECanbRegs.CANBTC.all = ECanbShadow.CANBTC.all;

    ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
	ECanbShadow.CANMC.bit.CCR = 0 ;            // Set CCR = 0
    ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;

    ECanbShadow.CANES.all = ECanbRegs.CANES.all;

    do
    {
       ECanbShadow.CANES.all = ECanbRegs.CANES.all;
    } while(ECanbShadow.CANES.bit.CCE != 0 ); 		// Wait for CCE bit to be  cleared..

/* Disable all Mailboxes  */
 	ECanbRegs.CANME.all = 0;		// Required before writing the MSGIDs

    EDIS;
}
void CANB_Tx_Msg(CanTxMsg *can_tx_msg)
{
	Uint32 mbox_enable_temp  = 0x0000;
	Uint32 mbox_disable_temp = 0x0000;
	Uint32 mbox_dir_temp     = 0x0000;
	mbox_enable_temp = 1<<(can_tx_msg->MBox_num);
	mbox_disable_temp = ~(1<<(can_tx_msg->MBox_num));
	mbox_dir_temp = ~(1<<(can_tx_msg->MBox_num));
	struct ECAN_REGS ECanbShadow;
	volatile struct MBOX *Mailbox;
	Mailbox = &ECanbMboxes.MBOX0+can_tx_msg->MBox_num;
	ECanbShadow.CANME.all  = ECanbRegs.CANME.all;
	ECanbShadow.CANME.all &= mbox_disable_temp;
	ECanbRegs.CANME.all    = ECanbShadow.CANME.all;
	if(can_tx_msg->IDE == CAN_ID_STD)
	{

		Mailbox->MSGID.bit.STDMSGID = can_tx_msg->StdId.bit.StdId; //standard identifier
		Mailbox->MSGID.bit.IDE = can_tx_msg->IDE;
	}
	else if(can_tx_msg->IDE == CAN_ID_EXT)
	{
		if(can_tx_msg->SAE_J1939_Flag == 0)
		{
			Mailbox->MSGID.all = can_tx_msg->ExtId.bit.ExtId; //extended identifier.
			Mailbox->MSGID.bit.IDE = can_tx_msg->IDE;
		}
		else
		{
			Mailbox->MSGID.all = can_tx_msg->SAE_J1939_ID.id; //extended identifier.
			Mailbox->MSGID.bit.IDE = can_tx_msg->IDE;
		}
	}
   ECanbShadow.CANMD.all = ECanbRegs.CANMD.all;
   ECanbShadow.CANMD.all &=mbox_dir_temp;//�������乤������,0��ʾ���乤���ڷ���,1��ʾ�����ڽ���
   ECanbRegs.CANMD.all = ECanbShadow.CANMD.all;
   ECanbShadow.CANME.all = ECanbRegs.CANME.all;
   ECanbShadow.CANME.all |= mbox_enable_temp;//ʹ������
   ECanbRegs.CANME.all = ECanbShadow.CANME.all;
   Mailbox->MSGCTRL.bit.DLC = can_tx_msg->DLC;//���ݳ���
   Mailbox->MDL.byte.BYTE0 = can_tx_msg->CAN_Tx_msg_data.msg_Byte.byte0;
   Mailbox->MDL.byte.BYTE1 = can_tx_msg->CAN_Tx_msg_data.msg_Byte.byte1;
   Mailbox->MDL.byte.BYTE2 = can_tx_msg->CAN_Tx_msg_data.msg_Byte.byte2;
   Mailbox->MDL.byte.BYTE3 = can_tx_msg->CAN_Tx_msg_data.msg_Byte.byte3;
   Mailbox->MDH.byte.BYTE4 = can_tx_msg->CAN_Tx_msg_data.msg_Byte.byte4;
   Mailbox->MDH.byte.BYTE5 = can_tx_msg->CAN_Tx_msg_data.msg_Byte.byte5;
   Mailbox->MDH.byte.BYTE6 = can_tx_msg->CAN_Tx_msg_data.msg_Byte.byte6;
   Mailbox->MDH.byte.BYTE7 = can_tx_msg->CAN_Tx_msg_data.msg_Byte.byte7;
   ECanbShadow.CANTRS.all = 0;
   ECanbShadow.CANTRS.all |= mbox_enable_temp;             // Set TRS for mailbox under test
   ECanbRegs.CANTRS.all = ECanbShadow.CANTRS.all;
   do
	{
	ECanbShadow.CANTA.all = ECanbRegs.CANTA.all;

	} while(((ECanbShadow.CANTA.all&mbox_enable_temp) == 0 ));
   ECanbShadow.CANTA.all = 0;
   ECanbShadow.CANTA.all = mbox_enable_temp;
   ECanbRegs.CANTA.all = ECanbShadow.CANTA.all;

}
void CANB_Tx_Msg_CLR(CanTxMsg *can_tx_msg)
{
	int i;
	for(i = 0;i<8;i++)
	{
		can_tx_msg->CAN_Tx_msg_data.msg_byte.data[i] = 0;
	}
	can_tx_msg->DLC = 0x00;
	can_tx_msg->ExtId.all = 0x00;
	can_tx_msg->IDE = 0xFF;
	can_tx_msg->MBox_num =0xFF;
	can_tx_msg->SAE_J1939_Flag = 0;
	can_tx_msg->SAE_J1939_ID.id = 0x00;
	can_tx_msg->StdId.all = 0x00;
	can_tx_msg->Tx_timeout_cnt = 0x00;
}
void CANB_Rx_Msg(CanRxMsg *can_rx_msg)
{
	volatile struct MBOX *Mailbox;
	Mailbox = &ECanbMboxes.MBOX0+can_rx_msg->MBox_num;
	can_rx_msg->DLC = Mailbox->MSGCTRL.bit.DLC;
	can_rx_msg->IDE = Mailbox->MSGID.bit.IDE;
	if(can_rx_msg->IDE == CAN_ID_EXT)
	{
		can_rx_msg->ExtId = Mailbox->MSGID.all&0x1FFFFFFF;
		can_rx_msg->SAE_J1939_ID.id = can_rx_msg->ExtId;
	}
	else if(can_rx_msg->IDE == CAN_ID_STD)
	{
		can_rx_msg->StdId = Mailbox->MSGID.bit.STDMSGID;
	}
	can_rx_msg->CAN_Rx_msg_data.msg_Byte.byte0 = Mailbox->MDL.byte.BYTE0;
	can_rx_msg->CAN_Rx_msg_data.msg_Byte.byte1 = Mailbox->MDL.byte.BYTE1;
	can_rx_msg->CAN_Rx_msg_data.msg_Byte.byte2 = Mailbox->MDL.byte.BYTE2;
	can_rx_msg->CAN_Rx_msg_data.msg_Byte.byte3 = Mailbox->MDL.byte.BYTE3;
	can_rx_msg->CAN_Rx_msg_data.msg_Byte.byte4 = Mailbox->MDH.byte.BYTE4;
	can_rx_msg->CAN_Rx_msg_data.msg_Byte.byte5 = Mailbox->MDH.byte.BYTE5;
	can_rx_msg->CAN_Rx_msg_data.msg_Byte.byte6 = Mailbox->MDH.byte.BYTE6;
	can_rx_msg->CAN_Rx_msg_data.msg_Byte.byte7 = Mailbox->MDH.byte.BYTE7;
}
void CANB_Rx_Msg_CLR(CanRxMsg *can_rx_msg)
{
	int i;
	for( i = 0;i<8;i++)
	{
		can_rx_msg->CAN_Rx_msg_data.msg_byte.data[i] = 0;
	}
	can_rx_msg->DLC = 0;
	can_rx_msg->ExtId = 0;
	can_rx_msg->IDE = 0xFF;
	can_rx_msg->MBox_num = 0xFF;
	can_rx_msg->Rx_timeout_cnt = 0x00;
	can_rx_msg->SAE_J1939_Flag = 0x00;
	can_rx_msg->SAE_J1939_ID.id = 0x00;
	can_rx_msg->StdId = 0x00;
	can_rx_msg->upflag = 0xFF;
}
void CANB_Rx_Config(void)
{
	/******************************************************
	 *��Ҫ���յ����ݰ���
	 * 	.Erase     = 0x43, ����31
	 *	.GID       = 0x02, ����30
	 *	.GV        = 0x01, ����29
	 *	.Get       = 0x00, ����28
	 *	.Go        = 0x21, ����27
	 *	.RD        = 0x11, ����26
	 *	.WR_cmd    = 0x31, ����25
	 *	.WR_dara   = 0x04, ����24
	 *  .ack       = 0x79, ����23
	 *******************************************************/
	 /*************************************************************
		LAMI ���ؽ������α�ʶ����չλ
		1 ���Խ��ձ�׼����չ֡�� ����չ֡������£� ��ʶ�������� 29 λ���洢�������У� ���ؽ������μ�
		���������� 29 λ��������ʹ�á� ��һ����׼֡������£� ֻ�б�ʶ����ͷ 11 ��λ�� 28 �� 18 λ��
		�ͱ��ؽ������α�ʹ�á�
		0 �洢�������еı�ʶ����չλ��������Щ��ϢӦ�ñ����յ�
	**************************************************************/
	/*************************************************************
		LAM[28:0]
		��Щλ����һ��������Ϣ�������ʶ��λ�����Ρ�
		1 ��Խ��ܵ��ı�ʶ������Ӧλ�� ����һ�� 0 �� 1�� �޹أ� ��
		0 ���յ��ı�ʶ��λֵ������ MSGID �Ĵ�������Ӧ��ʶ��λ��ƥ�䡣
	***************************************************************/
	struct ECAN_REGS ECanbShadow;
	ECanbShadow.CANME.all = ECanbRegs.CANME.all;
	ECanbShadow.CANME.bit.ME31 = 0;//��ʹ������31
	ECanbShadow.CANME.bit.ME30 = 0;
	ECanbShadow.CANME.bit.ME29 = 0;
	ECanbShadow.CANME.bit.ME28 = 0;
	ECanbShadow.CANME.bit.ME27 = 0;
	ECanbShadow.CANME.bit.ME26 = 0;
	ECanbShadow.CANME.bit.ME25 = 0;
	ECanbShadow.CANME.bit.ME24 = 0;
	ECanbShadow.CANME.bit.ME23 = 0;
	ECanbRegs.CANME.all = ECanbShadow.CANME.all;
	/*----------���´��������ý����������ش���------------*/
	/******************************************************
		 *��Ҫ���յ����ݰ���
		 * 	.Erase     = 0x43, ����31
		 *	.GID       = 0x02, ����30
		 *	.GV        = 0x01, ����29
		 *	.Get       = 0x00, ����28
		 *	.Go        = 0x21, ����27
		 *	.RD        = 0x11, ����26
		 *	.WR_cmd    = 0x31, ����25
		 *	.WR_dara   = 0x04, ����24
		 *  .ack       = 0x79, ����23
		 *******************************************************/
	//����31�������
	ECanbMboxes.MBOX31.MSGCTRL.bit.DLC = 8;//�������ݳ��ȣ�Ӧ����û�����;
	ECanbMboxes.MBOX31.MSGID.bit.STDMSGID = isp_cmd_list.Erase;//���ý�����Ϣ����ЧID
	ECanbMboxes.MBOX31.MSGID.bit.AME =1;//����ʹ��λ,
	ECanbMboxes.MBOX31.MSGID.bit.IDE = CAN_ID_STD;
	ECanbLAMRegs.LAM31.all = 0x0000000;
	ECanbLAMRegs.LAM31.bit.LAMI = 1;
	//����30�������
	ECanbMboxes.MBOX30.MSGID.bit.STDMSGID = isp_cmd_list.GID;
	ECanbMboxes.MBOX30.MSGID.bit.AME = 1;
	ECanbMboxes.MBOX30.MSGID.bit.IDE = CAN_ID_STD;
	ECanbLAMRegs.LAM30.all = 0x00000000;
	ECanbLAMRegs.LAM30.bit.LAMI = 1;
	//����29�������
	ECanbMboxes.MBOX29.MSGID.bit.STDMSGID = isp_cmd_list.GV;
	ECanbMboxes.MBOX29.MSGID.bit.AME = 1;
	ECanbMboxes.MBOX29.MSGID.bit.IDE = CAN_ID_STD;
	ECanbLAMRegs.LAM29.all = 0x00000000;
	ECanbLAMRegs.LAM29.bit.LAMI = 1;
	//����28�������
	ECanbMboxes.MBOX28.MSGID.bit.STDMSGID = isp_cmd_list.Get;
	ECanbMboxes.MBOX28.MSGID.bit.AME = 1;
	ECanbMboxes.MBOX28.MSGID.bit.IDE = CAN_ID_STD;
	ECanbLAMRegs.LAM28.all = 0x00000000;
	ECanbLAMRegs.LAM28.bit.LAMI = 1;
	//����29�������
	ECanbMboxes.MBOX27.MSGID.bit.STDMSGID = isp_cmd_list.Go;
	ECanbMboxes.MBOX27.MSGID.bit.AME = 1;
	ECanbMboxes.MBOX27.MSGID.bit.IDE = CAN_ID_STD;
	ECanbLAMRegs.LAM27.all = 0x00000000;
	ECanbLAMRegs.LAM27.bit.LAMI = 1;
	//����29�������
	ECanbMboxes.MBOX26.MSGID.bit.STDMSGID = isp_cmd_list.RD;
	ECanbMboxes.MBOX26.MSGID.bit.AME = 1;
	ECanbMboxes.MBOX26.MSGID.bit.IDE = CAN_ID_STD;
	ECanbLAMRegs.LAM26.all = 0x00000000;
	ECanbLAMRegs.LAM26.bit.LAMI = 1;
	//����29�������
	ECanbMboxes.MBOX25.MSGID.bit.STDMSGID = isp_cmd_list.WR_cmd;
	ECanbMboxes.MBOX25.MSGID.bit.AME = 1;
	ECanbMboxes.MBOX25.MSGID.bit.IDE = CAN_ID_STD;
	ECanbLAMRegs.LAM25.all = 0x00000000;
	ECanbLAMRegs.LAM25.bit.LAMI = 1;
	//����29�������
	ECanbMboxes.MBOX24.MSGID.bit.STDMSGID = isp_cmd_list.WR_dara;
	ECanbMboxes.MBOX24.MSGID.bit.AME = 1;
	ECanbMboxes.MBOX24.MSGID.bit.IDE = CAN_ID_STD;
	ECanbLAMRegs.LAM24.all = 0x00000000;
	ECanbLAMRegs.LAM24.bit.LAMI = 1;
	//����29�������
	ECanbMboxes.MBOX23.MSGID.bit.STDMSGID = isp_cmd_list.ack;
	ECanbMboxes.MBOX23.MSGID.bit.AME = 1;
	ECanbMboxes.MBOX23.MSGID.bit.IDE = CAN_ID_STD;
	ECanbLAMRegs.LAM23.all = 0x00000000;
	ECanbLAMRegs.LAM23.bit.LAMI = 1;
	ECanbRegs.CANRMP.all      = 0xFFFFFFFF;
	ECanbShadow.CANMD.all     = ECanbRegs.CANMD.all;
	ECanbShadow.CANMD.bit.MD31 = 1;
	ECanbShadow.CANMD.bit.MD30 = 1;
	ECanbShadow.CANMD.bit.MD29 = 1;
	ECanbShadow.CANMD.bit.MD28 = 1;
	ECanbShadow.CANMD.bit.MD27 = 1;
	ECanbShadow.CANMD.bit.MD26 = 1;
	ECanbShadow.CANMD.bit.MD25 = 1;
	ECanbShadow.CANMD.bit.MD24 = 1;
	ECanbShadow.CANMD.bit.MD23 = 1;
	ECanbRegs.CANMD.all       = ECanbShadow.CANMD.all;
//ʹ����Ӧ������
	ECanbShadow.CANME.all       = ECanbRegs.CANME.all;
	ECanbShadow.CANME.bit.ME31  = 1;//��ʹ������31
	ECanbShadow.CANME.bit.ME30  = 1;
	ECanbShadow.CANME.bit.ME29  = 1;
	ECanbShadow.CANME.bit.ME28  = 1;
	ECanbShadow.CANME.bit.ME27  = 1;
	ECanbShadow.CANME.bit.ME26  = 1;
	ECanbShadow.CANME.bit.ME25  = 1;
	ECanbShadow.CANME.bit.ME24  = 1;
	ECanbShadow.CANME.bit.ME23  = 1;
	ECanbRegs.CANME.all         = ECanbShadow.CANME.all;
}
unsigned char CAN_ISP(void)
{
	/*********************************************************
	 * ���´�����Ҫ�ο�ST��˾���ṩAN3154��PDF�ļ����İ��Ӣ�İ�
	 *********************************************************/
	int i = 0;
	FRESULT res;
	unsigned char cnt = 0;
	unsigned short int time_out = 0;
	//�����ֳ��õĲ������г�ʼ�������Ͳ��ù�ע;
	isp_data.IDE = CAN_ID_STD;
	canb_tx_msg.ExtId.all = 0x00;
	canb_tx_msg.MBox_num = 12;
	canb_tx_msg.StdId.all = 0x00;
	canb_tx_msg.Tx_timeout_cnt = 500;
	canb_tx_msg.IDE  = CAN_ID_STD;
	//��һ��,����ACK�ź�;
	isp_data.StdId.bit.StdId = isp_cmd_list.ack;
	isp_data.DLC = 0;
	canb_tx_msg.DLC = isp_data.DLC;
	canb_tx_msg.StdId.bit.StdId = isp_data.StdId.bit.StdId;
	for(i = 0;i<canb_tx_msg.DLC;i++)
	{
		//canb_rx_msg.CAN_Rx_msg_data.msg_byte.data[i] = 0;
		canb_tx_msg.CAN_Tx_msg_data.msg_byte.data[i] = isp_data.data[i];
	}
	CANB_Tx_Msg(&canb_tx_msg);
	while(ECanbRegs.CANRMP.bit.RMP23 == 0)
	{
		time_out++;
		if(time_out%30000==0)
		{
			CANB_Tx_Msg(&canb_tx_msg);
		};
		time_out%=60000;
	}
	time_out = 0;
	ECanbRegs.CANRMP.bit.RMP23  = 1;
	canb_rx_msg.MBox_num = 23;
	CANB_Rx_Msg(&canb_rx_msg);
	if(canb_rx_msg.CAN_Rx_msg_data.msg_Byte.byte0 == ACK)
	{
			;
	}
	else
	{
		return 1;//Ӧ���źŴ���
	}
	CANB_Rx_Msg_CLR(&canb_rx_msg);//���������
	//�ڶ�����֤����ָ��
	isp_data.StdId.bit.StdId = isp_cmd_list.Erase;
	isp_data.DLC = 1;
	isp_data.data[0] = 0xFF;
	canb_tx_msg.DLC = isp_data.DLC;
	canb_tx_msg.StdId.bit.StdId = isp_data.StdId.bit.StdId;
	for(i = 0;i<canb_tx_msg.DLC;i++)
	{
		//canb_rx_msg.CAN_Rx_msg_data.msg_byte.data[i] = 0;
		canb_tx_msg.CAN_Tx_msg_data.msg_byte.data[i] = isp_data.data[i];
	}
	CANB_Tx_Msg(&canb_tx_msg);
	while(ECanbRegs.CANRMP.bit.RMP31 == 0)
	{
		time_out++;
		time_out%=60000;
	}
	time_out = 0;
	ECanbRegs.CANRMP.bit.RMP31  = 1;
	canb_rx_msg.MBox_num = 31;
	CANB_Rx_Msg(&canb_rx_msg);
	if(canb_rx_msg.CAN_Rx_msg_data.msg_Byte.byte0 == ACK)
	{
			;
	}
	else
	{
		return 2;//����ʧ�ܴ���
	}
	CANB_Rx_Msg_CLR(&canb_rx_msg);//���������
	while(ECanbRegs.CANRMP.bit.RMP31 == 0)
	{
		time_out++;
		time_out%=50000;
	}
	ECanbRegs.CANRMP.bit.RMP31  = 1;
	canb_rx_msg.MBox_num = 31;
	CANB_Rx_Msg(&canb_rx_msg);
	if(canb_rx_msg.CAN_Rx_msg_data.msg_Byte.byte0 == ACK)
	{
			;
	}
	else
	{
		return 3;//����ʧ�ܴ���
	}
	CANB_Rx_Msg_CLR(&canb_rx_msg);//���������
#if 1
	//������,�ؼ�;��������
	//�����ļ�ϵͳ
	res = f_mount(&fatfs,"0:",1);
	if(res != FR_OK)
	{
		return 4;//�����ļ�ϵͳʧ��
	}
	//���ļ�app.bin
	res = f_open(&fp,"0:/update/isp.bin",FA_READ);
	if(res != FR_OK)
	{
		return 5;//���ļ�ʧ��
	}
	//�ƶ��ļ�ָ�뵽�ļ�ͷ
	f_lseek(&fp,0);
	if(res != FR_OK)
	{
		f_close(&fp);//�ƶ��ļ�ָ��ʧ��,ͬʱ�ر��ļ�����
		return 6;
	}
	//-----������,׼����ȡ����,������ȡ�����ݷ��͸��Է�
	/********************************************
	 * ����ǰ�ķ���һ��,ÿ�ζ���ȡ64�ֽ�,�Ա���STM32д������;
	 * �ڷ������ݰ�֮ǰ��Ҫ���͵�ǰ���ݰ��ĳ��ȼ�����Ҫд���ƫ�Ƶ�ַ
	 * */
	while(file_size < f_size(&fp))
	{
		//��ȡ����
		res = f_read(&fp,file_temp,64,&read_cnt);
		if(res != FR_OK)
		{
			return 7;//��ȡ�ļ�����ʧ��
		}
		//����Goָ��,�����������Ҫд��ĵ�ַ���ֽڳ���
		isp_data.StdId.bit.StdId    = isp_cmd_list.WR_cmd;
		isp_data.DLC                = 5;
		isp_data.data[0]            = (app_start&0xFF000000)>>24;
		isp_data.data[1]            = (app_start&0xFF0000)>>16;
		isp_data.data[2]            = (app_start&0xFF00)>>8;
		isp_data.data[3]            = (app_start&0x00FF);
		isp_data.data[4]            = 64-1;
		canb_tx_msg.DLC             = isp_data.DLC;
		canb_tx_msg.StdId.bit.StdId = isp_data.StdId.bit.StdId;
		for(i = 0;i<canb_tx_msg.DLC;i++)
		{
			canb_tx_msg.CAN_Tx_msg_data.msg_byte.data[i] = isp_data.data[i];
		}
		CANB_Tx_Msg(&canb_tx_msg);
		while(ECanbRegs.CANRMP.bit.RMP25 == 0)
		{
			time_out++;
			if(time_out%30000==0)
			{
				//CANB_Tx_Msg(&canb_tx_msg);
			};
			time_out%=60000;
		}
		time_out = 0;
		ECanbRegs.CANRMP.bit.RMP25 = 1;
		canb_rx_msg.MBox_num = 25;
		CANB_Rx_Msg(&canb_rx_msg);
		if(canb_rx_msg.CAN_Rx_msg_data.msg_Byte.byte0 == ACK)
		{
				;
		}
		else
		{
			return 8;//Ӧ���źŴ���
		}
		CANB_Rx_Msg_CLR(&canb_rx_msg);//���������
		//��������
		while(cnt < 64)
		{
#if 0
			int temp;
			temp = 64-cnt;
			if(temp >8)
			{
				isp_data.StdId.bit.StdId = isp_cmd_list.WR_dara;
				isp_data.DLC             = 8;
				canb_tx_msg.StdId.bit.StdId = isp_data.StdId.bit.StdId;
				canb_tx_msg.DLC             = isp_data.DLC;
				for(i = 0;i<8;i++)
				{
					canb_tx_msg.CAN_Tx_msg_data.msg_byte.data[i] = file_temp[cnt];
					cnt++;
				}
			}
			else
			{
				isp_data.StdId.bit.StdId = isp_cmd_list.WR_dara;
				isp_data.DLC             = temp;
				canb_tx_msg.StdId.bit.StdId = isp_data.StdId.bit.StdId;
				canb_tx_msg.DLC             = isp_data.DLC;
				for(i = 0;i<8;i++)
				{
					canb_tx_msg.CAN_Tx_msg_data.msg_byte.data[i] = file_temp[cnt];
					cnt++;
				}
			}
#endif
			isp_data.StdId.bit.StdId = isp_cmd_list.WR_dara;
			isp_data.DLC             = 8;
			canb_tx_msg.StdId.bit.StdId = isp_data.StdId.bit.StdId;
			canb_tx_msg.DLC             = isp_data.DLC;
			for(i = 0;i<8;i++)
			{
				canb_tx_msg.CAN_Tx_msg_data.msg_byte.data[i] = file_temp[cnt];
				cnt++;
			}
			CANB_Tx_Msg(&canb_tx_msg);
			while(ECanbRegs.CANRMP.bit.RMP25 == 0)
			{
				time_out++;
				if(time_out%30000==0)
				{
					//CANB_Tx_Msg(&canb_tx_msg);
				};
				time_out%=60000;
			}
			time_out = 0;
			ECanbRegs.CANRMP.bit.RMP25 = 1;
			canb_rx_msg.MBox_num = 25;
			CANB_Rx_Msg(&canb_rx_msg);
			if(canb_rx_msg.CAN_Rx_msg_data.msg_Byte.byte0 == ACK)
			{
					;
			}
			else
			{
				return 9;//Ӧ���źŴ���
			}
			CANB_Rx_Msg_CLR(&canb_rx_msg);//���������

		}
		//�ȴ�Ӧ���ź�
		while(ECanbRegs.CANRMP.bit.RMP25 == 0)
		{
			time_out++;
			if(time_out%30000==0)
			{
				//CANB_Tx_Msg(&canb_tx_msg);
			};
			time_out%=60000;
		}
		time_out = 0;
		ECanbRegs.CANRMP.bit.RMP25 = 1;
		canb_rx_msg.MBox_num = 25;
		CANB_Rx_Msg(&canb_rx_msg);
		if(canb_rx_msg.CAN_Rx_msg_data.msg_Byte.byte0 == ACK)
		{
				;
		}
		else
		{
			return 10;//Ӧ���źŴ���
		}
		CANB_Rx_Msg_CLR(&canb_rx_msg);//���������
		//����ļ����ݱ������ϵ�ǰ�Ķ�ȡֵ
		//���ļ���������ȫ����ֵΪ0xFF;
		for(i = 0;i<66;i++)
		{
			file_temp[i] = 0xFF;
		}
		file_size=file_size+read_cnt;
		cnt = 0;
		app_start=app_start+64;
	}
	//����д��ָ��,�����������Ҫд��ĵ�ַ���ֽڳ���
	isp_data.StdId.bit.StdId    = isp_cmd_list.Go;
	isp_data.DLC                = 4;
	isp_data.data[0]            = (0x8000000&0xFF000000)>>24;
	isp_data.data[1]            = (0x8000000&0xFF0000)>>16;
	isp_data.data[2]            = (0x8000000&0xFF00)>>8;
	isp_data.data[3]            = (0x8000000&0x00FF);
	canb_tx_msg.DLC             = isp_data.DLC;
	canb_tx_msg.StdId.bit.StdId = isp_data.StdId.bit.StdId;
	for(i = 0;i<canb_tx_msg.DLC;i++)
	{
		canb_tx_msg.CAN_Tx_msg_data.msg_byte.data[i] = isp_data.data[i];
	}
	CANB_Tx_Msg(&canb_tx_msg);
	while(ECanbRegs.CANRMP.bit.RMP27 == 0)
	{
		time_out++;
		if(time_out%30000==0)
		{
			//CANB_Tx_Msg(&canb_tx_msg);
		};
		time_out%=60000;
	}
	time_out = 0;
	ECanbRegs.CANRMP.bit.RMP27 = 1;
	canb_rx_msg.MBox_num = 27;
	CANB_Rx_Msg(&canb_rx_msg);
	if(canb_rx_msg.CAN_Rx_msg_data.msg_Byte.byte0 == ACK)
	{
			;
	}
	else
	{
		return 11;//Ӧ���źŴ���
	}
	CANB_Rx_Msg_CLR(&canb_rx_msg);//���������
#endif
	read_cnt = 0;
	file_size = 0;;
	return 0xFF;
}
