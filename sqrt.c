#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#define TYPE 12345

struct msgbuf { long type; double msg[50];};


volatile int flag = 0;


void Output(double res){
    printf("Sqrt process: output %f\n", res);
}

void sigint_handler(int sig) {
        int t, q, n;
	double rec_msg;
	struct msgbuf buf;
	struct msqid_ds d;
	t = ftok(".",0x24);
	q = msgget( t, 0666 );
	n = msgrcv( q, &buf, sizeof(buf)-sizeof(long), TYPE, MSG_NOERROR | IPC_NOWAIT );
	rec_msg = buf.msg[0];
	printf("Received in sqrt, message=%f\n", rec_msg);
	n = msgctl( q, IPC_RMID, NULL); 

      double  res = sqrt(rec_msg);

        Output(res);

        struct msgbuf buf1;
	int q1;
	key_t t1;
	t1 = ftok(".",0x24);
	q1 = msgget( t1, 0666 | IPC_CREAT);
	buf1.type = TYPE;
	buf1.msg[0] = res;
	msgsnd( q1, &buf1, sizeof(buf1)-sizeof(long), 0);
	printf("Sended from mult: %f \n",res); 

        flag = sig;
        kill(getppid(), SIGUSR1);
}

int main() {
    struct sigaction sa;
    sigset_t set;

    sigemptyset(&set); // Clear set of signals
    sigaddset(&set, SIGUSR1); // Add signal to set
    sa.sa_handler = sigint_handler;
    sa.sa_flags = 0;
    sa.sa_mask = set;
    sigaction(SIGUSR1, &sa, NULL);
    while(1)
        pause();
}

