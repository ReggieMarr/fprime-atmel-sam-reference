#include <stdint.h>
#include "Driver_GPIO.h"
#include "Driver_I2C.h"
#include "Driver_USART.h"
#include "cmsis_os2.h"
#include "definitions.h"
#include "hal_defs.h"
#include "os_tick.h"
#include "peripheral/clk/plib_clk.h"
#include "pioConfig.h"
#include "sam.h"
#include "samv71_bsp_cfg.h"
#include "samv71q21b.h"

extern ARM_DRIVER_I2C Driver_I2C0;
static ARM_DRIVER_I2C* I2Cdrv = &Driver_I2C0;

extern ARM_DRIVER_USART Driver_USART0;
static ARM_DRIVER_USART* usartDrv = &Driver_USART0;

extern ARM_DRIVER_GPIO Driver_GPIO;
static ARM_DRIVER_GPIO* gpioDrv = &Driver_GPIO;

static const uint32_t LEDS[] = {PIN_PA23, PIN_PC9};

// Pre-Main startup bootstrap
void _on_bootstrap(void) {
    /* Initialize the SAM system */

    /* Enable ICache (CMSIS-Core API) */
    SCB_EnableICache();

    /* Enable DCache (CMSIS-Core API) */
    SCB_EnableDCache();

    /* Initialize Embedded Flash Controller */
    EFC_Initialize();

    /* Clear & Configure System IO Pins */
    MATRIX_REGS->CCFG_SYSIO = 0;
    MATRIX_REGS->CCFG_SYSIO |= (1 << 4);  // Enables PB4 as a regular GPIO instead of a system function

    // Disable peripheral clocks by default
    PMC_REGS->PMC_PCER0 = 0;

    /* Initialize PIO with pin configurations */
    // NOTE we now simply use the Driver_GPIO interface for this
    // however it may still be desired in other cases to set the port registers to
    // some default values
    /* initPio(gpioConfigs, 0); */

    /* Initialize Clocks */
    CLOCK_Initialize();

    /* Disable Watchdog Timers */
    RSWDT_REGS->RSWDT_MR = RSWDT_MR_WDDIS_Msk;  // Disable Reinforced Watchdog Timer
    WDT_REGS->WDT_MR = WDT_MR_WDDIS_Msk;        // Disable Standard Watchdog Timer

    /* Initialize Communication Peripherals */
    TWIHS0_Initialize();
    USART1_Initialize();

    /* Initialize Peripheral Drivers */
    I2Cdrv->Initialize(NULL);
    usartDrv->Initialize(NULL);

    /* Configure Nested Vector Interrupt Controller (NVIC) */
    NVIC_Initialize();

    /* System Clock Update */
    SystemCoreClockUpdate();
}

typedef struct {
    size_t idx;
    size_t tickDelay;
} blinkArg_t;

void setup_leds(void) {
    gpioDrv->Setup(LEDS[0], NULL);
    gpioDrv->SetDirection(LEDS[0], ARM_GPIO_OUTPUT);
    gpioDrv->SetOutputMode(LEDS[0], ARM_GPIO_PUSH_PULL);
    gpioDrv->SetEventTrigger(LEDS[0], ARM_GPIO_TRIGGER_NONE);
    gpioDrv->SetPullResistor(LEDS[0], ARM_GPIO_PULL_NONE);

    gpioDrv->Setup(LEDS[1], NULL);
    gpioDrv->SetDirection(LEDS[1], ARM_GPIO_OUTPUT);
    gpioDrv->SetOutputMode(LEDS[1], ARM_GPIO_PUSH_PULL);
    gpioDrv->SetEventTrigger(LEDS[1], ARM_GPIO_TRIGGER_NONE);
    gpioDrv->SetPullResistor(LEDS[1], ARM_GPIO_PULL_NONE);
}

void blink_leds(void* argument) {
    blinkArg_t* args = (blinkArg_t*)argument;
    size_t dfltTickDelay = 50000;

    // Set everything off
    gpioDrv->SetOutput(LEDS[0], GPIO_LOW);
    gpioDrv->SetOutput(LEDS[1], GPIO_LOW);
    osDelay(dfltTickDelay);

    // Set everything on
    gpioDrv->SetOutput(LEDS[0], GPIO_HIGH);
    gpioDrv->SetOutput(LEDS[1], GPIO_HIGH);

    osDelay(dfltTickDelay);

    gpioDrv->SetOutput(LEDS[0], GPIO_LOW);
    gpioDrv->SetOutput(LEDS[1], GPIO_HIGH);

    osDelay(dfltTickDelay);

    // Toggle LEDs
    while (true) {
        gpioDrv->SetOutput(LEDS[0], GPIO_HIGH);
        gpioDrv->SetOutput(LEDS[1], GPIO_LOW);
        osDelay(args->tickDelay);

        gpioDrv->SetOutput(LEDS[0], GPIO_LOW);
        gpioDrv->SetOutput(LEDS[1], GPIO_HIGH);
        osDelay(args->tickDelay);
    }
}

void setup_console(void) {
    int32_t status;

    // STEP 1: Signal start (both LEDs OFF)
    gpioDrv->SetOutput(LEDS[0], GPIO_LOW);
    gpioDrv->SetOutput(LEDS[1], GPIO_LOW);
    osDelay(500);

    // STEP 2: Initialize USART
    status = usartDrv->Initialize(NULL);
    if (status != ARM_DRIVER_OK) {
        // ERROR: Turn Both LEDs ON & halt (USART Init Failed)
        gpioDrv->SetOutput(LEDS[0], GPIO_HIGH);
        gpioDrv->SetOutput(LEDS[1], GPIO_HIGH);
        return;
    }
    // SUCCESS: Keep LED[0] ON
    gpioDrv->SetOutput(LEDS[0], GPIO_HIGH);
    osDelay(500);

    // STEP 3: Power on USART
    status = usartDrv->PowerControl(ARM_POWER_FULL);
    if (status != ARM_DRIVER_OK) {
        gpioDrv->SetOutput(LEDS[1], GPIO_HIGH);  // ERROR: Both LEDs ON ==> USART Power Fail
        return;
    }
    // SUCCESS: Keep LED[1] ON
    gpioDrv->SetOutput(LEDS[1], GPIO_HIGH);
    osDelay(500);

    // STEP 4: Configure USART (8N1, 115200 baud)
    status = usartDrv->Control(ARM_USART_MODE_ASYNCHRONOUS | ARM_USART_DATA_BITS_8 | ARM_USART_PARITY_NONE |
                                   ARM_USART_STOP_BITS_1 | ARM_USART_FLOW_CONTROL_NONE,
                               115200);
    if (status != ARM_DRIVER_OK) {
        // ERROR: Both LEDs ON (solid) ==> USART Config Fail
        gpioDrv->SetOutput(LEDS[0], GPIO_HIGH);
        gpioDrv->SetOutput(LEDS[1], GPIO_HIGH);
        return;
    }

    // SUCCESS: Toggle LEDs Fast (USART Config Successful)
    for (int i = 0; i < 5; i++) {
        /* gpioDrv->ToggleOutput(LEDS[0]); */
        /* gpioDrv->ToggleOutput(LEDS[1]); */
        osDelay(100);
    }

    // STEP 5: Send Test Data
    const char testMsg[] = "USART Test\n";
    status = usartDrv->Send(testMsg, sizeof(testMsg) - 1);
    if (status != ARM_DRIVER_OK) {
        // ERROR: Turn Both LEDs ON (solid) ==> USART Send Fail
        gpioDrv->SetOutput(LEDS[0], GPIO_HIGH);
        gpioDrv->SetOutput(LEDS[1], GPIO_HIGH);
        return;
    }

    // SUCCESS: Toggle LEDs Slowly (USART Sent Data Successfully)
    for (int i = 0; i < 5; i++) {
        /* gpioDrv->ToggleOutput(LEDS[0]); */
        /* gpioDrv->ToggleOutput(LEDS[1]); */
        osDelay(500);
    }

    // STEP 6: Indicate Ready State (Blink Normally)
    blinkArg_t blinkArgs = {.tickDelay = 1000};
    osThreadNew(blink_leds, &blinkArgs, NULL);
}

/* void run_console(void* argument) {} */

int main(void) {
    static blinkArg_t led0 = {.idx = 0, .tickDelay = 5000};
    static blinkArg_t led1 = {.idx = 1, .tickDelay = 10000};

    // Initialize LED's
    setup_leds();

    /* setup_console(); */

    // Initialize CMSIS-RTOS
    osKernelInitialize();
    // Create application main thread(s)
    osThreadNew(blink_leds, &led0, NULL);
    /* osThreadNew(blink_leds, &led1, NULL); */
    /* osThreadNew(run_console, NULL, NULL); */
    // Start thread execution
    osKernelStart();

    /* Wait forever here other wise the owned threads will die */
    for (;;) {
    }

    return 0;
}
