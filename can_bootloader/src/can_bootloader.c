/*
 * can_bootloader.c
 *
 *  Created on: 2016��12��11��
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
int send_cnt;//��ʾCAN���߷���64�ֽڴ���;
 Boot_CMD_LIST cmd_list =
 {
  .Erase = 0x00,      //����APP��������
  .WriteInfo = 0x01,  //���ö��ֽ�д������ز�����д��ʼ��ַ����������
  .Write = 0x02,      //�Զ��ֽ���ʽд����
  .Check = 0x03,      //���ڵ��Ƿ����ߣ�ͬʱ���ع̼���Ϣ
  .SetBaudRate = 0x04,//���ýڵ㲨����
  .Excute = 0x05,     //ִ�й̼�
  .CmdSuccess = 0x08, //����ִ�гɹ�
  .CmdFaild = 0x09,   //����ִ��ʧ��
 };
/*******************************************
 *������:unsigned short int CRCcalc16 (unsigned char *data,unsigned short int len)
 *�������:unsigned char *data:CRCУ������,unsigned short int len:CRCУ�鳤��
 *����ֵ:unsigned short int crc_res:CRCУ����,��λ��ǰ
 *��ע:��CRCУ����õ���Modbus��CRCУ�鷽ʽ;
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
 *������:u8 File_send(void);
 *�������:��;
 *����ֵ:u8 res:�ɹ�����0;
 *
 ********************************************/
u8 CAN_IAP(void)
{
	int i = 0;//��������
	unsigned char cnt = 0;
	unsigned long int CAN_BL_temp;//��ǰ�豸���еĹ̼�����
	FRESULT res;
	Bootloader_data.ExtId.bit.reserve = 0;
	unsigned short int time_out = 0x00;
	for(i = 0; i  <8;i++)
	{
		can_tx_msg.CAN_Tx_msg_data.msg_byte.data[i] = 0;
		can_rx_msg.CAN_Rx_msg_data.msg_byte.data[i] = 0;
	}
	//--��ѯ��ǰ���еĳ�����APP����BOOT;
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

		return 12;//�ӻ�Ӧ�����
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
	//�����APP������������,����Ҫ����һ����תָ��,��оƬ��λ
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
		//��ת֮��ע���ӳ�ʱ�䣬���ԼӴ�һЩ,���ӳ�ʱ����Ҫ����ΪоƬ��λʱ�����
		//500ms->250ms->100ms->50ms,���ѡ��50ms����;
		delay_ms(50);
		//--��ѯ��ǰ���еĳ�����APP����BOOT;
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

			return 12;//���ش���
		}

	}
	//�����ļ�ϵͳ
	res = f_mount(&fatfs,"0:",1);
	if(res != FR_OK)
	{
		return res;
	}
	//���ļ�app.bin
	res = f_open(&fp,"0:/update/app.bin",FA_READ);
	if(res != FR_OK)
	{
		return res;
	}
	//�ƶ��ļ�ָ�뵽�ļ�ͷ
	f_lseek(&fp,0);
	if(res != FR_OK)
	{
		f_close(&fp);
		return res;
	}
	//��ȡ�ļ���С,׼�������ļ�����ָ��
	//�ļ���С����4�ֽ�����ʾ,������Ա�ʾ4Gb
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
	//-----������,׼����ȡ����,������ȡ�����ݷ��͸��Է�
	/********************************************
	 * ����ǰ�ķ���һ��,ÿ�ζ���ȡ64�ֽ�,�Ա���STM32д������;
	 * �ڷ������ݰ�֮ǰ��Ҫ���͵�ǰ���ݰ��ĳ��ȼ�����Ҫд���ƫ�Ƶ�ַ
	 * */
	while(file_size < f_size(&fp))
	{
		//��ȡ����
		res = f_read(&fp,read_temp,64,&read_cnt);
		if(res != FR_OK)
		{
			return res;
		}
		//����crc����,�����и�ֵ
	    crc_value             = CRCcalc16(read_temp,read_cnt);
	    read_temp[read_cnt]   = crc_value&0xFF;
	    read_temp[read_cnt+1] = (crc_value>>8)&0xFF;
	    //���͵�ǰ���ݰ��������Ϣ,���ݰ������ݳ���,���ݰ���ƫ�Ƶ�ַ
	    //����8�ֽ�,0-3�ֽڱ�ʾ��ַƫ��
	    //4-7��ʾ���ݰ��Ĵ�С
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
		//--�������ݰ�
		//�˴�Ϊ�μ�2,��Ϊ�����λ��CRC�ļ���ֵ
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
		//�������һ�����ݵȴ���Ӧ
		while(can_rx_msg.upflag !=1)
		{
			;
		}
		can_rx_msg.upflag = 0;
		//��������������ȫ������
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
		//�˴�һ���ǵý�cnt�������ڶ��η�������ʱ��������
		cnt = 0;
	}
	//--�������ת����
	//��תָ�����ó����Boot��ת��APP��,ִ��APP����
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
