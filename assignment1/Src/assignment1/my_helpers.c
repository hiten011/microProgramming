#include "my_helpers.h"
#include "assignment_1.h"

static void Pin_Set_If(GPIO_TypeDef *port, uint8_t pin, bool condition) {
    if (condition) {
        GPIO_OutputSetPin(port, pin);
    } else {
        GPIO_OutputClearPin(port, pin);
    }
}

void Set_Truth_RGB(ItemColour_t truth_colour) {
    Pin_Set_If(GPIOA, 0, truth_colour == RED);
    Pin_Set_If(GPIOA, 1, truth_colour == GREEN);
    Pin_Set_If(GPIOA, 4, truth_colour == BLUE);
}

void Set_Sensor_RGB(ItemColour_t sensor_colour) {
    Pin_Set_If(GPIOA, 3, sensor_colour == RED);
    Pin_Set_If(GPIOA, 11, sensor_colour == GREEN);
    Pin_Set_If(GPIOA, 8, sensor_colour == BLUE);
}

void Set_Fault(PinState st) {
    Pin_Set_If(GPIOA, 9, st == PIN_HIGH);
}



ItemColour_t Read_Truth_RGB() {
    if (GPIO_InputReadPin(GPIOA, 0)) return RED;
    if (GPIO_InputReadPin(GPIOA, 1)) return GREEN;
    if (GPIO_InputReadPin(GPIOA, 4)) return BLUE;
    return NONE;
}

ItemColour_t Read_Sensor_RGB() {
    if (GPIO_InputReadPin(GPIOA, 3)) return RED;
    if (GPIO_InputReadPin(GPIOA, 11)) return GREEN;
    if (GPIO_InputReadPin(GPIOA, 8)) return BLUE;
    return NONE;
}

void Gate1_SetWidth(uint16_t width_ms) {
    TIM3->CCR2 = width_ms;
}

void Gate2_SetWidth(uint16_t width_ms) {
    TIM3->CCR3 = width_ms;
}