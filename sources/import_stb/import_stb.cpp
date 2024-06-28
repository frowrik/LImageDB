#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_RECT_PACK_IMPLEMENTATION

#include "header_import_stb.h"

FILE* __fopen(char const* filename, char const* mode) {
    return stbi__fopen(filename, mode);
}
