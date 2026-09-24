#include "my_helpers.h"

static void Pin_Set_If(GPIO_TypeDef *port, uint8_t pin, bool condition) {
    if (condition) {
        GPIO_OutputSetPin(port, pin);
    } else {
        GPIO_OutputClearPin(port, pin);
    }
}

void Set_Truth_RGB(ItemColour_t truth_colour) {
    Pin_Set_If(GPIOA, 0, truth_colour == 1);
    Pin_Set_If(GPIOA, 1, truth_colour == 2);
    Pin_Set_If(GPIOA, 4, truth_colour == 3);
}

void Set_Sensor_RGB(ItemColour_t sensor_colour) {
    Pin_Set_If(GPIOA, 3, sensor_colour == 1);
    Pin_Set_If(GPIOA, 11, sensor_colour == 2);
    Pin_Set_If(GPIOA, 8, sensor_colour == 3);
}

void Set_Fault(PinState st) {
    Pin_Set_If(GPIOA, 9, st == PIN_HIGH);
}

void Gate1_SetWidth(uint16_t width_ms) {
    TIM3->CCR2 = width_ms;
}

void Gate2_SetWidth(uint16_t width_ms) {
    TIM3->CCR3 = width_ms;
}