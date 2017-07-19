/*
 * can_bootloader.h
 *
 *  Created on: 2016年12月11日
 *      Author: lpr
 */

#ifndef CAN_BOOTLOADER_INC_CAN_BOOTLOADER_H_
#define CAN_BOOTLOADER_INC_CAN_BOOTLOADER_H_
#include "ff.h"
#include "diskio.h"
#include "CAN.h"
#define Devie_addr  0x123  //该地址是设备地址;
//--以下宏定义白哦是当前设备运行的固件属性
#define CAN_BL_BOOT     0x55555555  //运行的是Boot
#define CAN_BL_APP      0xAAAAAAAA //运行的是APP
//
typedef struct
{
	union
	{
		u32 all;
		struct
		{
			u16 cmd:4;//命令
			u16 addr:12;//设备地址
			u16 reserve:16;//保留位
		}bit;
	}ExtId;//扩展帧ID
	unsigned char IDE;   //帧类型,可为：CAN_ID_STD(标准帧),CAN_ID_EXT(扩展帧)
	unsigned char DLC;  //数据长度，可为0到8;
	u8 data[8];
}bootloader_data;
typedef struct{
  //Bootloader相关命令
  unsigned char Erase;        //擦出APP储存扇区数据
  unsigned char WriteInfo;    //设置多字节写数据相关参数（写起始地址，数据量）
  unsigned char Write;        //以多字节形式写数据
  unsigned char Check;        //检测节点是否在线，同时返回固件信息
  unsigned char SetBaudRate;  //设置节点波特率
  unsigned char Excute;       //执行固件
  //节点返回状态
  unsigned char CmdSuccess;   //命令执行成功
  unsigned char CmdFaild;     //命令执行失败
} Boot_CMD_LIST;

unsigned short int CRCcalc16 (unsigned char *data,unsigned short int len);
u8 CAN_IAP(void);
extern Boot_CMD_LIST cmd_list;
extern bootloader_data Bootloader_data;
#endif /* CAN_BOOTLOADER_INC_CAN_BOOTLOADER_H_ */
