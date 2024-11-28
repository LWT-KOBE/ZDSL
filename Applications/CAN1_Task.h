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
	
	u8 Reset_flag;		//重置flash
	
	u8 Rebot_flag;		//重启设备
	
	u8 Flash_SaveFlag;	//保存配置
	//设定电机的位置——float型
	formatTrans32Struct_t SetPos[4];
	
	//设定电机的速度——float型	
	formatTrans32Struct_t SetVel[4];
	
	//设定电机的电流	
	formatTrans16Struct_t SetCur[4];	
	
	//设定电机的速度加速度
	formatTrans16Struct_t Acce[4];
	
	//设定电机的速度减速度
	formatTrans16Struct_t Dece[4];
	
	uint32_t loops;
} Servo_MotorStruct_t;


/*Odrive 的CAN接收结构体*/
typedef struct {
	
	uint8_t current_presence[4];		//电流存在

	formatTrans16Struct_t Iq_measured[4];//电流
	
	formatTrans32Struct_t shadow_count[4];//shadow	
	formatTrans32Struct_t count_in_cpr[4];//CPR
	formatTrans32Struct_t pos_estimate[4];//位置	
	formatTrans32Struct_t vel_estimate[4];//速度
		

	formatTrans32Struct_t vel_limit[4]; //速度限制——接收
	formatTrans32Struct_t current_limit[4];//电流限制——接收
	formatTrans32Struct_t Target_Torque[4];//目标力矩
	formatTrans32Struct_t Torque_Slope[4];//力矩斜率
	formatTrans32Struct_t NMT[4];//CANOPen网络管理
	

}Servo_MotorDataRecv_t;

/*CAN发送结构体*/
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
