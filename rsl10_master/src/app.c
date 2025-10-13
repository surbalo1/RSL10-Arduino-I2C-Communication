#include "app.h"
#include "i2c_hal.h"

/* Address of the Arduino slave device (0x55) */
#define ARDUINO_ADDR 0x55

/* ----------------------------------------------------------------------------
 * Initialize()
 * Sets up system clocks, GPIO, power domains, and I2C interface.
 * Also configures the on-board LED and prints startup information.
 * ---------------------------------------------------------------------------- */
void Initialize(void)
{
    /* Disable all interrupts during initialization */
    __set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_DISABLE_INTERRUPTS);

    Sys_NVIC_DisableAllInt();
    Sys_NVIC_ClearAllPendingInt();

    /* Configure recovery pin as input with pull-up */
    DIO->CFG[RECOVERY_DIO] = DIO_MODE_INPUT | DIO_WEAK_PULL_UP |
                             DIO_LPF_DISABLE | DIO_6X_DRIVE;

    /* Wait until recovery pin is released */
    while (DIO_DATA->ALIAS[RECOVERY_DIO] == 0);

    /* Enable and stabilize the RF power domain */
    ACS_VDDRF_CTRL->ENABLE_ALIAS = VDDRF_ENABLE_BITBAND;
    ACS_VDDRF_CTRL->CLAMP_ALIAS = VDDRF_DISABLE_HIZ_BITBAND;
    while (ACS_VDDRF_CTRL->READY_ALIAS != VDDRF_READY_BITBAND);

    /* Enable RF clock and access */
    SYSCTRL_RF_POWER_CFG->RF_POWER_ALIAS = RF_POWER_ENABLE_BITBAND;
    SYSCTRL_RF_ACCESS_CFG->RF_ACCESS_ALIAS = RF_ACCESS_ENABLE_BITBAND;

    /* Configure crystal oscillator and wait until ready */
    RF->XTAL_CTRL = ((RF->XTAL_CTRL & ~XTAL_CTRL_DISABLE_OSCILLATOR) |
                     XTAL_CTRL_REG_VALUE_SEL_INTERNAL);
    RF_REG2F->CK_DIV_1_6_CK_DIV_1_6_BYTE = CK_DIV_1_6_PRESCALE_1_BYTE;

    while (RF_REG39->ANALOG_INFO_CLK_DIG_READY_ALIAS !=
           ANALOG_INFO_CLK_DIG_READY_BITBAND);

    /* Set system clock to 48 MHz from RF clock */
    Sys_Clocks_SystemClkConfig(JTCK_PRESCALE_1 | EXTCLK_PRESCALE_1 | SYSCLK_CLKSRC_RFCLK);
    SystemCoreClock = SYSTEM_CLOCK_48MHZ;

    /* Configure LED pin as output (initially low) */
    Sys_GPIO_Set_Low(LED_DIO);
    Sys_DIO_Config(LED_DIO, DIO_MODE_GPIO_OUT_0);

    /* Initialize I2C interface at 100 kHz */
    I2C_Init(I2C_SPEED_100KHZ);

    /* Re-enable interrupts */
    __set_FAULTMASK(FAULTMASK_ENABLE_INTERRUPTS);
    __set_PRIMASK(PRIMASK_ENABLE_INTERRUPTS);

    /* Print startup banner */
    PRINTF("\r\n========================================\r\n");
    PRINTF("RSL10 <-> Arduino I2C Communication\r\n");
    PRINTF("========================================\r\n");
    PRINTF("Arduino at 0x%02X\r\n", ARDUINO_ADDR);
    PRINTF("========================================\r\n\r\n");
}

/* ----------------------------------------------------------------------------
 * main()
 * Main program loop:
 * - Sends a counter value to the Arduino
 * - Waits for a response (counter + 1)
 * - Tracks successful and failed transfers
 * - Toggles LED and prints communication status
 * ---------------------------------------------------------------------------- */
int main(void)
{
    /* Initialize RTT console and system */
    SEGGER_RTT_Init();
    Initialize();

    uint8_t counter = 0;
    uint8_t success_count = 0;
    uint8_t fail_count = 0;

    while (1)
    {
        PRINTF("[%03d] TX: 0x%02X ", counter, counter);

        /* Write counter value to Arduino */
        I2C_Status_t tx_status = I2C_Write(ARDUINO_ADDR, &counter, 1);

        if (tx_status == I2C_STATUS_OK)
        {
            PRINTF("✓ ");
            Sys_GPIO_Set_High(LED_DIO);

            /* Give Arduino time to process and reply */
            Sys_Delay_ProgramROM((uint32_t)(0.1 * SystemCoreClock));  // 100 ms delay

            /* Read response byte from Arduino */
            uint8_t response = 0;
            I2C_Status_t rx_status = I2C_Read(ARDUINO_ADDR, &response, 1);

            if (rx_status == I2C_STATUS_OK)
            {
                PRINTF("| RX: 0x%02X ", response);

                /* Expected response is counter + 1 */
                uint8_t expected = counter + 1;
                if (response == expected)
                {
                    PRINTF("✓");
                    success_count++;
                }
                else
                {
                    PRINTF("✗ (expected 0x%02X)", expected);
                    fail_count++;
                }
            }
            else
            {
                PRINTF("| RX: FAIL");
                fail_count++;
            }

            Sys_GPIO_Set_Low(LED_DIO);
        }
        else
        {
            PRINTF("✗ NACK");
            fail_count++;
        }

        PRINTF("\r\n");

        /* Print success/failure stats every 10 iterations */
        if ((counter % 10) == 9)
        {
            PRINTF("--- Stats: Success=%d, Fail=%d ---\r\n", success_count, fail_count);
        }

        counter++;

        /* Wait 500 ms before next transfer */
        Sys_Delay_ProgramROM((uint32_t)(0.5 * SystemCoreClock));
        Sys_Watchdog_Refresh();
    }

    return 0;
}
