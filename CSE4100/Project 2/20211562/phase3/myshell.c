#include "myshell.h"


char command_addr[MAXLINE]; // command address
job_t job_list[MAXJOBS];    // job list
int next_job = 0;           // next job id

/* $begin shellmain */
int main() 
{
    char cmdline[MAXLINE];                                  // Command line 
    char *temp[MAXLINE];                                    // temporary buffer to delete error message

    Signal(SIGINT, sigint_handler);                         // 각 종류의 시그널에 대한 핸들러 설정
    Signal(SIGTSTP, sigtstp_handler);
    Signal(SIGCHLD, sigchld_handler);

    while (1) {                                             // 쉘 반복문
	/* Read */
        command_addr[0] = '\0';                             // Initialize command_addr
        temp[0] = NULL;                                     // Initialize temp buffer
	    printf("CSE4100-SP-P2> ");                          // shell prompt
        
	    temp[0] = fgets(cmdline, MAXLINE, stdin);           // Read line
	    if (feof(stdin))                                    // EOF만나면 terminate
	        exit(0);

	    /* Evaluate */
	    eval(cmdline);                                      // evaluate
    } 
}
/* $end shellmain */
  
/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline) 
{
    char *argv[MAXARGS];                                    /* Argument list execve() */
    char buf[MAXLINE];                                      /* Holds modified command line */
    int bg;                                                 /* Should the job run in bg or fg? */
    pid_t pid;                                              /* Process id */
    
    strcpy(buf, cmdline);                                   // 버퍼에 명령어 복사
    bg = parseline(buf, argv);                              // 버퍼에 들어있는 명령어 parsing해서 background 실행 명령어인지 flag set

    if (argv[0] == NULL)                        
	    return;                                             /* Ignore empty lines */

    if(strchr(cmdline, '|')){                               // pipe | 함수 처리
        run_pipe(cmdline);                                  // pipe 처리 함수 호출
        return;                                             // 이미 실행했으니 return
    }

    if (!builtin_command(argv)) {                           // quit -> exit(0), & -> ignore, other -> run
        sigset_t mask_all, prev_all;                        // mask_all은 차단하고 싶은 sig 목록, prev_all은 원래 sig(복구용)
        Sigemptyset(&mask_all);                             // 모든 sig 초기화
        Sigaddset(&mask_all, SIGCHLD);                      // SIGCHLD 추가

        Sigprocmask(SIG_BLOCK, &mask_all, &prev_all);       // block SIGCHLD하면서 이전의 sig 상태 저장

        if((pid = Fork()) == 0){                                // 자식 프로세스 생성
            Setpgid(0, 0);                                      // 새 process group 설정
            Sigprocmask(SIG_SETMASK, &prev_all, NULL);          // unblock

            if (execvp(argv[0], argv) < 0) {	                // ex) /bin/ls ls -al &
                printf("%s: Command not found.\n", argv[0]);    // error message
                exit(0);                                        // 나가기
            }
        }

        Sigprocmask(SIG_BLOCK, &mask_all, NULL);                // block SIGCHLD
        /* Parent waits for foreground job to terminate */
        if (!bg){                                               // foreground process
            set_foreground_pid(pid);                            // 전역 fg_pid 설정
            add_job(pid, FG, cmdline);                          // job list에 등록
            Sigprocmask(SIG_SETMASK, &prev_all, NULL);          // unblock
            wait_fg(pid);                                       // foreground 프로세스 끝날 때까지 대기
        } else {                                                // background process
            add_job(pid, BG, cmdline);                          // job list에 등록
            printf("[%d] %d %s", get_jid(pid), pid, cmdline);   // 실행되는 background job 출력
            usleep(100000);                                     // 프롬프트 정상 출력 순서를 위한 sleep
            Sigprocmask(SIG_SETMASK, &prev_all, NULL);          // unblock
        }

    }
    return;
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv)                                // builtin command는 run하고 true 리턴
{
    if (!strcmp(argv[0], "quit"))                               /* quit command */
        exit(0);                                                // exit
    if (!strcmp(argv[0], "exit"))                               /* exit command */
        exit(0);                                                // exit
    if (!strcmp(argv[0], "&"))                                  /* Ignore singleton & */
        return 1;
     if (!strcmp(argv[0], "fg")){                               // fg command
        if (argv[1] == NULL || argv[1][0] != '%') {             // 잘못된 명령어 입력
            printf("Usage: fg %%<jobid>\n");                    // 올바른 명령어 예시 출력
            return 1;                                           // 실행 처리는 했으니 1 리턴
        }
        int jid = atoi(&argv[1][1]);                            // job id 저장
        for (int i = 0; i < MAXJOBS; i++) {
            if (job_list[i].jid == jid && job_list[i].pid != 0) {   // 찾는 job id가 job list에 존재하면
                job_list[i].state = FG;                             // job list의 상태를 FG로 변경
                set_foreground_pid(job_list[i].pid);                // foreground process로 바꿨으니 전역 fg_pid 설정
                Kill(-job_list[i].pid, SIGCONT);                    // process group 전체에 SIGCONT 시그널 보내기
                wait_fg(job_list[i].pid);                           // foreground process가 끝날 때까지 대기
                return 1;
            }
        }
        printf("fg: No such job\n");                                // job id가 존재하지 않으면 에러 메시지 출력
        return 1;
     }  /* fg command */
    if (!strcmp(argv[0], "bg")){                                                    // bg command
        if (argv[1] == NULL || argv[1][0] != '%') {                                 // 잘못된 명령어 입력
            printf("Usage: bg %%<jobid>\n");                                        // 올바른 명령어 예시 출력
            return 1;
        }
        int jid = atoi(&argv[1][1]);                                                // job id 저장
        for (int i = 0; i < MAXJOBS; i++) { 
            if (job_list[i].jid == jid && job_list[i].pid != 0) {                   // 찾는 job id가 job list에 존재하면
                job_list[i].state = BG;                                             // job list의 상태를 BG로 변경
                Kill(-job_list[i].pid, SIGCONT);                                    // process group 전체에 SIGCONT 시그널 보내기
                printf("[%d] (%d) %s", jid, job_list[i].pid, job_list[i].cmdline);  // background job 출력
                return 1;
            }
        }
        printf("bg: No such job\n");                                                // job id가 존재하지 않으면 에러 메시지 출력
        return 1;
    }   /* bg command */
    if (!strcmp(argv[0], "kill")){                                // kill command   
        if (argv[1] == NULL || argv[1][0] != '%') {               // 잘못된 명령어 입력
            printf("Usage: kill %%<jobid>\n");                    // 올바른 명령어 예시 출력
            return 1;
        }
        int jid = atoi(&argv[1][1]);                                // job id 저장
        for (int i = 0; i < MAXJOBS; i++) {                         
            if (job_list[i].jid == jid && job_list[i].pid != 0) {   // 찾는 job id가 job list에 존재하면
                Kill(-job_list[i].pid, SIGKILL);                    // process group 전체에 SIGKILL 시그널 보내기
                return 1;
            }
        }
        printf("kill: No such job\n");                              // job id가 존재하지 않으면 에러 메시지 출력
        return 1;
    } /* kill command */
    if (!strcmp(argv[0], "cd")) {                                   /* cd command */
        if (argv[1] == NULL) {                                      // cd command에 parameter가 없으면
            fprintf(stderr, "cd: missing argument\n");              // 에러 메시지 출력
            return 1;
        }
        if (chdir(argv[1]) < 0) {                                   // cd command가 비정상적 수행되면
            fprintf(stderr, "cd: %s: No such file or directory\n", argv[1]);    // 에러 메시지 출력
        }
        return 1;
    }
    if (!strcmp(argv[0], "jobs")) {                                         // jobs command
        for (int i = 0; i < MAXJOBS; i++) {
            if (job_list[i].pid != 0) {                                     // job list에 pid가 존재하면
                char *state_str;
                if (job_list[i].state == BG) state_str = "Running";         // background 실행중
                else if (job_list[i].state == FG) state_str = "Foreground"; // foreground 실행중
                else if (job_list[i].state == ST) state_str = "Stopped";    // stopped
                else state_str = "Unknown";                                 // 예외 상황
                printf("[%d] (%d) %s %s", job_list[i].jid, job_list[i].pid, state_str, job_list[i].cmdline);        //상태 출력
            }
        }
        return 1;
    }
    return 0;                     /* Not a builtin command */
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv) 
{
    char *delim;         /* Points to first space delimiter */
    int argc;            /* Number of args */
    int bg;              /* Background job? */

    buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* Ignore leading spaces */
	buf++;

    /* Build the argv list */
    argc = 0;
    while ((delim = strchr(buf, ' '))) {
	argv[argc++] = buf;
	*delim = '\0';
	buf = delim + 1;
	while (*buf && (*buf == ' ')) /* Ignore spaces */
            buf++;
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* Ignore blank line */
	return 1;


    for (int i = 0; i < argc; i++) {                // 따옴표 제거 추가(grep 때문에)
        remove_quotes(argv[i]);
    }

    /* Should the job run in the background? */
    int last = argc - 1;
    int len = strlen(argv[last]);
    if (len > 0 && argv[last][len - 1] == '&') {
        bg = 1;
        if (len == 1) {
            // 인자가 &인 경우
            argv[last] = NULL;
        } else {
            // 인자가 "ls&"처럼 붙어 있는 경우 → & 제거
            argv[last][len - 1] = '\0';
        }
        argc--; // & 제거 후 인자 수 감소
    }

    return bg;
}
/* $end parseline */

void run_pipe(char *cmdline) {                                              // 파이프 처리 함수
    int fd[2];                                                              // 파이프 fd
    pid_t pid;                                                              // 자식 프로세스 pid
    char *left_cmd, *right_cmd;                                             // | 기준 왼쪽 명령어, 오른쪽 명령어

    left_cmd = strtok(cmdline, "|");                                        // 왼쪽 명령어
    right_cmd = strtok(NULL, "");                                           // 오른쪽 명령어

    if (right_cmd == NULL) {                                                // 파이프로 끝났으면
        fprintf(stderr, "Syntax error: missing command after pipe\n");      // 에러 메시지 출력
        return;
    }

    if (pipe(fd) < 0) {                                                     // 파이프 생성
        perror("pipe error");                                               // 파이프 생성 실패 시 에러 메세지 출력
        return;
    }

    if ((pid = fork()) == 0) {                                              // 자식 프로세스: 왼쪽 명령 실행
        Setpgid(0, 0);                                                      // 새 process group 설정
        close(fd[0]);                                                       // 읽기 끝
        dup2(fd[1], STDOUT_FILENO);                                         // 표준 출력에 파이프의 쓰기 끝을 연결
        close(fd[1]);                                                       // 쓰기 끝                          

        char *argv[MAXARGS];                                                // 왼쪽 명령어 담을 배열
        parseline(left_cmd, argv);                                          // 왼쪽 명령어 parsing

        if (argv[0] == NULL) {                                              // 왼쪽 명령어가 없으면
            fprintf(stderr, "Syntax error: missing command before pipe\n");
            exit(1);
        }

        if (builtin_command(argv)) {                                        // 왼쪽 명령어가 builtin command이면
            exit(0);                                                        // 실행하고 exit
        }

        execvp(argv[0], argv);                                              // 왼쪽 명령어 실행
        perror("exec failed");                                              // exec 실패 시 에러 메세지 출력
        exit(1);                                                            
    }

    
    close(fd[1]);                                                           // 쓰기 끝
    int saved_stdin = dup(STDIN_FILENO);                                    // 현재 stdin 저장
    dup2(fd[0], STDIN_FILENO);                                              // 읽기 끝을 표준 입력으로 연결
    close(fd[0]);                                                           // 읽기 끝 닫기         

    int status;
    waitpid(pid, &status, 0);                                               // 자식 프로세스가 종료될 때까지 대기

    if (strchr(right_cmd, '|')) {                                           // 오른쪽 명령어에 또 파이프가 있으면 재귀적으로 처리
        run_pipe(right_cmd);                                                // 오른쪽 명령어에 대해 파이프 재귀적 처리
    } else {                                                                // 더 이상 오른쪽 파이프가 더 없으면
        char *argv[MAXARGS];
        parseline(right_cmd, argv);                                         // 오른쪽 명령어 parsing

        if (argv[0] == NULL) {                                              // 오른쪽 명령어가 없으면
            fprintf(stderr, "Syntax error: missing command after pipe\n");  // 에러 메시지 출력
            return;
        }

        if (builtin_command(argv)) {                                        // 오른쪽 명령어가 builtin command이면
            dup2(saved_stdin, STDIN_FILENO);                                // 원래 표준 입력 복구
            close(saved_stdin);                                             // 닫기
            return;
        }

        pid = fork();                                                       // 자식 프로세스 생성
        if (pid == 0) {                                                     // 자식 프로세스에서 마지막 명령 실행
            execvp(argv[0], argv);                                          // 오른쪽 명령어 실행
            perror("exec failed");                                          // exec 실패 시 에러 메세지 출력
            exit(1);                                                        // 자식 프로세스 종료
        } else {                                                            // 부모 프로세스
            waitpid(pid, &status, 0);                                       // 자식 프로세스가 종료될 때까지 대기
        }
    }

    dup2(saved_stdin, STDIN_FILENO);                                        // 원래 표준 입력 복구
    close(saved_stdin);                                                     // 닫기         
}

void remove_quotes(char *str) {                                             // 따옴표 제거 함수(grep에서 사용)
    char *src = str, *dst = str;                                            // src는 원본 문자열, dst는 따옴표 제거된 문자열
    while (*src) {                                                          // src가 NULL이 아닐 때까지 반복
        if (*src != '"' && *src != '\'') {                                  // 따옴표가 아니면
            *dst++ = *src;                                                  // dst에 src의 값을 복사
        }
        src++;                                                              // src 포인터 이동  
    }
    *dst = '\0';                                                            // 문자열 종료
}

pid_t get_foreground_pid() {                                                // fg_pid 리턴
    return fg_pid;                                                        
}

void set_foreground_pid(pid_t pid) {                                        // fg_pid 설정
    fg_pid = pid;
}

void sigint_handler(int sig) {                                              // Ctrl+C(SIGINT) 시그널 핸들러
    pid_t pid = get_foreground_pid();                                       // foreground process pid 가져오기
    if (pid > 0) {                                                          // foreground process가 존재하면
        Kill(-pid, SIGINT);                                                 // -pid: process group 전체에 보냄
    }
}

void sigtstp_handler(int sig) {                                             // Ctrl+Z(SIGTSTP) 시그널 핸들러
    pid_t pid = get_foreground_pid();                                       // foreground process pid 가져오기
    if (pid > 0) {                                                          // foreground process가 존재하면
        Kill(-pid, SIGTSTP);                                                // process group 전체 정지
    }
}

void sigchld_handler(int sig) {                                             // 자식 프로세스 종료 시그널 핸들러
    int status;                                                             // 종료 상태
    pid_t pid;                                                              // 자식 프로세스 pid

    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {         // 자식 프로세스가 종료되면
        if (WIFEXITED(status)) {                                            // 정상 종료        
            delete_job(pid);                                                // job list에서 삭제
            if (pid == fg_pid) {                                            // foreground process가 종료되면
                fg_pid = 0;                                                 // fg_pid 초기화
            } else {                                                        // background process가 종료되면
                delete_job(pid);                                            // job list에서 삭제
            }
        } 
        else if (WIFSIGNALED(status)) {                                     // 시그널에 의해 종료됨 (예: kill, Ctrl+C)
            int jid = get_jid(pid);                                         // job id 가져오기
            delete_job(pid);                                                // job list에서 삭제
            if (pid == fg_pid) {                                            // foreground process가 종료되면
                printf("\n");
                fg_pid = 0;                                                 // fg_pid 초기화
            }
        } 
        else if (WIFSTOPPED(status)) {                                      // 시그널에 의해 정지(예: Ctrl+Z)
            update_job_state(pid, ST);                                      // job list에서 상태 업데이트
            int jid = get_jid(pid);                                         // job id 가져오기
            for (int i = 0; i < MAXJOBS; i++) {                             
                if (job_list[i].pid == pid) {                               // job list에서 pid 찾기
                    printf("\n[%d] (%d) Stopped %s", job_list[i].jid, pid, job_list[i].cmdline);    // job list에서 pid 찾기
                    break;                                                                          
                }
            }
            if (pid == fg_pid) {                                            // foreground process가 정지되면
                fg_pid = 0;                                                 // stopped여도 기다림 종료
            }
        }
    }
}


void wait_fg(pid_t pid) {                                                   // foreground process가 끝날 때까지 대기
    while (get_foreground_pid() == pid) {                                   // foreground process가 존재하면
        sleep(1);                                                           // 또는 pause(), 또는 sigsuspend()
    }
}

void add_job(pid_t pid, job_state state, const char *cmdline) {             // job list에 job 추가
    for (int i = 0; i < MAXJOBS; i++) {
        if (job_list[i].pid == 0) {                                         // 빈 job list에 추가
            job_list[i].pid = pid;                                          // pid 저장
            job_list[i].jid = next_jid++;                                   // job id 저장
            job_list[i].state = state;                                      // 상태 저장
            strcpy(job_list[i].cmdline, cmdline);                           // 명령어 저장
            if (i >= next_job) next_job = i + 1;                            // 다음 job id 업데이트
            return;
        }
    }
    printf("Job list full!\n");                                             // job list가 가득 차면 에러 메시지 출력
}

int get_jid(pid_t pid) {                                                    // pid에 해당하는 job id 리턴(에러 시 0 리턴)
    for (int i = 0; i < MAXJOBS; i++) {
        if (job_list[i].pid == pid)                                         // pid에 해당하는 job id 찾기
            return job_list[i].jid;                                         // job id 리턴
    }
    return 0;                                                               // 존재하지 않으면 0 리턴
}

void delete_job(pid_t pid) {                                                // job list에서 pid에 해당하는 job 삭제
    for (int i = 0; i < MAXJOBS; i++) {
        if (job_list[i].pid == pid) {                                       // pid에 해당하는 job 찾기
            job_list[i].pid = 0;                                            // pid 초기화
            job_list[i].jid = 0;                                            // job id 초기화
            job_list[i].state = 0;                                          // 상태 초기화  
            job_list[i].cmdline[0] = '\0';                                  // 명령어 초기화
            return;
        }
    }
}
void update_job_state(pid_t pid, job_state state) {                         // job list에서 pid에 해당하는 job 상태 업데이트
    for (int i = 0; i < MAXJOBS; i++) {
        if (job_list[i].pid == pid) {                                       // pid에 해당하는 job 찾기
            job_list[i].state = state;                                      // 상태 업데이트
            return;                                                         // 성공적으로 업데이트
        }
    }
    return;                                                                 // pid에 해당하는 job이 없으면 아무것도 안함
}

