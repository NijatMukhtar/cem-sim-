#include "cem_scheduler.h"
#include <stdio.h>
#include <unistd.h>   // usleep()
#include <math.h>
#include <time.h>

static void cem_step(CemConfig* cfg, CemState* st) {
    float deg_per_s = cfg->rotor_speed_rpm * 360.0f / 60.0f;
    st->t_s += cfg->step_time_s;
    st->rotor_deg = wrap360(st->rotor_deg + cfg->direction * deg_per_s * cfg->step_time_s);
    update_EM_state(cfg, st);
}

void cem_run_realtime(CemConfig* cfg, float duration_s) {
    CemState st = { .t_s = 0.0f, .rotor_deg = wrap360(cfg->rotor_angle_deg) };
    int steps = (int)ceilf(duration_s / cfg->step_time_s);

    fprintf(stderr, ">> Starting realtime simulation for %.2f s (%.1f ms step)\n",
            duration_s, cfg->step_time_s * 1000.0f);

    for (int k = 0; k < steps; k++) {
        cem_step(cfg, &st);
        usleep((useconds_t)(cfg->step_time_s * 1e6));  // Realtime sleep
    }

    fprintf(stderr, ">> Simulation finished\n");
}
