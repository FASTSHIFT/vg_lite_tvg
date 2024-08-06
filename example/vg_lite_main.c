/*
 * MIT License
 * Copyright (c) 2023 - 2024 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*********************
 *      INCLUDES
 *********************/

#include "../vg_lite_def.h"

#if LV_USE_VG_LITE_MAIN

#include "vg_lite.h"
#include <getopt.h>
#include <png.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/* for aligned_alloc */
#ifndef __USE_ISOC11
    #define __USE_ISOC11
#endif
#include <stdlib.h>

#ifdef CACHE_API_H
#include CACHE_API_H
#else
#define CACHE_FLUSH()
#define CACHE_INVALIDATE()
#endif /* CACHE_API_H */

/*********************
 *      DEFINES
 *********************/

#define VG_LITE_PREFIX "VG-Lite: "

#define IS_STR_EQUAL(str1, str2) (strcmp(str1, str2) == 0)

#define VG_LITE_IS_ERROR(err) ((err) != VG_LITE_SUCCESS)

#define VG_LITE_CHECK_ERROR(func)                                        \
    do {                                                                 \
        vg_lite_error_t error_code = func;                               \
        if (VG_LITE_IS_ERROR(error_code)) {                              \
            printf(VG_LITE_PREFIX "Execute '" #func "' error: %d(%s)\n", \
                (int)error_code, vg_lite_error_string(error_code));      \
            goto error_handler;                                          \
        }                                                                \
    } while (0)

#define VG_LITE_ALIGN(number, align_bytes) \
    (((number) + ((align_bytes) - 1)) & ~((align_bytes) - 1))

#define STRING_TO_ENUM(e)          \
    do {                           \
        if (IS_STR_EQUAL(str, #e)) \
            return e;              \
    } while (0)

#define ENUM_TO_STRING(e) \
    case (e):             \
        return #e

/**********************
 *      TYPEDEFS
 **********************/

typedef struct
{
    const char* output_path;
    const char* func_name;

    vg_lite_buffer_t target;
    vg_lite_buffer_t source;

    vg_lite_path_t path;

    vg_lite_matrix_t matrix;
    vg_lite_matrix_t pattern_matrix;
    vg_lite_rectangle_t rect;

    vg_lite_blend_t blend;
    vg_lite_fill_t fill_rule;
    vg_lite_filter_t filter;
    vg_lite_pattern_mode_t pattern_mode;

    vg_lite_color_t color;
    vg_lite_color_t pattern_color;
    vg_lite_color_t source_color;
} vg_lite_context_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void vg_lite_context_init(vg_lite_context_t* context);
static void vg_lite_context_deinit(vg_lite_context_t* context);
static void vg_lite_run_test(vg_lite_context_t* context);
static int parse_commandline(int argc, char** argv, vg_lite_context_t* context);
static const char* vg_lite_error_string(vg_lite_error_t error);
static int save_buffer(const char* filename, const vg_lite_buffer_t* buffer);
static vg_lite_error_t alloc_buffer(vg_lite_buffer_t* buffer);
static vg_lite_error_t free_buffer(vg_lite_buffer_t* buffer);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

int main(int argc, char** argv)
{
#if LV_VG_LITE_USE_GPU_INIT
#if LV_VG_LITE_USE_GPU_INIT_ONCE
    static bool is_initialized = false;
    if (!is_initialized) {
#endif
        /* Initialize the GPU */
        extern void gpu_init(void);
        gpu_init();
#if LV_VG_LITE_USE_GPU_INIT_ONCE
        is_initialized = true;
    }
#endif
#endif

    vg_lite_context_t context;
    vg_lite_context_init(&context);

    if (parse_commandline(argc, argv, &context) < 0) {
        goto error_handler;
    }

    /* Create a target buffer */
    VG_LITE_CHECK_ERROR(alloc_buffer(&context.target));
    VG_LITE_CHECK_ERROR(alloc_buffer(&context.source));

    vg_lite_run_test(&context);

    save_buffer(context.output_path, &context.target);

error_handler:
    vg_lite_context_deinit(&context);

#if LV_VG_LITE_USE_GPU_INIT && !LV_VG_LITE_USE_GPU_INIT_ONCE
    /* Deinitialize the GPU */
    extern void gpu_deinit(void);
    gpu_deinit();
#endif

    return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#include "vg_lite_custom.h"

static void vg_lite_context_init(vg_lite_context_t* context)
{
    memset(context, 0, sizeof(vg_lite_context_t));
    context->output_path = "target.png";
    context->func_name = "vg_lite_draw_custom";
    context->target.format = VG_LITE_BGRA8888;
    context->target.width = 480;
    context->target.height = 480;
    context->source.format = VG_LITE_BGRA8888;
    context->source.width = 240;
    context->source.height = 240;
    context->source_color = 0xFFFF0000;
    vg_lite_identity(&context->matrix);
    vg_lite_identity(&context->pattern_matrix);
    context->rect.x = 0;
    context->rect.y = 0;
    context->rect.width = 480;
    context->rect.height = 480;
    context->blend = VG_LITE_BLEND_SRC_OVER;
    context->fill_rule = VG_LITE_FILL_EVEN_ODD;
    context->filter = VG_LITE_FILTER_BI_LINEAR;

    vg_lite_init_path(
        &context->path,
        VG_LITE_S16,
        VG_LITE_HIGH,
        sizeof(custom_path_data),
        (void*)custom_path_data,
        0, 0, 100, 100);
}

static void vg_lite_context_deinit(vg_lite_context_t* context)
{
    if (context->target.memory) {
        free_buffer(&context->target);
    }

    if (context->source.memory) {
        free_buffer(&context->source);
    }
}

static void vg_lite_run_test(vg_lite_context_t* context)
{
    printf(VG_LITE_PREFIX "Running test function: %s\n", context->func_name);

    /* Clear buffer */
    VG_LITE_CHECK_ERROR(vg_lite_clear(
        &context->target,
        NULL,
        0xFFFFFFFF));
    VG_LITE_CHECK_ERROR(vg_lite_clear(
        &context->source,
        NULL,
        context->source_color));

    /* Run test function */
    if (IS_STR_EQUAL(context->func_name, "vg_lite_draw_custom")) {
        VG_LITE_CHECK_ERROR(vg_lite_draw_custom(context));
    } else if (IS_STR_EQUAL(context->func_name, "vg_lite_clear")) {
        VG_LITE_CHECK_ERROR(vg_lite_clear(
            &context->target,
            &context->rect,
            context->color));
    } else if (IS_STR_EQUAL(context->func_name, "vg_lite_blit")) {
        VG_LITE_CHECK_ERROR(vg_lite_blit(
            &context->target,
            &context->source,
            &context->matrix,
            context->blend,
            context->color,
            context->filter));
    } else if (IS_STR_EQUAL(context->func_name, "vg_lite_blit_rect")) {
        VG_LITE_CHECK_ERROR(vg_lite_blit_rect(
            &context->target,
            &context->source,
            &context->rect,
            &context->matrix,
            context->blend,
            context->color,
            context->filter));
    } else if (IS_STR_EQUAL(context->func_name, "vg_lite_draw")) {
        VG_LITE_CHECK_ERROR(vg_lite_draw(
            &context->target,
            &context->path,
            context->fill_rule,
            &context->matrix,
            context->blend,
            context->color));
    } else if (IS_STR_EQUAL(context->func_name, "vg_lite_draw_pattern")) {
        VG_LITE_CHECK_ERROR(vg_lite_draw_pattern(
            &context->target,
            &context->path,
            context->fill_rule,
            &context->matrix,
            &context->source,
            &context->pattern_matrix,
            context->blend,
            context->pattern_mode,
            context->pattern_color,
            context->color,
            context->filter));
    } else {
        printf(VG_LITE_PREFIX "Unknown function name: %s\n", context->func_name);
    }

    VG_LITE_CHECK_ERROR(vg_lite_finish());

error_handler:
    printf(VG_LITE_PREFIX "Test finished\n");
}

static void show_usage(const char* progname)
{
    printf("\nUsage: %s"
           " -h"
           " -o <string>"
           " --func <string>"
           " --target <string>"
           " --source <string>"
           " --matrix <string>"
           " --pattern-matrix <string>\n"
           " --rect <string>"
           " --path-bounding-box <string>"
           " --blend <string>"
           " --fill-rule <string>\n"
           " --filter <string>"
           " --pattern-mode <string>"
           " --color <hex-value>"
           " --pattern-color <hex-value>"
           " --source-color <hex-value>\n"
           " --scissor <string>"
           "\n",
        progname);

    printf("\nWhere:\n");
    printf("  -h Show this help message.\n");
    printf("  -o <string> Output file path.\n");
    printf("  --func <string> Test case function name.\n");
    printf("  --target <string> Target buffer arguments in the format of 'width,height,format'.\n");
    printf("  --source <string> Source buffer arguments in the format of 'width,height,format'.\n");
    printf("  --matrix <string> Matrix arguments in the format of 'm[3][3]' (e.g. '1,0,0,0,1,0,0,0,1').\n");
    printf("  --pattern-matrix <string> Pattern matrix arguments in the format of 'm[3][3]' (e.g. '1,0,0,0,1,0,0,0,1').\n");
    printf("  --rect <string> Rectangle arguments in the format of 'x,y,width,height' (e.g. '0,0,100,100').\n");
    printf("  --path-bounding-box <string> Path bonding box arguments in the format of 'min_x,min_y,max_x,max_y' (e.g. '0,0,100,100').\n");
    printf("  --blend <string> Blend mode, see enum vg_lite_blend_t.\n");
    printf("  --fill-rule <string> Fill rule, see enum vg_lite_fill_t.\n");
    printf("  --filter <string> Filter', see enum vg_lite_filter_t.\n");
    printf("  --pattern-mode <string> Pattern mode', see enum vg_lite_pattern_mode_t.\n");
    printf("  --color <hex-value> Color in the format of 'AABBGGRR'.\n");
    printf("  --pattern-color <hex-value> Pattern Color in the format of 'AABBGGRR'.\n");
    printf("  --source-color <hex-value> Source color in the format of 'AABBGGRR'.\n");
    printf("  --scissor <string> Scissor area arguments in the format of 'x,y,right,bottom' (e.g. '0,0,100,100').\n");
}

static const char* vg_lite_error_string(vg_lite_error_t error)
{
    switch (error) {
        ENUM_TO_STRING(VG_LITE_SUCCESS);
        ENUM_TO_STRING(VG_LITE_INVALID_ARGUMENT);
        ENUM_TO_STRING(VG_LITE_OUT_OF_MEMORY);
        ENUM_TO_STRING(VG_LITE_NO_CONTEXT);
        ENUM_TO_STRING(VG_LITE_TIMEOUT);
        ENUM_TO_STRING(VG_LITE_OUT_OF_RESOURCES);
        ENUM_TO_STRING(VG_LITE_GENERIC_IO);
        ENUM_TO_STRING(VG_LITE_NOT_SUPPORT);
        ENUM_TO_STRING(VG_LITE_ALREADY_EXISTS);
        ENUM_TO_STRING(VG_LITE_NOT_ALIGNED);
        ENUM_TO_STRING(VG_LITE_FLEXA_TIME_OUT);
        ENUM_TO_STRING(VG_LITE_FLEXA_HANDSHAKE_FAIL);
    default:
        break;
    }
    return "UNKNOW_ERROR";
}

static vg_lite_buffer_format_t parse_buffer_format_args(const char* str)
{
    STRING_TO_ENUM(VG_LITE_RGBA8888);
    STRING_TO_ENUM(VG_LITE_BGRA8888);
    STRING_TO_ENUM(VG_LITE_RGBX8888);
    STRING_TO_ENUM(VG_LITE_BGRX8888);
    STRING_TO_ENUM(VG_LITE_RGB565);
    STRING_TO_ENUM(VG_LITE_BGR565);
    STRING_TO_ENUM(VG_LITE_RGBA4444);
    STRING_TO_ENUM(VG_LITE_BGRA4444);
    STRING_TO_ENUM(VG_LITE_BGRA5551);
    STRING_TO_ENUM(VG_LITE_A4);
    STRING_TO_ENUM(VG_LITE_A8);
    STRING_TO_ENUM(VG_LITE_L8);
    STRING_TO_ENUM(VG_LITE_YUYV);
    STRING_TO_ENUM(VG_LITE_YUY2);
    STRING_TO_ENUM(VG_LITE_ANV12);
    STRING_TO_ENUM(VG_LITE_AYUY2);
    STRING_TO_ENUM(VG_LITE_NV12);
    STRING_TO_ENUM(VG_LITE_YV12);
    STRING_TO_ENUM(VG_LITE_YV24);
    STRING_TO_ENUM(VG_LITE_YV16);
    STRING_TO_ENUM(VG_LITE_NV16);
    STRING_TO_ENUM(VG_LITE_YUY2_TILED);
    STRING_TO_ENUM(VG_LITE_NV12_TILED);
    STRING_TO_ENUM(VG_LITE_ANV12_TILED);
    STRING_TO_ENUM(VG_LITE_AYUY2_TILED);
    STRING_TO_ENUM(VG_LITE_RGBA2222);
    STRING_TO_ENUM(VG_LITE_BGRA2222);
    STRING_TO_ENUM(VG_LITE_ABGR2222);
    STRING_TO_ENUM(VG_LITE_ARGB2222);
    STRING_TO_ENUM(VG_LITE_ABGR4444);
    STRING_TO_ENUM(VG_LITE_ARGB4444);
    STRING_TO_ENUM(VG_LITE_ABGR8888);
    STRING_TO_ENUM(VG_LITE_ARGB8888);
    STRING_TO_ENUM(VG_LITE_ABGR1555);
    STRING_TO_ENUM(VG_LITE_RGBA5551);
    STRING_TO_ENUM(VG_LITE_ARGB1555);
    STRING_TO_ENUM(VG_LITE_XBGR8888);
    STRING_TO_ENUM(VG_LITE_XRGB8888);
    STRING_TO_ENUM(VG_LITE_RGBA8888_ETC2_EAC);
    STRING_TO_ENUM(VG_LITE_RGB888);
    STRING_TO_ENUM(VG_LITE_BGR888);
    STRING_TO_ENUM(VG_LITE_ABGR8565);
    STRING_TO_ENUM(VG_LITE_BGRA5658);
    STRING_TO_ENUM(VG_LITE_ARGB8565);
    STRING_TO_ENUM(VG_LITE_RGBA5658);
    STRING_TO_ENUM(VG_LITE_ABGR8565_PLANAR);
    STRING_TO_ENUM(VG_LITE_BGRA5658_PLANAR);
    STRING_TO_ENUM(VG_LITE_ARGB8565_PLANAR);
    STRING_TO_ENUM(VG_LITE_RGBA5658_PLANAR);
    STRING_TO_ENUM(VG_LITE_INDEX_1);
    STRING_TO_ENUM(VG_LITE_INDEX_2);
    STRING_TO_ENUM(VG_LITE_INDEX_4);
    STRING_TO_ENUM(VG_LITE_INDEX_8);

    printf(VG_LITE_PREFIX "Unknown buffer format: %s\n", str);
    return VG_LITE_BGRA8888;
}

static int parse_buffer_args(vg_lite_buffer_t* buffer, const char* str)
{
    char format_str[32] = { 0 };
    int num = sscanf(str, "%d,%d,%31s", &buffer->width, &buffer->height, format_str);
    if (num != 3) {
        printf(VG_LITE_PREFIX "Invalid buffer arguments: %s\n", str);
        return -1;
    }

    buffer->format = parse_buffer_format_args(format_str);

    return 0;
}

static int parse_matrix_args(vg_lite_matrix_t* matrix, const char* str)
{
    int num = sscanf(str,
        "%f,%f,%f,%f,%f,%f,%f,%f,%f",
        &matrix->m[0][0], &matrix->m[0][1], &matrix->m[0][2],
        &matrix->m[1][0], &matrix->m[1][1], &matrix->m[1][2],
        &matrix->m[2][0], &matrix->m[2][1], &matrix->m[2][2]);

    if (num != 9) {
        printf(VG_LITE_PREFIX "Invalid matrix arguments: %s\n", str);
        return -1;
    }

    return 0;
}

static int parse_rect_args(vg_lite_rectangle_t* rect, const char* str)
{
    int num = sscanf(str, "%d,%d,%d,%d", &rect->x, &rect->y, &rect->width, &rect->height);
    if (num != 4) {
        printf(VG_LITE_PREFIX "Invalid rectangle arguments: %s\n", str);
        return -1;
    }

    return 0;
}

static vg_lite_color_t parse_color_args(const char* str)
{
    char* ptr;
    uint32_t color = strtoul(str, &ptr, 16);
    if (*ptr != '\0') {
        printf(VG_LITE_PREFIX "Invalid color arguments: %s\n", str);
        return -1;
    }

    return color;
}

static int parse_path_bounding_box_args(vg_lite_path_t* path, const char* str)
{
    int num = sscanf(str,
        "%f,%f,%f,%f",
        &path->bounding_box[0],
        &path->bounding_box[1],
        &path->bounding_box[2],
        &path->bounding_box[3]);
    if (num != 4) {
        printf(VG_LITE_PREFIX "Invalid path bounding box arguments: %s\n", str);
        return -1;
    }

    return 0;
}

static vg_lite_blend_t parse_blend_args(const char* str)
{
    STRING_TO_ENUM(VG_LITE_BLEND_NONE); /*! S, No blend, Non-premultiplied */
    STRING_TO_ENUM(VG_LITE_BLEND_SRC_OVER); /*! S + (1 - Sa) * D , Non-premultiplied */
    STRING_TO_ENUM(VG_LITE_BLEND_DST_OVER); /*! (1 - Da) * S + D , Non-premultiplied */
    STRING_TO_ENUM(VG_LITE_BLEND_SRC_IN); /*! Da * S , Non-premultiplied */
    STRING_TO_ENUM(VG_LITE_BLEND_DST_IN); /*! Sa * D , Non-premultiplied */
    STRING_TO_ENUM(VG_LITE_BLEND_MULTIPLY); /*! S * (1 - Da) + D * (1 - Sa) + S * D , Non-premultiplied */
    STRING_TO_ENUM(VG_LITE_BLEND_SCREEN); /*! S + D - S * D , Non-premultiplied */
    STRING_TO_ENUM(VG_LITE_BLEND_DARKEN); /*! min(SrcOver, DstOver) , Non-premultiplied */
    STRING_TO_ENUM(VG_LITE_BLEND_LIGHTEN); /*! max(SrcOver, DstOver) , Non-premultiplied */
    STRING_TO_ENUM(VG_LITE_BLEND_ADDITIVE); /*! S + D , Non-premultiplied */
    STRING_TO_ENUM(VG_LITE_BLEND_SUBTRACT); /*! D * (1 - Sa) , Non-premultiplied */
    STRING_TO_ENUM(VG_LITE_BLEND_SUBTRACT_LVGL); /*! D - S , Non-premultiplied */
    STRING_TO_ENUM(VG_LITE_BLEND_NORMAL_LVGL); /*! S * Sa + (1 - Sa) * D , Non-premultiplied */
    STRING_TO_ENUM(VG_LITE_BLEND_ADDITIVE_LVGL); /*! (S + D) * Sa + D * (1 - Sa) , Non-premultiplied */
    STRING_TO_ENUM(VG_LITE_BLEND_MULTIPLY_LVGL); /*! (S * D) * Sa + D * (1 - Sa) , Non-premultiplied */
    STRING_TO_ENUM(VG_LITE_BLEND_PREMULTIPLY_SRC_OVER); /*! S * Sa + (1 - Sa) * D , Non-premultiplied */

    printf(VG_LITE_PREFIX "Unknown blend mode: %s\n", str);
    return VG_LITE_BLEND_NONE;
}

static vg_lite_fill_t parse_fill_rule_args(const char* str)
{
    STRING_TO_ENUM(VG_LITE_FILL_EVEN_ODD);
    STRING_TO_ENUM(VG_LITE_FILL_NON_ZERO);

    printf(VG_LITE_PREFIX "Unknown fill rule: %s\n", str);
    return VG_LITE_FILL_EVEN_ODD;
}

static vg_lite_filter_t parse_filter_args(const char* str)
{
    STRING_TO_ENUM(VG_LITE_FILTER_POINT); /*! Fetch the nearest image pixel. */
    STRING_TO_ENUM(VG_LITE_FILTER_LINEAR); /*! Used for linear paint. */
    STRING_TO_ENUM(VG_LITE_FILTER_BI_LINEAR); /*! Use a 2x2 box around the image pixel and perform an interpolation. */
    STRING_TO_ENUM(VG_LITE_FILTER_GAUSSIAN); /*! Perform 3x3 gaussian blur with the convolution for image pixel. */

    printf(VG_LITE_PREFIX "Unknown filter: %s\n", str);
    return VG_LITE_FILTER_POINT;
}

static vg_lite_pattern_mode_t parse_pattern_mode_args(const char* str)
{
    STRING_TO_ENUM(VG_LITE_PATTERN_COLOR); /*! Pixel outside the bounds of sourceimage should be taken as the color */
    STRING_TO_ENUM(VG_LITE_PATTERN_PAD); /*! Pixel outside the bounds of sourceimage should be taken as having the same color as the closest edge pixel */
    STRING_TO_ENUM(VG_LITE_PATTERN_REPEAT); /*! Pixel outside the bounds of sourceimage should be repeated indefinitely in all directions */
    STRING_TO_ENUM(VG_LITE_PATTERN_REFLECT);

    printf(VG_LITE_PREFIX "Unknown pattern mode: %s\n", str);
    return VG_LITE_PATTERN_COLOR;
}

static int parse_scissor_args(const char* str)
{
    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_SCISSOR)) {
        printf(VG_LITE_PREFIX "Scissor is not supported\n");
        return -1;
    }

    int x, y, right, bottom;
    int num = sscanf(str, "%d,%d,%d,%d", &x, &y, &right, &bottom);
    if (num != 4) {
        printf(VG_LITE_PREFIX "Invalid scissor arguments: %s\n", str);
    }

    VG_LITE_CHECK_ERROR(vg_lite_set_scissor(x, y, right, bottom));

    return 0;

error_handler:
    return -1;
}

static int parse_long_commandline(int longindex, const struct option* longopts, vg_lite_context_t* context)
{
    int retval = 0;
    printf(VG_LITE_PREFIX "longindex: %d(%s), optarg: %s\n", longindex, longopts[longindex].name, optarg);
    switch (longindex) {
    case 0:
        context->func_name = optarg;
        break;

    case 1:
        retval = parse_buffer_args(&context->target, optarg);
        break;

    case 2:
        retval = parse_buffer_args(&context->source, optarg);
        break;

    case 3:
        retval = parse_matrix_args(&context->matrix, optarg);
        break;

    case 4:
        retval = parse_matrix_args(&context->pattern_matrix, optarg);
        break;

    case 5:
        retval = parse_rect_args(&context->rect, optarg);
        break;

    case 6:
        retval = parse_path_bounding_box_args(&context->path, optarg);
        break;

    case 7:
        context->blend = parse_blend_args(optarg);
        break;

    case 8:
        context->fill_rule = parse_fill_rule_args(optarg);
        break;

    case 9:
        context->filter = parse_filter_args(optarg);
        break;

    case 10:
        context->pattern_mode = parse_pattern_mode_args(optarg);
        break;

    case 11:
        context->color = parse_color_args(optarg);
        break;

    case 12:
        context->pattern_color = parse_color_args(optarg);
        break;

    case 13:
        context->source_color = parse_color_args(optarg);
        break;

    case 14:
        retval = parse_scissor_args(optarg);
        break;

    default:
        printf(VG_LITE_PREFIX "Unknown longindex: %d\n", longindex);
        return -1;
    }

    return retval;
}

static int parse_commandline(int argc, char** argv, vg_lite_context_t* context)
{
    const char* shortopt = "ho:";

    int ch;
    int longindex = 0;
    const struct option longopts[] = {
        { "func", required_argument, NULL, 0 },
        { "target", required_argument, NULL, 0 },
        { "source", required_argument, NULL, 0 },
        { "matrix", required_argument, NULL, 0 },
        { "pattern-matrix", required_argument, NULL, 0 },
        { "rect", required_argument, NULL, 0 },
        { "path-bounding-box", required_argument, NULL, 0 },
        { "blend", required_argument, NULL, 0 },
        { "fill-rule", required_argument, NULL, 0 },
        { "filter", required_argument, NULL, 0 },
        { "pattern-mode", required_argument, NULL, 0 },
        { "color", required_argument, NULL, 0 },
        { "pattern-color", required_argument, NULL, 0 },
        { "source-color", required_argument, NULL, 0 },
        { "scissor", required_argument, NULL, 0 },
        { 0, 0, NULL, 0 }
    };

    while ((ch = getopt_long(argc, argv, shortopt, longopts, &longindex)) >= 0) {
        switch (ch) {
        case 0:
            if (parse_long_commandline(longindex, longopts, context) < 0) {
                return -1;
            }
            break;

        case 'o':
            context->output_path = optarg;
            break;

        case 'h':
            show_usage(argv[0]);
            return -1;

        default:
            printf(VG_LITE_PREFIX "Unknown option: %c", optopt);
            show_usage(argv[0]);
            return -1;
        }
    }

    return 0;
}

static int save_buffer(const char* filename, const vg_lite_buffer_t* buffer)
{
    printf(VG_LITE_PREFIX "Save buffer to %s\n", filename);

    /* Construct the PNG image structure. */
    png_image image;
    memset(&image, 0, sizeof(image));

    image.version = PNG_IMAGE_VERSION;
    image.width = buffer->width;
    image.height = buffer->height;

    switch (buffer->format) {
    case VG_LITE_RGB888:
        image.format = PNG_FORMAT_RGB;
        break;
    case VG_LITE_BGR888:
        image.format = PNG_FORMAT_BGR;
        break;
    case VG_LITE_RGBX8888:
    case VG_LITE_RGBA8888:
        image.format = PNG_FORMAT_RGBA;
        break;
    case VG_LITE_XRGB8888:
    case VG_LITE_ARGB8888:
        image.format = PNG_FORMAT_ARGB;
        break;
    case VG_LITE_BGRX8888:
    case VG_LITE_BGRA8888:
        image.format = PNG_FORMAT_BGRA;
        break;
    case VG_LITE_XBGR8888:
    case VG_LITE_ABGR8888:
        image.format = PNG_FORMAT_ABGR;
        break;
    default:
        printf(VG_LITE_PREFIX "Unsupport format: %d\n", buffer->format);
        return -1;
    }

    /* Invlidate the cache to ensure the memory is updated. */
    CACHE_INVALIDATE();

    /* Write the PNG image. */
    int success = png_image_write_to_file(&image, filename, 0, buffer->memory, buffer->stride, NULL);
    if (!success) {
        printf(VG_LITE_PREFIX "Failed to save image\n");
    }

    return success;
}

static void get_format_bytes(
    vg_lite_buffer_format_t format,
    vg_lite_uint32_t* mul,
    vg_lite_uint32_t* div,
    vg_lite_uint32_t* bytes_align)
{
    *mul = *div = 1;
    *bytes_align = 4;
    switch (format) {
    case VG_LITE_L8:
    case VG_LITE_A8:
    case VG_LITE_RGBA8888_ETC2_EAC:
        break;

    case VG_LITE_A4:
        *div = 2;
        break;

    case VG_LITE_ABGR1555:
    case VG_LITE_ARGB1555:
    case VG_LITE_BGRA5551:
    case VG_LITE_RGBA5551:
    case VG_LITE_RGBA4444:
    case VG_LITE_BGRA4444:
    case VG_LITE_ABGR4444:
    case VG_LITE_ARGB4444:
    case VG_LITE_RGB565:
    case VG_LITE_BGR565:
    case VG_LITE_YUYV:
    case VG_LITE_YUY2:
    case VG_LITE_YUY2_TILED:
    /* AYUY2 buffer memory = YUY2 + alpha. */
    case VG_LITE_AYUY2:
    case VG_LITE_AYUY2_TILED:
    /* ABGR8565_PLANAR buffer memory = RGB565 + alpha. */
    case VG_LITE_ABGR8565_PLANAR:
    case VG_LITE_ARGB8565_PLANAR:
    case VG_LITE_RGBA5658_PLANAR:
    case VG_LITE_BGRA5658_PLANAR:
        *mul = 2;
        break;

    case VG_LITE_RGBA8888:
    case VG_LITE_BGRA8888:
    case VG_LITE_ABGR8888:
    case VG_LITE_ARGB8888:
    case VG_LITE_RGBX8888:
    case VG_LITE_BGRX8888:
    case VG_LITE_XBGR8888:
    case VG_LITE_XRGB8888:
        *mul = 4;
        break;

    case VG_LITE_NV12:
    case VG_LITE_NV12_TILED:
        *mul = 3;
        break;

    case VG_LITE_ANV12:
    case VG_LITE_ANV12_TILED:
        *mul = 4;
        break;

    case VG_LITE_INDEX_1:
        *div = 8;
        *bytes_align = 8;
        break;

    case VG_LITE_INDEX_2:
        *div = 4;
        *bytes_align = 8;
        break;

    case VG_LITE_INDEX_4:
        *div = 2;
        *bytes_align = 8;
        break;

    case VG_LITE_INDEX_8:
        *bytes_align = 1;
        break;

    case VG_LITE_RGBA2222:
    case VG_LITE_BGRA2222:
    case VG_LITE_ABGR2222:
    case VG_LITE_ARGB2222:
        *mul = 1;
        break;

    case VG_LITE_RGB888:
    case VG_LITE_BGR888:
    case VG_LITE_ABGR8565:
    case VG_LITE_BGRA5658:
    case VG_LITE_ARGB8565:
    case VG_LITE_RGBA5658:
        *mul = 3;
        break;

    /* OpenVG format*/
    case VG_sRGBX_8888:
    case VG_sRGBA_8888:
    case VG_sRGBA_8888_PRE:
    case VG_lRGBX_8888:
    case VG_lRGBA_8888:
    case VG_lRGBA_8888_PRE:
    case VG_sXRGB_8888:
    case VG_sARGB_8888:
    case VG_sARGB_8888_PRE:
    case VG_lXRGB_8888:
    case VG_lARGB_8888:
    case VG_lARGB_8888_PRE:
    case VG_sBGRX_8888:
    case VG_sBGRA_8888:
    case VG_sBGRA_8888_PRE:
    case VG_lBGRX_8888:
    case VG_lBGRA_8888:
    case VG_sXBGR_8888:
    case VG_sABGR_8888:
    case VG_lBGRA_8888_PRE:
    case VG_sABGR_8888_PRE:
    case VG_lXBGR_8888:
    case VG_lABGR_8888:
    case VG_lABGR_8888_PRE:
        *mul = 4;
        break;

    case VG_sRGBA_5551:
    case VG_sRGBA_4444:
    case VG_sARGB_1555:
    case VG_sARGB_4444:
    case VG_sBGRA_5551:
    case VG_sBGRA_4444:
    case VG_sABGR_1555:
    case VG_sABGR_4444:
    case VG_sRGB_565:
    case VG_sBGR_565:
        *mul = 2;
        break;

    case VG_sL_8:
    case VG_lL_8:
    case VG_A_8:
        break;

    case VG_BW_1:
    case VG_A_4:
    case VG_A_1:
        *div = 2;
        break;

    default:
        break;
    }
}

static vg_lite_error_t alloc_buffer(vg_lite_buffer_t* buffer)
{
    if (buffer->format == VG_LITE_RGBA8888_ETC2_EAC && (buffer->width % 16 || buffer->height % 4)) {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Reset planar. */
    buffer->yuv.uv_planar = buffer->yuv.v_planar = buffer->yuv.alpha_planar = 0;

    /* Align height in case format is tiled. */
    if (buffer->format >= VG_LITE_YUY2 && buffer->format <= VG_LITE_NV16) {
        buffer->height = VG_LITE_ALIGN(buffer->height, 4);
        buffer->yuv.swizzle = VG_LITE_SWIZZLE_UV;
    }

    if (buffer->format >= VG_LITE_YUY2_TILED && buffer->format <= VG_LITE_AYUY2_TILED) {
        buffer->height = VG_LITE_ALIGN(buffer->height, 4);
        buffer->tiled = VG_LITE_TILED;
        buffer->yuv.swizzle = VG_LITE_SWIZZLE_UV;
    }

    vg_lite_uint32_t mul, div, align;
    get_format_bytes(buffer->format, &mul, &div, &align);
    vg_lite_uint32_t stride = VG_LITE_ALIGN((buffer->width * mul / div), align);

    buffer->stride = stride;
    /* Size must be multiple of align, See: https://en.cppreference.com/w/c/memory/aligned_alloc */
    size_t size = VG_LITE_ALIGN(buffer->height * stride, LV_VG_LITE_THORVG_BUF_ADDR_ALIGN);
    buffer->memory = aligned_alloc(LV_VG_LITE_THORVG_BUF_ADDR_ALIGN, size);
    LV_ASSERT(buffer->memory);
    buffer->address = (vg_lite_uint32_t)(uintptr_t)buffer->memory;
    buffer->handle = buffer->memory;
    return VG_LITE_SUCCESS;
}

static vg_lite_error_t free_buffer(vg_lite_buffer_t* buffer)
{
    LV_ASSERT(buffer->memory);
    free(buffer->memory);
    memset(buffer, 0, sizeof(vg_lite_buffer_t));
    return VG_LITE_SUCCESS;
}

#endif /* LV_USE_VG_LITE_MAIN */
