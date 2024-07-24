#include "lsm6dsox.h"

HAL_StatusTypeDef LSM6DSOX_ReadWHOAMI(I2C_HandleTypeDef *hi2c, uint8_t *who_am_i)
{
    return HAL_I2C_Mem_Read(hi2c, LSM6DSOX_ADDR, LSM6DSOX_WHO_AM_I_REG, I2C_MEMADD_SIZE_8BIT, who_am_i, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef LSM6DSOX_Init(I2C_HandleTypeDef *hi2c)
{
    uint8_t ctrl1_xl = 0x60; // ODR = 416 Hz, FS = ±2g
    uint8_t ctrl2_g = 0x60;  // ODR = 416 Hz, FS = 2000 dps

    if (HAL_I2C_Mem_Write(hi2c, LSM6DSOX_ADDR, LSM6DSOX_CTRL1_XL, I2C_MEMADD_SIZE_8BIT, &ctrl1_xl, 1, HAL_MAX_DELAY) != HAL_OK)
    {
        return HAL_ERROR;
    }
    if (HAL_I2C_Mem_Write(hi2c, LSM6DSOX_ADDR, LSM6DSOX_CTRL2_G, I2C_MEMADD_SIZE_8BIT, &ctrl2_g, 1, HAL_MAX_DELAY) != HAL_OK)
    {
        return HAL_ERROR;
    }
    return HAL_OK;
}

HAL_StatusTypeDef LSM6DSOX_ReadGyroX(I2C_HandleTypeDef *hi2c, int16_t *gyroX)
{
    uint8_t buffer[2];
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Read(hi2c, LSM6DSOX_ADDR, LSM6DSOX_OUTX_L_G, I2C_MEMADD_SIZE_8BIT, buffer, 2, HAL_MAX_DELAY);
    if (ret == HAL_OK)
    {
        *gyroX = (int16_t)((buffer[1] << 8) | buffer[0]); // X-axis
    }
    return ret;
}
HAL_StatusTypeDef LSM6DSOX_ReadGyroY(I2C_HandleTypeDef *hi2c, int16_t *gyroY)
{
    uint8_t buffer[2];
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Read(hi2c, LSM6DSOX_ADDR, LSM6DSOX_OUTY_L_G, I2C_MEMADD_SIZE_8BIT, buffer, 2, HAL_MAX_DELAY);
    if (ret == HAL_OK)
    {
        *gyroY = (int16_t)((buffer[1] << 8) | buffer[0]);
    }
    return ret;
}
HAL_StatusTypeDef LSM6DSOX_ReadGyroZ(I2C_HandleTypeDef *hi2c, int16_t *gyroZ)
{
    uint8_t buffer[2];
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Read(hi2c, LSM6DSOX_ADDR, LSM6DSOX_OUTX_L_G + 4, I2C_MEMADD_SIZE_8BIT, buffer, 2, HAL_MAX_DELAY);
    if (ret == HAL_OK)
    {
        *gyroZ = (int16_t)((buffer[1] << 8) | buffer[0]); // Combine high and low bytes
    }
    return ret;
}
