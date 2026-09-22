#include "my_stm_helper.h"
#include <stdint.h>

void Clock_Enable(PeripheralBus_t peripheral) {
  switch (peripheral) {
    case GPIOA_BUS: RCC->IOPENR  |= 1<<0;  break;
    case GPIOB_BUS: RCC->IOPENR  |= 1<<1;  break;
    case GPIOC_BUS: RCC->IOPENR  |= 1<<2;  break;
    case TIM16_BUS: RCC->APBENR2 |= 1<<17; break;
    case TIM17_BUS: RCC->APBENR2 |= 1<<18; break;
    case TIM3_BUS:  RCC->APBENR1 |= 1<<1;  break;
  }
}

void GPIO_InitPin(GPIO_TypeDef *port, uint8_t pin, GPIOMode_t mode, GPIOPull_t pull) {
  port->MODER &= ~(0b11 << (pin*2));
  port->MODER |=  (mode << (pin*2));
  port->PUPDR &= ~(0b11 << (pin*2));
  port->PUPDR |=  (pull << (pin*2));
}

void GPIO_InitPins(GPIO_TypeDef *port, uint8_t *pins, GPIOMode_t *mode, GPIOPull_t *pull, uint16_t len) {
  for (int i = 0; i < len; i++) {
    GPIO_InitPin(port, pins[i], mode[i], pull[i]);
  }
}

void GPIO_InitPinsSameMode(GPIO_TypeDef *port, uint8_t *pins, GPIOMode_t mode, GPIOPull_t pull, uint16_t len) {
  for (int i = 0; i < len; i++) {
    GPIO_InitPin(port, pins[i], mode, pull);
  }
}

// Fills AFR with the peripheral's AF number (per STM32C0 datasheet AF table).
// Caller is responsible for setting the pin to ALT mode.
void GPIO_ConnectPeripheral(GPIO_TypeDef *port, uint8_t pin, PeripheralBus_t peripheral) {
  uint8_t af;
  switch (peripheral) {
    case TIM3_BUS:  af = 1; break;
    case TIM16_BUS: af = 5; break;
    case TIM17_BUS: af = 5; break;
    default: return; // no AF mapping for this peripheral
  }

  if (pin < 8) {
    port->AFR[0] &= ~(0xFUL << (pin * 4));
    port->AFR[0] |=  (af << (pin * 4));
  } else {
    port->AFR[1] &= ~(0xFUL << ((pin - 8) * 4));
    port->AFR[1] |=  (af << ((pin - 8) * 4));
  }
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


// --- EXTI Wrappers ---
void EXTI_Init(GPIO_TypeDef *port, uint8_t pin, ExtiTrigger trigger) {
  // 1. Establish the numerical port identifier
  uint32_t port_idx = 0;
  if (port == GPIOA) {
    port_idx = 0;
  } else if (port == GPIOB) {
    port_idx = 1;
  } else if (port == GPIOC) {
    port_idx = 2;
  }

  // 2. Configure the EXTI_EXTICRx Multiplexer
  // The STM32C0 allocates 8 bits per pin across 4 registers
  uint8_t register_index = pin / 4;
  uint8_t bit_position = (pin % 4) * 8;
  EXTI->EXTICR[register_index] &= ~(0xFF << bit_position);
  EXTI->EXTICR[register_index] |=  (port_idx << bit_position);

  // 3. Configure Trigger Selection Registers : EXTI - > FTSR1 and EXTI - >
  // RTSR1
  if (trigger == TRIG_FALLING || trigger == TRIG_BOTH) {
    EXTI->FTSR1 |= (1 << pin);
  }
  if (trigger == TRIG_RISING || trigger == TRIG_BOTH) {
    EXTI->RTSR1 |= (1 << pin);
  }

  // 4. Unmask the designated interrupt line : EXTI - > IMR1
  EXTI->IMR1 |= (1<< pin);
}

uint8_t Pin_Read(GPIO_TypeDef *port, uint8_t pin) {
  return (uint8_t)((port->IDR >> pin) & 1U);
}

void Pin_Write(GPIO_TypeDef *port, uint8_t pin, PinState state) {
  if (state == PIN_HIGH) {
    port->ODR |= (1U << pin);
  } else {
    port->ODR &= ~(1U << pin);
  }
}

void GPIO_OutputTogglePin(GPIO_TypeDef *port, uint8_t pin) {
  port->ODR ^= (1 << pin);
}
