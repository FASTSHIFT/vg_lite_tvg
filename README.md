# vg_lite_tvg
VG-Lite API Simulator based on [ThorVG](https://github.com/thorvg/thorvg)

## LVGL integration
The simulator is integrated into [LVGL](https://github.com/lvgl/lvgl/tree/master/src/others/vg_lite_tvg) and participates in CI compilation and automated testing.

You need to open the configuration in `lv_conf.h`
```c
/* Use VG-Lite GPU. */
#define LV_USE_DRAW_VG_LITE 1

#if LV_USE_DRAW_VG_LITE
/* Enable VG-Lite custom external 'gpu_init()' function */
#define LV_VG_LITE_USE_GPU_INIT 1

/* VG-Lite Simulator */
/*Requires: LV_USE_THORVG_INTERNAL or LV_USE_THORVG_EXTERNAL */
#define LV_USE_VG_LITE_THORVG  1

/*Enable Vector Graphic APIs
 *Requires `LV_USE_MATRIX = 1`*/
#define LV_USE_VECTOR_GRAPHIC  1

/* Enable ThorVG (vector graphics library) from the src/libs folder */
#define LV_USE_THORVG_INTERNAL 1
```

## Tiger Path
![image](https://github.com/FASTSHIFT/vg_lite_tvg/assets/26767803/35734cbc-e814-4715-9b45-b6a1e5f2c252)

## 2.5D Effect
https://github.com/FASTSHIFT/vg_lite_tvg/assets/26767803/d3a8df37-7df8-438d-a440-8e5a5b1660cb

