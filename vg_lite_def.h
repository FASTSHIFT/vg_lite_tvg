#ifndef VG_LITE_CONF_H
#define VG_LITE_CONF_H

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define LV_USE_VG_LITE_THORVG 1

#define LV_USE_DRAW_VG_LITE 1

/* Enable VG-Lite custom external 'gpu_init()' function */
#ifndef LV_VG_LITE_USE_GPU_INIT
#define LV_VG_LITE_USE_GPU_INIT 1

#ifndef LV_VG_LITE_USE_GPU_INIT_ONCE
#define LV_VG_LITE_USE_GPU_INIT_ONCE 1
#endif

#endif

#ifndef LV_USE_VG_LITE_MAIN
#define LV_USE_VG_LITE_MAIN 1
#endif

/* 1: Use thorvg release version, 0: Use thorvg main branch version */
#ifndef LV_VG_LITE_THORVG_USE_RELEASE
#define LV_VG_LITE_THORVG_USE_RELEASE 0
#endif

#define LV_ASSERT assert
#define LV_ASSERT_NULL(ptr) assert(ptr != NULL)

#define lv_memzero(dst, size) memset(dst, 0, size)
#define lv_memcpy memcpy
#define lv_free free
static inline void* lv_malloc_zeroed(size_t size)
{
    void* ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}

#define LV_LOG_USER printf
#define LV_LOG_ERROR printf
#define LV_LOG_WARN printf
#define LV_LOG_INFO printf
#define LV_LOG_TRACE printf

#define LV_UNUSED(x) (void)(x)

/*Enable LVGL's blend mode support*/
#define LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT 0

/*Enable YUV color format support*/
#ifndef LV_VG_LITE_THORVG_YUV_SUPPORT
#define LV_VG_LITE_THORVG_YUV_SUPPORT 0
#endif

/*Enable Linear gradient extension support*/
#define LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT 0

/*Enable 16 pixels alignment*/
#define LV_VG_LITE_THORVG_16PIXELS_ALIGN 1

/*Buffer address alignment*/
#define LV_VG_LITE_THORVG_BUF_ADDR_ALIGN 64

/*Enable multi-thread render*/
#define LV_VG_LITE_THORVG_THREAD_RENDER 0

#endif /* VG_LITE_CONF_H */
