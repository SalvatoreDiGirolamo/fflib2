#ifndef _FFCOMP_H_
#define _FFCOMP_H_


typedef struct ffcomp{
    ffop_mem_t buffer1;
    ffop_mem_t buffer2;
    uint32_t count;
    size_t datatype_size;
    ffoperator_internal_t op;
    uint8_t flags;

} ffcomp_t;

#endif /* _FFCOMP_H_ */
