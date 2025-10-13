/* ----------------------------------------------------------------------------
 * i2c_hal.h
 * - Simple I2C hardware abstraction layer for the RSL10 chip
 * - Works only as a master device, running at standard 100 kHz speed
 * - Does not depend on CMSIS or other complex drivers
 * ---------------------------------------------------------------------------- */

 #ifndef I2C_HAL_H
 #define I2C_HAL_H
 
 #ifdef __cplusplus
 extern "C"
 {
 #endif
 
 #include <stdint.h>
 #include <stdbool.h>
 #include <rsl10.h>
 
 /* ----------------------------------------------------------------------------
  * I2C Configuration
  * ---------------------------------------------------------------------------- */
 
 /* Defines two available I2C speeds:
  * - 100 kHz for standard mode
  * - 400 kHz for fast mode */
 #define I2C_SPEED_100KHZ        100000
 #define I2C_SPEED_400KHZ        400000
 
 /* Basic timeout value to prevent the program from hanging
  * if the bus gets stuck or the slave does not respond */
 #define I2C_TIMEOUT             100000
 
 /* ----------------------------------------------------------------------------
  * I2C Status Codes
  * ---------------------------------------------------------------------------- */
 
 /* Enumeration for function return values
  * Helps detect if the communication was successful or failed */
 typedef enum {
     I2C_STATUS_OK = 0,      /* Operation completed successfully */
     I2C_STATUS_ERROR,       /* General communication error */
     I2C_STATUS_TIMEOUT,     /* Timeout reached waiting for response */
     I2C_STATUS_NACK         /* Slave did not acknowledge a byte */
 } I2C_Status_t;
 
 /* ----------------------------------------------------------------------------
  * Function Prototypes
  * ---------------------------------------------------------------------------- */
 
 /* Initializes the I2C pins and timing according to the selected speed */
 void I2C_Init(uint32_t speed);
 
 /* Sends a block of data to a specific slave address */
 I2C_Status_t I2C_Write(uint8_t slave_addr, const uint8_t *data, uint16_t length);
 
 /* Reads a block of data from a specific slave address */
 I2C_Status_t I2C_Read(uint8_t slave_addr, uint8_t *data, uint16_t length);
 
 /* Combined write and read function:
  * sends a command to the slave, then reads back the response */
 I2C_Status_t I2C_WriteRead(uint8_t slave_addr, const uint8_t *tx_data,
                            uint16_t tx_length, uint8_t *rx_data, uint16_t rx_length);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* I2C_HAL_H */
 