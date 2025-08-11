/* stb_image - v2.28 - public domain image loader - http://nothings.org/stb
                                  no warranty implied; use at your own risk

   Do this:
      #define STB_IMAGE_IMPLEMENTATION
   before you include this file in *one* C or C++ file to create the
 implementation.

   // i.e. it should look like this:
   #include ...
   #include ...
   #include ...
   #define STB_IMAGE_IMPLEMENTATION
   #include "stb_image.h"

   You can #define STBI_ASSERT(x) before the #include to avoid using assert.h.
   And #define STBI_MALLOC, STBI_REALLOC, and STBI_FREE to avoid using
 malloc,realloc,free


   QUICK NOTES:
      Primarily of interest to game developers and other people who can
          avoid problematic images and only need the common cases

      JPEG baseline & progressive (12 bpc/arithmetic not supported, same as
 stock IJG lib) PNG 1/2/4/8/16-bit-per-channel

      TGA (not sure what subset, if a subset)
      BMP non-1bpp, non-RLE
      PSD (composited view only, no extra channels, 8/16 bit-per-channel)

      GIF (*comp always reports as 4-channel)
      HDR (radiance rgbE format)
      PIC (Softimage PIC)
      PNM (PPM and PGM binary only)

      Animated GIF still needs a proper API, but here's one way to do it:
          http://gist.github.com/urraka/685d9a6340b26b830d49

      - decode from memory or through FILE (define STBI_NO_STDIO to remove code)
      - decode from arbitrary I/O callbacks
      - SIMD acceleration on x86/x64 (SSE2) and ARM (NEON)

   Full documentation under "DOCUMENTATION" below.


   LICENSE

     See end of file for license information.

   RECENT REVISION HISTORY:

      2.28  (2023-01-29) many error fixes, security errors, just tons of stuff
      2.27  (2021-07-11) document stbi_info better, 16-bit PNM support, bug
 fixes 2.26  (2020-07-13) many minor fixes 2.25  (2020-02-02) fix warnings 2.24
 (2020-02-02) fix warnings; thread-local failure_reason and flip_vertically 2.23
 (2019-08-11) fix clang static analysis warning 2.22  (2019-03-04) gif fixes,
 fix warnings 2.21  (2019-02-25) fix typo in comment 2.20  (2019-02-07) support
 utf8 filenames in Windows; fix warnings and platform ifdefs 2.19  (2018-02-11)
 fix warning 2.18  (2018-01-30) fix warnings 2.17  (2018-01-29) bugfix, 1-bit
 BMP, 16-bitness query, fix warnings 2.16  (2017-07-23) all functions have
 16-bit variants; optimizations; bugfixes 2.15  (2017-03-18) fix png-1,2,4; all
 Imagenet JPGs; no runtime SSE detection; Adobe Camera Raw 2.14  (2017-03-03)
 remove deprecated STBI_JPEG_OLD; fixes for Imagenet JPGs 2.13  (2016-12-04)
 experimental 16-bit API, only for PNG so far; fixes 2.12  (2016-04-02) fix typo
 in 2.11 PSD fix that caused crashes 2.11  (2016-04-02) 16-bit PNGS; enable SSE2
 in non-gcc x64 RGB-format JPEG; remove white-space in PSD string 2.10
 (2016-01-22) avoid warning introduced in 2.09 2.09  (2016-01-16) 16-bit TGA;
 comments in PNM files; STBI_REALLOC_SIZED

   See end of file for full revision history.

 ============================    Contributors    =========================

 Image formats                          Extensions, features
   Sean Barrett (jpeg, png, bmp)          Jetro Lauha (stbi_info)
   Nicolas Schulz (hdr, psd)               Martin "SpartanJ" Golini (stbi_info)
   Jonathan Dummer (tga)                   James "moose2000" Brown (iPhone PNG)
   Jean-Marc Lienher (gif)                 Ben "Disch" Wenger (io callbacks)
   Tom Seddon (pic)                        Omar Cornut (1/2/4-bit PNG)
   Thatcher Ulrich (psd)                   Nicolas Guillemot (vertical flip)
   Ken Miller (pgm, ppm)                   Richard Mitton (16-bit PSD)
   github:urraka (animated gif)            Junggon Kim (PNM comments)
   Christopher Forseth (animated gif)      Daniel Gibson (16-bit TGA)
                                           socks-the-fox (16-bit PNG)
                                           Jeremy Sawicki (handle all ImageNet
 JPGs) Optimizations & bugfixes                  Mikhail Morozov (1-bit BMP)
   Fabian "ryg" Giesen                     Anael Seghezzi (is-16-bit query)
   Arseny Kapoulkine                       Simon Breuss (16-bit PNM)
   John-Mark Allen
   Carmelo J Fdez-Aguera

 Bug & warning fixes
   Marc LeBlanc            David Woo          Guillaume George     Martins
 Mozeiko Christpher Lloyd        Jerry Jansson      Joseph Thomson       Blazej
 Dariusz Roszkowski Phil Jordan                               Dave Moore Roy
 Eltham Hayaki Saito            Nathan Reed        Won Chun Luke Graham Johan
 Duparc       Nick Verigakis       the Horde3D community Thomas Ruf Ronny
 Chevalier                         github:rlyeh Janez Zemva             John
 Bartholomew   Michal Cichon        github:romigrou Jonathan Blow           Ken
 Hamada         Tero Hanninen        github:svdijk Eugene Golushkov Laurent
 Gomila     Cort Stratton        github:snagar Aruelien Pocheville     Sergio
 Gonzalez    Thibault Reuille     github:Zelex Cass Everitt            Ryamond
 Barbiero   Paul Du Bois         github:grim210 Engin Manap             Aldo
 Culquicondor  Philipp Wiesemann    github:sammyhw Dale Weiler             Oriol
 Ferrer Mesia Josh Tobin           github:phprus Matthew Gregan          Elias
 Oenal        Fabian Giesen        github:poppolopoppo Neil Bickford Manuel
 Studer      Don Heyse            github:darealshinji Baldur Karlsson Bill
 Meltsner      Jason Summers        github:Michaelangel007 Jeff Roberts Alex
 Evans         Blazej Dariusz Roszkowski
*/

#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STBI_INCLUDE_STB_IMAGE_H

// DOCUMENTATION
//
// Limitations:
//    - no 12-bit-per-channel JPEG
//    - no JPEGs with arithmetic coding
//    - GIF always returns *comp=4
//
// Basic usage (see HDR discussion below for HDR usage):
//    int x,y,n;
//    unsigned char *data = stbi_load(filename, &x, &y, &n, 0);
//    // ... process data if not NULL ...
//    // ... x = width, y = height, n = # 8-bit components per pixel ...
//    // ... replace '0' with '1'..'4' to force that many components per pixel
//    // ... but 'n' will always be the number that it would have been if you
//    said 0 stbi_image_free(data)
//
// Standard parameters:
//    int *x                 -- outputs image width in pixels
//    int *y                 -- outputs image height in pixels
//    int *channels_in_file  -- outputs # of image components in image file
//    int desired_channels   -- if non-zero, # of image components requested in
//    result
//
// The return value from an image loader is an 'unsigned char *' which points
// to the pixel data, or NULL on an allocation failure or if the image is
// corrupt or invalid. The pixel data consists of *y scanlines of *x pixels,
// with each pixel consisting of N interleaved 8-bit components; the first
// pixel pointed to is top-left-most in the image. There is no padding between
// image scanlines or between pixels, regardless of format. The number of
// components N is 'desired_channels' if desired_channels is non-zero, or
// *channels_in_file otherwise. If desired_channels is non-zero,
// *channels_in_file has the number of components that _would_ have been
// output otherwise. E.g. if you set desired_channels to 4, you will always
// get RGBA output, but you can check *channels_in_file to see if it's trivially
// opaque because e.g. there were only 3 channels in the source image.
//
// An output image with N components has the following components interleaved
// in this order in each pixel:
//
//     N=#comp     components
//       1           grey
//       2           grey, alpha
//       3           red, green, blue
//       4           red, green, blue, alpha
//
// If image loading fails for any reason, the return value will be NULL,
// and *x, *y, *channels_in_file will be unchanged. The function
// stbi_failure_reason() can be queried for an extremely brief, end-user
// unfriendly explanation of why the load failed. Define STBI_NO_FAILURE_STRINGS
// to avoid compiling these strings at all, and STBI_FAILURE_USERMSG to get
// slightly more user-friendly ones.
//
// Paletted PNG, BMP, GIF, and PIC images are automatically depalettized.
//
// To query the width, height and component count of an image without having to
// decode the full file, you can use the stbi_info family of functions:
//
//   int x,y,n,ok;
//   ok = stbi_info(filename, &x, &y, &n);
//   // returns ok=1 and sets x, y, n if image is a supported format,
//   // 0 otherwise.
//
// Note that stb_image pervasively uses ints in its public API for sizes,
// including sizes of memory buffers. This is now part of the API and thus
// hard to change without causing breakage. As a result, the various image
// loaders all have certain limits on image size; these differ by format but
// generally boil down to either (a) 2^24 total pixels or (b) 2^31 total
// bytes. Generally speaking, you're not going to run into these in normal
// circumstances, but it's a good thing to be aware of.
//
// HDR image support   (disable by defining STBI_NO_HDR)
//
// stb_image supports loading HDR images in general, and currently the Radiance
// .HDR file format specifically. You can still load any file through the
// existing interface; if you attempt to load an HDR file, it will be
// automatically toneMapped to LDR, assuming gamma 2.2 and an arbitrary scale
// factor defaulting to 1; both of these constants can be reconfigured through
// this interface:
//
//     stbi_hdr_to_ldr_gamma(2.2f);
//     stbi_hdr_to_ldr_scale(1.0f);
//
// (note, do not use _inverse_ constants; stbi_image will invert them
// appropriately).
//
// Additionally, there is a new, parallel interface for loading files as
// (linear) floats to preserve the full dynamic range:
//
//    float *data = stbi_loadf(filename, &x, &y, &n, 0);
//
// If you load LDR images through this interface, those images will
// be promoted to floating point values on the range [0,1].
// If you load HDR images through this interface, the result will
// be at whatever scale the file was written at.
//
// Additionally, you can enable the "linear" flag (through
// stbi_set_flip_vertically_on_load) which will cause the image
// to be loaded such that the first pixel in the output array is
// the bottom left of the image.
//
// ===========================================================================
//
// UNICODE:
//
//   If compiling for Windows and you wish to use Unicode filenames, compile
//   with
//       #define STBI_WINDOWS_UTF8
//   and pass utf8-encoded filenames. Call stbi_convert_wchar_to_utf8 to convert
//   Windows wchar_t filenames to utf8.
//
// ===========================================================================
//
// Philosophy
//
// stb libraries are designed with the following priorities:
//
//    1. easy to use
//    2. easy to maintain
//    3. good performance
//
// Sometimes I let "good performance" creep up in priority over "easy to
// maintain", and for best performance I may provide less-easy-to-use APIs that
// give higher performance, in addition to the easy-to-use ones. Nevertheless,
// it's important to keep in mind that from the standpoint of you, a client of
// this library, all you care about is #1 and #3, and stb libraries DO NOT
// emphasize #3 above all.
//
// Some secondary priorities arise directly from the first two, some of which
// provide more explicit reasons why performance can't be emphasized.
//
//    - Portable ("ease of use")
//    - Small source code footprint ("easy to maintain")
//    - No dependencies ("ease of use")
//
// ===========================================================================
//
// I/O callbacks
//
// I/O callbacks allow you to read from arbitrary sources, like packaged
// files or some other source. Data read from callbacks are processed
// through a small internal buffer (currently 128 bytes) to try to reduce
// overhead.
//
// The three functions you must define are "read" (reads some bytes of data),
// "skip" (skips some bytes of data), "eof" (reports if the stream is at the
// end).
//

#ifdef __cplusplus
extern "C" {
#endif

#ifndef STBI_NO_STDIO
#include <stdio.h>
#endif

#ifdef STB_IMAGE_STATIC
#define STBIDEF static
#else
#define STBIDEF extern
#endif

//////////////////////////////////////////////////////////////////////////////
//
// PRIMARY API - works on images of any type
//

//
// load image from memory or from file
// these functions return an 'unsigned char *' which points to the pixel data
// the pixel data consists of *y scanlines of *x pixels,
// with each pixel consisting of N interleaved 8-bit components; the first
// pixel pointed to is top-left-most in the image.
// there is no padding between image scanlines or between pixels, regardless of
// format. the number of components N is 'req_comp' if req_comp is non-zero, or
// *comp otherwise. if req_comp is non-zero, *comp has the number of components
// that _would_ have been output otherwise. I.e. if you set req_comp to 4, you
// will always get RGBA output, but you can check *comp to easily see if it's
// opaque (e.g. *comp == 3).
//
// An output image with N components has the following components interleaved
// in this order in each pixel:
//     N=#comp     components
//       1           grey
//       2           grey, alpha
//       3           red, green, blue
//       4           red, green, blue, alpha
//
// If image loading fails for any reason, the return value will be NULL,
// and *x, *y, *comp will be unchanged. The function stbi_failure_reason() can
// be queried for an extremely brief, end-user unfriendly explanation of why the
// load failed. Define STBI_NO_FAILURE_STRINGS to avoid compiling these strings
// at all, and STBI_FAILURE_USERMSG to get slightly more user-friendly ones.
//
// Paletted PNG, BMP, GIF, and PIC images are automatically depalettized.
//

typedef struct {
  int (*read)(void *user, char *data,
              int size); // fill 'data' with 'size' bytes.  return number of
                         // bytes actually read
  void (*skip)(void *user, int n); // skip the next 'n' bytes, or 'unget' the
                                   // last -n bytes if negative
  int (*eof)(void *user); // returns nonzero if we are at end of file/data
} stbi_io_callbacks;

STBIDEF unsigned char *stbi_load(char const *filename, int *x, int *y,
                                 int *comp, int req_comp);
STBIDEF unsigned char *stbi_load_from_memory(unsigned char const *buffer,
                                             int len, int *x, int *y, int *comp,
                                             int req_comp);
STBIDEF unsigned char *stbi_load_from_callbacks(stbi_io_callbacks const *clbk,
                                                void *user, int *x, int *y,
                                                int *comp, int req_comp);

#ifndef STBI_NO_STDIO
STBIDEF unsigned char *stbi_load_from_file(FILE *f, int *x, int *y, int *comp,
                                           int req_comp);
// for stbi_load_from_file, file pointer is left pointing immediately after
// image
#endif

#ifndef STBI_NO_GIF
STBIDEF unsigned char *stbi_load_gif_from_memory(unsigned char const *buffer,
                                                 int len, int **delays, int *x,
                                                 int *y, int *z, int *comp,
                                                 int req_comp);
#endif

#ifdef STBI_WINDOWS_UTF8
STBIDEF int stbi_convert_wchar_to_utf8(char *buffer, size_t bufferlen,
                                       const wchar_t *input);
#endif

////////////////////////////////////
//
// 16-bits-per-channel interface
//

STBIDEF unsigned short *stbi_load_16(char const *filename, int *x, int *y,
                                     int *comp, int req_comp);
STBIDEF unsigned short *stbi_load_from_memory_16(unsigned char const *buffer,
                                                 int len, int *x, int *y,
                                                 int *comp, int req_comp);
STBIDEF unsigned short *
stbi_load_from_callbacks_16(stbi_io_callbacks const *clbk, void *user, int *x,
                            int *y, int *comp, int req_comp);

#ifndef STBI_NO_STDIO
STBIDEF unsigned short *stbi_load_from_file_16(FILE *f, int *x, int *y,
                                               int *comp, int req_comp);
#endif

////////////////////////////////////
//
// float-per-channel interface
//
#ifndef STBI_NO_HDR
STBIDEF float *stbi_loadf(char const *filename, int *x, int *y, int *comp,
                          int req_comp);
STBIDEF float *stbi_loadf_from_memory(unsigned char const *buffer, int len,
                                      int *x, int *y, int *comp, int req_comp);
STBIDEF float *stbi_loadf_from_callbacks(stbi_io_callbacks const *clbk,
                                         void *user, int *x, int *y, int *comp,
                                         int req_comp);

#ifndef STBI_NO_STDIO
STBIDEF float *stbi_loadf_from_file(FILE *f, int *x, int *y, int *comp,
                                    int req_comp);
#endif
#endif

#ifndef STBI_NO_HDR
STBIDEF void stbi_hdr_to_ldr_gamma(float gamma);
STBIDEF void stbi_hdr_to_ldr_scale(float scale);
#endif // STBI_NO_HDR

#ifndef STBI_NO_LINEAR
STBIDEF void stbi_ldr_to_hdr_gamma(float gamma);
STBIDEF void stbi_ldr_to_hdr_scale(float scale);
#endif // STBI_NO_LINEAR

// stbi_is_hdr is always defined, but always returns false if STBI_NO_HDR
STBIDEF int stbi_is_hdr_from_callbacks(stbi_io_callbacks const *clbk,
                                       void *user);
STBIDEF int stbi_is_hdr_from_memory(unsigned char const *buffer, int len);
#ifndef STBI_NO_STDIO
STBIDEF int stbi_is_hdr(char const *filename);
STBIDEF int stbi_is_hdr_from_file(FILE *f);
#endif // STBI_NO_STDIO

// get a VERY brief reason for failure
// on most compilers (and ALL modern mainstream compilers) this is threadsafe
STBIDEF const char *stbi_failure_reason(void);

// free the loaded image -- this is just free()
STBIDEF void stbi_image_free(void *retval_from_stbi_load);

// get image dimensions & components without fully decoding
STBIDEF int stbi_info_from_memory(unsigned char const *buffer, int len, int *x,
                                  int *y, int *comp);
STBIDEF int stbi_info_from_callbacks(stbi_io_callbacks const *clbk, void *user,
                                     int *x, int *y, int *comp);
STBIDEF int stbi_is_16_bit_from_memory(unsigned char const *buffer, int len);
STBIDEF int stbi_is_16_bit_from_callbacks(stbi_io_callbacks const *clbk,
                                          void *user);

#ifndef STBI_NO_STDIO
STBIDEF int stbi_info(char const *filename, int *x, int *y, int *comp);
STBIDEF int stbi_info_from_file(FILE *f, int *x, int *y, int *comp);
STBIDEF int stbi_is_16_bit(char const *filename);
STBIDEF int stbi_is_16_bit_from_file(FILE *f);
#endif

// for image formats that explicitly notate that they have premultiplied alpha,
// we just return the colors as stored in the file. set this flag to force
// unpremultiplication. results are undefined if the input is not actually
// premultiplied.
STBIDEF void
stbi_set_unpremultiply_on_load(int flag_true_if_should_unpremultiply);

// indicate whether we should process iphone images back to canonical format,
// or just pass them through "as-is"
STBIDEF void stbi_convert_iphone_png_to_rgb(int flag_true_if_should_convert);

// flip the image vertically, so the first pixel in the output array is the
// bottom left
STBIDEF void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip);

// as above, but only applies to images loaded on the thread that calls this
// function this function is only available if your compiler supports
// thread-local variables; calling it will fail to compile if your compiler
// doesn't
STBIDEF void
stbi_set_unpremultiply_on_load_thread(int flag_true_if_should_unpremultiply);
STBIDEF void
stbi_convert_iphone_png_to_rgb_thread(int flag_true_if_should_convert);
STBIDEF void
stbi_set_flip_vertically_on_load_thread(int flag_true_if_should_flip);

// ZLIB client - used by PNG, available for other purposes

STBIDEF char *stbi_zlib_decode_malloc_guesssize(const char *buffer, int len,
                                                int initial_size, int *outlen);
STBIDEF char *stbi_zlib_decode_malloc_guesssize_headerflag(const char *buffer,
                                                           int len,
                                                           int initial_size,
                                                           int *outlen,
                                                           int parse_header);
STBIDEF char *stbi_zlib_decode_malloc(const char *buffer, int len, int *outlen);
STBIDEF int stbi_zlib_decode_buffer(char *obuffer, int olen,
                                    const char *ibuffer, int ilen);

STBIDEF char *stbi_zlib_decode_noheader_malloc(const char *buffer, int len,
                                               int *outlen);
STBIDEF int stbi_zlib_decode_noheader_buffer(char *obuffer, int olen,
                                             const char *ibuffer, int ilen);

#ifdef __cplusplus
}
#endif

//
//
////   end header file   /////////////////////////////////////////////////////
#endif // STBI_INCLUDE_STB_IMAGE_H

#ifdef STB_IMAGE_IMPLEMENTATION

// Implementation would go here, but for this header-only version,
// we'll include it from the implementation file

#endif // STB_IMAGE_IMPLEMENTATION