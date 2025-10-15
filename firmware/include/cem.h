#ifndef CEM_H
#define CEM_H

#include <stdint.h>
#include <stddef.h>

#define MAX_EM 64

typedef struct {
    int    num_EM;
    float  rotor_angle_deg;
    float  rotor_speed_rpm;
    int    direction;
    int    mode;
    float  step_time_s;
    float  overlap_deg;
    float  phase_offset_deg;
    float  piece_angle_css_deg;
    float  *timing_table_deg;
} CemConfig;

typedef struct {
    float t_s;
    float rotor_deg;
    int   active_idx[MAX_EM];
} CemState;

float wrap360(float a);
float angdiff_abs(float a, float b);
int compute_active(const CemConfig* cfg, float piece_abs_deg, int* out_list);
void update_EM_state(const CemConfig* cfg, CemState* st);

#endif /* CEM_H */
