// STB Image Implementation - Simplified version for AquaVisual
// This is a minimal implementation focusing on basic image loading

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Thread-local storage for error messages
static const char* stbi_failure_reason_msg = nullptr;

// Basic error handling
static void stbi_set_failure_reason(const char* reason) {
    stbi_failure_reason_msg = reason;
}

STBIDEF const char* stbi_failure_reason(void) {
    return stbi_failure_reason_msg ? stbi_failure_reason_msg : "Unknown error";
}

// Memory management
STBIDEF void stbi_image_free(void* retval_from_stbi_load) {
    if (retval_from_stbi_load) {
        free(retval_from_stbi_load);
    }
}

// Simplified BMP loader (24-bit RGB only)
static unsigned char* stbi_load_bmp_simple(FILE* f, int* x, int* y, int* comp, int req_comp) {
    unsigned char header[54];
    if (fread(header, 1, 54, f) != 54) {
        stbi_set_failure_reason("BMP: Could not read header");
        return nullptr;
    }

    // Check BMP signature
    if (header[0] != 'B' || header[1] != 'M') {
        stbi_set_failure_reason("BMP: Invalid signature");
        return nullptr;
    }

    // Extract basic info
    int width = *(int*)&header[18];
    int height = *(int*)&header[22];
    int bits_per_pixel = *(short*)&header[28];

    if (bits_per_pixel != 24) {
        stbi_set_failure_reason("BMP: Only 24-bit RGB supported");
        return nullptr;
    }

    *x = width;
    *y = height;
    *comp = 3; // RGB

    int row_size = ((width * 3 + 3) / 4) * 4; // BMP rows are padded to 4 bytes
    int data_size = row_size * height;
    
    unsigned char* data = (unsigned char*)malloc(width * height * 3);
    if (!data) {
        stbi_set_failure_reason("BMP: Out of memory");
        return nullptr;
    }

    // Read pixel data (BMP is stored bottom-up)
    for (int row = height - 1; row >= 0; row--) {
        unsigned char* row_data = data + row * width * 3;
        if (fread(row_data, 1, width * 3, f) != width * 3) {
            free(data);
            stbi_set_failure_reason("BMP: Could not read pixel data");
            return nullptr;
        }
        
        // Convert BGR to RGB
        for (int i = 0; i < width * 3; i += 3) {
            unsigned char temp = row_data[i];
            row_data[i] = row_data[i + 2];
            row_data[i + 2] = temp;
        }
        
        // Skip padding
        int padding = row_size - width * 3;
        if (padding > 0) {
            fseek(f, padding, SEEK_CUR);
        }
    }

    // Handle requested component count
    if (req_comp != 0 && req_comp != 3) {
        unsigned char* new_data = (unsigned char*)malloc(width * height * req_comp);
        if (!new_data) {
            free(data);
            stbi_set_failure_reason("BMP: Out of memory for conversion");
            return nullptr;
        }

        for (int i = 0; i < width * height; i++) {
            if (req_comp == 1) {
                // Convert to grayscale
                new_data[i] = (unsigned char)(0.299f * data[i*3] + 0.587f * data[i*3+1] + 0.114f * data[i*3+2]);
            } else if (req_comp == 4) {
                // Add alpha channel
                new_data[i*4] = data[i*3];
                new_data[i*4+1] = data[i*3+1];
                new_data[i*4+2] = data[i*3+2];
                new_data[i*4+3] = 255;
            }
        }
        
        free(data);
        data = new_data;
        *comp = req_comp;
    }

    return data;
}

// Main loading function
STBIDEF unsigned char* stbi_load(char const* filename, int* x, int* y, int* comp, int req_comp) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        stbi_set_failure_reason("Could not open file");
        return nullptr;
    }

    unsigned char* result = stbi_load_from_file(f, x, y, comp, req_comp);
    fclose(f);
    return result;
}

STBIDEF unsigned char* stbi_load_from_file(FILE* f, int* x, int* y, int* comp, int req_comp) {
    if (!f) {
        stbi_set_failure_reason("Invalid file pointer");
        return nullptr;
    }

    // Read first few bytes to determine format
    unsigned char header[4];
    long pos = ftell(f);
    if (fread(header, 1, 4, f) != 4) {
        stbi_set_failure_reason("Could not read file header");
        return nullptr;
    }
    fseek(f, pos, SEEK_SET);

    // Check for BMP
    if (header[0] == 'B' && header[1] == 'M') {
        return stbi_load_bmp_simple(f, x, y, comp, req_comp);
    }

    stbi_set_failure_reason("Unsupported image format (only BMP supported in this simplified version)");
    return nullptr;
}

STBIDEF unsigned char* stbi_load_from_memory(unsigned char const* buffer, int len, int* x, int* y, int* comp, int req_comp) {
    stbi_set_failure_reason("Memory loading not implemented in simplified version");
    return nullptr;
}

STBIDEF unsigned char* stbi_load_from_callbacks(stbi_io_callbacks const* clbk, void* user, int* x, int* y, int* comp, int req_comp) {
    stbi_set_failure_reason("Callback loading not implemented in simplified version");
    return nullptr;
}

// Info functions
STBIDEF int stbi_info(char const* filename, int* x, int* y, int* comp) {
    FILE* f = fopen(filename, "rb");
    if (!f) return 0;
    
    int result = stbi_info_from_file(f, x, y, comp);
    fclose(f);
    return result;
}

STBIDEF int stbi_info_from_file(FILE* f, int* x, int* y, int* comp) {
    if (!f) return 0;

    unsigned char header[54];
    long pos = ftell(f);
    if (fread(header, 1, 54, f) != 54) {
        fseek(f, pos, SEEK_SET);
        return 0;
    }
    fseek(f, pos, SEEK_SET);

    // Check for BMP
    if (header[0] == 'B' && header[1] == 'M') {
        *x = *(int*)&header[18];
        *y = *(int*)&header[22];
        *comp = 3; // Assuming 24-bit RGB
        return 1;
    }

    return 0;
}

STBIDEF int stbi_info_from_memory(unsigned char const* buffer, int len, int* x, int* y, int* comp) {
    return 0; // Not implemented
}

STBIDEF int stbi_info_from_callbacks(stbi_io_callbacks const* clbk, void* user, int* x, int* y, int* comp) {
    return 0; // Not implemented
}

// Stub implementations for other functions
STBIDEF unsigned short* stbi_load_16(char const* filename, int* x, int* y, int* comp, int req_comp) { return nullptr; }
STBIDEF unsigned short* stbi_load_from_memory_16(unsigned char const* buffer, int len, int* x, int* y, int* comp, int req_comp) { return nullptr; }
STBIDEF unsigned short* stbi_load_from_callbacks_16(stbi_io_callbacks const* clbk, void* user, int* x, int* y, int* comp, int req_comp) { return nullptr; }
STBIDEF unsigned short* stbi_load_from_file_16(FILE* f, int* x, int* y, int* comp, int req_comp) { return nullptr; }

STBIDEF float* stbi_loadf(char const* filename, int* x, int* y, int* comp, int req_comp) { return nullptr; }
STBIDEF float* stbi_loadf_from_memory(unsigned char const* buffer, int len, int* x, int* y, int* comp, int req_comp) { return nullptr; }
STBIDEF float* stbi_loadf_from_callbacks(stbi_io_callbacks const* clbk, void* user, int* x, int* y, int* comp, int req_comp) { return nullptr; }
STBIDEF float* stbi_loadf_from_file(FILE* f, int* x, int* y, int* comp, int req_comp) { return nullptr; }

STBIDEF void stbi_hdr_to_ldr_gamma(float gamma) {}
STBIDEF void stbi_hdr_to_ldr_scale(float scale) {}
STBIDEF void stbi_ldr_to_hdr_gamma(float gamma) {}
STBIDEF void stbi_ldr_to_hdr_scale(float scale) {}

STBIDEF int stbi_is_hdr_from_callbacks(stbi_io_callbacks const* clbk, void* user) { return 0; }
STBIDEF int stbi_is_hdr_from_memory(unsigned char const* buffer, int len) { return 0; }
STBIDEF int stbi_is_hdr(char const* filename) { return 0; }
STBIDEF int stbi_is_hdr_from_file(FILE* f) { return 0; }

STBIDEF int stbi_is_16_bit_from_memory(unsigned char const* buffer, int len) { return 0; }
STBIDEF int stbi_is_16_bit_from_callbacks(stbi_io_callbacks const* clbk, void* user) { return 0; }
STBIDEF int stbi_is_16_bit(char const* filename) { return 0; }

STBIDEF void stbi_set_unpremultiply_on_load(int flag_true_if_should_unpremultiply) {}
STBIDEF void stbi_convert_iphone_png_to_rgb(int flag_true_if_should_convert) {}
STBIDEF void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip) {}
STBIDEF void stbi_set_unpremultiply_on_load_thread(int flag_true_if_should_unpremultiply) {}
STBIDEF void stbi_convert_iphone_png_to_rgb_thread(int flag_true_if_should_convert) {}
STBIDEF void stbi_set_flip_vertically_on_load_thread(int flag_true_if_should_flip) {}

// ZLIB stubs
STBIDEF char* stbi_zlib_decode_malloc_guesssize(const char* buffer, int len, int initial_size, int* outlen) { return nullptr; }
STBIDEF char* stbi_zlib_decode_malloc_guesssize_headerflag(const char* buffer, int len, int initial_size, int* outlen, int parse_header) { return nullptr; }
STBIDEF char* stbi_zlib_decode_malloc(const char* buffer, int len, int* outlen) { return nullptr; }
STBIDEF int stbi_zlib_decode_buffer(char* obuffer, int olen, const char* ibuffer, int ilen) { return 0; }
STBIDEF char* stbi_zlib_decode_noheader_malloc(const char* buffer, int len, int* outlen) { return nullptr; }
STBIDEF int stbi_zlib_decode_noheader_buffer(char* obuffer, int olen, const char* ibuffer, int ilen) { return 0; }

#ifdef STBI_WINDOWS_UTF8
STBIDEF int stbi_convert_wchar_to_utf8(char* buffer, size_t bufferlen, const wchar_t* input) { return 0; }
#endif

#ifndef STBI_NO_GIF
STBIDEF unsigned char* stbi_load_gif_from_memory(unsigned char const* buffer, int len, int** delays, int* x, int* y, int* z, int* comp, int req_comp) { return nullptr; }
#endif