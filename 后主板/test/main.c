/*------------------------------------------------------------*- 
  红外收发.C 
  ------------------------------------------------------------ 
  遥控器测试 
-*------------------------------------------------------------*/  
  
#include <STC15.H>  
  
// --- 红外接收一体化输出口 ----------------------------------  
sbit IR_Out = P3^5;  
  
bit START_Flag = 0;  
bit BOOT_REPEATING_CODE_Flag = 0;  
unsigned char DATA[4] = {0};  
bdata unsigned char TEMP_BIT;  
  
sbit B0 = TEMP_BIT^0;  
sbit B1 = TEMP_BIT^1;  
sbit B2 = TEMP_BIT^2;  
sbit B3 = TEMP_BIT^3;  
sbit B4 = TEMP_BIT^4;  
sbit B5 = TEMP_BIT^5;  
sbit B6 = TEMP_BIT^6;  
sbit B7 = TEMP_BIT^7;  
  
// --- 有无遥控信号判断函数 ----------------------------------  
bit START_Judge();  
  
// --- 连发码判断函数 ----------------------------------------  
bit BOOT_REPEATING_CODE_Judge();  
  
// --- "0"和"1"识别 ------------------------------------------  
bit H_L_LEVEL_Judge();  
  
// --- 串口初始化 --------------------------------------------  
void UART_Initial();  
  
void DELAY_Us(unsigned int Us)  
{  
    unsigned int x;  
    for(x = 0; x <= (Us/200-1); x++);  
}  
void DELAY_Ms(unsigned int Ms)  
{  
    unsigned int x,y;  
    for(x = 0; x <= (Ms-1); x++)  
    {  
        for(y = 0; y <= 120; y++);  
    }  
}  
  
void main()  
{  
    unsigned char i;  
    UART_Initial();  
    IR_Out = 1;  
    while(1)  
    {         
        START_Flag = START_Judge();  
        BOOT_REPEATING_CODE_Flag = BOOT_REPEATING_CODE_Judge();  
        if ( START_Flag && !BOOT_REPEATING_CODE_Flag )  
        {             
            for(i =0;i <4; i++)  
            {                 
                B0 = H_L_LEVEL_Judge();  
                B1 = H_L_LEVEL_Judge();  
                B2 = H_L_LEVEL_Judge();  
                B3 = H_L_LEVEL_Judge();  
                B4 = H_L_LEVEL_Judge();  
                B5 = H_L_LEVEL_Judge();  
                B6 = H_L_LEVEL_Judge();  
                B7 = H_L_LEVEL_Judge();                               
                DATA[i] = TEMP_BIT;  
            }  
            for(i =0;i <4; i++)  
            {  
                SBUF = DATA[i];  
                while( TI == 0 );  
                TI = 0;  
            }  
        }  
				DELAY_Ms(500);
    }  
}  
  
void UART_Initial()  
{  
    SCON = 0x50;            // SCON: 模式 1, 8-bit UART, 使能接收  
  
    TMOD |= 0x20;           // TMOD: timer 1, mode 2, 8-bit reload  
  
    TH1 = 0xFD;             // TH1: reload value for 9600 baud @  
                            // 11.0592MHz   
    TR1 = 1;                // TR1: timer 1 run  
  
    EA = 0;                 // 关闭总中断  
    ES = 0;                 // 关闭串口中断  
}  
      
  
bit START_Judge()  
{  
    bit TEMP_Flag = 1;  
    unsigned char i = 0;  
  
    //在正常无遥控信号时，一体化红外接收头输出是高电平，程序一直在循环。  
    while ( IR_Out == 1);  
  
    //重复10次，目的是检测在6876~8352微秒内如果出现高电平就退出解码程序  
    for(i =0;i <9; i++)  
    {  
        DELAY_Us(800);      // 测试实际延时约为764~928us  
        if ( IR_Out == 1 )  
        {  
            TEMP_Flag = 0;  
            break;  
        }  
    }  
      
    return TEMP_Flag;  
}  
  
bit BOOT_REPEATING_CODE_Judge()  
{  
    bit TEMP_Flag = 1;  
    while( IR_Out == 0 ) ;  // 等待高电平避开9毫秒低电平引导脉冲  
  
    DELAY_Ms(1);            // 测试实际延时约为1.007ms   
    DELAY_Ms(1);            // 测试实际延时约为1.007ms   
    DELAY_Us(200);          // 0.086ms  
    DELAY_Us(200);          // 0.086ms    
    DELAY_Us(200);          // 0.086ms  
                            // 共计2.272ms      
  
    if( IR_Out == 0 )  
    {  
        TEMP_Flag = 1;      // 是连发码  
    }  
    else  
    {  
        TEMP_Flag = 0;      // 不是连发码，而是引导码  
    }  
    return TEMP_Flag;  
}  
bit H_L_LEVEL_Judge()  
{  
    while( IR_Out == 0 );   // 等待地址码第一位的高电平信号  
    DELAY_Us(800);          // 测试实际延时约为764~928us  
    if ( IR_Out == 1)  
    {  
        DELAY_Ms(1);        // 测试实际延时约为1.007ms   
        return 1;  
    }  
    else  
    {  
        return 0;  
    }  
}