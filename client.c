#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <signal.h>

#define MSGQ_KEY 1234
#define RQS 1
#define RSP 2 
#define RQS_SIZE (sizeof(struct request)-sizeof(long))
#define RSP_SIZE (sizeof(struct response)-sizeof(long))
#define MAXDATA 20

static void handler(int sig);

struct request
{
    long type;             // RQS
    char data[MAXDATA];    // message
    pid_t pid;
};

struct response
{ 
    long type;
    int word_length, number_of_tries;                                      
    char word[MAXDATA];
    pid_t pid;                            
};

int main(int argc, char* argv[])
{
    int mqid, shmid, flag;
    int i=0;
    char letter;
    char *p, *space_line;
    pid_t ppid, pid;
    struct request req;                            
    struct response rsp; 
    struct sigaction act = { 0 };

    act.sa_handler = handler;
    act.sa_flags = SA_RESTART;
    sigaction(SIGCONT, &act, NULL);

    pid = getpid();
    printf("Client's pid: %d\n", pid);
//-----------------------------------------------find messsage queue----------------------------------------------------//
    mqid = msgget(MSGQ_KEY, 0);
    if(mqid==-1)
    {
        perror("first msgget:");
        exit(1);
    }
//------------------------------------------------send hi to the server------------------------------------------------//
    req.type = RQS;
    strcpy(req.data, "hi!");
    req.pid = pid;
    msgsnd(mqid, &req, RQS_SIZE, 0 );
    printf("Welcome!\n");
//-----------------------------------------receive words details and print them---------------------------------------//
    msgrcv(mqid, &rsp, RSP_SIZE, 0, 0); 

    printf("\n---------------------\n");
    printf("%s\n", rsp.word);
    printf("---------------------\n");
    printf("Number of tries: %d\n", rsp.number_of_tries);
    
    msgctl(mqid, IPC_RMID, NULL);
//---------------------------------------------find the shared memory and connect-------------------------------------//
    shmid = shmget(MSGQ_KEY, 0, 0);
    if(shmid ==-1)
    {
        perror("shmget:");
        exit(1);
    }
    p = shmat(shmid, NULL, 0);
    if(*p ==-1) 
    {
        printf("no memory connected.\n");
        exit(1);
    }
//------------------------------------------------guess letters-------------------------------------------------------//
    while(rsp.number_of_tries!=0)
    {
        printf("Enter a letter: ");
        scanf(" %c", &letter);

        strcpy(p,&letter);
        kill(rsp.pid, SIGCONT);
        pause();

        flag = 0;
        for(i=0; i<=rsp.word_length; i++)
        {
            if(p[i] == '-')
            {
                flag = 1;
                break;
            }
        }
    
        printf("\n---------------------\n");
        printf("%s\n", p);
        printf("---------------------\n");
        rsp.number_of_tries--;
        
        if(flag==0)
        {
            printf("CONGRATULATIONS!\nYOU WON!\n");
            break;
        }
        else if(rsp.number_of_tries == 0)
            printf("SORRY :(\nYOU LOST...\n");
        else
            printf("Number of tries: %d\n", rsp.number_of_tries);   
    }
//--------------------------------------------delete shared memory----------------------------------------------------//    
    shmdt(p);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
//------------------------------------------SIGNAL HANDLER FUNCTION-----------------------------------------------------//
static void handler(int sig)
{}