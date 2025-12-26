#include "main.h"

// ??????(???)
typedef enum {
    MODE_NONE,         // ???
    MODE_ENCODER,      // ?????
    VOLTAGE_JUMP,      // ??????
    MODE_BLUETOOTH     // ????
} OperationMode;

// ???:??????(??)
#define TEMP_INTERVAL 2000

int main(void)
{ 
    // 1. ?????
    SystemInit();
    OLED_Init();           
    ds18b20_init();        
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  
    INA226_Init();         
    Encoder_Init();        
    TLC5615_GPIO_Init();   
    Serial_Init();         
    SerialNumParser_Init();

    // 2. ?????
    float time = 10.00;
    float Tem = 0.0;                      // ???
    char str_Tem[12] = {0};                // ???????
    char str_EncoderVol[12] = {0};         // ??????????
    char str_time[12] = {0};               // ?????????
    int16_t Encoder_Value = 0;           // ??????
    uint16_t INA226_VolData = 0;           // INA226??????
    uint8_t jump_step = 0;                 // ??????
    uint32_t jump_tick = 0;                // ??????
    
    float INA226_Vol = 0.0;               // INA226???(V)
    float INA226_Cur = 0.0;               // INA226???(A)
    float INA226_Pow = 0.0;               // INA226???(W)
    char str_INA226_Vol[12] = {0};         // INA226???????
    char str_INA226_Cur[12] = {0};         // INA226???????
    char str_INA226_Pow[12] = {0};         // INA226???????
    char str_BluetoothData[12] = {0};      // ?????????
    float Bluetooth_Data = 2.5;           // ??????
    uint32_t Time_Count = 2000;            // ?????
    OperationMode Current_Mode = MODE_NONE;// ??????
		char str_Value[12]={0};
		
    // 3. ???(????????=60????)
    //while(Key_GetNum() == 1 && Tem <= 60) 
		while(Tem<=60)
    { 
        SerialNumParser_Process();

        /********** ??1:?????(?????) **********/
        if(Encoder_Flag == 1)
        {
            Current_Mode = MODE_ENCODER;  // ????????
            OLED_Clear();
            
            // ??????????DA
            Encoder_Value = Encoder_Get_Count()/4;
            TLC5615_Send12BitData(Encoder_Value); 
            
            // ?????:TL5615???3.21V,12????(4096),???2048
            snprintf(str_EncoderVol, sizeof(str_EncoderVol), "%.2fV", (float)Encoder_Value * 2.53*3.7 /2048);
            OLED_ShowString(1, 1, str_EncoderVol); // ???????
            snprintf(str_Value, sizeof(str_Value), "%d", Encoder_Value);
            OLED_ShowString(1, 7, str_Value); // ???????
            Encoder_Flag = 0; // ?????????
        }
        /********** ??2:?????? **********/
        else if(Bluetooth_Flag == 1)
        {			
            OLED_Clear();
            Current_Mode = MODE_BLUETOOTH; // ???????
            Bluetooth_Data = SerialNumParser_GetFinalNum();
            
            if(Bluetooth_Data != 74.0f)
            {
                TLC5615_Send12BitData(Bluetooth_Data * 2048/2.53/3.7);
                snprintf(str_BluetoothData, sizeof(str_BluetoothData), "%.2f", Bluetooth_Data);
                OLED_ShowString(1, 1, str_BluetoothData); // ??????
                Bluetooth_Flag = 0; // ????????
            }
            else
            {
                Current_Mode = VOLTAGE_JUMP; // ?????????
                Bluetooth_Flag = 0;
            }
        }
        /********** ??3:???????? **********/
        else
        {
            if(Current_Mode == MODE_ENCODER)
            {
                //TLC5615_Send12BitData(Encoder_Value);
                snprintf(str_EncoderVol, sizeof(str_EncoderVol), "%.2fV", (float)Encoder_Value * 2.53*2.7 /2048);
                OLED_ShowString(1, 1, str_EncoderVol);
            }
            else if(Current_Mode == MODE_BLUETOOTH)
            {
                //TLC5615_Send12BitData(Bluetooth_Data * 512/2.52);
                snprintf(str_BluetoothData, sizeof(str_BluetoothData), "%.2f", Bluetooth_Data);
                OLED_ShowString(1, 1, str_BluetoothData);
            }
        }

        /********** ???????? **********/
        if(Current_Mode == VOLTAGE_JUMP)
        {
            //time = SerialNumParser_GetFinalNum();
						//Current_Mode = VOLTAGE_JUMP;
            OLED_ShowString(1, 1, "JUMP");
            snprintf(str_time, sizeof(str_time), "%.2f", time);
            OLED_ShowString(1, 7, str_time);

            if(jump_tick > 0)
            {
                jump_tick--;
            }
            else
            {
                switch(jump_step)
                {
                    case 0:;
                        TLC5615_Send12BitData(567);
												OLED_ShowString(1, 14, "3V");
                        jump_tick = time * 125;
                        jump_step++;
                        break;

                    case 1:
                        TLC5615_Send12BitData(767);
												OLED_ShowString(1, 14, "4V");
                        jump_tick = time * 125;
                        jump_step++;
                        break;

                    case 2:
                        TLC5615_Send12BitData(959);
												OLED_ShowString(1, 14, "5V");
                        jump_tick = time * 125;
                        jump_step = 0;
                        Current_Mode = MODE_NONE;   // ????,?????
                        break;
                }
            }
        }

        /********** ?????:????(2?) **********/
        if(Time_Count >= TEMP_INTERVAL)
        {
            Tem = ds18b20_get_temp();  
            snprintf(str_Tem, sizeof(str_Tem), "T:%.2f", Tem);
            OLED_ShowString(4, 1, str_Tem);
            Time_Count = 0;
            //Delay_ms(50); // ????????
        }

        /********** INA226??????? **********/
        // ????
        INA226_VolData = INA226_GetVoltage(WRITE_ADDR);
        INA226_Vol = (float)INA226_VolData * 0.00125f;
        snprintf(str_INA226_Vol, sizeof(str_INA226_Vol), "V:%.2fV", INA226_Vol);
        OLED_ShowString(2, 1, str_INA226_Vol);

        // ????
        INA226_Cur = INA226_ReadCurrent_A(WRITE_ADDR);
        snprintf(str_INA226_Cur, sizeof(str_INA226_Cur), "I:%.2fA", INA226_Cur);
        OLED_ShowString(3, 1, str_INA226_Cur);

        // ???????
        INA226_Pow = INA226_Vol * INA226_Cur;
        snprintf(str_INA226_Pow, sizeof(str_INA226_Pow), "P:%.2fW", INA226_Pow);
        OLED_ShowString(4, 8, str_INA226_Pow); // ??????4??8?

        /********** ???? **********/
        Time_Count++;
        OLED_ShowString(4, 1, str_Tem);
        
        // ??????
        if(Time_Count > 10000)
        {
            Time_Count = 0;
        }
        
        //Delay_us(500); // ??CPU???
    }	

    /********** ?????? **********/
    // ??????
    if(Tem > 60)
    {
        TLC5615_Send12BitData(50);
        OLED_ShowString(1, 1, "OVER TEMPERATURE");
    }	
}
