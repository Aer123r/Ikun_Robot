//
// Created by WuGaoyuan on 2022/10/16.
//
#include <cstdlib>
#include "stdio.h"
#include "main.h"
#include "Robot.h"
#include "tim.h"
#include "usart.h"
#include "i2c.h"

static void MPU6050_Init(void);
short acceleStock[3];
short gyroStock[3];
float eulerStock[3];
uint8_t data_u1[20];
u8g2_t u8g2;
Robot robot;
void Main(){
    MPU6050_Init();
    mpu_dmp_init();
    u8g2Init(&u8g2);
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2,u8g2_font_courB14_tf);
    u8g2_DrawStr(&u8g2,20,20,"MPU is ");
    u8g2_DrawStr(&u8g2,60,50,"ready!");
    u8g2_SendBuffer(&u8g2);
    robot.init();
    robot.pointer=0;
    robot.setAction(robot.actionGroup[robot.pointer]);
    HAL_UART_Transmit(&huart1, "Hello\n",5,HAL_MAX_DELAY);


    while(1){
        for(int i=0;i<40;i++){
            mpuGetAcceler(&acceleStock[0], &acceleStock[1], &acceleStock[2]);
            mpuGetGyro(&gyroStock[0], &gyroStock[1], &gyroStock[2]);
            mpu_dmp_get_data(&eulerStock[0], &eulerStock[1], &eulerStock[2]);
            if(i==39){
                HAL_TIM_Base_Start_IT(&htim1);
            }
        }

        //右脚激光
        HAL_UART_Receive_IT(&huart3,data_u1,20);
        //左脚激光
        HAL_UART_Receive_IT(&huart1,data_u1,20);

    }
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART3){
        //右边识别到黑线，左转
        if(data_u1[0]==0x55){
            if(data_u1[4]!=0x00){
                u8g2_ClearBuffer(&u8g2);
                u8g2_SetFont(&u8g2,u8g2_font_courB18_tr);
                u8g2_DrawStr(&u8g2,0,20, "LEFT");
                u8g2_SendBuffer(&u8g2);
                robot.pointer==5;
                HAL_UART_Transmit(&huart2, (const char*)robot.pointer,1,HAL_MAX_DELAY);
                HAL_UART_Transmit(&huart5, (const char*)robot.pointer,1,HAL_MAX_DELAY);
                robot.setAction(robot.actionGroup[robot.pointer]);
                HAL_TIM_Base_Stop_IT(&htim1);
                return;
            }
        }
        HAL_TIM_Base_Start_IT(&htim1);
    }else if(huart->Instance == USART1){
        //左边识别到黑线，右转
        if(data_u1[0]==0x55){
            if(data_u1[4]!=0x00){
                robot.pointer==9;
                HAL_UART_Transmit(&huart2, (const char*)robot.pointer,1,HAL_MAX_DELAY);
                HAL_UART_Transmit(&huart5, (const char*)robot.pointer,1,HAL_MAX_DELAY);
                robot.setAction(robot.actionGroup[robot.pointer]);
                HAL_TIM_Base_Stop_IT(&htim1);
                return;
            }
        }
        HAL_TIM_Base_Start_IT(&htim1);
    }

}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    HAL_TIM_Base_Stop_IT(htim);
    static int i=0;

    u8g2_ClearBuffer(&u8g2);
//    u8g2_DrawFilledEllipse(&u8g2,20,30,rx,ry,U8G2_DRAW_ALL);
//    u8g2_DrawFilledEllipse(&u8g2,100,30,rx,ry,U8G2_DRAW_ALL);
    u8g2_SetFont(&u8g2,u8g2_font_courB14_tr);
    char pitch_str[10],yaw_str[10],roll_str[10],i_s[10];
    itoa((int)eulerStock[0],pitch_str,10);
    itoa((int)eulerStock[1],roll_str,10);
    itoa((int)eulerStock[2],yaw_str,10);
    itoa(i,i_s,10);
    u8g2_DrawStr(&u8g2,0,20, "pitch");
    u8g2_DrawStr(&u8g2,83,20, pitch_str);
    u8g2_DrawStr(&u8g2,0,40, "yaw");
    u8g2_DrawStr(&u8g2,83,40, yaw_str);
    u8g2_DrawStr(&u8g2,0,60, "roll");
    u8g2_DrawStr(&u8g2,83,60, roll_str);
    u8g2_SendBuffer(&u8g2);
    robot.updateAngle();
    if(robot.isIDLE) {
        robot.isIDLE = false;
        robot.setRule();
        robot.setAction(robot.actionGroup[robot.pointer]);
    }
    i++;
//    HAL_TIM_Base_Start_IT(htim);
}
static void MPU6050_Init(void){
    uint8_t data;
    //1初始化I2C

    //2复位MPU6050：一、将电源管理寄存器1的bit7写1（从第0位开始数起） 二、对电源管理寄存器1写0x00以唤醒MPU6050
    while(HAL_I2C_IsDeviceReady(&hi2c1, (MPU_Device_Addr << 1) | 1, 5, 0xffff) != HAL_OK);
    data = 0x80;
    while(HAL_I2C_Mem_Write(&hi2c1, (MPU_Device_Addr<<1) | 0, MPU_PowerManageReg1_Addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xffff) != HAL_OK);
    HAL_Delay(100);		//不知道为什么要延迟0.1s，应该没什么意义。。。。。
    while(HAL_I2C_IsDeviceReady(&hi2c1, (MPU_Device_Addr<<1) | 1, 5, 0xffff) != HAL_OK);
    data = 0x00;
    while(HAL_I2C_Mem_Write(&hi2c1, (MPU_Device_Addr<<1) | 0, MPU_PowerManageReg1_Addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xffff) != HAL_OK);

    //3设置陀螺仪配置寄存器、加速度配置寄存器的最大计数范围
    while(HAL_I2C_IsDeviceReady(&hi2c1, (MPU_Device_Addr<<1) | 1, 5, 0xffff) != HAL_OK);
    data = (0x03 << 3);		//为什么要左移三位？具体参见“STM32F407ZG中文开发手册库函数版本”中MPU6050的实验
    while(HAL_I2C_Mem_Write(&hi2c1, (MPU_Device_Addr<<1) | 0, MPU_GyroConfigReg_Addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xffff) != HAL_OK);
    while(HAL_I2C_IsDeviceReady(&hi2c1, (MPU_Device_Addr<<1) | 1, 5, 0xffff) != HAL_OK);
    data = (0x00 << 3);		//为什么要左移三位？具体参见“STM32F407ZG中文开发手册库函数版本”中MPU6050的实验
    while(HAL_I2C_Mem_Write(&hi2c1, (MPU_Device_Addr<<1) | 0, MPU_AcceConfigReg_Addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xffff) != HAL_OK);

    //4关闭MPU的中断、AUX_I2C、FIFO
    while(HAL_I2C_IsDeviceReady(&hi2c1, (MPU_Device_Addr<<1) | 1, 5, 0xffff) != HAL_OK);
    data = 0x00;
    while(HAL_I2C_Mem_Write(&hi2c1, (MPU_Device_Addr<<1) | 0, MPU_InterruEnableReg_Addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xffff) != HAL_OK);
    while(HAL_I2C_IsDeviceReady(&hi2c1, (MPU_Device_Addr<<1) | 1, 5, 0xffff) != HAL_OK);
    while(HAL_I2C_Mem_Write(&hi2c1, (MPU_Device_Addr<<1) | 0, MPU_UserControlReg_Addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xffff) != HAL_OK);
    while(HAL_I2C_IsDeviceReady(&hi2c1, (MPU_Device_Addr<<1) | 1, 5, 0xffff) != HAL_OK);
    while(HAL_I2C_Mem_Write(&hi2c1, (MPU_Device_Addr<<1) | 0, MPU_FIFOConfigReg_Addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xffff) != HAL_OK);

    //5设定陀螺仪采样率
    while(HAL_I2C_IsDeviceReady(&hi2c1, (MPU_Device_Addr<<1) | 1, 5, 0xffff) != HAL_OK);
    data = mpuGetSMPLART_DIV(1, 50);
    while(HAL_I2C_Mem_Write(&hi2c1, (MPU_Device_Addr<<1) | 0, MPU_GyroSampleRateReg_Addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xffff) != HAL_OK);

    //6设定DLPF的带宽
    while(HAL_I2C_IsDeviceReady(&hi2c1, (MPU_Device_Addr<<1) | 1, 5, 0xffff) != HAL_OK);
    data = 0x04;		//这一步做的不是很完美，之后自己可以试着写一个根据陀螺仪输出频率、陀螺仪采样率得到DLPF进而得到CFG_DLPF的函数。。。。。
    while(HAL_I2C_Mem_Write(&hi2c1, (MPU_Device_Addr<<1) | 0, MPU_DLPFConfigReg_Addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xffff) != HAL_OK);

    //7设定时钟，使能陀螺仪、加速度寄存器
    while(HAL_I2C_IsDeviceReady(&hi2c1, (MPU_Device_Addr<<1) | 1, 5, 0xffff) != HAL_OK);
    while(HAL_I2C_Mem_Read(&hi2c1, (MPU_Device_Addr<<1) | 1, MPU_DeviceIDReg_Addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xffff) != HAL_OK);
    if(data == MPU_Device_Addr){
        while(HAL_I2C_IsDeviceReady(&hi2c1, (MPU_Device_Addr<<1) | 1, 5, 0xffff) != HAL_OK);
        data = 0x01;
        while(HAL_I2C_Mem_Write(&hi2c1, (MPU_Device_Addr<<1) | 0, MPU_PowerManageReg1_Addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xffff) != HAL_OK);
        while(HAL_I2C_IsDeviceReady(&hi2c1, (MPU_Device_Addr<<1) | 1, 5, 0xffff) != HAL_OK);
        data = 0x00;
        while(HAL_I2C_Mem_Write(&hi2c1, (MPU_Device_Addr<<1) | 0, MPU_PowerManageReg2_Addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xffff) != HAL_OK);
    }
}