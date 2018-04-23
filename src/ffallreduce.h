#ifndef _SALLREDUCE_H_
#define _SALLREDUCE_H_

int ffsolo_allreduce_create(void * sendbuf, void * recvbuf, int count, sdatatype_t datatype, soperator_t op, int options, MPI_Comm comm, scollective_h * coll);

int ffsolo_allreduce_increase_async_degree(scollective_h coll, void * sendbuf, void * recvbuf); 

int ffcollective_start(scollective_t coll);

#endif /* _SALLREDUCE_H_ */
