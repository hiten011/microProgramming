#include "assignment_1.h"
#include "stm32c0xx.h"
#include <stdint.h>

volatile ItemColour_t truth_colour, sensor_colour;

volatile uint32_t lfsr_state = 0xA5A5A5A5;

void SysTick_Init(void){
    // Part of the Arm Cortex Peripherals. Not found in RM0490
    SysTick->LOAD = 0x00FFFFFF;
    SysTick->VAL  = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
}
// Subroutine to add faulty detections
ItemColour_t Apply_LFSR_Fault(ItemColour_t true_col) {
    lfsr_state = (lfsr_state >> 1) ^ (-(lfsr_state & 1u) & 0xD0000001u);
    
    if ((lfsr_state % 100) < 20) {
        return (ItemColour_t)((true_col % 3) + 1);
    }
    return true_col;
}
// Subroutine to generate item and sensor detection 
void Generate_Next_Item(void) {
    lfsr_state ^= SysTick->VAL;
    truth_colour = (ItemColour_t)((lfsr_state % 3) + 1);
    sensor_colour = Apply_LFSR_Fault(truth_colour);
    Set_Truth_RGB(truth_colour);
    Set_Sensor_RGB(sensor_colour);
}