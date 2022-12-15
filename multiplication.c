#include <stdio.h>
#include <string.h>
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
    printf("Multiplication process: output %f\n", res);
}
double Receiver2() {
int t, q, n;
double rec_msg,rec_msg1;
struct msgbuf buf;
struct msqid_ds d;
t = ftok(".",0x24);
q = msgget( t, 0666 );
n = msgrcv( q, &buf, sizeof(struct msgbuf)-sizeof(long), TYPE, MSG_NOERROR | IPC_NOWAIT );
rec_msg = buf.msg[0];
rec_msg1 = buf.msg[1];
printf("Received in sum, message=%f, %f\n", rec_msg, rec_msg1);
n = msgctl( q, IPC_RMID, NULL);
double res = rec_msg*rec_msg1;
return res;
}

double Sender1(double snd_msg) {
struct msgbuf buf;
int q;
key_t t;
t = ftok(".",0x24);
q = msgget( t, 0666 | IPC_CREAT);
buf.type = TYPE;
buf.msg[0] = snd_msg;
msgsnd( q, &buf, sizeof(struct msgbuf)-sizeof(long), 0);
printf("Sended from mult: %f \n",snd_msg);
} 

void sigint_handler(int sig) {
        

     double res = Receiver2();

        Output(res);

	Sender1(res);
	
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

