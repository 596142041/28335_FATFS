/*
 * can_bootloader.c
 *
 *  Created on: 2016年12月11日
 *      Author: lpr
 */
#include "can_bootloader.h"
bootloader_data Bootloader_data;
FIL fp;
FATFS fatfs;
unsigned char read_temp[66];
UINT read_cnt = 0;
FSIZE_t file_size = 0;
UINT calc_temp = 0;
UINT crc_value = 0x0000;
int send_cnt;//表示CAN总线发送64字节次数;
 Boot_CMD_LIST cmd_list =
 {
  .Erase = 0x00,      //擦除APP区域数据
  .WriteInfo = 0x01,  //设置多字节写数据相关参数（写起始地址，数据量）
  .Write = 0x02,      //以多字节形式写数据
  .Check = 0x03,      //检测节点是否在线，同时返回固件信息
  .SetBaudRate = 0x04,//设置节点波特率
  .Excute = 0x05,     //执行固件
  .CmdSuccess = 0x08, //命令执行成功
  .CmdFaild = 0x09,   //命令执行失败
 };
/*******************************************
 *函数名:unsigned short int CRCcalc16 (unsigned char *data,unsigned short int len)
 *输入参数:unsigned char *data:CRC校验数据,unsigned short int len:CRC校验长度
 *返回值:unsigned short int crc_res:CRC校验结果,高位在前
 *备注:该CRC校验采用的是Modbus的CRC校验方式;
 *******************************************/
unsigned short int CRCcalc16 (unsigned char *data,unsigned short int len)
{

	int i;
	unsigned short int crc_res =  0xFFFF;
	while(len--)
	{
		crc_res^=*data++;
		for(i = 0;i < 8;i++)
		{
			if(crc_res&0x01)
			{
				crc_res = (crc_res>>1)^0xa001;
			}
			else
			{
				crc_res = (crc_res>>1);
			}
		}
	}
	return crc_res;
}
/*********************************************
 *函数名:u8 File_send(void);
 *输入参数:无;
 *返回值:u8 res:成功返回0;
 *
 ********************************************/
u8 CAN_IAP(void)
{
	int i = 0;//计数变量
	unsigned char cnt = 0;
	unsigned long int CAN_BL_temp;//当前设备运行的固件缓存
	FRESULT res;
	Bootloader_data.ExtId.bit.reserve = 0;
	unsigned short int time_out = 0x00;
	for(i = 0; i  <8;i++)
	{
		can_tx_msg.CAN_Tx_msg_data.msg_byte.data[i] = 0;
		can_rx_msg.CAN_Rx_msg_data.msg_byte.data[i] = 0;
	}
	//--查询当前运行的程序是APP还是BOOT;
	Bootloader_data.DLC               = 0;
	Bootloader_data.ExtId.bit.cmd     = cmd_list.Check;
	Bootloader_data.ExtId.bit.addr    = 0x123;
	Bootloader_data.ExtId.bit.reserve = 0;
	Bootloader_data.IDE               = CAN_ID_EXT;
	can_tx_msg.DLC                    = Bootloader_data.DLC;
	can_tx_msg.IDE                    = Bootloader_data.IDE;
	can_tx_msg.ExtId.all              = Bootloader_data.ExtId.all;
	can_tx_msg.MBox_num               = 12;
	CAN_Tx_Msg(&can_tx_msg);
	while(can_rx_msg.upflag !=1)
	{
		time_out++;
		if(time_out%30000 == 0)
		{
			CAN_Tx_Msg(&can_tx_msg);
		}
		time_out%=65000;
	}
	can_rx_msg.upflag = 0;
	if(can_rx_msg.ExtId!=((Bootloader_data.ExtId.all&0xFFF0)|cmd_list.CmdSuccess))
	{

		return 12;//从机应答错误
	}
	if(can_rx_msg.CAN_Rx_msg_data.msg_Byte.byte4 == 0xAA)
	{
		for(i = 4;i<7;i++)
		{
			if(can_rx_msg.CAN_Rx_msg_data.msg_Byte.byte4 == 0xAA)
			{
				CAN_BL_temp = CAN_BL_APP;
			}
			else
			{
				CAN_BL_temp = 0x00;
			}
		}
	}
	else if(can_rx_msg.CAN_Rx_msg_data.msg_Byte.byte4 == 0x55)
	{
		for(i = 4;i<7;i++)
		{
			if(can_rx_msg.CAN_Rx_msg_data.msg_Byte.byte4 == 0x55)
			{
				CAN_BL_temp = CAN_BL_BOOT;
			}
			else
			{
				CAN_BL_temp = 0x00;
			}
		}
	}
	//如果是APP程序正在运行,则需要发送一次跳转指令,让芯片复位
	if(CAN_BL_temp == CAN_BL_APP)
	{
		Bootloader_data.DLC               = 4;
		Bootloader_data.ExtId.bit.cmd     = cmd_list.Excute;
		Bootloader_data.ExtId.bit.addr    = 0x123;
		Bootloader_data.ExtId.bit.reserve = 0;
		Bootloader_data.IDE               = CAN_ID_EXT;
		can_tx_msg.DLC                    = Bootloader_data.DLC;
		can_tx_msg.IDE                    = Bootloader_data.IDE;
		can_tx_msg.ExtId.all              = Bootloader_data.ExtId.all;
		can_tx_msg.MBox_num               = 12;
		for(i = 0;i<can_tx_msg.DLC;i++)
		{
			can_tx_msg.CAN_Tx_msg_data.msg_byte.data[i] = 0x55;
		}
		CAN_Tx_Msg(&can_tx_msg);
		//跳转之后注意延迟时间，可以加大一些,这延迟时间主要是因为芯片复位时间决定
		//500ms->250ms->100ms->50ms,最后选择50ms即可;
		delay_ms(50);
		//--查询当前运行的程序是APP还是BOOT;
		Bootloader_data.DLC                = 0;
		Bootloader_data.ExtId.bit.cmd      = cmd_list.Check;
		Bootloader_data.ExtId.bit.addr     = 0x123;
		Bootloader_data.ExtId.bit.reserve  = 0;
		Bootloader_data.IDE                = CAN_ID_EXT;
		can_tx_msg.DLC                     = Bootloader_data.DLC;
		can_tx_msg.IDE                     = Bootloader_data.IDE;
		can_tx_msg.ExtId.all               = Bootloader_data.ExtId.all;
		can_tx_msg.MBox_num                = 12;
		CAN_Tx_Msg(&can_tx_msg);
		while(can_rx_msg.upflag !=1)
		{
			CAN_Tx_Msg(&can_tx_msg);
			delay_us(1000);
		}
		can_rx_msg.upflag = 0;
		if(can_rx_msg.ExtId!=((Bootloader_data.ExtId.all&0xFFF0)|cmd_list.CmdSuccess))
		{

			return 12;//返回错误
		}

	}
	//挂载文件系统
	res = f_mount(&fatfs,"0:",1);
	if(res != FR_OK)
	{
		return res;
	}
	//打开文件app.bin
	res = f_open(&fp,"0:/update/app.bin",FA_READ);
	if(res != FR_OK)
	{
		return res;
	}
	//移动文件指针到文件头
	f_lseek(&fp,0);
	if(res != FR_OK)
	{
		f_close(&fp);
		return res;
	}
	//获取文件大小,准备发送文件擦除指令
	//文件大小采用4字节来表示,最带可以表示4Gb
	Bootloader_data.DLC               = 4;
	Bootloader_data.ExtId.bit.cmd     = cmd_list.Erase;
	Bootloader_data.ExtId.bit.addr    = 0x123;
	Bootloader_data.ExtId.bit.reserve = 0;
	Bootloader_data.IDE               = CAN_ID_EXT;
	Bootloader_data.data[0]           = (fp.obj.objsize&0xFF000000)>>24;
	Bootloader_data.data[1]           = (fp.obj.objsize&0xFF0000)>>16;
	Bootloader_data.data[2]           = (fp.obj.objsize&0xFF00)>>8;
	Bootloader_data.data[3]           = (fp.obj.objsize&0x00FF);
	can_tx_msg.DLC                    = Bootloader_data.DLC;
	can_tx_msg.IDE                    = Bootloader_data.IDE;
	can_tx_msg.ExtId.all              = Bootloader_data.ExtId.all;
	can_tx_msg.MBox_num               = 12;
	for(i = 0;i < Bootloader_data.DLC;i++)
	{
		can_tx_msg.CAN_Tx_msg_data.msg_byte.data[i] = Bootloader_data.data[i];
	}
	CAN_Tx_Msg(&can_tx_msg);
	while(can_rx_msg.upflag !=1)
	{
		;
	}
	can_rx_msg.upflag = 0;
	if(can_rx_msg.ExtId!=((Bootloader_data.ExtId.all&0xFFF0)|cmd_list.CmdSuccess))
	{
		f_close(&fp);
		return 1;
	}
	//-----第三步,准备读取数据,并将读取的数据发送给对方
	/********************************************
	 * 和以前的方案一样,每次都读取64字节,以便于STM32写入数据;
	 * 在发送数据包之前需要发送当前数据包的长度及其需要写入的偏移地址
	 * */
	while(file_size < f_size(&fp))
	{
		//读取数据
		res = f_read(&fp,read_temp,64,&read_cnt);
		if(res != FR_OK)
		{
			return res;
		}
		//进行crc计算,并进行赋值
	    crc_value             = CRCcalc16(read_temp,read_cnt);
	    read_temp[read_cnt]   = crc_value&0xFF;
	    read_temp[read_cnt+1] = (crc_value>>8)&0xFF;
	    //发送当前数据包的相关信息,数据包的数据长度,数据包的偏移地址
	    //包含8字节,0-3字节表示地址偏移
	    //4-7表示数据包的大小
		Bootloader_data.DLC               = 8;
		Bootloader_data.ExtId.bit.cmd     = cmd_list.WriteInfo;
		Bootloader_data.ExtId.bit.addr    = 0x123;
		Bootloader_data.ExtId.bit.reserve = 0;
		Bootloader_data.IDE               = CAN_ID_EXT;
		Bootloader_data.data[0]           = (file_size&0xFF000000)>>24;
		Bootloader_data.data[1]           = (file_size&0xFF0000)>>16;
		Bootloader_data.data[2]           = (file_size&0xFF00)>>8;
		Bootloader_data.data[3]           = (file_size&0x00FF);

		Bootloader_data.data[4]           = ((read_cnt+2)&0xFF000000)>>24;
		Bootloader_data.data[5]           = ((read_cnt+2)&0xFF0000)>>16;
		Bootloader_data.data[6]           = ((read_cnt+2)&0xFF00)>>8;
		Bootloader_data.data[7]           = ((read_cnt+2)&0x00FF);
		can_tx_msg.DLC                    = Bootloader_data.DLC;
	    can_tx_msg.IDE                    = Bootloader_data.IDE;
		can_tx_msg.ExtId.all              = Bootloader_data.ExtId.all;
		can_tx_msg.MBox_num               = 12;
		for(i = 0;i < Bootloader_data.DLC;i++)
		{
			can_tx_msg.CAN_Tx_msg_data.msg_byte.data[i] = Bootloader_data.data[i];
		}
		CAN_Tx_Msg(&can_tx_msg);
		while(can_rx_msg.upflag !=1)
		{
			;
		}
		can_rx_msg.upflag = 0;
		if(can_rx_msg.ExtId!=((Bootloader_data.ExtId.all&0xFFF0)|cmd_list.CmdSuccess))
		{
			f_close(&fp);
			return 2;
		}
		//--发送数据包
		//此处为何加2,因为最后两位是CRC的检验值
		while(cnt < read_cnt+2)
		{
			int temp;
			temp = read_cnt+2-cnt;
			if(temp>=8)
			{
				Bootloader_data.DLC               = 8;
				Bootloader_data.ExtId.bit.cmd     = cmd_list.Write;
				Bootloader_data.ExtId.bit.addr    = 0x123;
				Bootloader_data.ExtId.bit.reserve = 0;
				Bootloader_data.IDE               = CAN_ID_EXT;
				//--------------------------------------------------------
				can_tx_msg.DLC                    = Bootloader_data.DLC;
				can_tx_msg.IDE                    = Bootloader_data.IDE;
				can_tx_msg.ExtId.all              = Bootloader_data.ExtId.all;
				can_tx_msg.MBox_num               = 12;
				for(i = 0;i < Bootloader_data.DLC;i++)
				{
					can_tx_msg.CAN_Tx_msg_data.msg_byte.data[i] = read_temp[cnt];
					cnt++;
				}
			}
			else
			{
				Bootloader_data.DLC               = temp;
				Bootloader_data.ExtId.bit.cmd     = cmd_list.Write;
				Bootloader_data.ExtId.bit.addr    = 0x123;
				Bootloader_data.ExtId.bit.reserve = 0;
				Bootloader_data.IDE               = CAN_ID_EXT;
				//--------------------------------------------------------
				can_tx_msg.DLC                    = Bootloader_data.DLC;
				can_tx_msg.IDE                    = Bootloader_data.IDE;
				can_tx_msg.ExtId.all              = Bootloader_data.ExtId.all;
				can_tx_msg.MBox_num = 12;
				for(i = 0;i < Bootloader_data.DLC;i++)
				{
					can_tx_msg.CAN_Tx_msg_data.msg_byte.data[i] = read_temp[cnt];
					cnt++;
				}
			}
			CAN_Tx_Msg(&can_tx_msg);
		}
		//发送完成一包数据等待回应
		while(can_rx_msg.upflag !=1)
		{
			;
		}
		can_rx_msg.upflag = 0;
		//将缓存数组数据全部清零
		for(i = 0;i <read_cnt+2;i++)
		{
			read_temp[cnt] = 0x00;
		}
		if(can_rx_msg.ExtId!=((Bootloader_data.ExtId.all&0xFFF0)|cmd_list.CmdSuccess))
		{
			f_close(&fp);
			return 3;
		}
		file_size =file_size+read_cnt;
		//此处一定记得将cnt清零否则第二次发送数据时会有问题
		cnt = 0;
	}
	//--最后发送跳转命令
	//跳转指令是让程序从Boot跳转至APP区,执行APP程序
	Bootloader_data.DLC               = 4;
	Bootloader_data.ExtId.bit.cmd     = cmd_list.Excute;
	Bootloader_data.ExtId.bit.addr    = 0x123;
	Bootloader_data.ExtId.bit.reserve = 0;
	Bootloader_data.IDE               = CAN_ID_EXT;
	can_tx_msg.DLC                    = Bootloader_data.DLC;
	can_tx_msg.IDE                    = Bootloader_data.IDE;
	can_tx_msg.ExtId.all              = Bootloader_data.ExtId.all;
	can_tx_msg.MBox_num               = 12;
	for(i = 0;i < Bootloader_data.DLC;i++)
	{
		can_tx_msg.CAN_Tx_msg_data.msg_byte.data[i] =0xAA;
	}
	CAN_Tx_Msg(&can_tx_msg);
	f_close(&fp);
	return 0;
}
