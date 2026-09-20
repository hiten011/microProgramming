typedef enum {
    OFF = 0,
    RED,
    GREEN,
    BLUE
} ItemColour_t;

ItemColour_t Apply_LFSR_Fault(ItemColour_t true_col);
void Generate_Next_Item(void);
void SysTick_Init(void);