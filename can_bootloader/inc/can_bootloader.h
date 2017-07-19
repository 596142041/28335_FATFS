/*
 * can_bootloader.h
 *
 *  Created on: 2016��12��11��
 *      Author: lpr
 */

#ifndef CAN_BOOTLOADER_INC_CAN_BOOTLOADER_H_
#define CAN_BOOTLOADER_INC_CAN_BOOTLOADER_H_
#include "ff.h"
#include "diskio.h"
#include "CAN.h"
#define Devie_addr  0x123  //�õ�ַ���豸��ַ;
//--���º궨���Ŷ�ǵ�ǰ�豸���еĹ̼�����
#define CAN_BL_BOOT     0x55555555  //���е���Boot
#define CAN_BL_APP      0xAAAAAAAA //���е���APP
//
typedef struct
{
	union
	{
		u32 all;
		struct
		{
			u16 cmd:4;//����
			u16 addr:12;//�豸��ַ
			u16 reserve:16;//����λ
		}bit;
	}ExtId;//��չ֡ID
	unsigned char IDE;   //֡����,��Ϊ��CAN_ID_STD(��׼֡),CAN_ID_EXT(��չ֡)
	unsigned char DLC;  //���ݳ��ȣ���Ϊ0��8;
	u8 data[8];
}bootloader_data;
typedef struct{
  //Bootloader�������
  unsigned char Erase;        //����APP������������
  unsigned char WriteInfo;    //���ö��ֽ�д������ز�����д��ʼ��ַ����������
  unsigned char Write;        //�Զ��ֽ���ʽд����
  unsigned char Check;        //���ڵ��Ƿ����ߣ�ͬʱ���ع̼���Ϣ
  unsigned char SetBaudRate;  //���ýڵ㲨����
  unsigned char Excute;       //ִ�й̼�
  //�ڵ㷵��״̬
  unsigned char CmdSuccess;   //����ִ�гɹ�
  unsigned char CmdFaild;     //����ִ��ʧ��
} Boot_CMD_LIST;

unsigned short int CRCcalc16 (unsigned char *data,unsigned short int len);
u8 CAN_IAP(void);
extern Boot_CMD_LIST cmd_list;
extern bootloader_data Bootloader_data;
#endif /* CAN_BOOTLOADER_INC_CAN_BOOTLOADER_H_ */
