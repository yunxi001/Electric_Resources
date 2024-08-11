import math


class PID:
    kp = 0.0
    ki = 0.0
    kd = 0.0
    err = 0.0  # 误差
    last_err = 0.0  # 上次误差
    err_sum = 0.0  # 误差的累计
    err_difference = 0.0  # 误差的差值


def pid_servo(measure, calcu):
    PID.err = measure - calcu  # 误差
    PID.err_sum += PID.err  # 误差的累计
    PID.err_difference = PID.err - PID.last_err  # 误差的差值
    PID.last_err = PID.err  # 记录此次误差，保存为上次误差使用
    return PID.kp * PID.err + PID.ki * PID.err_sum + PID.kd * PID.err_difference


# 距离转为角度 calcu原位置 measure要移动到的位置 h舵机云台距平面的距离
def degree_out(measure, calcu, h):
    degree = math.degrees(math.atan2(h, measure - calcu))
    return degree


# calcu原位置 measure要移动到的位置 h舵机云台距平面的距离
# 根据PID算法输出要转动的角度
# pid_servo(measure, calcu) 应该是还要移动的距离
# calcu + pid_servo(measure, calcu) 原位置 + 还要移动的距离 = 要移动到的位置
def degree_pid(measure, calcu, h):
    return degree_out(calcu + pid_servo(measure, calcu), calcu, h)


def degree_out_xy(measure_x, calcu_x, h, measure_y, calcu_y):
    if abs(measure_x - calcu_x) >= abs(measure_y - calcu_y):
        x_y = abs(measure_x - calcu_x) / abs(measure_y - calcu_y)
        move_x = degree_out_xunji(measure_x, calcu_x, h)
        move_y = move_x * x_y
    else:
        x_y = abs(measure_y - calcu_y) / abs(measure_x - calcu_x)
        move_x = degree_out_xunji(measure_x, calcu_x, h)
        move_y = move_x * x_y


# 2ms计算一次pid
# 循迹转动可用
def degree_out_xunji(measure, calcu, h):
    degree_compare = degree_out(measure, calcu, h)  # 0.7 068 7cm 0.135*3=0.405度
    if int(degree_compare > 0.54):  # 精度不够可以改3,4,5等 0.135*4=0.54
        return degree_pid(calcu + 0.71, calcu, h)
    elif int(degree_compare < -0.54):  # 精度不够可以改3,4,5等 0.135*4=0.54
        return degree_pid(calcu - 0.71, calcu, h)
    elif int(degree_compare < 0.54) & int(degree_compare >= 0):  # 精度不够可以改3,4,5等 0.135*4=0.54
        return degree_pid(measure, calcu, h)
    elif int(degree_compare > -0.54) & int(degree_compare < 0):  # 精度不够可以改3,4,5等 0.135*4=0.54
        return degree_pid(measure, calcu, h)
    else:
        return 0
    #   return degree_out(calcu, pid_servo(calcu + 0.71, calcu), h)  # 0.7 068 7cm 0.135*3=0.405度


'''
# 循迹转动可用
def degree_out_xunji_x(measure, calcu, h):
    degree_panduan = math.fabs(degree_out(measure, calcu, h))
    if int(degree_panduan >= 0.27) & int(degree_panduan < 0.54):  # 精度不够可以改为0.27以下
        return degree_pid(measure, calcu, h)
    #   return degree_out(calcu, pid_servo(calcu + 0.71, calcu), h)  # 0.7 068 7cm 0.135*3=0.405度
    else:
        return degree_pid(calcu + 0.71, calcu, h)
    #   return degree_out(calcu, pid_servo(calcu + 0.71, calcu), h)  # 0.7 068 7cm 0.135*3=0.405度
'''
