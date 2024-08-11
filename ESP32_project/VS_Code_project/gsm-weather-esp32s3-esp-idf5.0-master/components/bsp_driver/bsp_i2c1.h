#ifndef __bsp_i2c1_h__
#define __bsp_i2c1_h__
#include "main.h"


#define IIC_SCL_IO  GPIO_NUM_5
#define IIC_SDA_IO  GPIO_NUM_4

 //IO操作函数
#define IIC_SCL(X)      gpio_set_level(IIC_SCL_IO, X) //SCL
#define IIC_SDA(X)      gpio_set_level(IIC_SDA_IO, X) //SDA
#define READ_SDA        gpio_get_level(IIC_SDA_IO)  //输入SDA

//IO方向设置
#define SDA_IN()        gpio_set_direction(IIC_SDA_IO, GPIO_MODE_INPUT)
#define SDA_OUT()       gpio_set_direction(IIC_SDA_IO, GPIO_MODE_OUTPUT)


//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t IIC_Read_Byte(uint8_t ack);//IIC读取一个字节
uint8_t IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

uint8_t IIC_read_addr_byte(uint8_t device_addr,uint8_t read_addr);
void IIC_read_addr_str(uint8_t device_addr,uint8_t read_addr,uint8_t read_amount,uint8_t *read_buf);
void IIC_write_addr_byte(uint8_t device_addr,uint8_t write_addr,uint8_t write_dat);
unsigned int IIC_read_addr_int(uint8_t device_addr,uint8_t read_addr);
void IIC_write_addr_str(uint8_t device_addr,uint8_t write_addr,uint8_t write_amount,uint8_t *write_buf);


#endif
















