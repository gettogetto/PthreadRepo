/*
 *this read/write lock is
 *a read advance version
 *
 */

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include"rwlock.h"

int rwl_init(rwlock_t *rwlock){
    int status;
    rwlock->r_active=0;
    rwlock->r_wait=rwlock->w_wait=0;
    rwlock->w_active=0;

    status=pthread_mutex_init(&rwlock->mutex,NULL);
    if(status) return status;
    status=pthread_cond_init(&rwlock->read,NULL);
    if(status) {
        pthread_mutex_destroy(&rwlock->mutex);
        return status;
    }
    status=pthread_cond_init(&rwlock->write,NULL);
    if(status) {
        pthread_cond_destroy(&rwlock->read);
        pthread_mutex_destroy(&rwlock->mutex);
        return status;
    }
    rwlock->valid=RWLOCK_VALID;
    return 0;
}

int rwl_destroy(rwlock_t *rwlock){
    int status,status1,status2;
    if(rwlock->valid!=RWLOCK_VALID){
        return EINVAL;
    }
    status=pthread_mutex_lock(&rwlock->mutex);
    if(status) return status;

    //check if any threads own the lock,if so return EBUSY
    if(rwlock->r_active>0||rwlock->w_active>0)
    {
        pthread_mutex_unlock(&rwlock->mutex);
        return EBUSY;
    }
    //check whether any threads are known to be waiting,report EBUSY if so.
    if(rwlock->r_wait>0||rwlock->w_wait>0)
    {
        pthread_mutex_unlock(&rwlock->mutex);
        return EBUSY;
    }
    rwlock->valid=0;
    status=pthread_mutex_unlock(&rwlock->mutex);
    if(status) return status;

    status=pthread_mutex_destroy(&rwlock->mutex);

    status1=pthread_cond_destroy(&rwlock->read);

    status2=pthread_cond_destroy(&rwlock->write);

    return (status!=0? status : (status1!=0 ? status1:status2));

}

//handle cleanup when the read lock condition variable wait is canceled
//simply record that the thread is no longer waiting and unlock the mutex
static void rwl_readcleanup(void* arg){
    rwlock_t *rwl=(rwlock_t*) arg;
    rwl->r_wait--;
    pthread_mutex_unlock(&rwl->mutex);
}
//handle cleanup when the write lock condition variable wait is canceled
//simply record that the thread is no longer waiting and unlock the mutex
static void rwl_writecleanup(void* arg){
    rwlock_t *rwl=(rwlock_t*)arg;
    rwl->w_wait--;
    pthread_mutex_unlock(&rwl->mutex);

}

int rwl_readlock(rwlock_t *rwlock){
    int status;
    if(rwlock->valid!=RWLOCK_VALID){
        return EINVAL;
    }
    status=pthread_mutex_lock(&rwlock->mutex);
    if(status) return status;
    if(rwlock->w_active){//if there are writers,the reader should waitfor condition virable
        rwlock->r_wait++;
        pthread_cleanup_push(rwl_readcleanup,(void*)rwlock);
        while(rwlock->w_active){
            status=pthread_cond_wait(&rwlock->read,&rwlock->mutex);
            if(status) break;
        }
        pthread_cleanup_pop(0);
    }
    if(status==0) rwlock->r_active++;

    pthread_mutex_unlock(&rwlock->mutex);
    return status;
}

int rwl_readtrylock(rwlock_t *rwlock){
    int status,status2;
    if(rwlock->valid!=RWLOCK_VALID){
        return EINVAL;
    }
    status=pthread_mutex_lock(&rwlock->mutex);
    if(status) return status;
    if(rwlock->w_active) status=EBUSY;//if there are writers active,return EBUSY
    else rwlock->r_active++;//else the reader can be active

    status2=pthread_mutex_unlock(&rwlock->mutex);
    return (status2!=0?status2:status);
}
//unlock a rwlock from read access
int rwl_readunlock(rwlock_t *rwlock){
    int status,status2;
    if(rwlock->valid!=RWLOCK_VALID){
        return EINVAL;
    }
    status=pthread_mutex_lock(&rwlock->mutex);
    if(status) return status;
    rwlock->r_active--;
    //if there are no readthreads running,and there are writerthreads waiting,then do conditon signal
    if(rwlock->r_active==0&&rwlock->w_wait>0){//read advance version
        status=pthread_cond_signal(&rwlock->write);
    }
    status2=pthread_mutex_unlock(&rwlock->mutex);
    return status2!=0?status2:status;
}
//lock a rwlock for write access
int rwl_writelock(rwlock_t *rwlock){
    int status;
    if(rwlock->valid!=RWLOCK_VALID){
        return EINVAL;
    }
    status=pthread_mutex_lock(&rwlock->mutex);
    if(status) return status;
    if(rwlock->w_active==1||rwlock->r_active>0){//there is most 1 write thread,once there is 1 writer or any readers,the new writer should wait for write condition variable
        rwlock->w_wait++;
        pthread_cleanup_push(rwl_writecleanup,(void*)rwlock);
        while(rwlock->w_active==1||rwlock->r_active>0){
            status=pthread_cond_wait(&rwlock->write,&rwlock->mutex);
            if(status) break;
        }
        pthread_cleanup_pop(0);
        rwlock->w_wait--;
    }
    if(status==0){
        rwlock->w_active=1;
    }
    pthread_mutex_unlock(&rwlock->mutex);
    return status;
}

int rwl_writetrylock(rwlock_t *rwlock){
    int status,status2;
    if(rwlock->valid!=RWLOCK_VALID){
        return EINVAL;
    }
    status=pthread_mutex_lock(&rwlock->mutex);
    if(status) return status;
    if(rwlock->r_active||rwlock->w_active==1) status=EBUSY;//if there are writers or readers active,return EBUSY
    else rwlock->w_active=1;//else the writer can be active

    status2=pthread_mutex_unlock(&rwlock->mutex);
    return (status!=0?status:status2);
}

int rwl_writeunlock(rwlock_t *rwlock){
    int status;
    if(rwlock->valid!=RWLOCK_VALID){
        return EINVAL;
    }
    status=pthread_mutex_lock(&rwlock->mutex);
    if(status) return status;

    rwlock->w_active=0;
    if(rwlock->r_wait>0){//read advance,so if there are readers waiting ,condition brocast to wake them
        status=pthread_cond_broadcast(&rwlock->read);
        if(status){
            pthread_mutex_unlock(&rwlock->write);
            return status;
        }
    }else if(rwlock->w_wait>0){//if there are no readers waiting,wake up a waiting writers
        status=pthread_cond_signal(&rwlock->write);
        if(status){
            pthread_mutex_unlock(&rwlock->mutex);
            return status;
        }
    }

    status=pthread_mutex_unlock(&rwlock->mutex);

    return status;
}





