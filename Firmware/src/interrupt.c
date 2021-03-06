/***************************************************************************//**
 * @file        interrupt.c
 * @brief       Interrupt Handler Mainfile
 * @author      Created: CLU
 *
 * This file provides all system interrupt handlers. It linkes all non maskable
 * cortex interrupts to a system exception function. These handlers must never
 * be executed if the system is running as it should.
 *
 * Also this file provides the SysTick_Handler function, that is periodically
 * called when a system tick interrupt happens. This is the main timebase of
 * the applicationFTIM5
 ******************************************************************************/
#include "interrupt.h"
#include "common.h"
#include "statusLed.h"
#include "uart1.h"
#include "recv.h"
#include "adc.h"
#include "ledControl.h"
#include "softUart.h"

#include <stdio.h>
#include "teco.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SYSTICK_LOWSEPEED_DIVIDER 20

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t systick_highSpeedCounter;
uint8_t systick_lowSpeedCounter;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_Handler(void) {

    HAL_IncTick();
    ledControl_update();
    statusLed_update();

    systick_lowSpeedCounter++;
    if (systick_lowSpeedCounter >= SYSTICK_LOWSEPEED_DIVIDER) {
        systick_lowSpeedCounter = 0;
        adc_update();
        ledControl_lowSpeed();
    }
}

/*******************************************************************************
* Function Name  : NMI_Handler
* Description    : This function handles NMI exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NMI_Handler(void) {
}

/*******************************************************************************
* Function Name  : HardFault_Handler
* Description    : This function handles Hard Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HardFault_Handler(void) {
    while (1) {}
}

/*******************************************************************************
* Function Name  : MemManage_Handler
* Description    : This function handles Memory Manage exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MemManage_Handler(void) {
    while (1) {}
}

/*******************************************************************************
* Function Name  : BusFault_Handler
* Description    : This function handles Bus Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BusFault_Handler(void) {
    while (1) {}
}

/*******************************************************************************
* Function Name  : UsageFault_Handler
* Description    : This function handles Usage Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UsageFault_Handler(void) {
    while (1) {}
}

/*******************************************************************************
* Function Name  : DebugMon_Handler
* Description    : This function handles Debug Monitor exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DebugMon_Handler(void) {
}

/*******************************************************************************
* Function Name  : SVC_Handler
* Description    : This function handles SVCall exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVC_Handler(void) {
}

/*******************************************************************************
* Function Name  : PendSV_Handler
* Description    : This function handles PendSVC exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PendSV_Handler(void) {
}


/*******************************************************************************
* Function Name  : WWDG_IRQHandler
* Description    : This function handles WWDG interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WWDG_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : PVD_IRQHandler
* Description    : This function handles PVD interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PVD_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : TAMPER_IRQHandler
* Description    : This function handles Tamper interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TAMPER_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : RTC_IRQHandler
* Description    : This function handles RTC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : FLASH_IRQHandler
* Description    : This function handles Flash interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : RCC_IRQHandler
* Description    : This function handles RCC interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : EXTI0_IRQHandler
* Description    : This function handles External interrupt Line 0 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI0_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : EXTI1_IRQHandler
* Description    : This function handles External interrupt Line 1 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI1_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : EXTI2_IRQHandler
* Description    : This function handles External interrupt Line 2 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI2_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : EXTI3_IRQHandler
* Description    : This function handles External interrupt Line 3 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI3_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : EXTI4_IRQHandler
* Description    : This function handles External interrupt Line 4 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI4_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : DMAChannel1_IRQHandler
* Description    : This function handles DMA Stream 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel1_IRQHandler(void){
}

/*******************************************************************************
* Function Name  : DMAChannel2_IRQHandler
* Description    : This function handles DMA Stream 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel2_IRQHandler(void){
    softuart_txDmaIsr();
}

/*******************************************************************************
* Function Name  : DMAChannel3_IRQHandler
* Description    : This function handles DMA Stream 3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel3_IRQHandler(void){
}

/*******************************************************************************
* Function Name  : DMAChannel4_IRQHandler
* Description    : This function handles DMA Stream 4 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel4_IRQHandler(void){
}

/*******************************************************************************
* Function Name  : DMAChannel5_IRQHandler
* Description    : This function handles DMA Stream 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel5_IRQHandler(void){
}

/*******************************************************************************
* Function Name  : DMAChannel6_IRQHandler
* Description    : This function handles DMA Stream 6 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel6_IRQHandler(void){
    softuart_rxDmaIsr();
}

/*******************************************************************************
* Function Name  : DMAChannel7_IRQHandler
* Description    : This function handles DMA Stream 7 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel7_IRQHandler(void){
}

/*******************************************************************************
* Function Name  : ADC_IRQHandler
* Description    : This function handles ADC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_IRQHandler(void){
}

/*******************************************************************************
* Function Name  : USB_HP_CAN_TX_IRQHandler
* Description    : This function handles USB High Priority or CAN TX interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_HP_CAN_TX_IRQHandler(void){
}

/*******************************************************************************
* Function Name  : USB_LP_CAN_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_LP_CAN_RX0_IRQHandler(void){
}

/*******************************************************************************
* Function Name  : CAN_RX1_IRQHandler
* Description    : This function handles CAN RX1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_RX1_IRQHandler(void){
}

/*******************************************************************************
* Function Name  : CAN_SCE_IRQHandler
* Description    : This function handles CAN SCE interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_SCE_IRQHandler(void){
}

/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : This function handles External lines 9 to 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI9_5_IRQHandler(void){
}

/*******************************************************************************
* Function Name  : TIM1_BRK_IRQHandler
* Description    : This function handles TIM1 Break interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_BRK_IRQHandler(void){
}

/*******************************************************************************
* Function Name  : TIM1_UP_IRQHandler
* Description    : This function handles TIM1 overflow and update interrupt
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_UP_IRQHandler(void){
}

/*******************************************************************************
* Function Name  : TIM1_TRG_COM_IRQHandler
* Description    : This function handles TIM1 Trigger and commutation interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_TRG_COM_IRQHandler(void){
}

/*******************************************************************************
* Function Name  : TIM1_CC_IRQHandler
* Description    : This function handles TIM1 capture compare interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_CC_IRQHandler(void){
}

/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : This function handles TIM4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM4_IRQHandler(void) {
    recv_isr();
}

/*******************************************************************************
* Function Name  : I2C1_EV_IRQHandler
* Description    : This function handles I2C1 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_EV_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : I2C1_ER_IRQHandler
* Description    : This function handles I2C1 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_ER_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : I2C2_EV_IRQHandler
* Description    : This function handles I2C2 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_EV_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : I2C2_ER_IRQHandler
* Description    : This function handles I2C2 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_ER_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : SPI1_IRQHandler
* Description    : This function handles SPI1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI1_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : SPI2_IRQHandler
* Description    : This function handles SPI2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI2_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_IRQHandler(void) {
    uart1_isr();
}

/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles USART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART2_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : This function handles USART3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART3_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : EXTI15_10_IRQHandler
* Description    : This function handles External lines 15 to 10 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI15_10_IRQHandler(void) {
    softuart_rxExtiIsr();
}

/*******************************************************************************
* Function Name  : RTCAlarm_IRQHandler
* Description    : This function handles RTC Alarm interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTCAlarm_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : USBWakeUp_IRQHandler
* Description    : This function handles USB WakeUp interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBWakeUp_IRQHandler(void) {
}


/*******************************************************************************
* Function Name  : DMA2_Channel1_IRQHandler
* Description    : This function handles DMA Stream 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel1_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : DMA2_Channel2_IRQHandler
* Description    : This function handles DMA Stream 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel2_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : DMA2_Channel3_IRQHandler
* Description    : This function handles DMA Stream 3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel3_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : DMA2_Channel4_IRQHandler
* Description    : This function handles DMA Stream 4 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel4_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : DMA2_Channel5_IRQHandler
* Description    : This function handles DMA Stream 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel5_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : TIM5_IRQHandler
* Description    : This function handles DMA Stream 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM5_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : TIM6_IRQHandler
* Description    : This function handles DMA Stream 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM6_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : TIM7_IRQHandler
* Description    : This function handles DMA Stream 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM7_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : SPI3_IRQHandler
* Description    : This function handles DMA Stream 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI3_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : UART4_IRQHandler
* Description    : This function handles DMA Stream 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART4_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : UART5_IRQHandler
* Description    : This function handles DMA Stream 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART5_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : ETH_IRQHandler
* Description    : This function handles DMA Stream 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ETH_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : ETH_WKUP_IRQHandler
* Description    : This function handles DMA Stream 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ETH_WKUP_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : CAN2_TX_IRQHandler
* Description    : This function handles DMA Stream 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN2_TX_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : CAN2_RX0_IRQHandler
* Description    : This function handles DMA Stream 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN2_RX0_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : CAN2_RX1_IRQHandler
* Description    : This function handles DMA Stream 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN2_RX1_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : CAN2_SCE_IRQHandler
* Description    : This function handles DMA Stream 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN2_SCE_IRQHandler(void) {
}

/*******************************************************************************
* Function Name  : OTG_FS_IRQHandler
* Description    : This function handles DMA Stream 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void OTG_FS_IRQHandler(void) {
}


