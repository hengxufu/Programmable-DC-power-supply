#include "key.h"
#include "Delay.h"

u8 FLAG = 0;  //FLAG这个变量为1时，证明有按钮正在被按下

void KEY_4x4_Init(void)     //键盘IO口配置及初始化
{
 	GPIO_InitTypeDef GPIO_InitStructure;       	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);      
	//使用GPIOA的0,1,2,3引脚为行 R1~R4对应矩形按钮的5,6,7,8引脚
	GPIO_InitStructure.GPIO_Pin  = KEY_HANG;  //行  0123
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     //使用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,KEY_HANG);   //令GPIO的0,1,2,3引脚输出为1
	/********************************************************************/
	GPIO_InitStructure.GPIO_Pin  = lie1|lie2|lie3|lie4;         
	//使用GPIOA的4,5,6,7引脚为列  C1~C4对应矩形按钮的4,3,2,1引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;        //使用上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,lie1|lie2|lie3|lie4);	//令GPIO的4,5,6,7引脚输出为1	
}
//端口配置后，GPIOA的所有端口输出均为高电平
void KEY_Scan(u8 *key) 
{	 	
	GPIO_Write(GPIOA,0x00fe); //第一行  0000 0000 1111 1110   低四位为行   高四位为列
	/*令第一行输出为低电平，这时再判断4个列的输入状态的值，
	在按键没有被按下时，四列输出都是1，如果其中一列变为1，则可以判断S1按钮被按下*/
	if((lie1_Input==0)||(lie2_Input==0)||(lie3_Input==0)||(lie4_Input==0))   
	{
		Delay_ms(10);  //去抖动 
		
		if(lie1_Input==0)   //如果第一列被按下
		{
			FLAG = 1;    //按钮判断变量
			*key = 1;   //输出key的值为1传递到主函数，当值为x时，则说明按钮Sx被按下
				/*在变量前增加*，为c语言中的指针操作，使用指针进行地址传递，在子函数中修改的值在主函数中可以利用地址读取，
				而不需要利用子函数返回值，然后主函数在增加一个变量进行接收*/
			while(!GPIO_ReadInputDataBit(GPIOA,lie1));   
				//当按钮处于被按下的状态的时候，程序一直卡在循环读取按钮的状态，避免多按钮同时按下时读取错误
		}
		else if(lie2_Input==0)
		{
			FLAG = 1;
			*key = 2;
			while(!GPIO_ReadInputDataBit(GPIOA,lie2));
		}
		else if(lie3_Input==0)
		{
			FLAG = 1;
			*key = 3;
			while(!GPIO_ReadInputDataBit(GPIOA,lie3));
		}
		else if(lie4_Input==0)
		{
	    	FLAG = 1;
			*key = 4;
			while(!GPIO_ReadInputDataBit(GPIOA,lie4));
		}
		else  //如果第一行四列中没有按钮被按下
		{
			FLAG = 0;
			GPIO_Write(GPIOA,0x00ff);
		}
	}//第一行判断完成，这是我们判断第二行
	GPIO_Write(GPIOA,0x00fd);    //第二行         
	if((lie1_Input==0)||(lie2_Input==0)||(lie3_Input==0)||(lie4_Input==0))
	{
		Delay_ms(10);//去抖动 
		if(lie1_Input==0)
		{
			FLAG = 1;
			*key = 5;
			while(!GPIO_ReadInputDataBit(GPIOA,lie1));
		}
		else if(lie2_Input==0)
		{
	     	FLAG = 1;
			*key = 6;
			while(!GPIO_ReadInputDataBit(GPIOA,lie2));
		}
		else if(lie3_Input==0)
		{
	    	FLAG = 1;
			*key = 7;
			while(!GPIO_ReadInputDataBit(GPIOA,lie3));
		}
		else if(lie4_Input==0)
		{	 
	    	FLAG = 1;
			*key = 8;
			while(!GPIO_ReadInputDataBit(GPIOA,lie4));
		}
		else 
		{
			FLAG = 0;
			GPIO_Write(GPIOA,0x00ff);
		}
	}
	GPIO_Write(GPIOA,0x00fb);//第三行
	if((lie1_Input==0)||(lie2_Input==0)||(lie3_Input==0)||(lie4_Input==0))
	{
		Delay_ms(10);//去抖动 
		if(lie1_Input==0)
		{
			FLAG = 1;
			*key = 9;
			while(!GPIO_ReadInputDataBit(GPIOA,lie1));
		}
		else if(lie2_Input==0)
		{
	     	FLAG = 1;
			*key = 10;
			while(!GPIO_ReadInputDataBit(GPIOA,lie2));
		}
		else if(lie3_Input==0)
		{
	    	FLAG = 1;
			*key = 11;
			while(!GPIO_ReadInputDataBit(GPIOA,lie3));
		}
		else if(lie4_Input==0)
		{
	    	FLAG = 1;
			*key = 12;
			while(!GPIO_ReadInputDataBit(GPIOA,lie4));
		}
		else 
		{
			FLAG = 0;
			GPIO_Write(GPIOA,0x00ff);
		}
	}
	GPIO_Write(GPIOA,0x00f7);//第四行
	if((lie1_Input==0)||(lie2_Input==0)||(lie3_Input==0)||(lie4_Input==0))
	{
		Delay_ms(10);//去抖动 
		if(lie1_Input==0)
		{
			FLAG = 1;
			*key = 13;
			while(!GPIO_ReadInputDataBit(GPIOA,lie1));
		}
		else if(lie2_Input==0)
		{
	     	FLAG = 1;
			*key = 14;
			while(!GPIO_ReadInputDataBit(GPIOA,lie2));
		}
		else if(lie3_Input==0)
		{
	    	FLAG = 1;
			*key = 15;
			while(!GPIO_ReadInputDataBit(GPIOA,lie3));
		}
		else if(lie4_Input==0)
		{
	    	FLAG = 1;
			*key = 16;
			while(!GPIO_ReadInputDataBit(GPIOA,lie4));
		}
		else 
		{
			FLAG = 0;
			GPIO_Write(GPIOA,0x00ff);
		}
	}
	
}





