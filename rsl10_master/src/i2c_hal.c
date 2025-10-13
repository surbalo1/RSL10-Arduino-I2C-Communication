/* ----------------------------------------------------------------------------
 * i2c_hal.c - Software I2C implementation with fixed read timing
 * Provides simple bit-banged I2C master functions for the RSL10.
 * Designed for stable operation at low speeds and reliable communication.
 * ---------------------------------------------------------------------------- */

 #include "i2c_hal.h"

 /* I2C pin assignments */
 #define SDA_PIN  2   /* Data line */
 #define SCL_PIN  3   /* Clock line */
 
 /* Delay constant for timing control (higher = slower, more reliable) */
 #define I2C_DELAY  250
 
 /* ----------------------------------------------------------------------------
  * Helper delay function
  * Creates a small software delay to control I2C bit timing.
  * ---------------------------------------------------------------------------- */
 static void i2c_delay(void)
 {
     for(volatile uint32_t i = 0; i < I2C_DELAY; i++);
 }
 
 /* ----------------------------------------------------------------------------
  * SDA line control
  * These functions control or read the SDA pin manually.
  * ---------------------------------------------------------------------------- */
 static void sda_low(void)
 {
     Sys_DIO_Config(SDA_PIN, DIO_MODE_GPIO_OUT_0);
     Sys_GPIO_Set_Low(SDA_PIN);
 }
 
 static void sda_release(void)
 {
     Sys_DIO_Config(SDA_PIN, DIO_MODE_GPIO_IN_0);
 }
 
 static uint8_t sda_read(void)
 {
     Sys_DIO_Config(SDA_PIN, DIO_MODE_GPIO_IN_0);
     for(volatile int i = 0; i < 50; i++); /* short stabilization delay */
     return (DIO->DATA & (1 << SDA_PIN)) ? 1 : 0;
 }
 
 /* ----------------------------------------------------------------------------
  * SCL line control
  * ---------------------------------------------------------------------------- */
 static void scl_low(void)
 {
     Sys_DIO_Config(SCL_PIN, DIO_MODE_GPIO_OUT_0);
     Sys_GPIO_Set_Low(SCL_PIN);
 }
 
 static void scl_release(void)
 {
     Sys_DIO_Config(SCL_PIN, DIO_MODE_GPIO_IN_0);
 }
 
 /* ----------------------------------------------------------------------------
  * I2C start and stop conditions
  * ---------------------------------------------------------------------------- */
 static void i2c_start(void)
 {
     sda_release();
     i2c_delay();
     scl_release();
     i2c_delay();
     sda_low();
     i2c_delay();
     scl_low();
     i2c_delay();
 }
 
 static void i2c_stop(void)
 {
     sda_low();
     i2c_delay();
     scl_release();
     i2c_delay();
     sda_release();
     i2c_delay();
 }
 
 /* ----------------------------------------------------------------------------
  * Write one byte and read ACK/NACK from slave
  * Returns 1 if ACK received, 0 otherwise.
  * ---------------------------------------------------------------------------- */
 static uint8_t i2c_write_byte(uint8_t byte)
 {
     uint8_t ack;
 
     for(uint8_t bit = 0; bit < 8; bit++)
     {
         scl_low();
         i2c_delay();
 
         if (byte & 0x80)
             sda_release();
         else
             sda_low();
 
         i2c_delay();
         scl_release();
         i2c_delay();
 
         byte <<= 1;
     }
 
     /* Read ACK from slave */
     scl_low();
     i2c_delay();
     sda_release();
     i2c_delay();
     scl_release();
     i2c_delay();
     ack = sda_read();
     i2c_delay();
     scl_low();
     i2c_delay();
 
     return (ack == 0) ? 1 : 0;
 }
 
 /* ----------------------------------------------------------------------------
  * Read one byte from slave
  * Reads bits in the middle of SCL high period for stability.
  * Sends ACK or NACK depending on send_ack flag.
  * ---------------------------------------------------------------------------- */
 static uint8_t i2c_read_byte(uint8_t send_ack)
 {
     uint8_t byte = 0;
 
     sda_release();
 
     for(uint8_t bit = 0; bit < 8; bit++)
     {
         scl_low();
         i2c_delay();
         i2c_delay();  /* extra delay for reliability */
 
         scl_release();
         i2c_delay();
 
         /* sample data in middle of high period */
         byte <<= 1;
         if (sda_read())
             byte |= 1;
 
         i2c_delay();
     }
 
     /* send ACK or NACK */
     scl_low();
     i2c_delay();
 
     if (send_ack)
         sda_low();
     else
         sda_release();
 
     i2c_delay();
     scl_release();
     i2c_delay();
     i2c_delay();
     scl_low();
     i2c_delay();
 
     sda_release();
 
     return byte;
 }
 
 /* ----------------------------------------------------------------------------
  * I2C_Init
  * Configures SDA and SCL pins as open-drain inputs (idle high state).
  * ---------------------------------------------------------------------------- */
 void I2C_Init(uint32_t speed)
 {
     Sys_DIO_Config(SDA_PIN, DIO_MODE_GPIO_IN_0);
     Sys_DIO_Config(SCL_PIN, DIO_MODE_GPIO_IN_0);
 
     Sys_GPIO_Set_Low(SDA_PIN);
     Sys_GPIO_Set_Low(SCL_PIN);
 
     sda_release();
     scl_release();
 
     for(volatile int i = 0; i < 50000; i++);
 }
 
 /* ----------------------------------------------------------------------------
  * I2C_Write
  * Sends data buffer to slave address.
  * Returns status depending on ACK reception.
  * ---------------------------------------------------------------------------- */
 I2C_Status_t I2C_Write(uint8_t slave_addr, const uint8_t *data, uint16_t length)
 {
     i2c_start();
 
     if (!i2c_write_byte((slave_addr << 1) | 0))
     {
         i2c_stop();
         return I2C_STATUS_NACK;
     }
 
     for(uint16_t i = 0; i < length; i++)
     {
         if (!i2c_write_byte(data[i]))
         {
             i2c_stop();
             return I2C_STATUS_NACK;
         }
     }
 
     i2c_stop();
     return I2C_STATUS_OK;
 }
 
 /* ----------------------------------------------------------------------------
  * I2C_Read
  * Reads a buffer from slave address.
  * ---------------------------------------------------------------------------- */
 I2C_Status_t I2C_Read(uint8_t slave_addr, uint8_t *data, uint16_t length)
 {
     i2c_start();
 
     if (!i2c_write_byte((slave_addr << 1) | 1))
     {
         i2c_stop();
         return I2C_STATUS_NACK;
     }
 
     for(uint16_t i = 0; i < length; i++)
     {
         uint8_t send_ack = (i < length - 1);
         data[i] = i2c_read_byte(send_ack);
     }
 
     i2c_stop();
     return I2C_STATUS_OK;
 }
 
 /* ----------------------------------------------------------------------------
  * I2C_WriteRead
  * Combines write followed by read sequence with small delay in between.
  * ---------------------------------------------------------------------------- */
 I2C_Status_t I2C_WriteRead(uint8_t slave_addr, const uint8_t *tx_data,
                            uint16_t tx_length, uint8_t *rx_data, uint16_t rx_length)
 {
     I2C_Status_t status;
 
     status = I2C_Write(slave_addr, tx_data, tx_length);
     if (status != I2C_STATUS_OK)
         return status;
 
     for(volatile uint32_t i = 0; i < 50000; i++);
 
     status = I2C_Read(slave_addr, rx_data, rx_length);
     return status;
 }
 