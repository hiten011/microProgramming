#include "my_stm_helper.h"

void Clock_Enable(PeripheralBus_t peripheral) {
  switch (peripheral) {
    case GPIOA_BUS: RCC->IOPENR  |= 1<<0;  break;
    case GPIOB_BUS: RCC->IOPENR  |= 1<<1;  break;
    case TIM16_BUS: RCC->APBENR2|= 1<<17; break;
    case TIM17_BUS: RCC->APBENR2|= 1<<18; break;
  }
}

void GPIO_InitPin(GPIO_TypeDef *port, uint8_t pin, GPIOMode_t mode, GPIOPull_t pull) {
  port->MODER &= ~(0b11 << (pin*2));
  port->MODER |=  (mode << (pin*2));
  port->PUPDR &= ~(0b11 << (pin*2));
  port->PUPDR |=  (pull << (pin*2));
}

void Timer_Init(TIM_TypeDef *timer, IRQn_Type irq_type, uint16_t psc, uint16_t arr, bool enable_interrupt) {
  timer->PSC = psc;
  timer->ARR = arr;
  if (enable_interrupt) {
    timer->DIER |= 1<<0;
    NVIC_EnableIRQ(irq_type);
  }
  timer->CR1 |= 1<<0;
}

void GPIO_OutputTogglePin(GPIO_TypeDef *port, uint8_t pin) {
  port->ODR ^= (1 << pin);
}