#ifndef MY_STM_HELPER_H
#define MY_STM_HELPER_H
#include "stm32c0xx.h"
#include <stdbool.h>

typedef enum { GPIOA_BUS, GPIOB_BUS, TIM16_BUS, TIM17_BUS } PeripheralBus_t;
typedef enum { INPUT=0b00, OUTPUT=0b01, ALT=0b10, ANALOG=0b11 } GPIOMode_t;
typedef enum { PIN_LOW = 0, PIN_HIGH = 1 } PinState;
typedef enum { NONE=0b00, PULL_UP=0b01, PULL_DOWN=0b10 } GPIOPull_t;
typedef enum { TRIG_RISING = 0, TRIG_FALLING = 1, TRIG_BOTH = 2 } ExtiTrigger;

void Clock_Enable(PeripheralBus_t peripheral);
void GPIO_InitPin(GPIO_TypeDef *port, uint8_t pin, GPIOMode_t mode, GPIOPull_t pull);
void Timer_Init(TIM_TypeDef *timer, IRQn_Type irq_type, uint16_t psc, uint16_t arr, bool enable_interrupt);
void EXTI_Init(GPIO_TypeDef *port, uint8_t pin, ExtiTrigger trigger);

uint8_t Pin_Read(GPIO_TypeDef *port, uint8_t pin);
void Pin_Write(GPIO_TypeDef *port, uint8_t pin, PinState state);
void GPIO_OutputTogglePin(GPIO_TypeDef *port, uint8_t pin);

#endif