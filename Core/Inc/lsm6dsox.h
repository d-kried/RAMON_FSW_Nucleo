#ifndef LSM6DSOX_H
#define LSM6DSOX_H

#include "stm32g0xx_hal.h"

#define LSM6DSOX_ADDR         (0x6A << 1) // Adjust if SA0 is high
#define LSM6DSOX_WHO_AM_I_REG 0x0F

// Control registers
#define LSM6DSOX_CTRL1_XL     0x10
#define LSM6DSOX_CTRL2_G      0x11

// Accelerometer output registers
#define LSM6DSOX_OUTX_L_A     0x28
#define LSM6DSOX_OUTX_H_A     0x29
#define LSM6DSOX_OUTY_L_A     0x2A
#define LSM6DSOX_OUTY_H_A     0x2B
#define LSM6DSOX_OUTZ_L_A     0x2C
#define LSM6DSOX_OUTZ_H_A     0x2D

// Gyroscope output registers
#define LSM6DSOX_OUTX_L_G     0x22
#define LSM6DSOX_OUTX_H_G     0x23
#define LSM6DSOX_OUTY_L_G     0x24
#define LSM6DSOX_OUTY_H_G     0x25
#define LSM6DSOX_OUTZ_L_G     0x26
#define LSM6DSOX_OUTZ_H_G     0x27

HAL_StatusTypeDef LSM6DSOX_ReadWHOAMI(I2C_HandleTypeDef *hi2c, uint8_t *who_am_i);
HAL_StatusTypeDef LSM6DSOX_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef LSM6DSOX_ReadAcc(I2C_HandleTypeDef *hi2c, int16_t *accelData);
HAL_StatusTypeDef LSM6DSOX_ReadGyroZ(I2C_HandleTypeDef *hi2c, int16_t *gyroZ);
HAL_StatusTypeDef LSM6DSOX_ReadGyroX(I2C_HandleTypeDef *hi2c, int16_t *gyroX);
HAL_StatusTypeDef LSM6DSOX_ReadGyroY(I2C_HandleTypeDef *hi2c, int16_t *gyroY);

HAL_StatusTypeDef LSM6DSOX_ReadReg(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t *data);

#endif // LSM6DSOX_H
