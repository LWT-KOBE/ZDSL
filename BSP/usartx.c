#include "usartx.h"
#include "Odrive.h"

uint8_t flag;
SEND_DATA Send_Data;
RECEIVE_DATA Receive_Data;
#define 	RECV_BUF_SIZE 	400

#define 	SEND_BUF_SIZE 	200
//定义存储接收字符串的数组
char g_usart1_recv_buf[RECV_BUF_SIZE] = {0};


//定义存储发送字符串的数组
char g_usart1_send_buf[SEND_BUF_SIZE] = {0};

//定义接收字符的计数
int g_usart1_recv_cnt = 0;


//定义存储接收字符串的数组
char g_usart3_recv_buf[RECV_BUF_SIZE] = {0};


//定义存储发送字符串的数组
char g_usart3_send_buf[SEND_BUF_SIZE] = {0};

//定义接收字符的计数
int g_usart3_recv_cnt = 0;

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
///重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到串口 */
		USART_SendData(USART1, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
		/* 等待串口输入数据 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART1);
}
#endif



/***************************************************串口初始化部分************************************************************/

/**************************************************************************
Function: Serial port 1 initialization
Input   : none
Output  : none
函数功能：串口1初始化
入口参数：无
返 回 值：无
**************************************************************************/
void uart1_init(u32 bound)
{  	 
//	GPIO_InitTypeDef GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	 //Enable the gpio clock //使能GPIO时钟
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); //Enable the Usart clock //使能USART时钟

//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_USART1);	
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7 ,GPIO_AF_USART1);	 
//	
//	// 配置PB6为复用功能（USART1_TX）
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);
//    
//    // 配置PB7为复用功能（USART1_RX）
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
//	
//  //UsartNVIC configuration //UsartNVIC配置
//	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//	//Preempt priority //抢占优先级
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;
//	//Subpriority //子优先级
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		
//	//Enable the IRQ channel //IRQ通道使能
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
//  //Initialize the VIC register with the specified parameters 
//	//根据指定的参数初始化VIC寄存器	
//	NVIC_Init(&NVIC_InitStructure);	
//	
//  //USART Initialization Settings 初始化设置
//	USART_InitStructure.USART_BaudRate = bound; //Port rate //串口波特率
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //The word length is 8 bit data format //字长为8位数据格式
//	USART_InitStructure.USART_StopBits = USART_StopBits_1; //A stop bit //一个停止位
//	USART_InitStructure.USART_Parity = USART_Parity_No; //Prosaic parity bits //无奇偶校验位
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //No hardware data flow control //无硬件数据流控制
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//Sending and receiving mode //收发模式
//	USART_Init(USART1, &USART_InitStructure); //Initialize serial port 1 //初始化串口1
//	
//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //Open the serial port to accept interrupts //开启串口接受中断
//	USART_Cmd(USART1, ENABLE);                     //Enable serial port 1 //使能串口1


	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟

	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1

	//USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10

	//USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口1

	USART_Cmd(USART1, ENABLE);  //使能串口1 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、


	
}


static unsigned char TxBuffer[256];
static unsigned char TxCounter=0;
static unsigned char count=0; 
/**************************************************************************
Function: Serial port 2 initialization
Input   : none
Output  : none
函数功能：串口2初始化
入口参数：无
返回  值：无
**************************************************************************/
void uart2_init(u32 bound)
{  	 
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	 //Enable the gpio clock  //使能GPIO时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 	//Enable the Usart clock //使能USART时钟
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);	//TX
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);	//RX 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;            //输出模式
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;          //推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;       //高速50MHZ
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;            //上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);  		          //初始化
	
	//UsartNVIC configuration //UsartNVIC配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	//Preempt priority //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;
	//Preempt priority //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		
	//Enable the IRQ channel //IRQ通道使能	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	//Initialize the VIC register with the specified parameters 
	//根据指定的参数初始化VIC寄存器		
	NVIC_Init(&NVIC_InitStructure);
	
	//USART Initialization Settings 初始化设置
	USART_InitStructure.USART_BaudRate = bound; //Port rate //串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //The word length is 8 bit data format //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //A stop bit //一个停止
	USART_InitStructure.USART_Parity = USART_Parity_No; //Prosaic parity bits //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //No hardware data flow control //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//Sending and receiving mode //收发模式
	USART_Init(USART2, &USART_InitStructure);      //Initialize serial port 2 //初始化串口2
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //Open the serial port to accept interrupts //开启串口接受中断
	USART_Cmd(USART2, ENABLE);                     //Enable serial port 2 //使能串口2 
}

/**************************************************************************
Function: Serial port 3 initialization
Input   : none
Output  : none
函数功能：串口3初始化
入口参数：无
返回  值：无
**************************************************************************/
void uart3_init(u32 bound)
{  	 
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	 //Enable the gpio clock  //使能GPIO时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); //Enable the Usart clock //使能USART时钟
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);	//RX
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);	//TX 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;            //输出模式
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;          //推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;       //高速50MHZ
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;            //上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);  		          //初始化
	
  //UsartNVIC configuration //UsartNVIC配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	//Preempt priority //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;
	//Preempt priority //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		
	//Enable the IRQ channel //IRQ通道使能	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
  //Initialize the VIC register with the specified parameters 
	//根据指定的参数初始化VIC寄存器		
	NVIC_Init(&NVIC_InitStructure);
	
  //USART Initialization Settings 初始化设置
	USART_InitStructure.USART_BaudRate = bound; //Port rate //串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //The word length is 8 bit data format //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //A stop bit //一个停止
	USART_InitStructure.USART_Parity = USART_Parity_No; //Prosaic parity bits //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //No hardware data flow control //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//Sending and receiving mode //收发模式
	USART_Init(USART3, &USART_InitStructure);      //Initialize serial port 3 //初始化串口3
	
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); //Open the serial port to accept interrupts //开启串口接受中断
    USART_Cmd(USART3, ENABLE);                     //Enable serial port 3 //使能串口3 
}

/**************************************************************************
Function: Serial port 4 initialization
Input   : none
Output  : none
函数功能：串口4初始化
入口参数：无
返回  值：无
**************************************************************************/
void uart4_init(u32 bound)
{  	 
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	 //Enable the gpio clock  //使能GPIO时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE); //Enable the Usart clock //使能USART时钟
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4);		//TX
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11 ,GPIO_AF_UART4);	//RX 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;            //输出模式
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;          //推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;       //高速50MHZ
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;            //上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);  		          //初始化
	
	//UsartNVIC configuration //UsartNVIC配置
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	//Preempt priority //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;
	//Subpriority //子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
  //Enable the IRQ channel //IRQ通道使能	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  //Initialize the VIC register with the specified parameters 
	//根据指定的参数初始化VIC寄存器		
	NVIC_Init(&NVIC_InitStructure);	
	
	//USART Initialization Settings 初始化设置
	USART_InitStructure.USART_BaudRate = bound; //Port rate //串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //The word length is 8 bit data format //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //A stop bit //一个停止
	USART_InitStructure.USART_Parity = USART_Parity_No; //Prosaic parity bits //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //No hardware data flow control //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//Sending and receiving mode //收发模式
	USART_Init(UART4, &USART_InitStructure);      //Initialize serial port 2 //初始化串口2
	
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE); //Open the serial port to accept interrupts //开启串口接受中断
	USART_Cmd(UART4, ENABLE);                     //Enable serial port 2 //使能串口2 
}


/**************************************************************************
Function: Serial port 5 initialization
Input   : none
Output  : none
函数功能：串口5初始化
入口参数：bound 波特率
返回  值：无
**************************************************************************/
void uart5_init(u32 bound)
{  	 
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);	 //Enable the gpio clock  //使能GPIO时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE); //Enable the Usart clock //使能USART时钟
	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5);	//RX
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_UART5);	//TX 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;            //输出模式
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;          //推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;       //高速50MHZ
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;            //上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);  		          //初始化
	
	//UsartNVIC configuration //UsartNVIC配置
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	//Preempt priority //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;
	//Preempt priority //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		
	//Enable the IRQ channel //IRQ通道使能	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	//Initialize the VIC register with the specified parameters 
	//根据指定的参数初始化VIC寄存器		
	NVIC_Init(&NVIC_InitStructure);
	
	//USART Initialization Settings 初始化设置
	USART_InitStructure.USART_BaudRate = bound; //Port rate //串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //The word length is 8 bit data format //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //A stop bit //一个停止
	USART_InitStructure.USART_Parity = USART_Parity_No; //Prosaic parity bits //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //No hardware data flow control //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//Sending and receiving mode //收发模式
	USART_Init(UART5, &USART_InitStructure);      //Initialize serial port 5 //初始化串口5
	
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE); //Open the serial port to accept interrupts //开启串口接受中断
	USART_Cmd(UART5, ENABLE);                     //Enable serial port 5 //使能串口5 
}


/***************************************************串口接收中断部分**********************************************************/

/**************************************************************************
Function: Serial port 1 receives interrupted
Input   : none
Output  : none
函数功能：串口1接收中断
入口参数：无
返 回 值：无
**************************************************************************/
#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记
//int USART1_IRQHandler(void)
//{	
//	u8 Res;
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //Check if data is received //判断是否接收到数据
//	{
//		Res = USART_ReceiveData(USART1);	//读取接收到的数据
//		Serial1Data(Res);
//		
//		if((USART_RX_STA&0x8000)==0)//接收未完成
//			{
//			if(USART_RX_STA&0x4000)//接收到了0x0d
//				{
//				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
//				else USART_RX_STA|=0x8000;	//接收完成了 
//				}
//				else //还没收到0X0D
//				{	
//				if(Res==0x0d)USART_RX_STA|=0x4000;
//				else
//					{
//					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
//					USART_RX_STA++;
//					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
//					}		 
//				}
//			}
//		//清空标志位
//		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
//	} 
//  return 0;
//}


int USART1_IRQHandler(void)
{	
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //Check if data is received //判断是否接收到数据
	{
		Res = USART_ReceiveData(USART1);	//读取接收到的数据
		Serial1Data(Res);
		
		if((USART_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART_RX_STA&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
				}
				else //还没收到0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}
		//***************代码添加部分*************//
			USART_ClearFlag(USART1,USART_FLAG_RXNE);//清除RXNE标志位
			USART_ClearITPendingBit(USART1,USART_FLAG_RXNE);
                                                    //清除RXNE中断标志
		//清空标志位
		//USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	}

		if(USART_GetFlagStatus(USART1, USART_IT_ORE) != RESET)  
                    //需要用USART_GetFlagStatus函数来检查ORE溢出中断
		{
			USART_ClearFlag(USART1,USART_FLAG_ORE);//清除ORE标志位
			USART_ReceiveData(USART1);	           //抛弃接收到的数据			
         } 
		  //****************代码添加结束****************//
  return 0;
}



//串口2中断服务函数
void USART2_IRQHandler(void)
{
  if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
  {   
    USART_SendData(USART2, TxBuffer[TxCounter++]); 
    USART_ClearITPendingBit(USART2, USART_IT_TXE);
    if(TxCounter == count) USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
  }
	else if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
		CopeSerial2Data((unsigned char)USART2->DR);//处理数据
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
  }
	
	USART_ClearITPendingBit(USART2,USART_IT_ORE);
}


/**************************************************************************
Function: Serial port 3 receives interrupted
Input   : none
Output  : none
函数功能：串口3接收中断
入口参数：无
返回  值：无
**************************************************************************/
int USART3_IRQHandler(void)
{	u8 res;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //Check if data is received //判断是否接收到数据
	{
		res = USART_ReceiveData(USART3);
		Serial3Data(res);
		USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	} 
  return 0;
	
	
	
	
}


/**************************************************************************
Function: Refresh the OLED screen
Input   : none
Output  : none
函数功能：串口4接收中断
入口参数：无
返回  值：无
**************************************************************************/
int UART4_IRQHandler(void)
{	
	u8 Res;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET) //Check if data is received //判断是否接收到数据
	{	      
		Res = USART_ReceiveData(UART4);	//读取接收到的数据
		//USART_SendData(USART1,Res);			//回显
		//Serial4Data(Res);
		
		USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	}
  return 0;	
}


/**************************************************************************
函数功能：串口5接收中断
入口参数：无
返回  值：无
**************************************************************************/
int UART5_IRQHandler(void)
{	
	u8 Res;
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) //Check if data is received //判断是否接收到数据
	{	      
		Res = USART_ReceiveData(UART5);	//读取接收到的数据
		//USART_SendData(USART1,Res);			//回显
		Serial5Data(Res);
		
		USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
	}
  return 0;	
}
/***************************************************串口发送数据部分**********************************************************/

/**************************************************************************
Function: Serial port 1 sends data
Input   : none
Output  : none
函数功能：串口1发送数据
入口参数：无
返回  值：无
**************************************************************************/
void USART1_SEND(void)
{
  unsigned char i = 0;	
	
	for(i=0; i<24; i++)
	{
		usart1_send(Send_Data.buffer[i]);
	}	 
}
/**************************************************************************
Function: Serial port 3 sends data
Input   : none
Output  : none
函数功能：串口3发送数据
入口参数：无
返回  值：无
**************************************************************************/
void USART3_SEND(void)
{
  unsigned char i = 0;	
	for(i=0; i<24; i++)
	{
		usart3_send(Send_Data.buffer[i]);
	}	 
}



void u1_SendByte(uint8_t Byte)		//发送一个字节数据
{
	USART_SendData(USART1, Byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

/*****************  发送一个16位数 **********************/
void u1_SendHalfWord(uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* 取出高八位 */
	temp_h = (ch&0XFF00)>>8;
	/* 取出低八位 */
	temp_l = ch&0XFF;
	
	/* 发送高八位 */
	USART_SendData(USART1,temp_h);	
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	
	/* 发送低八位 */
	USART_SendData(USART1,temp_l);	
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	
}

void u1_SendArray(uint8_t *Array, uint16_t Length)	//发送一个8位的数组
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		u1_SendByte(Array[i]);
	}
}

void u1_Send_HalfWordArray(uint16_t *Array, uint16_t Length)	//发送一个16位的数组
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		u1_SendHalfWord(Array[i]);
	}
}




void u3_SendByte(uint8_t Byte)		//发送一个字节数据
{
	USART_SendData(USART3, Byte);
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
}

void u3_SendArray(uint8_t *Array, uint16_t Length)	//发送一个8位的数组
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		u3_SendByte(Array[i]);
	}
}


/**************************************************************************
Function: Serial port 1 sends data
Input   : The data to send
Output  : none
函数功能：串口1发送数据
入口参数：要发送的数据
返回  值：无
**************************************************************************/
void usart1_send(u8 data)
{
	USART1->DR = data;
	while((USART1->SR&0x40)==0);	
}
/**************************************************************************
Function: Serial port 4 sends data
Input   : The data to send
Output  : none
函数功能：串口4发送数据
入口参数：要发送的数据
返回  值：无
**************************************************************************/
void usart4_send(u8 data)
{
	UART4->DR = data;
	while((UART4->SR&0x40)==0);	
}
/**************************************************************************
Function: Serial port 3 sends data
Input   : The data to send
Output  : none
函数功能：串口3发送数据
入口参数：要发送的数据
返回  值：无
**************************************************************************/
void usart3_send(u8 data)
{
	USART3->DR = data;
	while((USART3->SR&0x40)==0);	
}


void UART2_Put_Char(unsigned char DataToSend)
{
	TxBuffer[count++] = DataToSend;  
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);  
}

void UART2_Put_String(unsigned char *Str)
{
	while(*Str)
	{
		if(*Str=='\r')UART2_Put_Char(0x0d);
			else if(*Str=='\n')UART2_Put_Char(0x0a);
				else UART2_Put_Char(*Str);
		Str++;
	}
}




/***************************************************串口接收发送函数**********************************************************/


//串口1接收发送处理函数
void Serial1Data(uint8_t ucData){
	
	
		static unsigned char ucRxBuffer[250];
		static unsigned char ucRxCnt = 0;	
		ucRxBuffer[ucRxCnt++]=ucData;	//将收到的数据存入缓冲区中
		
		//保存接收的字符到字符串数组的对应的位置
		g_usart1_recv_buf[g_usart1_recv_cnt] =ucData;
		//接收字符的计数
		g_usart1_recv_cnt++;
		
		
			
	
		//当接收到符合格式的字符串，或者接收达到数组上限，就进行字符串数据的处理
		if(g_usart1_recv_cnt > RECV_BUF_SIZE-1 || g_usart1_recv_buf[g_usart1_recv_cnt-1] == '\n')
		{
			//strncmp函数是用来比较两个字符串前N个字节是否相同，strcmp函数是比较两个字符串是否相同
			/*
				参数1、2：要比较的两个字符串地址
				参数3：要比较的两个字符串的前n个字节
				返回0，则表示字符串前N个字节相同
			*/
			/*
						printf,scanf函数是标准输出输入函数，他会自动从标准设备中进行数据的输出与输入（stdout,stdin）
						sprintf,sscanf函数他们的输出输入已经实现重定向，他会自动把数据从所指定的内存空间中进行输出与输入（buf）
						参数1：要输出/输入的数据内存空间
						参数2：要输出/输入的字符串格式，格式化符号%d：整型，%c：字符，%s：字符串
						参数3：要格式化输出/输入的变量
				*/
			
			
			if(strncmp(g_usart1_recv_buf, "SM1_EN\n ", 6) == 0)//使能电机1
			{
				systemConfigData.SM_Enable[0] = 1;
			}
			
			if(strncmp(g_usart1_recv_buf, "SM2_EN\n ", 6) == 0)//使能电机2
			{
				systemConfigData.SM_Enable[1] = 1;
			}
			
			if(strncmp(g_usart1_recv_buf, "SM3_EN\n ", 6) == 0)//使能电机3
			{
				systemConfigData.SM_Enable[2] = 1;
			}
			
			if(strncmp(g_usart1_recv_buf, "FLASH_SAVE\n ", 10) == 0)//保存配置
			{
				supervisorData.flashSave = 1;
				//NVIC_SystemReset();
			}
			
			if(strncmp(g_usart1_recv_buf, "ERASE_FLASH\n ", 11) == 0)//恢复出厂设置
			{
				supervisorData.eraseflash = 1;
			}
			
			if(strncmp(g_usart1_recv_buf, "Rebot\n ", 5) == 0)//重启设备
			{
				SM.Rebot_flag = 1;
			}
			
			if(strncmp(g_usart1_recv_buf, "Check_SM\n ", 8) == 0)//重启设备
			{
				printf("SM1_EN = %d,SM2_EN = %d,SM3_EN = %d\r\n",systemConfigData.SM_Enable[0],systemConfigData.SM_Enable[1],systemConfigData.SM_Enable[2]);
			}
			
			if(strncmp(g_usart1_recv_buf, "SMV_1=\n ", 6) == 0)//伺服电机1速度控制命令
			{
				sscanf(g_usart1_recv_buf, "SMV_1=%d\n", &SM.SetVel[0].s32_temp);//速度修改
			}
			
			if(strncmp(g_usart1_recv_buf, "SMV_2=\n ", 6) == 0)//伺服电机2速度控制命令
			{
				sscanf(g_usart1_recv_buf, "SMV_2=%d\n", &SM.SetVel[1].s32_temp);//速度修改
				
			}
			
			if(strncmp(g_usart1_recv_buf, "SMV_3=\n ", 6) == 0)//伺服电机3速度控制命令
			{
				sscanf(g_usart1_recv_buf, "SMV_3=%d\n", &SM.SetVel[2].s32_temp);//速度修改
				
			}
			
			if(strncmp(g_usart1_recv_buf, "MOB=\n ", 4) == 0)//伺服电机3速度控制命令
			{
				sscanf(g_usart1_recv_buf, "MOB=%f\n", &Motor_SpeedB_Goal.target);//速度修改
				
			}
			
			
			if(strncmp(g_usart1_recv_buf, "F=\n ", 2) == 0)//速度控制命令
			{
				sscanf(g_usart1_recv_buf, "F=%d\n", &balanceData.flag);//速度修改
			}
			
			//清空字符串数组
			memset(g_usart1_recv_buf, 0, sizeof(g_usart1_recv_buf));
			
			//计数值清空
			g_usart1_recv_cnt = 0;
		}
		
		if (ucRxBuffer[0]!=0xFC) //数据头不对，则重新开始寻找0xFC数据头
			{
				if(ucRxBuffer[1]!=0x05){
						ucRxCnt=0;
						return;
				}
			}
		
		if (ucRxCnt<4) {return;}//数据不满4个，则返回
			else
			{
				Motor_SpeedA_Goal.target = XYZ_Target_Speed_transition(ucRxBuffer[3],ucRxBuffer[2]);
				ucRxCnt=0;//清空缓存区
			}
	
	
}


//串口3接收处理函数
void Serial3Data(uint8_t ucData){
	static unsigned char ucRxBuffer[250];
	static unsigned char ucRxCnt = 0;	
	ucRxBuffer[ucRxCnt++]=ucData;	//将收到的数据存入缓冲区中
	if (ucRxBuffer[0]!=0xFC) //数据头不对，则重新开始寻找0x66数据头
	{
		if(ucRxBuffer[1]!=0x05){
				ucRxCnt=0;
				return;
		}
	}
	if (ucRxCnt<4) {return;}//数据不满4个，则返回
	else
	{
		Motor_SpeedA_Goal.target = XYZ_Target_Speed_transition(ucRxBuffer[3],ucRxBuffer[2]);
		ucRxCnt=0;//清空缓存区
	}
	
	
	//保存接收的字符到字符串数组的对应的位置
		g_usart3_recv_buf[g_usart3_recv_cnt] =ucData;
		//接收字符的计数
		g_usart3_recv_cnt++;
		
		
			
	
		//当接收到符合格式的字符串，或者接收达到数组上限，就进行字符串数据的处理
		if(g_usart3_recv_cnt > RECV_BUF_SIZE-1 || g_usart3_recv_buf[g_usart3_recv_cnt-1] == '\n')
		{
			//strncmp函数是用来比较两个字符串前N个字节是否相同，strcmp函数是比较两个字符串是否相同
			/*
				参数1、2：要比较的两个字符串地址
				参数3：要比较的两个字符串的前n个字节
				返回0，则表示字符串前N个字节相同
			*/
			/*
						printf,scanf函数是标准输出输入函数，他会自动从标准设备中进行数据的输出与输入（stdout,stdin）
						sprintf,sscanf函数他们的输出输入已经实现重定向，他会自动把数据从所指定的内存空间中进行输出与输入（buf）
						参数1：要输出/输入的数据内存空间
						参数2：要输出/输入的字符串格式，格式化符号%d：整型，%c：字符，%s：字符串
						参数3：要格式化输出/输入的变量
				*/
			
			
			if(strncmp(g_usart3_recv_buf, "V=\n ", 2) == 0)//速度控制命令
			{
				sscanf(g_usart3_recv_buf, "V=%f\n", &OdriveData.SetVel[0].float_temp);//速度修改
				OdriveData.SetVel[1].float_temp = -OdriveData.SetVel[0].float_temp;
				flag = 1;
				//OdriveData.Vel_gain[0].float_temp += 5;
				//printf("%f\r\n",OdriveData.SetVel[1].float_temp);
				//sprintf(g_usart1_send_buf, "修改后Angle=%f\r\n", OdriveData.SetVel[0].float_temp);//修改后显示
			}
			
			
			if(strncmp(g_usart3_recv_buf, "P=\n ", 2) == 0)//
			{
				
				sscanf(g_usart3_recv_buf, "P=%f\n", &OdriveData.SetPos[0].float_temp);//速度修改
				//sprintf(g_usart1_send_buf, "修改后Angle=%f\r\n", A);//速度限制修改后显示
			}
			
			if(strncmp(g_usart3_recv_buf, "Pos\n ", 4) == 0)//
			{
				OdriveData.AxisState[axis0] = CMD_MENU;
				OdriveData.ControlMode[0] = CONTROL_MODE_POSITION_TRAP;
	
				OdriveData.ControlModeFlag = 1;
				OdriveData.RequestedStateFlag = 1;
				//sscanf(g_usart1_recv_buf, "P=%f\n", &OdriveData.SetPos[0].float_temp);//速度修改
				//sprintf(g_usart1_send_buf, "修改后Angle=%f\r\n", A);//速度限制修改后显示
			}
			
			if(strncmp(g_usart3_recv_buf, "Vel\n ", 4) == 0)//
			{
				
				OdriveData.AxisState[axis0] = CMD_MENU;
				OdriveData.ControlMode[0] = CONTROL_MODE_VELOCITY_RAMP;
	
				OdriveData.ControlModeFlag = 1;
				OdriveData.RequestedStateFlag = 1;
				//sscanf(g_usart1_recv_buf, "P=%f\n", &OdriveData.SetPos[0].float_temp);//速度修改
				//sprintf(g_usart1_send_buf, "修改后Angle=%f\r\n", A);//速度限制修改后显示
			}
			
			if(strncmp(g_usart3_recv_buf, "Set\n ", 4) == 0)//
			{
				OdriveData.AxisState[0] = 27;
				//OdriveData.AxisState[axis0] = CMD_MOTOR;
				OdriveData.RequestedStateFlag = 1;
				//sscanf(g_usart1_recv_buf, "P=%f\n", &OdriveData.SetPos[0].float_temp);//速度修改
				//sprintf(g_usart1_send_buf, "修改后Angle=%f\r\n", A);//速度限制修改后显示
			}
			
			if(strncmp(g_usart3_recv_buf, "Pos_gain=\n ", 9) == 0)//位置环增益修改
			{
				sscanf(g_usart3_recv_buf, "Pos_gain=%f\n", &OdriveData.Pos_gain[0].float_temp);//增益修改
				OdriveData.Pos_gain[1].float_temp = OdriveData.Pos_gain[0].float_temp;
				//增益修改标识符
				flag = 1;
			}
			
			if(strncmp(g_usart3_recv_buf, "Vel_gain=\n ", 9) == 0)//速度环增益修改
			{
				sscanf(g_usart3_recv_buf, "Vel_gain=%f\n", &OdriveData.Vel_gain[0].float_temp);//增益修改
				OdriveData.Vel_gain[1].float_temp = OdriveData.Vel_gain[0].float_temp;
				//增益修改标识符
				flag = 1;
			}
			
			if(strncmp(g_usart3_recv_buf, "Vel_integrator_gain=\n ", 20) == 0)//速度环积分增益修改
			{
				sscanf(g_usart3_recv_buf, "Vel_integrator_gain=%f\n", &OdriveData.Vel_integrator_gain[0].float_temp);//增益修改
				OdriveData.Vel_integrator_gain[1].float_temp = OdriveData.Vel_integrator_gain[0].float_temp;
				//增益修改标识符
				flag = 1;
			}
			
			if(strncmp(g_usart3_recv_buf, "OYY=\n ", 4) == 0)//速度控制命令
			{
				sscanf(g_usart3_recv_buf, "OYY=%f\n", &Angle_Goal.target);//速度修改
				Angle_Goal.finish = 0;
				//OdriveData.Vel_gain[0].float_temp += 5;
				//printf("%f\r\n",OdriveData.SetVel[1].float_temp);
				//sprintf(g_usart1_send_buf, "修改后Angle=%f\r\n", OdriveData.SetVel[0].float_temp);//修改后显示
			}
			
			if(strncmp(g_usart3_recv_buf, "MOA=\n ", 4) == 0)//速度控制命令
			{
				balanceData.flag = 0;
				sscanf(g_usart3_recv_buf, "MOA=%f\n", &Motor_SpeedA_Goal.target);//速度修改
				//OdriveData.Vel_gain[0].float_temp += 5;
				//printf("%f\r\n",OdriveData.SetVel[1].float_temp);
				//sprintf(g_usart1_send_buf, "修改后Angle=%f\r\n", OdriveData.SetVel[0].float_temp);//修改后显示
			}
			
			if(strncmp(g_usart3_recv_buf, "MOB=\n ", 4) == 0)//速度控制命令
			{
				balanceData.flag = 1;
				sscanf(g_usart3_recv_buf, "MOB=%f\n", &Motor_SpeedB_Goal.target);//速度修改
				//OdriveData.Vel_gain[0].float_temp += 5;
				//printf("%f\r\n",OdriveData.SetVel[1].float_temp);
				//sprintf(g_usart1_send_buf, "修改后Angle=%f\r\n", OdriveData.SetVel[0].float_temp);//修改后显示
			}
			
			if(strncmp(g_usart3_recv_buf, "F=\n ", 2) == 0)//速度控制命令
			{
				sscanf(g_usart3_recv_buf, "F=%d\n", &balanceData.flag);//速度修改
			}
			
			//清空字符串数组
			memset(g_usart3_recv_buf, 0, sizeof(g_usart3_recv_buf));
			
			//计数值清空
			g_usart3_recv_cnt = 0;
		}
	
}


//串口4接收处理函数
void Serial4Data(uint8_t ucData){
	static unsigned char ucRxBuffer[250];
	static unsigned char ucRxCnt = 0;	
	ucRxBuffer[ucRxCnt++]=ucData;	//将收到的数据存入缓冲区中
	if (ucRxBuffer[0]!=0x66) //数据头不对，则重新开始寻找0x66数据头
	{
		ucRxCnt=0;
		return;
	}
	if (ucRxCnt<65) {return;}//数据不满65个，则返回
	else
	{
		for(int i = 1;i < 65; i++){
			//buf[i - 1] = ucRxBuffer[i];
		}
		ucRxCnt=0;//清空缓存区
	}
}


//串口5接收处理函数
void Serial5Data(uint8_t ucData){
	static unsigned char ucRxBuffer[250];
	static unsigned char ucRxCnt = 0;	
	ucRxBuffer[ucRxCnt++]=ucData;	//将收到的数据存入缓冲区中
	if(ucRxBuffer[0] != 0xAA){//数据头不对，则重新开始寻找0xAA、0x55数据头
		if(ucRxBuffer[1] != 0x55){
			ucRxCnt=0;
			return;
		}
	}
	if (ucRxCnt<4) {return;}//数据不满2个，则返回
	else
	{
		for(int i = 0;i < 4; i++){
			NFC.NFC_buf[i] = ucRxBuffer[i];
		}
		ucRxCnt=0;//清空缓存区
	}
	
//	static unsigned char ucRxBuffer[250];
//	static unsigned char ucRxCnt = 0;	
//	ucRxBuffer[ucRxCnt++]=ucData;	//将收到的数据存入缓冲区中
//	if (ucRxCnt<4) {return;}//数据不满8个，则返回
//	else
//	{
//		for(int i = 0;i < 4; i++){
//			NFC.NFC_buf[i] = ucRxBuffer[i];
//		}
//		ucRxCnt=0;//清空缓存区
//	}
}



/**************************************************************************
Function: Calculates the check bits of data to be sent/received
Input   : Count_Number: The first few digits of a check; Mode: 0-Verify the received data, 1-Validate the sent data
Output  : Check result
函数功能：计算要发送/接收的数据校验结果
入口参数：Count_Number：校验的前几位数；Mode：0-对接收数据进行校验，1-对发送数据进行校验
返回  值：校验结果
**************************************************************************/
u8 Check_Sum(unsigned char Count_Number,unsigned char Mode)
{
    unsigned char check_sum=0,k;

    //Validate the data to be sent
    //对要发送的数据进行校验
    if(Mode==1)
        for(k=0; k<Count_Number; k++)
        {
            check_sum=check_sum^Send_Data.buffer[k];
        }

    //Verify the data received
    //对接收到的数据进行校验
    if(Mode==0)
        for(k=0; k<Count_Number; k++)
        {
            check_sum=check_sum^Receive_Data.buffer[k];
        }

    //对要发送的超声波数据进行校验
    if(Mode==3)
    {
        for(k=0; k<Count_Number; k++)
        {
            //check_sum=check_sum^Distance_Data.buffer[k];
        }
    }
    return check_sum;
}




//数据处理函数
void Data_Processing(uint8_t ucData){
	
	//定义接收的数据数量
	static u8 Count=0;
	
	//Fill the array with serial data
	//串口数据填入数组
	Receive_Data.buffer[Count]=ucData;
	
	//Ensure that the first data in the array is FRAME_HEADER
	//确保数组第一个数据为FRAME_HEADER
	if(ucData == FRAME_HEADER||Count>0)
		Count++;
	else
		Count=0;
	
	if (Count == 11) //Verify the length of the packet //验证数据包的长度
	{
		Count=0; //Prepare for the serial port data to be refill into the array //为串口数据重新填入数组做准备
		if(Receive_Data.buffer[10] == FRAME_TAIL) //Verify the frame tail of the packet //验证数据包的帧尾
		{
			//Data exclusionary or bit check calculation, mode 0 is sent data check
			//数据异或位校验计算，模式0是发送数据校验
			if(Receive_Data.buffer[9] ==Check_Sum(9,0))
			{
				//disable_robot_count=0;//计时变量清零

				//Serial port 1 controls flag position 1, other flag position 0
				//串口1控制标志位置1，其它标志位置0
				//Set_Control_Mode(_USART_Control);

				//Calculate the target speed of three axis from serial data, unit m/s
				//从串口数据求三轴目标速度， 单位m/s
//				Move_X=XYZ_Target_Speed_transition(Receive_Data.buffer[3],Receive_Data.buffer[4]);
//				Move_Y=XYZ_Target_Speed_transition(Receive_Data.buffer[5],Receive_Data.buffer[6]);
//				Move_Z=XYZ_Target_Speed_transition(Receive_Data.buffer[7],Receive_Data.buffer[8]);
			}
		}
	}
		
}


/**************************************************************************
Function: After the top 8 and low 8 figures are integrated into a short type data, the unit reduction is converted
Input   : 8 bits high, 8 bits low
Output  : The target velocity of the robot on the X/Y/Z axis
函数功能：将上位机发过来的高8位和低8位数据整合成一个short型数据后，再做单位还原换算
入口参数：高8位，低8位
返回  值：机器人X/Y/Z轴的目标速度
**************************************************************************/
float XYZ_Target_Speed_transition(u8 high,u8 low)
{
    //Data conversion intermediate variable
    //数据转换的中间变量
    short transition = 0;
	float out = 0;

    //将高8位和低8位整合成一个16位的short型数据
    //The high 8 and low 8 bits are integrated into a 16-bit short data
    transition=(int16_t)((high<<8) | low);
    
    return transition/100+(transition%100)*0.01f; //Unit conversion, cm/s->m/s //单位转换, cm/s->m/s
//	out = transition;
//	 out;
}

