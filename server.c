#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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

struct request 
{
    long type;                                    
    char data[MAXDATA];
    pid_t pid;                        
};

struct response
{ 
    long type;
    int word_length;
    int number_of_tries;                                      
    char word[MAXDATA];
    pid_t pid;                        
};

char** load_dict(char* filename);                  
char* pick_a_word(char** dictionary);  
static void handler(int sig);

int main(int argc, char* argv[])
{
    int i=0,flag=0, mqid, shmid;
    char* filename, *rand_word, *p;
    char** dictionary;
    pid_t ppid, pid;
    struct request req;                            
    struct response rsp;
    struct sigaction act = { 0 };

    act.sa_handler = handler;
    act.sa_flags = SA_RESTART;
    sigaction(SIGCONT, &act, NULL);

    ppid = getpid();
    printf("Server's pid: %d\n", ppid);
//------------------------------------------------load dictionary from file-------------------------------------------//
    srand(time(0));

    filename = (char*)malloc(sizeof(char)*20);
    printf("Give me the dictionary file: ");
    scanf("%s", filename);

    dictionary = load_dict(filename);
//-----------------------------------------------create message queue------------------------------------------------//
    mqid = msgget(MSGQ_KEY, IPC_CREAT | S_IRWXU);
    if(mqid==-1)
    {
        perror("msgget:");
        exit(1);
    }
//-----------------------------------create shared memory and connect------------------------------------------------//
    shmid = shmget(MSGQ_KEY, 1024, IPC_CREAT| IPC_EXCL | S_IRWXU); 
    if(shmid==-1)
    {
        perror("shmget:");
        exit(1);
    }
    p = shmat(shmid, NULL,0);
    if(*p ==-1)
    {
        printf("no memory connected.\n");
        exit(1);
    }
//------------------------------------------receive "hi" from client------------------------------------------------//
    msgrcv(mqid, &req, RQS_SIZE, 0, 0);
    printf("%s\n", req.data);

//----------------------------------pick a random word and send details to client-----------------------------------//
    rsp.type = RSP;
    rand_word = pick_a_word(dictionary);
    
    rsp.word_length = strlen(rand_word);

    rsp.word[0] = rand_word[0];
    rsp.word[rsp.word_length-1] = rand_word[rsp.word_length-1];
    for(i=0; i<=rsp.word_length-1; i++)
    {
        if(rsp.word[i] == 0)
            rsp.word[i] = '-';
    }
    rsp.number_of_tries = rsp.word_length;
    rsp.pid = ppid;
    
    msgsnd(mqid, &rsp, RSP_SIZE, 0);
    msgctl(mqid, IPC_RMID, NULL);

//------------------------------------wait for client to connect to shared memory-----------------------------------//
    while(rsp.number_of_tries!=0)
    {
        pause();

        flag = 0;
        for(i=0; i<=rsp.word_length; i++)
        {
            if(*p == rand_word[i])
                rsp.word[i] = *p;

            if(rsp.word[i]== '-')
                flag = 1;
        }
        strcpy(p, rsp.word);
        
        rsp.number_of_tries--;
        kill(req.pid, SIGCONT);
        
        if(flag==0)
            break;
    }

//--------------------------------------------delete shared memory--------------------------------------------------// 
    shmdt(p);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}

//-----------------------------------------LOAD DICTIONARY FUNCTION-------------------------------------------------//
char** load_dict(char* filename)
{
    FILE *fp;
    int i=1;
    char** dictionary; 
    fp=fopen(filename, "r");
    if(fp==NULL)
    {
        perror("fopen");
        exit(1);
    }
    dictionary = (char**)malloc(sizeof(char*)*100);
    while(!feof(fp))
    {
        dictionary[i] = (char*)malloc(sizeof(char)*50);
        fscanf(fp, "%s", dictionary[i]);
        i++;
    }
    return dictionary;
}
//-----------------------------------------PICK A RANDOM WORD FUNCTION--------------------------------------------------//
char* pick_a_word(char** dictionary)
{
    int rand_number;

    rand_number = rand()%100;
    printf("the number i picked was: %d\n", rand_number);
    printf("the word i picked was: %s\n", dictionary[rand_number]);
     
    return dictionary[rand_number];
}
//------------------------------------------SIGNAL HANDLER FUNCTION-----------------------------------------------------//
static void handler(int sig)
{}
