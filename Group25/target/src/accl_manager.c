/*
 * Acceleration Manager
 * Michael Zhu
 * ENCE464 2024
 *
 * 
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "i2c_driver.h"
#include "circBufV.h"
#include "circBufF.h"
#include "accl_manager.h"
#include "accl_hal.h"


//********************************************************
// Constants and static vars
//********************************************************
#define BUF_SIZE 2 // WARNING: If this is set too high, we run out of heap space and the z-buffer gets garbled data

#define G_A_ZERO 1
#define G_A_ONE -1.979133761292768
#define G_A_TWO 0.979521463540373
#define G_B_ZERO 0.000086384997973502
#define G_B_ONE 0.000172769995947004
#define G_B_TWO 0.000086384997973502

#define LP_A_ZERO 1
#define LP_A_ONE -1.80898117793047
#define LP_A_TWO 0.827224480562408
#define LP_B_ZERO 0.095465967120306
#define LP_B_ONE -0.172688631608676 
#define LP_B_TWO 0.095465967120306

#define HP_A_ZERO 1
#define HP_A_ONE -1.905384612118461
#define HP_A_TWO 0.910092542787947
#define HP_B_ZERO 0.953986986993339 
#define HP_B_ONE -1.907503180919730 
#define HP_B_TWO 0.953986986993339 

static circBufVec_t acclBuffer;
static circBufVec_t gravityBuffer;
static circBufFloat_t amptitude;
static circBufFloat_t LPF_Buffer;
static circBufFloat_t HPF_Buffer;

vector3_t getAcclData(void);

void acclInit(void)
{
    initAcclChip(); // Init the chip over I2C
    initVecCircBuf(&acclBuffer, BUF_SIZE);
    initVecCircBuf(&gravityBuffer, BUF_SIZE);
    initFloatCircBuf(&amptitude, BUF_SIZE);
    initFloatCircBuf(&LPF_Buffer, BUF_SIZE);
    initFloatCircBuf(&HPF_Buffer, BUF_SIZE);
}

void acclProcess(void)
{
    vector3_t acceleration = getAcclData();
    writeVecCircBuf(&acclBuffer, acceleration);
}

float IIRFilter(void)
{
    vector3_t gravity;
    vector3_t nextVector = readVecCircBuf(&acclBuffer);
    vector3_t oldestVector = readVecCircBuf(&acclBuffer);
    vector3_t prevVector = readVecCircBuf(&acclBuffer);
    readVecCircBuf(&acclBuffer);

    vector3_t oldestGravity = readVecCircBuf(&gravityBuffer);
    vector3_t prevGravity = readVecCircBuf(&gravityBuffer);
    readVecCircBuf(&gravityBuffer);

    gravity.x = nextVector.x * G_B_ZERO + prevVector.x * G_B_ONE \
              + oldestVector.x * G_B_TWO - prevGravity.x * G_A_ONE \
              - oldestGravity.x * G_A_TWO;

    gravity.y = nextVector.y * G_B_ZERO + prevVector.y * G_B_ONE \
              + oldestVector.y * G_B_TWO - prevGravity.y * G_A_ONE \
              - oldestGravity.y * G_A_TWO;

    gravity.z = nextVector.z * G_B_ZERO + prevVector.z * G_B_ONE \
              + oldestVector.z * G_B_TWO - prevGravity.z * G_A_ONE \
              - oldestGravity.z * G_A_TWO;
    
    writeVecCircBuf(&gravityBuffer, gravity);

    float a_t = (nextVector.x - gravity.x) * gravity.x \
              + (nextVector.y - gravity.y) * gravity.y \
              + (nextVector.z - gravity.z) * gravity.z;

    writeFloatCircBuf(&amptitude, a_t);

    float prev_lp_in  = readFloatCircBuf(&amptitude);
    float oldest_lp_in = readFloatCircBuf(&amptitude);

    float oldest_lp_output = readFloatCircBuf(&LPF_Buffer);
    float prev_lp_output = readFloatCircBuf(&LPF_Buffer);
    readFloatCircBuf(&LPF_Buffer);

    float lp_output = a_t * LP_B_ZERO + prev_lp_in * LP_B_ONE + oldest_lp_in * LP_B_TWO - prev_lp_output * LP_A_ONE - oldest_lp_output * LP_A_TWO;
    writeFloatCircBuf(&LPF_Buffer, lp_output);

    float prev_hp_in  = readFloatCircBuf(&LPF_Buffer);
    float oldest_hp_in = readFloatCircBuf(&LPF_Buffer);

    float oldest_hp_output = readFloatCircBuf(&HPF_Buffer);
    float prev_hp_output = readFloatCircBuf(&HPF_Buffer);
    readFloatCircBuf(&HPF_Buffer);
    
    float hp_output = HP_A_ZERO * (lp_output * HP_B_ZERO + prev_hp_in * HP_B_ONE + oldest_hp_in * HP_B_TWO - prev_hp_output * HP_A_ONE - oldest_hp_output * HP_A_TWO);
    writeFloatCircBuf(&HPF_Buffer, hp_output);

    return hp_output;
}
