#include"workq.h"
#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<unistd.h>
#include<errno.h>
//initialize a work queue
int workq_init(workq_t *wq, int maximum_threads, void (*engine)(void *))
{
    int status;
    status=pthread_attr_init(&wq->attr);
    if(status) return status;
    status=pthread_attr_setdetachstate(&wq->attr,PTHREAD_CREATE_DETACHED);
    if(status){
        pthread_attr_destroy(&wq->attr);
        return status;
    }
    status=pthread_mutex_init(&wq->mutex,NULL);
    if(status){
        pthread_attr_destroy(&wq->attr);
        return status;
    }
    status=pthread_cond_init(&wq->cv,NULL);
    if(status){
        pthread_mutex_destroy(&wq->mutex);
        pthread_attr_destroy(&wq->attr);
        return status;
    }
    wq->quit=0;//not the time to quit
    wq->first=wq->last=NULL;//no queue entries
    wq->parallelism=maximum_threads;//max servers
    wq->counter=0;//no server threads
    wq->idle=0;//no idel servers
    wq->engine=engine;
    wq->valid=WORKQ_VALID;
    return 0;
}

int workq_destroy(workq_t *wq)
{
    int status,status1,status2;
    if(wq->valid!=WORKQ_VALID){
        return EINVAL;
    }
    status=pthread_mutex_lock(&wq->mutex);
    if(status) return status;
    wq->valid=0;//prevent any other operations
    //check any threads are active,and run them
    if(wq->counter>0)
    {
        wq->quit=1;
        //if any threads are idling ,wake them
        if(wq->idle>0){
            status=pthread_cond_broadcast(&wq->cv);
            if(status){
                pthread_mutex_unlock(&wq->mutex);
                return status;
            }
        }
        while(wq->counter>0){
            status=pthread_cond_wait(&wq->cv,&wq->mutex);
            if(status){
                pthread_mutex_unlock(&wq->mutex);
                return status;
            }
        }

    }
    status=pthread_mutex_unlock(&wq->mutex);
    if(status) return status;
    status=pthread_mutex_destroy(&wq->mutex);
    status1=pthread_cond_destroy(&wq->cv);
    status2=pthread_attr_destroy(&wq->attr);
    return status?status:status1?status1:status2;
}

//add an item to a work queue

int workq_add(workq_t *wq, void *element)
{
    workq_ele_t* item;
    pthread_t id;
    int status;
    if(wq->valid!=WORKQ_VALID) return EINVAL;
    //create and initialize a request structure
    item=(workq_ele_t*)malloc(sizeof(workq_ele_t));
    if(item==NULL) return ENOMEM;
    item->data=element;
    item->next=NULL;
    status=pthread_mutex_lock(&wq->mutex);
    if(status){
        free(item);
        return status;
    }
    //add an request to the end of the queue,updating the first and last pointers
    if(wq->first==NULL){
        wq->first=item;
    }
    else{
        wq->last->next=item;

    }
    wq->last=item;
    //if any threads are idling,wake one
    if(wq->idle>0){
        status=pthread_cond_signal(&wq->cv);
        if(status){
            pthread_mutex_unlock(&wq->mutex);
            return status;
        }
    }else if(wq->counter<wq->parallelism){
       //if there are no idling threads but we are allowed to create a new thread,do so
        printf("creating new worker\n");
        status=pthread_create(&id,&wq->attr,workq_server,(void*)wq);
        if(status){
            pthread_mutex_unlock(&wq->mutex);
            return status;
        }
        wq->counter++;
    }
    pthread_mutex_unlock(&wq->mutex);
    return 0;

}

//thread start routine to serve the work queue

static void* workq_server(void* arg)
{
    struct timespec timeout;
    workq_t* wq=(workq_t*)arg;
    workq_ele_t* we;
    int status,timedout;
    printf("A worker is starting\n");
    status=pthread_mutex_lock(&wq->mutex);
    if(status) return NULL;
    while(1){
        timedout=0;
        printf("worker waiting for work\n");
        clock_gettime(CLOCK_REALTIME,&timeout);
        timeout.tv_sec+=2;
        while(wq->first==NULL&&!wq->quit){
            //server threads time out after spending 2 seconds waiting for new work and exit
            status=pthread_cond_timedwait(&wq->cv,&wq->mutex,&timeout);
            if(status=ETIMEDOUT){
                printf("worker wait timed out\n");
                timedout=1;
                break;
            }else if(status!=0){
                printf("worker wait failed ,%d(%s)\n",status,strerror(status));
                wq->counter--;
                pthread_mutex_unlock(&wq->mutex);
                return NULL;
            }
        }
        printf("work queue:0x%p,quit:%d\n",wq->first,wq->quit);
        we=wq->first;
        if(we!=NULL){
            wq->first=we->next;
            if(wq->last==we){
                wq->last=NULL;

            }
            status=pthread_mutex_unlock(&wq->mutex);
            if(status) return NULL;
            printf("worker calling engine\n");
            wq->engine(we->data);
            free(we);
            status=pthread_mutex_lock(&wq->mutex);
            if(status) return NULL;
        }
        //if there are no more work requests and the servers have been asked to quit,then shut down
        if(wq->first==NULL&&wq->quit){
            printf("worker hutting down\n");
            wq->counter--;
            if(wq->counter==0){
                pthread_cond_broadcast(&wq->cv);
            }
            pthread_mutex_unlock(&wq->mutex);
            return NULL;
        }
        //if there is no more work and we wait for as long as we are allowed then terminate this server thread
        if(wq->first==NULL&&timedout){
            printf("engine terminating due to time out\n");
            wq->counter--;
            break;
        }
    }
    pthread_mutex_unlock(&wq->mutex);
    printf("worker exiting\n");
    return NULL;
}


