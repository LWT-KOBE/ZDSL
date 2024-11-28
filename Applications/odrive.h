#ifndef __ODRIVE_H
#define __ODRIVE_H

#include "stm32f4xx.h"
#include "Util.h"
#include "board.h"

#define AXIS0_ID 0x000 //M0的CANID
#define AXIS1_ID 0x002 //M1的CANID
#define AXIS2_ID 0x004 //M2的CANID
#define AXIS3_ID 0x006 //M3的CANID
//#define AXIS3_ID 0x003 //M3的CANID

#define ODRIVE_PRIORITY 3
#define ODRIVE_STACK_SIZE 1024
#define ODRIVE_NORMAL_PERIOD 8



//电机的ID 
enum{	
	axis0=0,	
	axis1=1,		
	axis2=2,
	axis3=3
};

//电机错误值
enum MOTOR_Error {
            ERROR_NONE  = 0,
			ERR_OVER_VOLTAGE =  1,
			ERR_UNDER_VOLTAGE = 2,
			ERR_OVER_SPEED = 4,
			ERR_OVER_TEMP = 10,
};

//编码器错误值
 enum ENCODER_Error {
	CE_NULL = 0,
	CE_PHASE_RESISTANCE_OUT_OF_RANGE,
	CE_MOTOR_POLE_PAIRS_OUT_OF_RANGE,
};		




	
	//CAN的电机控制命令
typedef  enum {
        MSG_CO_NMT_CTRL = 0x000,             		 // CANOpen NMT Message REC
		MSG_ODRIVE_HEARTBEAT = 0x001,                // ODrive心跳消息
		MSG_ODRIVE_ESTOP = 0x002,                    // ODrive急停消息
		MSG_GET_MOTOR_ERROR = 0x003,                 // 获取电机错误信息
		MSG_GET_ENCODER_ERROR = 0x004,               // 获取编码器错误信息
		MSG_GET_SENSORLESS_ERROR = 0x005,            // 获取无传感器错误信息
		MSG_SET_AXIS_NODE_ID = 0x006,                // 设置轴节点ID
		MSG_SET_AXIS_REQUESTED_STATE = 0x007,        // 设置轴请求状态
		MSG_SET_AXIS_STARTUP_CONFIG = 0x008,         // 设置轴启动配置
		MSG_GET_ENCODER_ESTIMATES = 0x009,           // 获取编码器估计值
		MSG_GET_ENCODER_COUNT = 0x00A,               // 获取编码器计数
		MSG_SET_CONTROLLER_MODES = 0x00B,            // 设置控制器模式
		MSG_SET_INPUT_POS = 0x00C,                   // 设置位置控制输入
		MSG_SET_INPUT_VEL = 0x00D,                   // 设置速度控制输入
		MSG_SET_INPUT_TORQUE = 0x00E,                // 设置扭矩控制输入
		MSG_SET_LIMITS = 0x00F,                      // 设置限制
		MSG_START_ANTICOGGING = 0x010,               // 启动反扭矩
		MSG_SET_TRAJ_VEL_LIMIT = 0x011,              // 设置轨迹速度限制
		MSG_SET_TRAJ_ACCEL_LIMITS = 0x012,           // 设置轨迹加速度限制
		MSG_SET_TRAJ_INERTIA = 0x013,                // 设置轨迹惯性
		MSG_GET_IQ = 0x014,                          // 获取电流
		MSG_GET_SENSORLESS_ESTIMATES = 0x015,        // 获取无传感器估计值
		MSG_GET_VBUS_VOLTAGE = 0x016,                // 获取总线电压
		MSG_CLEAR_ERRORS = 0x017,                    // 清除错误
		
		
		MSG_SET_LINEAR_COUNT = 0x018,				 //
        MSG_SET_POS_GAIN = 0x019,					 // 设置位置环增益
        MSG_SET_VEL_GAINS = 0x01A,					 //	设置速度环增益
//        MSG_GET_ADC_VOLTAGE = 0x01B,				 //
       
		MSG_RESET_ODRIVE = 0x01B,                    // 重置ODrive
		MSG_SAVE_CONFIG = 0x01C,					 // 保存ODrive
		//MSG_GET_CONTROLLER_ERROR = 0x01C,			 //
		MSG_GET_MOTOR_TEMP = 0x01D,					 // 获取温度
		//新增

		MSG_GET_POS_GAIN = 0x01E,
		MSG_GET_VEL_GAINS = 0x01F,
		//MSG_SAVE_CONFIG = 0x01A,                     // 保存配置

		MSG_CO_HEARTBEAT_CMD = 0x700,        // CANOpen NMT Heartbeat SEND
    }ODCmdStruct_t;
	
	
	
//电机控制模式
typedef enum {
	CONTROL_MODE_CURRENT		 = 0,//电流控制模式——直接控制
	CONTROL_MODE_CURRENT_RAMP	 = 1,//电流控制模式——梯形
	CONTROL_MODE_VELOCITY		 = 2,//速度控制模式——直接控制
	CONTROL_MODE_VELOCITY_RAMP	 = 3,//速度控制模式——梯形
	CONTROL_MODE_POSITION		 = 4,//位置控制模式——直接控制
	CONTROL_MODE_POSITION_TRAP	 = 5,//位置控制模式——梯形
	CONTROL_MODE_POSITION_FILTER = 6,//滤波位置模式

} ODControlMode;


//电机控制状态
typedef enum {
	AXIS_STATE_UNDEFINED             = 0,
	AXIS_STATE_IDLE                  = 1,
	AXIS_STATE_STARTUP_SEQUENCE      = 2,
	AXIS_STATE_FULL_CALIBRATION_SEQUENCE = 3,
	AXIS_STATE_MOTOR_CALIBRATION     = 4,
	AXIS_STATE_ENCODER_INDEX_SEARCH  = 6,
	AXIS_STATE_ENCODER_OFFSET_CALIBRATION = 7,
	AXIS_STATE_CLOSED_LOOP_CONTROL   = 8,
	AXIS_STATE_LOCKIN_SPIN           = 9,
	AXIS_STATE_ENCODER_DIR_FIND      = 10,
	AXIS_STATE_HOMING                = 11,
	AXIS_STATE_ENCODER_HALL_POLARITY_CALIBRATION = 12,
	AXIS_STATE_ENCODER_HALL_PHASE_CALIBRATION = 13,
	
	// CMD
	CMD_MENU	=	0x1B,	// Esc 释放电机
	CMD_MOTOR   		=	'm',//电机进入闭环模式
	CMD_CALIBRATION		=	'c',//电机校准
		
	//以下控制模式请在串口上位机上配置	
	CMD_ANTICOGGING		=	'a',
	CMD_UPDATE_CONFIGS	=	'u',
	CMD_RESET_ERROR		=	'z',
	CMD_DEBUG_Q			=	'q',
	CMD_DEBUG_W			=	'w',
	CMD_UART_SETUP 		=	's',
	
}ODAxisStateStruct_t;


/*Odrive 的CAN发送结构体*/
typedef struct {
	uint8_t cmd;
	uint8_t data[8];
}ODCANSendStruct_t;


/*Odrive 的CAN接收结构体*/
typedef struct {   
		
	formatTrans32Struct_t vbus_voltage[4] ;//Odrive总线电压
	formatTrans32Struct_t ibus[4] ;//Odrive总线电流	
	formatTrans32Struct_t MotorError[4];//电机错误
	formatTrans32Struct_t EncoderError[4];//编码器错误		
	formatTrans32Struct_t temperature[4];//板载温度
	formatTrans32Struct_t motor_temperature[4];//电机温度
	
	formatTrans32Struct_t Pos_gain[4];//位置环增益		
	formatTrans32Struct_t Vel_gain[4];//速度环增益
	formatTrans32Struct_t Vel_integrator_gain[4];//速度环积分增益
	formatTrans32Struct_t CG[4];
	
	//心跳信号反馈的轴错误数据
	formatTrans32Struct_t heartbeat_AxisError[4];
	
	//心跳信号反馈当前电机状态
	uint8_t heartbeat_current_state_[4];
	
	//心跳信号反馈电机标志
	uint8_t heartbeat_motorFlags[4];
	//心跳信号反馈编码器标志
	uint8_t heartbeat_encoderFlags[4];
	//心跳信号反馈的电机控制器错误和轨迹完成标志
	uint8_t heartbeat_controllerFlags[4];
	
	

	
	//电机控制模式
	ODControlMode control_mode[4];				
	
	
	float motor_phase_resistance[4];//电机电阻值
	float motor_phase_inductance[4];//电机电感值
	uint8_t motor_pole_pairs[4];//电机的极对数

	formatTrans32Struct_t shadow_count[4];//shadow	
	formatTrans32Struct_t count_in_cpr[4];//CPR
	formatTrans32Struct_t pos_estimate[4];//位置	
	formatTrans32Struct_t vel_estimate[4];//速度
	formatTrans32Struct_t Iq_measured[4];//电流	

	formatTrans32Struct_t vel_limit[4]; //速度限制——接收
	formatTrans32Struct_t current_limit[4];//电流限制——接收


}ODCanDataRecv_t;


typedef struct {
	
	//位置环增益
	formatTrans32Struct_t Pos_gain[4];
	
	//速度环增益
	formatTrans32Struct_t Vel_gain[4];
	
	//速度环积分增益
	formatTrans32Struct_t Vel_integrator_gain[4];
	
	//电机的控制状态
	ODAxisStateStruct_t	AxisState[4];
	
	//电机的控制模式	
	ODControlMode ControlMode[4];
	
	//设定电机的位置——float型
	formatTrans32Struct_t SetPos[4];
	
	//设定电机的速度——float型	
	formatTrans32Struct_t SetVel[4];
	formatTrans32Struct_t Vel_cnt[4];//速度指令计数
	
	//设定电机的电流——float型	
	formatTrans32Struct_t SetCur[4];	
		
	//配置电机状态FLAG	
	uint8_t RequestedStateFlag;
	uint8_t RequestedStateFlag1;

	//保存ODRIVE的配置参数FLAG
	uint8_t flashSaveFlag;
	uint8_t flashSaveFlag1;
	
	//重启odrive
	uint8_t RebotFlag;
	uint8_t RebotFlag1;
	
	//配置电机控制模式FLAG	
	uint8_t ControlModeFlag;	
	uint8_t ControlModeFlag1;
	//清楚电机错误FLAG
	uint8_t clearerrorFlag;
	uint8_t clearerrorFlag1;
	
	//查询电机的电感/电阻/极对数和电机控制状态
	uint8_t readpairsFlag;
	uint8_t readpairsFlag1;
	
	//设置电流电压限制FLAG	
	uint8_t SetLimitFlag;
	uint8_t SetLimitFlag1;

	
	//配置电机位置环增添FLAG	
	uint8_t Pos_gainFlag;
	uint8_t Pos_gainFlag1;
	
	//配置电机速度环增益FLAG	
	uint8_t Vel_gainFlag;
	uint8_t Vel_gainFlag1;
	
	//速度限制——发送
	formatTrans32Struct_t vel_limit[4]; 
	
	//电流限制——发送
	formatTrans32Struct_t current_limit[4];

	

	//freertos相关
	TaskHandle_t xHandleTask;
	uint8_t dataInitFlag;
	uint32_t loops;
	
	errorScanStruct_t OdError;
	
	
	
	//CAN的电机控制CMD结构体
	ODCmdStruct_t OdriveCmd;
} OdriveStruct_t;	

void OdriveSendData(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN, uint8_t len,ODCANSendStruct_t* CanSendData);
void ODSendInputPosData(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN, uint8_t len,OdriveStruct_t* Spetsnaz,uint8_t axis,ODCANSendStruct_t* CanSendData);
void set_axis_requested_state(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN, uint8_t len,OdriveStruct_t* Spetsnaz,uint8_t axis,ODCANSendStruct_t* CanSendData);


void ODReadHeartBeatData(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis);
void ODReadVbusData(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis);
void OdriveSend_RemoteCmd(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN);
void ODReadTempData(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis);
void ODReadMotorErrorData(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis);
void ODReadEncoderCountData(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis);
void ODReadEncodeEstimatesData(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis);
void ODReadEncodeErrorData(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis);
void ODSendInputVelData(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN, uint8_t len,OdriveStruct_t* Spetsnaz,uint8_t axis,ODCANSendStruct_t* CanSendData);
void ODSendInputCurData(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN, uint8_t len,OdriveStruct_t* Spetsnaz,uint8_t axis,ODCANSendStruct_t* CanSendData);
void ODReadMotorPolePairs(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis);
void ODReadLimitData(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis);
void ODSendLimitData(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN, uint8_t len,OdriveStruct_t* Spetsnaz,uint8_t axis,ODCANSendStruct_t* CanSendData);
void ODSendPos_gainData(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN, uint8_t len,OdriveStruct_t* Spetsnaz,uint8_t axis,ODCANSendStruct_t* CanSendData);
void ODSendVel_gainsData(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN, uint8_t len,OdriveStruct_t* Spetsnaz,uint8_t axis,ODCANSendStruct_t* CanSendData);
void OdriveInit(void);
void ODRequestedState(void);	
void ODFlashSave(void);	
void ODClearError(void);		
void ODFReadMotorPairs(void);	
void ODSetLimit(void);	

void ODSendInputVelData(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN, uint8_t len,OdriveStruct_t* Spetsnaz,uint8_t axis,ODCANSendStruct_t* CanSendData);

extern OdriveStruct_t OdriveData; 
extern ODCanDataRecv_t OdReceivedData;
extern ODCANSendStruct_t ODSendData;


#endif
