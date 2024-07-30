
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
    /* Draw something here */
    return VG_LITE_SUCCESS;
}
