#include "my_helpers.h"

void Set_Truth_RGB(ItemColour_t truth_colour) {
    Pin_Write(GPIOA, 0, truth_colour == 1);
    Pin_Write(GPIOA, 1, truth_colour == 2);
    Pin_Write(GPIOA, 4, truth_colour == 3);
}

void Set_Sensor_RGB(ItemColour_t sensor_colour) {
    Pin_Write(GPIOA, 3, sensor_colour == 1);
    Pin_Write(GPIOA, 11, sensor_colour == 2);
    Pin_Write(GPIOA, 8, sensor_colour == 3);
}

void Set_Fault(PinState st) {
    Pin_Write(GPIOA, 9, st);
}