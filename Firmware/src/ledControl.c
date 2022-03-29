/***************************************************************************//**
 * @file        ledControl.h
 * @author      Christian Luethi
 * @brief       LED steuerung
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "ledControl.h"
#include "conf.h"
#include "pwm.h"
#include "teco.h"
#include "recv.h"
#include "statusLed.h"
#include "adc.h"
#include "masterSlave.h"
#include "crc.h"
#include <stdio.h>

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


/* Private typedef -----------------------------------------------------------*/
typedef enum {
    SOB = 0x01, //Set Output Brightness
    LOB = 0x02, //Linear Output Brightness
    BLK = 0x03, //Blink
    DIM = 0x04, //Dimm
    EOD = 0xF0, //End of Definition
    DBG = 0xF1  //Debug
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

typedef struct {
    outputMode_e outputMode;
    timeIncrementMode_e incrementMode;
    sequenceDimMode_e sequenceDimMode;
    uint8_t   pwm;
    uint8_t*  pObjectTableBase;
    uint8_t*  pCurrentObject;
    uint16_t  runTime;
    uint16_t  lifeTime;
    uint16_t  params[LED_NUM_PARAMS_PER_HANDLE];
    uint8_t   sequenceDim;
    uint8_t   simObjects[LED_NUM_SIM_OBJECTS][LED_OBJECT_SIZE];
    uint8_t   simReadIndex, simWriteIndex;
    uint8_t   lineEnd;
    uint8_t   localControl;
    uint8_t   outputNumber;
    uint32_t  runTimeExtended;
    uint32_t  runTimeIncrement;
    uint8_t   channelDim;
    uint8_t   minAux;
    uint8_t   maxAux;
} ledHandle_t;

/* Private functions ---------------------------------------------------------*/
void ledControl_startObject(ledHandle_t* h, uint8_t* pObject);
void ledControl_nextObject(ledHandle_t* h);
void ledControl_setupExtension(bool enableTx);
void ledControl_simReset(void);
bool ledControl_checkAutoSequence(void);
void ledControl_updateRuntime(ledHandle_t* h);
void ledControl_reinitRuntime(ledHandle_t* h);
void ledControl_clearRuntime(ledHandle_t* h);
uint16_t  ledControl_randScale(uint16_t rand, uint16_t min, uint16_t max);
uint8_t   ledControl_getNumSimObjects(ledHandle_t* h);
int16_t   ledControl_getLinearValue(int16_t xMin, int16_t xMax, int16_t yMin, int16_t yMax, int16_t x);
uint8_t*  ledControl_getExtensionObject(ledHandle_t* h);
uint8_t*  ledControl_consumeSimObject(ledHandle_t* h);

/* Private variables ---------------------------------------------------------*/
ledHandle_t ledHandle[LED_MAX_NUM_OUTPUTS];
bool rxFailsafe, batteryWarning, autoSelectionActive;
uint8_t   numSequences, currentSequence, masterTimeout;
uint8_t   battVoltageRounded, battWarningThreshold, numOutputsReport, sequenceDimReport, variableSequenceDim;
uint8_t   pwmBuffer[LED_MAX_NUM_OUTPUTS];
uint8_t*  configBase;
uint32_t* pSequenceTable;
uint16_t  sectionWidth, auxSignal, dimBatteryPreset, auxSim;
int16_t   dimXMin, dimXMax, rxSignal;
uint32_t  ledControl_configSize, variableTimeIncrement;
uint32_t  speedInfoReport;
uint16_t* pFlashRand;
runMode_e runMode;
uint16_t scope;

const uint32_t timeFactorTable[] = {
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

const uint8_t reductionTable[] = {
    255, 250, 245, 235,
    220, 200, 180, 160,
    140, 120, 100, 80,
    60, 40, 20, 0
};

const uint16_t flashTimeArray[] = {
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
    * LED Ausgänge neu berechnen
    ***************************************************************************/
    for (uint8_t i=0; i<LED_MAX_NUM_OUTPUTS; i++) {

        ledControl_updateRuntime(&ledHandle[i]);

        switch (ledHandle[i].outputMode) {

            case FORWARD:
            case REVERSE: {
                switch ((objects_e)ledHandle[i].pCurrentObject[0]) {

                    case SOB: {
                        if (ledHandle[i].runTime >= ledHandle[i].lifeTime) ledControl_nextObject(&ledHandle[i]);
                        break;
                    }

                    case LOB: {
                        if (ledHandle[i].runTime >= ledHandle[i].lifeTime) {
                            ledHandle[i].pwm = (uint8_t)ledHandle[i].params[1];
                            ledControl_nextObject(&ledHandle[i]);
                        }
                        else ledHandle[i].pwm = ledControl_getLinearValue(0, ledHandle[i].lifeTime, ledHandle[i].params[0], ledHandle[i].params[1], ledHandle[i].runTime);
                        break;
                    }

                    case BLK: {
                        if (ledHandle[i].runTime >= ledHandle[i].lifeTime) ledControl_nextObject(&ledHandle[i]);
                        else {
                            uint16_t timeElapsed = ledHandle[i].runTime - ledHandle[i].params[5];
                            if (ledHandle[i].params[4] == 0) {
                                if (timeElapsed >= ledHandle[i].params[2]) {
                                    ledHandle[i].params[4] = 1;
                                    ledHandle[i].params[5] = ledHandle[i].runTime;
                                    ledHandle[i].pwm = (uint8_t)ledHandle[i].params[1];
                                }
                            }
                            else {
                                if (timeElapsed >= ledHandle[i].params[3]) {
                                    ledHandle[i].params[4] = 0;
                                    ledHandle[i].params[5] = ledHandle[i].runTime;
                                    ledHandle[i].pwm = (uint8_t)ledHandle[i].params[0];
                                }
                            }
                        }
                        break;
                    }

                    case DIM: {
                        if (ledHandle[i].runTime >= ledHandle[i].lifeTime) ledControl_nextObject(&ledHandle[i]);
                        else ledHandle[i].pwm = (uint8_t)ledControl_getLinearValue(0, LED_MAX_AUX, ledHandle[i].params[0], ledHandle[i].params[1], auxSignal);
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
    for (uint8_t i=0; i<LED_MAX_NUM_OUTPUTS; i++) {
        uint32_t newPwm = ledHandle[i].pwm;

        //apply various dim's and put pwm value to buffer
        if (runMode != RUNMODE_DISABLED) newPwm = (newPwm*ledHandle[i].channelDim) >> 8;
        if (batteryWarning && (runMode == RUNMODE_MASTER)) newPwm = (newPwm*dimBatteryPreset) >> 8;
        else newPwm = (newPwm*ledHandle[i].sequenceDim) >> 8;
        if (ledHandle[i].outputNumber != LED_OUTPUT_UNLINKED) pwmBuffer[ledHandle[i].outputNumber] = (uint8_t)newPwm;
    }

    /***************************************************************************
    * Neue Werte aktivieren
    ***************************************************************************/
    for (uint8_t i=0; i<LED_MAX_NUM_OUTPUTS; i++) {
        uint8_t currentOutputNumber = ledHandle[i].outputNumber;
        if (currentOutputNumber < LED_NUM_OUTPUTS) {
            if (ledHandle[i].localControl) *((uint16_t*)pwmValue[currentOutputNumber]) = pwmTable[pwmBuffer[currentOutputNumber]];
        }
    }

    //if (!(skyBus_tx_busy())) skyBus_send(LED_SKYBUS_SLAVE_PORT, LED_MAX_NUM_OUTPUTS - LED_NUM_OUTPUTS, &(pwmBuffer[LED_NUM_OUTPUTS]));
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
void ledControl_skyBusCallback(uint8_t port, uint8_t len, uint8_t* pData) {

    /*
    if (runMode == RUNMODE_DISABLED)   return;
    if (runMode == RUNMODE_SIMULATION) return;

    //prüfe den port
    if (port == LED_SKYBUS_SLAVE_PORT) {

        //wenn noch nicht im slave modus, jetzt wechseln
        if (runMode == RUNMODE_MASTER) {
            runMode = RUNMODE_SLAVE;
            ledControl_setupExtension(true);
            ledControl_stopSequence();
        }

        //prüfe die anzahl bytes, gegebenenfalls das paket weiterschicken
        if (len > LED_NUM_OUTPUTS) {
            for (uint8_t i=0; i<LED_NUM_OUTPUTS; i++) *((uint16_t*)pwmValue[i]) = pwmTable[pData[i]];
            skyBus_send(LED_SKYBUS_SLAVE_PORT, len - LED_NUM_OUTPUTS, pData + LED_NUM_OUTPUTS);
        }
        else {
            for (uint8_t i=0; i<len; i++) *((uint16_t*)pwmValue[i]) = pwmTable[pData[i]];
            for (uint8_t i=len; i<LED_NUM_OUTPUTS; i++) *((uint16_t*)pwmValue[i]) = 0;
        }

        masterTimeout = 0;
    }
    else {
        //andere Pakete direkt weiterleiten
        skyBus_send(port, len, pData);
    }
    */
}

/******************************************************************************
 * ledControl_lowSpeed
 ******************************************************************************/
void ledControl_lowSpeed() {

    /**************************************************************************
    * Batteriespannung Überwachen und Batteriedimmer berechnen
    ***************************************************************************/
    //batteriespannung vom Millivolt auf Zehntelvolt runden (add 50mv to get propoer rounding)
    battVoltageRounded = (adc_battery + 50) / 100;
    //prüfe ob batteriespannung zu tief und setzet gegebenenfalls flag
    if ((!batteryWarning) && (battWarningThreshold) && (battVoltageRounded) && (battVoltageRounded <= battWarningThreshold)) batteryWarning = true;
    dimBatteryPreset++;
    if (dimBatteryPreset > LED_MAX_DIM_BATT_PRESET) dimBatteryPreset = 0;

    /**************************************************************************
    * Empfänger und Aux berechnen
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
        else auxSignal = (uint16_t)ledControl_getLinearValue(dimXMin, dimXMax, 0, LED_MAX_AUX, rxSignal);
    }

    /**************************************************************************
    * Variables Zeitinkrement und Dimmerwert berechnen
    ***************************************************************************/
    if (rxFailsafe) {
        variableTimeIncrement = LED_DEFAULT_INCREMENT;
        variableSequenceDim = LED_DEFAULT_DIM;
    }
    else {
        uint8_t index = auxSignal / 10;
        variableTimeIncrement = timeFactorTable[index];
        variableSequenceDim = (uint8_t)ledControl_getLinearValue(0, LED_MAX_AUX, 0, LED_DEFAULT_DIM, auxSignal);
    }

    for (uint8_t i=0; i<LED_MAX_NUM_OUTPUTS; i++) {
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
      
        if (!rxFailsafe) {
            if (rxSignal < -LED_RX_THRESHOLD) ledControl_stopSequence();
            else if (rxSignal > +LED_RX_THRESHOLD) {
                if (currentSequence == LED_SEQUENCE_NONE) ledControl_startSequence(0);
                autoSelectionActive = true;
                auxSignal = LED_MAX_AUX >> 1;
            }
            else {
                uint8_t nextSequence = 0;
                int16_t currentTestValue = -100;
                for (uint8_t i=0; i<numSequences; i++) {
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
            uint8_t* pNext = (uint8_t*)(h->pCurrentObject + LED_OBJECT_SIZE);
            ledControl_startObject(h, pNext);
        }
        else if (h->outputMode == REVERSE) {
            if (h->pCurrentObject <= h->pObjectTableBase) {
                h->outputMode = FORWARD;
                ledControl_startObject(h, h->pObjectTableBase);
            }
            else {
                uint8_t* pNext = (uint8_t*)(h->pCurrentObject - LED_OBJECT_SIZE);
                ledControl_startObject(h, pNext);
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
        //prüfe ob der ausgang noch weitere objekte hat
        uint8_t* p = ledControl_consumeSimObject(h);
        //wenn es objekte gibt, das nächste objekt starten
        if (p != null) ledControl_startObject(h, p);        
        else h->outputMode = STOP;
    }
}

/******************************************************************************
 * ledControl_startObject
 ******************************************************************************/
void ledControl_startObject(ledHandle_t* h, uint8_t* pObject) {

    h->pCurrentObject = pObject;
    h->localControl = 1;

    switch ((objects_e)h->pCurrentObject[0]) {

        case SOB: {
            uint16_t lifeTime = ((uint16_t*)h->pCurrentObject)[1];

            ledControl_clearRuntime(h);

            h->pwm = h->pCurrentObject[1];
            h->lifeTime = lifeTime;
            break;
        }

        case LOB: {
            uint16_t start    = h->pCurrentObject[1];
            uint16_t end      = h->pCurrentObject[2];
            uint16_t lifeTime = ((uint16_t*)h->pCurrentObject)[2];

            ledControl_clearRuntime(h);

            if (h->outputMode == FORWARD) {
                h->params[0] = start;
                h->params[1] = end;
            }
            else {
                h->params[0] = end;
                h->params[1] = start;
            }
            h->pwm = (uint8_t)h->params[0];

            h->lifeTime = lifeTime;
            break;
        }

        case BLK: {
            uint8_t  valueOn    = h->pCurrentObject[1];
            uint8_t  valueOff   = h->pCurrentObject[2];
            uint8_t  dutyCycle  = h->pCurrentObject[3];
            uint16_t period     = ((uint16_t*)h->pCurrentObject)[3];
            uint16_t lifeTime   = ((uint16_t*)h->pCurrentObject)[2];

            ledControl_clearRuntime(h);

            if (dutyCycle > 100) dutyCycle = 100;
            uint16_t timeOn  = (((uint32_t)period)*dutyCycle) / 100;
            uint16_t timeOff = period - timeOn;

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
            h->pwm = (uint8_t)h->params[0];
            h->lifeTime = lifeTime;
            break;
        }

       case DIM: {
            uint8_t  valueMin      = h->pCurrentObject[1];
            uint8_t  valueMax      = h->pCurrentObject[2];
            uint8_t  valueFailsafe = h->pCurrentObject[3];
            uint16_t lifeTime      = ((uint16_t*)h->pCurrentObject)[2];

            ledControl_clearRuntime(h);

            h->params[0] = valueMin;
            h->params[1] = valueMax;
            h->params[2] = valueFailsafe;
            h->pwm = (uint8_t)ledControl_getLinearValue(0, LED_MAX_AUX, valueMin, valueMax, auxSignal);
            h->lifeTime = lifeTime;
            break;
        }

        case DBG: {
            //uint8_t  debugBytes[6];
            uint16_t lifeTime = ((uint16_t*)h->pCurrentObject)[1];
            //for (uint8_t i=0; i<6; i++) debugBytes[i] = ((uint8_t*)h->pCurrentObject)[4+i];
            ledControl_clearRuntime(h);

            TRACE("DEBUG OBJECT EXECUTED");
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
                    else ledControl_startObject(h, (uint8_t*)(pObject - LED_OBJECT_SIZE));
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
 * ledControl_consumeSimObject
 ******************************************************************************/
uint8_t* ledControl_consumeSimObject(ledHandle_t* h) {
    uint8_t numObjects = ledControl_getNumSimObjects(h);
    if (numObjects > 0) {
        uint8_t ri = h->simReadIndex;
        uint8_t ni = (ri + 1) % LED_NUM_SIM_OBJECTS;
        h->simReadIndex = ni;
        return h->simObjects[ri];
    }
    return null;
}

/******************************************************************************
 * ledControl_startSequence
 ******************************************************************************/
void ledControl_startSequence(uint8_t sequenceNumber) {

    if (sequenceNumber >= numSequences) return;
    if (currentSequence == sequenceNumber) return;
    if (numSequences == LED_SEQUENCE_NONE) return;

    ledControl_stopSequence();

    for (uint8_t i=0; i<LED_MAX_NUM_OUTPUTS; i++) ledHandle[i].outputNumber = LED_OUTPUT_UNLINKED;

    currentSequence = sequenceNumber;

    uint8_t*  sequenceHeader = (uint8_t*)(configBase + pSequenceTable[currentSequence]);
    uint8_t   outputs = sequenceHeader[0x0C];
    uint8_t   sequenceDim = sequenceHeader[0x0D];
    uint32_t  sequenceSpeed = *((uint32_t*)&sequenceHeader[0x10]);
    uint32_t* channelTable = (uint32_t*)&sequenceHeader[0x14];
 
    if (outputs > LED_MAX_NUM_OUTPUTS) outputs = LED_MAX_NUM_OUTPUTS;

    numOutputsReport = outputs;
    speedInfoReport = sequenceSpeed;
    sequenceDimReport = sequenceDim;

    for (uint8_t i=0; i<outputs; i++) {

        //channel settings
        uint8_t*  channelHeader = (uint8_t*)(configBase + channelTable[i]);        
        uint32_t ledObjectsOffset = *((uint32_t*)&channelHeader[0x08]);

        ledHandle[i].outputNumber = channelHeader[0];
        ledHandle[i].channelDim = channelHeader[1];
        ledHandle[i].minAux = channelHeader[2];
        ledHandle[i].maxAux = channelHeader[3];
        ledHandle[i].outputMode = FORWARD;
        ledHandle[i].pObjectTableBase = (uint8_t*)(configBase + ledObjectsOffset);
        ledHandle[i].lineEnd = 0;
        
        //sequence speed control
        ledHandle[i].incrementMode = FIX_INCREMENT;
        ledHandle[i].runTimeIncrement = sequenceSpeed;
        if (sequenceSpeed == 0) {
            ledHandle[i].incrementMode = VARIABLE_INCREMENT;
            ledHandle[i].runTimeIncrement = LED_DEFAULT_INCREMENT;
        }

        //sequence dim control
        ledHandle[i].sequenceDimMode = FIX_DIM;
        ledHandle[i].sequenceDim = sequenceDim;
        if (sequenceDim == 0) {
            ledHandle[i].sequenceDimMode = VARIABLE_DIM;
            ledHandle[i].sequenceDim = LED_DEFAULT_DIM;
        }

        //check if the first object is an EOL. In this case, do not start
        if ((objects_e)ledHandle[i].pObjectTableBase[0] == EOD) {
            ledHandle[i].outputMode = STOP;
            ledHandle[i].pwm = 0;
        }
        else {
            ledControl_startObject(&ledHandle[i], ledHandle[i].pObjectTableBase);
        }

    }

}

/******************************************************************************
 * ledControl_getLinearValue
 ******************************************************************************/
int16_t ledControl_getLinearValue(int16_t xMin, int16_t xMax, int16_t yMin, int16_t yMax, int16_t x) {
    if (xMin < xMax) {
        if (x < xMin) return yMin;
        if (x > xMax) return yMax;
    }
    else {
        if (x > xMin) return yMin;
        if (x < xMax) return yMax;
    }

    int32_t dX = xMax - xMin;
    int32_t dY = yMax - yMin;
    int32_t x0 = x - xMin;
    return (int16_t)(((x0*dY)/dX) + yMin);
}

/******************************************************************************
 * ledControl_stopSequence
 ******************************************************************************/
void ledControl_stopSequence() {

    for (uint8_t i=0; i<LED_MAX_NUM_OUTPUTS; i++) {
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

        ledControl_reinitRuntime(&ledHandle[i]);
    }

    numOutputsReport = 0;
    speedInfoReport = LED_DEFAULT_INCREMENT;
    sequenceDimReport = LED_DEFAULT_DIM;
    currentSequence = LED_SEQUENCE_NONE;
}

/******************************************************************************
 * ledControl_setPWM
 ******************************************************************************/
void ledControl_setPWM(uint8_t* values) {
    ledControl_stopSequence();
    runMode = RUNMODE_DISABLED;
    for (uint8_t i=0; i<LED_MAX_NUM_OUTPUTS; i++) ledHandle[i].pwm = values[i];
}

/******************************************************************************
 * ledControl_activate
 ******************************************************************************/
void ledControl_activate(bool enable) {
    ledControl_simReset();
    ledControl_stopSequence();
    if (enable) runMode = RUNMODE_MASTER;
    else runMode = RUNMODE_DISABLED;
}

/******************************************************************************
 * ledControl_setSimChannelSettings
 ******************************************************************************/
bool ledControl_setSimChannelSettings(uint8_t output, uint8_t outputDim, uint8_t auxMin, uint8_t auxMax) {
    
    ledHandle[output].channelDim = outputDim;
    ledHandle[output].minAux = auxMin;
    ledHandle[output].maxAux = auxMax;
    return true;
}

/******************************************************************************
 * ledControl_setSimObject
 ******************************************************************************/
bool ledControl_setSimObjects(uint8_t output, uint8_t numObjects, uint8_t* pObject) {
    //prüfe ob ausgang gültig ist
    if (output >= LED_MAX_NUM_OUTPUTS) return false;

    for (uint8_t j=0; j<numObjects; j++) {
        //check if buffer is full
        uint8_t ri = ledHandle[output].simReadIndex;
        uint8_t wi = ledHandle[output].simWriteIndex;
        uint8_t ni = (wi + 1) % LED_NUM_SIM_OBJECTS;
        //wenn puffer voll ist abbrechen
        if (ni == ri) return false;
        //speichere objekt
        uint8_t* p = pObject + (j*LED_OBJECT_SIZE);
        for (uint8_t i=0; i<LED_OBJECT_SIZE; i++) ledHandle[output].simObjects[wi][i] = p[i];
        ledHandle[output].simWriteIndex = ni;
    }

    return true;
}

/******************************************************************************
 * ledControl_startSim
 ******************************************************************************/
void ledControl_startSim(uint32_t speedInfo, uint8_t dimInfo, bool useOffsetData, uint8_t* pDirectionData, uint8_t* pOffsetData) {

    ledControl_stopSequence();
    runMode = RUNMODE_SIMULATION;
    numOutputsReport = 0;

    uint32_t directionData = 0;
    if (useOffsetData) {
        directionData += pDirectionData[0] << 24;
        directionData += pDirectionData[1] << 16;
        directionData += pDirectionData[2] <<  8;
        directionData += pDirectionData[3];
    }

    for (uint8_t i=0; i<LED_MAX_NUM_OUTPUTS; i++) {
        uint8_t* p = ledControl_consumeSimObject(&ledHandle[i]);
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
                uint32_t mask = 1 << i;
                if (directionData & mask) ledHandle[i].outputMode = REVERSE;
                uint32_t offset = 0;
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
    for (uint8_t i=0; i<LED_MAX_NUM_OUTPUTS; i++) {
        ledHandle[i].simReadIndex = 0;
        ledHandle[i].simWriteIndex = 0;
    }
}

/******************************************************************************
 * ledControl_getNumSimObjects
 ******************************************************************************/
uint8_t ledControl_getNumSimObjects(ledHandle_t* h) {
    //prüfe wieviele simulationsobjekte vorhanden sind
    int8_t numObjects = h->simWriteIndex - h->simReadIndex;
    if (numObjects < 0) numObjects += LED_NUM_SIM_OBJECTS;
    return (uint8_t)numObjects;
}

/******************************************************************************
 * ledControl_getBufferInfo
 ******************************************************************************/
void ledControl_getBufferInfo(uint8_t* rsp) {
    rsp[0] = LED_RESPONSE_BUFFER_INFO;
    rsp[1] = LED_NUM_SIM_OBJECTS - 1;
    rsp[2] = LED_NUM_OBJECTS_PER_TRANSFER;
    rsp[3] = LED_MAX_NUM_OUTPUTS;
}

/******************************************************************************
 * ledControl_getBufferState
 ******************************************************************************/
void ledControl_getBufferState(uint8_t* rsp) {
    rsp[0] = LED_RESPONSE_BUFFER_STATE;
    for (uint8_t i=0; i<LED_MAX_NUM_OUTPUTS; i++) rsp[i+1] = ledControl_getNumSimObjects(&ledHandle[i]);
}

/******************************************************************************
 * ledControl_checkAutoSequence
 ******************************************************************************/
bool ledControl_checkAutoSequence(void) {
    if (numSequences <= 1) return false;
    if (!autoSelectionActive) return false;

    for (uint8_t i=0; i<LED_MAX_NUM_OUTPUTS; i++) if (ledHandle[i].lineEnd == 0) return false;

    uint8_t nextSequence = (currentSequence + 1) % numSequences;
    ledControl_startSequence(nextSequence);
    return true;
}


/******************************************************************************
 * ledControl_init
 ******************************************************************************/
void ledControl_init() {

    pwm_init();
    masterSlave_init();

    //initialisiere variabeln und überprüfe die konfiguration
    configBase = (uint8_t*)configBaseFlash;
    bool configOk = true;

    //prüfe ob Konfiguration CRC korrekt ist
    ledControl_configSize = ((uint32_t*)configBase)[0];    
    if (ledControl_configSize > configConfigSize) {
        TRACE("Invalid configuration size detected. Switching to backup configuration");
        configOk = false;
    }
    else {
        uint16_t crcCalc  = crc_calc(configBase, ledControl_configSize);
        uint16_t crcFlash = *((uint16_t*)(&configBase[ledControl_configSize]));
        if(crcCalc != crcFlash) {
            TRACE("Invalid configuration CRC detected. Switching to backup configuration");
            configOk = false;
        }
    }

    //prüfe Konfigurations inhalte
    numSequences = configBase[4];
    uint8_t configType    = configBase[6];
    uint8_t configVersion = configBase[7];
    if (configOk) {
        if ((numSequences == 0x00) || (numSequences == 0xFF)) {
            configOk = false;
            TRACE("Invalid amount of seuquences in configuration. Switching to backup configuration");
        }
        else if (configType != CONFIG_TYPE_XWLEDCONTROL) {
            TRACE("Invalid configuration type detected. Switching to backup configuration");
            configOk = false;
        }
        else if (configVersion != 1) {
            TRACE("Invalid configuration version detected. Switching to backup configuration");
            configOk = false;
        }
    }

    //batterie abschaltspannung steht im boarcConfig
    battWarningThreshold = boardConfig.batteryMinVoltage;

    //variabeln neu definitiv initialisieren
    if (!configOk) {
        ledControl_configSize = 0;
        configBase = (uint8_t*)configBaseBackup;
        numSequences = LED_SEQUENCE_NONE;
    }
    else numSequences = configBase[4];

    pSequenceTable = (uint32_t*)&configBase[0x08];
    sectionWidth = 200 / numSequences;
    batteryWarning = false;
    runMode = RUNMODE_MASTER;

    variableTimeIncrement = LED_DEFAULT_INCREMENT;
    ledControl_activate(true);
}
