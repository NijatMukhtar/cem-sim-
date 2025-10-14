#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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

static float wrap360(float a){ float x=fmodf(a,360.0f); return x<0? x+360.0f : x; }
static float angdiff_abs(float a, float b){ float d=fabsf(wrap360(a)-wrap360(b)); return (d>180.f)? 360.f-d : d; }

static void activate_EM(int idx){ (void)idx; /* TODO: HAL set pin[idx] */ }
static void deactivate_EM(int idx){ (void)idx; /* TODO: HAL reset pin[idx] */ }

static int compute_active(const CemConfig* cfg, float piece_abs_deg, int* out_list){
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

static void update_EM_state(const CemConfig* cfg, CemState* st){
    
    float piece_abs = wrap360(st->rotor_deg + (cfg->piece_angle_css_deg - 90.0f) + cfg->phase_offset_deg);

    int n = compute_active(cfg, piece_abs, st->active_idx);
    (void)n; 

    
    printf("{\"t\":%.3f,\"disk_angle\":%.1f,\"piece_angle\":%.1f,\"rpm\":%.1f,\"active\":[",
           st->t_s, st->rotor_deg, piece_abs, cfg->rotor_speed_rpm);
    for(int i=0;i<n;i++) printf("%s%d", (i?",":""), st->active_idx[i]);
    printf("]}\n");
    fflush(stdout);
}

static void rstrip(char* s){
    int n=(int)strlen(s);
    while(n>0 && (s[n-1]=='\n'||s[n-1]=='\r'||isspace((unsigned char)s[n-1]))) s[--n]=0;
}

int main(void){
    CemConfig cfg = {
        .num_EM=4, .rotor_angle_deg=0.0f, .rotor_speed_rpm=50.0f, .direction=+1,
        .mode=2, .step_time_s=0.010f, .overlap_deg=20.0f, .phase_offset_deg=0.0f,
        .piece_angle_css_deg=90.0f, .timing_table_deg=NULL
    };
    float table[MAX_EM]; cfg.timing_table_deg=table;
    for(int i=0;i<cfg.num_EM;i++) table[i]=(360.0f/cfg.num_EM)*i; // 0°=top

    CemState st={ .t_s=0.0f, .rotor_deg=wrap360(cfg.rotor_angle_deg) };

    fprintf(stderr, ">> Ready. Commands: set ..., run <sec>  (e.g., 'set rpm 120', 'run 2')\n");

    char line[512];
    while(fgets(line,sizeof(line), stdin)){
        rstrip(line);
        if(strncmp(line,"set ",4)==0){
            char key[64], val[384]={0};
            if(sscanf(line+4,"%63s %383[^\n]", key, val)==2){
                if(strcmp(key,"n")==0){ int n=atoi(val); if(n<1)n=1; if(n>MAX_EM)n=MAX_EM; cfg.num_EM=n; }
                else if(strcmp(key,"rpm")==0){ cfg.rotor_speed_rpm=strtof(val,NULL); }
                else if(strcmp(key,"step_ms")==0){ cfg.step_time_s=strtof(val,NULL)/1000.0f; }
                else if(strcmp(key,"dir")==0){ cfg.direction=atoi(val)>=0?+1:-1; }
                else if(strcmp(key,"mode")==0){ cfg.mode=atoi(val)==2?2:1; }
                else if(strcmp(key,"overlap")==0){ cfg.overlap_deg=strtof(val,NULL); }
                else if(strcmp(key,"phase")==0){ cfg.phase_offset_deg=strtof(val,NULL); }
                else if(strcmp(key,"piece_css")==0){ cfg.piece_angle_css_deg=strtof(val,NULL); }
                else if(strcmp(key,"angles")==0){
                    int i=0; char *p=strtok(val,",");
                    while(p && i<cfg.num_EM){ cfg.timing_table_deg[i++]=wrap360(strtof(p,NULL)); p=strtok(NULL,","); }
                    for(; i<cfg.num_EM; i++) cfg.timing_table_deg[i]=wrap360((360.0f/cfg.num_EM)*i);
                }
                fprintf(stderr, ">> ok\n");
            } else fprintf(stderr, ">> syntax: set <key> <value>\n");
        }else if(strncmp(line,"run ",4)==0){
            float seconds=strtof(line+4,NULL); if(seconds<=0){ fprintf(stderr,">> run: seconds>0\n"); continue; }
            int steps=(int)ceilf(seconds/cfg.step_time_s);
            for(int k=0;k<steps;k++){
                float deg_per_s=cfg.rotor_speed_rpm*360.0f/60.0f;
                st.t_s+=cfg.step_time_s;
                st.rotor_deg=wrap360(st.rotor_deg + cfg.direction*deg_per_s*cfg.step_time_s);
                update_EM_state(&cfg,&st);
            }
            fprintf(stderr, ">> done (ran %.2fs)\n", seconds);
        }else if(line[0]==0){
            
        }else{
            fprintf(stderr, ">> unknown cmd: %s\n", line);
        }
    }
    return 0;
}
