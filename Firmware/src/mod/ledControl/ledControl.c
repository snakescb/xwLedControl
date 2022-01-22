/***************************************************************************//**
 * @file        ledControl.c
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 712 $
 *              $LastChangedDate: 2010-06-01 17:10:29 +0200 (Di, 01 Jun 2010) $
 *
 * LED steuerung
 *****************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "ledControl.h"
#include "conf.h"
#include "pwm.h"
#include "teco.h"
#include "jumper.h"
#include "recv.h"
#include "statusLed.h"
#include "adc.h"
#include "skyBus.h"
#include "crc.h"
#include "sensor.h"
#include "simpleString.h"
#include "dreamExtension.h"
#include "switch.h"

/* Private define ------------------------------------------------------------*/
#define LED_MAX_NUM_OUTPUTS          24
#define LED_OBJECT_SIZE              12
#define LED_MAX_TIMEOUT             100
#define LED_SEQUENCE_NONE          0xFF
#define LED_MAX_AUX                1000
#define LED_MAX_DIM_BATT_PRESET      10
#define LED_NUM_SIM_OBJECTS          35
#define LED_NUM_OBJECTS_PER_TRANSFER  8
#define LED_SKYBUS_SLAVE_PORT      0xA1
#define LED_RX_AREA_OFFSET            5
#define LED_RX_THRESHOLD            105
#define LED_DEFAULT_INCREMENT   0x10000
#define LED_DEFAULT_DIM            0xFF
#define LED_OUTPUT_UNLINKED        0xFF
#define LED_NUM_RANDOM_NUMBERS        4
#define LED_NUM_PARAMS_PER_HANDLE    14
#define LED_JUMPER_DEBOUNCE_LEN      10

/* Defines for Afterburner Object --------------------------------------------*/
#define AFTERBURNER_MIN_NUM_TURNON            6
#define AFTERBURNER_MAX_NUM_TURNON           12
#define AFTERBURNER_MIN_TURNON_TIME          30
#define AFTERBURNER_MAX_TURNON_TIME          60

#define AFTERBURNER_MIN_NUM_TURNOFF           4
#define AFTERBURNER_MAX_NUM_TURNOFF           8
#define AFTERBURNER_MIN_TURNOFF_TIME         30
#define AFTERBURNER_MAX_TURNOFF_TIME         60

#define AFTERBURNER_FLASH_TIME_MIN_MAX_RATIO  3

#define pRANDSTART 0x08000000
#define pRANDSTOP  0x08006000

/* Private typedef -----------------------------------------------------------*/
typedef enum {
    SOB = 0x01,
    LOB = 0x02,
    BLK = 0x03,
    DIM = 0x04,
    AWR = 0x05,
    AFB = 0x06,
    DRE = 0xA0,
    EOD = 0xF0,
    EXT = 0xF1,
    DBG = 0xF2
} objects_e;

typedef enum {
    STOP    = 0,
    FORWARD = 1,
    REVERSE = 2
} outputMode_e;

typedef enum {
    FIX_INCREMENT = 0,
    VARIABLE_INCREMENT = 1
} timeIncrementMode_e;

typedef enum {
    FIX_DIM = 0,
    VARIABLE_DIM = 1
} sequenceDimMode_e;

typedef enum {
    OUTPUT_TYPE_LED = 0,
    OUTPUT_TYPE_DREAMSTRIP = 1
} outputType_e;

typedef struct {
    outputMode_e outputMode;
    timeIncrementMode_e incrementMode;
    sequenceDimMode_e sequenceDimMode;
    u8   pwm;
    u8*  pObjectTableBase;
    u8*  pCurrentObject;
    u16  runTime;
    u16  lifeTime;
    u16  params[LED_NUM_PARAMS_PER_HANDLE];
    u8   sequenceDim;
    u8   simObjects[LED_NUM_SIM_OBJECTS][LED_OBJECT_SIZE];
    u8   simReadIndex, simWriteIndex;
    u8   lineEnd;
    u8   localControl;
    u8   outputNumber;
    u8   extensionBackup[LED_OBJECT_SIZE];
    u32  runTimeExtended;
    u32  runTimeIncrement;
    u16* pRand;
    outputType_e outputType;
    u8   dreamAddress;
    u16  dreamNumPixel;
} ledHandle_t;

/* Private functions ---------------------------------------------------------*/
void ledControl_startObject(ledHandle_t* h, u8* pObject);
void ledControl_nextObject(ledHandle_t* h);
void ledControl_setupExtension(bool enableTx);
void ledControl_simReset(void);
bool ledControl_checkAutoSequence(void);
void ledControl_updateRuntime(ledHandle_t* h);
void ledControl_reinitRuntime(ledHandle_t* h);
void ledControl_clearRuntime(ledHandle_t* h);
void ledControl_randUpdate(void);
u16  ledControl_randScale(u16 rand, u16 min, u16 max);
u8   ledControl_getNumSimObjects(ledHandle_t* h);
s16  ledControl_getLinearValue(s16 xMin, s16 xMax, s16 yMin, s16 yMax, s16 x);
u8*  ledControl_getExtensionObject(ledHandle_t* h);
u8*  ledControl_consumeSimObject(ledHandle_t* h);

/* Private variables ---------------------------------------------------------*/
ledHandle_t ledHandle[LED_MAX_NUM_OUTPUTS];
bool rxFailsafe, batteryWarning, autoSelectionActive;
u8   numSequences, currentSequence, numControledOutputs, masterTimeout;
u8   battVoltageRounded, battWarningThreshold, numOutputsReport, sequenceDimReport, variableSequenceDim;
u8   pwmBuffer[LED_MAX_NUM_OUTPUTS];
u8*  configBase;
u32* pSequenceTable;
u16  sectionWidth, auxSignal, dimBatteryPreset, auxSim;
s16  dimXMin, dimXMax, rxSignal;
u32  ledControl_configSize, variableTimeIncrement;
u32  speedInfoReport;
u16  randArray[LED_NUM_RANDOM_NUMBERS];
u16* pFlashRand;
u8   jumperDebounceArray[LED_JUMPER_DEBOUNCE_LEN];
u8   jumperDebounce;

selectionMode_e selectionMode;
runMode_e runMode;
u16 scope;

const u32 timeFactorTable[] = {
    6554, 6862, 7186, 7525, 7879, 8250, 8639, 9046, 9473, 9919, 10387, 10876,
    11389, 11926, 12488, 13076, 13692, 14338, 15013, 15721, 16462, 17238,18050,
    18901, 19792, 20724, 21701, 22724, 23795, 24916, 26090, 27320, 28608, 29956,
    31368, 32846, 34394, 36015, 37712, 39489, 41350, 43299, 45340, 47477, 49714,
    52057, 54510, 57079, 59770, 62586, 65536, 68625, 71859, 75245, 78792, 82505,
    86393, 90465, 94728, 99193, 103868, 108763, 113889, 119256, 124876, 130762,
    136924, 143377, 150134, 157210, 164619, 172377, 180501, 189008, 197916, 207243,
    217010, 227237, 237947, 249161,260904, 273200, 286075, 299557, 313675, 328458,
    343938, 360147, 377120, 394893, 413504, 432992, 453398, 474766, 497141,520571,
    545105, 570795, 597695, 625864, 655360
};

const u8 reductionTable[] = {
    255, 250, 245, 235,
    220, 200, 180, 160,
    140, 120, 100, 80,
    60, 40, 20, 0
};

const u16 flashTimeArray[] = {
    5, 10, 15, 20,
    25, 30, 35, 40,
    45, 50, 55, 60,
    65, 70, 75, 80
};

/******************************************************************************
 * ledControl_update, Normale steuerung und MasterbetriebmasterByteCount
 ******************************************************************************/
void ledControl_update() {

    if (runMode == RUNMODE_SLAVE) return;

    /***************************************************************************
    * LED Ausg�nge neu berechnen
    ***************************************************************************/
    for (u8 i=0; i<numControledOutputs; i++) {

        ledControl_updateRuntime(&ledHandle[i]);

        switch (ledHandle[i].outputMode) {

            case FORWARD:
            case REVERSE: {
                switch ((objects_e)ledHandle[i].pCurrentObject[0]) {

                    case SOB: {
                        if (ledHandle[i].runTime >= ledHandle[i].lifeTime) ledControl_nextObject(&ledHandle[i]);
                        break;
                    }

                    case DRE: {
                        if (ledHandle[i].runTime >= ledHandle[i].lifeTime) ledControl_nextObject(&ledHandle[i]);
                        break;
                    }

                    case LOB: {
                        if (ledHandle[i].runTime >= ledHandle[i].lifeTime) {
                            ledHandle[i].pwm = (u8)ledHandle[i].params[1];
                            ledControl_nextObject(&ledHandle[i]);
                        }
                        else ledHandle[i].pwm = ledControl_getLinearValue(0, ledHandle[i].lifeTime, ledHandle[i].params[0], ledHandle[i].params[1], ledHandle[i].runTime);
                        break;
                    }

                    case BLK: {
                        if (ledHandle[i].runTime >= ledHandle[i].lifeTime) ledControl_nextObject(&ledHandle[i]);
                        else {
                            u16 timeElapsed = ledHandle[i].runTime - ledHandle[i].params[5];
                            if (ledHandle[i].params[4] == 0) {
                                if (timeElapsed >= ledHandle[i].params[2]) {
                                    ledHandle[i].params[4] = 1;
                                    ledHandle[i].params[5] = ledHandle[i].runTime;
                                    ledHandle[i].pwm = (u8)ledHandle[i].params[1];
                                }
                            }
                            else {
                                if (timeElapsed >= ledHandle[i].params[3]) {
                                    ledHandle[i].params[4] = 0;
                                    ledHandle[i].params[5] = ledHandle[i].runTime;
                                    ledHandle[i].pwm = (u8)ledHandle[i].params[0];
                                }
                            }
                        }
                        break;
                    }

                    case DIM: {
                        if (ledHandle[i].runTime >= ledHandle[i].lifeTime) ledControl_nextObject(&ledHandle[i]);
                        else ledHandle[i].pwm = (u8)ledControl_getLinearValue(0, LED_MAX_AUX, ledHandle[i].params[0], ledHandle[i].params[1], auxSignal);
                        break;
                    }

                    case AWR: {
                        if ((ledHandle[i].params[7] == 0) && (ledHandle[i].runTime >= ledHandle[i].lifeTime)) {
                            sensor_disableOutput(ledHandle[i].outputNumber);
                            ledHandle[i].localControl = 1;
                            ledControl_nextObject(&ledHandle[i]);
                        }
                        else {
                            for (u8 j=0; j<ledHandle[i].params[8]; j++) {
                                s32 w = (s32)((s16)ledHandle[i].params[j]);
                                w *= ledHandle[i].runTimeIncrement;
                                w /= LED_DEFAULT_INCREMENT;
                                sensor_rotateAngle(ledHandle[i].outputNumber, j, (s16)w);
                            }
                        }
                        break;
                    }

                    case AFB: {
                        ledHandle_t* h = &ledHandle[i];
                        if (h->runTime >= h->lifeTime) ledControl_nextObject(h);
                        else {
                            switch (h->params[3]) {

                                case 0: {
                                    if (auxSignal > h->params[2]) {
                                        h->params[3] = 10;
                                        h->params[4] = ledControl_randScale(*(h->pRand), AFTERBURNER_MIN_NUM_TURNON, AFTERBURNER_MAX_NUM_TURNON);
                                    }
                                    break;
                                }

                                case 10: {
                                    if (auxSignal <= h->params[2]) {
                                        h->params[3] = 0;
                                        h->pwm = 0;
                                    }
                                    else {
                                        if (h->params[4] == 0) {
                                            u16 delayMin = ledControl_getLinearValue(h->params[2], LED_MAX_AUX, ledHandle[i].params[11], ledHandle[i].params[12], auxSignal);
                                            u16 delayMax = delayMin*AFTERBURNER_FLASH_TIME_MIN_MAX_RATIO;
                                            u16 nextFlashDelay = ledControl_randScale(*(h->pRand), delayMin, delayMax);
                                            h->params[5] = h->runTime + nextFlashDelay;
                                            h->params[3] = 20;
                                        }
                                        else {
                                            u16 nextOnTime = ledControl_randScale(*(h->pRand), AFTERBURNER_MIN_TURNON_TIME, AFTERBURNER_MAX_TURNON_TIME);
                                            h->params[5] = h->runTime + nextOnTime;
                                            h->params[3] = 11;
                                            h->pwm = (u8)ledControl_getLinearValue(h->params[2], LED_MAX_AUX, ledHandle[i].params[0], ledHandle[i].params[1], auxSignal);
                                        }
                                    }
                                    break;
                                }

                                case 11: {
                                    if (auxSignal <= h->params[2]) {
                                        h->params[3] = 0;
                                        h->pwm = 0;
                                    }
                                    else {
                                        if (h->runTime > h->params[5]) {
                                            u16 nextOffTime = ledControl_randScale(*(h->pRand), AFTERBURNER_MIN_TURNON_TIME, AFTERBURNER_MAX_TURNON_TIME);
                                            h->params[5] = h->runTime + nextOffTime;
                                            h->params[3] = 12;
                                            h->pwm = 0;
                                        }
                                    }
                                    break;
                                }

                                case 12: {
                                    if (h->runTime > h->params[5]) {
                                        h->params[4]--;
                                        h->params[3] = 10;
                                    }
                                    break;
                                }

                                case 20: {
                                    if (auxSignal <= h->params[2]) {
                                        h->params[4] = ledControl_randScale(*(h->pRand), AFTERBURNER_MIN_NUM_TURNOFF, AFTERBURNER_MAX_NUM_TURNOFF);
                                        h->params[3] = 30;
                                    }
                                    else {
                                        if (h->runTime > h->params[5]) {
                                            u16 onTimeMin = ledControl_getLinearValue(h->params[2], LED_MAX_AUX, ledHandle[i].params[11], ledHandle[i].params[12], auxSignal);
                                            u16 onTimeMax = onTimeMin*AFTERBURNER_FLASH_TIME_MIN_MAX_RATIO;
                                            u16 flashTime = ledControl_randScale(*(h->pRand), onTimeMin, onTimeMax);
                                            u8 reduction = (u8)ledControl_getLinearValue(h->params[2], LED_MAX_AUX, h->params[9], h->params[10], auxSignal);
                                            h->params[5] = h->runTime;
                                            h->params[6] = h->runTime + flashTime;
                                            h->params[7] = h->pwm;
                                            h->params[8] = reduction;
                                            h->params[3] = 21;
                                        }
                                        else {
                                            h->pwm = (u8)ledControl_getLinearValue(h->params[2], LED_MAX_AUX, ledHandle[i].params[0], ledHandle[i].params[1], auxSignal);
                                        }
                                    }
                                    break;
                                }

                                case 21: {
                                    if (h->runTime > h->params[6]) {
                                        u16 onTimeMin = ledControl_getLinearValue(h->params[2], LED_MAX_AUX, ledHandle[i].params[11], ledHandle[i].params[12], auxSignal);
                                        u16 onTimeMax = onTimeMin*AFTERBURNER_FLASH_TIME_MIN_MAX_RATIO;
                                        u16 flashTime = ledControl_randScale(*(h->pRand), onTimeMin, onTimeMax);
                                        h->params[5] = h->runTime;
                                        h->params[6] = h->runTime + flashTime;
                                        h->params[7] = h->pwm;
                                        h->params[3] = 22;
                                    }
                                    else {
                                        u16 endColor = (u8)ledControl_getLinearValue(h->params[2], LED_MAX_AUX, ledHandle[i].params[0], ledHandle[i].params[1], auxSignal);
                                        endColor     = (endColor * h->params[8])>>8;
                                        u8 color     = ledControl_getLinearValue(h->params[5], h->params[6], h->params[7] , endColor, h->runTime);
                                        h->pwm = color;
                                    }
                                    break;
                                }

                                case 22: {
                                    if (h->runTime > h->params[6]) {
                                        u16 delayMin = (u8)ledControl_getLinearValue(h->params[2], LED_MAX_AUX, ledHandle[i].params[11], ledHandle[i].params[12], auxSignal);
                                        u16 delayMax = delayMin*AFTERBURNER_FLASH_TIME_MIN_MAX_RATIO;
                                        u16 nextFlashDelay = ledControl_randScale(*(h->pRand), delayMin, delayMax);
                                        h->params[5] = h->runTime + nextFlashDelay;
                                        h->params[3] = 20;
                                    }
                                    else {
                                        u16 endColor = (u8)ledControl_getLinearValue(h->params[2], LED_MAX_AUX, ledHandle[i].params[0], ledHandle[i].params[1], auxSignal);
                                        u8 color     = ledControl_getLinearValue(h->params[5], h->params[6], h->params[7] , endColor, h->runTime);
                                        h->pwm = color;
                                    }
                                    break;
                                }

                                case 30: {
                                    if (h->params[4] == 0) {
                                        h->params[3] = 0;
                                    }
                                    else {
                                        u16 nextOffTime = ledControl_randScale(*(h->pRand), AFTERBURNER_MIN_TURNOFF_TIME, AFTERBURNER_MAX_TURNOFF_TIME);
                                        h->params[5] = h->runTime + nextOffTime;
                                        h->params[3] = 31;
                                    }
                                    h->pwm = 0;
                                    break;
                                }

                                case 31: {
                                    if (auxSignal > h->params[2]) {
                                        h->params[5] = 0;
                                        h->params[3] = 20;
                                    }
                                    else {
                                        if (h->runTime > h->params[5]) {
                                            u16 nextOnTime = ledControl_randScale(*(h->pRand), AFTERBURNER_MIN_TURNOFF_TIME, AFTERBURNER_MAX_TURNOFF_TIME);
                                            h->params[5] = h->runTime + nextOnTime;
                                            h->params[3] = 32;
                                            h->pwm = ledHandle[i].params[0];
                                        }
                                    }
                                    break;
                                }

                                case 32: {
                                    if (h->runTime > h->params[5]) {
                                        h->params[4]--;
                                        h->params[3] = 30;
                                    }
                                    break;
                                }

                            }
                        }
                        break;
                    }

                    case DBG: {
                        ledHandle_t* h = &ledHandle[i];
                        ledHandle[i].pwm = 0;
                        if (h->runTime >= h->lifeTime) ledControl_nextObject(h);
                        break;
                    }

                    default: {
                        //invalid object, stop output
                        ledHandle[i].localControl = 1;
                        ledHandle[i].outputMode = STOP;
                        ledHandle[i].pwm = 0;
                        break;
                    }
                }
                break;
            }

            default: {
                break;
            }
        }
    }

    /***************************************************************************
    * Neue Werte in sortiertes Array schreiben
    ***************************************************************************/
    for (u8 i=0; i<numControledOutputs; i++) {
        u32 newPwm = ledHandle[i].pwm;
        if (batteryWarning && (runMode == RUNMODE_MASTER)) {
            newPwm *= dimBatteryPreset;
            newPwm /= LED_MAX_DIM_BATT_PRESET;
        }
        else {
            newPwm *= ledHandle[i].sequenceDim;
            newPwm /= LED_DEFAULT_DIM;
        }
        if (ledHandle[i].outputNumber != LED_OUTPUT_UNLINKED) pwmBuffer[ledHandle[i].outputNumber] = (u8)newPwm;
    }

    /***************************************************************************
    * Neue Werte aktivieren
    ***************************************************************************/
    for (u8 i=0; i<numControledOutputs; i++) {
        u8 currentOutputNumber = ledHandle[i].outputNumber;
        if (currentOutputNumber < LED_NUM_OUTPUTS) {
            if (ledHandle[i].localControl) *((u16*)pwmValue[currentOutputNumber]) = pwmTable[pwmBuffer[currentOutputNumber]];
        }
    }

    if (!(skyBus_tx_busy())) skyBus_send(LED_SKYBUS_SLAVE_PORT, LED_MAX_NUM_OUTPUTS - LED_NUM_OUTPUTS, &(pwmBuffer[LED_NUM_OUTPUTS]));

    ledControl_randUpdate();
}

/******************************************************************************
 * ledControl_updateRuntime
 ******************************************************************************/
void ledControl_updateRuntime(ledHandle_t* h) {
    h->runTimeExtended += h->runTimeIncrement;
    h->runTime = h->runTimeExtended >> 16;
}

/******************************************************************************
 * ledControl_clearRuntime
 ******************************************************************************/
void ledControl_clearRuntime(ledHandle_t* h) {
    h->runTimeExtended -= h->lifeTime << 16;
    h->runTime = h->runTimeExtended >> 16;
}

/******************************************************************************
 * ledControl_reinitRuntime
 ******************************************************************************/
void ledControl_reinitRuntime(ledHandle_t* h) {
    h->runTimeExtended = 0;
    h->runTime = 0;
}

/******************************************************************************
 * ledControl_skyBusCallback, Datenhandling
 ******************************************************************************/
void ledControl_skyBusCallback(u8 port, u8 len, u8* pData) {

    if (runMode == RUNMODE_DISABLED)   return;
    if (runMode == RUNMODE_SIMULATION) return;

    //pr�fe den port
    if (port == LED_SKYBUS_SLAVE_PORT) {

        //wenn noch nicht im slave modus, jetzt wechseln
        if (runMode == RUNMODE_MASTER) {
            runMode = RUNMODE_SLAVE;
            ledControl_setupExtension(true);
            ledControl_stopSequence();
        }

        //pr�fe die anzahl bytes, gegebenenfalls das paket weiterschicken
        if (len > LED_NUM_OUTPUTS) {
            for (u8 i=0; i<LED_NUM_OUTPUTS; i++) *((u16*)pwmValue[i]) = pwmTable[pData[i]];
            skyBus_send(LED_SKYBUS_SLAVE_PORT, len - LED_NUM_OUTPUTS, pData + LED_NUM_OUTPUTS);
        }
        else {
            for (u8 i=0; i<len; i++) *((u16*)pwmValue[i]) = pwmTable[pData[i]];
            for (u8 i=len; i<LED_NUM_OUTPUTS; i++) *((u16*)pwmValue[i]) = 0;
        }

        masterTimeout = 0;
    }
    else {
        //andere Pakete direkt weiterleiten
        skyBus_send(port, len, pData);
    }
}

/******************************************************************************
 * ledControl_lowSpeed
 ******************************************************************************/
void ledControl_lowSpeed() {

    /**************************************************************************
    * Batteriespannung �berwachen und Batteriedimmer berechnen
    ***************************************************************************/
    //batteriespannung vom Millivolt auf Zehntelvolt runden
    battVoltageRounded = (adc_battery + 50) / 100;
    //pr�fe ob batteriespannung zu tief und setzet gegebenenfalls flag
    if ((!batteryWarning) && (battWarningThreshold) && (battVoltageRounded) && (battVoltageRounded <= battWarningThreshold)) batteryWarning = true;
    dimBatteryPreset++;
    if (dimBatteryPreset > LED_MAX_DIM_BATT_PRESET) dimBatteryPreset = 0;

    /**************************************************************************
    * Empf�nger und Aux berechnen
    ***************************************************************************/
    //Dimmervorgabe berechnen
    if (runMode == RUNMODE_SIMULATION) {
        auxSignal = auxSim;
        rxFailsafe = false;
        rxSignal = 0;
    }
    else {
        rxFailsafe = recv_failsafe();
        rxSignal   = recv_signal();
        if (rxFailsafe) auxSignal = LED_MAX_AUX >> 1;
        else auxSignal = (u16)ledControl_getLinearValue(dimXMin, dimXMax, 0, LED_MAX_AUX, rxSignal);
    }

    /**************************************************************************
    * Variables Zeitinkrement und Dimmerwert berechnen
    ***************************************************************************/
    if (rxFailsafe) {
        variableTimeIncrement = LED_DEFAULT_INCREMENT;
        variableSequenceDim = LED_DEFAULT_DIM;
    }
    else {
        u8 index = auxSignal / 10;
        variableTimeIncrement = timeFactorTable[index];
        variableSequenceDim = (u8)ledControl_getLinearValue(0, LED_MAX_AUX, 0, LED_DEFAULT_DIM, auxSignal);
    }

    for (u8 i=0; i<LED_MAX_NUM_OUTPUTS; i++) {
        if (ledHandle[i].incrementMode == VARIABLE_INCREMENT) {
            ledHandle[i].runTimeIncrement = variableTimeIncrement;
            speedInfoReport = variableTimeIncrement;
        }
        if (ledHandle[i].sequenceDimMode == VARIABLE_DIM) {
            ledHandle[i].sequenceDim = variableSequenceDim;
            sequenceDimReport = variableSequenceDim;
        }
    }


    /**************************************************************************
    * Master Slave Timeout
    ***************************************************************************/
    if (masterTimeout >= LED_MAX_TIMEOUT) {
        if (runMode == RUNMODE_SLAVE) ledControl_init();
    }
    else masterTimeout++;

    /**************************************************************************
    * Status LED
    ***************************************************************************/
    if (runMode == RUNMODE_MASTER) {
        if (currentSequence == LED_SEQUENCE_NONE) statusLed_setState(STATUS_LED_BLINK_1);
        else statusLed_setState(STATUS_LED_ON);
    }
    else if (runMode == RUNMODE_SLAVE) statusLed_setState(STATUS_LED_BLINK_2);
    else statusLed_setState(STATUS_LED_BLINK_3);

    /**************************************************************************
    * Sequenzkontrolle
    ***************************************************************************/
    autoSelectionActive = false;
    if (runMode == RUNMODE_MASTER) {
        if (selectionMode == SELECTION_RECEIVER) {
            if (!rxFailsafe) {
                if (rxSignal < -LED_RX_THRESHOLD) ledControl_stopSequence();
                else if (rxSignal > +LED_RX_THRESHOLD) {
                    if (currentSequence == LED_SEQUENCE_NONE) ledControl_startSequence(0);
                    autoSelectionActive = true;
                    auxSignal = LED_MAX_AUX >> 1;
                }
                else {
                    u8 nextSequence = 0;
                    s16 currentTestValue = -100;
                    for (u8 i=0; i<numSequences; i++) {
                        if (rxSignal > currentTestValue) nextSequence = i;
                        currentTestValue += sectionWidth;
                    }
                    ledControl_startSequence(nextSequence);

                    dimXMin = -100 + (nextSequence*sectionWidth) + LED_RX_AREA_OFFSET;
                    dimXMax = -100 + ((nextSequence+1)*sectionWidth) - LED_RX_AREA_OFFSET;
                }
            }
            else if (currentSequence == LED_SEQUENCE_NONE) ledControl_startSequence(0);
        }
        else {
            dimXMin = -100;
            dimXMax = +100;

            ledControl_jumperDebounce();
            u8 jumper = jumperDebounce;
            if (jumper >= numSequences) jumper = numSequences-1;
            if (rxFailsafe) ledControl_startSequence(jumper);
            else {
                if (rxSignal < -LED_RX_THRESHOLD) ledControl_stopSequence();
                else ledControl_startSequence(jumper);
            }
        }
    }
}

/******************************************************************************
 * ledControl_nextObject
 ******************************************************************************/
void ledControl_nextObject(ledHandle_t* h) {

    /**************************************************************************
    * Normale Steuerung
    ***************************************************************************/
    if (runMode != RUNMODE_SIMULATION) {
        if (h->outputMode == FORWARD) {
            u8* pNext = (u8*)(h->pCurrentObject + LED_OBJECT_SIZE);
            //check if the next is an expansion, if so, skip and call next again
            if (*pNext == EXT) {
                h->pCurrentObject = pNext;
                ledControl_nextObject(h);
            }
            else ledControl_startObject(h, pNext);
        }
        else if (h->outputMode == REVERSE) {
            if (h->pCurrentObject <= h->pObjectTableBase) {
                h->outputMode = FORWARD;
                ledControl_startObject(h, h->pObjectTableBase);
            }
            else {
                u8* pNext = (u8*)(h->pCurrentObject - LED_OBJECT_SIZE);
                //check if the next is an expansion, if so, skip and call next again
                if (*pNext == EXT) {
                    h->pCurrentObject = pNext;
                    ledControl_nextObject(h);
                }
                else ledControl_startObject(h, pNext);
            }
        }
        else {
            h->outputMode = STOP;
        }
    }
    /**************************************************************************
    * Simulationsmodus
    ***************************************************************************/
    else {
        //pr�fe ob der ausgang noch weitere objekte hat
        u8* p = ledControl_consumeSimObject(h);
        //wenn es objekte gibt, das n�chste objekt starten
        if (p != null) {
            //check if it is an extension object
            if (*p == EXT) {
                //save it and go to next
                for (u8 i=0; i<LED_OBJECT_SIZE; i++) h->extensionBackup[i] = p[i];
                ledControl_nextObject(h);
            }
            else ledControl_startObject(h, p);
        }
        else {
            h->outputMode = STOP;
        }
    }
}

/******************************************************************************
 * ledControl_startObject
 ******************************************************************************/
void ledControl_startObject(ledHandle_t* h, u8* pObject) {

    h->pCurrentObject = pObject;
    h->localControl = 1;
    sensor_disableOutput(h->outputNumber);

    switch ((objects_e)h->pCurrentObject[0]) {

        case SOB: {
            u16 lifeTime = ((u16*)h->pCurrentObject)[1];

            ledControl_clearRuntime(h);

            if (h->outputType == OUTPUT_TYPE_DREAMSTRIP) dreamEffect_Stop(h->dreamAddress);

            h->pwm = h->pCurrentObject[1];
            h->lifeTime = lifeTime;
            break;
        }

        case DRE: {
            u16 lifeTime = ((u16*)h->pCurrentObject)[1];
            u8  effectId = h->pCurrentObject[4];
            u16  fadeTime = (u16)(h->pCurrentObject[1])*100;
            u8  brightness = h->pCurrentObject[5];

            ledControl_clearRuntime(h);

            if (h->outputType == OUTPUT_TYPE_DREAMSTRIP) {
                dreamEffect_Start(h->dreamAddress, h->dreamNumPixel, lifeTime, fadeTime, brightness, effectId, &h->pCurrentObject[6]);
            }

            h->lifeTime = lifeTime;
            break;
        }

        case LOB: {
            u16 start    = h->pCurrentObject[1];
            u16 end      = h->pCurrentObject[2];
            u16 lifeTime = ((u16*)h->pCurrentObject)[2];

            ledControl_clearRuntime(h);

            if (h->outputMode == FORWARD) {
                h->params[0] = start;
                h->params[1] = end;
            }
            else {
                h->params[0] = end;
                h->params[1] = start;
            }
            h->pwm = (u8)h->params[0];

            h->lifeTime = lifeTime;
            break;
        }

        case BLK: {
            u8  valueOn    = h->pCurrentObject[1];
            u8  valueOff   = h->pCurrentObject[2];
            u8  dutyCycle  = h->pCurrentObject[3];
            u16 period     = ((u16*)h->pCurrentObject)[3];
            u16 lifeTime   = ((u16*)h->pCurrentObject)[2];

            ledControl_clearRuntime(h);

            if (dutyCycle > 100) dutyCycle = 100;
            u16 timeOn  = (((u32)period)*dutyCycle) / 100;
            u16 timeOff = period - timeOn;

            if (h->outputMode == FORWARD) {
                h->params[0] = valueOn;
                h->params[1] = valueOff;
                h->params[2] = timeOn;
                h->params[3] = timeOff;
            }
            else {
                h->params[0] = valueOff;
                h->params[1] = valueOn;
                h->params[2] = timeOff;
                h->params[3] = timeOn;
            }
            h->params[4] = 0;
            h->params[5] = 0;
            h->pwm = (u8)h->params[0];
            h->lifeTime = lifeTime;
            break;
        }

       case DIM: {
            u8  valueMin      = h->pCurrentObject[1];
            u8  valueMax      = h->pCurrentObject[2];
            u8  valueFailsafe = h->pCurrentObject[3];
            u16 lifeTime      = ((u16*)h->pCurrentObject)[2];

            ledControl_clearRuntime(h);

            h->params[0] = valueMin;
            h->params[1] = valueMax;
            h->params[2] = valueFailsafe;
            h->pwm = (u8)ledControl_getLinearValue(0, LED_MAX_AUX, valueMin, valueMax, auxSignal);
            h->lifeTime = lifeTime;
            break;
        }

        case AWR: {
            u16 offset    = (u16)((h->pCurrentObject[1])*2);
            u8  endless   = 0;
            u8  numAngles = 0;
            u16 lifeTime  = ((u16*)h->pCurrentObject)[1];

            ledControl_clearRuntime(h);

            u8* pObj = h->pCurrentObject;
            u8* pExt = ledControl_getExtensionObject(h);

            if (pExt != null) endless = pExt[1];

            for (u8 i=0; i<4; i++) {
                u16 on  = (u16)((pObj[i*2 + 4])*2);
                u16 off = (u16)((pObj[i*2 + 5])*2);
                h->params[i] = 0;
                if ((on > 0) || (off > 0)) {
                    on  = (on  + offset) % 360;
                    off = (off + offset) % 360;
                    sensor_setAngle(h->outputNumber, numAngles, on, off);
                    s32 w;
                    if (pExt == null) w = 0;
                    else w = ((s16)((s8)pExt[i + 4])) * 2;
                    w *= h->runTimeIncrement;
                    w /= LED_DEFAULT_INCREMENT;
                    if (h->outputMode == FORWARD) h->params[numAngles] = (u16)w;
                    else h->params[numAngles] = (u16)(-w);
                    numAngles++;
                }
            }
            h->params[7] = endless;
            h->params[8] = numAngles;
            h->localControl = 0;
            sensor_enableOutput(h->outputNumber, numAngles);
            h->lifeTime = lifeTime;

            //TRACE(intToString(h->outputNumber, false));

            break;
        }

        case AFB: {
            u16 lifeTime        = ((u16*)h->pCurrentObject)[1];
            u8  startColor      = h->pCurrentObject[4];
            u8  endColor        = h->pCurrentObject[5];
            u16 tripPoint       = h->pCurrentObject[1]*10;
            u8  substate        = 0;

            ledControl_clearRuntime(h);

            if (auxSignal > tripPoint) substate = 20;

            h->params[0] = startColor;
            h->params[1] = endColor;
            h->params[2] = tripPoint;
            h->params[3] = substate;
            h->params[4] = 0;
            h->params[5] = 0;
            h->params[6] = 0;
            h->params[7] = 0;
            h->params[8] = 0;
            h->params[9] = reductionTable[(u8)(h->pCurrentObject[6] & 0x0F)];
            h->params[10]= reductionTable[(u8)(h->pCurrentObject[6] >> 4)];
            h->params[11]= flashTimeArray[(u8)(h->pCurrentObject[7] & 0x0F)];
            h->params[12]= flashTimeArray[(u8)(h->pCurrentObject[7] >> 4)];

            if (substate == 0) h->pwm = 0;

            h->lifeTime = lifeTime;
            break;
        }

        case DBG: {
            u8  debugBytes[6];
            u16 lifeTime = ((u16*)h->pCurrentObject)[1];
            for (u8 i=0; i<6; i++) debugBytes[i] = ((u8*)h->pCurrentObject)[4+i];
            ledControl_clearRuntime(h);

            //DEBUG COLDE
            if (selectionMode == SELECTION_RECEIVER) {
                dreamEffect_Start(0, 18, lifeTime, 1000, 255, debugBytes[0], &(debugBytes[1]));
            }
            h->pwm = 0;
            //END DEBUG CODE

            TRACE("DEBUG OBJECT EXECUTED ");
            h->lifeTime = lifeTime;
            break;
        }

        case EOD: {
            h->lineEnd = 1;
            if (runMode != RUNMODE_SIMULATION) if (ledControl_checkAutoSequence()) return;

            switch (h->pCurrentObject[1]) {
                //start first object again
                //in simulation mode start next object
                case 1: {
                    h->outputMode = FORWARD;
                    if (runMode == RUNMODE_SIMULATION) ledControl_nextObject(h);
                    else ledControl_startObject(h, h->pObjectTableBase);
                    break;
                }
                //change direction and start previous object
                //in simulation mode start next object
                case 2: {
                    h->outputMode = REVERSE;
                    if (runMode == RUNMODE_SIMULATION) ledControl_nextObject(h);
                    else ledControl_startObject(h, (u8*)(pObject - LED_OBJECT_SIZE));
                    break;
                }
                //other parameter, just stop sequence
                default: {
                    h->outputMode = STOP;
                    break;
                }
            }
            break;
        }

        default: {
            //invalid object, stop output
            h->outputMode = STOP;
            h->pwm = 0;
            break;
        }
    }
}

/******************************************************************************
 * ledControl_getExtension
 ***************************AFB***************************************************/
u8* ledControl_getExtensionObject(ledHandle_t* h) {
    if (runMode == RUNMODE_SIMULATION) {
        if (h->outputMode == REVERSE) return h->extensionBackup;
        else return ledControl_consumeSimObject(h);
    }
    return (u8*)(h->pCurrentObject + LED_OBJECT_SIZE);
}

/******************************************************************************
 * ledControl_consumeSimObject
 ******************************************************************************/
u8* ledControl_consumeSimObject(ledHandle_t* h) {
    u8 numObjects = ledControl_getNumSimObjects(h);
    if (numObjects > 0) {
        u8 ri = h->simReadIndex;
        u8 ni = (ri + 1) % LED_NUM_SIM_OBJECTS;
        h->simReadIndex = ni;
        return h->simObjects[ri];
    }
    return null;
}

/******************************************************************************
 * ledControl_startSequence
 ******************************************************************************/
void ledControl_startSequence(u8 sequenceNumber) {

    if (sequenceNumber >= numSequences) return;
    if (currentSequence == sequenceNumber) return;

    ledControl_stopSequence();

    for (u8 i=0; i<LED_MAX_NUM_OUTPUTS; i++) ledHandle[i].outputNumber = LED_OUTPUT_UNLINKED;

    currentSequence = sequenceNumber;

    u8*  sequenceHeader = (u8*)(configBase + pSequenceTable[currentSequence]);
    u8   outputs = sequenceHeader[0x0C];
    u8   dimInfo = sequenceHeader[0x0D];
    u32  speedInfo = *((u32*)&sequenceHeader[0x10]);

    u32* offsetTable = (u32*)&sequenceHeader[0x1C];
    u8*  optionTable = (u8*)((u32)offsetTable + (outputs << 2));
    u8*  rgbTable =    (u8*)((u32)optionTable + (outputs << 2));

    if (outputs > numControledOutputs) outputs = numControledOutputs;

    numOutputsReport = outputs;
    speedInfoReport = speedInfo;
    sequenceDimReport = dimInfo;

    for (u8 i=0; i<outputs; i++) {

        ledHandle[i].incrementMode = FIX_INCREMENT;
        ledHandle[i].runTimeIncrement = speedInfo;
        if (speedInfo == 0) {
            ledHandle[i].incrementMode = VARIABLE_INCREMENT;
            ledHandle[i].runTimeIncrement = LED_DEFAULT_INCREMENT;
        }

        ledHandle[i].sequenceDimMode = FIX_DIM;
        ledHandle[i].sequenceDim = dimInfo;
        if (dimInfo == 0) {
            ledHandle[i].sequenceDimMode = VARIABLE_DIM;
            ledHandle[i].sequenceDim = LED_DEFAULT_DIM;
        }

        ledHandle[i].outputMode = FORWARD;
        ledHandle[i].pObjectTableBase = (u8*)(configBase + offsetTable[i]);
        ledHandle[i].lineEnd = 0;

        //check output type and set settings
        u8* pOption = &optionTable[4*i];
        if ((outputType_e)pOption[0] == OUTPUT_TYPE_LED) {
            ledHandle[i].outputType = OUTPUT_TYPE_LED;
            ledHandle[i].outputNumber = pOption[1];
        }
        if ((outputType_e)pOption[0] == OUTPUT_TYPE_DREAMSTRIP) {
            ledHandle[i].outputType = OUTPUT_TYPE_DREAMSTRIP;
            ledHandle[i].outputNumber = LED_OUTPUT_UNLINKED;
            ledHandle[i].dreamAddress = pOption[1];
            ledHandle[i].dreamNumPixel  = pOption[2];
            ledHandle[i].dreamNumPixel += pOption[3] << 8;
        }

        //check if the first object is an EOL. In this case, do not start
        if ((objects_e)ledHandle[i].pObjectTableBase[0] == EOD) {
            ledHandle[i].outputMode = STOP;
            ledHandle[i].pwm = 0;
        }
        else {
            ledControl_startObject(&ledHandle[i], ledHandle[i].pObjectTableBase);
        }

        u8 channel = (rgbTable[i] & 0x1F) % LED_NUM_RANDOM_NUMBERS;
        ledHandle[i].pRand = &randArray[channel];
    }

}

/******************************************************************************
 * ledControl_getLinearValue
 ******************************************************************************/
s16 ledControl_getLinearValue(s16 xMin, s16 xMax, s16 yMin, s16 yMax, s16 x) {
    if (xMin < xMax) {
        if (x < xMin) return yMin;
        if (x > xMax) return yMax;
    }
    else {
        if (x > xMin) return yMin;
        if (x < xMax) return yMax;
    }

    s32 dX = xMax - xMin;
    s32 dY = yMax - yMin;
    s32 x0 = x - xMin;
    return (s16)(((x0*dY)/dX) + yMin);
}

/******************************************************************************
 * ledControl_stopSequence
 ******************************************************************************/
void ledControl_stopSequence() {

    for (u8 i=0; i<LED_NUM_OUTPUTS; i++) sensor_disableOutput(i);
    for (u8 i=0; i<LED_MAX_NUM_OUTPUTS; i++) {
        ledHandle[i].outputMode = STOP;
        ledHandle[i].localControl = 1;
        ledHandle[i].lineEnd = 1;
        ledHandle[i].pwm = 0;
        ledHandle[i].lifeTime = 0;
        ledHandle[i].incrementMode = FIX_INCREMENT;
        ledHandle[i].runTimeIncrement = LED_DEFAULT_INCREMENT;
        ledHandle[i].sequenceDimMode = FIX_DIM;
        ledHandle[i].sequenceDim = LED_DEFAULT_DIM;
        ledHandle[i].outputNumber = i;
        ledHandle[i].pRand = &randArray[0];
        ledHandle[i].outputType = OUTPUT_TYPE_LED;

        ledControl_reinitRuntime(&ledHandle[i]);
    }

    numOutputsReport = 0;
    speedInfoReport = LED_DEFAULT_INCREMENT;
    sequenceDimReport = LED_DEFAULT_DIM;
    currentSequence = LED_SEQUENCE_NONE;

    //alle dreamextensions stoppen
    dreamEffect_StopImmediate(DREAMEXTENSION_ADDRESS_BROADCAST);

}

/******************************************************************************
 * ledControl_setPWM
 ******************************************************************************/
void ledControl_setPWM(u8* values) {
    ledControl_stopSequence();
    runMode = RUNMODE_DISABLED;
    for (u8 i=0; i<numControledOutputs; i++) ledHandle[i].pwm = values[i];
}

/******************************************************************************
 * ledControl_enableAutoSequence
 ******************************************************************************/
void ledControl_activate(bool enable) {
    ledControl_simReset();
    ledControl_stopSequence();
    if (enable) runMode = RUNMODE_MASTER;
    else runMode = RUNMODE_DISABLED;
}

/******************************************************************************
 * ledControl_setSimObject
 ******************************************************************************/
void ledControl_setSimObjects(u8 output, u8 numObjects, u8* pObject) {
    //pr�fe ob ausgang g�ltig ist
    if (output >= LED_MAX_NUM_OUTPUTS) return;

    for (u8 j=0; j<numObjects; j++) {
        //check if buffer is full
        u8 ri = ledHandle[output].simReadIndex;
        u8 wi = ledHandle[output].simWriteIndex;
        u8 ni = (wi + 1) % LED_NUM_SIM_OBJECTS;
        //wenn puffer voll ist abbrechen
        if (ni == ri) return;
        //speichere objekt
        u8* p = pObject + (j*LED_OBJECT_SIZE);
        for (u8 i=0; i<LED_OBJECT_SIZE; i++) ledHandle[output].simObjects[wi][i] = p[i];
        ledHandle[output].simWriteIndex = ni;
    }
}

/******************************************************************************
 * ledControl_startSim
 ******************************************************************************/
void ledControl_startSim(u32 speedInfo, u8 dimInfo, bool useOffsetData, u8* pDirectionData, u8* pOffsetData) {
    ledControl_stopSequence();
    runMode = RUNMODE_SIMULATION;
    numOutputsReport = 0;

    u32 directionData = 0;
    if (useOffsetData) {
        directionData += pDirectionData[0] << 24;
        directionData += pDirectionData[1] << 16;
        directionData += pDirectionData[2] <<  8;
        directionData += pDirectionData[3];
    }

    for (u8 i=0; i<numControledOutputs; i++) {
        u8* p = ledControl_consumeSimObject(&ledHandle[i]);
        if (p != null) {
            ledHandle[i].outputMode = FORWARD;

            ledHandle[i].incrementMode = FIX_INCREMENT;
            ledHandle[i].runTimeIncrement = speedInfo;
            if (speedInfo == 0) {
                ledHandle[i].incrementMode = VARIABLE_INCREMENT;
                ledHandle[i].runTimeIncrement = LED_DEFAULT_INCREMENT;
            }

            ledHandle[i].sequenceDimMode = FIX_DIM;
            ledHandle[i].sequenceDim = dimInfo;
            if (dimInfo == 0) {
                ledHandle[i].sequenceDimMode = VARIABLE_DIM;
                ledHandle[i].sequenceDim = LED_DEFAULT_DIM;
            }

            //check if offset and Direction is used
            if (useOffsetData) {
                u32 mask = 1 << i;
                if (directionData & mask) ledHandle[i].outputMode = REVERSE;
                u32 offset = 0;
                offset += pOffsetData[i*2 + 0] << 8;
                offset += pOffsetData[i*2 + 1];
                offset = offset << 16;
                ledHandle[i].runTimeExtended = offset;
            }

            //check if the first object is an eod. In this case, do not start
            if ((objects_e)p[0] == EOD) {
                ledHandle[i].outputMode = STOP;
                ledHandle[i].pwm = 0;
            }
            else {
                ledControl_startObject(&ledHandle[i], p);
            }

            numOutputsReport++;
        }
    }

    speedInfoReport = speedInfo;
    sequenceDimReport = dimInfo;
    auxSim = LED_MAX_AUX >> 1;
}

/******************************************************************************
 * ledControl_simReset
 ******************************************************************************/
void ledControl_simReset(void) {
    for (u8 i=0; i<LED_MAX_NUM_OUTPUTS; i++) {
        ledHandle[i].simReadIndex = 0;
        ledHandle[i].simWriteIndex = 0;
    }
}

/******************************************************************************
 * ledControl_getNumSimObjects
 ******************************************************************************/
u8 ledControl_getNumSimObjects(ledHandle_t* h) {
    //pr�fe wieviele simulationsobjekte vorhanden sind
    s8 numObjects = h->simWriteIndex - h->simReadIndex;
    if (numObjects < 0) numObjects += LED_NUM_SIM_OBJECTS;
    return (u8)numObjects;
}

/******************************************************************************
 * ledControl_getBufferInfo
 ******************************************************************************/
void ledControl_getBufferInfo(u8* rsp) {
    rsp[0] = LED_RESPONSE_BUFFER_INFO;
    rsp[1] = LED_NUM_SIM_OBJECTS - 1;
    rsp[2] = LED_NUM_OBJECTS_PER_TRANSFER;
    rsp[3] = LED_MAX_NUM_OUTPUTS;
}

/******************************************************************************
 * ledControl_getBufferState
 ******************************************************************************/
void ledControl_getBufferState(u8* rsp) {
    rsp[0] = LED_RESPONSE_BUFFER_STATE;
    for (u8 i=0; i<LED_MAX_NUM_OUTPUTS; i++) rsp[i+1] = ledControl_getNumSimObjects(&ledHandle[i]);
}

/******************************************************************************
 * ledControl_setupExtension
 ******************************************************************************/
void ledControl_setupExtension(bool enableTx) {

    jumper_init();

    if (enableTx) skyBus_init(SKYBUS_MODE_RX_TX);
    else skyBus_init(SKYBUS_MODE_RX_ONLY);

    skyBus_registerCallback((skybusCallback_t)*ledControl_skyBusCallback);
}

/******************************************************************************
 * ledControl_checkAutoSequence
 ******************************************************************************/
bool ledControl_checkAutoSequence(void) {
    if (numSequences <= 1) return false;
    if (!autoSelectionActive) return false;

    for (u8 i=0; i<numControledOutputs; i++) if (ledHandle[i].lineEnd == 0) return false;

    u8 nextSequence = (currentSequence + 1) % numSequences;
    ledControl_startSequence(nextSequence);
    return true;
}

/******************************************************************************
 * ledControl_randUpdate
 ******************************************************************************/
void ledControl_randUpdate(void) {
    for (u8 i=0; i<LED_NUM_RANDOM_NUMBERS; i++) randArray[i] = *pFlashRand++;
    if ((u32)pFlashRand > pRANDSTOP) pFlashRand = (u16*)pRANDSTART;
}

/******************************************************************************
 * ledControl_randScale
 ******************************************************************************/
u16 ledControl_randScale(u16 rand, u16 min, u16 max) {
    u16 mod = max - min + 1;
    return ((rand % mod) + min);
}

/******************************************************************************
 * ledControl_jumperDebounce
 ******************************************************************************/
void ledControl_jumperDebounce() {

    u8 jumper = jumper_update();

    for (u8 i=LED_JUMPER_DEBOUNCE_LEN-1; i>0; i--) jumperDebounceArray[i] = jumperDebounceArray[i-1];
    jumperDebounceArray[0] = jumper;

    bool ok = true;
    for (u8 i=0; i<LED_JUMPER_DEBOUNCE_LEN; i++) if (jumperDebounceArray[i] != jumper) ok = false;

    if (ok) jumperDebounce = jumper;
}

/******************************************************************************
 * ledControl_init
 ******************************************************************************/
void ledControl_init() {

    pwm_init();

    //initialisiere variabeln und �berpr�fe die konfiguration
    configBase = (u8*)configBaseFlash;
    bool configOk = true;
    //pr�fe crc der konfiguration
    ledControl_configSize = ((u32*)configBase)[0];
    if (ledControl_configSize > configConfigSize) configOk = false;
    else {
        u16 crcCalc  = crc_calc(configBase, ledControl_configSize);
        u16 crcFlash = *((u16*)(&configBase[ledControl_configSize]));
        if(crcCalc != crcFlash) configOk = false;
    }

    //pr�fe config inhalte
    numSequences = configBase[4];
    u8 configType    = configBase[6];
    u8 configVersion = configBase[7];
    if (numSequences == 0x00) configOk = false;
    if (numSequences == 0xFF) configOk = false;
    if (configType != CONFIG_TYPE_SKYLED) configOk = false;
    if (configVersion != 2) configOk = false;

    //variabeln initialisieren
    if (!configOk) {
        ledControl_configSize = 0;
        configBase = (u8*)configBaseBackup;
    }

    //selection mode und batterie-abschaltspannung stehen im boarcConfig
    selectionMode = (selectionMode_e)boardConfig.modeSelection;
    battWarningThreshold = boardConfig.batteryMinVoltage;

    //variabeln definitiv initialisieren
    numSequences = configBase[4];
    pSequenceTable = (u32*)&configBase[0x18];
    sectionWidth = 200 / numSequences;
    batteryWarning = false;
    runMode = RUNMODE_MASTER;

    if (selectionMode == SELECTION_JUMPER) {
        ledControl_setupExtension(false);
        numControledOutputs = LED_NUM_OUTPUTS;
    }
    else {
        ledControl_setupExtension(true);
        numControledOutputs = LED_MAX_NUM_OUTPUTS;
    }

    //empfänger und sensorport neu initialisieren
    recv_init();

    jumperDebounce = jumper_update();

    variableTimeIncrement = LED_DEFAULT_INCREMENT;
    ledControl_activate(true);

    for (u8 i=0; i<LED_NUM_RANDOM_NUMBERS; i++) randArray[i] = 0;
    pFlashRand = (u16*)pRANDSTART;
}
