#include <modi.h>

enum ChromaticNote modi_period_to_note(u_int16_t period) {
    return MODI_A_SHARP;
}


uint8_t modi_period_to_octave(u_int16_t period) {
    return 4;
}
