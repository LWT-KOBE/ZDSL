#include "application.h"
#include "usartx.h"
#include "board.h"
CAN1_TaskStruct_t CAN1Data;
Servo_MotorStruct_t SM = {0};
Servo_MotorDataRecv_t SM_Recv;
CANSendStruct_t can1data;

u8 TxMessage[13];

/*
***************************************************
��������CAN1SendData
���ܣ��������CAN����_����֡
��ڲ�����	CANx��CAN1 orCAN2
			ID_CAN��CANID ���0����1��ID��ַ �涨�� AXIS0_ID 0x001   AXIS1_ID 0x002
			CMD_CAN��odrive�ĸ�������  
			len������֡���� 
			CanSendData��CAN���͵����ݽṹ
����ֵ����
Ӧ�÷�Χ���ڲ�����
��ע�� �ú������͵�������֡���м�
***************************************************
*/

void CANSendData(CAN_TypeDef *CANx, uint32_t ID_CAN, uint8_t len,CANSendStruct_t* CanSendData) {
	CanTxMsg *txMessage;
	uint8_t mbox;
	uint8_t count;
	uint16_t i = 0;
	txMessage = (CanTxMsg*)aqCalloc(8,sizeof(CanTxMsg));
	
	//CAN ID 	
	txMessage->StdId = ID_CAN;	
	txMessage->IDE = CAN_Id_Standard;
	txMessage->RTR = CAN_RTR_Data;
	txMessage->DLC = len;
	for (count = 0; count < len; count++) {
		txMessage->Data[count] = (uint8_t)CanSendData->data[count];
	}
	mbox = CAN_Transmit(CANx, txMessage);
	while (CAN_TransmitStatus(CANx,mbox) == 0x00) {
		i++;
		if (i >= 0xFFF)break;
	}
	aqFree(txMessage,8,sizeof(CanTxMsg));
}

/*
***************************************************
��������CANSendInputVelData
���ܣ�����ٶȱջ�����_����֡
��ڲ�����	CANx��CAN1 orCAN2
			ID_CAN��CANID ���0����1��ID��ַ �涨�� AXIS0_ID 0x001   AXIS1_ID 0x002
			CMD_CAN��odrive���ٶȱջ����MSG_SET_INPUT_VEL ��
			len������֡���� 
			CanSendData��CAN���͵����ݽṹ
			Spetsnaz�����͵��ٶȿ��ƽṹ��
			axis: ѡ����0����1����� �涨 �� axis0=0  axis1=1 
����ֵ����
Ӧ�÷�Χ���ڲ�����
��ע��
	 
***************************************************
*/
void CANSendInputVelData(CAN_TypeDef *CANx, uint32_t ID_CAN, uint8_t len,Servo_MotorStruct_t* Spetsnaz,uint8_t axis,CANSendStruct_t* CanSendData) {

	CanSendData->data[0] = 0x23;
	CanSendData->data[1] = 0x81;
	CanSendData->data[2] = 0x60;
	CanSendData->data[3] = 0x00;
	
	CanSendData->data[4] = Spetsnaz->SetVel[axis].u8_temp[0];
	CanSendData->data[5] = Spetsnaz->SetVel[axis].u8_temp[1];
	CanSendData->data[6] = Spetsnaz->SetVel[axis].u8_temp[2];
	CanSendData->data[7] = Spetsnaz->SetVel[axis].u8_temp[3];
	
	CANSendData(CANx,ID_CAN,len,CanSendData); 
	vTaskDelay(5);
}

/*
***************************************************
��������CANSendAcceData
���ܣ�������ٶ��޸�����
��ڲ�����	CANx��CAN1 orCAN2
			ID_CAN��CANID ���0����1��ID��ַ �涨�� AXIS0_ID 0x001   AXIS1_ID 0x002
			CMD_CAN��odrive���ٶȱջ����MSG_SET_INPUT_VEL ��
			len������֡���� 
			CanSendData��CAN���͵����ݽṹ
			Spetsnaz�����͵��ٶȿ��ƽṹ��
			axis: ѡ����0����1����� �涨 �� axis0=0  axis1=1 
����ֵ����
Ӧ�÷�Χ���ڲ�����
��ע��
	 
***************************************************
*/
void CANSendAcceData(CAN_TypeDef *CANx, uint32_t ID_CAN, uint8_t len,Servo_MotorStruct_t* Spetsnaz,uint8_t axis,CANSendStruct_t* CanSendData) {

	CanSendData->data[0] = 0x2b;
	CanSendData->data[1] = 0x83;
	CanSendData->data[2] = 0x60;
	CanSendData->data[3] = 0x00;
	
	CanSendData->data[4] = Spetsnaz->Acce[axis].u8_temp[0];
	CanSendData->data[5] = Spetsnaz->Acce[axis].u8_temp[1];
	CanSendData->data[6] = 0x00;
	CanSendData->data[7] = 0x00;
	
	CANSendData(CANx,ID_CAN,len,CanSendData);
	vTaskDelay(5);
	
}


/*
***************************************************
��������CANSend_Torque
���ܣ�������ٶ��޸�����
��ڲ�����	CANx��CAN1 orCAN2
			ID_CAN��CANID ���0����1��ID��ַ �涨�� AXIS0_ID 0x001   AXIS1_ID 0x002
			CMD_CAN��odrive���ٶȱջ����MSG_SET_INPUT_VEL ��
			len������֡���� 
			CanSendData��CAN���͵����ݽṹ
			Spetsnaz�����͵��ٶȿ��ƽṹ��
			axis: ѡ����0����1����� �涨 �� axis0=0  axis1=1 
����ֵ����
Ӧ�÷�Χ���ڲ�����
��ע��
	 
***************************************************
*/
void CANSend_Torque(CAN_TypeDef *CANx, uint32_t ID_CAN, uint8_t len,Servo_MotorStruct_t* Spetsnaz,uint8_t axis,CANSendStruct_t* CanSendData) {

	CanSendData->data[0] = 0x2b;
	CanSendData->data[1] = 0x71;
	CanSendData->data[2] = 0x60;
	CanSendData->data[3] = 0x00;
	
	CanSendData->data[4] = Spetsnaz->Acce[axis].u8_temp[0];
	CanSendData->data[5] = Spetsnaz->Acce[axis].u8_temp[1];
	CanSendData->data[6] = 0x00;
	CanSendData->data[7] = 0x00;
	
	CANSendData(CANx,ID_CAN,len,CanSendData);
	vTaskDelay(5);
	
}



/*
***************************************************
��������CANSend_Slope
���ܣ�������ٶ��޸�����
��ڲ�����	CANx��CAN1 orCAN2
			ID_CAN��CANID ���0����1��ID��ַ �涨�� AXIS0_ID 0x001   AXIS1_ID 0x002
			CMD_CAN��odrive���ٶȱջ����MSG_SET_INPUT_VEL ��
			len������֡���� 
			CanSendData��CAN���͵����ݽṹ
			Spetsnaz�����͵��ٶȿ��ƽṹ��
			axis: ѡ����0����1����� �涨 �� axis0=0  axis1=1 
����ֵ����
Ӧ�÷�Χ���ڲ�����
��ע��
	 
***************************************************
*/
void CANSend_Slope(CAN_TypeDef *CANx, uint32_t ID_CAN, uint8_t len,Servo_MotorStruct_t* Spetsnaz,uint8_t axis,CANSendStruct_t* CanSendData) {

	CanSendData->data[0] = 0x2b;
	CanSendData->data[1] = 0x87;
	CanSendData->data[2] = 0x60;
	CanSendData->data[3] = 0x00;
	
	CanSendData->data[4] = Spetsnaz->Acce[axis].u8_temp[0];
	CanSendData->data[5] = Spetsnaz->Acce[axis].u8_temp[1];
	CanSendData->data[6] = 0x00;
	CanSendData->data[7] = 0x00;
	
	CANSendData(CANx,ID_CAN,len,CanSendData);
	vTaskDelay(5);
	
}



/*
***************************************************
��������CANSendAccData
���ܣ�������ٶ��޸�����
��ڲ�����	CANx��CAN1 orCAN2
			ID_CAN��CANID ���0����1��ID��ַ �涨�� AXIS0_ID 0x001   AXIS1_ID 0x002
			CMD_CAN��odrive���ٶȱջ����MSG_SET_INPUT_VEL ��
			len������֡���� 
			CanSendData��CAN���͵����ݽṹ
			Spetsnaz�����͵��ٶȿ��ƽṹ��
			axis: ѡ����0����1����� �涨 �� axis0=0  axis1=1 
����ֵ����
Ӧ�÷�Χ���ڲ�����
��ע��
	 
***************************************************
*/
void CANSendDeceData(CAN_TypeDef *CANx, uint32_t ID_CAN, uint8_t len,Servo_MotorStruct_t* Spetsnaz,uint8_t axis,CANSendStruct_t* CanSendData) {
	
	CanSendData->data[0] = 0x2b;
	CanSendData->data[1] = 0x84;
	CanSendData->data[2] = 0x60;
	CanSendData->data[3] = 0x00;
	
	CanSendData->data[4] = Spetsnaz->Dece[axis].u8_temp[0];
	CanSendData->data[5] = Spetsnaz->Dece[axis].u8_temp[1];
	CanSendData->data[6] = 0x00;
	CanSendData->data[7] = 0x00;
	
	CANSendData(CANx,ID_CAN,len,CanSendData); 
	
}


void Set_Pos_mode(CAN_TypeDef *CANx, uint32_t ID_CAN, uint8_t len,CANSendStruct_t* CanSendData){
	
	u8 p_mode_set[8]= {0x2f, 0x60, 0x60, 0x00, 0x01, 0x00, 0x00, 0x00};
	
	//����λ��ģʽ
	for(int i = 0;i < 8;i++){
		CanSendData->data[i] = p_mode_set[i];
	}
	CANSendData(CANx,ID_CAN,len,CanSendData);
	vTaskDelay(5);
}

void Set_Vel_mode(CAN_TypeDef *CANx, uint32_t ID_CAN, uint8_t len,CANSendStruct_t* CanSendData){
	
	u8 v_mode_set[8]= {0x2f, 0x60, 0x60, 0x00, 0x03, 0x00, 0x00, 0x00};
	
	//�����ٶ�ģʽ
	for(int i = 0;i < 8;i++){
		CanSendData->data[i] = v_mode_set[i];
	}
	CANSendData(CANx,ID_CAN,len,CanSendData);
	vTaskDelay(5);
}



void Set_Cur_mode(CAN_TypeDef *CANx, uint32_t ID_CAN, uint8_t len,CANSendStruct_t* CanSendData){
	
	u8 c_mode_set[8]= {0x2f, 0x60, 0x60, 0x00, 0x04, 0x00, 0x00, 0x00};
	
	//��������ģʽ
	for(int i = 0;i < 8;i++){
		CanSendData->data[i] = c_mode_set[i];
	}
	CANSendData(CANx,ID_CAN,len,CanSendData);
	vTaskDelay(5);
}


//ʹ�ܵ��
void Enable(CAN_TypeDef *CANx, uint32_t ID_CAN, uint8_t len,CANSendStruct_t* CanSendData){
	
	u8 enable1[8]= {0x2b, 0x40, 0x60, 0x00, 0x01, 0x00, 0x00, 0x00};
	u8 enable2[8]= {0x2b, 0x40, 0x60, 0x00, 0x03, 0x00, 0x00, 0x00};
	u8 enable3[8]= {0x2b, 0x40, 0x60, 0x00, 0x0f, 0x00, 0x00, 0x00};
	
	//��ʼ���豸
	for(int i = 0;i < 8;i++){
		CanSendData->data[i] = enable1[i];
	}
	CANSendData(CANx,ID_CAN,len,CanSendData);
	vTaskDelay(5);
	
	//��ɲ��
	for(int i = 0;i < 8;i++){
		CanSendData->data[i] = enable2[i];
	}
	CANSendData(CANx,ID_CAN,len,CanSendData);
	vTaskDelay(5);
	
	//ʹ�ܵ��
	for(int i = 0;i < 8;i++){
		CanSendData->data[i] = enable3[i];
	}
	CANSendData(CANx,ID_CAN,len,CanSendData);
	vTaskDelay(5);
	
}

//ֹͣ
void Stop(CAN_TypeDef *CANx, uint32_t ID_CAN, uint8_t len,CANSendStruct_t* CanSendData){
	
	u8 Stop[8]= {0x2b, 0x40, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00};
	
	
	for(int i = 0;i < 8;i++){
		CanSendData->data[i] = Stop[i];
	}
	CANSendData(CANx,ID_CAN,len,CanSendData);
	vTaskDelay(5);
}

//�޸�ת��б��
void Cur_Target_Torque(CAN_TypeDef *CANx, uint32_t ID_CAN, uint8_t len,CANSendStruct_t* CanSendData){
	
	u8 Stop[8]= {0x2b, 0x40, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00};
	
	
	for(int i = 0;i < 8;i++){
		CanSendData->data[i] = Stop[i];
	}
	CANSendData(CANx,ID_CAN,len,CanSendData);
	vTaskDelay(5);
}

//CANOpenNMTʹ��
void CIA301_Enable(CAN_TypeDef *CANx,CANSendStruct_t* CanSendData){
	
	u8 cia301_enable[2] = {0x01,0x00};
	//**** CIA301 ״̬�� �������״̬****
	for(int i = 0;i < 2;i++){
		CanSendData->data[i] = cia301_enable[i];
	}
	CANSendData(CANx,000,2,CanSendData);
	vTaskDelay(50);
}


//CANOpenNMTֹͣ
void CIA301_Stop(CAN_TypeDef *CANx,CANSendStruct_t* CanSendData){
	
	u8 cia301_stop[2] = {0x02,0x00};
	//**** CIA301 ״̬�� ֹͣԶ�̽ڵ�****
	for(int i = 0;i < 2;i++){
		CanSendData->data[i] = cia301_stop[i];
	}
	CANSendData(CANx,000,2,CanSendData);
	vTaskDelay(50);
}

//CANOpenNMT����Ԥ����
void CIA301_yucaozuo(CAN_TypeDef *CANx,CANSendStruct_t* CanSendData){
	
	u8 cia301_yucaozuo[2] = {0x80,0x00};
	//**** CIA301 ״̬�� ����Ԥ����״̬****
	for(int i = 0;i < 2;i++){
		CanSendData->data[i] = cia301_yucaozuo[i];
	}
	CANSendData(CANx,000,2,CanSendData);
	vTaskDelay(50);
}

//CANOpenNMT�������ýڵ�
void CIA301_Rebot(CAN_TypeDef *CANx,CANSendStruct_t* CanSendData){
	
	u8 cia301_rebot[2] = {0x81,0x00};
	//**** CIA301 ״̬�� �������ýڵ�****
	for(int i = 0;i < 2;i++){
		CanSendData->data[i] = cia301_rebot[i];
	}
	CANSendData(CANx,000,2,CanSendData);
	vTaskDelay(50);
}


//CANOpenNMT��������ͨ��
void CIA301_Rest(CAN_TypeDef *CANx,CANSendStruct_t* CanSendData){
	
	u8 cia301_rest[2] = {0x82,0x00};
	//**** CIA301 ״̬�� ��������ͨ��****
	for(int i = 0;i < 2;i++){
		CanSendData->data[i] = cia301_rest[i];
	}
	CANSendData(CANx,000,2,CanSendData);
	vTaskDelay(50);
}

//����PDO_ͨѶ����0
void Set_PDO_Vel_Pos(CAN_TypeDef *CANx, uint32_t ID_CAN, uint8_t len,CANSendStruct_t* CanSendData){
	
//	u8 CIA301_Enable[2] = {0x01,0x00};
	
	u8 PDO1[8]= {0x23, 0x00, 0x18, 0x01, 0x81, 0x01, 0x00, 0x00};	//����TPD0ͨѶ������COB_ID��0x181
	u8 PDO2[8]= {0x2f, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00};   //�������������
	u8 PDO3[8]= {0x23, 0x00, 0x1a, 0x01, 0x20, 0x00, 0x6c, 0x60};   //ӳ�����1:ʵ���ٶ�
	u8 PDO4[8]= {0x23, 0x00, 0x1a, 0x02, 0x20, 0x00, 0x64, 0x60};   //ӳ�����2:ʵ��λ��
	u8 PDO5[8]= {0x2f, 0x00, 0x1a, 0x00, 0x02, 0x00, 0x00, 0x00};   //����������2
	u8 PDO6[8]= {0x2f, 0x00, 0x18, 0x02, 0xfe, 0x00, 0x00, 0x00};   //��������:0XFE(254)�첽����,�����¼��趨ʱ�䷵��PDO����
	u8 PDO7[8]= {0x2b, 0x00, 0x18, 0x05, 0x0a, 0x00, 0x00, 0x00};   //�¼�ʱ��10ms
	
	//���ݽڵ�ID����PDO�ش�ID
	switch(ID_CAN){
		case 1537:PDO1[4] = 0x81;break;
		case 1538:PDO1[4] = 0x82;break;
		case 1539:PDO1[4] = 0x83;break;
		
		default:	break;
	}
	
	//**** CIA301 ״̬�� Ԥ����������״̬****
//	for(int i = 0;i < 2;i++){
//		CanSendData->data[i] = CIA301_Enable[i];
//	}
//	CANSendData(CANx,000,2,CanSendData);
//	vTaskDelay(50);
	
	// ����PDOͨѶ������COB_ID
	for(int i = 0;i < 8;i++){
		CanSendData->data[i] = PDO1[i];
	}
	CANSendData(CANx,ID_CAN,len,CanSendData);
	vTaskDelay(50);
	
	// �������������
	for(int i = 0;i < 8;i++){
		CanSendData->data[i] = PDO2[i];
	}
	CANSendData(CANx,ID_CAN,len,CanSendData);
	vTaskDelay(50);
	
	// ӳ�����1��ʵ���ٶȣ�0x606C
	for(int i = 0;i < 8;i++){
		CanSendData->data[i] = PDO3[i];
	}
	CANSendData(CANx,ID_CAN,len,CanSendData);
	vTaskDelay(50);
	
	// ӳ�����2��ʵ��λ�ã�0x6064
	for(int i = 0;i < 8;i++){
		CanSendData->data[i] = PDO4[i];
	}
	CANSendData(CANx,ID_CAN,len,CanSendData);
	vTaskDelay(50);
	
	// ����������2
	for(int i = 0;i < 8;i++){
		CanSendData->data[i] = PDO5[i];
	}
	CANSendData(CANx,ID_CAN,len,CanSendData);
	vTaskDelay(50);
	
	// �������ͣ�0xFE��254���첽���䣬�����¼��趨ʱ�䷵��PDO����
	for(int i = 0;i < 8;i++){
		CanSendData->data[i] = PDO6[i];
	}
	CANSendData(CANx,ID_CAN,len,CanSendData);
	vTaskDelay(50);
	
	// ����10ms�ش�
	for(int i = 0;i < 8;i++){
		CanSendData->data[i] = PDO7[i];
	}
	CANSendData(CANx,ID_CAN,len,CanSendData);
	
	vTaskDelay(50);
	
}


//����PDO_ͨѶ����1
void Set_PDO_Cur(CAN_TypeDef *CANx, uint32_t ID_CAN, uint8_t len,CANSendStruct_t* CanSendData){
	
//	u8 CIA301_Enable[2] = {0x01,0x00};
	
	u8 PDO1[8]= {0x23, 0x01, 0x18, 0x02, 0x81, 0x01, 0x00, 0x00};   //����TPD1ͨѶ������COB_ID��0x281
	u8 PDO2[8]= {0x2f, 0x01, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00};	//�������������
	u8 PDO3[8]= {0x23, 0x01, 0x1a, 0x01, 0x10, 0x00, 0x77, 0x60};	//ӳ�����1:ʵ������
	u8 PDO4[8]= {0x23, 0x01, 0x1a, 0x02, 0x10, 0x00, 0x41, 0x60};	//ӳ�����2:״̬��
	u8 PDO5[8]= {0x2f, 0x01, 0x1a, 0x00, 0x02, 0x00, 0x00, 0x00};	//����������2
	u8 PDO6[8]= {0x2f, 0x01, 0x18, 0x02, 0xfe, 0x00, 0x00, 0x00};	//��������:0XFE(254)�첽����,�����¼��趨ʱ�䷵��PDO����
	u8 PDO7[8]= {0x2b, 0x01, 0x18, 0x05, 0x0a, 0x00, 0x00, 0x00};	//�¼�ʱ��10ms
	
	//���ݽڵ�ID����PDO�ش�ID
	switch(ID_CAN){
		case 1537:PDO1[4] = 0x81;break;
		case 1538:PDO1[4] = 0x82;break;
		case 1539:PDO1[4] = 0x83;break;
		
		default:	break;
	}
	
//	//**** CIA301 ״̬�� Ԥ����������״̬****
//	for(int i = 0;i < 2;i++){
//		CanSendData->data[i] = CIA301_Enable[i];
//	}
//	CANSendData(CANx,000,2,CanSendData);
//	vTaskDelay(50);
	
	// ����PDOͨѶ������COB_ID
	for(int i = 0;i < 8;i++){
		CanSendData->data[i] = PDO1[i];
	}
	CANSendData(CANx,ID_CAN,len,CanSendData);
	vTaskDelay(50);
	
	// �������������
	for(int i = 0;i < 8;i++){
		CanSendData->data[i] = PDO2[i];
	}
	CANSendData(CANx,ID_CAN,len,CanSendData);
	vTaskDelay(50);
	
	// ӳ�����1��ʵ���ٶȣ�0x606C
	for(int i = 0;i < 8;i++){
		CanSendData->data[i] = PDO3[i];
	}
	CANSendData(CANx,ID_CAN,len,CanSendData);
	vTaskDelay(50);
	
	// ӳ�����2��ʵ��λ�ã�0x6064
	for(int i = 0;i < 8;i++){
		CanSendData->data[i] = PDO4[i];
	}
	CANSendData(CANx,ID_CAN,len,CanSendData);
	vTaskDelay(50);
	
	// ����������2
	for(int i = 0;i < 8;i++){
		CanSendData->data[i] = PDO5[i];
	}
	CANSendData(CANx,ID_CAN,len,CanSendData);
	vTaskDelay(50);
	
	// �������ͣ�0xFE��254���첽���䣬�����¼��趨ʱ�䷵��PDO����
	for(int i = 0;i < 8;i++){
		CanSendData->data[i] = PDO6[i];
	}
	CANSendData(CANx,ID_CAN,len,CanSendData);
	vTaskDelay(50);
	
	// ����10ms�ش�
	for(int i = 0;i < 8;i++){
		CanSendData->data[i] = PDO7[i];
	}
	CANSendData(CANx,ID_CAN,len,CanSendData);
	
	vTaskDelay(50);
	
}


/*
***************************************************
��������ReadVel_Pos
���ܣ���ȡʵʱ�ٶȡ�λ��
��ڲ�����	
			CanSendData��CAN���͵����ݽṹ
			Spetsnaz������CANOPen�ŷ�������ٶȡ�λ�����ݽṹ��
			axis: ѡ����0����1����� �涨 �� axis0=0  axis1=1 
����ֵ����
Ӧ�÷�Χ���ڲ�����
***************************************************
*/
void ReadVel_Pos(CanRxMsg* CanRevData,Servo_MotorDataRecv_t* Spetsnaz,uint8_t axis) {
		
	
	Spetsnaz->vel_estimate[axis].u8_temp[0] = CanRevData->Data[0];	
	Spetsnaz->vel_estimate[axis].u8_temp[1] = CanRevData->Data[1];	
	Spetsnaz->vel_estimate[axis].u8_temp[2] = CanRevData->Data[2];	
	Spetsnaz->vel_estimate[axis].u8_temp[3] = CanRevData->Data[3];
	
	Spetsnaz->pos_estimate[axis].u8_temp[0] = CanRevData->Data[4];
	Spetsnaz->pos_estimate[axis].u8_temp[1] = CanRevData->Data[5];
	Spetsnaz->pos_estimate[axis].u8_temp[2] = CanRevData->Data[6];
	Spetsnaz->pos_estimate[axis].u8_temp[3] = CanRevData->Data[7];
}

//��ȡ����
void ReadCur(CanRxMsg* CanRevData,Servo_MotorDataRecv_t* Spetsnaz,uint8_t axis) {
		
	
	Spetsnaz->Iq_measured[axis].u8_temp[0] = CanRevData->Data[0];	
	Spetsnaz->Iq_measured[axis].u8_temp[1] = CanRevData->Data[1];
}


//��ȡCANOpen�������
void Read_NMT(CanRxMsg* CanRevData,Servo_MotorDataRecv_t* Spetsnaz,uint8_t axis){
	Spetsnaz->NMT[axis].u8_temp[0]	= CanRevData->Data[0];
	Spetsnaz->NMT[axis].u8_temp[1]	= CanRevData->Data[1];
}

CAN1_TaskStruct_t* getCAN1_Task(){
    return &CAN1Data;
}

/* ���ó�ʼ�� */
void CAN2_TaskGlobalInit(void){
	//��ʼ��CAN2 ������125K
	//CAN2_Mode_Init(CAN_SJW_1tq,CAN_BS2_5tq,CAN_BS1_9tq,24,CAN_Mode_Normal);
	//��ʼ��CAN2 ������250K
	//CAN2_Mode_Init(CAN_SJW_1tq,CAN_BS2_5tq,CAN_BS1_9tq,12,CAN_Mode_Normal);
	//��ʼ��CAN2 ������1M
	//CAN2_Mode_Init(CAN_SJW_1tq,CAN_BS2_5tq,CAN_BS1_9tq,3,CAN_Mode_Normal);
	
	uart1_init(115200);
}

void Send_Message(void){
	//�̶�֡ͷ
	TxMessage[0] = 0xaa;
	TxMessage[1] = 0x55;
	
	//���ݳ���
	TxMessage[2] = 0x08;
	
//	//�����ŷ����ʹ�ܺž������͵�����,ǰ3~6λΪ��ǰ�ٶ�,7~8λΪNMT�ش��ź�,9~10λΪ��ǰ����
//	if(systemConfigData.SM_Enable[0] != 0){
//		TxMessage[3] = SM_Recv.vel_estimate[0].u8_temp[0];
//		TxMessage[4] = SM_Recv.vel_estimate[0].u8_temp[1];
//		TxMessage[5] = SM_Recv.vel_estimate[0].u8_temp[2];
//		TxMessage[6] = SM_Recv.vel_estimate[0].u8_temp[3];
//		
//		TxMessage[7] = SM_Recv.NMT[0].u8_temp[0];
//		TxMessage[8] = SM_Recv.NMT[0].u8_temp[1];
//		TxMessage[9] = SM_Recv.Iq_measured[0].u8_temp[0];
//		TxMessage[10] = SM_Recv.Iq_measured[0].u8_temp[1];
//	}
//	
//	if(systemConfigData.SM_Enable[1] != 0){
//		TxMessage[3] = SM_Recv.vel_estimate[1].u8_temp[0];
//		TxMessage[4] = SM_Recv.vel_estimate[1].u8_temp[1];
//		TxMessage[5] = SM_Recv.vel_estimate[1].u8_temp[2];
//		TxMessage[6] = SM_Recv.vel_estimate[1].u8_temp[3];
//		
//		TxMessage[7] = SM_Recv.NMT[1].u8_temp[0];
//		TxMessage[8] = SM_Recv.NMT[1].u8_temp[1];
//		TxMessage[9] = SM_Recv.Iq_measured[1].u8_temp[0];
//		TxMessage[10] = SM_Recv.Iq_measured[1].u8_temp[1];
//	}
//	
//	if(systemConfigData.SM_Enable[2] != 0){
//		TxMessage[3] = SM_Recv.vel_estimate[2].u8_temp[0];
//		TxMessage[4] = SM_Recv.vel_estimate[2].u8_temp[1];
//		TxMessage[5] = SM_Recv.vel_estimate[2].u8_temp[2];
//		TxMessage[6] = SM_Recv.vel_estimate[2].u8_temp[3];
//		
//		TxMessage[7] = SM_Recv.NMT[2].u8_temp[0];
//		TxMessage[8] = SM_Recv.NMT[2].u8_temp[1];
//		TxMessage[9] = SM_Recv.Iq_measured[2].u8_temp[0];
//		TxMessage[10] = SM_Recv.Iq_measured[2].u8_temp[1];
//	}
	
	TxMessage[3] = SM_Recv.vel_estimate[1].u8_temp[0];
	TxMessage[4] = SM_Recv.vel_estimate[1].u8_temp[1];
	TxMessage[5] = SM_Recv.vel_estimate[1].u8_temp[2];
	TxMessage[6] = SM_Recv.vel_estimate[1].u8_temp[3];
	
	TxMessage[7] = SM_Recv.NMT[1].u8_temp[0];
	TxMessage[8] = SM_Recv.NMT[1].u8_temp[1];
	TxMessage[9] = SM_Recv.Iq_measured[1].u8_temp[0];
	TxMessage[10] = SM_Recv.Iq_measured[1].u8_temp[1];
	
	//�̶�֡β
	TxMessage[11] = 0x55;
	TxMessage[12] = 0xaa;
	
	//��������
	u1_SendArray(TxMessage,13);
	//��շ�������
	memset(TxMessage, 0, sizeof(TxMessage));
}



//�����Զ�������
void RESET_SM(void){
	int i = 0;
	//�������1�����2
	if(SM.Rebot_flag){                                              						
//		//config
		CIA301_Rebot(CAN1,&can1data);
//		CIA301_Rebot(CAN1,&can1data);
//		CIA301_Rebot(CAN1,&can1data);
//		
//		i++;
//            //���п���ȫ����ʼ��            
//			//CAN2_TaskGlobalInit();
//			if(i < 3){
//				if(systemConfigData.SM_Enable[0] || systemConfigData.SM_Enable[1] || systemConfigData.SM_Enable[2]){
//					CIA301_Enable(CAN1,&can1data);
//					CIA301_Enable(CAN1,&can1data);
//					CIA301_Enable(CAN1,&can1data);
//				}
//				
//				//��ʼ��POD���ã�Ĭ�ϻش�ʵʱ�ٶ���λ��
//				if(systemConfigData.SM_Enable[0])
//				Set_PDO_Vel_Pos(CAN1,Servo_Motor_ID0,8,&can1data);
//				if(systemConfigData.SM_Enable[1])
//				Set_PDO_Vel_Pos(CAN1,Servo_Motor_ID1,8,&can1data);
//				if(systemConfigData.SM_Enable[2])
//				Set_PDO_Vel_Pos(CAN1,Servo_Motor_ID2,8,&can1data);
//				
//				//��ʼ��PDO���ã�Ĭ�ϻش�ʵʱ������״̬��
//				if(systemConfigData.SM_Enable[0])
//				Set_PDO_Cur(CAN1,Servo_Motor_ID0,8,&can1data);
//				if(systemConfigData.SM_Enable[1])
//				Set_PDO_Cur(CAN1,Servo_Motor_ID1,8,&can1data);
//				if(systemConfigData.SM_Enable[2])
//				Set_PDO_Cur(CAN1,Servo_Motor_ID2,8,&can1data);
//			}
//			if(i > 3 && i < 6){
//				//ʹ�ܵ��
//				Enable(CAN1,Servo_Motor_ID0,8,&can1data);
//				Enable(CAN1,Servo_Motor_ID1,8,&can1data);
//				Enable(CAN1,Servo_Motor_ID2,8,&can1data);
//			}
//			if(i > 6){
//				//�����ٶ�ģʽ
//				if(systemConfigData.SM_Enable[0])
//				Set_Vel_mode(CAN1,Servo_Motor_ID0,8,&can1data);
//				if(systemConfigData.SM_Enable[1])
//				Set_Vel_mode(CAN1,Servo_Motor_ID1,8,&can1data);
//				if(systemConfigData.SM_Enable[2])
//				Set_Vel_mode(CAN1,Servo_Motor_ID2,8,&can1data);
//				
//				//���ü��ٶ�
//				if(systemConfigData.SM_Enable[0])
//				CANSendDeceData(CAN1,Servo_Motor_ID0,8,&SM,0,&can1data);
//				if(systemConfigData.SM_Enable[1])
//				CANSendDeceData(CAN1,Servo_Motor_ID1,8,&SM,0,&can1data);
//				if(systemConfigData.SM_Enable[2])
//				CANSendDeceData(CAN1,Servo_Motor_ID2,8,&SM,0,&can1data);
//				
//				//���ü��ٶ�
//				if(systemConfigData.SM_Enable[0])
//				CANSendAcceData(CAN1,Servo_Motor_ID0,8,&SM,0,&can1data);
//				if(systemConfigData.SM_Enable[1])
//				CANSendAcceData(CAN1,Servo_Motor_ID1,8,&SM,0,&can1data);
//				if(systemConfigData.SM_Enable[2])
//				CANSendAcceData(CAN1,Servo_Motor_ID2,8,&SM,0,&can1data);
//				
//			}
//			if(i == 9)
			//SM.Rebot_flag = 0;
		//digitalLo(&getCAN1_Task()->dataInitFlag);
		digitalLo(&SM.Rebot_flag);         
        
	}
}


void CAN1_TaskUpdateTask(void *Parameters){
	TickType_t xLastWakeTime = xTaskGetTickCount();
	digitalLo(&getCAN1_Task()->dataInitFlag);
	int i = 0,j = 0;
	while(true){
		vTaskDelayUntil(&xLastWakeTime,CAN1_Task_NORMAL_PERIOD);
		//�����ŷ�������ٶȺͼ��ٶ�
		SM.Acce[0].s16_temp = 1000;
		SM.Dece[0].s16_temp = 1000;
		
		
        //��ֹ�ظ���ʼ��
		if(CAN1Data.dataInitFlag == 0){
			i++;
            //���п���ȫ����ʼ��            
			CAN2_TaskGlobalInit();
			if(i < 3){
				if(systemConfigData.SM_Enable[0] || systemConfigData.SM_Enable[1] || systemConfigData.SM_Enable[2]){
					CIA301_Enable(CAN1,&can1data);
					CIA301_Enable(CAN1,&can1data);
					CIA301_Enable(CAN1,&can1data);
				}
				
				//��ʼ��POD���ã�Ĭ�ϻش�ʵʱ�ٶ���λ��
				if(systemConfigData.SM_Enable[0])
				Set_PDO_Vel_Pos(CAN1,Servo_Motor_ID0,8,&can1data);
				if(systemConfigData.SM_Enable[1])
				Set_PDO_Vel_Pos(CAN1,Servo_Motor_ID1,8,&can1data);
				if(systemConfigData.SM_Enable[2])
				Set_PDO_Vel_Pos(CAN1,Servo_Motor_ID2,8,&can1data);
				
				//��ʼ��PDO���ã�Ĭ�ϻش�ʵʱ������״̬��
				if(systemConfigData.SM_Enable[0])
				Set_PDO_Cur(CAN1,Servo_Motor_ID0,8,&can1data);
				if(systemConfigData.SM_Enable[1])
				Set_PDO_Cur(CAN1,Servo_Motor_ID1,8,&can1data);
				if(systemConfigData.SM_Enable[2])
				Set_PDO_Cur(CAN1,Servo_Motor_ID2,8,&can1data);
			}
			if(i > 3 && i < 6){
				//ʹ�ܵ��
				Enable(CAN1,Servo_Motor_ID0,8,&can1data);
				Enable(CAN1,Servo_Motor_ID1,8,&can1data);
				Enable(CAN1,Servo_Motor_ID2,8,&can1data);
			}
			if(i > 6){
				//�����ٶ�ģʽ
				if(systemConfigData.SM_Enable[0])
				Set_Vel_mode(CAN1,Servo_Motor_ID0,8,&can1data);
				if(systemConfigData.SM_Enable[1])
				Set_Vel_mode(CAN1,Servo_Motor_ID1,8,&can1data);
				if(systemConfigData.SM_Enable[2])
				Set_Vel_mode(CAN1,Servo_Motor_ID2,8,&can1data);
				
				//���ü��ٶ�
				if(systemConfigData.SM_Enable[0])
				CANSendDeceData(CAN1,Servo_Motor_ID0,8,&SM,0,&can1data);
				if(systemConfigData.SM_Enable[1])
				CANSendDeceData(CAN1,Servo_Motor_ID1,8,&SM,0,&can1data);
				if(systemConfigData.SM_Enable[2])
				CANSendDeceData(CAN1,Servo_Motor_ID2,8,&SM,0,&can1data);
				
				//���ü��ٶ�
				if(systemConfigData.SM_Enable[0])
				CANSendAcceData(CAN1,Servo_Motor_ID0,8,&SM,0,&can1data);
				if(systemConfigData.SM_Enable[1])
				CANSendAcceData(CAN1,Servo_Motor_ID1,8,&SM,0,&can1data);
				if(systemConfigData.SM_Enable[2])
				CANSendAcceData(CAN1,Servo_Motor_ID2,8,&SM,0,&can1data);
				
			}
			if(i == 9)
			digitalHi(&getCAN1_Task()->dataInitFlag);
			
		}
		
		
		
		//�����ٶȿ���ָ��
		if(systemConfigData.SM_Enable[0])
		CANSendInputVelData(CAN1,Servo_Motor_ID0,8,&SM,0,&can1data);
		if(systemConfigData.SM_Enable[1])
		CANSendInputVelData(CAN1,Servo_Motor_ID1,8,&SM,1,&can1data);
		if(systemConfigData.SM_Enable[2])
		CANSendInputVelData(CAN1,Servo_Motor_ID2,8,&SM,2,&can1data);
		
		
		
		//100Hz  ��ѭ���͡�
		if(!((CAN1Data.loops + 1) % 50)){	
			
			//�����豸
			RESET_SM();
			
			//��������
			Send_Message();
			
//			if(SM_Recv.NMT[1].u8_temp[0] == 0x7f){
//				j++;
//				i = 0;
//				if(j >= 8){
////					digitalLo(&getCAN1_Task()->dataInitFlag);
//					i++;
//            //���п���ȫ����ʼ��            
//			CAN2_TaskGlobalInit();
//			if(i < 3){
//				if(systemConfigData.SM_Enable[0] || systemConfigData.SM_Enable[1] || systemConfigData.SM_Enable[2]){
//					CIA301_Enable(CAN1,&can1data);
//					CIA301_Enable(CAN1,&can1data);
//					CIA301_Enable(CAN1,&can1data);
//				}
//				
//				//��ʼ��POD���ã�Ĭ�ϻش�ʵʱ�ٶ���λ��
//				if(systemConfigData.SM_Enable[0])
//				Set_PDO_Vel_Pos(CAN1,Servo_Motor_ID0,8,&can1data);
//				if(systemConfigData.SM_Enable[1])
//				Set_PDO_Vel_Pos(CAN1,Servo_Motor_ID1,8,&can1data);
//				if(systemConfigData.SM_Enable[2])
//				Set_PDO_Vel_Pos(CAN1,Servo_Motor_ID2,8,&can1data);
//				
//				//��ʼ��PDO���ã�Ĭ�ϻش�ʵʱ������״̬��
//				if(systemConfigData.SM_Enable[0])
//				Set_PDO_Cur(CAN1,Servo_Motor_ID0,8,&can1data);
//				if(systemConfigData.SM_Enable[1])
//				Set_PDO_Cur(CAN1,Servo_Motor_ID1,8,&can1data);
//				if(systemConfigData.SM_Enable[2])
//				Set_PDO_Cur(CAN1,Servo_Motor_ID2,8,&can1data);
//			}
//			if(i > 3 && i < 6){
//				//ʹ�ܵ��
//				Enable(CAN1,Servo_Motor_ID0,8,&can1data);
//				Enable(CAN1,Servo_Motor_ID1,8,&can1data);
//				Enable(CAN1,Servo_Motor_ID2,8,&can1data);
//			}
//			if(i > 6){
//				//�����ٶ�ģʽ
//				if(systemConfigData.SM_Enable[0])
//				Set_Vel_mode(CAN1,Servo_Motor_ID0,8,&can1data);
//				if(systemConfigData.SM_Enable[1])
//				Set_Vel_mode(CAN1,Servo_Motor_ID1,8,&can1data);
//				if(systemConfigData.SM_Enable[2])
//				Set_Vel_mode(CAN1,Servo_Motor_ID2,8,&can1data);
//				
//				//���ü��ٶ�
//				if(systemConfigData.SM_Enable[0])
//				CANSendDeceData(CAN1,Servo_Motor_ID0,8,&SM,0,&can1data);
//				if(systemConfigData.SM_Enable[1])
//				CANSendDeceData(CAN1,Servo_Motor_ID1,8,&SM,0,&can1data);
//				if(systemConfigData.SM_Enable[2])
//				CANSendDeceData(CAN1,Servo_Motor_ID2,8,&SM,0,&can1data);
//				
//				//���ü��ٶ�
//				if(systemConfigData.SM_Enable[0])
//				CANSendAcceData(CAN1,Servo_Motor_ID0,8,&SM,0,&can1data);
//				if(systemConfigData.SM_Enable[1])
//				CANSendAcceData(CAN1,Servo_Motor_ID1,8,&SM,0,&can1data);
//				if(systemConfigData.SM_Enable[2])
//				CANSendAcceData(CAN1,Servo_Motor_ID2,8,&SM,0,&can1data);
//				
//			}
//			if(i == 9)
//					j = 0;
//				}
//			}
		}
		
		
		digitalIncreasing(&getCAN1_Task()->loops);        
		
		
		
		
	}
}



//     Frame
// nodeID
CanRxMsg can1_rx_msg;
u32 rxbuf3;
void CAN1_RX0_IRQHandler(void){
	//CanRxMsg can1_rx_msg;
	if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET){
		// ����жϱ�־�ͱ�־λ
		CAN_ClearITPendingBit(CAN1, CAN_IT_FF0);
		CAN_ClearFlag(CAN1, CAN_FLAG_FF0);
		
		// �ӽ��� FIFO �ж�ȡ��Ϣ		
		CAN_Receive(CAN1, CAN_FIFO0, &can1_rx_msg);
		
		// �洢���յ��ı�׼ ID
		rxbuf3=can1_rx_msg.StdId;
		// �����������
		
		digitalIncreasing(&OdriveData.OdError.errorCount);

		/*********�������Զ��岿��**********/
		
		switch(can1_rx_msg.StdId){
			
			//��ȡCANOPen�������ڵ��ź�
			case 1793:
				Read_NMT(&can1_rx_msg,&SM_Recv,0);
				break;
			case 1794:
				Read_NMT(&can1_rx_msg,&SM_Recv,1);
				break;
			case 1795:
				Read_NMT(&can1_rx_msg,&SM_Recv,2);
				break;
			
			//��ȡ�ٶȺ�λ��
			case 385:
				ReadVel_Pos(&can1_rx_msg,&SM_Recv,0);
			break;
			case 386:
				ReadVel_Pos(&can1_rx_msg,&SM_Recv,1);
			break;
			case 387:
				ReadVel_Pos(&can1_rx_msg,&SM_Recv,2);
			break;							
			
			//��ȡ״̬�ֺ͵���
			case 641:
				ReadCur(&can1_rx_msg,&SM_Recv,0);
			break;
			
			case 642:
				ReadCur(&can1_rx_msg,&SM_Recv,1);
			break;
			
			case 643:
				ReadCur(&can1_rx_msg,&SM_Recv,2);
			break;
				
			default:	break;
		}

	}
}


/*
***************************************************
��������CAN1_TX_IRQHandler
���ܣ�CAN1�����ж�
��ע��
***************************************************
*/
void CAN1_TX_IRQHandler(void){
	if (CAN_GetITStatus(CAN1,CAN_IT_TME)!= RESET){
		CAN_ClearITPendingBit(CAN1,CAN_IT_TME);

		/*********�������Զ��岿��**********/
        OdriveSend_RemoteCmd(CAN1,AXIS0_ID,MSG_GET_MOTOR_ERROR);
        
	}
}




void CAN1DataInit(void){
	getsupervisorData()->taskEvent[CAN1_Task] = xTaskCreate(CAN1_TaskUpdateTask,"CAN1_Task",CAN1_Task_STACK_SIZE,NULL,CAN1_Task_PRIORITY,&CAN1Data.xHandleTask);
    
    
}
