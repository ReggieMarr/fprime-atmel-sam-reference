#include "sam.h"
#include "device_vectors.h"
#include "startup_samv71q21b.h"
#include "FreeRTOS.h"
#include "interrupts.h"

/* Default empty handler */
void __attribute__((weak, used, optimize("-O1"), long_call, externally_visible)) Dummy_Handler(void);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"

/* Reset handler and application-provided reset handler */
void __attribute__((weak, optimize("-O1"), long_call, naked, externally_visible)) Reset_Handler(void);
extern void __attribute__((weak, used, long_call)) __XC32_RESET_HANDLER_NAME(void);

#pragma GCC diagnostic pop

/* /\* Cortex-M7 core handlers *\/ */
/* void NonMaskableInt_Handler ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void HardFault_Handler    ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void MemoryManagement_Handler ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void BusFault_Handler     ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void UsageFault_Handler   ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void SVCall_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void DebugMonitor_Handler ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void PendSV_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void SysTick_Handler      ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */

/* /\* Peripherals handlers *\/ */
/* void SUPC_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void RSTC_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void RTC_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void RTT_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void WDT_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void PMC_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void EFC_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void UART0_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void UART1_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void Reserved9_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void PIOA_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void PIOB_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void Reserved12_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void USART0_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void USART1_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void Reserved15_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void PIOD_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void Reserved17_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void Reserved18_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TWIHS0_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TWIHS1_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void Reserved21_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void SSC_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TC0_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TC0_CH0_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TC1_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TC0_CH1_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TC2_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TC0_CH2_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TC3_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TC1_CH0_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TC4_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TC1_CH1_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TC5_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TC1_CH2_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void AFEC0_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void DACC_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void PWM0_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void ICM_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void ACC_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void USBHS_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void MCAN0_INT0_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void MCAN0_INT1_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void Reserved37_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void Reserved38_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void GMAC_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void AFEC1_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void Reserved41_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void Reserved42_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void QSPI_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void UART2_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void Reserved45_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void Reserved46_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TC6_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TC2_CH0_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TC7_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TC2_CH1_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TC8_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TC2_CH2_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TC9_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TC3_CH0_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TC10_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TC3_CH1_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TC11_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TC3_CH2_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void MLB_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void Reserved54_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void Reserved55_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void AES_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void TRNG_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void XDMAC_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void ISI_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void PWM1_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void FPU_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void Reserved62_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void RSWDT_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void CCW_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void CCF_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void GMAC_Q1_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void GMAC_Q2_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void IXC_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void Reserved69_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void Reserved70_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void GMAC_Q3_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void GMAC_Q4_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */
/* void GMAC_Q5_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler"))); */

/* #pragma GCC diagnostic push */
/* #pragma GCC diagnostic ignored "-Wpedantic" */

/* /\* Exception Table *\/ */
/* __attribute__ ((section(".vectors.default"), weak, externally_visible)) */
/* const DeviceVectors exception_table= */
/* { */
/*         /\* Configurable interrupts with MCC names *\/ */
/*         /\* .pfnSUPC_Handler         = *\/ (void*) SUPC_Handler,        /\* 0  Supply Controller *\/ */
/*         /\* .pfnRSTC_Handler         = *\/ (void*) RSTC_Handler,        /\* 1  Reset Controller *\/ */
/*         /\* .pfnRTC_Handler          = *\/ (void*) RTC_Handler,         /\* 2  Real-time Clock *\/ */
/*         /\* .pfnRTT_Handler          = *\/ (void*) RTT_Handler,         /\* 3  Real-time Timer *\/ */
/*         /\* .pfnWDT_Handler          = *\/ (void*) WDT_Handler,         /\* 4  Watchdog Timer *\/ */
/*         /\* .pfnPMC_Handler          = *\/ (void*) PMC_Handler,         /\* 5  Power Management Controller *\/ */
/*         /\* .pfnEFC_Handler          = *\/ (void*) EFC_Handler,         /\* 6  Embedded Flash Controller *\/ */
/*         /\* .pfnUART0_Handler        = *\/ (void*) UART0_Handler,       /\* 7  Universal Asynchronous Receiver Transmitter *\/ */
/*         /\* .pfnUART1_Handler        = *\/ (void*) UART1_Handler,       /\* 8  Universal Asynchronous Receiver Transmitter *\/ */
/*         /\* .pvReserved9             = *\/ (void*) (0UL),               /\* 9  Reserved *\/ */
/*         /\* .pfnPIOA_Handler         = *\/ (void*) PIOA_Handler,        /\* 10 Parallel Input/Output Controller *\/ */
/*         /\* .pfnPIOB_Handler         = *\/ (void*) PIOB_Handler,        /\* 11 Parallel Input/Output Controller *\/ */
/*         /\* .pvReserved12            = *\/ (void*) (0UL),               /\* 12 Reserved *\/ */
/*         /\* .pfnUSART0_Handler       = *\/ (void*) USART0_Handler,      /\* 13 Universal Synchronous Asynchronous Receiver Transmitter *\/ */
/*         /\* .pfnUSART1_Handler       = *\/ (void*) USART1_Handler,      /\* 14 Universal Synchronous Asynchronous Receiver Transmitter *\/ */
/*         /\* .pvReserved15            = *\/ (void*) (0UL),               /\* 15 Reserved *\/ */
/*         /\* .pfnPIOD_Handler         = *\/ (void*) PIOD_Handler,        /\* 16 Parallel Input/Output Controller *\/ */
/*         /\* .pvReserved17            = *\/ (void*) (0UL),               /\* 17 Reserved *\/ */
/*         /\* .pvReserved18            = *\/ (void*) (0UL),               /\* 18 Reserved *\/ */
/*         /\* .pfnTWIHS0_Handler       = *\/ (void*) TWIHS0_Handler,      /\* 19 Two-wire Interface High Speed *\/ */
/*         /\* .pfnTWIHS1_Handler       = *\/ (void*) TWIHS1_Handler,      /\* 20 Two-wire Interface High Speed *\/ */
/*         /\* .pvReserved21            = *\/ (void*) (0UL),               /\* 21 Reserved *\/ */
/*         /\* .pfnSSC_Handler          = *\/ (void*) SSC_Handler,         /\* 22 Synchronous Serial Controller *\/ */
/*         /\* .pfnTC0_CH0_Handler      = *\/ (void*) TC0_CH0_Handler,     /\* 23 Timer/Counter 0 Channel 0 *\/ */
/*         /\* .pfnTC0_CH1_Handler      = *\/ (void*) TC0_CH1_Handler,     /\* 24 Timer/Counter 0 Channel 1 *\/ */
/*         /\* .pfnTC0_CH2_Handler      = *\/ (void*) TC0_CH2_Handler,     /\* 25 Timer/Counter 0 Channel 2 *\/ */
/*         /\* .pfnTC1_CH0_Handler      = *\/ (void*) TC1_CH0_Handler,     /\* 26 Timer/Counter 1 Channel 0 *\/ */
/*         /\* .pfnTC1_CH1_Handler      = *\/ (void*) TC1_CH1_Handler,     /\* 27 Timer/Counter 1 Channel 1 *\/ */
/*         /\* .pfnTC1_CH2_Handler      = *\/ (void*) TC1_CH2_Handler,     /\* 28 Timer/Counter 1 Channel 2 *\/ */
/*         /\* .pfnAFEC0_Handler        = *\/ (void*) AFEC0_Handler,       /\* 29 Analog Front-End Controller *\/ */
/*         /\* .pfnDACC_Handler         = *\/ (void*) DACC_Handler,        /\* 30 Digital-to-Analog Converter Controller *\/ */
/*         /\* .pfnPWM0_Handler         = *\/ (void*) PWM0_Handler,        /\* 31 Pulse Width Modulation Controller *\/ */
/*         /\* .pfnICM_Handler          = *\/ (void*) ICM_Handler,         /\* 32 Integrity Check Monitor *\/ */
/*         /\* .pfnACC_Handler          = *\/ (void*) ACC_Handler,         /\* 33 Analog Comparator Controller *\/ */
/*         /\* .pfnUSBHS_Handler        = *\/ (void*) USBHS_Handler,       /\* 34 USB High-Speed Interface *\/ */
/*         /\* .pfnMCAN0_INT0_Handler   = *\/ (void*) MCAN0_INT0_Handler,  /\* 35 Controller Area Network *\/ */
/*         /\* .pfnMCAN0_INT1_Handler   = *\/ (void*) MCAN0_INT1_Handler,  /\* 36 Controller Area Network *\/ */
/*         /\* .pvReserved37            = *\/ (void*) (0UL),               /\* 37 Reserved *\/ */
/*         /\* .pvReserved38            = *\/ (void*) (0UL),               /\* 38 Reserved *\/ */
/*         /\* .pfnGMAC_Handler         = *\/ (void*) GMAC_Handler,        /\* 39 Gigabit Ethernet MAC *\/ */
/*         /\* .pfnAFEC1_Handler        = *\/ (void*) AFEC1_Handler,       /\* 40 Analog Front-End Controller *\/ */
/*         /\* .pvReserved41            = *\/ (void*) (0UL),               /\* 41 Reserved *\/ */
/*         /\* .pvReserved42            = *\/ (void*) (0UL),               /\* 42 Reserved *\/ */
/*         /\* .pfnQSPI_Handler         = *\/ (void*) QSPI_Handler,        /\* 43 Quad Serial Peripheral Interface *\/ */
/*         /\* .pfnUART2_Handler        = *\/ (void*) UART2_Handler,       /\* 44 Universal Asynchronous Receiver Transmitter *\/ */
/*         /\* .pvReserved45            = *\/ (void*) (0UL),               /\* 45 Reserved *\/ */
/*         /\* .pvReserved46            = *\/ (void*) (0UL),               /\* 46 Reserved *\/ */
/*         /\* .pfnTC2_CH0_Handler      = *\/ (void*) TC2_CH0_Handler,     /\* 47 Timer/Counter 2 Channel 0 *\/ */
/*         /\* .pfnTC2_CH1_Handler      = *\/ (void*) TC2_CH1_Handler,     /\* 48 Timer/Counter 2 Channel 1 *\/ */
/*         /\* .pfnTC2_CH2_Handler      = *\/ (void*) TC2_CH2_Handler,     /\* 49 Timer/Counter 2 Channel 2 *\/ */
/*         /\* .pfnTC3_CH0_Handler      = *\/ (void*) TC3_CH0_Handler,     /\* 50 Timer/Counter 3 Channel 0 *\/ */
/*         /\* .pfnTC3_CH1_Handler      = *\/ (void*) TC3_CH1_Handler,     /\* 51 Timer/Counter 3 Channel 1 *\/ */
/*         /\* .pfnTC3_CH2_Handler      = *\/ (void*) TC3_CH2_Handler,     /\* 52 Timer/Counter 3 Channel 2 *\/ */
/*         /\* .pfnMLB_Handler          = *\/ (void*) MLB_Handler,         /\* 53 MediaLB *\/ */
/*         /\* .pvReserved54            = *\/ (void*) (0UL),               /\* 54 Reserved *\/ */
/*         /\* .pvReserved55            = *\/ (void*) (0UL),               /\* 55 Reserved *\/ */
/*         /\* .pfnAES_Handler          = *\/ (void*) AES_Handler,         /\* 56 Advanced Encryption Standard *\/ */
/*         /\* .pfnTRNG_Handler         = *\/ (void*) TRNG_Handler,        /\* 57 True Random Number Generator *\/ */
/*         /\* .pfnXDMAC_Handler        = *\/ (void*) XDMAC_Handler,       /\* 58 Extensible DMA Controller *\/ */
/*         /\* .pfnISI_Handler          = *\/ (void*) ISI_Handler,         /\* 59 Image Sensor Interface *\/ */
/*         /\* .pfnPWM1_Handler         = *\/ (void*) PWM1_Handler,        /\* 60 Pulse Width Modulation Controller *\/ */
/*         /\* .pfnFPU_Handler          = *\/ (void*) FPU_Handler,         /\* 61 Floating Point Unit *\/ */
/*         /\* .pvReserved62            = *\/ (void*) (0UL),               /\* 62 Reserved *\/ */
/*         /\* .pfnRSWDT_Handler        = *\/ (void*) RSWDT_Handler,       /\* 63 Reinforced Safety Watchdog Timer *\/ */
/*         /\* .pfnCCW_Handler          = *\/ (void*) CCW_Handler,         /\* 64 System Control Block *\/ */
/*         /\* .pfnCCF_Handler          = *\/ (void*) CCF_Handler,         /\* 65 System Control Block *\/ */
/*         /\* .pfnGMAC_Q1_Handler      = *\/ (void*) GMAC_Q1_Handler,     /\* 66 Gigabit Ethernet MAC *\/ */
/*         /\* .pfnGMAC_Q2_Handler      = *\/ (void*) GMAC_Q2_Handler,     /\* 67 Gigabit Ethernet MAC *\/ */
/*         /\* .pfnIXC_Handler          = *\/ (void*) IXC_Handler,         /\* 68 Floating Point Unit *\/ */
/*         /\* .pvReserved69            = *\/ (void*) (0UL),               /\* 69 Reserved *\/ */
/*         /\* .pvReserved70            = *\/ (void*) (0UL),               /\* 70 Reserved *\/ */
/*         /\* .pfnGMAC_Q3_Handler      = *\/ (void*) GMAC_Q3_Handler,     /\* 71 Gigabit Ethernet MAC *\/ */
/*         /\* .pfnGMAC_Q4_Handler      = *\/ (void*) GMAC_Q4_Handler,     /\* 72 Gigabit Ethernet MAC *\/ */
/*         /\* .pfnGMAC_Q5_Handler      = *\/ (void*) GMAC_Q5_Handler      /\* 73 Gigabit Ethernet MAC *\/ */

/* }; */

/* MISRAC 2012 deviation block start */
/* MISRA C-2012 Rule 8.6 deviated 70 times.  Deviation record ID -  H3_MISRAC_2012_R_8_6_DR_1 */
/* Device vectors list dummy definition*/
extern void vPortSVCHandler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void xPortPendSVHandler         ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void SUPC_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void RSTC_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void RTC_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void RTT_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void WDT_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void PMC_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void EFC_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void UART0_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void UART1_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void PIOA_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void PIOB_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void PIOC_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void USART0_Handler             ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void USART2_Handler             ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void PIOD_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void PIOE_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void HSMCI_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void TWIHS1_Handler             ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void SPI0_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void SSC_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void TC0_CH0_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void TC0_CH1_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void TC0_CH2_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void TC1_CH0_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void TC1_CH1_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void TC1_CH2_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void AFEC0_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void DACC_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void PWM0_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void ICM_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void ACC_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void USBHS_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void MCAN0_INT0_Handler         ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void MCAN0_INT1_Handler         ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void MCAN1_INT0_Handler         ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void MCAN1_INT1_Handler         ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void GMAC_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void AFEC1_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void TWIHS2_Handler             ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void SPI1_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void QSPI_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void UART2_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void UART3_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void UART4_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void TC2_CH0_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void TC2_CH1_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void TC2_CH2_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void TC3_CH0_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void TC3_CH1_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void TC3_CH2_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void MLB_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void AES_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void TRNG_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void XDMAC_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void ISI_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void PWM1_Handler               ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void FPU_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void RSWDT_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void CCW_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void CCF_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void GMAC_Q1_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void GMAC_Q2_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void IXC_Handler                ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void I2SC0_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void I2SC1_Handler              ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void GMAC_Q3_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void GMAC_Q4_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));
extern void GMAC_Q5_Handler            ( void ) __attribute__((weak, alias("Dummy_Handler")));


/* MISRAC 2012 deviation block end */

/* Multiple handlers for vector */



__attribute__ ((section(".vectors")))
const H3DeviceVectors exception_table=
{
    /* Configure Initial Stack Pointer, using linker-generated symbols */
    .pvStack = &_estack,

    .pfnReset_Handler              = Reset_Handler,
    .pfnNonMaskableInt_Handler     = NonMaskableInt_Handler,
    .pfnHardFault_Handler          = HardFault_Handler,
    .pfnMemoryManagement_Handler   = MemoryManagement_Handler,
    .pfnBusFault_Handler           = BusFault_Handler,
    .pfnUsageFault_Handler         = UsageFault_Handler,
    .pfnSVCall_Handler             = vPortSVCHandler,
    .pfnDebugMonitor_Handler       = DebugMonitor_Handler,
    .pfnPendSV_Handler             = xPortPendSVHandler,
    .pfnSysTick_Handler            = xPortSysTickHandler,
    .pfnSUPC_Handler               = SUPC_Handler,
    .pfnRSTC_Handler               = RSTC_Handler,
    .pfnRTC_Handler                = RTC_Handler,
    .pfnRTT_Handler                = RTT_Handler,
    .pfnWDT_Handler                = WDT_Handler,
    .pfnPMC_Handler                = PMC_Handler,
    .pfnEFC_Handler                = EFC_Handler,
    .pfnUART0_Handler              = UART0_Handler,
    .pfnUART1_Handler              = UART1_Handler,
    .pfnPIOA_Handler               = PIOA_Handler,
    .pfnPIOB_Handler               = PIOB_Handler,
    .pfnPIOC_Handler               = PIOC_Handler,
    .pfnUSART0_Handler             = USART0_Handler,
    .pfnUSART1_Handler             = USART1_InterruptHandler,
    .pfnUSART2_Handler             = USART2_Handler,
    .pfnPIOD_Handler               = PIOD_Handler,
    .pfnPIOE_Handler               = PIOE_Handler,
    .pfnHSMCI_Handler              = HSMCI_Handler,
    .pfnTWIHS0_Handler             = TWIHS0_InterruptHandler,
    .pfnTWIHS1_Handler             = TWIHS1_Handler,
    .pfnSPI0_Handler               = SPI0_Handler,
    .pfnSSC_Handler                = SSC_Handler,
    .pfnTC0_CH0_Handler            = TC0_CH0_Handler,
    .pfnTC0_CH1_Handler            = TC0_CH1_Handler,
    .pfnTC0_CH2_Handler            = TC0_CH2_Handler,
    .pfnTC1_CH0_Handler            = TC1_CH0_Handler,
    .pfnTC1_CH1_Handler            = TC1_CH1_Handler,
    .pfnTC1_CH2_Handler            = TC1_CH2_Handler,
    .pfnAFEC0_Handler              = AFEC0_Handler,
    .pfnDACC_Handler               = DACC_Handler,
    .pfnPWM0_Handler               = PWM0_Handler,
    .pfnICM_Handler                = ICM_Handler,
    .pfnACC_Handler                = ACC_Handler,
    .pfnUSBHS_Handler              = USBHS_Handler,
    .pfnMCAN0_INT0_Handler         = MCAN0_INT0_Handler,
    .pfnMCAN0_INT1_Handler         = MCAN0_INT1_Handler,
    .pfnMCAN1_INT0_Handler         = MCAN1_INT0_Handler,
    .pfnMCAN1_INT1_Handler         = MCAN1_INT1_Handler,
    .pfnGMAC_Handler               = GMAC_Handler,
    .pfnAFEC1_Handler              = AFEC1_Handler,
    .pfnTWIHS2_Handler             = TWIHS2_Handler,
    .pfnSPI1_Handler               = SPI1_Handler,
    .pfnQSPI_Handler               = QSPI_Handler,
    .pfnUART2_Handler              = UART2_Handler,
    .pfnUART3_Handler              = UART3_Handler,
    .pfnUART4_Handler              = UART4_Handler,
    .pfnTC2_CH0_Handler            = TC2_CH0_Handler,
    .pfnTC2_CH1_Handler            = TC2_CH1_Handler,
    .pfnTC2_CH2_Handler            = TC2_CH2_Handler,
    .pfnTC3_CH0_Handler            = TC3_CH0_Handler,
    .pfnTC3_CH1_Handler            = TC3_CH1_Handler,
    .pfnTC3_CH2_Handler            = TC3_CH2_Handler,
    .pfnMLB_Handler                = MLB_Handler,
    .pfnAES_Handler                = AES_Handler,
    .pfnTRNG_Handler               = TRNG_Handler,
    .pfnXDMAC_Handler              = XDMAC_Handler,
    .pfnISI_Handler                = ISI_Handler,
    .pfnPWM1_Handler               = PWM1_Handler,
    .pfnFPU_Handler                = FPU_Handler,
    .pfnRSWDT_Handler              = RSWDT_Handler,
    .pfnCCW_Handler                = CCW_Handler,
    .pfnCCF_Handler                = CCF_Handler,
    .pfnGMAC_Q1_Handler            = GMAC_Q1_Handler,
    .pfnGMAC_Q2_Handler            = GMAC_Q2_Handler,
    .pfnIXC_Handler                = IXC_Handler,
    .pfnI2SC0_Handler              = I2SC0_Handler,
    .pfnI2SC1_Handler              = I2SC1_Handler,
    .pfnGMAC_Q3_Handler            = GMAC_Q3_Handler,
    .pfnGMAC_Q4_Handler            = GMAC_Q4_Handler,
    .pfnGMAC_Q5_Handler            = GMAC_Q5_Handler,


};

/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */
void __attribute__((weak, optimize("-O1"), section(".text.Reset_Handler"), long_call,
                    naked, externally_visible))
Reset_Handler(void)
{
        uint32_t *pSrc, *pDest;

        /* Initialize the relocate segment */
        pSrc = &_etext;
        pDest = &_srelocate;

        if (pSrc != pDest) {
                for (; pDest < &_erelocate;) {
                        *pDest++ = *pSrc++;
                }
        }

        /* Clear the zero segment */
        for (pDest = &_szero; pDest < &_ezero;) {
                *pDest++ = 0;
        }

        /* Set the vector table base address */
        pSrc = (uint32_t *) & _sfixed;
        SCB->VTOR = ((uint32_t) pSrc & SCB_VTOR_TBLOFF_Msk);

        /* Call the optional application-provided _on_reset() function. */
        if (_on_reset) {
                _on_reset();
        }

        // TODO check this
        #ifdef __FPU_PRESENT
        /* Enable FPU if present and used */
        SCB->CPACR |= (0xFu << 20);
        __DSB();
        __ISB();
        #endif

        /* Initialize the C library */
        __libc_init_array();

        /* Call the optional application-provided _on_bootstrap() function. */
        if (_on_bootstrap) {
                _on_bootstrap();
        }

        // Initialize the SAM system
        SystemInit();

        /* Branch to main function */
        int __launchStatus = main();
        // TODO do something to handle this being an error

        // Should never reach here (since main should be calling a while loop)
        /* return (EXIT_FAILURE); */
}

/**
* \brief Default interrupt handler for unused IRQs.
*/
void __attribute__((weak, optimize("-O1"), section(".text.Dummy_Handler"),
                    long_call, externally_visible))
Dummy_Handler(void)
{
/* #if (defined(__DEBUG) || defined(__DEBUG_D)) */
/*     __builtin_software_breakpoint(); */
/* #endif */
    while (1) {}
}
