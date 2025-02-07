#include <stdint.h>
#include "Driver_Common.h"
#include "Driver_GPIO.h"
#include "Driver_I2C.h"
#include "Driver_USART.h"
#include "FreeRTOS.h"
#include "cmsis_compiler.h"
#include "cmsis_os2.h"
#include "definitions.h"
#include "freertos_evr.h"
#include "hal_defs.h"
#include "os_tick.h"
#include "peripheral/clk/plib_clk.h"
#include "pioConfig.h"
#include "portmacro.h"
#include "sam.h"
#include "samv71_bsp_cfg.h"
#include "samv71q21b.h"

/* extern ARM_DRIVER_I2C Driver_I2C0; */
/* static ARM_DRIVER_I2C* I2Cdrv = &Driver_I2C0; */

extern ARM_DRIVER_USART Driver_USART1;
static ARM_DRIVER_USART* usartDrv = &Driver_USART1;

extern ARM_DRIVER_GPIO Driver_GPIO;
static ARM_DRIVER_GPIO* gpioDrv = &Driver_GPIO;

static const uint32_t LEDS[] = {PIN_PA23, PIN_PC9};
static const uint32_t SWITCHES[] = {PIN_PA9, PIN_PB12};
static const uint32_t VBUS_HOST_EN = PIN_PC16;

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

    /* Configure Nested Vector Interrupt Controller (NVIC) */
    NVIC_Initialize();

    /* System Clock Update */
    SystemCoreClockUpdate();
}

typedef struct {
    size_t idx;
    size_t tickDelay;
} blinkArg_t;

static StaticTask_t led_thread_cb;

static StackType_t led_thread_stack[256];

static const osThreadAttr_t led_thread_cfg = {
    .cb_mem = &led_thread_cb,
    .cb_size = sizeof(led_thread_cb),
    .stack_mem = &led_thread_stack[0],
    .stack_size = sizeof(led_thread_stack),
};

static osSemaphoreId_t readyToBlink;

void setup_gpios(void) {
    int32_t sts;

    sts = gpioDrv->Setup(LEDS[0], NULL);
    CHECK(sts == ARM_DRIVER_OK, return);
    sts = gpioDrv->SetDirection(LEDS[0], ARM_GPIO_OUTPUT);
    CHECK(sts == ARM_DRIVER_OK, return);
    sts = gpioDrv->SetOutputMode(LEDS[0], ARM_GPIO_PUSH_PULL);
    CHECK(sts == ARM_DRIVER_OK, return);
    sts = gpioDrv->SetEventTrigger(LEDS[0], ARM_GPIO_TRIGGER_NONE);
    CHECK(sts == ARM_DRIVER_OK, return);
    sts = gpioDrv->SetPullResistor(LEDS[0], ARM_GPIO_PULL_NONE);
    CHECK(sts == ARM_DRIVER_OK, return);

    sts = gpioDrv->Setup(LEDS[1], NULL);
    CHECK(sts == ARM_DRIVER_OK, return);
    sts = gpioDrv->SetDirection(LEDS[1], ARM_GPIO_OUTPUT);
    CHECK(sts == ARM_DRIVER_OK, return);
    sts = gpioDrv->SetOutputMode(LEDS[1], ARM_GPIO_PUSH_PULL);
    CHECK(sts == ARM_DRIVER_OK, return);
    sts = gpioDrv->SetEventTrigger(LEDS[1], ARM_GPIO_TRIGGER_NONE);
    CHECK(sts == ARM_DRIVER_OK, return);
    sts = gpioDrv->SetPullResistor(LEDS[1], ARM_GPIO_PULL_NONE);
    CHECK(sts == ARM_DRIVER_OK, return);

    sts = gpioDrv->Setup(VBUS_HOST_EN, NULL);
    CHECK(sts == ARM_DRIVER_OK, return);
    sts = gpioDrv->SetDirection(VBUS_HOST_EN, ARM_GPIO_OUTPUT);
    CHECK(sts == ARM_DRIVER_OK, return);
    sts = gpioDrv->SetOutputMode(VBUS_HOST_EN, ARM_GPIO_PUSH_PULL);
    CHECK(sts == ARM_DRIVER_OK, return);
    sts = gpioDrv->SetEventTrigger(VBUS_HOST_EN, ARM_GPIO_TRIGGER_NONE);
    CHECK(sts == ARM_DRIVER_OK, return);
    sts = gpioDrv->SetPullResistor(VBUS_HOST_EN, ARM_GPIO_PULL_NONE);
    CHECK(sts == ARM_DRIVER_OK, return);
}

void toggle_oscillating_leds(int cycleCnt, int delay) {
    for (int i = 0; i < cycleCnt; i++) {
        gpioDrv->SetOutput(LEDS[0], GPIO_HIGH);
        gpioDrv->SetOutput(LEDS[1], GPIO_LOW);
        osDelay(delay);

        gpioDrv->SetOutput(LEDS[0], GPIO_LOW);
        gpioDrv->SetOutput(LEDS[1], GPIO_HIGH);
        osDelay(delay);
    }
}

void toggle_combined_leds(int cycleCnt, int delay) {
    for (int i = 0; i < cycleCnt; i++) {
        gpioDrv->SetOutput(LEDS[0], GPIO_HIGH);
        gpioDrv->SetOutput(LEDS[1], GPIO_HIGH);
        osDelay(delay);

        gpioDrv->SetOutput(LEDS[0], GPIO_LOW);
        gpioDrv->SetOutput(LEDS[1], GPIO_LOW);
        osDelay(delay);
    }
}

__NO_RETURN void blink_leds(void* argument) {
    blinkArg_t* args = (blinkArg_t*)argument;
    osSemaphoreAcquire(readyToBlink, osWaitForever);

    toggle_combined_leds(3, args->tickDelay * 3);

    // Toggle LEDs
    while (true) {
        toggle_oscillating_leds(10, args->tickDelay);

        gpioDrv->SetOutput(LEDS[0], GPIO_LOW);
        gpioDrv->SetOutput(LEDS[1], GPIO_LOW);
        osDelay(args->tickDelay * 10);
    }
}

static StaticTask_t console_thread_cb;

static StackType_t console_thread_stack[256];

static const osThreadAttr_t console_thread_cfg = {
    .cb_mem = &console_thread_cb,
    .cb_size = sizeof(console_thread_cb),
    .stack_mem = &console_thread_stack[0],
    .stack_size = sizeof(console_thread_stack),
};

__NO_RETURN void run_console(void* argument) {
    volatile int32_t status;
    uint32_t shortDelay = 5000;
    uint32_t longDelay = 10000;

    toggle_combined_leds(3, 5000);

    // STEP 1: Signal start (both LEDs OFF)
    gpioDrv->SetOutput(LEDS[0], GPIO_LOW);
    gpioDrv->SetOutput(LEDS[1], GPIO_LOW);
    osDelay(shortDelay);

    // STEP 2: Initialize USART
    status = usartDrv->Initialize(NULL);
    CHECK(status == ARM_DRIVER_OK, {
        // ERROR: Turn Both LEDs ON & halt (USART Init Failed)
        gpioDrv->SetOutput(LEDS[0], GPIO_HIGH);
        gpioDrv->SetOutput(LEDS[1], GPIO_HIGH);
    });
    // SUCCESS: Keep LED[0] ON
    gpioDrv->SetOutput(LEDS[0], GPIO_HIGH);
    osDelay(shortDelay);

    // STEP 3: Power on USART
    status = usartDrv->PowerControl(ARM_POWER_FULL);
    CHECK(status == ARM_DRIVER_OK, {
        gpioDrv->SetOutput(LEDS[1], GPIO_HIGH);  // ERROR: Both LEDs ON ==> USART Power Fail
    });
    // SUCCESS: Keep LED[1] ON
    gpioDrv->SetOutput(LEDS[1], GPIO_HIGH);
    osDelay(shortDelay);

    // STEP 4: Configure USART (8N1, 115200 baud)
    status = usartDrv->Control(ARM_USART_MODE_ASYNCHRONOUS | ARM_USART_DATA_BITS_8 | ARM_USART_PARITY_NONE |
                                   ARM_USART_STOP_BITS_1 | ARM_USART_FLOW_CONTROL_NONE,
                               115200);
    CHECK(status == ARM_DRIVER_OK, {
        // ERROR: Both LEDs ON (solid) ==> USART Config Fail
        gpioDrv->SetOutput(LEDS[0], GPIO_HIGH);
        gpioDrv->SetOutput(LEDS[1], GPIO_HIGH);
    });

    // SUCCESS: Toggle LEDs Fast (USART Config Successful)
    toggle_combined_leds(5, shortDelay);

    gpioDrv->SetOutput(LEDS[0], GPIO_LOW);
    gpioDrv->SetOutput(LEDS[1], GPIO_LOW);
    osDelay(longDelay * 2);

    // STEP 5: Send Test Data
    const char testMsg[] = "USART Test\n";
    status = usartDrv->Send(testMsg, sizeof(testMsg) - 1);
    CHECK(status == ARM_DRIVER_OK, {
        // ERROR: Turn Both LEDs ON (solid) ==> USART Send Fail
        gpioDrv->SetOutput(LEDS[0], GPIO_HIGH);
        gpioDrv->SetOutput(LEDS[1], GPIO_HIGH);
    });

    // SUCCESS: Toggle LEDs Slowly (USART Sent Data Successfully)
    toggle_oscillating_leds(5, longDelay);
    gpioDrv->SetOutput(LEDS[0], GPIO_LOW);
    gpioDrv->SetOutput(LEDS[1], GPIO_LOW);
    osDelay(longDelay * 3);

    // STEP 6: Indicate Ready State
    (void)osSemaphoreRelease(readyToBlink);

    osThreadExit();
}

int main(void) {
    // NOTE I'm not entirely sure why this needs to be static but seeminly if its not the
    // thread doesn't retain it (could be something about being the the main launch)
    static blinkArg_t blinkArgs = {.tickDelay = 1000};

    // Initialize LED's
    setup_gpios();

    // Initialize CMSIS-RTOS
    osStatus_t osSts;
    osSts = osKernelInitialize();
    CHECK(osSts == osOK, return ARM_DRIVER_ERROR);

    readyToBlink = osSemaphoreNew(1, 0, NULL);
    CHECK(readyToBlink, return ARM_DRIVER_ERROR);

    // Create application main thread(s)
    osThreadId_t tId;
    tId = osThreadNew(blink_leds, &blinkArgs, &led_thread_cfg);
    CHECK(tId, return -1);

    tId = osThreadNew(run_console, NULL, &console_thread_cfg);
    CHECK(tId, return ARM_DRIVER_ERROR);

    // Start thread execution
    osSts = osKernelStart();
    CHECK(osSts == osOK, return ARM_DRIVER_ERROR);

    /* Wait forever here other wise the owned threads will die */
    for (;;) {
        __NOP();
    }

    return 0;
}
