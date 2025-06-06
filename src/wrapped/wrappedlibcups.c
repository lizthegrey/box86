#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include "wrappedlibs.h"

#include "debug.h"
#include "wrapper.h"
#include "bridge.h"
#include "librarian/library_private.h"
#include "x86emu.h"
#include "emu/x86emu_private.h"
#include "callback.h"
#include "librarian.h"
#include "box86context.h"
#include "emu/x86emu_private.h"
#include "myalign.h"

const char* libcupsName =
#ifdef ANDROID
    "libcups.so"
#else
    "libcups.so.2"
#endif
    ;
#define LIBNAME libcups

#if 0
#define SUPER()
    GO(BZ2_bzCompressInit, iFpiii_t)

typedef struct libcups_my_s {
    // functions
    #define GO(A, B)    B   A;
    SUPER()
    #undef GO
} libcups_my_t;

void* getCupsMy(library_t* lib)
{
    libcups_my_t* my = (libcups_my_t*)calloc(1, sizeof(libcups_my_t));
    #define GO(A, W) my->A = (W)dlsym(lib->priv.w.lib, #A);
    SUPER()
    #undef GO
    return my;
}
#undef SUPER

void freeCupsMy(void* lib)
{
    libcups_my_t *my = (libcups_my_t *)lib;
}

#define CUSTOM_INIT \
    lib->priv.w.p2 = getCupsMy(lib);

#define CUSTOM_FINI \
    freeCupsMy(lib->priv.w.p2); \
    free(lib->priv.w.p2);
#endif

#include "wrappedlib_init.h"

