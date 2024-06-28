
#define STBIW_WINDOWS_UTF8
#define STBI_WINDOWS_UTF8

#include "../stb/stb_image.h"
#include "../stb/stb_image_resize2.h"
#include "../stb/stb_image_write.h"
#include "../stb/stb_rect_pack.h"

//extern int stbir__resize_arbitrary_export(
//    void* alloc_context,
//    const void* input_data, int input_w, int input_h, int input_stride_in_bytes,
//    void* output_data, int output_w, int output_h, int output_stride_in_bytes,
//    float s0, float t0, float s1, float t1, float* transform,
//    int channels, int alpha_channel, stbir_uint32 flags, stbir_datatype type,
//    stbir_filter h_filter, stbir_filter v_filter,
//    stbir_edge edge_horizontal, stbir_edge edge_vertical, stbir_colorspace colorspace);


extern FILE* __fopen(char const* filename, char const* mode);