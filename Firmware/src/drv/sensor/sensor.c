/***************************************************************************//**
 * @file        sensor.c
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 440 $
 *              $LastChangedDate: 2010-05-20 09:59:35 +0200 (Do, 20 Mai 2010) $
 * @brief       Treiber f�r den hall sensor
 *****************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "sensor.h"
#include "pwm.h"
#include "lowpass.h"
#include "hwVersion.h"
#include "ledControl.h"
#include "xwCom.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SENSOR_0_IS_ON          ((GPIOB->IDR) & GPIO_Pin_8)
#define SENSOR_TICK_FREQUENCY   500000
#define SENSOR_LP_LENGTH        4
#define SENSOR_LP_SHIFT         2
#define SENSOR_3600_SHIFT_20    3774873600

/* Private variables ---------------------------------------------------------*/
u16  sensor_lastSync, sensor_speed;
u32  sensor_beta;
bool sensor_rotationDetect, sensor_edgeDetect, sensor_timeoutDetect, sensor_updateDetect;
u16 speedLPBuf[SENSOR_LP_LENGTH];
lowpassHandle_t speedLP = DEFINE_LOWPASS_FILTER(speedLPBuf,SENSOR_LP_LENGTH,SENSOR_LP_SHIFT);

typedef struct {
    bool enabled;
    u8   numAngles;
    bool zeroTransition[SENSOR_MAX_NUM_ANGLES];
    u16  angleOn[SENSOR_MAX_NUM_ANGLES];
    u16  angleOff[SENSOR_MAX_NUM_ANGLES];
    s32  wBuffer[SENSOR_MAX_NUM_ANGLES];
} syncHandle_t;
syncHandle_t syncHandle[LED_NUM_OUTPUTS];

/*******************************************************************************
 * sensor_enableOutput
 ******************************************************************************/
void sensor_enableOutput(u8 output, u8 numAngles) {
    if (output >= LED_NUM_OUTPUTS) return;
    if (numAngles >= SENSOR_MAX_NUM_ANGLES) return;
    syncHandle[output].enabled = true;
    syncHandle[output].numAngles = numAngles;
}

/*******************************************************************************
 * sensor_disableOutput
 ******************************************************************************/
void sensor_disableOutput(u8 output) {
    if (output >= LED_NUM_OUTPUTS) return;
    syncHandle[output].enabled = false;
}

/*******************************************************************************
 * sensor_setAngle
 ******************************************************************************/
void sensor_setAngle(u8 output, u8 angle, u16 angleOn, u16 angleOff) {
    if (output >= LED_NUM_OUTPUTS) return;
    if (angle >= SENSOR_MAX_NUM_ANGLES) return;

    u16 on  = (angleOn*10)  % 3600;
    u16 off = (angleOff*10) % 3600;

    syncHandle[output].zeroTransition[angle] = false;
    if (on > off) syncHandle[output].zeroTransition[angle] = true;

    syncHandle[output].angleOn[angle]  = on;
    syncHandle[output].angleOff[angle] = off;
}

/*******************************************************************************
 * sensor_increaseAngle
 ******************************************************************************/
void sensor_rotateAngle(u8 output, u8 angle, s16 w) {
    if (output >= LED_NUM_OUTPUTS) return;
    if (angle >= SENSOR_MAX_NUM_ANGLES) return;

    s32 inc = 0;
    s32 tmp = syncHandle[output].wBuffer[angle];
    tmp += w;

    inc = tmp / 100;
    tmp = tmp - 100*inc;


    syncHandle[output].wBuffer[angle] = tmp;

    if (inc != 0) {
        s16 angleOn =  syncHandle[output].angleOn[angle] + inc;
        s16 angleOff = syncHandle[output].angleOff[angle] + inc;

        if (angleOn  <    0) angleOn  += 3600;
        if (angleOff <    0) angleOff += 3600;
        if (angleOn  > 3600) angleOn  -= 3600;
        if (angleOff > 3600) angleOff -= 3600;

        syncHandle[output].zeroTransition[angle] = false;
        if (angleOn > angleOff) syncHandle[output].zeroTransition[angle] = true;

        syncHandle[output].angleOn[angle]  = angleOn;
        syncHandle[output].angleOff[angle] = angleOff;
    }
}

/*******************************************************************************
 * sensor_active
 ******************************************************************************/
bool sensor_active(void) {
    if (SENSOR_0_IS_ON) return false;
    return true;
}

/*******************************************************************************
 * sensor_hisr
 ******************************************************************************/
void sensor_hisr(void) {

    syncHandle_t* p;

    u16 timeElapsed = TIM4->CNT - sensor_lastSync;
    u32 alpha = ((u32)timeElapsed * sensor_beta) >> 20;

    for (u8 i=0; i<LED_NUM_OUTPUTS; i++) {
        if (syncHandle[i].enabled) {
            p = (syncHandle_t*) &syncHandle[i];
            if (sensor_rotationDetect) {
                bool turnOn = false;
                for (u8 j=0; j<p->numAngles;j++) {
                    if (p->zeroTransition[j]) {
                        if ((alpha >= p->angleOn[j]) || (alpha < p->angleOff[j])) turnOn = true;
                    }
                    else {
                        if ((alpha >= p->angleOn[j]) && (alpha < p->angleOff[j])) turnOn = true;
                    }
                }
                if (turnOn) *((u16*)pwmValue[i]) = pwmTable[0xFF];
                else *((u16*)pwmValue[i]) = 0;

            }
            else *((u16*)pwmValue[i]) = pwmTable[0x7F];
        }
    }
}

/*******************************************************************************
 * sensor_getRPM
 ******************************************************************************/
u16 sensor_getRPM(void) {
    if (!sensor_speed) return 0;
    if (!sensor_rotationDetect) return 0;
    return (SENSOR_TICK_FREQUENCY*60) / sensor_speed;
}

/*******************************************************************************
 * sensor_callback
 ******************************************************************************/
void sensor_callback(u16 timValue) {

    sensor_timeoutDetect = false;
    if (sensor_rotationDetect) {
        u16 elapsed = timValue - sensor_lastSync;
        sensor_speed = lowpass_update(&speedLP, elapsed);
        sensor_lastSync = timValue;
        if (sensor_speed > 0) sensor_beta = SENSOR_3600_SHIFT_20 / sensor_speed;
    }
    else {
        if (sensor_edgeDetect) {
            sensor_rotationDetect = true;
            sensor_lastSync = timValue;
        }
        else sensor_edgeDetect = true;
    }

}

/*******************************************************************************
 * sensor_callback
 ******************************************************************************/
void sensor_timeout(void) {
    if (sensor_timeoutDetect) {
        sensor_rotationDetect = false;
        sensor_edgeDetect = false;
    }
    else sensor_timeoutDetect = true;
}

/*******************************************************************************
 * sensor_init
 ******************************************************************************/
void sensor_init(void) {

    sensor_rotationDetect = false;
  	sensor_edgeDetect = false;
  	sensor_timeoutDetect = false;

  	for (u8 i=0; i<LED_NUM_OUTPUTS; i++) sensor_disableOutput(i);

    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    //TIM4 CC3 wird zur Flankenerkennung gebraucht -----------------------------
	TIM_ICInitTypeDef   TIM_ICInitStructure;
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
  	TIM_ICInitStructure.TIM_ICPolarity =  TIM_ICPolarity_Falling;
  	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  	TIM_ICInitStructure.TIM_ICFilter = 0x02;
  	TIM_ICInit(TIM4, &TIM_ICInitStructure);

  	TIM4->DIER |= TIM_IT_CC3;
}
