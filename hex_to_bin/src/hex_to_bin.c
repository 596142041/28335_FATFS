/*
 * hex_to_bin.c
 *
 *  Created on: 2017Äê1ÔÂ17ÈÕ
 *      Author: lpr
 */
#include "hex_to_bin.h"
extern FIL fp;
extern FATFS fatfs;
extern UINT read_cnt;
extern FSIZE_t file_size;
PACK_INFO pack_info;
unsigned char read_buf[128];
unsigned char bin_buf[128];
void Data_clear(unsigned char *data,unsigned long int len)
{
     unsigned long int i;
     for(i = 0;i < len;i++)
     {
         *data = 0;
         data++;
     }
}
unsigned char convertion(unsigned char *hex_data)
{
    unsigned char bin_data = 0xFF;
    if(*hex_data >= '0'&&*hex_data <= '9')
    {

        bin_data = *hex_data-NUM_OFFSET;
    }
    else if(*hex_data >= 'A'&&*hex_data <= 'F')
    {
        bin_data = *hex_data-CHAR_OFFSET;
    }
    return bin_data;
}
void hex_to_bin(unsigned char *hex_src,unsigned char *bin_dst,unsigned char len)
{
    unsigned char i;
    for(i = 0;i <len;i++)
    {
        *bin_dst = convertion(hex_src);
        bin_dst++;
        hex_src++;
    }
}
u8 Hex_to_Bin(void)
{


	return 0xFF;
}
