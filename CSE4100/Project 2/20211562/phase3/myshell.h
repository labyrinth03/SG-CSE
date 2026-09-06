#define _POSIX_C_SOURCE 1
#include "csapp.h"
#include <errno.h>
#define MAXARGS   128
#define MAXJOBS   100

/* Function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv); 
void run_pipe(char *cmdline);                           // 파이프 처리 함수
void remove_quotes(char *str);                          // 따옴표 제거 함수

typedef enum { 
    FG,                 // Foreground
    BG,                 // Background
    ST                  // Stopped
} job_state;            // job 상태를 나타내는 enum

typedef struct {
    pid_t pid;              // pid
    char cmdline[MAXLINE];  // command line
    int running;            // 1: running, 0: stopped
    int jid;                // job id
    job_state state;        // job 상태
} job_t;                    // job 구조체

int next_jid = 1;           // next job id
pid_t fg_pid = 0;           // foreground process id

void sigchld_handler(int sig);                                          // 자식 프로세스 종료 시그널 핸들러
void sigint_handler(int sig);                                           // Ctrl+C 시그널 핸들러                        
void sigtstp_handler(int sig);                                          // Ctrl+Z 시그널 핸들러                     
void wait_fg(pid_t pid);                                                // foreground process가 끝날 때까지 대기
void add_job(pid_t pid, job_state state, const char *cmdline);          // job list에 job 추가
void delete_job(pid_t pid);                                             // job list에서 job 삭제                   
void update_job_state(pid_t pid, job_state state);                      // job list에서 job 상태 업데이트
int get_jid(pid_t pid);                                                 // job list에서 pid에 해당하는 job id 가져오기
pid_t get_foreground_pid();                                             // foreground process id 가져오기
void set_foreground_pid(pid_t pid);                                     // foreground process id 설정             
