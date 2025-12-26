#ifndef __OLED_H
#define __OLED_H

void OLED_Init(void);   //初始化
void OLED_Clear(void);  //清屏
void OLED_delete(u8 k); //清除显示屏某一行的显示，参数为删除的行
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);   //显示单个字符
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);  //显示字符串
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
//显示十进制数字
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
//显示带有符号的十进制数字
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
//显示十六进制的数字
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
//显示二进制
void OLED_ShowChinese(uint8_t Line,uint8_t Column,uint8_t num);
//显示单个汉字  参数(行数，列数，打印函数在Font_Chinese[][32]数组中的第n位)
void OLED_BMP(int i);
//显示图片

#endif
