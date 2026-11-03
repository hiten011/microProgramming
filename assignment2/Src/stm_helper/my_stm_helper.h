#ifndef MY_STM_HELPER_H
#define MY_STM_HELPER_H
#include "stm32c0xx.h"
#include <stdbool.h>

#define MPU6050_ADDR          0x68   // Default 7-bit I2C address (AD0 not tied to VCC)
#define MPU6050_PWR_MGMT_1    0x6B   // Power management 1 (clear SLEEP to wake the sensor)
#define MPU6050_ACCEL_CONFIG  0x1C   // Accelerometer range select (AFS_SEL)
#define MPU6050_ACCEL_XOUT_H  0x3B   // First of the six accelerometer data registers

typedef enum { GPIOA_BUS, GPIOB_BUS, GPIOC_BUS, TIM16_BUS, TIM17_BUS, TIM3_BUS, I2C1_BUS } PeripheralBus_t;
typedef enum { INPUT=0b00, OUTPUT=0b01, ALT=0b10, ANALOG=0b11 } GPIOMode_t;
typedef enum { PIN_LOW = 0, PIN_HIGH = 1 } PinState;
typedef enum { NONE=0b00, PULL_UP=0b01, PULL_DOWN=0b10 } GPIOPull_t;
typedef enum { TRIG_RISING = 0, TRIG_FALLING = 1, TRIG_BOTH = 2 } ExtiTrigger;
typedef enum { TIM_CHANNEL_1 = 0, TIM_CHANNEL_2 = 1, TIM_CHANNEL_3 = 2, TIM_CHANNEL_4 = 3 } TimerChannel_t;
typedef enum {
  TIM_MODE_INPUT_CAPTURE, 
  TIM_MODE_OUTPUT_FROZEN, 
  TIM_MODE_OUTPUT_TOGGLE, 
  TIM_MODE_OUTPUT_PWM1,   
  TIM_MODE_OUTPUT_PWM2,   
} TimerChannelMode_t;


void Clock_Enable(PeripheralBus_t peripheral);

// TIMER
void Timer_Init(TIM_TypeDef *timer, IRQn_Type irq_type, uint16_t psc, uint16_t arr, bool enable_interrupt, bool enable_arpe, bool force_update, bool enable_counter);

// PWM
void PWM_Init(TIM_TypeDef *timer, TimerChannel_t channel, TimerChannelMode_t mode, bool enable_counter);

// EXTI
void EXTI_Init(GPIO_TypeDef *port, uint8_t pin, ExtiTrigger trigger, IRQn_Type irq_type);

// GPIO
void GPIO_InitPin(GPIO_TypeDef *port, uint8_t pin, GPIOMode_t mode, GPIOPull_t pull);
void GPIO_InitPins(GPIO_TypeDef *port, uint8_t *pins, GPIOMode_t *mode, GPIOPull_t *pull, uint16_t len);
void GPIO_InitPinsSameMode(GPIO_TypeDef *port, uint8_t *pins, GPIOMode_t mode, GPIOPull_t pull, uint16_t len);
void GPIO_ConnectPeripheral(GPIO_TypeDef *port, uint8_t pin, uint8_t af);

void GPIO_OutputTogglePin(GPIO_TypeDef *port, uint8_t pin);
void GPIO_OutputSetPin(GPIO_TypeDef *port, uint8_t pin);
void GPIO_OutputClearPin(GPIO_TypeDef *port, uint8_t pin);
bool GPIO_InputReadPin(GPIO_TypeDef *port, uint8_t pin);

// I2C (code from ENGE X200 Lecture 8, slides 31-37)
void I2C_Master_Init(I2C_TypeDef *i2c, uint8_t presc, uint8_t sclh, uint8_t scll);
bool check_timeout(uint16_t start_tick, uint16_t timeout_ms);
bool I2C_Master_ReadRegs(I2C_TypeDef *i2c, uint8_t dev_addr, uint8_t reg_addr, uint8_t *rx_buf, uint16_t size);
bool I2C_Master_WriteReg(I2C_TypeDef *i2c, uint8_t dev_addr, uint8_t reg_addr, uint8_t data);

// Wake the MPU6050 and select the +/- 2g range. Returns false if an I2C transfer fails.
bool MPU6050_Init(void);
// Read raw X, Y, Z accelerometer values (signed 16-bit counts, 16384 LSB/g at +/- 2g).
// Returns false if the I2C transfer fails.
bool MPU6050_ReadAccel(int16_t *ax, int16_t *ay, int16_t *az);

#endif