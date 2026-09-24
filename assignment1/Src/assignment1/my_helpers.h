#ifndef MY_HELPERS_H
#define MY_HELPERS_H

#include "assignment_1.h"
#include "my_stm_helper.h"

void Set_Truth_RGB(ItemColour_t truth_colour);
void Set_Sensor_RGB(ItemColour_t sensor_colour);
void Set_Fault(PinState st);

// SERVO GATES
#define GATE_ANGLE_0    1000
#define GATE_ANGLE_90   1500
#define GATE_ANGLE_180  2000

void Gate1_SetWidth(uint16_t width_ms); // PB0 / TIM3_CH3
void Gate2_SetWidth(uint16_t width_ms); // PC7 / TIM3_CH2

#endif
