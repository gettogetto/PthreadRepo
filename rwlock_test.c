#include"rwlock.h"
#include<stdio.h>
#include<stdlib.h>
#define THREADS 5
#define DATASIZE 15
#define ITERATIONS 10000
//keep statistics for each thread
typedef struct thread_tag{
    int thread_num;//thread index in the vector
    pthread_t thread_id;//thread id
    int updates;//write counters
    int reads;//read counters
    int interval;//the loop times the thread do reading before it do writing
}thread_t;

//read/write lock and shared data
typedef struct data_tag{
    rwlock_t lock;
    int data;
    int updates;//the times a thread wrote the data
}data_t;

thread_t threads[THREADS];
data_t data[DATASIZE];


//thread start routine that uses read/write lock
void* thread_routine(void* arg)
{
    thread_t* self=(thread_t*)arg;
    int repeats=0;
    int iteration;
    int element=0;
    int status;
    for(iteration=0;iteration<ITERATIONS;iteration++)
    {
        //each self->interval iterations,perform an update operation
        if((iteration%self->interval)==0)//do write
        {
            status=rwl_writelock(&data[element].lock);
            if(status){
                fprintf(stderr,"write lock\n");
                exit(status);
            }
            data[element].data=self->thread_num;
            data[element].updates++;
            self->updates++;
            status=rwl_writeunlock(&data[element].lock);
            if(status){
                fprintf(stderr,"write unlock\n");
                exit(status);
            }
        }
        else{//do read
            status=rwl_readlock(&data[element].lock);
            if(status){
                fprintf(stderr,"read lock\n");
                exit(status);
            }
            self->reads++;
            if(data[element].data==self->thread_num)
            {
                repeats++;
            }
            status=rwl_readunlock(&data[element].lock);
            if(status){
                fprintf(stderr,"read unlock\n");
                exit(status);
            }
        }
        element++;
        if(element>=DATASIZE){
            element=0;
        }
    }
    if(repeats>0){
        printf("Thread %d found unchanged elements %d times\n",self->thread_num,repeats);

    }
    return NULL;
}


int main(int argc,char* argv[]){
    int count;
    int data_count;
    int status;
    unsigned int seed=1;
    int thread_updates=0;
    int data_updates=0;
    //initialize the shared data
    for(data_count=0;data_count<DATASIZE;data_count++)
    {
        data[data_count].data=0;
        data[data_count].updates=0;
        status=rwl_init(&data[data_count].lock);
        if(status){
            fprintf(stderr,"Init rw lock\n");
            exit(status);
        }

    }
    //create threads to access the share data
    for(count=0;count<THREADS;count++)
    {
        threads[count].thread_num=count;
        threads[count].updates=0;
        threads[count].reads=0;
        threads[count].interval=rand_r(&seed)%71;
        status=pthread_create(&threads[count].thread_id,NULL,thread_routine,(void*)&threads[count]);
        if(status){
            fprintf(stderr,"pthread_create\n");
            exit(status);
        }
    }

    //wait for allthreads to complete and collect stastics
    for(count=0;count<THREADS;count++){
        status=pthread_join(threads[count].thread_id,NULL);
        if(status){
            fprintf(stderr,"pthread_join\n");
            exit(status);
        }
        thread_updates+=threads[count].updates;//sum all updates
        printf("%02d:interval %d,updates %d,reads %d\n",count,threads[count].interval,threads[count].updates,threads[count].reads);

    }
    //collect statistics for the data
    for(data_count=0;data_count<DATASIZE;data_count++){
        data_updates+=data[count].updates;
        printf("data %02d:value %d, %d updates\n",
               data_count,data[data_count].data,data[data_count].updates);
        rwl_destroy(&data[data_count].lock);
    }
    printf("%d thread updates,%d data updates\n",thread_updates,data_updates);
    return 0;
}




