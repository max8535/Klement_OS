#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdio.h>
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

pid_t plus, minus, mul, div, sqr;

volatile int flag = 0;

void sigint_handler(int sig) {
    flag = sig;
}

void Sender2(double snd_msg, double snd_msg1) {
	struct msgbuf buf;
	int q;
	key_t t;
	t = ftok(".",0x24);
	q = msgget( t, 0666 | IPC_CREAT);
	buf.type = TYPE;
	buf.msg[0] = snd_msg;
	buf.msg[1] = snd_msg1;
	msgsnd( q, &buf, sizeof(struct msgbuf)-sizeof(long), 0);
	printf("Sended from main: %f %f \n",snd_msg,snd_msg1);
} 

double Receiver1() {
	int t, q, n;
	double rec_msg;
	struct msgbuf buf;
	struct msqid_ds d;
	t = ftok(".",0x24);
	q = msgget( t, 0666 );
	n = msgrcv( q, &buf, sizeof(struct msgbuf)-sizeof(long), TYPE, MSG_NOERROR |IPC_NOWAIT );
	rec_msg = buf.msg[0];
	printf("Received in main, message=%f\n", rec_msg);
	n = msgctl( q, IPC_RMID, NULL);
	return rec_msg;
}
double SendRecv(pid_t child, double x, double y){
    double res;
	Sender2(x,y);

    kill(child, SIGUSR1);
    pause();
    res=Receiver1();

    return res;
}

void MainWork(){

    double inputValue_1, inputValue_2, inputValue_3;
    double a, b, c, add, x1, x2, post;

    printf("Enter a, b, c: ");
    scanf("%lf", &inputValue_1);
    scanf("%lf", &inputValue_2);
    scanf("%lf", &inputValue_3);

    a = inputValue_1;
    b = inputValue_2; 
    c = inputValue_3;

    x2 = SendRecv(mul, b, b);
    post = 4;
    add = SendRecv(mul, post, a);
    add = SendRecv(mul, add, c);
    x2 = SendRecv(minus, x2, add);
    if(x2<0)
    {printf("No solution in real numbers\n"); }
    else{
    x2 = SendRecv(sqr, x2, x2);
    post = 0;
    add = SendRecv(minus, post, b);
    x1 = SendRecv(plus, add, x2);
    x2 = SendRecv(minus, add, x2);
    post = 2;
    add = SendRecv(mul, post, a);
    x1 = SendRecv(div, x1, add);
    x2 = SendRecv(div, x2, add);
     printf("X1=%.2f   X2=%.2f\n",x1,x2); 
     }
}

void End(){
   kill(plus, SIGINT);
   kill(minus, SIGINT);
   kill(mul, SIGINT);
   kill(div, SIGINT);
   kill(sqr, SIGINT);
}

void creatProcess(){
    plus = fork();
    if (plus){
        minus = fork();
        if (minus){
            mul = fork();
            if (mul){
                div = fork();
                if (div){
                    sqr = fork();
                    if (sqr){
                        MainWork();
                        End();
                    }
                    else{
                        execl("./sqrt", "", NULL);
                        perror("Error to start sqrt");
                    }
                }
                else{
                    execl("./division", "", NULL);
                    perror("Error to start division");
                }
            }
            else{
                execl("./multiplication", "", NULL);
                perror("Error to start multiplication");
            }
        }
        else{
            execl("./minus", "", NULL);
            perror("Error to start minus");
        }
    }
    else{
        execl("./plus", "", NULL);
        perror("Error to start plus");
    }
}


int main() {
    struct sigaction sa;
    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sa.sa_handler = sigint_handler;
    sa.sa_flags = 0;
    sa.sa_mask = set;
    sigaction(SIGUSR1, &sa, NULL);
    creatProcess();
}
