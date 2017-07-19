/*
 * CAN.c
 *
 *  Created on: 2016��6��24��
 *      Author: lpr
 */
#include "CAN.h"
CanTxMsg can_tx_msg;
CanRxMsg can_rx_msg =
{
	.upflag = 0,
};
#if USE_CANA
static void CANA_Config(void)
{
		struct ECAN_REGS ECanaShadow;
		EALLOW;
		SysCtrlRegs.PCLKCR0.bit.ECANAENCLK = 1;
		EDIS;
		EALLOW;		// EALLOW enables access to protected bits

	/* Configure eCAN RX and TX pins for CAN operation using eCAN regs*/

	    ECanaShadow.CANTIOC.all = ECanaRegs.CANTIOC.all;
	    ECanaShadow.CANTIOC.bit.TXFUNC = 1;
	    ECanaRegs.CANTIOC.all = ECanaShadow.CANTIOC.all;

	    ECanaShadow.CANRIOC.all = ECanaRegs.CANRIOC.all;
	    ECanaShadow.CANRIOC.bit.RXFUNC = 1;
	    ECanaRegs.CANRIOC.all = ECanaShadow.CANRIOC.all;

	/* Configure eCAN for HECC mode - (reqd to access mailboxes 16 thru 31) */
	// HECC mode also enables time-stamping feature
	    //ѡ��CAN����ģʽ������eCANģʽ,32�����䶼������
		ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
		ECanaShadow.CANMC.bit.SCB = 1;
		ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;

	/* Initialize all bits of 'Master Control Field' to zero */
	// Some bits of MSGCTRL register come up in an unknown state. For proper operation,
	// all bits (including reserved bits) of MSGCTRL must be initialized to zero

		ECanaMboxes.MBOX0.MSGCTRL.all  = 0x00000000;
	    ECanaMboxes.MBOX1.MSGCTRL.all  = 0x00000000;
	    ECanaMboxes.MBOX2.MSGCTRL.all  = 0x00000000;
	    ECanaMboxes.MBOX3.MSGCTRL.all  = 0x00000000;
	    ECanaMboxes.MBOX4.MSGCTRL.all  = 0x00000000;
	    ECanaMboxes.MBOX5.MSGCTRL.all  = 0x00000000;
	    ECanaMboxes.MBOX6.MSGCTRL.all  = 0x00000000;
	    ECanaMboxes.MBOX7.MSGCTRL.all  = 0x00000000;
	    ECanaMboxes.MBOX8.MSGCTRL.all  = 0x00000000;
	    ECanaMboxes.MBOX9.MSGCTRL.all  = 0x00000000;
	    ECanaMboxes.MBOX10.MSGCTRL.all = 0x00000000;
	    ECanaMboxes.MBOX11.MSGCTRL.all = 0x00000000;
	    ECanaMboxes.MBOX12.MSGCTRL.all = 0x00000000;
	    ECanaMboxes.MBOX13.MSGCTRL.all = 0x00000000;
	    ECanaMboxes.MBOX14.MSGCTRL.all = 0x00000000;
	    ECanaMboxes.MBOX15.MSGCTRL.all = 0x00000000;
	    ECanaMboxes.MBOX16.MSGCTRL.all = 0x00000000;
	    ECanaMboxes.MBOX17.MSGCTRL.all = 0x00000000;
	    ECanaMboxes.MBOX18.MSGCTRL.all = 0x00000000;
	    ECanaMboxes.MBOX19.MSGCTRL.all = 0x00000000;
	    ECanaMboxes.MBOX20.MSGCTRL.all = 0x00000000;
	    ECanaMboxes.MBOX21.MSGCTRL.all = 0x00000000;
	    ECanaMboxes.MBOX22.MSGCTRL.all = 0x00000000;
	    ECanaMboxes.MBOX23.MSGCTRL.all = 0x00000000;
	    ECanaMboxes.MBOX24.MSGCTRL.all = 0x00000000;
	    ECanaMboxes.MBOX25.MSGCTRL.all = 0x00000000;
	    ECanaMboxes.MBOX26.MSGCTRL.all = 0x00000000;
	    ECanaMboxes.MBOX27.MSGCTRL.all = 0x00000000;
	    ECanaMboxes.MBOX28.MSGCTRL.all = 0x00000000;
	    ECanaMboxes.MBOX29.MSGCTRL.all = 0x00000000;
	    ECanaMboxes.MBOX30.MSGCTRL.all = 0x00000000;
	    ECanaMboxes.MBOX31.MSGCTRL.all = 0x00000000;

	// TAn, RMPn, GIFn bits are all zero upon reset and are cleared again
	//	as a matter of precaution.
	    /* Clear all TAn bits */
		ECanaRegs.CANTA.all	= 0xFFFFFFFF;
		/* Clear all RMPn bits */
		ECanaRegs.CANRMP.all = 0xFFFFFFFF;
		/* Clear all interrupt flag bits */
		ECanaRegs.CANGIF0.all = 0xFFFFFFFF;
		ECanaRegs.CANGIF1.all = 0xFFFFFFFF;


	/* Configure bit timing parameters for eCANA*/
//��ʼ����CAN���߲�����
		ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
		ECanaShadow.CANMC.bit.CCR = 1 ;            // Set CCR = 1
	    ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;

	    ECanaShadow.CANES.all = ECanaRegs.CANES.all;

	    do
		{
		    ECanaShadow.CANES.all = ECanaRegs.CANES.all;
	    } while(ECanaShadow.CANES.bit.CCE != 1 );  		// Wait for CCE bit to be set..

	    ECanaShadow.CANBTC.all = 0;

	    #if (CPU_FRQ_150MHZ)
/********************************************************
 *CANͨѶ����������Ϊ500kbps
 *******************************************************/
		ECanaShadow.CANBTC.bit.BRPREG = 9;
		ECanaShadow.CANBTC.bit.TSEG2REG = 2;
		ECanaShadow.CANBTC.bit.TSEG1REG = 10;
	    #endif
	    ECanaShadow.CANBTC.bit.SAM = 1;
	    ECanaRegs.CANBTC.all = ECanaShadow.CANBTC.all;

	    ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
		ECanaShadow.CANMC.bit.CCR = 0 ;            // Set CCR = 0
	    ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;

	    ECanaShadow.CANES.all = ECanaRegs.CANES.all;

	    do
	    {
	       ECanaShadow.CANES.all = ECanaRegs.CANES.all;
	    } while(ECanaShadow.CANES.bit.CCE != 0 ); 		// Wait for CCE bit to be  cleared..

	/* Disable all Mailboxes  */
	 	ECanaRegs.CANME.all = 0;		// Required before writing the MSGIDs

	    EDIS;
}
#endif
#if USE_CANB
static void CANB_Config(void)
{

}
#endif
void CAN_GPIO_Config(void)
{

	//----------
	EALLOW;
#if USE_CANA
	GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;	    // Enable pull-up for GPIO18 (CANRXA)
	GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;	    // Enable pull-up for GPIO19 (CANTXA)
	GpioCtrlRegs.GPAQSEL2.bit.GPIO18 = 3;   // Asynch qual for GPIO18 (CANRXA)
	GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 3;	// Configure GPIO18 for CANRXA operation
	GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 3;	// Configure GPIO19 for CANTXA operation
#endif

#if USE_CANB
	GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0;   // Enable pull-up for GPIO12 (CANTXB)
	GpioCtrlRegs.GPAPUD.bit.GPIO13 = 0;   // Enable pull-up for GPIO13 (CANRXB)
	GpioCtrlRegs.GPAQSEL1.bit.GPIO13 = 3; // Asynch qual for GPIO13 (CANRXB)
	GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 2;  // Configure GPIO12 for CANTXB operation
	GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 2;  // Configure GPIO13 for CANRXB operation
#endif
	EDIS;
}
void CAN_Config(void)
{
	//InitECan();
	CANA_Config();
}
void CAN_Tx_Msg(CanTxMsg *can_tx_msg)
{
	//Uint16 time_cnt;
	Uint32 mbox_enable_temp  = 0x0000;
	Uint32 mbox_disable_temp = 0x0000;
	Uint32 mbox_dir_temp     = 0x0000;
	mbox_enable_temp = 1<<(can_tx_msg->MBox_num);
	mbox_disable_temp = ~(1<<(can_tx_msg->MBox_num));
	mbox_dir_temp = ~(1<<(can_tx_msg->MBox_num));
	struct ECAN_REGS ECanaShadow;
	volatile struct MBOX *Mailbox;
	Mailbox = &ECanaMboxes.MBOX0+can_tx_msg->MBox_num;
	ECanaShadow.CANME.all  = ECanaRegs.CANME.all;
	ECanaShadow.CANME.all &= mbox_disable_temp;
	ECanaRegs.CANME.all    = ECanaShadow.CANME.all;
	if(can_tx_msg->IDE == CAN_ID_STD)
	{

		Mailbox->MSGID.all = can_tx_msg->StdId.bit.StdId; //standard identifier
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
   ECanaShadow.CANMD.all = ECanaRegs.CANMD.all;
   ECanaShadow.CANMD.all &=mbox_dir_temp;//�������乤������,0��ʾ���乤���ڷ���,1��ʾ�����ڽ���
   ECanaRegs.CANMD.all = ECanaShadow.CANMD.all;
   ECanaShadow.CANME.all = ECanaRegs.CANME.all;
   ECanaShadow.CANME.all |= mbox_enable_temp;//ʹ������
   ECanaRegs.CANME.all = ECanaShadow.CANME.all;
   Mailbox->MSGCTRL.bit.DLC = can_tx_msg->DLC;//���ݳ���
   Mailbox->MDL.byte.BYTE0 = can_tx_msg->CAN_Tx_msg_data.msg_Byte.byte0;
   Mailbox->MDL.byte.BYTE1 = can_tx_msg->CAN_Tx_msg_data.msg_Byte.byte1;
   Mailbox->MDL.byte.BYTE2 = can_tx_msg->CAN_Tx_msg_data.msg_Byte.byte2;
   Mailbox->MDL.byte.BYTE3 = can_tx_msg->CAN_Tx_msg_data.msg_Byte.byte3;
   Mailbox->MDH.byte.BYTE4 = can_tx_msg->CAN_Tx_msg_data.msg_Byte.byte4;
   Mailbox->MDH.byte.BYTE5 = can_tx_msg->CAN_Tx_msg_data.msg_Byte.byte5;
   Mailbox->MDH.byte.BYTE6 = can_tx_msg->CAN_Tx_msg_data.msg_Byte.byte6;
   Mailbox->MDH.byte.BYTE7 = can_tx_msg->CAN_Tx_msg_data.msg_Byte.byte7;
   ECanaShadow.CANTRS.all = 0;
   ECanaShadow.CANTRS.all |= mbox_enable_temp;             // Set TRS for mailbox under test
   ECanaRegs.CANTRS.all = ECanaShadow.CANTRS.all;
   do
	{
	ECanaShadow.CANTA.all = ECanaRegs.CANTA.all;

	} while(((ECanaShadow.CANTA.all&mbox_enable_temp) == 0 ));
   ECanaShadow.CANTA.all = 0;
   ECanaShadow.CANTA.all = mbox_enable_temp;
   ECanaRegs.CANTA.all = ECanaShadow.CANTA.all;

}
void CAN_Rx_Msg(CanRxMsg *can_rx_msg)
{
//	struct ECAN_REGS ECanaShadow;
	volatile struct MBOX *Mailbox;
	Mailbox = &ECanaMboxes.MBOX0+can_rx_msg->MBox_num;
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
void CAN_Rx_Config(void)
{

	struct ECAN_REGS ECanaShadow;
	ECanaShadow.CANME.all = ECanaRegs.CANME.all;
	ECanaShadow.CANME.bit.ME1 = 0;//��ʹ������1
	ECanaShadow.CANME.bit.ME31 = 0;//��ʹ������31
	ECanaRegs.CANME.all = ECanaShadow.CANME.all;
	/*----------���´��������ý����������ش���------------*/
	//����1�������
	ECanaMboxes.MBOX1.MSGCTRL.bit.DLC = 8;//�������ݳ��ȣ�Ӧ����û�����;
	ECanaMboxes.MBOX1.MSGID.all = 0x0123C;//���ý�����Ϣ����ЧID
	ECanaMboxes.MBOX1.MSGID.bit.AME =1;//����ʹ��λ,
	ECanaMboxes.MBOX1.MSGID.bit.IDE = CAN_ID_EXT;
	/*
		LAM[28:0]
		��Щλ����һ��������Ϣ�������ʶ��λ�����Ρ�
		1 ��Խ��ܵ��ı�ʶ������Ӧλ�� ����һ�� 0 �� 1�� �޹أ� ��
		0 ���յ��ı�ʶ��λֵ������ MSGID �Ĵ�������Ӧ��ʶ��λ��ƥ�䡣
	*/

	ECanaLAMRegs.LAM1.all = 0x000000F;//
	 /*
		LAMI ���ؽ������α�ʶ����չλ
		1 ���Խ��ձ�׼����չ֡�� ����չ֡������£� ��ʶ�������� 29 λ���洢�������У� ���ؽ������μ�
		���������� 29 λ��������ʹ�á� ��һ����׼֡������£� ֻ�б�ʶ����ͷ 11 ��λ�� 28 �� 18 λ��
		�ͱ��ؽ������α�ʹ�á�
		0 �洢�������еı�ʶ����չλ��������Щ��ϢӦ�ñ����յ�
	*/
	ECanaLAMRegs.LAM1.bit.LAMI = 1;
	//����31�������
	ECanaMboxes.MBOX31.MSGCTRL.bit.DLC = 8;//�������ݳ��ȣ�Ӧ����û�����;
	ECanaMboxes.MBOX31.MSGID.all = 0x07909ABC;//���ý�����Ϣ����ЧID
	ECanaMboxes.MBOX31.MSGID.bit.AME =1;//����ʹ��λ,
	ECanaMboxes.MBOX31.MSGID.bit.IDE = CAN_ID_EXT;
	/*
		LAM[28:0]
		��Щλ����һ��������Ϣ�������ʶ��λ�����Ρ�
		1 ��Խ��ܵ��ı�ʶ������Ӧλ�� ����һ�� 0 �� 1�� �޹أ� ��
		0 ���յ��ı�ʶ��λֵ������ MSGID �Ĵ�������Ӧ��ʶ��λ��ƥ�䡣
	*/

	ECanaLAMRegs.LAM31.all = 0x0000000;//
	 /*
		LAMI ���ؽ������α�ʶ����չλ
		1 ���Խ��ձ�׼����չ֡�� ����չ֡������£� ��ʶ�������� 29 λ���洢�������У� ���ؽ������μ�
		���������� 29 λ��������ʹ�á� ��һ����׼֡������£� ֻ�б�ʶ����ͷ 11 ��λ�� 28 �� 18 λ��
		�ͱ��ؽ������α�ʹ�á�
		0 �洢�������еı�ʶ����չλ��������Щ��ϢӦ�ñ����յ�
	*/
	ECanaLAMRegs.LAM31.bit.LAMI = 1;
	ECanaRegs.CANRMP.all      = 0xFFFFFFFF;
	ECanaShadow.CANMD.all     = ECanaRegs.CANMD.all;
	ECanaShadow.CANMD.bit.MD1 = 1;
	ECanaShadow.CANMD.bit.MD31 = 1;
	ECanaRegs.CANMD.all       = ECanaShadow.CANMD.all;

	ECanaShadow.CANME.all       = ECanaRegs.CANME.all;
	ECanaShadow.CANME.bit.ME1   = 1;//ʹ������1
	ECanaShadow.CANME.bit.ME31  = 1;//ʹ������1
	ECanaRegs.CANME.all         = ECanaShadow.CANME.all;
}
void CAN_Rx_IT_Concig(void)
{
	EALLOW;
	ECanaRegs.CANMIM.bit.MIM1 = 1;//ʹ���ж�����1���ж�;
	ECanaRegs.CANMIL.bit.MIL1 = 1;//���ж�1�������ж�1;
	ECanaRegs.CANMIM.bit.MIM31 = 1;//ʹ���ж�����1���ж�;
	ECanaRegs.CANMIL.bit.MIL31 = 1;//���ж�1�������ж�1;
	ECanaRegs.CANGIM.bit.I1EN = 1;//ʹ���ж�1;
	EDIS;
}
__interrupt void Ecana_isr1(void)
{
	if(ECanaRegs.CANGIF1.bit.GMIF1 == 1)
	{
		if(ECanaRegs.CANRMP.bit.RMP1 == 1)
		{
			can_rx_msg.MBox_num = ECanaRegs.CANGIF1.bit.MIV1;
			can_rx_msg.DLC = ECanaMboxes.MBOX1.MSGCTRL.bit.DLC;
			can_rx_msg.IDE = ECanaMboxes.MBOX1.MSGID.bit.IDE;
			if(can_rx_msg.IDE == CAN_ID_EXT)
			{
				can_rx_msg.ExtId = ECanaMboxes.MBOX1.MSGID.all&0x1FFFFFFF;
				can_rx_msg.SAE_J1939_ID.id = can_rx_msg.ExtId;
			}
			else if(can_rx_msg.IDE == CAN_ID_STD)
			{
				can_rx_msg.StdId = ECanaMboxes.MBOX1.MSGID.bit.STDMSGID;
			}
		   can_rx_msg.CAN_Rx_msg_data.msg_Byte.byte0 = ECanaMboxes.MBOX1.MDL.byte.BYTE0;
		   can_rx_msg.CAN_Rx_msg_data.msg_Byte.byte1 = ECanaMboxes.MBOX1.MDL.byte.BYTE1;
		   can_rx_msg.CAN_Rx_msg_data.msg_Byte.byte2 = ECanaMboxes.MBOX1.MDL.byte.BYTE2;
		   can_rx_msg.CAN_Rx_msg_data.msg_Byte.byte3 = ECanaMboxes.MBOX1.MDL.byte.BYTE3;
		   can_rx_msg.CAN_Rx_msg_data.msg_Byte.byte4 = ECanaMboxes.MBOX1.MDH.byte.BYTE4;
		   can_rx_msg.CAN_Rx_msg_data.msg_Byte.byte5 = ECanaMboxes.MBOX1.MDH.byte.BYTE5;
		   can_rx_msg.CAN_Rx_msg_data.msg_Byte.byte6 = ECanaMboxes.MBOX1.MDH.byte.BYTE6;
		   can_rx_msg.CAN_Rx_msg_data.msg_Byte.byte7 = ECanaMboxes.MBOX1.MDH.byte.BYTE7;
		   ECanaRegs.CANRMP.bit.RMP1 = 1;
		   can_rx_msg.upflag = 1;
		}
		else if(ECanaRegs.CANRMP.bit.RMP31 == 1)
		{
			can_rx_msg.MBox_num = ECanaRegs.CANGIF1.bit.MIV1;
			can_rx_msg.DLC = ECanaMboxes.MBOX31.MSGCTRL.bit.DLC;
			can_rx_msg.IDE = ECanaMboxes.MBOX31.MSGID.bit.IDE;
			if(can_rx_msg.IDE == CAN_ID_EXT)
			{
				can_rx_msg.ExtId = ECanaMboxes.MBOX31.MSGID.all&0x1FFFFFFF;
				can_rx_msg.SAE_J1939_ID.id = can_rx_msg.ExtId;
			}
			else if(can_rx_msg.IDE == CAN_ID_STD)
			{
				can_rx_msg.StdId = ECanaMboxes.MBOX31.MSGID.bit.STDMSGID;
			}
		   can_rx_msg.CAN_Rx_msg_data.msg_Byte.byte0 = ECanaMboxes.MBOX31.MDL.byte.BYTE0;
		   can_rx_msg.CAN_Rx_msg_data.msg_Byte.byte1 = ECanaMboxes.MBOX31.MDL.byte.BYTE1;
		   can_rx_msg.CAN_Rx_msg_data.msg_Byte.byte2 = ECanaMboxes.MBOX31.MDL.byte.BYTE2;
		   can_rx_msg.CAN_Rx_msg_data.msg_Byte.byte3 = ECanaMboxes.MBOX31.MDL.byte.BYTE3;
		   can_rx_msg.CAN_Rx_msg_data.msg_Byte.byte4 = ECanaMboxes.MBOX31.MDH.byte.BYTE4;
		   can_rx_msg.CAN_Rx_msg_data.msg_Byte.byte5 = ECanaMboxes.MBOX31.MDH.byte.BYTE5;
		   can_rx_msg.CAN_Rx_msg_data.msg_Byte.byte6 = ECanaMboxes.MBOX31.MDH.byte.BYTE6;
		   can_rx_msg.CAN_Rx_msg_data.msg_Byte.byte7 = ECanaMboxes.MBOX31.MDH.byte.BYTE7;
		   ECanaRegs.CANRMP.bit.RMP31 = 1;
		}
	}
	PieCtrlRegs.PIEACK.bit.ACK9 = 1;
	//ECanaRegs.CANGIF1.bit.GMIF1 = 1;

}
