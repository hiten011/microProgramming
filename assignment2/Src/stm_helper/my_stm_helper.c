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
    case I2C1_BUS:
      break;
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
void GPIO_ConnectPeripheral(GPIO_TypeDef *port, uint8_t pin, uint8_t af) {
  if (pin < 8) {
    port->AFR[0] &= ~(0xFUL << (pin * 4));
    port->AFR[0] |=  ((uint32_t)af << (pin * 4));
  } else {
    port->AFR[1] &= ~(0xFUL << ((pin - 8) * 4));
    port->AFR[1] |=  ((uint32_t)af << ((pin - 8) * 4));
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

void PWM_Init(TIM_TypeDef *timer, TimerChannel_t channel, TimerChannelMode_t mode, bool enable_counter) {
  uint32_t OC2M, CC2S;

  switch (mode) {
    case TIM_MODE_OUTPUT_PWM1:    OC2M = 0b110, CC2S = 0b00; break;  
    default:                      OC2M = 0b000, CC2S = 0b00; break;
  }

  switch (channel) {
    case TIM_CHANNEL_2:
      // Set mode
      timer->CCMR1 &= ~(0b11 << 8);
      timer->CCMR1 |=  (CC2S << 8);

      // set further 
      timer->CCMR1 &= ~(0b111 << 12);
      timer->CCMR1 |=  (OC2M << 12);

      timer->CCMR1 |= (1 << 11); // enable preload
      timer->CCER  |= (1 << 4);  // enable output
      break;

    case TIM_CHANNEL_3:
      // Set mode
      timer->CCMR2 &= ~(0b11 << 0);
      timer->CCMR2 |=  (CC2S << 0);

      // set further 
      timer->CCMR2 &= ~(0b111 << 4);
      timer->CCMR2 |=  (OC2M << 4);

      timer->CCMR2 |= (1 << 3);  // enable preload
      timer->CCER  |= (1 << 8);  // enable output
      break;

    default:
      break;
  }

  if (enable_counter) {
    timer->CR1 |= (1 << 0);
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

// I2C driver
void I2C_Master_Init(I2C_TypeDef *i2c, uint8_t presc, uint8_t sclh, uint8_t scll) {
  // Disable I2C peripheral before modifying timing
  i2c->CR1 &= ~(1<<0);
  // Configure the TIMINGR register
  i2c->TIMINGR = ((uint32_t)presc << 28) | ((uint32_t)sclh << 8) | ((uint32_t)scll<<0);
  // Re-enable the I2C peripheral
  i2c->CR1 |= (1<<0);
}

bool check_timeout(uint16_t start_tick, uint16_t timeout_ms) {
  uint16_t current_tick = (uint16_t)TIM16->CNT;
  return (uint16_t)(current_tick - start_tick) >= timeout_ms;
}

bool I2C_Master_ReadRegs(I2C_TypeDef *i2c, uint8_t dev_addr, uint8_t reg_addr, uint8_t *rx_buf, uint16_t size) {
  uint16_t start_tick;
  const uint16_t timeout_ms = 10; // 10 ms hard timeout
  // Wait for i2c bus free
  start_tick = (uint16_t)TIM16->CNT; // use current counter value as start time (1 tick = 1ms)
  while (i2c->ISR & (1<<15)) {
    // Check Busy flag. If timeout > 10 ms; bus is busy. Exit
    if (check_timeout(start_tick, timeout_ms)) return false;
  }
  // Write the target device register address over I2C ()
  // device_addrs, num)bytes, autoend = true, start transmission
  i2c->CR2 &= ~(1<<10); // Configure to write
  i2c->CR2 = (dev_addr << 1) | (1 << 16) | (1<<25) | (1<<13);
  start_tick = (uint16_t)TIM16->CNT; // reset start_tick counter
  // Wait for communication of device address to complete
  while (!(i2c->ISR & (1<<1))) {
    // Check status of TXIS
    if (check_timeout(start_tick, timeout_ms)) return false;
  }
  // Device address has been written, TXDR now empty
  i2c->TXDR = reg_addr; // Address of register on peripheral to read
  start_tick = (uint16_t)TIM16->CNT; // Reset Start Tick counter
  while (!(i2c->ISR & (1<<5))) {
    // Wait for communication with peripheral to be complete
    if (check_timeout(start_tick, timeout_ms)) return false;
  }
  i2c->ICR |= (1<<5); // Write 1 to clear stop flag
  // Small pause to check state of I2C line
  start_tick = (uint16_t)TIM16->CNT; // Reset Start Tick counter
  while (i2c->ISR & (1<<15)) {
    // Check Busy flag. If timeout > 10 ms; bus is busy. Exit
    if (check_timeout(start_tick, timeout_ms)) return false;
  }
  // Read back requested bytes from attached peripheral
  // device_addrs, num_bytes, set to read, autoend = true, start transmission
  i2c->CR2 = (dev_addr << 1) | ((uint32_t)size << 16) | (1<<10) | (1<<25) | (1<<13);
  // Populate buffer with number of bytes that are expected to be received from peripheral to micro
  for (uint16_t i = 0; i < size; i++) {
    start_tick = (uint16_t)TIM16->CNT; // Reset Start tick counter
    while (!(i2c->ISR & (1<<2))) {
      // While no data has been received, check if a NACK transmitted across or timeout
      if (i2c->ISR & (1<<4)) return false;
      if (check_timeout(start_tick, timeout_ms)) return false;
    }
    // Store received data in software buffer
    rx_buf[i] = (uint8_t)i2c->RXDR;
  }
  start_tick = (uint16_t)TIM16->CNT; // Reset Start Tick counter
  while (!(i2c->ISR & (1<<5))) {
    // Wait for communication with peripheral to be complete
    if (check_timeout(start_tick, timeout_ms)) return false;
  }
  i2c->ICR |= (1<<5); // Write 1 to clear stop flag
  return true;
}

bool I2C_Master_WriteReg(I2C_TypeDef *i2c, uint8_t dev_addr, uint8_t reg_addr, uint8_t data) {
  uint16_t start_tick;
  const uint16_t timeout_ms = 10;
  // Wait for i2c bus free
  start_tick = (uint16_t)TIM16->CNT; // use current counter value as start time (1 tick = 1ms)
  while (i2c->ISR & (1<<15)) {
    // Check Busy flag. If timeout > 10 ms; bus is busy. Exit
    if (check_timeout(start_tick, timeout_ms)) return false;
  }
  // Write the target device register address over I2C () and one byte of data
  // device_addrs, num_bytes, autoend = true, start transmission
  i2c->CR2 &= ~(1<<10); // Configure to write
  i2c->CR2 = (dev_addr << 1) | (2 << 16) | (1<<25) | (1<<13);
  start_tick = (uint16_t)TIM16->CNT; // reset start_tick counter
  // Wait for communication of device address to complete
  while (!(i2c->ISR & (1<<1))) {
    // Check status of TXIS
    if (check_timeout(start_tick, timeout_ms)) return false;
  }
  // Device address has been written, TXDR now empty
  i2c->TXDR = reg_addr; // Address of register on peripheral to read
  // Wait for communication of register address to complete
  while (!(i2c->ISR & (1<<1))) {
    // Check status of TXIS
    if (check_timeout(start_tick, timeout_ms)) return false;
  }
  i2c->TXDR = data; // Actual data to write to peripheral
  start_tick = (uint16_t)TIM16->CNT; // Reset Start Tick counter
  while (!(i2c->ISR & (1<<5))) {
    // Wait for communication with peripheral to be complete (check stop flag)
    if (check_timeout(start_tick, timeout_ms)) return false;
  }
  i2c->ICR |= (1<<5); // Write 1 to clear stop flag
  return true;
}

// MPU
// MPU6050 accelerometer functions
// Wake MPU6050 and select the +/- 2g accelerometer range
bool MPU6050_Init(void) {
    if (!I2C_Master_WriteReg(I2C1, MPU6050_ADDR, MPU6050_PWR_MGMT_1, 0x00)) {
        return false;
    }

    if (!I2C_Master_WriteReg(I2C1, MPU6050_ADDR, MPU6050_ACCEL_CONFIG, 0x00)) {
        return false;
    }

    return true;
}

// Read X, Y and Z accelerometer measurements (six consecutive bytes)
bool MPU6050_ReadAccel(int16_t *ax, int16_t *ay, int16_t *az) {
    uint8_t accel_buf[6];

    if (!I2C_Master_ReadRegs(I2C1, MPU6050_ADDR, MPU6050_ACCEL_XOUT_H, accel_buf, 6)) {
        return false;
    }

    *ax = (int16_t)(((uint16_t)accel_buf[0] << 8) | accel_buf[1]);
    *ay = (int16_t)(((uint16_t)accel_buf[2] << 8) | accel_buf[3]);
    *az = (int16_t)(((uint16_t)accel_buf[4] << 8) | accel_buf[5]);

    return true;
}