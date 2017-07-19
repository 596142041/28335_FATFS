/*
 * can_driver.c
 *
 *  Created on: 2016��12��15��
 *      Author: lpr
 */
#include "can_driver.h"
/**********************************
 * ������:void CAN_boot_Rx_Config(void)
 * �������:��;
 * ����ֵ:��;
 * ��ע:��Ҫ��ʼ��CAN���ߵĵĽ�������,ʹ������31,Ŀǰ������һ��;
 *
 */
void CAN_boot_Rx_Config(void)
{
	struct ECAN_REGS ECanaShadow;
	ECanaShadow.CANME.all = ECanaRegs.CANME.all;
	ECanaShadow.CANME.bit.ME31 = 0;//��ʹ������31
	ECanaRegs.CANME.all = ECanaShadow.CANME.all;
	/*----------���´��������ý����������ش���------------*/
	/*
	LAM[28:0]
	��Щλ����һ��������Ϣ�������ʶ��λ�����Ρ�
	1 ��Խ��ܵ��ı�ʶ������Ӧλ�� ����һ�� 0 �� 1�� �޹أ� ��
	0 ���յ��ı�ʶ��λֵ������ MSGID �Ĵ�������Ӧ��ʶ��λ��ƥ�䡣
	*/
	/*
	LAMI ���ؽ������α�ʶ����չλ
	1 ���Խ��ձ�׼����չ֡�� ����չ֡������£� ��ʶ�������� 29 λ���洢�������У� ���ؽ������μ�
	���������� 29 λ��������ʹ�á� ��һ����׼֡������£� ֻ�б�ʶ����ͷ 11 ��λ�� 28 �� 18 λ��
	�ͱ��ؽ������α�ʹ�á�
	0 �洢�������еı�ʶ����չλ��������Щ��ϢӦ�ñ����յ�
	*/
	//����31�������
	ECanaMboxes.MBOX31.MSGCTRL.bit.DLC = 8;//�������ݳ��ȣ�Ӧ����û�����;
	ECanaMboxes.MBOX31.MSGID.bit.AME =1;//����ʹ��λ,
	ECanaMboxes.MBOX31.MSGID.bit.IDE = CAN_ID_EXT;
	ECanaMboxes.MBOX31.MSGID.all = 0x000001238;//���ý�����Ϣ����ЧID
	ECanaLAMRegs.LAM31.bit.LAM_H = 0x0000;//
	ECanaLAMRegs.LAM31.bit.LAM_L = 0x0000;
	ECanaLAMRegs.LAM31.bit.LAMI = 1;
	ECanaRegs.CANRMP.all      = 0xFFFFFFFF;
	ECanaShadow.CANMD.all     = ECanaRegs.CANMD.all;
	ECanaShadow.CANMD.bit.MD31 = 1;
	ECanaRegs.CANMD.all       = ECanaShadow.CANMD.all;
	ECanaShadow.CANME.all       = ECanaRegs.CANME.all;
	ECanaShadow.CANME.bit.ME31  = 1;//ʹ������1
	ECanaRegs.CANME.all         = ECanaShadow.CANME.all;
}
