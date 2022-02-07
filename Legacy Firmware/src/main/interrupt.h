/***************************************************************************//**
 * @file        interrupt.h
 * @brief       Interrupt Header File
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 438 $
 *              $LastChangedDate: 2010-05-20 09:55:31 +0200 (Do, 20 Mai 2010) $
 ******************************************************************************/

#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

/*******************************************************************************
IRQ Priority organization (from highest to lowest):
The lower the preemtion priority the higher is the irq priority. Preemtion
priority is 4 bits when NVIC_PriorityGroup_4 is used, 0x0 is highest priority,
0xF is lowest.
*******************************************************************************/

#define IRQ_PRIORITY_DMA            0x01
#define IRQ_PRIORITY_TIM4           0x02
#define IRQ_PRIORITY_SYSTICK		0x03
#define IRQ_PRIORITY_USART3         0x04
#define IRQ_PRIORITY_ADC            0x05
#define IRQ_PRIORITY_USART1			0x06

#endif /* _INTERRUPT_H_ */
