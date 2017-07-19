/*
 * CAN_ISP.h
 *
 *  Created on: 2017��1��5��
 *      Author: lpr
 */

#ifndef CAN_ISP_INC_CAN_ISP_H_
#define CAN_ISP_INC_CAN_ISP_H_
#include "ff.h"
#include "diskio.h"
#include "CAN.h"
#define ACK 0x79
#define NACK 0x1F
typedef enum
{
	CAN_125kbps = 1,
    CAN_250kbps = 2,
	CAN_500kbps = 3,
	CAN_1Mbps   = 4

}CAN_Speed;
typedef struct
{
	unsigned char Get;
	unsigned char GV;
	unsigned char GID;
	unsigned char speed;//�ٶ�����
	CAN_Speed Speed;
	unsigned char RD;
	unsigned char Go;
	unsigned char WR_cmd;
	unsigned char WR_dara;
	unsigned char Erase;
	unsigned char ack;
}ISP_CMD_LIST;
typedef struct
{
		union
		{
			unsigned short int all;
			struct
			{
				unsigned short int StdId:11;//�豸��ַ
				unsigned short int reserve:5;//����λ
			}bit;
		}StdId;//��չ֡ID
		unsigned char IDE;   //֡����,��Ϊ��CAN_ID_STD(��׼֡),CAN_ID_EXT(��չ֡)
		unsigned char DLC;  //���ݳ��ȣ���Ϊ0��8;
		u8 data[8];
}ISP_data;
void CANB_GPIO_Config(void);
void CANB_Config(void);
void CANB_Tx_Msg(CanTxMsg *can_tx_msg);
void CANB_Tx_Msg_CLR(CanTxMsg *can_tx_msg);
void CANB_Rx_Msg(CanRxMsg *can_rx_msg);
void CANB_Rx_Msg_CLR(CanRxMsg *can_rx_msg);
void CANB_Rx_Config(void);
unsigned char  CAN_ISP(void);
extern ISP_CMD_LIST isp_cmd_list;
extern CanTxMsg canb_tx_msg;
extern CanRxMsg canb_rx_msg;
extern ISP_data isp_data;
#endif /* CAN_ISP_INC_CAN_ISP_H_ */
