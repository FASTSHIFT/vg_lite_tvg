#ifndef VG_LITE_CONF_H
#define VG_LITE_CONF_H

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define LV_USE_DRAW_VG_LITE 1

#define LV_USE_VG_LITE_THORVG 1

#define LV_ASSERT assert
#define LV_ASSERT_NULL(ptr) assert(ptr!= NULL)

#define lv_memcpy memcpy
#define lv_free free
static inline void* lv_malloc_zeroed(size_t size) { 
    void* ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}

#define LV_LOG_ERROR printf
#define LV_LOG_TRACE printf

#define LV_UNUSED(x) (void)(x)

/*Enable LVGL's blend mode support*/
#define LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT 0

/*Enable YUV color format support*/
#define LV_VG_LITE_THORVG_YUV_SUPPORT 0

/*Enable Linear gradient extension support*/
#define LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT 0

/*Enable 16 pixels alignment*/
#define LV_VG_LITE_THORVG_16PIXELS_ALIGN 0

/*Buffer address alignment*/
#define LV_VG_LITE_THORVG_BUF_ADDR_ALIGN 4

/*Enable multi-thread render*/
#define LV_VG_LITE_THORVG_THREAD_RENDER 0

#endif /* VG_LITE_CONF_H */
