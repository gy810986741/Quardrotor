#ifndef _OLED12864_H
#define _OLED12864_H
#include "stm32f10x.h"

#define byte unsigned char  //自己加的
#define word unsigned int   //自己加的


void OLED_WrDat(byte data);
void OLED_WrCmd(byte cmd);
void OLED_Set_Pos(byte x, byte y);
void OLED_CLS(void);
void OLED_Init(void);
void OLED_CLS(void);

void OLED_P6x8Str(byte x,byte y,byte *ch);
void OLED_P8x16Str(byte x,byte y,byte *ch);
void OLED_P14x16Str(byte x,byte y,byte *ch);
void OLED_ShowNum(u16 x,u16 y,s32 num,u8 len);

void OLED_Print(byte x, byte y, byte *ch);
void OLED_PutPixel(byte x,byte y);
void OLED_Rectangle(byte x1,byte y1,byte x2,byte y2,byte gif);
void Draw_LQLogo(void);
void Draw_LibLogo(void);
void Draw_BMP(byte x0,byte y0,byte x1,byte y1,byte *bmp);
void OLED_Fill(byte dat);
void OLED_Fill_Select(byte X1,byte X2,byte Y1,byte Y2,byte bmp_data);
void OLED_draw_CCDbmp_pixel(unsigned char X,unsigned char Y,unsigned char *map,
								unsigned char Pix_x,unsigned char Pix_y,unsigned char hhh);
void OLED_image_Diplay(void);
void OLED_draw_bmp_pixel2(unsigned char X,unsigned char Y,unsigned char *map, unsigned char Pix_x,unsigned char Pix_y);
void OLED_draw_bmp_pixel(unsigned char X,unsigned char Y,unsigned char *map, unsigned char Pix_x,unsigned char Pix_y);

void OLED_write_4_num(unsigned char X,unsigned char Y,signed int s);
void OLED_write_3_num(unsigned char X,unsigned char Y,signed int s);
void OLED_write_2_num(unsigned char X,unsigned char Y,signed int s);
void OLED_write_1_num(unsigned char X,unsigned char Y,signed int s);
void OLED_write_3_num_unsigned(unsigned char X,unsigned char Y,signed int s);
void OLED_write_4_num_unsigned(unsigned char X,unsigned char Y,signed int s);
void OLED_write_5_num_unsigned(unsigned char X,unsigned char Y,signed int s);


#endif

