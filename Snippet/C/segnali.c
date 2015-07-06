#define SIGHUP 1 /* Hangup (POSIX). Action: exit */
#define SIGINT 2 /* Interrupt (ANSI). Action: exit */
#define SIGQUIT 3 /* Quit (POSIX). Action: exit, core dump*/
#define SIGILL 4 /* Illegal instr.(ANSI).Action: exit,core dump */

#define SIGKILL 9 /* Kill, unblockable (POSIX). Action: exit*/
#define SIGUSR1 10 /* User-defined signal 1 (POSIX). Action: exit*/
#define SIGSEGV 11 /* Segm. violation (ANSI). Act: exit,core dump */
#define SIGUSR2 12 /* User-defined signal 2 (POSIX).Act: exit */
#define SIGPIPE 13 /* Broken pipe (POSIX).Act: exit */
#define SIGALRM 14 /* Alarm clock (POSIX). Act: exit */
#define SIGTERM 15 /* Termination (ANSI). Act:exit*/

#define SIGCHLD 17 /* Child status changed (POSIX).Act: ignore */
#define SIGCONT 18 /* Continue (POSIX).Act. ignore */
#define SIGSTOP 19 /* Stop, unblockable (POSIX). Act: stop */