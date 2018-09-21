#ifndef _FFOP_DEFAULT_PROGRESSER_H_
#define _FFOP_DEFAULT_PROGRESSER_H_

#include "ffinternal.h"

int ffop_default_progresser_init();
int ffop_default_progresser_finalize();
int ffop_default_progresser_track(ffop_t * op);
int ffop_default_progresser_progress(ffop_t ** ready_list);

#endif /* _FFOP_DEFAULT_PROGRESSER_H_ */
