#include "cem.h"
#include <stdio.h>
#include <math.h>

/* small helpers */
float wrap360(float a){ float x=fmodf(a,360.0f); return x<0? x+360.0f : x; }
float angdiff_abs(float a, float b){ float d=fabsf(wrap360(a)-wrap360(b)); return (d>180.f)? 360.f-d : d; }

int compute_active(const CemConfig* cfg, float piece_abs_deg, int* out_list){
    int count=0;
    for(int i=0;i<cfg->num_EM;i++){
        float diff = angdiff_abs(piece_abs_deg, cfg->timing_table_deg[i]);
        if(diff <= cfg->overlap_deg*0.5f){
            out_list[count++] = i;
            if(cfg->mode==2) out_list[count++] = (i+1) % cfg->num_EM;
        }
    }
    for(int i=0;i<count;i++){
        for(int j=i+1;j<count;j++){
            if(out_list[i]==out_list[j]){
                for(int k=j;k<count-1;k++) out_list[k]=out_list[k+1];
                count--; j--;
            }
        }
    }
    out_list[count] = -1;
    return count;
}

void update_EM_state(const CemConfig* cfg, CemState* st){
    float piece_abs = wrap360(st->rotor_deg + (cfg->piece_angle_css_deg - 90.0f) + cfg->phase_offset_deg);
    int n = compute_active(cfg, piece_abs, st->active_idx);
    printf("{\"t\":%.3f,\"disk_angle\":%.1f,\"piece_angle\":%.1f,\"rpm\":%.1f,\"active\":[",
           st->t_s, st->rotor_deg, piece_abs, cfg->rotor_speed_rpm);
    for(int i=0;i<n;i++) printf("%s%d", (i?",":""), st->active_idx[i]);
    printf("]}\n");
    fflush(stdout);
}
