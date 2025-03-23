/*
 * @file       delay library
 * @board      STM32F10x
 * @author     Tong Sy Tan
 * @date       Sun, 09/02/2025
*/

#include "delay.h"

static volatile uint32_t us;
static volatile uint32_t ms;

void Delay_Init(void) __attribute__((constructor));

void Delay_Init(void)
{
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000000);
}

void SysTick_Handler(void)
{
	us++;
}

uint32_t millis(void){
	return us/1000;
}
uint32_t micros(void){
	return us;
}
void delay_ms(uint32_t time_delay){
	uint32_t cur_time = millis();
	while(millis() - cur_time < time_delay);
}
void delay_us(uint32_t time_delay){
	uint32_t cur_time = micros();
	while(micros() - cur_time < time_delay);
}
