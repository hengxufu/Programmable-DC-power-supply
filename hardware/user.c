#include "user.h"

const int password[]={1,0,0,8,6,0,1,1};  //设定8位数密码
u8 key=0;   //用于按键值判断
uint8_t  outpassword[16] ;   //用于记录并显示输入的密码——输出密码
uint8_t k=0;    //k为临时变量，用于传送按键输入值到输出密码
uint8_t i=0;	//临时变量，用于记录输出密码数组的值
uint8_t  pdz=1;  //确认密码判断值

//参数对应     输入文字个数，行数，起始列数，文字起始编号
void showChinese(uint8_t a,uint8_t b,uint8_t c,uint8_t d)   //汉字连续打印输出
{
	while(a--)
	{
		OLED_ShowChinese(b,c,d);
		c++;
		d++;
	}
}



void pd(void)  //用于确认键函数判断
{
	u8 cs=0;
	for(int a=0;a<i;a++)
	{
		if(outpassword [a]==password[a])
		{
			cs++;
		}
	}
	if(cs==8)
	{
		pdz=0;
	}
	else
	{
		OLED_delete (3);
		showChinese (6,3,1,15);
		Delay_s (3);
		OLED_delete(3);
		i=0;
	}
}

void  OLED_key(void)
{
	KEY_Scan (&key);
	if(FLAG == 1)    //按键按下
	{
		FLAG = 0;
		if(key==1)  //按下S1，即输入1	
		{
			k=1;
			outpassword [i]=k;
			i++;
		}	
		else if(key==2)  //按下S2，即输入2
		{
			k=2;
			outpassword [i]=k;
			i++;	
		}
		else if(key==3)		//按下S3，即输入3
		{
			k=3;
			outpassword [i]=k;
			i++;
		}
		else if(key==4)   //按下S4，即输入4
		{
			k=4;
			outpassword [i]=k;
			i++;
		}
		else if(key==5)  	//按下S5，即输入5
		{
			k=5;
			outpassword [i]=k;
			i++;
		}
		else if(key==6)		//按下S6，即输入6
		{
			k=6;
			outpassword [i]=k;
			i++;
		}
		else if(key==7)		//按下S7，即输入7
		{
			k=7;
			outpassword [i]=k;
			i++;
		}
		else if(key==8)		//按下S8，即输入8
		{
			k=8;
			outpassword [i]=k;
			i++;
		}
		else if(key==9)		//按下S9，即输入9
		{
			k=9;
			outpassword [i]=k;
			i++;
		}
		else if(key==10)		//按下S10，即输入0
		{
			k=0;
			outpassword [i]=k;
			i++;
		}
		else if(key==11)    //删除键，按下后删除一个值
		{
			OLED_delete (3);
			i--;
			k=0;
			outpassword [i]=k;
		}
		else if(key==12)    //密码确认键
		{
			pd();
		}
		
	}
}
	

void OLED_print(void)		//遍历打印屏幕显示输出密码
{
	for(u8 a=0;a<i;a++)
	{ 
		OLED_ShowNum (3,a+1,outpassword[a],1);
	}
}


