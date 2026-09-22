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

  // Configure Pull-Up/Pull-Down Resistor
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

void Timer_Init(TIM_TypeDef *timer, IRQn_Type irq_type, uint16_t psc, uint16_t arr, bool enable_interrupt, bool enable_arpe, bool force_update, bool enable_counter) {
  timer->PSC = psc;
  timer->ARR = arr;

  if (enable_arpe) {
    timer->CR1 |= (1 << 7); // ARPE: buffer ARR, only reload on update event
  }

  if (force_update) {
    timer->EGR |= (1 << 0); // UG: force PSC/ARR to load now instead of on next overflow
  }

  if (enable_interrupt) {
    timer->DIER |= 1<<0;
    NVIC_EnableIRQ(irq_type);
  }

  if (enable_counter) {
    timer->CR1 |= (1 << 0);
  }
}

void Timer_ConfigChannel(TIM_TypeDef *timer, TimerChannel_t channel, TimerChannelMode_t mode, TimerPolarity_t polarity, bool enable_preload, bool enable_counter) {
  // Channels 1-2 live in CCMR1, channels 3-4 in CCMR2; each gets an 8-bit slice.
  volatile uint32_t *ccmr = (channel < TIM_CHANNEL_3) ? &timer->CCMR1 : &timer->CCMR2;
  uint8_t ccmr_shift = (channel % 2) * 8;
  uint8_t ccer_shift = channel * 4; // CCxE=+0, CCxP=+1, CCxNP=+3 (CC1E=bit0, CC2E=bit4, ...)

  *ccmr &= ~(0xFFUL << ccmr_shift); // Clear this channel's whole 8-bit slice

  switch (mode) {
    case TIM_MODE_INPUT_CAPTURE:
      *ccmr |= (0b01 << ccmr_shift); // CCxS = 01: IC mapped to direct input (TIx)
      break;
    case TIM_MODE_OUTPUT_FROZEN:
      break; // CCxS = 00, OCxM = 000: nothing to set
    case TIM_MODE_OUTPUT_TOGGLE:
      *ccmr |= (0b011 << (ccmr_shift + 4)); // OCxM = 011
      break;
    case TIM_MODE_OUTPUT_PWM1:
      *ccmr |= (0b110 << (ccmr_shift + 4)); // OCxM = 110
      break;
    case TIM_MODE_OUTPUT_PWM2:
      *ccmr |= (0b111 << (ccmr_shift + 4)); // OCxM = 111
      break;
  }

  if (enable_preload) {
    *ccmr |= (1 << (ccmr_shift + 3)); // OCxPE: buffer CCRx, only reload on update event
  }

  switch (polarity) {
    case TIM_POLARITY_RISING:
      timer->CCER &= ~(1 << (ccer_shift + 1)); // CCxP = 0
      timer->CCER &= ~(1 << (ccer_shift + 3)); // CCxNP = 0
      break;
    case TIM_POLARITY_FALLING:
      timer->CCER |=  (1 << (ccer_shift + 1)); // CCxP = 1
      timer->CCER &= ~(1 << (ccer_shift + 3)); // CCxNP = 0
      break;
    case TIM_POLARITY_BOTH:
      timer->CCER |=  (1 << (ccer_shift + 1)); // CCxP = 1
      timer->CCER |=  (1 << (ccer_shift + 3)); // CCxNP = 1
      break;
  }

  timer->CCER |= (1 << ccer_shift); // CCxE: enable this channel

  if (enable_counter) {
    timer->CR1 |= (1 << 0); // CEN: start counting
  }
}


// --- EXTI Wrappers ---
void EXTI_Init(GPIO_TypeDef *port, uint8_t pin, ExtiTrigger trigger, IRQn_Type irq_type) {
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
  // Clear existing trigger configurations
  EXTI->FTSR1 &= ~(1 << pin);
  EXTI->RTSR1 &= ~(1 << pin);
  if (trigger == TRIG_FALLING || trigger == TRIG_BOTH) {
    EXTI->FTSR1 |= (1 << pin); // Enable Falling Edge
  }
  if (trigger == TRIG_RISING || trigger == TRIG_BOTH) {
    EXTI->RTSR1 |= (1 << pin); // Enable Rising Edge
  }

  // 4. Unmask the designated interrupt line : EXTI - > IMR1
  EXTI->IMR1 |= (1<< pin);

  // Clear any pending flags (FPR1 = Falling, RPR1 = Rising)
  // Write to clear register
  EXTI->FPR1 |= (1 << pin);
  EXTI->RPR1 |= (1 << pin);

  // 5. Enable the shared NVIC line (idempotent if already enabled by another pin)
  NVIC_EnableIRQ(irq_type);
}

void GPIO_OutputTogglePin(GPIO_TypeDef *port, uint8_t pin) {
  port->ODR ^= (1 << pin);
}

void GPIO_OutputSetPin(GPIO_TypeDef *port, uint8_t pin) {
  port->ODR |= (1 << pin);
}

void GPIO_OutputClearPin(GPIO_TypeDef *port, uint8_t pin) {
  port->ODR &= ~(1 << pin);
}

bool GPIO_InputReadPin(GPIO_TypeDef *port, uint8_t pin) {
  return (port->IDR & (1 << pin)) != 0; // Check if Pin is high
}
