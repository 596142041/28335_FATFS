/*
 * can_driver.c
 *
 *  Created on: 2016年12月15日
 *      Author: lpr
 */
#include "can_driver.h"
/**********************************
 * 函数名:void CAN_boot_Rx_Config(void)
 * 输入参数:无;
 * 返回值:无;
 * 备注:需要初始化CAN总线的的接收邮箱,使用邮箱31,目前就配置一个;
 *
 */
void CAN_boot_Rx_Config(void)
{
	struct ECAN_REGS ECanaShadow;
	ECanaShadow.CANME.all = ECanaRegs.CANME.all;
	ECanaShadow.CANME.bit.ME31 = 0;//不使能邮箱31
	ECanaRegs.CANME.all = ECanaShadow.CANME.all;
	/*----------以下代码是配置接受邮箱的相关代码------------*/
	/*
	LAM[28:0]
	这些位启用一个进入消息的任意标识符位的屏蔽。
	1 针对接受到的标识符的相应位， 接受一个 0 或 1（ 无关） 。
	0 接收到的标识符位值必须与 MSGID 寄存器的相应标识符位相匹配。
	*/
	/*
	LAMI 本地接受屏蔽标识符扩展位
	1 可以接收标准和扩展帧。 在扩展帧的情况下， 标识符的所有 29 位被存储在邮箱中， 本地接受屏蔽寄
	存器的所有 29 位被过滤器使用。 在一个标准帧的情况下， 只有标识符的头 11 个位（ 28 至 18 位）
	和本地接受屏蔽被使用。
	0 存储在邮箱中的标识符扩展位决定了哪些消息应该被接收到
	*/
	//邮箱31相关配置
	ECanaMboxes.MBOX31.MSGCTRL.bit.DLC = 8;//配置数据长度，应该是没意义的;
	ECanaMboxes.MBOX31.MSGID.bit.AME =1;//屏蔽使能位,
	ECanaMboxes.MBOX31.MSGID.bit.IDE = CAN_ID_EXT;
	ECanaMboxes.MBOX31.MSGID.all = 0x000001238;//设置接收消息的有效ID
	ECanaLAMRegs.LAM31.bit.LAM_H = 0x0000;//
	ECanaLAMRegs.LAM31.bit.LAM_L = 0x0000;
	ECanaLAMRegs.LAM31.bit.LAMI = 1;
	ECanaRegs.CANRMP.all      = 0xFFFFFFFF;
	ECanaShadow.CANMD.all     = ECanaRegs.CANMD.all;
	ECanaShadow.CANMD.bit.MD31 = 1;
	ECanaRegs.CANMD.all       = ECanaShadow.CANMD.all;
	ECanaShadow.CANME.all       = ECanaRegs.CANME.all;
	ECanaShadow.CANME.bit.ME31  = 1;//使能邮箱1
	ECanaRegs.CANME.all         = ECanaShadow.CANME.all;
}
