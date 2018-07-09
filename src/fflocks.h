#ifndef _FFLOCKS_H_
#define _FFLOCKS_H_

//stolen from Portals 4
# define FFLOCK_TYPE pthread_spinlock_t
# define FFLOCK_INIT(x)    pthread_spin_init((x), PTHREAD_PROCESS_PRIVATE)
# define FFLOCK_DESTROY(x) pthread_spin_destroy(x)
# define FFLOCK_LOCK(x)    pthread_spin_lock(x)
# define FFLOCK_TRYLOCK(x) pthread_spin_trylock(x)
# define FFLOCK_UNLOCK(x) pthread_spin_unlock(x)

#endif // _FFLOCK_H_
