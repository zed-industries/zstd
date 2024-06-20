#include <emscripten.h>
#include "lib/zstd.h"
#include <stdlib.h>

EMSCRIPTEN_KEEPALIVE
unsigned char* compress(unsigned char* data, int size, int* out_size) {
    size_t max_size = ZSTD_compressBound(size);
    unsigned char* compressed = malloc(max_size);
    if (!compressed) return NULL;

    size_t compressed_size = ZSTD_compress(compressed, max_size, data, size, 0);
    if (ZSTD_isError(compressed_size)) {
        free(compressed);
        return NULL;
    }

    *out_size = (int)compressed_size;
    return compressed;
}

EMSCRIPTEN_KEEPALIVE
unsigned char* decompress(unsigned char* compressed_data, int compressed_size, int* out_size) {
    unsigned long long decompressed_size = ZSTD_getFrameContentSize(compressed_data, compressed_size);
    if (decompressed_size == ZSTD_CONTENTSIZE_ERROR || decompressed_size == ZSTD_CONTENTSIZE_UNKNOWN) {
        return NULL;
    }

    unsigned char* decompressed = malloc(decompressed_size);
    if (!decompressed) return NULL;

    size_t actual_size = ZSTD_decompress(decompressed, decompressed_size, compressed_data, compressed_size);
    if (ZSTD_isError(actual_size)) {
        free(decompressed);
        return NULL;
    }

    *out_size = (int)actual_size;
    return decompressed;
}
