import math
'''
距离转角度
c1为红点的x或轴坐标
h为设备（激光笔）距离屏幕距离
注意调数据精度，小数点后多少位
'''

def degree_j(c1,c2,h):
    degree = math.degrees(math.atan2(h,c2-c1))
    if((c2-c1) <= 0):
        degree = 180-degree
    print(degree)
'''PID算法
float Kp,Ki,Kd;          //参数
float err;               //此次误差
float last_err;          //上次误差
float err_sum=0;         //误差累加
float err_difference;    //误差的差值
'''
class PID():
    kp = 0
    ki = 0
    kd = 0
    err = 0
    last_err = 0
    err_sum = 0
    err_difference = 0
#2ms计算一次pid
'''应该不想要ki'''
def PID_servo(measure,calcu):
    PID.err = measure - calcu
    PID.err_sum += PID.err
    PID.err_difference = PID.err - PID.last_err
    PID.last_err = PID.err
    return PID.kp * PID.err + PID.ki * PID.err_sum + PID.kd * PID.err_difference
'''
'无ki'
def PID_servo(measure,calcu):
    PID.err = measure - calcu
    PID.err_difference = PID.err - PID.last_err
    PID.last_err = PID.err
    return PID.kp * PID.err + PID.kd * PID.err_difference
'''
'''
角度精度，0.27
可能需要pid
#include "pid.h"

extern float Kp,Ki,Kd;   //直立环参数
float err;               //此次误差
float last_err;          //上次误差
float err_sum=0;         //误差累加
float err_difference;    //误差的差值

extern float VKp,VKi;    //速度环参数
float filt_velocity;     //滤波后的速度
float last_filt_velocity;//上一次的滤波后的速度
float velocity_sum=0;    //速度的累加


//直立环：
int vertical_PID_value(float measure,float calcu)
{

	err = measure - calcu;             //误差
	err_sum+=err;                      //误差的累加
	err_difference = err - last_err;   //误差的差值
	last_err = err;                    //此次误差记录为“上次误差”
	
	return Kp*err + Ki*err_sum + Kd*err_difference;
}


//速度环：
int velocity_PID_value(int velocity)
{
	float a=0.3;                                           //滤波系数（反映滤波程度）
	filt_velocity = a*velocity + (1-a)*last_filt_velocity; //一阶速度滤波
	velocity_sum +=  filt_velocity;                        //速度的累加
	I_xianfu(3000);                                        //累加限幅
	last_filt_velocity = filt_velocity;                    //此次速度记录为“上次速度”

	return VKp*filt_velocity + VKi*velocity_sum;
}


//I限幅：
void I_xianfu(int max)
{
	if(velocity_sum>max)  velocity_sum=max;
	if(velocity_sum<-max) velocity_sum=-max;
}

'''
