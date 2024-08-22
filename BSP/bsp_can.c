#include "bsp_can.h"

/******************************�ⲿ���ú���************************************/
void BSP_CAN_Mode_Init(BSP_CAN_TypeDef *BSP_CANx,u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode,u8 PreemptionPriority,u8 SubPriority);
//void BSP_CAN_Mode_Init(CAN_TypeDef *CANx,u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode,u8 PreemptionPriority,u8 SubPriority);
 u8	 BSP_CAN_Send_Msg(BSP_CAN_TypeDef *BSP_CANx,CanTxMsg *TxMessage);
 u8	 BSP_CAN_Receive_Msg(BSP_CAN_TypeDef *BSP_CANx,CanRxMsg *RxMessage);
/*****************************************************************************/

const CAN_FilterInitTypeDef  CAN1_FilterInitStructure = {
	.CAN_FilterNumber = 0,										/*������0*/					
	.CAN_FilterMode = CAN_FilterMode_IdMask,	/*��ʶ������λģʽ*/	
	.CAN_FilterScale = CAN_FilterScale_32bit,	/*32λ��ģʽ*/				
	.CAN_FilterIdHigh = 0x0000,								/*32λMASK*/				
	.CAN_FilterIdLow = 0x0000,
	.CAN_FilterMaskIdHigh = 0x0000,
	.CAN_FilterMaskIdLow = 0x0000,
	.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0,	/*������0������FIFO0*/	
	.CAN_FilterActivation = ENABLE,								/*���������*/					
};

const CAN_FilterInitTypeDef  CAN2_FilterInitStructure = {
	.CAN_FilterNumber = 20,										/*������14*/
	.CAN_FilterMode = CAN_FilterMode_IdMask,	/*��ʶ������λģʽ*/	
	.CAN_FilterScale = CAN_FilterScale_32bit,	/*32λ��ģʽ*/				
	.CAN_FilterIdHigh = 0x0000,								/*32λMASK*/				
	.CAN_FilterIdLow = 0x0000,
	.CAN_FilterMaskIdHigh = 0x0000,
	.CAN_FilterMaskIdLow = 0x0000,
	//.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0,	/*������0������FIFO0*/
	.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0,	/*������0������FIFO0*/
	.CAN_FilterActivation = ENABLE,								/*���������*/
};

/*
***************************************************
��������BSP_SPI_RCBSP_CAN_RCC_InitC_Init
���ܣ�����CAN����ʱ��
��ڲ�����	CANx��CAN��
����ֵ����
Ӧ�÷�Χ���ڲ�����
��ע��
***************************************************
*/
static void BSP_CAN_RCC_Init(CAN_TypeDef* CANx){
	uint32_t RCC_CANx;
	
	if(CANx == CAN1)
		RCC_CANx = RCC_APB1Periph_CAN1;
	else if(CANx == CAN2)
		RCC_CANx = RCC_APB1Periph_CAN1 | RCC_APB1Periph_CAN2;
	
	RCC_APB1PeriphClockCmd(RCC_CANx,ENABLE);
}

/*
***************************************************
��������CANx_TO_GPIO_AF
���ܣ���CAN�����GPIO_AF
��ڲ�����	CANx��CAN��
����ֵ��GPIO_AF:���õ�CANģʽ
Ӧ�÷�Χ���ڲ�����
��ע��
***************************************************
*/
static uint8_t CANx_TO_GPIO_AF(CAN_TypeDef *CANx){
	uint8_t GPIO_AF;
	if(CANx == CAN1)			
		GPIO_AF = GPIO_AF_CAN1;
	else if(CANx == CAN2)	
		GPIO_AF = GPIO_AF_CAN2;
	
	return GPIO_AF;
}

/*
***************************************************
��������GPIO_TO_NVIC_IRQChannel
���ܣ����NVIC�ж�ͨ��
��ڲ�����	GPIO�����ź�
����ֵ��NVIC_IRQChannel
Ӧ�÷�Χ���ڲ�����
��ע��
***************************************************
*/
static uint8_t	CAN_TO_NVIC_IRQChannel(CAN_TypeDef* CANx){
	uint8_t NVIC_IRQChannel;
	
	if(CANx == CAN1)			
		NVIC_IRQChannel = CAN1_RX0_IRQn;
	else if(CANx == CAN2)	
		NVIC_IRQChannel = CAN2_RX0_IRQn;
	
	return NVIC_IRQChannel;	
}

/*
***************************************************
��������BSP_CAN_Mode_Init
���ܣ�CAN��ʼ��
��ڲ�����	BSP_CANx��CAN��
					tsjw������ͬ����Ծʱ�䵥Ԫ.��Χ:CAN_SJW_1tq~ CAN_SJW_4tq
					tbs2��ʱ���2��ʱ�䵥Ԫ.   ��Χ:CAN_BS2_1tq~CAN_BS2_8tq;
					tbs1��ʱ���1��ʱ�䵥Ԫ.   ��Χ:CAN_BS1_1tq ~CAN_BS1_16tq
					brp �������ʷ�Ƶ��.				��Χ:1~1024; tq=(brp)*tpclk1
					mode��CANxģʽ							��Χ:CAN_Mode_Normal,��ͨģʽ;CAN_Mode_LoopBack,�ػ�ģʽ;
					PreemptionPriority����ռ���ȼ�
					SubPriority�������ȼ�
����ֵ����
Ӧ�÷�Χ���ⲿ����
��ע���ѽ����ʼ��CAN2�����ȳ�ʼ��CAN1
BSP_CAN_Mode_Init(&can1,CAN_SJW_1tq,CAN_BS2_5tq,CAN_BS1_9tq,3,CAN_Mode_Normal,Preemption,Sub);

***************************************************
*/
//CANbps= Fpclk/((BRP+1)*((Tseg1+1)+(Tseg2+1)+1)  
//��������CANbps=APB1����Ƶ��45M/3/(1+5+9))=1000k bps
//�������÷���: Tseg1>=Tseg2  Tseg2>=tq; Tseg2>=2TSJW	



void BSP_CAN_Mode_Init(BSP_CAN_TypeDef *BSP_CANx,u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode,u8 PreemptionPriority,u8 SubPriority){
	CAN_InitTypeDef		CAN_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStruct;
	
	CAN_FilterInitStruct = BSP_CANx->CAN_FilterInitStructure;
	
	/*************��ʼ��CANxʱ��***************/
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//ʹ��CAN1ʱ��
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//ʹ��CAN1ʱ��
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//ʹ��CAN1ʱ��
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);//ʹ��CAN2ʱ��
	BSP_CAN_RCC_Init(BSP_CANx->CANx);
	
	/*************��ʼ��CANx����***************/
	BSP_GPIO_Init(BSP_CANx->CANx_RX,GPIO_Mode_AF_PP);	//��ʼ��CANx_RX����Ϊ��������
	BSP_GPIO_Init(BSP_CANx->CANx_TX,GPIO_Mode_AF_PP);	//��ʼ��CANx_TX����Ϊ��������
	
	/*************���Ÿ���ӳ������***************/
	GPIO_Pin_TO_PinAFConfig(BSP_CANx->CANx_RX,CANx_TO_GPIO_AF(BSP_CANx->CANx));	//CANx_RX���Ÿ���Ϊ CANx
	GPIO_Pin_TO_PinAFConfig(BSP_CANx->CANx_TX,CANx_TO_GPIO_AF(BSP_CANx->CANx));	//CANx_TX���Ÿ���Ϊ CANx
	
	/****************CAN��Ԫ����*****************/
	CAN_InitStructure.CAN_TTCM=DISABLE;		//��ʱ�䴥��ͨ��ģʽ   
	CAN_InitStructure.CAN_ABOM=ENABLE;		//�����Զ����߹���	  
	CAN_InitStructure.CAN_AWUM=DISABLE;		//˯��ģʽͨ����������(���CAN->MCR��SLEEPλ)
	CAN_InitStructure.CAN_NART=DISABLE;		//��ֹ�����Զ����� 
	CAN_InitStructure.CAN_RFLM=DISABLE;		//���Ĳ�����,�µĸ��Ǿɵ�  
	CAN_InitStructure.CAN_TXFP=DISABLE;		//���ȼ��ɱ��ı�ʶ������ 
	CAN_InitStructure.CAN_Mode= mode;	 		//ģʽ���� 
	CAN_InitStructure.CAN_SJW=tsjw;				//����ͬ����Ծ����(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1=tbs1; 			//Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2=tbs2;				//Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler=brp;  //��Ƶϵ��(Fdiv)Ϊbrp+1	
	CAN_Init(BSP_CANx->CANx, &CAN_InitStructure);   // ��ʼ��CANx 
	
	/****************CAN���ù�����*****************/
	//ʹ��CAN2�ü������
	CAN_SlaveStartBank(14);
	CAN_FilterInitStruct.CAN_FilterNumber=0;	  //������0
	CAN_FilterInitStruct.CAN_FilterMode=CAN_FilterMode_IdMask; 
	CAN_FilterInitStruct.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
	CAN_FilterInitStruct.CAN_FilterIdHigh=0x0000;////32λID
	CAN_FilterInitStruct.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStruct.CAN_FilterMaskIdHigh=0x0000;//32λMASK
	CAN_FilterInitStruct.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStruct.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
	CAN_FilterInitStruct.CAN_FilterActivation=ENABLE; //���������0

	CAN_FilterInit(&CAN_FilterInitStruct);   
	
	/****************CAN������Ϣ�ж�*****************/
	CAN_ITConfig(BSP_CANx->CANx,CAN_IT_FMP0,ENABLE);	//FIFO0��Ϣ�Һ��ж�����
	
	
	NVIC_InitStructure.NVIC_IRQChannel = CAN_TO_NVIC_IRQChannel(BSP_CANx->CANx);	//CANx�����ж�
	//NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn | CAN2_RX0_IRQn;	//CANx�����ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority;		//��ռ���ȼ�
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority;									//�����ȼ�
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//ʹ��CANx�����ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);	//����
}

//void BSP_CAN_Mode_Init(CAN_TypeDef *CANx,u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode,u8 PreemptionPriority,u8 SubPriority){
//	CAN_InitTypeDef		CAN_InitStructure;
//	NVIC_InitTypeDef  NVIC_InitStructure;
//	GPIO_InitTypeDef 	   GPIO_InitStructure;		//GPIO�����ӿ�
//	CAN_FilterInitTypeDef CAN_FilterInitStruct;
//	
//	//CAN_FilterInitTypeDef  CAN1_FilterInitStructure;
//	
//	
//	CAN_FilterInitTypeDef  CAN1_FilterInitStructure = {
//	.CAN_FilterNumber = 0,										/*������0*/					
//	.CAN_FilterMode = CAN_FilterMode_IdMask,	/*��ʶ������λģʽ*/	
//	.CAN_FilterScale = CAN_FilterScale_32bit,	/*32λ��ģʽ*/				
//	.CAN_FilterIdHigh = 0x0000,								/*32λMASK*/				
//	.CAN_FilterIdLow = 0x0000,
//	.CAN_FilterMaskIdHigh = 0x0000,
//	.CAN_FilterMaskIdLow = 0x0000,
//	.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0,	/*������0������FIFO0*/	
//	.CAN_FilterActivation = ENABLE,								/*���������*/					
//	};
//	/*************��ʼ��CANxʱ��***************/
//	BSP_CAN_RCC_Init(CANx);
//	
//	/*************��ʼ��CANx����***************/
//	
//	/*************���Ÿ���ӳ������***************/
//	//PD0 RX PD1 TX
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);
//	
//	GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_CAN1);
//	GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_CAN1);
//	
//	//Configure CAN1 TX
//	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
//	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
//	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_Speed=GPIO_Fast_Speed;
//	
//	GPIO_Init(GPIOD,&GPIO_InitStructure);
//	
//	//Configure CAN1 RX
//	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
//	GPIO_Init(GPIOD,&GPIO_InitStructure);
//	
//	/****************CAN��Ԫ����*****************/
//	CAN_InitStructure.CAN_TTCM=DISABLE;		//��ʱ�䴥��ͨ��ģʽ   
//	CAN_InitStructure.CAN_ABOM=ENABLE;		//�����Զ����߹���	  
//	CAN_InitStructure.CAN_AWUM=DISABLE;		//˯��ģʽͨ����������(���CAN->MCR��SLEEPλ)
//	CAN_InitStructure.CAN_NART=DISABLE;		//��ֹ�����Զ����� 
//	CAN_InitStructure.CAN_RFLM=DISABLE;		//���Ĳ�����,�µĸ��Ǿɵ�  
//	CAN_InitStructure.CAN_TXFP=DISABLE;		//���ȼ��ɱ��ı�ʶ������ 
//	CAN_InitStructure.CAN_Mode= mode;	 		//ģʽ���� 
//	CAN_InitStructure.CAN_SJW=tsjw;				//����ͬ����Ծ����(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
//	CAN_InitStructure.CAN_BS1=tbs1; 			//Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
//	CAN_InitStructure.CAN_BS2=tbs2;				//Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
//	CAN_InitStructure.CAN_Prescaler=brp;  //��Ƶϵ��(Fdiv)Ϊbrp+1	
//	CAN_Init(CANx, &CAN_InitStructure);   // ��ʼ��CANx 
//	
//		/****************CAN���ù�����*****************/
//	CAN_FilterInitStruct.CAN_FilterNumber=0;	  //������0
//	CAN_FilterInitStruct.CAN_FilterMode=CAN_FilterMode_IdMask; 
//	CAN_FilterInitStruct.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
//	CAN_FilterInitStruct.CAN_FilterIdHigh=0x0000;////32λID
//	CAN_FilterInitStruct.CAN_FilterIdLow=0x0000;
//	CAN_FilterInitStruct.CAN_FilterMaskIdHigh=0x0000;//32λMASK
//	CAN_FilterInitStruct.CAN_FilterMaskIdLow=0x0000;
//	CAN_FilterInitStruct.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
//	CAN_FilterInitStruct.CAN_FilterActivation=ENABLE; //���������0

//	CAN_FilterInit(&CAN_FilterInitStruct);   
//	
//	/****************CAN������Ϣ�ж�*****************/
//	CAN_ITConfig(CANx,CAN_IT_FMP0,ENABLE);	//FIFO0��Ϣ�Һ��ж�����
//	NVIC_InitStructure.NVIC_IRQChannel = CAN_TO_NVIC_IRQChannel(CANx);	//CANx�����ж�
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority;		//��ռ���ȼ�
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority;									//�����ȼ�
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//ʹ��CANx�����ж�ͨ��
//	NVIC_Init(&NVIC_InitStructure);	//����
//}


//CAN2��ʼ��
void CAN2_Mode_Init(uint8_t tsjw,uint8_t tbs2,uint8_t tbs1,uint16_t brp,uint8_t mode)
{
	GPIO_InitTypeDef		GPIO_InitStructure; 
	CAN_InitTypeDef			CAN_InitStructure;
	CAN_FilterInitTypeDef	CAN_FilterInitStructure;
		
	NVIC_InitTypeDef		NVIC_InitStructure;
	
	CAN_DeInit(CAN2);
	CAN_StructInit(&CAN_InitStructure);
	
	//ʹ�����ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��PORTBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//ʹ��CAN1ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);//ʹ��CAN2ʱ��
	
	//��ʼ��GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5| GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��PB5��PB6
	
	//���Ÿ���ӳ������
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_CAN2); //GPIOB5����ΪCAN2
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_CAN2); //GPIOB6����ΪCAN2
	
	//CAN��Ԫ����
	CAN_InitStructure.CAN_TTCM=DISABLE;	//��ʱ�䴥��ͨ��ģʽ
	CAN_InitStructure.CAN_ABOM=DISABLE;	//���������Զ����߹��� ENABLE
	CAN_InitStructure.CAN_AWUM=DISABLE;	//˯��ģʽͨ����������(���CAN->MCR��SLEEPλ)
	CAN_InitStructure.CAN_NART=ENABLE;	//��ֹ�����Զ�����
	CAN_InitStructure.CAN_RFLM=DISABLE;	//���Ĳ�����,�µĸ��Ǿɵ�
	CAN_InitStructure.CAN_TXFP=DISABLE;	//���ȼ��ɱ��ı�ʶ������
	
	CAN_InitStructure.CAN_Mode= mode;//ģʽ����
	CAN_InitStructure.CAN_SJW=tsjw;	//����ͬ����Ծ����(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1=tbs1; //Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2=tbs2;//Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler=brp;  //��Ƶϵ��(Fdiv)Ϊbrp+1
	
	CAN_Init(CAN2, &CAN_InitStructure);// ��ʼ��CAN2
	
	//���ù�����
	//ʹ��CAN2�ü������
	CAN_SlaveStartBank(15);
	CAN_FilterInitStructure.CAN_FilterNumber=15;//������15
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;//32λ
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //���������0
	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
	

	
	CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);//FIFO0��Ϣ�����ж�����   
	
	
	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;// �����ȼ�Ϊ4
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;// �����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);

}

/*
***************************************************
��������BSP_CAN_Send_Msg
���ܣ�CAN����һ������
��ڲ�����	BSP_CANx��CAN��
					TxMessage������ָ��
����ֵ��	0���ɹ�;
				1��ʧ��
Ӧ�÷�Χ���ⲿ����
��ע��
***************************************************
*/
u8 BSP_CAN_Send_Msg(BSP_CAN_TypeDef *BSP_CANx,CanTxMsg *TxMessage){	
  u8 mbox;
  u16 i=0;        
  mbox= CAN_Transmit(BSP_CANx->CANx, TxMessage);   
	
	//�ȴ����ͽ���
  while(CAN_TransmitStatus(BSP_CANx->CANx, mbox)==CAN_TxStatus_Failed){
		i++;	
		if(i>=0xFFF)
			return 1;
	}
  return 0;		
}

/*
***************************************************
��������BSP_CAN_Receive_Msg
���ܣ�CAN����һ������
��ڲ�����	BSP_CANx��CAN��
					RxMessage����������ָ��
����ֵ��	0��������;
				������������
Ӧ�÷�Χ���ⲿ����
��ע��
***************************************************
*/
u8 BSP_CAN_Receive_Msg(BSP_CAN_TypeDef *BSP_CANx,CanRxMsg *RxMessage){		   		   
	if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)	//û�н��յ�����,ֱ���˳� 
		return 0;		
	CAN_Receive(BSP_CANx->CANx, CAN_FIFO0, RxMessage);//��ȡ����	
	return RxMessage->DLC;	
}