#ifndef _FFSCHEDULE_H_
#define _FFSCHEDULE_H_



typedef struct ffschedule{

    /* list of executable ops */
    ffop_t * executable_ops;     
    ffop_mem_set_t * memory_descriptors; 

} ffschedule_t;

typedef ffschedule_t * ffschedule_h;

int ffschedule_post(ffschedule_h schedule);

#endif /* _FFSCHEDULE_H_ */
