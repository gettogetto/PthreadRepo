#ifndef RWLOCK_H
#define RWLOCK_H

#include<pthread.h>

typedef struct rwlock_tag{
    pthread_mutex_t mutex;
    pthread_cond_t read;//wait for read
    pthread_cond_t write;//wait for write
    int valid;//set when valid
    int r_active;//numbers of readers active
    int w_active;//numbers of writers active
    int r_wait;//numbers of readers waiting
    int w_wait;//numbers of writers waiting
}rwlock_t;

#define RWLOCK_VALID 0xfacade
#define RWL_INITIALIZER \
{\
    PTHREAD_MUTEX_INITIALIZER,\
    PTHREAD_COND_INITIALIZER,\
    PTHREAD_COND_INITIALIZER,\
    RELOCK_VALID,0,0,0,0\
}
extern int rwl_init(rwlock_t *rwlock);
extern int rwl_destroy(rwlock_t *rwlock);
extern int rwl_readlock(rwlock_t *rwlock);
extern int rwl_readtrylock(rwlock_t *rwlock);
extern int rwl_readunlock(rwlock_t* rwlock);
extern int rwl_writetrylock(rwlock_t *rwlock);
extern int rwl_writelock(rwlock_t* rwlock);
extern int rwl_writeunlock(rwlock_t* rwlock);


#endif // RWLOCK_H
