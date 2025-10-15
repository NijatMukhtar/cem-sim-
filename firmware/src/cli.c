#include "cem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

static void rstrip(char* s){
    int n=(int)strlen(s);
    while(n>0 && (s[n-1]=='\n'||s[n-1]=='\r'||isspace((unsigned char)s[n-1]))) s[--n]=0;
}

void repl_loop(CemConfig* cfg){
    static float table[MAX_EM];
    cfg->timing_table_deg = table;
    for(int i=0;i<cfg->num_EM;i++) table[i]=(360.0f/cfg->num_EM)*i;
    CemState st = { .t_s=0.0f, .rotor_deg=wrap360(cfg->rotor_angle_deg) };

    fprintf(stderr, ">> Ready. Commands: set ..., run <sec>\n");
    char line[512];
    while(fgets(line,sizeof(line), stdin)){
        rstrip(line);
        if(strncmp(line,"set ",4)==0){
            char key[64], val[384]={0};
            if(sscanf(line+4,"%63s %383[^\n]", key, val)==2){
                if(strcmp(key,"n")==0){ int n=atoi(val); if(n<1)n=1; if(n>MAX_EM)n=MAX_EM; cfg->num_EM=n; }
                else if(strcmp(key,"rpm")==0){ cfg->rotor_speed_rpm=strtof(val,NULL); }
                else if(strcmp(key,"step_ms")==0){ cfg->step_time_s=strtof(val,NULL)/1000.0f; }
                else if(strcmp(key,"dir")==0){ cfg->direction=atoi(val)>=0?+1:-1; }
                else if(strcmp(key,"mode")==0){ cfg->mode=atoi(val)==2?2:1; }
                else if(strcmp(key,"overlap")==0){ cfg->overlap_deg=strtof(val,NULL); }
                else if(strcmp(key,"phase")==0){ cfg->phase_offset_deg=strtof(val,NULL); }
                else if(strcmp(key,"piece_css")==0){ cfg->piece_angle_css_deg=strtof(val,NULL); }
                fprintf(stderr, ">> ok\n");
            } else fprintf(stderr, ">> syntax: set <key> <value>\n");
        }else if(strncmp(line,"run ",4)==0){
            float seconds=strtof(line+4,NULL); if(seconds<=0){ fprintf(stderr,">> run: seconds>0\n"); continue; }
            int steps=(int)ceilf(seconds/cfg->step_time_s);
            for(int k=0;k<steps;k++){
                float deg_per_s=cfg->rotor_speed_rpm*360.0f/60.0f;
                st.t_s+=cfg->step_time_s;
                st.rotor_deg=wrap360(st.rotor_deg + cfg->direction*deg_per_s*cfg->step_time_s);
                update_EM_state(cfg,&st);
            }
            fprintf(stderr, ">> done (ran %.2fs)\n", seconds);
        }else if(line[0]==0){
            /* ignore */
        }else{
            fprintf(stderr, ">> unknown cmd: %s\n", line);
        }
    }
}
