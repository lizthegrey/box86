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

const char* libsmName = "libSM.so.6";
#define LIBNAME libsm

typedef int     (*iFppp_t)          (void*, void*, void*);
typedef int     (*iFpipp_t)         (void*, int, void*, void*);
typedef void*   (*pFppiiLpppip_t)   (void*, void*, int, int, unsigned long, void*, void*, void*, int, void*);

#define SUPER() \
    GO(SmcOpenConnection, pFppiiLpppip_t)   \
    GO(SmcInteractRequest, iFpipp_t)        \
    GO(SmcRequestSaveYourselfPhase2, iFppp_t)

typedef struct libsm_my_s {
    // functions
    #define GO(A, B)    B   A;
    SUPER()
    #undef GO
} libsm_my_t;

void* getSMMy(library_t* lib)
{
    libsm_my_t* my = (libsm_my_t*)calloc(1, sizeof(libsm_my_t));
    #define GO(A, W) my->A = (W)dlsym(lib->priv.w.lib, #A);
    SUPER()
    #undef GO
    return my;
}
#undef SUPER

void freeSMMy(void* lib)
{
    //libsm_my_t *my = (libsm_my_t *)lib;
}

typedef struct my_SmcCallbacks_s {
    struct {
	void*	 callback;
	void*	 client_data;
    } save_yourself;

    struct {
	void*	 callback;
	void*	 client_data;
    } die;

    struct {
	void*	 callback;
	void*		 client_data;
    } save_complete;

    struct {
	void* callback;
	void*		 client_data;
    } shutdown_cancelled;
} my_SmcCallbacks_t;
#define SmcSaveYourselfProcMask		    (1L << 0)
#define SmcDieProcMask			        (1L << 1)
#define SmcSaveCompleteProcMask		    (1L << 2)
#define SmcShutdownCancelledProcMask	(1L << 3)

static uintptr_t my_save_yourself_fct = 0;
static void my_save_yourself(void* smcConn, void* clientData, int saveType, int shutdown, int interactStyle, int fast)
{
    RunFunction(my_context, my_save_yourself_fct, 6, smcConn, clientData, saveType, shutdown, interactStyle, fast);
}

static uintptr_t my_die_fct = 0;
static void my_die(void* smcConn, void* clientData)
{
    RunFunction(my_context, my_die_fct, 2, smcConn, clientData);
}

static uintptr_t my_shutdown_cancelled_fct = 0;
static void my_shutdown_cancelled(void* smcConn, void* clientData)
{
    RunFunction(my_context, my_shutdown_cancelled_fct, 2, smcConn, clientData);
}

static uintptr_t my_save_complete_fct = 0;
static void my_save_complete(void* smcConn, void* clientData)
{
    RunFunction(my_context, my_save_complete_fct, 2, smcConn, clientData);
}


EXPORT void* my_SmcOpenConnection(x86emu_t* emu, void* networkIdsList, void* context, int major, int minor, unsigned long mask, my_SmcCallbacks_t* cb, void* previousId, void* clientIdRet, int errorLength, void* errorRet)
{
    libsm_my_t* my = (libsm_my_t*)GetLibInternal(libsmName)->priv.w.p2;
    my_SmcCallbacks_t nat = {0};
    #define GO(A, B) if(mask&A) {my_##B##_fct = (uintptr_t)cb->B.callback; nat.B.callback = my_##B; nat.B.client_data=cb->B.client_data;}
    GO(SmcSaveYourselfProcMask, save_yourself)
    GO(SmcDieProcMask, die)
    GO(SmcSaveCompleteProcMask, save_complete)
    GO(SmcShutdownCancelledProcMask, shutdown_cancelled)
    #undef GO
    return my->SmcOpenConnection(networkIdsList, context, major, minor, mask, &nat, previousId, clientIdRet, errorLength, errorRet);
}

// utility functions
#define SUPER() \
GO(0)   \
GO(1)   \
GO(2)   \
GO(3)   \
GO(4)

// Request
#define GO(A)   \
static uintptr_t my_Request_fct_##A = 0;        \
static void my_Request_##A(void* a, void* b)     \
{                                               \
    RunFunction(my_context, my_Request_fct_##A, 2, a, b);\
}
SUPER()
#undef GO
static void* findRequestFct(void* fct)
{
    if(!fct) return NULL;
    void* p;
    if((p = GetNativeFnc((uintptr_t)fct))) return p;
    #define GO(A) if(my_Request_fct_##A == (uintptr_t)fct) return my_Request_##A;
    SUPER()
    #undef GO
    #define GO(A) if(my_Request_fct_##A == 0) {my_Request_fct_##A = (uintptr_t)fct; return my_Request_##A; }
    SUPER()
    #undef GO
    printf_log(LOG_NONE, "Warning, no more slot for libSM Request callback\n");
    return NULL;
}

#undef SUPER

EXPORT int my_SmcInteractRequest(x86emu_t* emu, void* smcConn, int f, void* cb, void* data)
{
    libsm_my_t* my = (libsm_my_t*)GetLibInternal(libsmName)->priv.w.p2;

    return my->SmcInteractRequest(smcConn, f, findRequestFct(cb), data);
}

EXPORT int my_SmcRequestSaveYourselfPhase2(x86emu_t* emu, void* smcConn, void* cb, void* data)
{
    libsm_my_t* my = (libsm_my_t*)GetLibInternal(libsmName)->priv.w.p2;

    return my->SmcRequestSaveYourselfPhase2(smcConn, findRequestFct(cb), data);
}

#define CUSTOM_INIT \
    lib->priv.w.p2 = getSMMy(lib);

#define CUSTOM_FINI \
    freeSMMy(lib->priv.w.p2); \
    free(lib->priv.w.p2);

#include "wrappedlib_init.h"

