#include <stdint.h>
#define STM32G474xx

#include "kobuki.h"
#include "stm32g4xx.h"
#include "uart.h"
int main(void) {
  // Create a structure to store kobuki values :
  Kobuki_Typedef my_kobuki = {0};
  // Initialize system UART
  UART_Init();
  while (1) {
    // Read the full payload arriving at the RX pin and parse it
    Kobuki_Read(&my_kobuki);
    // Pause so you can inspect 'my_kobuki' in your debugger
    for (volatile int i = 0; i < 500000; i++)
      ;
  }
}