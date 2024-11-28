#include "parameter.h"
#include "pid.h"
#include "board.h"
//pidStruct_t *pidInit(systemConfigPID_t *PIDConfigData){
//	pidStruct_t *pid;
//    
//	pid = (pidStruct_t *)aqDataCalloc(1, sizeof(pidStruct_t));

//	pid->pMax = &PIDConfigData->PID_PM;
//	pid->iMax = &PIDConfigData->PID_IM;
//	pid->dMax = &PIDConfigData->PID_DM;
//	pid->oMax = &PIDConfigData->PID_OM;
//	pid->pGain = &PIDConfigData->PID_P;
//	pid->iGain = &PIDConfigData->PID_I;
//	pid->dGain = &PIDConfigData->PID_D;
//	pid->fGain = &PIDConfigData->PID_F;
//	pid->pTrim = NULL;
//	pid->iTrim = NULL;
//	pid->dTrim = NULL;
//	pid->fTrim = NULL;

//	return pid;
//}

///*
//@ param differential:	differential data
//@ param currentData:	new data
//@ return:							update data
//*/
//f32_t differentialCal(differentialDataStruct_t *differential,f32_t currentData){
//	uint8_t loopNum = differential -> differentialLength;
//	differential -> inputData[loopNum - 1] = currentData;
//	
//	/* H(z)计算更新 */
//	differential -> outputData[loopNum - 1] = 0.0f;
//	for(uint8_t i = 0;i < loopNum;i++){
//		differential -> outputData[loopNum - 1] +=	\
//		differential -> coefficient -> xCoefficient[i] * differential -> inputData[loopNum - 1 - i];
//		
//		if(i != loopNum - 1)
//			differential -> outputData[loopNum - 1] -=	\
//			differential -> coefficient -> yCoefficient[i + 1] * differential -> outputData[loopNum - 1 - i - 1];
//	}

//	/* x(n) 序列保存 */
//	for(uint8_t i = 0;i < loopNum - 1;i++){
//			differential->inputData[i] = differential -> inputData[i + 1];
//	}
//	
//	/* y(n) 序列保存 */
//	for(uint8_t i = 0;i < loopNum - 1;i++){
//			differential->outputData[i] = differential -> outputData[i + 1];
//	}
//	return (differential->outputData[loopNum - 1]);
//}






//float pidUpdate(pidStruct_t *pid, float setpoint, float position,float Dt){
//    float error;
//    float p = *pid->pGain;
//    float i = *pid->iGain;
//    float d = (pid->dGain) ? *pid->dGain : 0.0f;
//    //如果地址存在输入，则取地址内的值，没有则取1
//    float f = (pid->fGain) ? *pid->fGain : 1.0f;															

//    error = setpoint - position;

//    //计算比例项                                       
//    pid->pTerm_1 = p * error;
//    if (pid->pTerm_1 > *pid->pMax) {
//			pid->pTerm_1 = *pid->pMax;
//    }
//    else if (pid->pTerm_1 < -*pid->pMax) {
//			pid->pTerm_1 = -*pid->pMax;
//    }
//    //用适当的极限计算积分状态   
//    pid->iState += error;
//    pid->iTerm_1 = i * pid->iState * Dt;
//    if (pid->iTerm_1 > *pid->iMax) {
//			pid->iTerm_1 = *pid->iMax;
//			pid->iState = pid->iTerm_1 / i;
//    }
//    else if (pid->iTerm_1 < -*pid->iMax) {
//			pid->iTerm_1 = -*pid->iMax;
//			pid->iState = pid->iTerm_1 / i;
//    }

//    //微分																															
//    if (pid->dGain) { //如果存在微分项																													
//			error = -position;
//        
//            //在此处去除时间系数
//			pid->dTerm_1 = (d * f) * (error - pid->dState);													
//			pid->dState += f * (error - pid->dState);
//			if (pid->dTerm_1 > *pid->dMax) {
//					pid->dTerm_1 = *pid->dMax;
//			}
//			else if (pid->dTerm_1 < -*pid->dMax) {
//					pid->dTerm_1 = -*pid->dMax;
//			}
//    }
//    else {  //不存在则微分输出取0                       
//			pid->dTerm_1 = 0.0f;																										
//    }

//    pid->pv_1 = position;
//    pid->sp_1 = setpoint;
//    pid->co_1 = pid->pTerm_1 + pid->iTerm_1 + pid->dTerm_1;

//    //给PID输出限幅
//    if (pid->co_1 > *pid->oMax) {																							
//			pid->co_1 = *pid->oMax;
//    }
//    else if (pid->co_1 < -*pid->oMax) {
//			pid->co_1 = -*pid->oMax;
//    }

//    return pid->co_1;
//}

//void pidZeroIntegral(pidStruct_t *pid, float pv, float iState){
//	if (*pid->iGain != 0.0f)
//		pid->iState = iState / *pid->iGain;
//	pid->dState = -pv;
//	pid->sp_1 = pv;
//	pid->co_1 = 0.0f;
//	pid->pv_1 = pv;
//	pid->pv_2 = pv;
//}

//void pidZeroState(pidStruct_t *pid){
//	pid->setPoint = 0.0f;
//	pid->dState = 0.0f;
//	pid->iState = 0.0f;
//	pid->co_1 = 0.0f;
//	pid->pv_1 = 0.0f;
//	pid->sp_1 = 0.0f;
//	pid->pTerm_1 = 0.0f;
//	pid->iTerm_1 = 0.0f;
//	pid->dTerm_1 = 0.0f;
//}


/**************************************************************************
函数功能：限幅函数
入口参数：insert幅值,low最小值,high最大值
返回  值：限幅后的值
**************************************************************************/
float target_limit_float(float insert,float low,float high)
{
    if (insert < low)
        return low;
    else if (insert > high)
        return high;
    else
        return insert;	
}
int target_limit_int(int insert,int low,int high)
{
    if (insert < low)
        return low;
    else if (insert > high)
        return high;
    else
        return insert;	
}



/**************************************************************************
函数功能：角度转向PID
入口参数：angle_target期望角度,angle_current当前角度,limit输出限制（速度）
返回  值：限幅后的值
**************************************************************************/
float PID_angel(float angle_target, float angle_current,float limit){
	
	//PID输出值
	static float motor_out;
	//定义当前偏差,上一个偏差,累差值
	static float bias,bias_last,err_num,Kp = 0.075f,Ki = 0.0000f,Kd = 0.5f;
	
	//获得偏差值,用于微分,起到阻尼的作用,防超调
	bias = angle_target - angle_current;
	//误差限幅，限制转向角度的最大误差值,
	bias = target_limit_float(bias,-90,90);
	//还没完成转向,标志位为0
	//获得累差值,用于积分,接近真实值
	err_num += bias;
	
	//PID计算电机输出PWM值
	motor_out = Kp*bias + Ki*err_num + Kd*(bias-bias_last);
	
	//记录上次偏差
	bias_last = bias;
	
	
	//限制最大输出
	if(motor_out > limit)
		motor_out = limit;
	if(motor_out < -limit)
		motor_out = -limit;
	
//	//提前刹停,利用惯性转完
	if(fabs(bias) < 0.25){
		err_num = 0;
		bias_last = 0;
		motor_out = 0;
		Angle_Goal.change++;
		if(Angle_Goal.change>50){
			Angle_Goal.finish = 1;
			Angle_Goal.change = 0;
		}
	}else{
		Angle_Goal.finish = 0;
		Angle_Goal.change = 0;
	}

	//返回脉冲频率值
	return motor_out;
}

/**************************************************************************
函数功能：位置式PID控制器
入口参数：实际位置，目标位置
返回  值：电机PWM
根据位置式离散PID公式
位置式PID的三个参数: Kp:提高响应速度 Ki:稳态 Kd:抑制震荡
pwm=Kp*e(k)+Ki*∑e(k)+Kd[e（k）-e(k-1)]
e(k)代表本次偏差 
e(k-1)代表上一次的偏差  
∑e(k)代表e(k)以及之前的偏差的累积和;其中k为1,2,...,k;
pwm代表输出
**************************************************************************/
float Position_PID(float reality,float target)
{ 	
    static float Bias,Pwm,Last_Bias,Integral_bias=0,Position_KP,Position_KI,Position_KD;
    Position_KP = 0.35f;
	Position_KI = 0.005f;
	Position_KD = 0.0f;
	
    Bias=target-reality;                            /* 计算偏差 */
    Integral_bias+=Bias;	                        /* 偏差累积 */
    
    if(Integral_bias> 4) Integral_bias = 4;   	/* 积分限幅 */
    if(Integral_bias<-4) Integral_bias =-4;
    
    Pwm = (Position_KP*Bias)                        /* 比例环节 */
         +(Position_KI*Integral_bias)               /* 积分环节 */
         +(Position_KD*(Bias-Last_Bias));           /* 微分环节 */
    
    Last_Bias=Bias;                                 	/* 保存上次偏差 */
	
	//限幅
//	if(Pwm >  0.9f) Pwm =  0.9f;
//	if(Pwm < -0.9f) Pwm = -0.9f;						/* 输出限幅 */
	
	if(Pwm >  0.1f) Pwm =  0.1f;
	if(Pwm < -0.1f) Pwm = -0.1f;						/* 输出限幅 */
	
//	if(fabs(Bias)<0.15){
//		Pwm = 0;
//	}
	
    return Pwm;                                     	/* 输出结果 */
}

/**************************************************************************
函数功能：增量PID控制器
入口参数：实际值，目标值
返回  值：电机PWM
根据增量式离散PID公式 
增量式PID的三个参数: Kp:抑制震荡 Ki:提高响应速度 Kd:稳态
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)代表本次偏差 
e(k-1)代表上一次的偏差  以此类推 
pwm代表增量输出
**************************************************************************/
float Incremental_PID(float reality,float target)
{ 	


	 static float Bias,Pwm,Last_bias=0,Prev_bias=0,Incremental_KP,Incremental_KI,Incremental_KD,last_dev,this_dev,alpha;
//   Incremental_KP = 0.00125f;
//	 Incremental_KI = 0.00125f;
//	 Incremental_KD = 0.00f;
	 
	 alpha = 0.0;
	 //5ms状态下
	 Incremental_KP = 0.05f;
	 Incremental_KI = 0.05f;
	 Incremental_KD = 0.00f;
	
//	 Incremental_KP = 0.000125f;
//	 Incremental_KI = 0.001f;
//	 Incremental_KD = 0.00f;
	 
	 if(target == 0){
		 Bias = 0;
		 Last_bias = 0;
		 Pwm = 0;
		 Prev_bias = 0;
	 }
	 Bias=target - reality;                                   /* 计算偏差 */

    
	 Pwm += (Incremental_KP*(Bias-Last_bias))               /* 比例环节 */
           +(Incremental_KI*Bias)                           /* 积分环节 */
           +(Incremental_KD*(Bias-2*Last_bias+Prev_bias));  /* 微分环节 */ 
    
     Prev_bias=Last_bias;                                   /* 保存上上次偏差 */
	 Last_bias=Bias;	                                    /* 保存上一次偏差 */
		
//	 if(fabs(Bias)<0.75){
//		 Prev_bias = 0;
//		 Last_bias = 0;
//		 Pwm = 0;
//	 }
    
	//限幅
	if(Pwm >  3.5f) Pwm =  3.5f;
	if(Pwm < -3.5f) Pwm = -3.5f;
	
	//一阶低通滤波
	this_dev = Pwm * (1 - alpha) + alpha * last_dev;
	//保存上一次滤波的值
	last_dev = this_dev;
	
	if(this_dev >  3.5f) this_dev =  3.5f;
	if(this_dev < -3.5f) this_dev = -3.5f;
	return this_dev;                                            /* 输出结果 */
}

//位置式PID计算
float Position_based_PID(float target,float current){
	static float err,err_num,Pwm,err_last,Kp,Ki,Kd;
	
	
	
	//计算误差
	err = target - current;
	if(fabs(err) > 100){
		err = 0;
	}
	//误差累计
	err_num += err;
	
	
	
	//
	//低速情况下转速
	Kp = 0.05f;
	Ki = 0.000f;
	Kd = 0.5f;
	//高速情况下
//	Kp = 2.5f;
//	Ki = 0.0005f;
//	Kd = 5.0f;
	
	//分段PID
	if(err < 5){ 
		Kp = 0.05f;
		Ki = 0.000f;
		Kd = 0.5f;
	}
	
	//死区
	Pwm = Kp*err + Ki*err_num + Kd*(err - err_last);
	//误差传递
	err_last = err;
	
	//限幅

	if(Pwm>0.1) Pwm  =  0.1;
	if(Pwm<-0.1)Pwm  = -0.1;
	
	return Pwm;
}


/**************************************************************************
函数功能：位置式PID控制器
入口参数：实际位置，目标位置
返回  值：电机PWM
根据位置式离散PID公式
位置式PID的三个参数: Kp:提高响应速度 Ki:稳态 Kd:抑制震荡
pwm=Kp*e(k)+Ki*∑e(k)+Kd[e（k）-e(k-1)]
e(k)代表本次偏差 
e(k-1)代表上一次的偏差  
∑e(k)代表e(k)以及之前的偏差的累积和;其中k为1,2,...,k;
pwm代表输出
**************************************************************************/
float Position_PID_G(float reality,float target)
{ 	
    static float Bias,Pwm,Last_Bias,Integral_bias=0,Position_KP,Position_KI,Position_KD;
	
	//不使用积分
	Position_KP = 0.055f;
	Position_KI = 0.001f;
	Position_KD = 7.0f;
	
//	//使用积分
//	Position_KP = 0.045f;
//	Position_KI = 0.0001f;
//	Position_KD = 4.5f;
	
//	Position_KP = 0.20f;
//	Position_KI = 0.0000f;
//	Position_KD = 00.0f;
	
    Bias=target-reality;                            /* 计算偏差 */
    Integral_bias+=Bias;	                        /* 偏差累积 */
    
    if(Integral_bias> 100) Integral_bias = 100;   	/* 积分限幅 */
    if(Integral_bias<-100) Integral_bias =-100;
    
    Pwm = (Position_KP*Bias)                        /* 比例环节 */
         +(Position_KI*Integral_bias)               /* 积分环节 */
         +(Position_KD*(Bias-Last_Bias));           /* 微分环节 */
    
    Last_Bias=Bias;                                 	/* 保存上次偏差 */
	
	//限幅
	if(Pwm >  1.5f) Pwm =  1.5f;
	if(Pwm < -1.5f) Pwm = -1.5f;						/* 输出限幅 */
	

	
	if(fabs(Bias)<0.75){
		Pwm = 0;
	}
	
    return Pwm;                                     	/* 输出结果 */
}


float Position_PID_P(float reality,float target)
{ 	
    static float Bias,Pwm,Last_Bias,Integral_bias=0,Position_KP,Position_KI,Position_KD;
	
	//不使用积分
	Position_KP = 0.055f;
	Position_KI = 0.001f;
	Position_KD = 7.0f;
	
	//使用积分
//	Position_KP = 0.045f;
//	Position_KI = 0.0000f;
//	Position_KD = 4.5f;
	
//	Position_KP = 0.20f;
//	Position_KI = 0.0000f;
//	Position_KD = 00.0f;
	
    Bias=target-reality;                            /* 计算偏差 */
    Integral_bias+=Bias;	                        /* 偏差累积 */
    
    if(Integral_bias> 100) Integral_bias = 100;   	/* 积分限幅 */
    if(Integral_bias<-100) Integral_bias =-100;
    
    Pwm = (Position_KP*Bias)                        /* 比例环节 */
         +(Position_KI*Integral_bias)               /* 积分环节 */
         +(Position_KD*(Bias-Last_Bias));           /* 微分环节 */
    
    Last_Bias=Bias;                                 	/* 保存上次偏差 */
	
	//限幅
	if(Pwm >  1.5f) Pwm =  1.5f;
	if(Pwm < -1.5f) Pwm = -1.5f;						/* 输出限幅 */
	

	
	if(fabs(Bias)<0.75){
		Pwm = 0;
	}
	
    return Pwm;                                     	/* 输出结果 */
}

/**************************************************************************
函数功能：不完全微分位置式PID控制器
入口参数：实际位置，目标位置
返回  值：电机PWM
根据位置式离散PID公式
位置式PID的三个参数: Kp:提高响应速度 Ki:稳态 Kd:抑制震荡
pwm=Kp*e(k)+Ki*∑e(k)+Kd[e（k）-e(k-1)]
e(k)代表本次偏差 
e(k-1)代表上一次的偏差  
∑e(k)代表e(k)以及之前的偏差的累积和;其中k为1,2,...,k;
pwm代表输出
**************************************************************************/
float Position_PID_N(float reality,float target)
{ 	
    static float Bias,Pwm,Last_Bias,Integral_bias=0,Position_KP,Position_KI,Position_KD,last_dev,this_dev,alpha,last_Input,dInput;
	//使用积分
	Position_KP = 0.1f;
	Position_KI = 0.0000f;
	Position_KD = 9.0f;
	
	//低通滤波系数
	alpha = 0.1;

    Bias=target-reality;                            /* 计算偏差 */
    Integral_bias+=Bias;	                        /* 偏差累积 */
	dInput = reality - last_Input;					/* 计算输入偏差 */
    
    if(Integral_bias> 100) Integral_bias = 100;   	/* 积分限幅 */
    if(Integral_bias<-100) Integral_bias =-100;
    
	//一阶低通滤波微分部分处理
	this_dev = Position_KD * (1 - alpha) * (Bias - Last_Bias) + alpha * last_dev;
	
//	//微分冲击之后低通滤波处理
//	this_dev = Position_KD * (1 - alpha) * dInput + alpha * last_dev;
	
    Pwm = (Position_KP*Bias)                        /* 比例环节 */
         +(Position_KI*Integral_bias)               /* 积分环节 */
         +(this_dev);           					/* 微分环节 */
		 
    
    Last_Bias=Bias;                                 	/* 保存上次偏差 */
	
	//限幅
	if(Pwm >  1.7f) Pwm =  1.7f;
	if(Pwm < -1.7f) Pwm = -1.7f;						/* 输出限幅 */
	
	
	//记录上一个低通微分处理之后的值
	last_dev = this_dev;
	
	//死区
	if(fabs(Bias)<0.7){
		Pwm = 0;
		Integral_bias = 0;
	}
	
	//保存上一次的输入值
	last_Input = reality;
    return Pwm;                                     	/* 输出结果 */
}



/**************************************************************************
函数功能：不完全微分位置式PID控制器
入口参数：实际位置，目标位置
返回  值：电机PWM
根据位置式离散PID公式
位置式PID的三个参数: Kp:提高响应速度 Ki:稳态 Kd:抑制震荡
pwm=Kp*e(k)+Ki*∑e(k)+Kd[e（k）-e(k-1)]
e(k)代表本次偏差 
e(k-1)代表上一次的偏差  
∑e(k)代表e(k)以及之前的偏差的累积和;其中k为1,2,...,k;
pwm代表输出
**************************************************************************/
float Position_PID_N1(float reality,float target)
{ 	
    static float Bias,Pwm,Last_Bias,Integral_bias=0,Position_KP,Position_KI,Position_KD,last_dev,this_dev,alpha;
	//使用积分
	Position_KP = 0.1f;
	Position_KI = 0.0000f;
	Position_KD = 9.5f;
	
	//低通滤波系数
	alpha = 0.1;

    Bias=target-reality;                            /* 计算偏差 */
    Integral_bias+=Bias;	                        /* 偏差累积 */
    
    if(Integral_bias> 100) Integral_bias = 100;   	/* 积分限幅 */
    if(Integral_bias<-100) Integral_bias =-100;
    
	//一阶低通滤波微分部分处理
	this_dev = Position_KD * (1 - alpha) * (Bias - Last_Bias) + alpha * last_dev;
	
    Pwm = (Position_KP*Bias)                        /* 比例环节 */
         +(Position_KI*Integral_bias)               /* 积分环节 */
         +(this_dev);           					/* 微分环节 */
		 
    
    Last_Bias=Bias;                                 	/* 保存上次偏差 */
	
	//限幅
	if(Pwm >  1.7f) Pwm =  1.7f;
	if(Pwm < -1.7f) Pwm = -1.7f;						/* 输出限幅 */
	
	
	//记录上一个低通微分处理之后的值
	last_dev = this_dev;
	
	//死区
	if(fabs(Bias)<0.7){
		Pwm = 0;
		Integral_bias = 0;
	}
	
    return Pwm;                                     	/* 输出结果 */
}


//用于初始化pid参数的函数
void PID_Init(PID *pid, float p, float i, float d, float maxI, float maxOut)
{
    pid->kp = p;
    pid->ki = i;
    pid->kd = d;
    pid->maxIntegral = maxI;
    pid->maxOutput = maxOut;
	
	//低通滤波器设置
	pid->alpha = 0.1f;
	pid->this_dev = 0;
	pid->last_dev = 0;
}
 
//进行一次pid计算
//参数为(pid结构体,目标值,反馈值)，计算结果放在pid结构体的output成员中
void PID_Calc(PID *pid, float reference, float feedback)
{
 	//更新数据
    pid->lastError = pid->error; //将旧error存起来
    pid->error = reference - feedback; //计算新error
		
    //计算低通滤波之后的微分
	pid->this_dev = (1 - pid->alpha) * pid->kd * (pid->error - pid->lastError) + pid->alpha * pid->last_dev; 

    //计算比例
    pid->pout = pid->error * pid->kp;
    //计算积分
    pid->integral += pid->error * pid->ki;
	
    //积分限幅
    if(pid->integral > pid->maxIntegral) pid->integral = pid->maxIntegral;
    else if(pid->integral < -pid->maxIntegral) pid->integral = -pid->maxIntegral;
    //计算输出
    pid->output = pid->pout + pid->this_dev + pid->integral;
	
	//保存上一次低通滤波之后的微分值
	pid->last_dev = pid->this_dev;
	
	
	if(fabs(pid->error)<0.75){
		pid->output = 0;
		Motor_SpeedB_Goal.change++;
		if(Motor_SpeedB_Goal.change > 50){
			Motor_SpeedB_Goal.finish = 1;
			Motor_SpeedB_Goal.change = 0;
		}
	}else{
		Motor_SpeedB_Goal.finish = 0;
		Motor_SpeedB_Goal.change = 0;
	}
	
    //输出限幅
    if(pid->output > pid->maxOutput) pid->output =   pid->maxOutput;
    else if(pid->output < -pid->maxOutput) pid->output = -pid->maxOutput;
}



//串级PID的计算函数
//参数(PID结构体,外环目标值,外环反馈值,内环反馈值)
//如果是位置串级控制,则outerRef为期望位置，outerFdb为当前位置,innerFdb为当前速度
void PID_CascadeCalc(CascadePID *pid, float outerRef, float outerFdb, float innerFdb)
{
    PID_Calc(&pid->outer, outerRef, outerFdb); //计算外环
    PID_Calc(&pid->inner, pid->outer.output, innerFdb); //计算内环
    pid->output = pid->inner.output; //内环输出就是串级PID的输出
}

