#include "tiger_paths.h"

#define CUSTOM_PATH_BOUNDING_BOX 0, 0, 100, 100

/* Custom path data */
static const int16_t custom_path_data[] = {
    VLC_OP_MOVE, 0, 0,
    VLC_OP_LINE, 100, 100,
    VLC_OP_LINE, 0, 100,
    VLC_OP_CLOSE,
    VLC_OP_END
};

static vg_lite_error_t vg_lite_draw_custom(vg_lite_context_t* context)
{
    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);
    vg_lite_translate(150, 150, &matrix);
    vg_lite_scale(3, 3, &matrix);

    for (int i = 0; i < sizeof(tiger_paths) / sizeof(vg_lite_path_t); i++) {
        VG_LITE_CHECK_ERROR(vg_lite_draw(
            &context->target,
            (vg_lite_path_t*)&tiger_paths[i],
            VG_LITE_FILL_EVEN_ODD,
            &matrix,
            VG_LITE_BLEND_NONE,
            tiger_color_data[i]));
        VG_LITE_CHECK_ERROR(vg_lite_flush());
    }

error_handler:
    return VG_LITE_SUCCESS;
}
