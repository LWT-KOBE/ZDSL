#ifndef __CAN1_TASK_H
#define __CAN1_TASK_H

#include "stm32f4xx.h"
#include "FreeRTOS_board.h"
#include "stdbool.h"
#include "bsp.h"
#include "util.h"

#define CAN1_Task_PRIORITY 11
#define CAN1_Task_STACK_SIZE 512
#define CAN1_Task_NORMAL_PERIOD 5


#define Servo_Motor_ID0 1537
#define Servo_Motor_ID1 1538
#define Servo_Motor_ID2 1539

typedef struct {
	TaskHandle_t xHandleTask;
	uint8_t dataInitFlag;
	uint32_t loops;
} CAN1_TaskStruct_t;

typedef struct {
	
	u8 Reset_flag;		//����flash
	
	u8 Rebot_flag;		//�����豸
	
	u8 Flash_SaveFlag;	//��������
	//�趨�����λ�á���float��
	formatTrans32Struct_t SetPos[4];
	
	//�趨������ٶȡ���float��	
	formatTrans32Struct_t SetVel[4];
	
	//�趨����ĵ���	
	formatTrans16Struct_t SetCur[4];	
	
	//�趨������ٶȼ��ٶ�
	formatTrans16Struct_t Acce[4];
	
	//�趨������ٶȼ��ٶ�
	formatTrans16Struct_t Dece[4];
	
	uint32_t loops;
} Servo_MotorStruct_t;


/*Odrive ��CAN���սṹ��*/
typedef struct {
	
	uint8_t current_presence[4];		//��������

	formatTrans16Struct_t Iq_measured[4];//����
	
	formatTrans32Struct_t shadow_count[4];//shadow	
	formatTrans32Struct_t count_in_cpr[4];//CPR
	formatTrans32Struct_t pos_estimate[4];//λ��	
	formatTrans32Struct_t vel_estimate[4];//�ٶ�
		

	formatTrans32Struct_t vel_limit[4]; //�ٶ����ơ�������
	formatTrans32Struct_t current_limit[4];//�������ơ�������
	formatTrans32Struct_t Target_Torque[4];//Ŀ������
	formatTrans32Struct_t Torque_Slope[4];//����б��
	formatTrans32Struct_t NMT[4];//CANOPen�������
	

}Servo_MotorDataRecv_t;

/*CAN���ͽṹ��*/
typedef struct {
	uint8_t cmd;
	uint8_t data[8];
}CANSendStruct_t;


CAN1_TaskStruct_t* getCAN1Data(void);

extern Servo_MotorStruct_t SM;
extern Servo_MotorDataRecv_t SM_Recv;
extern CANSendStruct_t can1data;

void CAN1DataInit(void);

#endif
