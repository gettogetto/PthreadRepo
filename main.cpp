//1 tong bu
//#include<error.h>
//#include<stdio.h>
//#include<stdlib.h>
//#include<string.h>
//#include<unistd.h>
//int main(int argc,char* argv[]){
//    int seconds;
//    char line[128];
//    char message[64];

//    while(1){
//        printf("Alarm> ");
//        if(fgets(line,sizeof(line),stdin)==NULL) exit(0);

//        if(strlen(line)<=1) continue;

//        if(sscanf(line,"%d %64[^\n]",&seconds,message)<2){
//            fprintf(stderr,"Bad command\n");
//        }else{
//            sleep(seconds);
//            printf("%d %s\n",seconds,message);
//        }
//    }
//}


//2 multi process

//#include<sys/types.h>
//#include<wait.h>
//#include<error.h>
//#include<errno.h>
//#include<stdio.h>
//#include<stdlib.h>
//#include<string.h>
//#include<unistd.h>

//int main(int argc ,char* argv[])
//{
//    int status;
//    char line[128];
//    int seconds;
//    pid_t pid;
//    char message[64];

//    while(1){
//        printf("Alarm> ");
//        if(fgets(line,sizeof(line),stdin)==NULL){
//            exit(0);
//        }
//        //return format numbers of arguments
//        if(sscanf(line,"%d %64[^\n]",&seconds,message)<2){
//            fprintf(stderr,"Bad command\n");

//        }else{
//            pid=fork();

//            if(pid==-1){
//                exit(-1);
//            }
//            if(pid==0){
//                //child
//                sleep(seconds);
//                printf("%d %s\n",seconds,message);
//                exit(0);
//            }else{
//                //parent
//                do{
//                    pid=waitpid(-1,NULL,WNOHANG);//wait for all child
//                    if(pid==-1){
//                        exit(-1);
//                    }
//                }while(pid!=0);
//            }
//        }
//    }
//}




//3 multi threads
//#include<pthread.h>
//#include<stdio.h>
//#include<stdlib.h>
//#include<string.h>
//#include<unistd.h>

//typedef struct alarm_tag{
//    int seconds;
//    char message[64];
//}alarm_t;

//void* alarm_thread(void* arg){
//    alarm_t* alarm=(alarm_t*)arg;
//    int status;
//    status=pthread_detach(pthread_self());
//    if(status!=0){
//        fprintf(stderr,"pthread_detch() error\n");
//        exit(-1);
//    }
//    sleep(alarm->seconds);
//    printf("%d %s\n",alarm->seconds,alarm->message);
//    free(alarm);
//    return NULL;
//}

//int main(){
//    int status;
//    char line[128];
//    alarm_t* alarm;
//    pthread_t thread;

//    while(1){
//        printf("Alarm> ");
//        if(fgets(line,sizeof(line),stdin)==NULL){
//            exit(0);
//        }
//        if(strlen(line)<=1) continue;
//        alarm=(alarm_t*)malloc(sizeof(alarm_t));
//        if(alarm==NULL){
//            fprintf(stderr,"allocate error\n");
//            exit(-1);
//        }
//        if(sscanf(line,"%d %64[^\n]",&alarm->seconds,&alarm->message)<2){
//            fprintf(stderr,"Bad commond\n");
//            free(alarm);
//        }else{
//            status=pthread_create(&thread,NULL,alarm_thread,(void*)alarm);
//            if(status!=0){
//                fprintf(stderr,"pthread_create error\n");
//                exit(-1);
//            }
//        }
//    }
//}



//4 pthread_join() a uninitialize pthread_t
//#include<pthread.h>
//#include<stdio.h>
//#include<errno.h>
//#include<string.h>//for strerror
//int main(){
//    pthread_t thread;
//    int status;

//    status=pthread_join(thread,NULL);
//    //if the uninitialized thread id happens to de a valid id,it is almost
//    //the main thread id.in that case,it will return EDEADLK
//    //or it may hang(self-deadlock)
//    if(status!=0){
//        fprintf(stderr,"error %d:%s\n",status,strerror(status));
//    }
//    return status;
//}


//#include<pthread.h>
//#include<stdlib.h>
//#include<stdio.h>
//#include<string.h>

//void* thread_routine(void* arg){
//    return arg;
//}

//int main(){
//    pthread_t thread_id;
//    void* thread_result;
//    int status;
//    status=pthread_create(&thread_id,NULL,thread_routine,NULL);

//    if(status!=0){
//        fprintf(stderr,"error:create thread\n");
//    }

//    status=pthread_join(thread_id,&thread_result);

//    if(status!=0){
//        fprintf(stderr,"JOin thread\n");
//    }
//    if(thread_result==NULL){
//        return 0;
//    }else{
//        return 1;
//    }
//}


//5 one thread meet all requests
//#include<pthread.h>
//#include<stdio.h>
//#include<stdlib.h>
//#include<time.h>
//#include<unistd.h>
//#include<string.h>
//typedef struct alarm_tag{
//    struct alarm_tag* link;
//    int seconds;
//    time_t time;
//    char message[64];

//} alarm_t;

//pthread_mutex_t alarm_mutex=PTHREAD_MUTEX_INITIALIZER;
//alarm_t *alarm_list=NULL;

//void* alarm_thread(void* arg){
//    alarm_t* alarm;
//    int sleep_time;
//    time_t now;
//    int status;
//    while(1){
//        status=pthread_mutex_lock(&alarm_mutex);
//        if(status!=0){
//            fprintf(stderr,"error:LOck mutex\n");
//            exit(status);
//        }
//        alarm=alarm_list;
//        if(alarm==NULL){
//            sleep_time=1;//allow main thread to run,so at least in 1s,the thread will not meet the new inserted alarm
//        }else{
//            alarm_list=alarm->link;
//            now=time(NULL);
//            if(alarm->time<=now){
//                sleep_time=0;
//            }else{
//                sleep_time=alarm->time-now;
//            }
//            printf("waiting:%d(%d)\"%s\"\n",(int)alarm->time,sleep_time,alarm->message);

//        }

//        status=pthread_mutex_unlock(&alarm_mutex);//unlock mutex before thread block
//        if(status!=0){
//            fprintf(stderr,"error:unlock mutex\n");
//            exit(status);
//        }
//        if(sleep_time>0){
//            sleep(sleep_time);
//        }else{
//            sched_yield();
//            //give cpu to another ready thread,if there are no ready threads
//            //it will return imediately
//        }
//        if(alarm!=NULL){
//            printf("%d %s\n",alarm->seconds,alarm->message);
//            free(alarm);
//        }
//    }
//}
//int main(){
//    int status;
//    char line[128];
//    alarm_t *alarm,**last,*next;
//    pthread_t thread;
//    status=pthread_create(&thread,NULL,alarm_thread,NULL);
//    //create a thread to do all requests in a list

//    if(status!=0){
//        fprintf(stderr,"error:create alarm thread\n");
//        exit(status);
//    }
//    while(1){
//        printf("Alarm> ");
//        if(fgets(line,sizeof(line),stdin)==NULL) exit(0);

//        if(strlen(line)<2) continue;

//        alarm=(alarm_t*)malloc(sizeof(alarm_t));
//        if(alarm==NULL){
//            fprintf(stderr,"error:allocate alarm\n");
//            exit(-1);
//        }
//        if(sscanf(line,"%d %64[^\n]",&alarm->seconds,alarm->message)<2){
//            fprintf(stderr,"Bad command\n");
//            free(alarm);
//        }else{
//            //alarm_list is shared by main thread and alarm_thread ,so need lock mutex
//            status=pthread_mutex_lock(&alarm_mutex);
//            if(status){
//                fprintf(stderr,"error:lock mutex\n");
//                exit(status);
//            }
//            alarm->time=time(NULL)+alarm->seconds;

//            //insert new alarm into list
//            //insert sort
//            last=&alarm_list;
//            next=*last;
//            while(next!=NULL){
//                if(next->time>=alarm->time){
//                    alarm->link=next;
//                    *last=alarm;
//                    break;
//                }
//                last=&next->link;
//                next=next->link;
//            }
//            if(next==NULL){
//                *last=alarm;
//                alarm->link=NULL;

//            }

//            printf("list: ");
//            for(next=alarm_list;next!=NULL;next=next->link)
//                printf("%d(%d)\"%s\" ",(int)next->time,(int)(next->time-time(NULL)),next->message);
//            printf("\n");

//            status=pthread_mutex_unlock(&alarm_mutex);
//            if(status!=0){
//                fprintf(stderr,"error:unlock mutex\n");
//                exit(status);
//            }

//        }

//    }

//}






//6 pthread_mutex_trylock()

//#include<pthread.h>
//#include<string.h>
//#include<stdio.h>
//#include<stdlib.h>
//#include<unistd.h>
//#include<errno.h>
//#define SPIN 100000000
////control the time mutex locked by counter_thread

//pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
//long counter;
//time_t end_time;

////thread repeatly lock a mutex and increments a counter
//void* counter_thread(void* arg){
//    int status;
//    int spin;
//    while(time(NULL)<end_time){
//        status=pthread_mutex_lock(&mutex);
//        if(status!=0){
//            fprintf(stderr,"error:lock mutex\n");
//            exit(status);
//        }
//        for(spin=0;spin<SPIN;spin++){
//            counter++;
//        }
//        printf("counter_thread:counter is %ld\n",counter);
//        status=pthread_mutex_unlock(&mutex);
//        if(status!=0){
//            fprintf(stderr,"error:unlock mutex\n");
//            exit(status);
//        }
//        sleep(1);
//    }
//    printf("counter thread:counter is %#lx\n",counter);
//    return NULL;
//}

//void* monitor_thread(void* arg){
//    int status;
//    int misses=0;
//    while(time(NULL)<end_time){
//        sleep(1);
//        status=pthread_mutex_trylock(&mutex);
//        if(status!=EBUSY){
//            if(status!=0){
//                fprintf(stderr,"error:trylock mutex\n");
//                exit(status);
//            }
//            printf("monitor thread:counter div SPIN is %ld\n",counter/SPIN);
//            status=pthread_mutex_unlock(&mutex);
//            if(status){
//                fprintf(stderr,"unlock mutex\n");
//                exit(status);

//            }
//        }else{
//            misses++;
//            printf("missed is %d\n",misses);
//        }


//    }
//    printf("monitor thread missed update %d times.\n",misses);
//    return NULL;

//}
//int main(){

//    int status;
//    pthread_t counter_thread_id;
//    pthread_t monitor_thread_id;

//    //pthread_setconcurrency(2);


//    end_time=time(NULL)+60;//run for 1 minute
//    status=pthread_create(&counter_thread_id,NULL,counter_thread,NULL);
//    if(status){
//        fprintf(stderr,"error create thread\n");
//        exit(status);
//    }
//    status=pthread_create(&monitor_thread_id,NULL,monitor_thread,NULL);
//    if(status){
//        fprintf(stderr,"error:create pthread\n");
//        exit(status);
//    }
//    status=pthread_join(counter_thread_id,NULL);
//    if(status){
//        fprintf(stderr,"error:join thread\n");
//        exit(status);
//    }
//    status=pthread_join(monitor_thread_id,NULL);
//    if(status){
//        fprintf(stderr,"error:join thread\n");
//        exit(status);
//    }

//    return 0;
//}



//7 backoff algorithm to solve dead lock
//when pthread_mutex_trylock() return EBUSY,backoff and unlock all its locks

//#include<pthread.h>
//#include<stdio.h>
//#include<stdlib.h>
//#include<unistd.h>
//#include<errno.h>
//#define ITERATION 10

//pthread_mutex_t mutex[3]={
//    PTHREAD_MUTEX_INITIALIZER,
//    PTHREAD_MUTEX_INITIALIZER,
//    PTHREAD_MUTEX_INITIALIZER
//};

//int backoff=1;
//int yield_flag=0;//0:no yield,>0:yield,<0:sleep

//void* lock_forward(void* arg){
//    int i,iterate,backoffs;
//    int status;

//    for(iterate =0;iterate<ITERATION;iterate++){
//        backoffs=0;
//        for(i=0;i<3;i++){
//            if(i==0){
//                status=pthread_mutex_lock(&mutex[0]);
//                if(status){
//                    fprintf(stderr,"forward locker First lock\n");
//                    exit(status);
//                }

//            }else{
//                if(backoff){
//                    status=pthread_mutex_trylock(&mutex[i]);
//                }else{
//                    status=pthread_mutex_lock(&mutex[i]);
//                }
//                if(status==EBUSY){
//                    backoffs++;
//                    printf("forward locker back off at %d\n",i);
//                    for(;i>=0;i--){
//                        status=pthread_mutex_unlock(&mutex[i]);
//                        if(status){
//                            fprintf(stderr,"forward locker backoff at %d\n",i);
//                            exit(status);
//                        }
//                    }
//                }else{
//                    if(status!=0){
//                        fprintf(stderr,"forward locker lock mutex\n");
//                        exit(status);
//                    }else{
//                        printf("forward locker got %d\n",i);
//                    }
//                }
//            }
//            if(yield_flag){
//                if(yield_flag>0){
//                    sched_yield();
//                }
//                else{
//                    sleep(1);
//                }
//            }
//        }
//        printf("lock forward got all locks,%d backoffs\n",backoffs);
//        pthread_mutex_unlock(&mutex[2]);
//        pthread_mutex_unlock(&mutex[1]);
//        pthread_mutex_unlock(&mutex[0]);
//        sched_yield();
//    }
//    return NULL;
//}


//void* lock_backward(void* arg){
//    int i,status,iterate,backoffs;
//    for(iterate=2;iterate>=0;iterate--){
//        backoffs=0;
//        for(i=2;i>=0;i--){
//            if(i==2){
//                status=pthread_mutex_lock(&mutex[i]);
//                if(status){
//                    fprintf(stderr,"backward locker first lock\n");
//                    exit(status);
//                }
//            }else{
//                if(backoff){
//                    status=pthread_mutex_trylock(&mutex[i]);

//                }else{
//                    status=pthread_mutex_lock(&mutex[i]);
//                }
//                if(status==EBUSY){
//                    backoffs++;
//                    printf("backward locker backing off at %d\n",i);
//                    for(;i<3;i++){
//                        status=pthread_mutex_unlock(&mutex[i]);
//                        if(status){
//                            fprintf(stderr,"backward locker backoff\n");
//                            exit(status);
//                        }
//                    }
//                }else{
//                    if(status!=0){
//                        fprintf(stderr,"backward locker lock mutex\n");
//                        exit(status);
//                    }
//                    else{
//                        printf("backward locker got %d\n",i);
//                    }
//                }
//            }
//            if(yield_flag){
//                if(yield_flag>0){
//                    sched_yield();
//                }else{
//                    sleep(1);
//                }
//            }
//        }
//        printf("lock backward got all locks,%d backoffs\n",backoffs);
//        pthread_mutex_unlock(&mutex[0]);
//        pthread_mutex_unlock(&mutex[1]);
//        pthread_mutex_unlock(&mutex[2]);
//        sched_yield();
//    }
//    return NULL;
//}


//int main(int argc,char* argv[]){
//    pthread_t forward,backward;
//    int status;
//    if(argc>1){
//        backoff=atoi(argv[1]);
//    }
//    if(argc>2){
//        yield_flag=atoi(argv[2]);
//    }

//    status=pthread_create(&forward,NULL,lock_forward,NULL);
//    if(status){
//        fprintf(stderr,"create lock_forward\n");
//        exit(status);
//    }
//    status=pthread_create(&backward,NULL,lock_backward,NULL);
//    if(status){
//        fprintf(stderr,"create back_forward\n");
//        exit(status);
//    }

//    pthread_exit(NULL);
//}




//8.condition variables
//#include<pthread.h>
//#include<time.h>
//#include<errno.h>
//#include<stdio.h>
//#include<stdlib.h>
//#include<unistd.h>

//typedef struct my_struct_tag{
//    pthread_mutex_t mutex;
//    pthread_cond_t cond;
//    int value;
//} my_struct_t;

//my_struct_t data={
//    PTHREAD_MUTEX_INITIALIZER,
//    PTHREAD_COND_INITIALIZER,
//    0
//};

//int hibernation=1;

//void* wait_thread(void* arg){
//    int status;
//    sleep(hibernation);
//    status=pthread_mutex_lock(&data.mutex);
//    if(status){
//        fprintf(stderr,"wait_thread():lock mutex\n");
//        exit(status);
//    }
//    data.value=1;
//    status=pthread_cond_signal(&data.cond);
//    if(status){
//        fprintf(stderr,"wait_thread():cond signal\n");
//        exit(status);
//    }
//    status=pthread_mutex_unlock(&data.mutex);
//    if(status){
//        fprintf(stderr,"wait_thread():pthread_mutex_unlock\n");
//        exit(status);
//    }
//    return NULL;

//}


//int main(int argc,char* argv[]){
//    int status;
//    pthread_t wait_thread_id;
//    struct timespec timeout;
//    if(argc>1){
//        hibernation=atoi(argv[1]);//set wait_thread sleep time
//    }

//    status=pthread_create(&wait_thread_id,NULL,wait_thread,NULL);

//    if(status){
//        fprintf(stderr,"main():pthread_create\n");
//        exit(status);
//    }

//    timeout.tv_sec=time(NULL)+2;//set condition wait time
//    timeout.tv_nsec=0;

//    status=pthread_mutex_lock(&data.mutex);
//    if(status){
//        fprintf(stderr,"main():pthread_mutex_lock\n");
//        exit(status);
//    }
//    while(data.value==0){
//        //condition wait should be always  in a loop,
//        //after the main thread waked ,next loop it will test the value
//        status=pthread_cond_timedwait(&data.cond,&data.mutex,&timeout);//go to sleep
//        if(status==ETIMEDOUT){
//            printf("main():pthread_cond_timed wait time out\n");
//            break;
//        }else if(status!=0){
//            fprintf(stderr,"main():pthread_cond_timedwait\n");
//            exit(status);
//        }
//    }
//    if(data.value!=0){
//        printf("main():condition success signaled\n");
//    }

//    status=pthread_mutex_unlock(&data.mutex);
//    if(status){
//        fprintf(stderr,"main():pthread_mutex_unlock\n");
//        exit(status);
//    }
//    return 0;
//}


//9.alarm using condition variabels

//#include<pthread.h>
//#include<stdlib.h>
//#include<stdio.h>
//#include<unistd.h>
//#include<errno.h>
//#include<string.h>
//typedef struct alarm_tag{
//    struct alarm_tag *link;
//    int seconds;
//    time_t time;
//    char message[64];

//} alarm_t;

//pthread_mutex_t alarm_mutex=PTHREAD_MUTEX_INITIALIZER;
//pthread_cond_t alarm_cond=PTHREAD_COND_INITIALIZER;
//alarm_t *alarm_list=NULL;
//time_t current_alarm=0;

//void alarm_insert(alarm_t* alarm){
//    int status;
//    alarm_t **last,*next;
//    last=&alarm_list;
//    next=*last;
//    while(next!=NULL){
//        if(next->time>=alarm->time){
//            alarm->link=next;
//            *last=alarm;
//            break;
//        }
//        last=&next->link;
//        next=next->link;

//    }
//    if(next==NULL){
//        *last=alarm;
//        alarm->link=NULL;
//    }
//    printf("list: ");
//    for(next=alarm_list;next!=NULL;next=next->link)
//        printf("%d(%d)\"%s\" ",(int)next->time,(int)(next->time-time(NULL)),next->message);
//    printf("\n");

//    if(current_alarm==0||alarm->time<current_alarm){
//        current_alarm=alarm->time;//update the new current_alarm
//        //if thread is no busy or new inserted alarm time is smaller than the current_alarm
//        //signal cond.
//        status=pthread_cond_signal(&alarm_cond);
//        if(status){
//            fprintf(stderr,"alarm_thread:pthread_cond_signal\n");
//            exit(status);
//        }
//    }

//}

//void* alarm_thread(void* arg){
//    alarm_t* alarm;
//    struct timespec cond_time;
//    time_t now;
//    int status,expired;
//    status=pthread_mutex_lock(&alarm_mutex);
//    if(status){
//        fprintf(stderr,"alarm_thread:pthread_mutex_lock\n");
//        exit(status);
//    }

//    while(1){
//        current_alarm=0;
//        while(alarm_list==NULL){
//            //if list empty,wait for new alarm using cond,do not use sleep like example 6
//            status=pthread_cond_wait(&alarm_cond,&alarm_mutex);
//            if(status){
//                fprintf(stderr,"alarm_thread:pthread_cond_wait\n");
//                exit(status);
//            }
//        }
//        alarm=alarm_list;
//        alarm_list=alarm->link;
//        now=time(NULL);
//        expired=0;
//        if(alarm->time>now){
//            printf("waiting:%d(%d)\"%s\"\n",(int)alarm->time,(int)(alarm->time-time(NULL)),alarm->message);
//            cond_time.tv_sec=alarm->time;
//            cond_time.tv_nsec=0;

//            current_alarm=alarm->time;//the time alarm will expire
//            while(current_alarm==alarm->time){
//                status=pthread_cond_timedwait(&alarm_cond,&alarm_mutex,&cond_time);
//                if(status==ETIMEDOUT){//alarm  expire
//                    expired=1;
//                    break;
//                }
//                if(status){
//                    fprintf(stderr,"alarm_thread:pthread_cond_timedwait\n");
//                    exit(status);
//                }
//            }
//            if(!expired){
//                alarm_insert(alarm);
//            }
//        }else{
//            expired=1;
//        }
//        if(expired){
//            printf("(%d) %s\n",alarm->seconds,alarm->message);
//            free(alarm);
//        }
//    }
//}

//int main(int argc,char* argv[]){
//    int status;
//    char line[128];
//    alarm_t* alarm;
//    pthread_t thread;
//    status=pthread_create(&thread,NULL,alarm_thread,NULL);
//    if(status){
//        fprintf(stderr,"main:pthread_create\n");
//        exit(status);
//    }
//    while(1){
//        printf("Alarm> ");
//        if(fgets(line,sizeof(line),stdin)==NULL){
//            exit(0);
//        }
//        if(strlen(line)<2) continue;
//        alarm=(alarm_t*)malloc(sizeof(alarm_t));
//        if(alarm==NULL){
//            fprintf(stderr,"main:allocate\n");
//            exit(-1);
//        }
//        //%*[^\n] means read  the char which is not \n
//        if(sscanf(line,"%d %64[^\n]",&alarm->seconds,alarm->message)<2){
//            fprintf(stderr,"bad command\n");
//            free(alarm);
//        }else{
//            status=pthread_mutex_lock(&alarm_mutex);//lock for alarm_insert()
//            if(status){
//                fprintf(stderr,"main:pthread_mutex_lock\n");
//                exit(status);
//            }
//            alarm->time=time(NULL)+alarm->seconds;
//            alarm_insert(alarm);
//            status=pthread_mutex_unlock(&alarm_mutex);
//            if(status){
//                fprintf(stderr,"main:pthread_mutex_unlock\n");
//                exit(status);
//            }
//        }

//    }
//}


//three thread programing models
//10.pipe.c
//#include<pthread.h>
//#include<errno.h>
//#include<stdlib.h>
//#include<stdio.h>
//#include<string.h>
//#include<unistd.h>

//typedef struct stage_tag{
//    pthread_mutex_t mutex;
//    pthread_cond_t avail;
//    pthread_cond_t ready;
//    int data_ready;
//    long data;
//    pthread_t thread;
//    struct stage_tag* next;
//}stage_t;

//typedef struct pipe_tag{
//    pthread_mutex_t mutex;
//    stage_t *head;
//    stage_t *tail;
//    int stages;
//    int active;
//}pipe_t;

//int pipe_send(stage_t* stage,long data){
//    int status;
//    status=pthread_mutex_lock(&stage->mutex);
//    if(status){
//        fprintf(stderr,"pipe_send:pthread_mutex_lock\n");
//        exit(status);
//    }
//    while(stage->data_ready){
//        status=pthread_cond_wait(&stage->avail,&stage->mutex);
//        if(status!=0){
//            pthread_mutex_unlock(&stage->mutex);
//            return status;
//        }
//    }
//    stage->data=data;
//    stage->data_ready=1;
//    status=pthread_cond_signal(&stage->avail);
//    if(status!=0){
//        pthread_mutex_unlock(&stage->mutex);
//        return status;
//    }
//    status=pthread_mutex_unlock(&stage->mutex);
//    return status;
//}
//void* pipe_stage(void* arg){
//    stage_t *stage=(stage_t*)arg;
//    stage_t *next_stage=stage->next;
//    int status;
//    status=pthread_mutex_lock(&stage->mutex);
//    if(status){
//        fprintf(stderr,"pipe_stage:pthread_mutex_lock\n");
//        exit(status);
//    }
//    while(1){
//        while(stage->data_ready!=1){
//            status=pthread_cond_wait(&stage->avail,&stage->mutex);
//            if(status!=0){
//                fprintf(stderr,"pipe_stage:pthread_cond_wait\n");
//                exit(status);
//            }
//        }
//        pipe_send(next_stage,stage->data+1);//next step
//        stage->data_ready=0;
//        status=pthread_cond_signal(&stage->ready);
//        if(status!=0){
//            fprintf(stderr,"pipe_stage:pthread_cond_signal\n");
//            exit(status);
//        }
//    }
//    //because the loop never terminates,this function has no need to unlock the mutex expliicitly

//}

//int pipe_create(pipe_t* pipe,int stages){
//    int pipe_index;
//    stage_t** link=&pipe->head,*new_stage,*stage;
//    int status;
//    status=pthread_mutex_init(&pipe->mutex,NULL);
//    if(status!=0){
//        fprintf(stderr,"pipe_create:pthread_mutex_init\n");
//        exit(status);
//    }
//    pipe->stages=stages;
//    pipe->active=0;

//    for(pipe_index=0;pipe_index<=stages;pipe_index++){
//        new_stage=(stage_t*)malloc(sizeof(stage_t));
//        if(new_stage==NULL){
//            fprintf(stderr,"pipe_create:malloc\n");
//            exit(-1);
//        }
//        status=pthread_mutex_init(&stage->mutex,NULL);
//        if(status!=0){
//            fprintf(stderr,"pipe_create:pthread_mutex_init\n");
//            exit(status);
//        }
//        status=pthread_cond_init(&stage->avail,NULL);
//        if(status!=0){
//            fprintf(stderr,"pipe_create:pthread_cond_init\n");
//            exit(status);
//        }
//        status=pthread_cond_init(&stage->ready,NULL);
//        if(status!=0){
//            fprintf(stderr,"pipe_create:pthread_cond_init\n");
//            exit(status);
//        }
//        new_stage->data_ready=0;
//        *link=new_stage;
//        link=&new_stage->next;
//    }
//    *link=(stage_t*)NULL;
//    pipe->tail=new_stage;
//    for(stage=pipe->head;stage->next!=NULL;stage=stage->next){
//        status=pthread_create(&stage->thread,NULL,pipe_stage,(void*)stage);
//        if(status!=0){
//            fprintf(stderr,"pipe_create:pthread_create\n");
//            exit(status);
//        }
//    }
//    return 0;
//}

//int pipe_start(pipe_t* pipe,long value){
//    int status;
//    status=pthread_mutex_lock(&pipe->mutex);
//    if(status){
//        fprintf(stderr,"pipe_start:pthread_mutex_lock\n");
//        exit(status);
//    }
//    pipe->active++;
//    status=pthread_mutex_unlock(&pipe->mutex);
//    if(status){
//        fprintf(stderr,"pipe_start:pthread_mutex_lock\n");
//        exit(status);
//    }
//    pipe_send(pipe->head,value);
//    return 0;
//}

//int pipe_result(pipe_t *pipe,long* result){
//    stage_t* tail=pipe->tail;
//    long value;
//    int empty=0;
//    int status;
//    status=pthread_mutex_lock(&pipe->mutex);
//    if(status){
//        fprintf(stderr,"pipe_result:pthread_mutex_lock\n");
//        exit(status);
//    }
//    if(pipe->active<=0){
//        empty=1;
//    }else{
//        pipe->active--;
//    }
//    status=pthread_mutex_unlock(&pipe->mutex);
//    if(status){
//        fprintf(stderr,"pipe_result:pthread_mutex_unlock\n");
//        exit(status);
//    }
//    if(empty){
//        return 0;
//    }
//    pthread_mutex_lock(&tail->mutex);
//    while(!tail->data_ready){
//        pthread_cond_wait(&tail->avail,&tail->mutex);

//    }
//    *result=tail->data;
//    tail->data_ready=0;

//    pthread_cond_signal(&tail->ready);
//    pthread_mutex_unlock(&tail->mutex);
//    return 1;
//}
//int main(int argc,char* argv[]){
//    pipe_t my_pipe;
//    long value,result;
//    int status;
//    char line[128];
//    pipe_create(&my_pipe,10);
//    printf("Enter integer values,or \"=\" for next result\n");
//    while(1){
//        printf("Data> ");
//        if(fgets(line,sizeof(line),stdin)==NULL) exit(0);
//        if(strlen(line)<=1) continue;
//        if(strlen(line)<=2&&line[0]=='='){
//            if(pipe_result(&my_pipe,&result)){
//                printf("result is %ld\n",result);
//            }else{
//                printf("pipe is empty\n");
//            }
//        }else
//        {
//            if(sscanf(line,"%ld",&value)<1){
//                fprintf(stderr,"enter an integer value\n");
//            }else{
//                pipe_start(&my_pipe,value);
//            }
//        }
//    }
//}


//11 pthread_once
/*
#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

pthread_once_t once_block=PTHREAD_ONCE_INIT;
pthread_mutex_t mutex;

void once_init_routine(void){
    pthread_mutex_init(&mutex,NULL);
}
void* thread_routine(void* arg){
    pthread_once(&once_block,once_init_routine);
    pthread_mutex_lock(&mutex);
    printf("thread_routine has locked the mutex\n");
    pthread_mutex_unlock(&mutex);
    return NULL;
}
int main(int argc,char* argv[]){
    pthread_t thread_id;
    char* input,buffer[64];

    pthread_create(&thread_id,NULL,thread_routine,NULL);
    pthread_once(&once_block,once_init_routine);
    pthread_mutex_lock(&mutex);
    printf("Main has locked the mutex\n");
    pthread_mutex_unlock(&mutex);

    pthread_join(thread_id,NULL);
    return 0;
}*/


//12 pthread_attr_init
/*
#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
void* thread_routine(void* arg){
    printf("the thread is here\n");
    return NULL;
}
int main(int argc, char *argv[]){
    pthread_t thread_id;
    pthread_attr_t thread_attr;
    struct sched_param thread_param;
    size_t stack_size;

    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_DETACHED);
    pthread_attr_getstacksize(&thread_attr,&stack_size);
    printf("default stack size is %u\n",stack_size);
    pthread_attr_setstacksize(&thread_attr,stack_size*2);
    pthread_attr_getstacksize(&thread_attr,&stack_size);
    printf("now stack size is %u\n",stack_size);

    pthread_create(&thread_id,&thread_attr,thread_routine,NULL);
    printf("main exiting \n");
    pthread_exit(NULL);

    return 0;
}*/

//12 pthread_cancel
/*
#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<errno.h>
static int counter;

void* thread_routine(void* arg){
    printf("thread_routine starting\n");
    for(counter=0;;counter++){
        if(counter%1000==0){
            printf("calling testcancel\n");
            pthread_testcancel();//set a "cancel point"
        }
    }
}

void* thread_routine_disabled(void* arg){
    printf("thread_routine starting\n");
    int state;
    for(counter=0;;counter++){
        if(counter%755==0){//forbid the cancel
            printf("setcancelstate disabled\n");
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,&state);
            sleep(1);
            printf("setcancelstate abled\n");
            pthread_setcancelstate(state,&state);
        }
        if(counter%1000==0){
            printf("calling testcancel\n");
            pthread_testcancel();//set a "cancel point"
        }
    }
}

int main(int argc,char* argv[]){
    pthread_t thread_id;
    void* result;

    pthread_create(&thread_id,NULL,thread_routine_disabled,NULL);
    sleep(2);
    printf("calling pthread_cancel\n");
    pthread_cancel(thread_id);

    printf("calling join\n");
    pthread_join(thread_id,&result);

    if(result==PTHREAD_CANCELED){
        printf("thread canceled at iteration %d\n",counter);
    }else{
        printf("thread was not canceled\n");
    }
    return 0;
}*/

//12 cancel_async.c
/*
#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>

#define SIZE 10

static int matrixa[SIZE][SIZE];
static int matrixb[SIZE][SIZE];
static int matrixc[SIZE][SIZE];

void* thread_routine(void* arg){
    int cancel_type;
    int i,j,k,value=1;

    for(i=0;i<SIZE;i++){
        matrixa[i][j]=i;
        matrixb[i][j]=j;
    }
    while(1){
        //compute the product of matrixa and matrixb
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&cancel_type);
        for(i=0;i<SIZE;i++){
            for(j=0;j<SIZE;j++){
                matrixc[i][j]=0;
                for(k=0;k<SIZE;k++){
                    matrixc[i][j]+=matrixa[i][j]*matrixb[i][j];
                }
            }
        }
        pthread_setcanceltype(cancel_type,&cancel_type);
        for(i=0;i<SIZE;i++){
            for(j=0;j<SIZE;j++){
                matrixa[i][j]=matrixc[i][j];
            }
        }
    }
}
int main(int argc, char *argv[])
{
    pthread_t thread_id;
    void* result;
    pthread_create(&thread_id,NULL,thread_routine,NULL);
    sleep(1);
    pthread_cancel(thread_id);
    pthread_join(thread_id,&result);
    if(result==PTHREAD_CANCELED){
        printf("thread canceled\n");
    }else{
        printf("thread was not canceled\n");
    }
    return 0;
}*/

//12 cancel_cleanup.c
/*
#include<pthread.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#define THREADS 5
typedef struct control_tag{
    int counter,busy;
    pthread_mutex_t mutex;
    pthread_cond_t cv;

} control_t;
control_t control={0,1,PTHREAD_MUTEX_INITIALIZER,PTHREAD_COND_INITIALIZER};

void clean_handler(void* arg){//will be called when thread is canceled or finished normally
    control_t* st=(control_t*)arg;
    int status;
    st->counter--;
    printf("cleanup_handler:counter==%d\n",st->counter);
    pthread_mutex_unlock(&st->mutex);
}

void* thread_routine(void* arg){
    int status;
    pthread_cleanup_push(clean_handler,(void*)&control);
    pthread_mutex_lock(&control.mutex);
    control.counter++;
    while(control.busy){//cond wait willnot stop until thread is canceled
        pthread_cond_wait(&control.cv,&control.mutex);
    }
    pthread_cleanup_pop(1);//pthread_clean_up and pthread_clean_pop must be in a same function


}
int main(int argc,char* argv[]){
    pthread_t thread_id[THREADS];
    int count;
    void* result;
    int status;
    for(count=0;count<THREADS;count++){
        status=pthread_create(&thread_id[count],NULL,thread_routine,NULL);
    }
    sleep(2);
    for(count=0;count<THREADS;count++){
        pthread_cancel(thread_id[count]);
        pthread_join(thread_id[count],&result);

        if(result==PTHREAD_CANCELED){
            printf("thread %d canceled\n",count);
        }else{
            printf("thread %d not canceled\n",count);
        }
    }
    return 0;
}
*/

//12
/*
#include<pthread.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>

#define THREADS 5

typedef struct team_tag{
    int join_i;//join index
    pthread_t workers[THREADS];
}team_t;

void* worker_routine(void* arg){//fen bao

    int counter;
    for(counter=0;;counter++){
        if(counter%1000==0){
            pthread_testcancel();
        }
    }
}

void cleanup(void* arg){
    team_t* team=(team_t*)arg;
    int count;
    for(count=team->join_i;count<THREADS;count++){
        //cancel the rest worker threads when the thread_routine is canceled
        printf("cancel the worker thread %d\n",count);
        pthread_cancel(team->workers[count]);
        pthread_join(team->workers[count],NULL);//or pthread_detach();
        //use command "valgrind --tool=memcheck ./projectname" to check memory leak
        //why pthread_detach() here will leak memory?
    }
}

void* thread_routine(void* arg){//cheng bao

    team_t team;
    int count;
    void* result;
    for(count=0;count<THREADS;count++){
        pthread_create(&team.workers[count],NULL,worker_routine,NULL);
    }
    pthread_cleanup_push(cleanup,(void*)&team);
    for(team.join_i=0;team.join_i<THREADS;team.join_i++){
        pthread_join(team.workers[team.join_i],&result);//pthread_join is a cancel pointer
    }
    pthread_cleanup_pop(0);
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t thread_id;
    pthread_create(&thread_id,NULL,thread_routine,NULL);
    sleep(5);
    printf("canceling thread_routine\n");
    pthread_cancel(thread_id);
    pthread_join(thread_id,NULL);
    return 0;
}
*/

//12 pthread_key_t
/*
#include<pthread.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>

typedef struct tsd_tag{
    pthread_t thread_id;
    char *string;
}tsd_t;

pthread_key_t tsd_key;
pthread_once_t key_once=PTHREAD_ONCE_INIT;

void once_routine(void){
    pthread_key_create(&tsd_key,NULL);
}

void* thread_routine(void* arg){
    tsd_t *value;
    pthread_once(&key_once,once_routine);//only run once
    value=(tsd_t*)malloc(sizeof(tsd_t));
    pthread_setspecific(tsd_key,value);
    printf("%s set tsd value %p\n",(char*)arg,value);
    value->thread_id=pthread_self();
    value->string=(char*)arg;


    value=(tsd_t*)pthread_getspecific(tsd_key);
    //even 2 threads set the value to
    //same tsd_key,every thread can get its own value from the tsd_key
    printf("%s starting...\n",value->string);
    sleep(2);

    value=(tsd_t*)pthread_getspecific(tsd_key);
    printf("%s done...tsd value=%p\n",value->string,value);
    free(value);
}
int main(int arg,char* argv[]){
    pthread_t thread1,thread2;
    pthread_create(&thread1,NULL,thread_routine,const_cast<char*>("thread1"));
    pthread_create(&thread2,NULL,thread_routine,const_cast<char*>("thread2"));
    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);

   // pthread_exit(NULL);
}*/
//12 tsd_destructor.c
#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>

typedef struct private_tag{
    pthread_t thread_id;
    char* string;
}private_t;

pthread_key_t identity_key;
pthread_mutex_t identity_key_mutex=PTHREAD_MUTEX_INITIALIZER;
long identity_key_counter=0;

void identity_key_destructor(void* value){
    private_t *priva=(private_t*)value;
    printf("thread \"%s\" exiting...\n",priva->string);
    free(value);
    pthread_mutex_lock(&identity_key_mutex);
    identity_key_counter--;
    if(identity_key_counter<=0){
        pthread_key_delete(identity_key);
        printf("key deleted\n");
    }
    pthread_mutex_unlock(&identity_key_mutex);
}
void* identity_key_get(){
    void* value;
    value=pthread_getspecific(identity_key);
    if(value==NULL){
        value=malloc(sizeof(private_t));
        pthread_setspecific(identity_key,value);
    }
    return value;
}

void* thread_routine(void* arg){
    private_t *value;
    value=(private_t*)identity_key_get();
    value->thread_id=pthread_self();
    value->string=(char*)arg;
    printf("thread \"%s\" starting...\n",value->string);
    sleep(2);
    return NULL;
}

int main(int argc,char* argv[]){
    pthread_t thread1,thread2;
    private_t* value;
    pthread_key_create(&identity_key,identity_key_destructor);
    identity_key_counter=2;
    value=(private_t*)identity_key_get();
    value->thread_id=pthread_self();
    value->string=const_cast<char*>("Main thread");
    pthread_create(&thread1,NULL,thread_routine,const_cast<char*>("thread1"));
    pthread_create(&thread2,NULL,thread_routine,const_cast<char*>("thread2"));

    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);

}



//13 pthread_atfork()
/*
#include<pthread.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/wait.h>
pid_t self_pid;//currrent process id
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
//will called in the parent before fork to ensure no other thread can lock it until the fork completes
//this function is to get all locks of parent process
void fork_prepare(void){
    int status;
    status=pthread_mutex_lock(&mutex);
    if(status){
        fprintf(stderr,"fork_prepare\n");
        exit(status);
    }

}
//will be called after executing the fork within the parent process
//to unlock all locks get from parent process
void fork_parent(void){
    int status;
    status=pthread_mutex_unlock(&mutex);
    if(status){
        fprintf(stderr,"fork_parent\n");
        exit(status);
    }
}
//will be called after executing the fork within the child process
//to unlock all locks get from parent process
void fork_child(void){
    int status;
    self_pid=getpid();
    status=pthread_mutex_unlock(&mutex);
    if(status){
        fprintf(stderr,"fork_child\n");
        exit(status);
    }
}

void* thread_routine(void* arg){
    pid_t child_pid;
    int status;
    child_pid=fork();
    if(child_pid==-1){
        fprintf(stderr,"fork\n");
        exit(-1);
    }

    status=pthread_mutex_lock(&mutex);

    if(status){
        fprintf(stderr,"thread_routine:pthread_mutex_lock\n");
        exit(status);
    }

    if(child_pid!=0) printf("parent thread got lock\n");
    else printf("child thread got lock\n");

    status=pthread_mutex_unlock(&mutex);
    if(status){
        fprintf(stderr,"thread_routine:pthread_mutex_unlock\n");
        exit(status);
    }
    printf("After fork:%d (%d)\n",child_pid,self_pid);

    if(child_pid!=0){
        if(waitpid(child_pid,NULL,0)==-1){
            fprintf(stderr,"wait for child");
            exit(-1);
        }
    }
    return NULL;
}

int main(int argc,char* argv[]){
    pthread_t fork_thread;
    int atfork_flag=1;
    int status;
    if(argc>1){
        atfork_flag=atoi(argv[1]);

    }
    if(atfork_flag){
        status=pthread_atfork(fork_prepare,fork_parent,fork_child);
        if(status){
            fprintf(stderr,"pthread_atfork");
            exit(status);
        }
    }
    self_pid=getpid();
    status=pthread_mutex_lock(&mutex);

    if(status){
        fprintf(stderr,"main:pthread_mutex_lock");
        exit(status);
    }

    status=pthread_create(&fork_thread,NULL,thread_routine,NULL);
    if(status){
        fprintf(stderr,"main:pthread_create");
        exit(status);
    }
    sleep(5);
    status=pthread_mutex_unlock(&mutex);
    if(status){
        fprintf(stderr,"main:pthread_mutex_unlock");
        exit(status);
    }
    status=pthread_join(fork_thread,NULL);
    if(status){
        fprintf(stderr,"main:pthread_join");
        exit(status);
    }

    return 0;
}
*/




//14.flockfile   funlockfile
/*
#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<string.h>


void* prompt_routine(void* arg){
    char* prompt=(char*)arg;
    char* string;
    int len;
    string=(char*)malloc(128);
    if(string==NULL){
        fprintf(stderr,"malloc\n");
        exit(-1);
    }
    flockfile(stdin);//first lock stdin
    flockfile(stdout);
    printf("%s",prompt);
    if(fgets(string,128,stdin)==NULL){
        string[0]='\0';
    }else{
        len=strlen(string);
        if(len>0&&string[len-1]=='\n'){
            string[len-1]='\0';
        }
    }
    funlockfile(stdout);//inverse
    funlockfile(stdin);
    return (void*)string;
}

int main(int argc,char*argv[]){
    pthread_t thread1,thread2,thread3;
    void* string;
    int status;
    pthread_create(&thread1,NULL,prompt_routine,const_cast<char*>("Thread1> "));
    pthread_create(&thread2,NULL,prompt_routine,const_cast<char*>("Thread2> "));
    pthread_create(&thread3,NULL,prompt_routine,const_cast<char*>("Thread3> "));

    pthread_join(thread1,&string);
    printf("thread1:\"%s\" \n",string);
    free(string);
    pthread_join(thread2,&string);
    printf("thread2:\"%s\" \n",string);
    free(string);
    pthread_join(thread3,&string);
    printf("thread3:\"%s\" \n",string);
    free(string);
    return 0;
}
*/


//15 getchar_unlocked putchar_unlocked getc_unlocked putc_unlocked
/*
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>

//lock the file stream and use putchar_unlocked to write each character individually
void* lock_routine(void* arg){
    char* pointer;
    flockfile(stdout);
    for(pointer=(char*)arg;*pointer!='\0';pointer++){
        putchar_unlocked(*pointer);
        sleep(1);
    }
    funlockfile(stdout);
    return NULL;

}
//although the internal locking of putchar prevents file stream
//corruption,the writes of vatious threads may be interleaved
void* unlock_routine(void* arg){
    char* pointer;
    for(pointer=(char*)arg;*pointer!='\0';pointer++){
        putchar(*pointer);
        sleep(1);
    }
    return NULL;
}

int main(int argc,char* argv[]){
    pthread_t thread1,thread2,thread3;
    int flock_flag=1;
    void*(*thread_func)(void*);
    if(argc>1){
        flock_flag=atoi(argv[1]);
    }
    if(flock_flag==1){
        thread_func=lock_routine;
    }else{
        thread_func=unlock_routine;
    }

    pthread_create(&thread1,NULL,thread_func,const_cast<char*>("this is thread1\n"));
    pthread_create(&thread2,NULL,thread_func,const_cast<char*>("this is thread2\n"));
    pthread_create(&thread3,NULL,thread_func,const_cast<char*>("this is thread3\n"));

    pthread_exit(NULL);

}
*/










