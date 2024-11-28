#include "board.h"

OdriveStruct_t OdriveData; 
ODCanDataRecv_t OdReceivedData;
uint8_t Flag = 0;
float buf[10];
////////////////////////////ODRIVE单驱板（M0）的CAN控制(STM32F407VET6) 波特率：1000K  使用120Ω终端电阻/////////////////////////
/*********************************以下为驱动层代码*****************************************************************************/

/*
***************************************************
函数名：OdriveSendData
功能：电机命令CAN发送_数据帧
入口参数：	CANx：CAN1 orCAN2
			ID_CAN：CANID 电机0或电机1的ID地址 规定： AXIS0_ID 0x001   AXIS1_ID 0x002
			CMD_CAN：odrive的各种命令  
			len：数据帧长度 
			CanSendData：CAN发送的数据结构
返回值：无
应用范围：内部调用
备注： 该函数发送的是数据帧，切记
***************************************************
*/

void OdriveSendData(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN, uint8_t len,ODCANSendStruct_t* CanSendData) {
	CanTxMsg *txMessage;
	uint8_t mbox;
	uint8_t count;
	uint16_t i = 0;
	txMessage = (CanTxMsg*)aqCalloc(8,sizeof(CanTxMsg));
	
	//CAN ID 的前六位是轴ID（在odrive端设置为0x001），后五位是控制命令（比如 MSG_GET_ENCODER_ERROR）	
	txMessage->StdId = (ID_CAN<<5)+CMD_CAN;	
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
函数名：OdriveSend_RemoteCmd
功能：电机命令CAN发送_远程帧帧
入口参数：	CANx：CAN1 orCAN2
			CMD_CAN：odrive的各种命令  
返回值：无
应用范围：内部调用
备注： 该函数发送的是远程帧，切记
***************************************************
*/
void OdriveSend_RemoteCmd(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN) {
	CanTxMsg *txMessage;
	uint8_t mbox;
	uint16_t i = 0;
	txMessage = (CanTxMsg*)aqCalloc(8,sizeof(CanTxMsg));
	
	//CAN ID 的前六位是轴ID（在odrive端设置为0x001），后五位是控制命令（比如 MSG_GET_ENCODER_ERROR）
	txMessage->StdId = (ID_CAN<<5)+CMD_CAN;

	//txMessage->ExtId=0x12; 	 // 设置扩展标示符（29位） 
	txMessage->ExtId=0x12; 	 // 设置扩展标示符（29位）
	
	txMessage->IDE = CAN_Id_Standard;
	//txMessage->RTR = CAN_RTR_Remote;
	txMessage->RTR = CAN_RTR_Data;
	mbox = CAN_Transmit(CANx, txMessage);
	while (CAN_TransmitStatus(CANx,mbox) == 0x00) {
		i++;
		if (i >= 0xFFF)break;
	}
	aqFree(txMessage,8,sizeof(CanTxMsg));
}


/*
***************************************************
函数名：ODSendInputPosData
功能：电机位置闭环命令_数据帧
入口参数：	CANx：CAN1 or CAN2
			ID_CAN：CANID 电机0或电机1的ID地址 规定： AXIS0_ID 0x001   AXIS1_ID 0x002
			CMD_CAN：odrive的位置闭环命令（MSG_SET_INPUT_POS ）
			len：数据帧长度 
			CanSendData：CAN发送的数据结构
			Spetsnaz：发送的位置控制结构体
			axis: 选择电机0或电机1命令发送 规定 ： axis0=0  axis1=1 
返回值：无
应用范围：内部调用
备注：
	 
***************************************************
*/
void ODSendInputPosData(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN, uint8_t len,OdriveStruct_t* Spetsnaz,uint8_t axis,ODCANSendStruct_t* CanSendData) {

	CanSendData->data[0] = Spetsnaz->SetPos[axis].u8_temp[0];
	CanSendData->data[1] = Spetsnaz->SetPos[axis].u8_temp[1];
	CanSendData->data[2] = Spetsnaz->SetPos[axis].u8_temp[2];
	CanSendData->data[3] = Spetsnaz->SetPos[axis].u8_temp[3];	

	OdriveSendData(CANx,ID_CAN,CMD_CAN,len,CanSendData); 
	
	
}



/*
***************************************************
函数名：ODSendPos_gainData
功能：电机位置增益命令_数据帧
入口参数：	CANx：CAN1 or CAN2
			ID_CAN：CANID 电机0或电机1的ID地址 规定： AXIS0_ID 0x001   AXIS1_ID 0x002
			CMD_CAN：odrive的位置增益命令（MSG_SET_INPUT_POS ）
			len：数据帧长度 
			CanSendData：CAN发送的数据结构
			Spetsnaz：发送的位置增益控制结构体
			axis: 选择电机0或电机1命令发送 规定 ： axis0=0  axis1=1 
返回值：无
应用范围：内部调用
备注：
	 
***************************************************
*/
void ODSendPos_gainData(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN, uint8_t len,OdriveStruct_t* Spetsnaz,uint8_t axis,ODCANSendStruct_t* CanSendData) {

	CanSendData->data[0] = Spetsnaz->Pos_gain[axis].u8_temp[0];
	CanSendData->data[1] = Spetsnaz->Pos_gain[axis].u8_temp[1];
	CanSendData->data[2] = Spetsnaz->Pos_gain[axis].u8_temp[2];
	CanSendData->data[3] = Spetsnaz->Pos_gain[axis].u8_temp[3];	

	OdriveSendData(CANx,ID_CAN,CMD_CAN,len,CanSendData); 
}


/*
***************************************************
函数名：ODSendInputVelData
功能：电机速度闭环命令_数据帧
入口参数：	CANx：CAN1 orCAN2
			ID_CAN：CANID 电机0或电机1的ID地址 规定： AXIS0_ID 0x001   AXIS1_ID 0x002
			CMD_CAN：odrive的速度闭环命令（MSG_SET_INPUT_VEL ）
			len：数据帧长度 
			CanSendData：CAN发送的数据结构
			Spetsnaz：发送的速度控制结构体
			axis: 选择电机0或电机1命令发送 规定 ： axis0=0  axis1=1 
返回值：无
应用范围：内部调用
备注：
	 
***************************************************
*/
void ODSendInputVelData(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN, uint8_t len,OdriveStruct_t* Spetsnaz,uint8_t axis,ODCANSendStruct_t* CanSendData) {
	
	Spetsnaz->SetCur[axis].float_temp += 0.00001f;
	
	CanSendData->data[0] = Spetsnaz->SetVel[axis].u8_temp[0];
	CanSendData->data[1] = Spetsnaz->SetVel[axis].u8_temp[1];
	CanSendData->data[2] = Spetsnaz->SetVel[axis].u8_temp[2];
	CanSendData->data[3] = Spetsnaz->SetVel[axis].u8_temp[3];
	
	CanSendData->data[4] = Spetsnaz->SetCur[axis].u8_temp[0];
	CanSendData->data[5] = Spetsnaz->SetCur[axis].u8_temp[1];
	CanSendData->data[6] = Spetsnaz->SetCur[axis].u8_temp[2];
	CanSendData->data[7] = Spetsnaz->SetCur[axis].u8_temp[3];
	

	OdriveSendData(CANx,ID_CAN,CMD_CAN,len,CanSendData); 
	
	
}

/*
***************************************************
函数名：ODSendVel_gainData
功能：电机速度环增益命令_数据帧
入口参数：	CANx：CAN1 or CAN2
			ID_CAN：CANID 电机0或电机1的ID地址 规定： AXIS0_ID 0x001   AXIS1_ID 0x002
			CMD_CAN：odrive的速度环增益命令（MSG_SET_VEL_GAIN ）
			len：数据帧长度 
			CanSendData：CAN发送的数据结构
			Spetsnaz：发送的位置增益控制结构体
			axis: 选择电机0或电机1命令发送 规定 ： axis0=0  axis1=1 
返回值：无
应用范围：内部调用
备注：
	 
***************************************************
*/
void ODSendVel_gainsData(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN, uint8_t len,OdriveStruct_t* Spetsnaz,uint8_t axis,ODCANSendStruct_t* CanSendData){

	CanSendData->data[0] = Spetsnaz->Vel_gain[axis].u8_temp[0];
	CanSendData->data[1] = Spetsnaz->Vel_gain[axis].u8_temp[1];
	CanSendData->data[2] = Spetsnaz->Vel_gain[axis].u8_temp[2];
	CanSendData->data[3] = Spetsnaz->Vel_gain[axis].u8_temp[3];

	CanSendData->data[4] = Spetsnaz->Vel_integrator_gain[axis].u8_temp[0];
	CanSendData->data[5] = Spetsnaz->Vel_integrator_gain[axis].u8_temp[1];
	CanSendData->data[6] = Spetsnaz->Vel_integrator_gain[axis].u8_temp[2];
	CanSendData->data[7] = Spetsnaz->Vel_integrator_gain[axis].u8_temp[3];

	OdriveSendData(CANx,ID_CAN,CMD_CAN,len,CanSendData); 
}


/*
***************************************************
函数名：ODSendInputCurData
功能：电机力矩闭环命令_数据帧
入口参数：	CANx：CAN1 orCAN2
			ID_CAN：CANID 电机0或电机1的ID地址 规定： AXIS0_ID 0x001   AXIS1_ID 0x002
			CMD_CAN：odrive的力矩闭环命令（MSG_SET_INPUT_TORQUE ）
			len：数据帧长度 
			CanSendData：CAN发送的数据结构
			Spetsnaz：发送的力矩控制结构体
			axis: 选择电机0或电机1命令发送 规定 ： axis0=0  axis1=1 
返回值：无
应用范围：内部调用
备注：
	 
***************************************************
*/
void ODSendInputCurData(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN, uint8_t len,OdriveStruct_t* Spetsnaz,uint8_t axis,ODCANSendStruct_t* CanSendData) {
	
	
	CanSendData->data[0] = Spetsnaz->SetCur[axis].u8_temp[0];
	CanSendData->data[1] = Spetsnaz->SetCur[axis].u8_temp[1];
	CanSendData->data[2] = Spetsnaz->SetCur[axis].u8_temp[2];
	CanSendData->data[3] = Spetsnaz->SetCur[axis].u8_temp[3];

	OdriveSendData(CANx,ID_CAN,CMD_CAN,len,CanSendData); 
	
	
}


/*
***************************************************
函数名：ODSET_CONTROL_MODE
功能：电机状态参数配置_数据帧
入口参数：	CANx：CAN1 orCAN2
			ID_CAN：CANID 电机0或电机1的ID地址 规定： AXIS0_ID 0x001   AXIS1_ID 0x002
			CMD_CAN：odrive的状态参数配置命令（MSG_SET_CONTROLLER_MODES ）
			len：数据帧长度
			CONTROL_MODE：电机控制模式
			CanSendData：CAN发送的数据结构
			Spetsnaz：发送的电机参数结构体
			axis: 选择电机0或电机1命令发送 规定 ： axis0=0  axis1=1 
返回值：无
应用范围：内部调用
备注： uint8_t len的数据范围
	 
    enum {
        CONTROL_MODE_CURRENT		 = 0,//电流控制模式——直接控制
        CONTROL_MODE_CURRENT_RAMP	 = 1,//电流控制模式——梯形
        CONTROL_MODE_VELOCITY		 = 2,//速度控制模式——直接控制
        CONTROL_MODE_VELOCITY_RAMP	 = 3,//速度控制模式——梯形
        CONTROL_MODE_POSITION		 = 4,//位置控制模式——直接控制
        CONTROL_MODE_POSITION_TRAP	 = 5,//位置控制模式——梯形
		CONTROL_MODE_POSITION_FILTER = 6,//位置滤波控制模式
    }; 

***************************************************
*/
void ODSET_CONTROL_MODE(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN, uint8_t len,OdriveStruct_t* Spetsnaz,uint8_t axis,ODCANSendStruct_t* CanSendData) {
	
	uint8_t CONTROL_MODE = 5;
	
	CONTROL_MODE = Spetsnaz->ControlMode[axis];
	switch (CONTROL_MODE){
		
		//直接力矩模式
		case CONTROL_MODE_CURRENT:
			CanSendData->data[0] = 0x01;
			CanSendData->data[1] = 0;
			CanSendData->data[2] = 0;
			CanSendData->data[3] = 0;
	
			CanSendData->data[4] = 0x01;
			CanSendData->data[5] = 0;
			CanSendData->data[6] = 0;
			CanSendData->data[7] = 0;
		break;
		
		//力矩爬升模式
		case CONTROL_MODE_CURRENT_RAMP:
			CanSendData->data[0] = 0x01;
			CanSendData->data[1] = 0;
			CanSendData->data[2] = 0;
			CanSendData->data[3] = 0;
	
			CanSendData->data[4] = 0x06;
			CanSendData->data[5] = 0;
			CanSendData->data[6] = 0;
			CanSendData->data[7] = 0;
		break;
		
		//直接速度模式
		case CONTROL_MODE_VELOCITY:
			CanSendData->data[0] = 0x02;
			CanSendData->data[1] = 0;
			CanSendData->data[2] = 0;
			CanSendData->data[3] = 0;
	
			CanSendData->data[4] = 0x01;
			CanSendData->data[5] = 0;
			CanSendData->data[6] = 0;
			CanSendData->data[7] = 0;
		break;
		
		//速度爬升模式
		case CONTROL_MODE_VELOCITY_RAMP:
			CanSendData->data[0] = 0x02;
			CanSendData->data[1] = 0;
			CanSendData->data[2] = 0;
			CanSendData->data[3] = 0;
	
			CanSendData->data[4] = 0x02;
			CanSendData->data[5] = 0;
			CanSendData->data[6] = 0;
			CanSendData->data[7] = 0;
		break;
		
		//直接位置模式
		case CONTROL_MODE_POSITION:
			CanSendData->data[0] = 0x03;
			CanSendData->data[1] = 0;
			CanSendData->data[2] = 0;
			CanSendData->data[3] = 0;
	
			CanSendData->data[4] = 0x01;
			CanSendData->data[5] = 0;
			CanSendData->data[6] = 0;
			CanSendData->data[7] = 0;
		break;
		
		//梯形位置模式
		case CONTROL_MODE_POSITION_TRAP:
			CanSendData->data[0] = 0x03;
			CanSendData->data[1] = 0;
			CanSendData->data[2] = 0;
			CanSendData->data[3] = 0;
	
			CanSendData->data[4] = 0x05;
			CanSendData->data[5] = 0;
			CanSendData->data[6] = 0;
			CanSendData->data[7] = 0;
		break;
		
		//位置滤波控制模式
		case CONTROL_MODE_POSITION_FILTER:
			CanSendData->data[0] = 0x03;
			CanSendData->data[1] = 0;
			CanSendData->data[2] = 0;
			CanSendData->data[3] = 0;
	
			CanSendData->data[4] = 0x03;
			CanSendData->data[5] = 0;
			CanSendData->data[6] = 0;
			CanSendData->data[7] = 0;
		break;
	}


	
	OdriveSendData(CANx,ID_CAN,CMD_CAN,len,CanSendData); 
	
	
}



/*
***************************************************
函数名：ODReadHeartBeatData
功能：读取心跳信号_数据帧
入口参数：	
			CanSendData：CAN发送的数据结构
			Spetsnaz：接收心跳信号结构体
			axis: 选择电机0或电机1命令发送 规定 ： axis0=0  axis1=1 
返回值：无
应用范围：内部调用
备注：
	
	心跳信号的发送频率为100HZ（ODRIVE端）,且心跳信号返回的数据0-1（低到高）是ODRIVE的电机电流，除100表示电流在发送前扩大了100倍，精度是0.01f
	2-3位是ODRIVE的电机位置，除100表示位置在发送前扩大了100倍，精度是0.01f
	4-5位是ODRIVE的电机速度，除1000表示速度在发送前扩大了1000倍，精度是0.01f
	第6位是轴错误信息（axes[0].error_），U8型，8个字节
	
	enum {
			ERROR_NONE		= 1,
			ERROR_INVALID_STATE	= 2,
			ERROR_WATCHDOG_TIMER_EXPIRED		= 3,
			ERROR_MIN_ENDSTOP_PRESSED	= 4,
			ERROR_MAX_ENDSTOP_PRESSED		= 5,
			ERROR_ESTOP_REQUESTED	= 6,
			ERROR_HOMING_WITHOUT_ENDSTOP = 7,
			ERROR_OVER_TEMP = 8,
			ERROR_UNKNOWN_POSITION = 9,
		
		};


	第7位是控制状态，U8型，8个字节
	
        enum AxisState {
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
        };

***************************************************
*/

void ODReadHeartBeatData(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis) {
		
//	Spetsnaz->heartbeat_Cur[axis] = (float)(s16)((CanRevData->Data[1]<<8)|CanRevData->Data[0])/100.0f;
//	Spetsnaz->heartbeat_Pos[axis] = (float)(s16)((CanRevData->Data[3]<<8)|CanRevData->Data[2])/100.0f;
//	Spetsnaz->heartbeat_Vel[axis] = (float)(s16)((CanRevData->Data[5]<<8)|CanRevData->Data[4])/100.0f;	
//	Spetsnaz->heartbeat_AxisError[axis] = CanRevData->Data[6];
//	Spetsnaz->heartbeat_Current_State[axis] = CanRevData->Data[7];
	
	Spetsnaz->heartbeat_AxisError[axis].u8_temp[0] = CanRevData->Data[0]; 
	Spetsnaz->heartbeat_AxisError[axis].u8_temp[1] = CanRevData->Data[1]; 
	Spetsnaz->heartbeat_AxisError[axis].u8_temp[2] = CanRevData->Data[2]; 
	Spetsnaz->heartbeat_AxisError[axis].u8_temp[3] = CanRevData->Data[3]; 
	
	Spetsnaz->heartbeat_current_state_[axis] = CanRevData->Data[4];
	Spetsnaz->heartbeat_motorFlags[axis] = CanRevData->Data[5];
	Spetsnaz->heartbeat_encoderFlags[axis] = CanRevData->Data[6];
	Spetsnaz->heartbeat_controllerFlags[axis] = CanRevData->Data[7];
	

}

/*
***************************************************
函数名：ODReadVbusData
功能：读取母线电压_数据帧
入口参数：	
			CanSendData：CAN发送的数据结构
			Spetsnaz：接收母线电压和母线电流的数据结构体
			axis: 选择电机0或电机1命令发送 规定 ： axis0=0  axis1=1 
返回值：无
应用范围：内部调用
备注：
	
	返回的数据前四位（低到高）是ODRIVE的母线电压数据，后四位返回的是ODRIVE的母线电流数据，都是FLOAT型
	所以用到了一个联合体进行数据转换，详情请看：
	//联合体用于转换数据
	typedef union{
		u8 		u8_temp[4];
		float float_temp;
		s32 	s32_temp;
		u32		u32_temp;
	} formatTrans32Struct_t;
***************************************************
*/
void ODReadVbusData(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis) {
		
	
	Spetsnaz->vbus_voltage[axis].u8_temp[0] = CanRevData->Data[0];	
	Spetsnaz->vbus_voltage[axis].u8_temp[1] = CanRevData->Data[1];	
	Spetsnaz->vbus_voltage[axis].u8_temp[2] = CanRevData->Data[2];	
	Spetsnaz->vbus_voltage[axis].u8_temp[3] = CanRevData->Data[3];
	
	Spetsnaz->ibus[axis].u8_temp[0] = CanRevData->Data[4];
	Spetsnaz->ibus[axis].u8_temp[1] = CanRevData->Data[5];
	Spetsnaz->ibus[axis].u8_temp[2] = CanRevData->Data[6];
	Spetsnaz->ibus[axis].u8_temp[3] = CanRevData->Data[7];
}

/*
***************************************************
函数名：ODReadTempData
功能：读取电机的温度_数据帧
入口参数：	
			CanSendData：CAN发送的数据结构
			Spetsnaz：接收电机温度的数据结构体
			axis: 选择电机0或电机1命令发送 规定 ： axis0=0  axis1=1 
返回值：无
应用范围：内部调用
备注：
	
	返回的数据前四位（低到高）是ODRIVE的电机温度数据，是FLOAT型
	所以用到了一个联合体进行数据转换，详情请看：
	//联合体用于转换数据
	typedef union{
		u8 		u8_temp[4];
		float float_temp;
		s32 	s32_temp;
		u32		u32_temp;
	} formatTrans32Struct_t;
***************************************************
*/void ODReadTempData(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis) {
	
	//板载温度
	Spetsnaz->temperature[axis].u8_temp[0] = CanRevData->Data[0];	
	Spetsnaz->temperature[axis].u8_temp[1] = CanRevData->Data[1];	
	Spetsnaz->temperature[axis].u8_temp[2] = CanRevData->Data[2];	
	Spetsnaz->temperature[axis].u8_temp[3] = CanRevData->Data[3];
	
	//电机温度
	Spetsnaz->motor_temperature[axis].u8_temp[0] = CanRevData->Data[4];
	Spetsnaz->motor_temperature[axis].u8_temp[1] = CanRevData->Data[5];
	Spetsnaz->motor_temperature[axis].u8_temp[2] = CanRevData->Data[6];
	Spetsnaz->motor_temperature[axis].u8_temp[3] = CanRevData->Data[7];
}

/*
***************************************************
函数名：ODReadMotorErrorData
功能：读取电机错误信息_数据帧
入口参数：	
			CMD_CAN：odrive的电机错误信息读取命令（MSG_GET_MOTOR_ERROR ）
			CanSendData：CAN发送的数据结构
			Spetsnaz：读取电机错误信息结构体
			axis: 选择电机0或电机1命令发送 规定 ： axis0=0  axis1=1 
返回值：无
应用范围：内部调用
备注：

	OdReceivedData.MotorError.u32_temp 电机错误对应值

        enum Error {
            ERROR_NONE                       = 0x00000000,
            ERROR_PHASE_RESISTANCE_OUT_OF_RANGE = 0x00000001,
            ERROR_PHASE_INDUCTANCE_OUT_OF_RANGE = 0x00000002,
            ERROR_DRV_FAULT                  = 0x00000008,
            ERROR_CONTROL_DEADLINE_MISSED    = 0x00000010,
            ERROR_MODULATION_MAGNITUDE       = 0x00000080,
            ERROR_CURRENT_SENSE_SATURATION   = 0x00000400,
            ERROR_CURRENT_LIMIT_VIOLATION    = 0x00001000,
            ERROR_MODULATION_IS_NAN          = 0x00010000,
            ERROR_MOTOR_THERMISTOR_OVER_TEMP = 0x00020000,
            ERROR_FET_THERMISTOR_OVER_TEMP   = 0x00040000,
            ERROR_TIMER_UPDATE_MISSED        = 0x00080000,
            ERROR_CURRENT_MEASUREMENT_UNAVAILABLE = 0x00100000,
            ERROR_CONTROLLER_FAILED          = 0x00200000,
            ERROR_I_BUS_OUT_OF_RANGE         = 0x00400000,
            ERROR_BRAKE_RESISTOR_DISARMED    = 0x00800000,
            ERROR_SYSTEM_LEVEL               = 0x01000000,
            ERROR_BAD_TIMING                 = 0x02000000,
            ERROR_UNKNOWN_PHASE_ESTIMATE     = 0x04000000,
            ERROR_UNKNOWN_PHASE_VEL          = 0x08000000,
            ERROR_UNKNOWN_TORQUE             = 0x10000000,
            ERROR_UNKNOWN_CURRENT_COMMAND    = 0x20000000,
            ERROR_UNKNOWN_CURRENT_MEASUREMENT = 0x40000000,
            ERROR_UNKNOWN_VBUS_VOLTAGE       = 0x80000000,
            ERROR_UNKNOWN_VOLTAGE_COMMAND    = 0x100000000,
            ERROR_UNKNOWN_GAINS              = 0x200000000,
            ERROR_CONTROLLER_INITIALIZING    = 0x400000000,
            ERROR_UNBALANCED_PHASES          = 0x800000000,
        };

***************************************************
*/

void ODReadMotorErrorData(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis) {
		
	Spetsnaz->MotorError[axis].u8_temp[0] = CanRevData->Data[0];	
	Spetsnaz->MotorError[axis].u8_temp[1] = CanRevData->Data[1];	
	Spetsnaz->MotorError[axis].u8_temp[2] = CanRevData->Data[2];	
	Spetsnaz->MotorError[axis].u8_temp[3] = CanRevData->Data[3];
	
}



/*
***************************************************
函数名：ODReadMotorErrorData
功能：读取编码器错误信息_数据帧
入口参数：	
			CMD_CAN：odrive的编码器错误信息读取命令（MSG_GET_ENCODER_ERROR ）
			CanSendData：CAN发送的数据结构
			Spetsnaz：读取编码器错误信息结构体
			axis: 选择电机0或电机1命令发送 规定 ： axis0=0  axis1=1 
返回值：无
应用范围：内部调用
备注：

	OdReceivedData.EncoderError.u32_temp 编码器错误对应值



        enum Error {
            ERROR_NONE                       = 0x00000000,
            ERROR_UNSTABLE_GAIN              = 0x00000001,
            ERROR_CPR_POLEPAIRS_MISMATCH     = 0x00000002,
            ERROR_NO_RESPONSE                = 0x00000004,
            ERROR_UNSUPPORTED_ENCODER_MODE   = 0x00000008,
            ERROR_ILLEGAL_HALL_STATE         = 0x00000010,
            ERROR_INDEX_NOT_FOUND_YET        = 0x00000020,
            ERROR_ABS_SPI_TIMEOUT            = 0x00000040,
            ERROR_ABS_SPI_COM_FAIL           = 0x00000080,
            ERROR_ABS_SPI_NOT_READY          = 0x00000100,
            ERROR_HALL_NOT_CALIBRATED_YET    = 0x00000200,
        };


***************************************************
*/
void ODReadEncodeErrorData(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis) {
		
	Spetsnaz->EncoderError[axis].u8_temp[0] = CanRevData->Data[0];	
	Spetsnaz->EncoderError[axis].u8_temp[1] = CanRevData->Data[1];	
	Spetsnaz->EncoderError[axis].u8_temp[2] = CanRevData->Data[2];	
	Spetsnaz->EncoderError[axis].u8_temp[3] = CanRevData->Data[3];
	
}


/*
***************************************************
函数名：ODReadEncoderCountData
功能：读取编码器的SHADOW和CPR_数据帧
入口参数：	
			CMD_CAN：odrive的编码器错误信息读取命令（MSG_GET_ENCODER_COUNT ）
			CanSendData：CAN发送的数据结构
			Spetsnaz：读取编码器CPR数据结构体
			axis: 选择电机0或电机1命令发送 规定 ： axis0=0  axis1=1 
返回值：无
应用范围：内部调用
备注：

***************************************************
*/

void ODReadEncoderCountData(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis) {
		
	Spetsnaz->shadow_count[axis].u8_temp[0] = CanRevData->Data[0];	
	Spetsnaz->shadow_count[axis].u8_temp[1] = CanRevData->Data[1];	
	Spetsnaz->shadow_count[axis].u8_temp[2] = CanRevData->Data[2];	
	Spetsnaz->shadow_count[axis].u8_temp[3] = CanRevData->Data[3];
	
	
	Spetsnaz->count_in_cpr[axis].u8_temp[0] = CanRevData->Data[4];	
	Spetsnaz->count_in_cpr[axis].u8_temp[1] = CanRevData->Data[5];	
	Spetsnaz->count_in_cpr[axis].u8_temp[2] = CanRevData->Data[6];	
	Spetsnaz->count_in_cpr[axis].u8_temp[3] = CanRevData->Data[7];	

}

/*
***************************************************
函数名：ODReadEncodeEstimatesData
功能：读取电机位置和速度_数据帧
入口参数：	
			CMD_CAN：odrive的编码器错误信息读取命令（MSG_GET_ENCODER_ESTIMATES ）
			CanSendData：CAN发送的数据结构
			Spetsnaz：读取电机位置和速度数据结构体
			axis: 选择电机0或电机1命令发送 规定 ： axis0=0  axis1=1 
返回值：无
应用范围：内部调用
备注：

***************************************************
*/
void ODReadEncodeEstimatesData(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis) {
		
	Spetsnaz->pos_estimate[axis].u8_temp[0] = CanRevData->Data[0];	
	Spetsnaz->pos_estimate[axis].u8_temp[1] = CanRevData->Data[1];	
	Spetsnaz->pos_estimate[axis].u8_temp[2] = CanRevData->Data[2];	
	Spetsnaz->pos_estimate[axis].u8_temp[3] = CanRevData->Data[3];
	
	
	Spetsnaz->vel_estimate[axis].u8_temp[0] = CanRevData->Data[4];	
	Spetsnaz->vel_estimate[axis].u8_temp[1] = CanRevData->Data[5];	
	Spetsnaz->vel_estimate[axis].u8_temp[2] = CanRevData->Data[6];	
	Spetsnaz->vel_estimate[axis].u8_temp[3] = CanRevData->Data[7];	

}

//读取电机极对数和电机电阻和控制模式
void ODReadMotorPolePairs(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis) {
		
	//CAN信号发送的电机电感信息，强制将S16转Float，并且除10000倍(因为发送的时候扩大了10000倍)	
	Spetsnaz->motor_phase_resistance[axis] = (float)(s16)((CanRevData->Data[1]<<8)|CanRevData->Data[0])/10000.0f;
	
	//CAN信号发送的电机电感信息，强制将S16转Float，并且扩大1000000倍(因为电感值实在是太小啦)	
	Spetsnaz->motor_phase_inductance[axis] = (float)(s16)((CanRevData->Data[3]<<8)|CanRevData->Data[2]);

	
	//读取电机的极对数
	Spetsnaz->motor_pole_pairs[axis] = CanRevData->Data[4];	
	//读取电机的控制模式
	//Spetsnaz->control_mode[axis] = CanRevData->Data[5];	


}

//读取电流限制和速度限制
void ODReadLimitData(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis) {
		
	Spetsnaz->vel_limit[axis].u8_temp[0] = CanRevData->Data[0];	
	Spetsnaz->vel_limit[axis].u8_temp[1] = CanRevData->Data[1];	
	Spetsnaz->vel_limit[axis].u8_temp[2] = CanRevData->Data[2];	
	Spetsnaz->vel_limit[axis].u8_temp[3] = CanRevData->Data[3];
	
	
	Spetsnaz->current_limit[axis].u8_temp[0] = CanRevData->Data[4];	
	Spetsnaz->current_limit[axis].u8_temp[1] = CanRevData->Data[5];	
	Spetsnaz->current_limit[axis].u8_temp[2] = CanRevData->Data[6];	
	Spetsnaz->current_limit[axis].u8_temp[3] = CanRevData->Data[7];	


}

//发送电流限制和速度限制
void ODSendLimitData(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN, uint8_t len,OdriveStruct_t* Spetsnaz,uint8_t axis,ODCANSendStruct_t* CanSendData) {

	CanSendData->data[0] = Spetsnaz->vel_limit[axis].u8_temp[0];
	CanSendData->data[1] = Spetsnaz->vel_limit[axis].u8_temp[1];
	CanSendData->data[2] = Spetsnaz->vel_limit[axis].u8_temp[2];
	CanSendData->data[3] = Spetsnaz->vel_limit[axis].u8_temp[3];

	CanSendData->data[4] = Spetsnaz->current_limit[axis].u8_temp[0];
	CanSendData->data[5] = Spetsnaz->current_limit[axis].u8_temp[1];
	CanSendData->data[6] = Spetsnaz->current_limit[axis].u8_temp[2];
	CanSendData->data[7] = Spetsnaz->current_limit[axis].u8_temp[3];	
		
	OdriveSendData(CANx,ID_CAN,CMD_CAN,len,CanSendData); 
	
	
}


/*
***************************************************
函数名：ODReadPos_gainData
功能：读取该轴位置环增益
入口参数：	
			CanSendData：CAN发送的数据结构
			Spetsnaz：接收母线电压和母线电流的数据结构体
			axis: 选择电机0或电机1命令发送 规定 ： axis0=0  axis1=1 
返回值：无
应用范围：内部调用
备注：
	
	返回的数据前四位（低到高）是ODRIVE的母线电压数据，后四位返回的是ODRIVE的母线电流数据，都是FLOAT型
	所以用到了一个联合体进行数据转换，详情请看：
	//联合体用于转换数据
	typedef union{
		u8 		u8_temp[4];
		float float_temp;
		s32 	s32_temp;
		u32		u32_temp;
	} formatTrans32Struct_t;
***************************************************
*/
void ODReadPos_gainData(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis) {
		
	
	Spetsnaz->Pos_gain[axis].u8_temp[0] = CanRevData->Data[0];	
	Spetsnaz->Pos_gain[axis].u8_temp[1] = CanRevData->Data[1];	
	Spetsnaz->Pos_gain[axis].u8_temp[2] = CanRevData->Data[2];	
	Spetsnaz->Pos_gain[axis].u8_temp[3] = CanRevData->Data[3];
}

/*
***************************************************
函数名：ODReadVel_gainsData
功能：读取速度环增益值
入口参数：	
			CanSendData：CAN发送的数据结构
			Spetsnaz：接收母线电压和母线电流的数据结构体
			axis: 选择电机0或电机1命令发送 规定 ： axis0=0  axis1=1 
返回值：无
应用范围：内部调用
备注：
	
	返回的数据前四位（低到高）是ODRIVE的母线电压数据，后四位返回的是ODRIVE的母线电流数据，都是FLOAT型
	所以用到了一个联合体进行数据转换，详情请看：
	//联合体用于转换数据
	typedef union{
		u8 		u8_temp[4];
		float float_temp;
		s32 	s32_temp;
		u32		u32_temp;
	} formatTrans32Struct_t;
***************************************************
*/
void ODReadVel_gainsData(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t axis) {
		
	
	Spetsnaz->Vel_gain[axis].u8_temp[0] = CanRevData->Data[0];	
	Spetsnaz->Vel_gain[axis].u8_temp[1] = CanRevData->Data[1];		
	Spetsnaz->Vel_gain[axis].u8_temp[2] = CanRevData->Data[2];		
	Spetsnaz->Vel_gain[axis].u8_temp[3] = CanRevData->Data[3];	
	
	Spetsnaz->Vel_integrator_gain[axis].u8_temp[0] = CanRevData->Data[4];	
	Spetsnaz->Vel_integrator_gain[axis].u8_temp[1] = CanRevData->Data[5];		
	Spetsnaz->Vel_integrator_gain[axis].u8_temp[2] = CanRevData->Data[6];		
	Spetsnaz->Vel_integrator_gain[axis].u8_temp[3] = CanRevData->Data[7];
}

/*
***************************************************
函数名：ODReadVel_gainsData
功能：读取速度环增益值
入口参数：	
			CanRevData：CAN接收的数据结构
			Spetsnaz：接收母线电压和母线电流的数据结构体
			num: 选择电机0或电机1命令发送 规定 ： axis0=0  axis1=1 
返回值：无
应用范围：内部调用
备注：
	
	返回的数据前四位（低到高）是ODRIVE的母线电压数据，后四位返回的是ODRIVE的母线电流数据，都是FLOAT型
	所以用到了一个联合体进行数据转换，详情请看：
	//联合体用于转换数据
	typedef union{
		u8 		u8_temp[4];
		float float_temp;
		s32 	s32_temp;
		u32		u32_temp;
	} formatTrans32Struct_t;
***************************************************
*/
void CG_Data(CanRxMsg* CanRevData,ODCanDataRecv_t* Spetsnaz,uint8_t num) {
		
	
	Spetsnaz->CG[num].u8_temp[0] = CanRevData->Data[0];	
	Spetsnaz->CG[num].u8_temp[1] = CanRevData->Data[1];
	Spetsnaz->CG[num].u8_temp[2] = CanRevData->Data[2];
	Spetsnaz->CG[num].u8_temp[3] = CanRevData->Data[3];
	
	
}


/*
***************************************************
函数名：set_axis_requested_state
功能：电机状态参数配置_数据帧
入口参数：	CANx：CAN1 orCAN2
			ID_CAN：CANID 电机0或电机1的ID地址 规定： AXIS0_ID 0x001   AXIS1_ID 0x002
			CMD_CAN：odrive的状态参数配置命令（MSG_SET_CONTROLLER_MODES ）
			len：数据帧长度
			CONTROL_MODE：电机控制模式
			CanSendData：CAN发送的数据结构
			Spetsnaz：发送的电机参数结构体
			axis: 选择电机0或电机1命令发送 规定 ： axis0=0  axis1=1 
返回值：无
应用范围：内部调用
备注： uint8_t len的数据范围
	 
    enum {
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
    }; 

***************************************************
*/
void set_axis_requested_state(CAN_TypeDef *CANx, uint32_t ID_CAN,uint32_t CMD_CAN, uint8_t len,OdriveStruct_t* Spetsnaz,uint8_t axis,ODCANSendStruct_t* CanSendData) {
	
	CanSendData->data[0] = Spetsnaz->AxisState[axis];
	CanSendData->data[1] = 0;
	CanSendData->data[2] = 0;
	CanSendData->data[3] = 0;

	
	OdriveSendData(CANx,ID_CAN,CMD_CAN,len,CanSendData); 
}



/*********************************以上为驱动层代码*****************************************************************************/



/*********************************以下为应用层代码*****************************************************************************/
ODCANSendStruct_t ODSendData;
static BSP_CAN_TypeDef can1;
static BSP_CAN_TypeDef can2;

/*
***************************************************
函数名：driver_can1_init
功能：电机CAN1初始化
入口参数：	rm_canx：使用的CAN通道
					rm_canx_rx：RM CAN接收引脚
					rm_canx_tx：RM CAN发送引脚
					Preemption：CAN中断抢占优先级
					Sub：CAN中断次优先级
返回值：无
应用范围：外部调用
备注： 波特率为1M
***************************************************
*/
//所以这里CANbps=APB1总线频率45M/3/(1+5+9))=1000k bps,3为分配系数,5为BS2时间单元,9为BS1时间单元
void driver_can1_init(CAN_TypeDef* rm_canx,BSP_GPIOSource_TypeDef *rm_canx_rx,BSP_GPIOSource_TypeDef *rm_canx_tx,u8 Preemption,u8 Sub){
	can1.CANx = rm_canx;
	can1.CANx_RX = rm_canx_rx;
	can1.CANx_TX = rm_canx_tx;
	if(rm_canx == CAN1){
		can1.CAN_FilterInitStructure = CAN1_FilterInitStructure;
	}
	else if(rm_canx == CAN2){
		can1.CAN_FilterInitStructure = CAN2_FilterInitStructure;
	}
	//1M波特率
	//BSP_CAN_Mode_Init(&can1,CAN_SJW_1tq,CAN_BS2_5tq,CAN_BS1_9tq,3,CAN_Mode_Normal,Preemption,Sub);
	
	//500K波特率
	BSP_CAN_Mode_Init(&can1,CAN_SJW_1tq,CAN_BS2_5tq,CAN_BS1_9tq,6,CAN_Mode_Normal,Preemption,Sub);
	//250K波特率
	//BSP_CAN_Mode_Init(&can1,CAN_SJW_1tq,CAN_BS2_5tq,CAN_BS1_9tq,12,CAN_Mode_Normal,Preemption,Sub);
	
	//125K波特率
	//BSP_CAN_Mode_Init(&can1,CAN_SJW_1tq,CAN_BS2_5tq,CAN_BS1_9tq,24,CAN_Mode_Normal,Preemption,Sub);
}

/*
***************************************************
函数名：driver_can2_init
功能：电机CAN2初始化
入口参数：	rm_canx：使用的CAN通道
					rm_canx_rx：RM CAN接收引脚
					rm_canx_tx：RM CAN发送引脚
					Preemption：CAN中断抢占优先级
					Sub：CAN中断次优先级
返回值：无
应用范围：外部调用
备注： 波特率为1M
***************************************************
*/
//所以这里CANbps=APB1总线频率45M/3/(1+5+9))=1000k bps,3为分配系数,5为BS2时间单元,9为BS1时间单元
void driver_can2_init(CAN_TypeDef* rm_canx,BSP_GPIOSource_TypeDef *rm_canx_rx,BSP_GPIOSource_TypeDef *rm_canx_tx,u8 Preemption,u8 Sub){
	can2.CANx = rm_canx;
	can2.CANx_RX = rm_canx_rx;
	can2.CANx_TX = rm_canx_tx;
	if(rm_canx == CAN1){
		can1.CAN_FilterInitStructure = CAN1_FilterInitStructure;
	}
	else if(rm_canx == CAN2){
		
	}
	can2.CAN_FilterInitStructure = CAN2_FilterInitStructure;
	//1M波特率
	//BSP_CAN_Mode_Init(&can1,CAN_SJW_1tq,CAN_BS2_5tq,CAN_BS1_9tq,3,CAN_Mode_Normal,Preemption,Sub);
	
	//250K波特率
	//BSP_CAN_Mode_Init(&can1,CAN_SJW_1tq,CAN_BS2_5tq,CAN_BS1_9tq,12,CAN_Mode_Normal,Preemption,Sub);
	
	//125K波特率
	BSP_CAN_Mode_Init(&can2,CAN_SJW_1tq,CAN_BS2_5tq,CAN_BS1_9tq,24,CAN_Mode_Normal,Preemption,Sub);
}

////     Frame
//// nodeID | CMD
//// 6 bits | 5 bits
//CanRxMsg can1_rx_msg;
//u32 rxbuf3;
//void CAN1_RX0_IRQHandler(void){
//	//CanRxMsg can1_rx_msg;
//	if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET){
//		// 清除中断标志和标志位
//		CAN_ClearITPendingBit(CAN1, CAN_IT_FF0);
//		CAN_ClearFlag(CAN1, CAN_FLAG_FF0);
//		
//		// 从接收 FIFO 中读取消息		
//		CAN_Receive(CAN1, CAN_FIFO0, &can1_rx_msg);
//		
//		// 存储接收到的标准 ID
//		rxbuf3=can1_rx_msg.StdId;
//		// 递增错误计数
//		
//		digitalIncreasing(&OdriveData.OdError.errorCount);

//		/*********以下是自定义部分**********/
//		switch(can1_rx_msg.StdId>>5){         
//				case AXIS0_ID:
//					//目前接收处理数据指令最大可支持到31
//					switch(can1_rx_msg.StdId&0x01F){ 

//						case MSG_ODRIVE_HEARTBEAT:
//							ODReadHeartBeatData(&can1_rx_msg,&OdReceivedData,axis0);
//						break;							
//						
//						case MSG_GET_VBUS_VOLTAGE:
//								ODReadVbusData(&can1_rx_msg,&OdReceivedData,axis0);
//						break;	
//						
////						case MSG_GET_TEMP:
////								ODReadTempData(&can1_rx_msg,&OdReceivedData,axis0);
//											
//						break;								
//					
//						case MSG_GET_MOTOR_ERROR:
//								ODReadMotorErrorData(&can1_rx_msg,&OdReceivedData,axis0);			
//						break;
//						
//						case MSG_GET_ENCODER_COUNT:
//								ODReadEncoderCountData(&can1_rx_msg,&OdReceivedData,axis0);			
//						break;						
//						
//						case MSG_GET_ENCODER_ESTIMATES:
//								ODReadEncodeEstimatesData(&can1_rx_msg,&OdReceivedData,axis0);			
//						break;								
//		
//						case MSG_GET_ENCODER_ERROR:
//								ODReadEncodeErrorData(&can1_rx_msg,&OdReceivedData,axis0);			
//						break;						
//							
//						
//						case MSG_SET_LIMITS:
//								ODReadLimitData(&can1_rx_msg,&OdReceivedData,axis0);			
//						break;

//						case MSG_GET_POS_GAIN:
//								ODReadPos_gainData(&can1_rx_msg,&OdReceivedData,axis0);
//								
//						break;
//						
//						case MSG_GET_VEL_GAINS:
//								ODReadVel_gainsData(&can1_rx_msg,&OdReceivedData,axis0);
//						break;
//						
//						case MSG_GET_MOTOR_TEMP:
//								ODReadTempData(&can1_rx_msg,&OdReceivedData,axis0);
//						break;
//						
//					default:	break;																			

//					
//					}						
//						
//				break;			
//			
//				case AXIS1_ID:							
//					switch(can1_rx_msg.StdId & 0x01F){ 

//						case MSG_ODRIVE_HEARTBEAT:
//							ODReadHeartBeatData(&can1_rx_msg,&OdReceivedData,axis1);
//						break;							
//						
//						case MSG_GET_VBUS_VOLTAGE:
//								ODReadVbusData(&can1_rx_msg,&OdReceivedData,axis1);
//											
//						break;	
//						
////						case MSG_GET_TEMP:
////								ODReadTempData(&can1_rx_msg,&OdReceivedData,axis1);
//											
//						break;								
//					
//						case MSG_GET_MOTOR_ERROR:
//								ODReadMotorErrorData(&can1_rx_msg,&OdReceivedData,axis1);			
//						break;
//						
//						case MSG_GET_ENCODER_COUNT:
//								ODReadEncoderCountData(&can1_rx_msg,&OdReceivedData,axis1);			
//						break;						
//						
//						case MSG_GET_ENCODER_ESTIMATES:
//								ODReadEncodeEstimatesData(&can1_rx_msg,&OdReceivedData,axis1);			
//						break;								
//		
//						case MSG_GET_ENCODER_ERROR:
//								ODReadEncodeErrorData(&can1_rx_msg,&OdReceivedData,axis1);			
//						break;						
//							
//						
//						case MSG_SET_LIMITS:
//								ODReadLimitData(&can1_rx_msg,&OdReceivedData,axis1);			
//						break;

//						case MSG_GET_VEL_GAINS:
//								ODReadVel_gainsData(&can1_rx_msg,&OdReceivedData,axis1);
//						break;
//						
//						case MSG_GET_POS_GAIN:
//								ODReadPos_gainData(&can1_rx_msg,&OdReceivedData,axis1);
//								
//						break;
//						
//						case MSG_GET_MOTOR_TEMP:
//								ODReadTempData(&can1_rx_msg,&OdReceivedData,axis1);
//						break;
//						
//					default:	break;			


//																							

//					
//					}
//					break;
//					
//					case AXIS2_ID:							
//					switch(can1_rx_msg.StdId & 0x01F){ 

//						case MSG_ODRIVE_HEARTBEAT:
//							ODReadHeartBeatData(&can1_rx_msg,&OdReceivedData,axis2);
//						break;							
//						
//						case MSG_GET_VBUS_VOLTAGE:
//								ODReadVbusData(&can1_rx_msg,&OdReceivedData,axis2);
//											
//						break;	
//						
////						case MSG_GET_TEMP:
////								ODReadTempData(&can1_rx_msg,&OdReceivedData,axis1);
//											
//						break;								
//					
//						case MSG_GET_MOTOR_ERROR:
//								ODReadMotorErrorData(&can1_rx_msg,&OdReceivedData,axis2);			
//						break;
//						
//						case MSG_GET_ENCODER_COUNT:
//								ODReadEncoderCountData(&can1_rx_msg,&OdReceivedData,axis2);			
//						break;						
//						
//						case MSG_GET_ENCODER_ESTIMATES:
//								ODReadEncodeEstimatesData(&can1_rx_msg,&OdReceivedData,axis2);			
//						break;								
//		
//						case MSG_GET_ENCODER_ERROR:
//								ODReadEncodeErrorData(&can1_rx_msg,&OdReceivedData,axis2);			
//						break;						
//							
//						
//						case MSG_SET_LIMITS:
//								ODReadLimitData(&can1_rx_msg,&OdReceivedData,axis2);			
//						break;

//						case MSG_GET_VEL_GAINS:
//								ODReadVel_gainsData(&can1_rx_msg,&OdReceivedData,axis2);
//						break;
//						
//						case MSG_GET_POS_GAIN:
//								ODReadPos_gainData(&can1_rx_msg,&OdReceivedData,axis2);
//								
//						break;
//						
//						case MSG_GET_MOTOR_TEMP:
//								ODReadTempData(&can1_rx_msg,&OdReceivedData,axis2);
//						break;
//						
//					default:	break;			


//																						
//					}
//					break;

//					case AXIS3_ID:							
//					switch(can1_rx_msg.StdId & 0x01F){ 

//						case MSG_ODRIVE_HEARTBEAT:
//							ODReadHeartBeatData(&can1_rx_msg,&OdReceivedData,axis3);
//						break;							
//						
//						case MSG_GET_VBUS_VOLTAGE:
//								ODReadVbusData(&can1_rx_msg,&OdReceivedData,axis3);
//											
//						break;	
//						
////						case MSG_GET_TEMP:
////								ODReadTempData(&can1_rx_msg,&OdReceivedData,axis1);
//											
//						break;								
//					
//						case MSG_GET_MOTOR_ERROR:
//								ODReadMotorErrorData(&can1_rx_msg,&OdReceivedData,axis3);			
//						break;
//						
//						case MSG_GET_ENCODER_COUNT:
//								ODReadEncoderCountData(&can1_rx_msg,&OdReceivedData,axis3);			
//						break;						
//						
//						case MSG_GET_ENCODER_ESTIMATES:
//								ODReadEncodeEstimatesData(&can1_rx_msg,&OdReceivedData,axis3);			
//						break;								
//		
//						case MSG_GET_ENCODER_ERROR:
//								ODReadEncodeErrorData(&can1_rx_msg,&OdReceivedData,axis3);			
//						break;						
//							
//						
//						case MSG_SET_LIMITS:
//								ODReadLimitData(&can1_rx_msg,&OdReceivedData,axis3);			
//						break;

//						case MSG_GET_VEL_GAINS:
//								ODReadVel_gainsData(&can1_rx_msg,&OdReceivedData,axis3);
//						break;
//						
//						case MSG_GET_POS_GAIN:
//								ODReadPos_gainData(&can1_rx_msg,&OdReceivedData,axis3);
//								
//						break;
//						
//						case MSG_GET_MOTOR_TEMP:
//								ODReadTempData(&can1_rx_msg,&OdReceivedData,axis3);
//						break;
//						
//					default:	break;			


//																							

//					
//					}
//						
//				break;					
//				
//				
//			default:	break;		

//        }


//	}
//}


///*
//***************************************************
//函数名：CAN1_TX_IRQHandler
//功能：CAN1发送中断
//备注：
//***************************************************
//*/
//void CAN1_TX_IRQHandler(void){
//	if (CAN_GetITStatus(CAN1,CAN_IT_TME)!= RESET){
//		CAN_ClearITPendingBit(CAN1,CAN_IT_TME);

//		/*********以下是自定义部分**********/
//        OdriveSend_RemoteCmd(CAN1,AXIS0_ID,MSG_GET_MOTOR_ERROR);
//        
//	}
//}



/*
***************************************************
函数名：CAN2_RX0_IRQHandler
功能：CAN2接收中断
备注：
***************************************************
*/
u32 rxbuf2;
void CAN2_RX0_IRQHandler(void){
	CanRxMsg can2_rx_msg;
	if (CAN_GetITStatus(CAN2,CAN_IT_FMP0)!= RESET){
		CAN_ClearITPendingBit(CAN2, CAN_IT_FF0);
		CAN_ClearFlag(CAN2, CAN_FLAG_FF0);		
		CAN_Receive(CAN2, CAN_FIFO0, &can2_rx_msg);
		
		rxbuf2=can2_rx_msg.StdId;	
		digitalIncreasing(&OdriveData.OdError.errorCount);
		/*********以下是自定义部分**********/
			
		switch(can2_rx_msg.StdId>>5){         
				case AXIS0_ID:
					//目前接收处理数据指令最大可支持到31
					switch(can2_rx_msg.StdId&0x01F){ 

						case MSG_ODRIVE_HEARTBEAT:
							ODReadHeartBeatData(&can2_rx_msg,&OdReceivedData,axis0);
						break;							
						
						case MSG_GET_VBUS_VOLTAGE:
								ODReadVbusData(&can2_rx_msg,&OdReceivedData,axis0);
						break;	
						
//						case MSG_GET_TEMP:
//								ODReadTempData(&can1_rx_msg,&OdReceivedData,axis0);
											
						break;								
					
						case MSG_GET_MOTOR_ERROR:
								ODReadMotorErrorData(&can2_rx_msg,&OdReceivedData,axis0);			
						break;
						
						case MSG_GET_ENCODER_COUNT:
								ODReadEncoderCountData(&can2_rx_msg,&OdReceivedData,axis0);			
						break;						
						
						case MSG_GET_ENCODER_ESTIMATES:
								ODReadEncodeEstimatesData(&can2_rx_msg,&OdReceivedData,axis0);			
						break;								
		
						case MSG_GET_ENCODER_ERROR:
								ODReadEncodeErrorData(&can2_rx_msg,&OdReceivedData,axis0);			
						break;						
							
						
						case MSG_SET_LIMITS:
								ODReadLimitData(&can2_rx_msg,&OdReceivedData,axis0);			
						break;

						case MSG_GET_POS_GAIN:
								ODReadPos_gainData(&can2_rx_msg,&OdReceivedData,axis0);
								
						break;
						
						case MSG_GET_VEL_GAINS:
								ODReadVel_gainsData(&can2_rx_msg,&OdReceivedData,axis0);
						break;
						
						case MSG_GET_MOTOR_TEMP:
								ODReadTempData(&can2_rx_msg,&OdReceivedData,axis0);
						break;
						
					default:	break;																			

					
					}						
						
				break;			
			
				case AXIS1_ID:							
					switch(can2_rx_msg.StdId & 0x01F){ 

						case MSG_ODRIVE_HEARTBEAT:
							ODReadHeartBeatData(&can2_rx_msg,&OdReceivedData,axis1);
						break;							
						
						case MSG_GET_VBUS_VOLTAGE:
								ODReadVbusData(&can2_rx_msg,&OdReceivedData,axis1);
											
						break;	
						
//						case MSG_GET_TEMP:
//								ODReadTempData(&can1_rx_msg,&OdReceivedData,axis1);
											
						break;								
					
						case MSG_GET_MOTOR_ERROR:
								ODReadMotorErrorData(&can2_rx_msg,&OdReceivedData,axis1);			
						break;
						
						case MSG_GET_ENCODER_COUNT:
								ODReadEncoderCountData(&can2_rx_msg,&OdReceivedData,axis1);			
						break;						
						
						case MSG_GET_ENCODER_ESTIMATES:
								ODReadEncodeEstimatesData(&can2_rx_msg,&OdReceivedData,axis1);			
						break;								
		
						case MSG_GET_ENCODER_ERROR:
								ODReadEncodeErrorData(&can2_rx_msg,&OdReceivedData,axis1);			
						break;						
							
						
						case MSG_SET_LIMITS:
								ODReadLimitData(&can2_rx_msg,&OdReceivedData,axis1);			
						break;

						case MSG_GET_VEL_GAINS:
								ODReadVel_gainsData(&can2_rx_msg,&OdReceivedData,axis1);
						break;
						
						case MSG_GET_POS_GAIN:
								ODReadPos_gainData(&can2_rx_msg,&OdReceivedData,axis1);
								
						break;
						
						case MSG_GET_MOTOR_TEMP:
								ODReadTempData(&can2_rx_msg,&OdReceivedData,axis1);
						break;
						
					default:	break;			


																							

					
					}
					break;
					
					case AXIS2_ID:							
					switch(can2_rx_msg.StdId & 0x01F){ 

						case MSG_ODRIVE_HEARTBEAT:
							ODReadHeartBeatData(&can2_rx_msg,&OdReceivedData,axis2);
						break;							
						
						case MSG_GET_VBUS_VOLTAGE:
								ODReadVbusData(&can2_rx_msg,&OdReceivedData,axis2);
											
						break;	
						
//						case MSG_GET_TEMP:
//								ODReadTempData(&can1_rx_msg,&OdReceivedData,axis1);
											
						break;								
					
						case MSG_GET_MOTOR_ERROR:
								ODReadMotorErrorData(&can2_rx_msg,&OdReceivedData,axis2);			
						break;
						
						case MSG_GET_ENCODER_COUNT:
								ODReadEncoderCountData(&can2_rx_msg,&OdReceivedData,axis2);			
						break;						
						
						case MSG_GET_ENCODER_ESTIMATES:
								ODReadEncodeEstimatesData(&can2_rx_msg,&OdReceivedData,axis2);			
						break;								
		
						case MSG_GET_ENCODER_ERROR:
								ODReadEncodeErrorData(&can2_rx_msg,&OdReceivedData,axis2);			
						break;						
							
						
						case MSG_SET_LIMITS:
								ODReadLimitData(&can2_rx_msg,&OdReceivedData,axis2);			
						break;

						case MSG_GET_VEL_GAINS:
								ODReadVel_gainsData(&can2_rx_msg,&OdReceivedData,axis2);
						break;
						
						case MSG_GET_POS_GAIN:
								ODReadPos_gainData(&can2_rx_msg,&OdReceivedData,axis2);
								
						break;
						
						case MSG_GET_MOTOR_TEMP:
								ODReadTempData(&can2_rx_msg,&OdReceivedData,axis2);
						break;
						
					default:	break;			


																						
					}
					break;

					case AXIS3_ID:							
					switch(can2_rx_msg.StdId & 0x01F){ 

						case MSG_ODRIVE_HEARTBEAT:
							ODReadHeartBeatData(&can2_rx_msg,&OdReceivedData,axis3);
						break;							
						
						case MSG_GET_VBUS_VOLTAGE:
								ODReadVbusData(&can2_rx_msg,&OdReceivedData,axis3);
											
						break;	
						
//						case MSG_GET_TEMP:
//								ODReadTempData(&can1_rx_msg,&OdReceivedData,axis1);
											
						break;								
					
						case MSG_GET_MOTOR_ERROR:
								ODReadMotorErrorData(&can2_rx_msg,&OdReceivedData,axis3);			
						break;
						
						case MSG_GET_ENCODER_COUNT:
								ODReadEncoderCountData(&can2_rx_msg,&OdReceivedData,axis3);			
						break;						
						
						case MSG_GET_ENCODER_ESTIMATES:
								ODReadEncodeEstimatesData(&can2_rx_msg,&OdReceivedData,axis3);			
						break;								
		
						case MSG_GET_ENCODER_ERROR:
								ODReadEncodeErrorData(&can2_rx_msg,&OdReceivedData,axis3);			
						break;						
							
						
						case MSG_SET_LIMITS:
								ODReadLimitData(&can2_rx_msg,&OdReceivedData,axis3);			
						break;

						case MSG_GET_VEL_GAINS:
								ODReadVel_gainsData(&can2_rx_msg,&OdReceivedData,axis3);
						break;
						
						case MSG_GET_POS_GAIN:
								ODReadPos_gainData(&can2_rx_msg,&OdReceivedData,axis3);
								
						break;
						
						case MSG_GET_MOTOR_TEMP:
								ODReadTempData(&can2_rx_msg,&OdReceivedData,axis3);
						break;
						
					default:	break;			


																							

					
					}
						
				break;					
				
				
			default:	break;		

        }
		

	}
}


/*
***************************************************
函数名：CAN2_TX_IRQHandler
功能：CAN2发送中断
备注：
***************************************************
*/
void CAN2_TX_IRQHandler(void){
	if (CAN_GetITStatus(CAN2,CAN_IT_TME)!= RESET){
		CAN_ClearITPendingBit(CAN2,CAN_IT_TME);

		/*********以下是自定义部分**********/
        
        
	}
}




void ODRequestedState(void){		
    //如果想要对状态参数进行操作，直接将OdriveData.flashSave拉高一次即可    
	if(OdriveData.RequestedStateFlag&&(OdriveData.AxisState[axis0] == AXIS_STATE_IDLE)){                                              
       //检测到需要改变状态参数		
		
		ODSET_CONTROL_MODE(CAN1,AXIS0_ID,MSG_SET_CONTROLLER_MODES,8,&OdriveData,axis0,&ODSendData);
		ODSET_CONTROL_MODE(CAN1,AXIS1_ID,MSG_SET_CONTROLLER_MODES,8,&OdriveData,axis1,&ODSendData);

		digitalLo(&OdriveData.RequestedStateFlag);         
        
	}

	if(OdriveData.RequestedStateFlag1&&(OdriveData.AxisState[axis0] == AXIS_STATE_IDLE)){                                              
       //检测到需要改变状态参数		
		
		ODSET_CONTROL_MODE(CAN1,AXIS2_ID,MSG_SET_CONTROLLER_MODES,8,&OdriveData,axis2,&ODSendData);
		ODSET_CONTROL_MODE(CAN1,AXIS3_ID,MSG_SET_CONTROLLER_MODES,8,&OdriveData,axis3,&ODSendData);

		digitalLo(&OdriveData.RequestedStateFlag1);         
        
	}

}

//保存参数
void ODFlashSave(void){
	//保存电机1、电机2数据	
	if(OdriveData.flashSaveFlag){                                              						
		//config
		OdriveSend_RemoteCmd(CAN1,AXIS0_ID,MSG_SAVE_CONFIG);
		OdriveSend_RemoteCmd(CAN1,AXIS1_ID,MSG_SAVE_CONFIG);

		digitalLo(&OdriveData.flashSaveFlag);         
        
	} 
	
	//保存电机3、电机4数据
	if(OdriveData.flashSaveFlag1){                                              						
		OdriveSend_RemoteCmd(CAN1,AXIS2_ID,MSG_SAVE_CONFIG);
		OdriveSend_RemoteCmd(CAN1,AXIS3_ID,MSG_SAVE_CONFIG);
		
		digitalLo(&OdriveData.flashSaveFlag1);         
        
	} 
}

//修改速度环增益参数
void ODChangeVel_gain(void){
	//修改电机1、电机2的速度环参数	
	if(OdriveData.Vel_gainFlag){                                              						
		ODSendVel_gainsData(CAN1,AXIS0_ID,MSG_SET_VEL_GAINS,8,&OdriveData,axis0,&ODSendData);
		ODSendVel_gainsData(CAN1,AXIS1_ID,MSG_SET_VEL_GAINS,8,&OdriveData,axis1,&ODSendData);
		digitalLo(&OdriveData.Vel_gainFlag);         
	}
	//修改电机3、电机4的速度环参数
	if(OdriveData.Vel_gainFlag1){                                              								
		ODSendVel_gainsData(CAN1,AXIS2_ID,MSG_SET_VEL_GAINS,8,&OdriveData,axis2,&ODSendData);
		ODSendVel_gainsData(CAN1,AXIS3_ID,MSG_SET_VEL_GAINS,8,&OdriveData,axis3,&ODSendData);
		
		digitalLo(&OdriveData.Vel_gainFlag1);         
        
	}
	
}

//修改位置环环增益参数
void ODChangePos_gain(void){
	//修改电机1、电机2的位置环参数
	if(OdriveData.Pos_gainFlag){                                              						
		//config
		
		ODSendPos_gainData(CAN1,AXIS0_ID,MSG_SET_POS_GAIN,4,&OdriveData,axis0,&ODSendData);
		ODSendPos_gainData(CAN1,AXIS1_ID,MSG_SET_POS_GAIN,4,&OdriveData,axis1,&ODSendData);
		digitalLo(&OdriveData.Pos_gainFlag);         
	}
	//修改电机3、电机4的位置环参数
	if(OdriveData.Pos_gainFlag1){                                              								
		ODSendPos_gainData(CAN1,AXIS2_ID,MSG_SET_POS_GAIN,4,&OdriveData,axis2,&ODSendData);
		ODSendPos_gainData(CAN1,AXIS3_ID,MSG_SET_POS_GAIN,4,&OdriveData,axis3,&ODSendData);
		
		digitalLo(&OdriveData.Pos_gainFlag1);         
        
	}
	
}



//清除电机错误
void ODClearError(void){		
    //如果想要对状态参数进行操作，直接将OdriveData.clearerror高一次即可
	//清除电机1、电机2错误    
	if(OdriveData.clearerrorFlag){                                              
		OdriveSend_RemoteCmd(CAN1,AXIS0_ID,MSG_CLEAR_ERRORS);
		OdriveSend_RemoteCmd(CAN1,AXIS1_ID,MSG_CLEAR_ERRORS);
		digitalLo(&OdriveData.clearerrorFlag);         
        
	}
	//清除电机3、电机4错误
	if(OdriveData.clearerrorFlag1){                                              
		OdriveSend_RemoteCmd(CAN1,AXIS2_ID,MSG_CLEAR_ERRORS);
		OdriveSend_RemoteCmd(CAN1,AXIS3_ID,MSG_CLEAR_ERRORS);

		digitalLo(&OdriveData.clearerrorFlag1);         
        
	}
}


//重启odrive
void ODRESET_ODRIVE(void){
	//重启电机1、电机2
	if(OdriveData.RebotFlag){                                              						
		//config
		OdriveSend_RemoteCmd(CAN1,AXIS0_ID,MSG_RESET_ODRIVE);
		OdriveSend_RemoteCmd(CAN1,AXIS1_ID,MSG_RESET_ODRIVE);
		digitalLo(&OdriveData.RebotFlag);         
        
	}
	//重启电机3、电机4
	if(OdriveData.RebotFlag1){                                              						
		//config
		OdriveSend_RemoteCmd(CAN1,AXIS2_ID,MSG_RESET_ODRIVE);
		OdriveSend_RemoteCmd(CAN1,AXIS3_ID,MSG_RESET_ODRIVE);
		digitalLo(&OdriveData.RebotFlag1);         
        
	}
}


//设置电机控制状态
void ODSetMotorState(void){		
    //如果想要对电机控制模式进行操作，直接将OdriveData.ControlModeFlag高一次即可,并且电机控制模式为CMD_MENU
	if((OdriveData.ControlModeFlag)){                                              
		//修改电机1、电机2状态
		set_axis_requested_state(CAN1,AXIS0_ID,MSG_SET_AXIS_REQUESTED_STATE,4,&OdriveData,axis0,&ODSendData);
		set_axis_requested_state(CAN1,AXIS1_ID,MSG_SET_AXIS_REQUESTED_STATE,4,&OdriveData,axis1,&ODSendData);
					
		digitalLo(&OdriveData.ControlModeFlag);         
        
	}

	if((OdriveData.ControlModeFlag1)){                                              
		//修改电机3、电机4状态
		set_axis_requested_state(CAN1,AXIS2_ID,MSG_SET_AXIS_REQUESTED_STATE,4,&OdriveData,axis2,&ODSendData);
		set_axis_requested_state(CAN1,AXIS3_ID,MSG_SET_AXIS_REQUESTED_STATE,4,&OdriveData,axis3,&ODSendData);
			
		digitalLo(&OdriveData.ControlModeFlag1);         
        
	}
}

//设置电机控制状态
void ODSetLimit(void){		
    //如果想要对电机速度电流限制进行操作，直接将OdriveData.SetLimitFlag高一次即可,并且电机控制模式为CMD_MENU
	if((OdriveData.SetLimitFlag)&&(OdriveData.AxisState[axis0] == AXIS_STATE_IDLE)){                                              
			

		ODSendLimitData(CAN1,AXIS0_ID,MSG_SET_LIMITS,8,&OdriveData,axis0,&ODSendData);
										
		digitalLo(&OdriveData.SetLimitFlag);         
        
	}
	
	if((OdriveData.SetLimitFlag)&&(OdriveData.AxisState[axis1] == AXIS_STATE_IDLE)){                                              
			

		ODSendLimitData(CAN1,AXIS1_ID,MSG_SET_LIMITS,8,&OdriveData,axis1,&ODSendData);
										
		digitalLo(&OdriveData.SetLimitFlag);         
        
	}
	
	if((OdriveData.SetLimitFlag)&&(OdriveData.AxisState[axis2] == AXIS_STATE_IDLE)){                                              
			

		ODSendLimitData(CAN1,AXIS2_ID,MSG_SET_LIMITS,8,&OdriveData,axis2,&ODSendData);
										
		digitalLo(&OdriveData.SetLimitFlag);         
        
	}
	
	if((OdriveData.SetLimitFlag)&&(OdriveData.AxisState[axis3] == AXIS_STATE_IDLE)){                                              
			

		ODSendLimitData(CAN1,AXIS3_ID,MSG_SET_LIMITS,8,&OdriveData,axis3,&ODSendData);
										
		digitalLo(&OdriveData.SetLimitFlag);         
        
	}
	
}



void OdriveGlobalInit(void){
	
	//初始化CAN1
    driver_can1_init(CAN1,BSP_GPIOB8,BSP_GPIOB9,4,0);//405
	//driver_can1_init(CAN1,BSP_GPIOD0,BSP_GPIOD1,4,0);//407
	//driver_can2_init(CAN2,BSP_GPIOB12,BSP_GPIOB13,4,0);
	
	CAN2_Mode_Init(CAN_SJW_1tq,CAN_BS2_5tq,CAN_BS1_9tq,3,CAN_Mode_Normal);
	
	//电机初始配置	
	OdriveData.AxisState[axis0]   = AXIS_STATE_CLOSED_LOOP_CONTROL;//使能电机1			
	OdriveData.ControlMode[axis0] = CONTROL_MODE_VELOCITY_RAMP;//速度梯形模式
	OdriveData.current_limit[axis0].float_temp = 15.0f;
	OdriveData.vel_limit[axis0].float_temp = 35.0f;
	
	OdriveData.AxisState[axis1]   = AXIS_STATE_CLOSED_LOOP_CONTROL;//使能电机2			
	OdriveData.ControlMode[axis1] = CONTROL_MODE_VELOCITY_RAMP;//速度梯形模式
	OdriveData.current_limit[axis1].float_temp = 15.0f;
	OdriveData.vel_limit[axis1].float_temp = 35.0f;
	
	OdriveData.AxisState[axis2]   = AXIS_STATE_CLOSED_LOOP_CONTROL;//使能电机3			
	OdriveData.ControlMode[axis2] = CONTROL_MODE_VELOCITY_RAMP;//速度梯形模式
	OdriveData.current_limit[axis2].float_temp = 15.0f;
	OdriveData.vel_limit[axis2].float_temp = 35.0f;
	
	OdriveData.AxisState[axis3]   = AXIS_STATE_CLOSED_LOOP_CONTROL;//使能电机4			
	OdriveData.ControlMode[axis3] = CONTROL_MODE_VELOCITY_RAMP;//速度梯形模式
	OdriveData.current_limit[axis3].float_temp = 15.0f;
	OdriveData.vel_limit[axis3].float_temp = 35.0f;
	
}

//ODrive控制更新任务
void odrivelUpdateTask(void *Parameters){
	//获取当前任务运行的时间
	TickType_t xLastWakeTime = xTaskGetTickCount();
	//拉低状态
	digitalLo(&OdriveData.dataInitFlag);
	while(true){
		
		vTaskDelayUntil(&xLastWakeTime,ODRIVE_NORMAL_PERIOD);
        //防止重复初始化
		if(!OdriveData.dataInitFlag){	
            //所有控制全部初始化            
			OdriveGlobalInit();
			
			digitalHi(&OdriveData.dataInitFlag);
			
		}
		
	//如果电机处于闭环状态，发送力矩/位置/速度控制指令  频率100HZ
	if(EW.change == 0){
	if(OdriveData.AxisState[axis0] == AXIS_STATE_CLOSED_LOOP_CONTROL){
					
					switch(OdriveData.ControlMode[axis0]){ 
		
						case CONTROL_MODE_CURRENT:
						case CONTROL_MODE_CURRENT_RAMP:

							 //通过这个变量设置力矩
							//OdriveData.SetCur[0].float_temp;	
						
							//发送力矩命令
							ODSendInputCurData(CAN1,AXIS0_ID,MSG_SET_INPUT_TORQUE,4,&OdriveData,axis0,&ODSendData);	
							ODSendInputCurData(CAN1,AXIS1_ID,MSG_SET_INPUT_TORQUE,4,&OdriveData,axis0,&ODSendData);
						break;	
						
						case CONTROL_MODE_VELOCITY:
						case CONTROL_MODE_VELOCITY_RAMP:
							
							//通过这个变量设置速度
							//OdriveData.SetVel[0].float_temp;
						
							//发送速度命令
//							if(flag == 1){
								
								if(NFC.flag == 0){
//									ODSendInputVelData(CAN1,AXIS0_ID,MSG_SET_INPUT_VEL,4,&OdriveData,axis0,&ODSendData);
//									ODSendInputVelData(CAN1,AXIS2_ID,MSG_SET_INPUT_VEL,4,&OdriveData,axis0,&ODSendData);
									
									ODSendInputVelData(CAN2,AXIS0_ID,MSG_SET_INPUT_VEL,8,&OdriveData,axis0,&ODSendData);
									ODSendInputVelData(CAN2,AXIS2_ID,MSG_SET_INPUT_VEL,8,&OdriveData,axis0,&ODSendData);
								}
								NFC.flag++;
//								ODSendInputVelData(CAN2,AXIS1_ID,MSG_SET_INPUT_VEL,4,&OdriveData,axis1,&ODSendData);
//								ODSendInputVelData(CAN2,AXIS0_ID,MSG_SET_INPUT_VEL,4,&OdriveData,axis0,&ODSendData);
//								flag = 0;
//							}
						break;
						case CONTROL_MODE_POSITION:
						case CONTROL_MODE_POSITION_TRAP:

							//通过这个变量设置位置						
							//OdriveData.SetPos[0].float_temp;
						
							//发送位置命令
							ODSendInputPosData(CAN1,AXIS0_ID,MSG_SET_INPUT_POS,4,&OdriveData,axis0,&ODSendData);
							//ODSendInputPosData(CAN1,AXIS1_ID,MSG_SET_INPUT_POS,4,&OdriveData,axis0,&ODSendData);
						
						break;																		
					default:	break;																								
					}			
		
	}
	//如果电机处于闭环状态，发送力矩/位置/速度控制指令  频率100HZ
	if(OdriveData.AxisState[axis1] == AXIS_STATE_CLOSED_LOOP_CONTROL){

					switch(OdriveData.ControlMode[axis1]){ 
		
						case CONTROL_MODE_CURRENT:
						case CONTROL_MODE_CURRENT_RAMP:

							 //通过这个变量设置力矩
							//OdriveData.SetCur[0].float_temp;	
						
							//发送力矩命令
							ODSendInputCurData(CAN1,AXIS1_ID,MSG_SET_INPUT_TORQUE,4,&OdriveData,axis1,&ODSendData);	
						break;	
						
						case CONTROL_MODE_VELOCITY:
						case CONTROL_MODE_VELOCITY_RAMP:
							
							//通过这个变量设置速度
							//OdriveData.SetVel[0].float_temp;
						
							//发送速度命令
//								if(flag == 1){
								
								if(NFC.flag == 2){
//									ODSendInputVelData(CAN1,AXIS1_ID,MSG_SET_INPUT_VEL,6,&OdriveData,axis1,&ODSendData);
//									ODSendInputVelData(CAN1,AXIS3_ID,MSG_SET_INPUT_VEL,6,&OdriveData,axis1,&ODSendData);
									
									ODSendInputVelData(CAN2,AXIS1_ID,MSG_SET_INPUT_VEL,8,&OdriveData,axis1,&ODSendData);
									ODSendInputVelData(CAN2,AXIS3_ID,MSG_SET_INPUT_VEL,8,&OdriveData,axis1,&ODSendData);
									NFC.flag = 0;
								}
								
								//ODSendInputVelData(CAN1,AXIS0_ID,MSG_SET_INPUT_VEL,4,&OdriveData,axis0,&ODSendData);
								
//								ODSendInputVelData(CAN2,AXIS1_ID,MSG_SET_INPUT_VEL,4,&OdriveData,axis1,&ODSendData);
//								ODSendInputVelData(CAN2,AXIS0_ID,MSG_SET_INPUT_VEL,4,&OdriveData,axis0,&ODSendData);
//								flag = 0;
//							}
								
						
						break;							
						case CONTROL_MODE_POSITION:
						case CONTROL_MODE_POSITION_TRAP:

							//通过这个变量设置位置						
							//OdriveData.SetPos[0].float_temp;
						
							//发送位置命令
							//ODSendInputPosData(CAN1,AXIS1_ID,MSG_SET_INPUT_POS,4,&OdriveData,axis1,&ODSendData);	
						
						break;																		
					default:	break;																								
					}			
		
	}
}
	
	//通过这个参数修改位置环增益参数
	//OdriveData.Pos_gain[0].float_temp = test;
	//发送位置环增益参数修改命令
	
	//10Hz  轮循发送　
	if(!((OdriveData.loops + 1) % 100)){	

		ODSetMotorState();		
		//更改状态信息
		ODRequestedState();		
////	//保存参数
		ODFlashSave();
////	//清除错误
		ODClearError();
		
		//重启odrive
		ODRESET_ODRIVE();
		
//		//设置电机电流/速度限制
//		ODSetLimit();
//		


		
		//读取编码器CPR值
//		OdriveSend_RemoteCmd(CAN1,AXIS1_ID,MSG_GET_ENCODER_COUNT);
		
		//修改速度环增益
		ODChangeVel_gain();
		
		//修改位置环增益
		ODChangePos_gain();
		
	}
        //vofa_sendData(OdReceivedData.pos_estimate[0].float_temp,OdReceivedData.pos_estimate[1].float_temp,(OdReceivedData.vel_estimate[0].float_temp * 0.2199f),(OdReceivedData.vel_estimate[1].float_temp * 0.2199f),EW.Current_Mileage,OdReceivedData.ibus[0].float_temp,OdReceivedData.vbus_voltage[0].float_temp,OdReceivedData.Pos_gain[0].float_temp,OdReceivedData.Vel_gain[0].float_temp,OdReceivedData.Vel_integrator_gain[0].float_temp,OdReceivedData.Pos_gain[1].float_temp,OdReceivedData.Vel_gain[1].float_temp,OdReceivedData.Vel_integrator_gain[1].float_temp,Motor_SpeedB_Goal.target);
		digitalIncreasing(&OdriveData.loops);        

	}
}

void OdriveInit(void){
	getsupervisorData()->taskEvent[ODRIVE_TASK] = xTaskCreate(odrivelUpdateTask,"ODRIVE",ODRIVE_STACK_SIZE,NULL,ODRIVE_PRIORITY,&OdriveData.xHandleTask);
    
}




