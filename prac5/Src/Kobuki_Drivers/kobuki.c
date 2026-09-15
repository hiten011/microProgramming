#include <stdint.h>
#define STM32G474xx

#include "stm32g4xx.h"
#include "uart.h"
#include "kobuki.h"

void Kobuki_Tx(uint8_t *payload, uint8_t size_payload) {
  uint8_t checksum = 0, i;
  UART_Tx(0xAA);  // Send header 0
  UART_Tx(0x55);  // Send header 1
  UART_Tx(size_payload);  // Send size of payload in bytes
  checksum ^= size_payload;  // Calculate running checksum

  // Send each byte of payload one-by-one
  for (i = size_payload - 1; i >= 0; i--) {
    UART_Tx(payload[i]);
    checksum ^= payload[i];
  }

  UART_Tx(checksum);  // Send checksum
}
