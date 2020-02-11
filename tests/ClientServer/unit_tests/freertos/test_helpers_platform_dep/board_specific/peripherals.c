/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Peripherals v5.0
processor: MIMXRT1064xxxxA
package_id: MIMXRT1064DVL6A
mcu_data: ksdk2_0
processor_version: 5.0.1
board: MIMXRT1064-EVK
functionalGroups:
- name: BOARD_InitPeripherals
  called_from_default_init: true
  selectedCore: core0
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
component:
- type: 'system'
- type_id: 'system_54b53072540eeeb8f8e9343e71f28176'
- global_system_definitions: []
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/
#include "peripherals.h"
#include "board.h"

/***********************************************************************************************************************
 * BOARD_InitPeripherals functional group
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * LPUART_1 initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'LPUART_1'
- type: 'lpuart'
- mode: 'polling'
- type_id: 'lpuart_bebe3e12b6ec22bbd14199038f2bf459'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'LPUART1'
- config_sets:
  - lpuartConfig_t:
    - lpuartConfig:
      - clockSource: 'LpuartClock'
      - lpuartSrcClkFreq: 'BOARD_BootClockRUN'
      - baudRate_Bps: '115200'
      - parityMode: 'kLPUART_ParityDisabled'
      - dataBitsCount: 'kLPUART_EightDataBits'
      - isMsb: 'false'
      - stopBitCount: 'kLPUART_OneStopBit'
      - txFifoWatermark: '0'
      - rxFifoWatermark: '1'
      - enableRxRTS: 'false'
      - enableTxCTS: 'false'
      - txCtsSource: 'kLPUART_CtsSourcePin'
      - txCtsConfig: 'kLPUART_CtsSampleAtStart'
      - rxIdleType: 'kLPUART_IdleTypeStartBit'
      - rxIdleConfig: 'kLPUART_IdleCharacter1'
      - enableTx: 'true'
      - enableRx: 'true'
    - quick_selection: 'QuickSelection1'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const lpuart_config_t LPUART_1_config = {.baudRate_Bps = 115200,
                                         .parityMode = kLPUART_ParityDisabled,
                                         .dataBitsCount = kLPUART_EightDataBits,
                                         .isMsb = false,
                                         .stopBitCount = kLPUART_OneStopBit,
                                         .txFifoWatermark = 0,
                                         .rxFifoWatermark = 1,
                                         .enableRxRTS = false,
                                         .enableTxCTS = false,
                                         .txCtsSource = kLPUART_CtsSourcePin,
                                         .txCtsConfig = kLPUART_CtsSampleAtStart,
                                         .rxIdleType = kLPUART_IdleTypeStartBit,
                                         .rxIdleConfig = kLPUART_IdleCharacter1,
                                         .enableTx = true,
                                         .enableRx = true};

static void LPUART_1_init(void)
{
    LPUART_Init(LPUART_1_PERIPHERAL, &LPUART_1_config, LPUART_1_CLOCK_SOURCE);
}

/***********************************************************************************************************************
 * Timer 3 initialization code used by RTOS
 **********************************************************************************************************************/
const qtmr_config_t TMR3_Config = {.primarySource = kQTMR_ClockDivide_2,
                                   .secondarySource = kQTMR_Counter0InputPin,
                                   .enableMasterMode = false,
                                   .enableExternalForce = false,
                                   .faultFilterCount = 0,
                                   .faultFilterPeriod = 0,
                                   .debugMode = kQTMR_RunNormalInDebug};

static uint32_t RTOS_RunTimeCounter = 0;
void TMR3_IRQHandler(void)
{
    QTMR_ClearStatusFlags(TMR3, kQTMR_Channel_1, kQTMR_CompareFlag);
    RTOS_RunTimeCounter++; /* increment runtime counter */
}

void RTOS_portCONFIGURE_TIMER_FOR_RUN_TIME_STATS(void)
{
    RTOS_RunTimeCounter = 0;
    EnableIRQ(TMR3_IRQn);
}

uint32_t RTOS_portGET_RUN_TIME_COUNTER_VALUE(void)
{
    return RTOS_RunTimeCounter;
}

static void TMR3_Init(void)
{
    QTMR_Init(TMR3, kQTMR_Channel_1, &TMR3_Config);
    QTMR_SetTimerPeriod(TMR3, kQTMR_Channel_1, USEC_TO_COUNT(100U, (CLOCK_GetFreq(kCLOCK_IpgClk) / 2)));
    QTMR_EnableInterrupts(TMR3, kQTMR_Channel_1, kQTMR_CompareInterruptEnable);
    EnableIRQ(TMR3_IRQn);
    QTMR_StartTimer(TMR3, kQTMR_Channel_1, kQTMR_PriSrcRiseEdge);
}

/***********************************************************************************************************************
 * Initialization functions
 **********************************************************************************************************************/
void BOARD_InitPeripherals(void)
{
    /* Initialize components */
    LPUART_1_init();
    TMR3_Init();
}

/***********************************************************************************************************************
 * BOARD_InitBootPeripherals function
 **********************************************************************************************************************/
void BOARD_InitBootPeripherals(void)
{
    BOARD_InitPeripherals();
    BOARD_InitEnet();
}