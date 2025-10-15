#include "cem_scheduler.h"

int main(void) {
    CemConfig cfg = {
        .num_EM = 4,
        .rotor_angle_deg = 0.0f,
        .rotor_speed_rpm = 60.0f,
        .direction = +1,
        .mode = 2,
        .step_time_s = 0.05f,
        .overlap_deg = 20.0f,
        .phase_offset_deg = 0.0f,
        .piece_angle_css_deg = 90.0f,
        .timing_table_deg = NULL
    };

    float table[MAX_EM];
    cfg.timing_table_deg = table;
    for (int i = 0; i < cfg.num_EM; i++)
        table[i] = (360.0f / cfg.num_EM) * i;

    cem_run_realtime(&cfg, 2.0f); // 2 second simulation
    return 0;
}
