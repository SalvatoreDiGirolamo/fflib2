#ifndef _FFMPI_H_
#define _FFMPI_H_

int ffmpi_init(int * argc, char *** argv);
int ffmpi_finalize();

int ffmpi_get_rank();
int ffmpi_get_size();

#endif /* _FFMPI_H_ */
