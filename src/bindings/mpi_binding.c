#include "ff.h"
#include "ffprogress.h"
#include "ffinternal.h"
#include "ffbind.h"
#include "components/mpi/ffmpi.h"
#include "components/mpi/ffop_mpi_progresser.h"
#include "components/gcomp/ffgcomp.h"
#include "components/gcomp/ffop_gcomp.h"


int ffbind(ffdescr_t * ffdescr){

    ffdescr->impl.init = ffmpi_init;
    ffdescr->impl.finalize = ffmpi_finalize;
    ffdescr->impl.get_rank = ffmpi_get_rank;
    ffdescr->impl.get_size = ffmpi_get_size;

    ffdescr->impl.ops[FFSEND].init = ffop_mpi_init;
    ffdescr->impl.ops[FFSEND].post = ffop_mpi_send_post;
    ffdescr->impl.ops[FFSEND].wait = ffop_mpi_send_wait;
    ffdescr->impl.ops[FFSEND].test = ffop_mpi_send_test;
    

    ffdescr->impl.ops[FFRECV].init = ffop_mpi_init;
    ffdescr->impl.ops[FFRECV].post = ffop_mpi_recv_post;
    ffdescr->impl.ops[FFRECV].wait = ffop_mpi_recv_wait;
    ffdescr->impl.ops[FFRECV].test = ffop_mpi_recv_test;

    ffdescr->impl.ops[FFCOMP].init = ffop_gcomp_init;
    ffdescr->impl.ops[FFCOMP].post = ffop_gcomp_post;
    ffdescr->impl.ops[FFCOMP].wait = ffop_gcomp_wait;
    ffdescr->impl.ops[FFCOMP].test = ffop_gcomp_test;

    ffdescr->impl.operator_create = ffop_gcomp_operator_custom_create;
    ffdescr->impl.operator_delete = ffop_gcomp_operator_custom_delete;

    ffprogresser_t mpi_progresser;
    mpi_progresser.init = ffop_mpi_progresser_init;
    mpi_progresser.finalize = ffop_mpi_progresser_finalize;
    mpi_progresser.progress = ffop_mpi_progresser_progress;
    ffprogresser_register(mpi_progresser);

    return FFSUCCESS;
}






