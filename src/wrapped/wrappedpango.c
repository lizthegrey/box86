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

const char* pangoName = "libpango-1.0.so.0";
#define LIBNAME pango
static library_t *my_lib = NULL;

#include "generated/wrappedpangotypes.h"

typedef struct pango_my_s {
    // functions
    #define GO(A, B)    B   A;
    SUPER()
    #undef GO
} pango_my_t;

void* getPangoMy(library_t* lib)
{
    pango_my_t* my = (pango_my_t*)calloc(1, sizeof(pango_my_t));
    #define GO(A, W) my->A = (W)dlsym(lib->priv.w.lib, #A);
    SUPER()
    #undef GO
    return my;
}
#undef SUPER

void freePangoMy(void* lib)
{
    //pango_my_t *my = (pango_my_t *)lib;
}

typedef struct my_PangoAttrClass_s {
  int                type;
  void*            (*copy) (void *attr);
  void             (*destroy) (void *attr);
  int              (*equal) (void *attr1, void *attr2);
} my_PangoAttrClass_t;

// utility functions
#define SUPER() \
GO(0)   \
GO(1)   \
GO(2)   \
GO(3)   \
GO(4)

// PangoAttrClass
#define GO(A)   \
static my_PangoAttrClass_t* my_PangoAttrClass_used_##A = NULL;  \
static my_PangoAttrClass_t my_PangoAttrClass_struct_##A = {0};  \
static uintptr_t my_PangoAttrClass_copy_##A = 0;                \
static void* my_PangoAttrClass_copyfct##A(void* attr)           \
{                                                               \
    return (void*)RunFunction(my_context, my_PangoAttrClass_copy_##A, 1, attr);  \
}                                                               \
static uintptr_t my_PangoAttrClass_del_##A = 0;                 \
static void my_PangoAttrClass_delfct##A(void* attr)             \
{                                                               \
    RunFunction(my_context, my_PangoAttrClass_del_##A, 1, attr);\
}                                                               \
static uintptr_t my_PangoAttrClass_equal_##A = 0;               \
static int my_PangoAttrClass_equalfct##A(void* a, void* b)      \
{                                                               \
    return (int)RunFunction(my_context, my_PangoAttrClass_equal_##A, 2, a, b);\
}
SUPER()
#undef GO
static void* find_PangoAttrClass_Fct(my_PangoAttrClass_t* klass)
{
    if(!klass) return NULL;
    #define GO(A) if(my_PangoAttrClass_used_##A == klass) return &my_PangoAttrClass_struct_##A;
    SUPER()
    #undef GO
    #define GO(A) if(my_PangoAttrClass_used_##A == 0) {         \
        my_PangoAttrClass_used_##A = klass;                     \
        my_PangoAttrClass_t*p=&my_PangoAttrClass_struct_##A;    \
        p->type = klass->type;                                  \
        p->copy = my_PangoAttrClass_copyfct##A;                 \
        my_PangoAttrClass_copy_##A = (uintptr_t)klass->copy;    \
        p->destroy = my_PangoAttrClass_delfct##A;               \
        my_PangoAttrClass_del_##A = (uintptr_t)klass->destroy;  \
        p->equal = my_PangoAttrClass_equalfct##A;               \
        my_PangoAttrClass_equal_##A = (uintptr_t)klass->equal;  \
        return p;                                               \
    }
    SUPER()
    #undef GO
    printf_log(LOG_NONE, "Warning, no more slot for pango PangoAttrClass klass\n");
    return NULL;
}
// AttrFilter
#define GO(A)   \
static uintptr_t my_AttrFilter_fct_##A = 0;                                 \
static int my_AttrFilter_##A(void* a, void* b)                              \
{                                                                           \
    return (int)RunFunction(my_context, my_AttrFilter_fct_##A, 2, a, b);    \
}
SUPER()
#undef GO
static void* find_AttrFilter_Fct(void* fct)
{
    if(!fct) return fct;
    if(GetNativeFnc((uintptr_t)fct))  return GetNativeFnc((uintptr_t)fct);
    #define GO(A) if(my_AttrFilter_fct_##A == (uintptr_t)fct) return my_AttrFilter_##A;
    SUPER()
    #undef GO
    #define GO(A) if(my_AttrFilter_fct_##A == 0) {my_AttrFilter_fct_##A = (uintptr_t)fct; return my_AttrFilter_##A; }
    SUPER()
    #undef GO
    printf_log(LOG_NONE, "Warning, no more slot for pango AttrFilter callback\n");
    return NULL;
}
// AttrDataCopy
#define GO(A)   \
static uintptr_t my_AttrDataCopy_fct_##A = 0;                               \
static void* my_AttrDataCopy_##A(void* a)                                   \
{                                                                           \
    return (void*)RunFunction(my_context, my_AttrDataCopy_fct_##A, 1, a);   \
}
SUPER()
#undef GO
static void* find_AttrDataCopy_Fct(void* fct)
{
    if(!fct) return fct;
    if(GetNativeFnc((uintptr_t)fct))  return GetNativeFnc((uintptr_t)fct);
    #define GO(A) if(my_AttrDataCopy_fct_##A == (uintptr_t)fct) return my_AttrDataCopy_##A;
    SUPER()
    #undef GO
    #define GO(A) if(my_AttrDataCopy_fct_##A == 0) {my_AttrDataCopy_fct_##A = (uintptr_t)fct; return my_AttrDataCopy_##A; }
    SUPER()
    #undef GO
    printf_log(LOG_NONE, "Warning, no more slot for pango AttrDataCopy callback\n");
    return NULL;
}
// GDestroyNotify
#define GO(A)   \
static uintptr_t my_GDestroyNotify_fct_##A = 0;   \
static void my_GDestroyNotify_##A(void* data)     \
{                                       \
    RunFunction(my_context, my_GDestroyNotify_fct_##A, 1, data);\
}
SUPER()
#undef GO
static void* findGDestroyNotifyFct(void* fct)
{
    if(!fct) return fct;
    if(GetNativeFnc((uintptr_t)fct))  return GetNativeFnc((uintptr_t)fct);
    #define GO(A) if(my_GDestroyNotify_fct_##A == (uintptr_t)fct) return my_GDestroyNotify_##A;
    SUPER()
    #undef GO
    #define GO(A) if(my_GDestroyNotify_fct_##A == 0) {my_GDestroyNotify_fct_##A = (uintptr_t)fct; return my_GDestroyNotify_##A; }
    SUPER()
    #undef GO
    printf_log(LOG_NONE, "Warning, no more slot for pango GDestroyNotify callback\n");
    return NULL;
}

#undef SUPER

EXPORT void my_pango_attribute_init(x86emu_t* emu, void* attr, my_PangoAttrClass_t* klass)
{
    pango_my_t* my = (pango_my_t*)my_lib->priv.w.p2;

    my->pango_attribute_init(attr, find_PangoAttrClass_Fct(klass));
}

EXPORT void* my_pango_attr_list_filter(x86emu_t* emu, void* list, void* f, void* data)
{
    pango_my_t* my = (pango_my_t*)my_lib->priv.w.p2;

    return my->pango_attr_list_filter(list, find_AttrFilter_Fct(f), data);
}

EXPORT void* my_pango_attr_shape_new_with_data(x86emu_t* emu, void* ink, void* loc, void* data, void* f, void* d)
{
    pango_my_t* my = (pango_my_t*)my_lib->priv.w.p2;

    return my->pango_attr_shape_new_with_data(ink, loc, data, find_AttrDataCopy_Fct(f), findGDestroyNotifyFct(d));
}

#define PRE_INIT    \
    if(box86_nogtk) \
        return -1;

#define CUSTOM_INIT \
    my_lib = lib;                   \
    lib->priv.w.p2 = getPangoMy(lib); \
    lib->priv.w.needed = 2;         \
    lib->priv.w.neededlibs = (char**)calloc(lib->priv.w.needed, sizeof(char*)); \
    lib->priv.w.neededlibs[0] = strdup("libgobject-2.0.so.0");                  \
    lib->priv.w.neededlibs[1] = strdup("libglib-2.0.so.0");

#define CUSTOM_FINI \
    my_lib = NULL;              \
    freePangoMy(lib->priv.w.p2);  \
    free(lib->priv.w.p2);

#include "wrappedlib_init.h"
