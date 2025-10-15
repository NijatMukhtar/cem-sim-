#include "cem_scheduler.h"
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <sys/select.h>  // select()
#include <termios.h>     // terminal ayarları
#include <fcntl.h>       // fcntl()
#include <string.h>
#include <stdlib.h>



static void cem_step(CemConfig* cfg, CemState* st) {
    float deg_per_s = cfg->rotor_speed_rpm * 360.0f / 60.0f;
    st->t_s += cfg->step_time_s;
    st->rotor_deg = wrap360(st->rotor_deg + cfg->direction * deg_per_s * cfg->step_time_s);
    update_EM_state(cfg, st);
}

// stdin’i non-blocking moda al
static void set_nonblocking_input(void) {
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

static int check_user_input(char* buf, size_t len) {
    fd_set set;
    struct timeval timeout = {0, 0}; // 0 saniye bekle (polling)
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    int rv = select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout);
    if (rv > 0 && FD_ISSET(STDIN_FILENO, &set)) {
        if (fgets(buf, len, stdin)) return 1;
    }
    return 0;
}

void cem_run_realtime(CemConfig* cfg, float duration_s) {
    CemState st = { .t_s = 0.0f, .rotor_deg = wrap360(cfg->rotor_angle_deg) };
    int steps = (int)ceilf(duration_s / cfg->step_time_s);

    fprintf(stderr, ">> Starting realtime simulation (%.2fs, %.1f ms step)\n",
            duration_s, cfg->step_time_s * 1000.0f);

    set_nonblocking_input();

    for (int k = 0; k < steps; k++) {
        cem_step(cfg, &st);

        // Kullanıcı girdisini kontrol et
        char line[128];
        if (check_user_input(line, sizeof(line))) {
            // Gereksiz newline’ı temizle
            line[strcspn(line, "\r\n")] = 0;

            if (strcmp(line, "stop") == 0) {
                fprintf(stderr, ">> Simulation stopped by user\n");
                break;
            } else if (strncmp(line, "rpm ", 4) == 0) {
                float new_rpm = strtof(line + 4, NULL);
                cfg->rotor_speed_rpm = new_rpm;
                fprintf(stderr, ">> RPM updated to %.1f\n", new_rpm);
            } else if (strcmp(line, "dir") == 0) {
                cfg->direction *= -1;
                fprintf(stderr, ">> Direction reversed\n");
            } else {
                fprintf(stderr, ">> Unknown cmd: %s\n", line);
            }
        }

        usleep((useconds_t)(cfg->step_time_s * 1e6));
    }

    fprintf(stderr, ">> Simulation finished\n");
}
