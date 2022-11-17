//
// Created by WuGaoyuan on 2022/10/16.
//

#include <cstdio>
#include "Robot.h"
#include "tim.h"
#include "string.h"
void Robot::init() {

    offset[2]=-35;
    offset[15]=35;

    offset[3]=40;
    offset[4]=8;
    offset[6]=1;
    offset[7]=-8;
    offset[11]=-5;
    offset[10]=-25;
    servo[0].tim=&htim1;
    servo[0].TIM_CHANNEL=TIM_CHANNEL_1;
    servo[1].tim=&htim1;
    servo[1].TIM_CHANNEL=TIM_CHANNEL_2;
    servo[2].tim=&htim1;
    servo[2].TIM_CHANNEL=TIM_CHANNEL_3;
    servo[3].tim=&htim1;
    servo[3].TIM_CHANNEL=TIM_CHANNEL_4;

    servo[4].tim=&htim2;
    servo[4].TIM_CHANNEL=TIM_CHANNEL_1;
    servo[5].tim=&htim2;
    servo[5].TIM_CHANNEL=TIM_CHANNEL_2;
    servo[6].tim=&htim2;
    servo[6].TIM_CHANNEL=TIM_CHANNEL_3;
    servo[7].tim=&htim2;
    servo[7].TIM_CHANNEL=TIM_CHANNEL_4;

    servo[8].tim=&htim3;
    servo[8].TIM_CHANNEL=TIM_CHANNEL_1;
    servo[9].tim=&htim3;
    servo[9].TIM_CHANNEL=TIM_CHANNEL_2;
    servo[10].tim=&htim3;
    servo[10].TIM_CHANNEL=TIM_CHANNEL_3;
    servo[11].tim=&htim3;
    servo[11].TIM_CHANNEL=TIM_CHANNEL_4;

    servo[12].tim=&htim5;
    servo[12].TIM_CHANNEL=TIM_CHANNEL_1;
    servo[13].tim=&htim5;
    servo[13].TIM_CHANNEL=TIM_CHANNEL_2;

    servo[14].tim=&htim8;
    servo[14].TIM_CHANNEL=TIM_CHANNEL_1;
    servo[15].tim=&htim8;
    servo[15].TIM_CHANNEL=TIM_CHANNEL_4;
    for(int i=0;i<16;i++){
        HAL_TIM_PWM_Start(servo[i].tim,servo[i].TIM_CHANNEL);
        __HAL_TIM_SET_COMPARE(servo[i].tim,servo[i].TIM_CHANNEL,(((offset[i]+90)/90.0)+0.5)/20.0*2000);
    }
}

Robot::Robot() {
//    init();
}

Robot::~Robot() {

}

void Robot::updateAngle() {
    for(uint8_t i=0;i<14;i++){
        if(servo[i].speed==0) continue;
        if(servo[i].speed>0 && servo[i].goalAngle<servo[i].angle){
            servo[i].speed=0;
        }else if(servo[i].speed<0&&servo[i].goalAngle>servo[i].angle){
            servo[i].speed=0;
        }else if(servo[i].speed>0 &&servo[i].goalAngle-servo[i].angle<=servo[i].speed||servo[i].speed<0&&servo[i].goalAngle-servo[i].angle>=servo[i].speed){
            servo[i].angle = servo[i].goalAngle;
            isIDLE = true;
        }else{
            servo[i].angle+= servo[i].speed;
        }
        __HAL_TIM_SET_COMPARE(servo[i].tim,servo[i].TIM_CHANNEL,(((offset[i]+servo[i].angle)/90.0)+0.5)/20.0*2000);
    }
}
void Robot::setAngle(uint8_t i, uint8_t angle, uint8_t speed) {
    servo[i].goalAngle = angle;
    servo[i].speed = (angle-servo[i].angle)/55.0*speed;
}
//#0 P1500 #1 P1500 #2 P1500 #3 P1500 #4 P1500 #5 P0 #6 P1500 #7 P1500 #8 P1500 #9 P1500 #10 P1500
void Robot::setAction(char *action) {
    uint8_t id;
    uint32_t angle;
    uint8_t speed=1;
    for(int i=0;i<strlen(action);i++){
        if(action[i]=='s'){
            speed = action[i+1]-'0';
        }else if(action[i]=='#'){
            if(action[i+2]==' '){
                id=action[i+1]-'0';
            }else if(action[i+3]==' '){
                id=(action[i+1]-'0')*10+action[i+2]-'0';
            }
        }else if(action[i]=='P'){
            if(action[i+4]==' '){
                angle=(action[i+1]-'0')*100+(action[i+2]-'0')*10+action[i+3]-'0';
            }else if(action[i+5]==' '){
                angle=(action[i+1]-'0')*1000+(action[i+2]-'0')*100+(action[i+3]-'0')*10+action[i+4]-'0';
            }
            setAngle(id, (uint8_t)(angle*0.09-45),speed);
        }else continue;
    }
    printf("%d\n",angle);
}
void Robot::setRule() {
    if (pointer==3){pointer=0;}
    else if(pointer==4){pointer=4;}
    else if(pointer==8){pointer=5;}
    else if(pointer==12){pointer=9;}
    else{pointer++;}
}