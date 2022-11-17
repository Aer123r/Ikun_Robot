//
// Created by WuGaoyuan on 2022/10/16.
//

#ifndef STM32F103C8T6_ROBOT_H
#define STM32F103C8T6_ROBOT_H

//#include "stm32f1xx_hal_tim.h"

#include <cstdint>
#include "stm32f1xx_hal.h"
using namespace std;

struct Servo{
    TIM_HandleTypeDef *tim;
    uint16_t TIM_CHANNEL;
    float angle=90;
    uint8_t goalAngle=90;
    float speed;
};

class Robot {
private:
    int8_t offset[16];
public:
    char* actionGroup[100]= {
            //脚掌晃
            (char*)"s3#0 P1500 #1 P1800 #2 P1500 #3 P1700 #4 P1650  #6 P1500 #7 P1800 #15 P1600 #10 P1700 #11 P1600 ",
            (char*)"s4#0 P1500 #1 P1700 #2 P1500 #3 P1700 #4 P1400  #6 P1500 #7 P1700 #15 P1500 #10 P1700 #11 P1350 ",
            (char*)"s3#0 P1500 #1 P1200 #2 P1400 #3 P1300 #4 P1400  #6 P1500 #7 P1300 #15 P1500 #10 P1400 #11 P1350 ",
            (char*)"s4#0 P1500 #1 P1300 #2 P1500 #3 P1300 #4 P1650  #6 P1500 #7 P1300 #15 P1500 #10 P1300 #11 P1600 ",
//            //直立状
            (char*)"s2#0 P1500 #1 P1500 #2 P1500 #3 P1500 #4 P1500     #6 P1500 #7 P1500 #15 P1500 #10 P1500 #11 P1500  " ,
            //左拐？
            (char*)"s3#0 P1500 #1 P1800 #2 P1500 #3 P1700 #4 P1600  #6 P1500 #7 P1800 #15 P1600 #10 P1700 #11 P1600 ",
            (char*)"s4#0 P1500 #1 P1700 #2 P1500 #3 P1700 #4 P1450  #6 P1500 #7 P1700 #15 P1500 #10 P1700 #11 P1350 ",
            (char*)"s3#0 P1500 #1 P1200 #2 P1400 #3 P1300 #4 P1450  #6 P1500 #7 P1300 #15 P1500 #10 P1400 #11 P1350 ",
            (char*)"s4#0 P1500 #1 P1300 #2 P1500 #3 P1300 #4 P1600  #6 P1500 #7 P1300 #15 P1500 #10 P1300 #11 P1600 ",
            //右拐？
            (char*)"s3#0 P1500 #1 P1800 #2 P1500 #3 P1700 #4 P1650  #6 P1500 #7 P1800 #15 P1600 #10 P1700 #11 P1550 ",
            (char*)"s4#0 P1500 #1 P1700 #2 P1500 #3 P1700 #4 P1400  #6 P1500 #7 P1700 #15 P1500 #10 P1700 #11 P1400 ",
            (char*)"s3#0 P1500 #1 P1200 #2 P1400 #3 P1300 #4 P1400  #6 P1500 #7 P1300 #15 P1500 #10 P1400 #11 P1400 ",
            (char*)"s4#0 P1500 #1 P1300 #2 P1500 #3 P1300 #4 P1650  #6 P1500 #7 P1300 #15 P1500 #10 P1300 #11 P1550 ",
                            };
    uint8_t pointer = 0;
    bool isIDLE = false;
    Robot();
    ~Robot();
    Servo servo[16];
    void init();
    void setAngle(uint8_t i, uint8_t angle,uint8_t speed);
    void setAction(char* action);
    void setRule();
    void updateAngle();

};



#endif //STM32F103C8T6_ROBOT_H
