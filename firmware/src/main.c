#include "cem.h"

void repl_loop(CemConfig* cfg);

int main(void){
    CemConfig cfg = {
        .num_EM=4, .rotor_angle_deg=0.0f, .rotor_speed_rpm=50.0f, .direction=+1,
        .mode=2, .step_time_s=0.010f, .overlap_deg=20.0f, .phase_offset_deg=0.0f,
        .piece_angle_css_deg=90.0f, .timing_table_deg=NULL
    };
    repl_loop(&cfg);
    return 0;
}
