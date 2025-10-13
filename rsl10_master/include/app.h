/* include the core system and standard libraries */
#include <rsl10.h>
#include <rsl10_romvect.h>
#include <rsl10_map_nvr.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/* enable real-time terminal (rtt) support for debugging */
#include "SEGGER_RTT.h"

/* use rtt for formatted printing */
#define PRINTF(...) SEGGER_RTT_printf(0, __VA_ARGS__)

/* -------------------------------------------------------------------------
 * constant definitions
 * ------------------------------------------------------------------------- */

/* gpio pins */
#define LED_DIO                 6       /* on-board led pin */
#define RECOVERY_DIO            12      /* pin used for recovery mode */

/* i2c communication pins */
#define I2C_SDA_DIO             7       /* data line for i2c */
#define I2C_SCL_DIO             8       /* clock line for i2c */

/* system clock frequency */
#define SYSTEM_CLOCK_48MHZ      48000000

/* delay time in milliseconds between main loop actions */
#define MAIN_LOOP_DELAY_MS      1000

/* -------------------------------------------------------------------------
 * function declarations
 * ------------------------------------------------------------------------- */

/* sets up the system clock, gpio, and peripherals */
void Initialize(void);

/* prints a value in binary format for debugging */
void PrintBinary(uint8_t value);
