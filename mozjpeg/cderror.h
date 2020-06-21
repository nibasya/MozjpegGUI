/*
 * cderror.h
 *
 * Copyright (C) 1994-1997, Thomas G. Lane.
 * Modified 2009-2017 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README.ijg
 * file.
 *
 * This file defines the error and message codes for the cjpeg/djpeg
 * applications.  These strings are not needed as part of the JPEG library
 * proper.
 * Edit this file to add new codes, or to translate the message strings to
 * some other language.
 */

/*
 * To define the enum list of message codes, include this file without
 * defining macro JMESSAGE.  To create a message string table, include it
 * again with a suitable JMESSAGE definition (see jerror.c for an example).
 */
#ifndef JMESSAGE
#ifndef CDERROR_H
#define CDERROR_H
/* First time through, define the enum list */
#define JMAKE_ENUM_LIST
#else
/* Repeated inclusions of this file are no-ops unless JMESSAGE is defined */
#define JMESSAGE(code, string)
#endif /* CDERROR_H */
#endif /* JMESSAGE */

#ifdef JMAKE_ENUM_LIST

typedef enum {

#define JMESSAGE(code, string)  code,

#endif /* JMAKE_ENUM_LIST */

JMESSAGE(JMSG_FIRSTADDONCODE = 1000, NULL) /* Must be first entry! */

#ifdef BMP_SUPPORTED
JMESSAGE(JERR_BMP_BADCMAP, "Unsupported BMP colormap format")
JMESSAGE(JERR_BMP_BADDEPTH, "Only 8- and 24-bit BMP files are supported")
JMESSAGE(JERR_BMP_BADHEADER, "Invalid BMP file: bad header length")
JMESSAGE(JERR_BMP_BADPLANES, "Invalid BMP file: biPlanes not equal to 1")
JMESSAGE(JERR_BMP_COLORSPACE, "BMP output must be grayscale or RGB")
JMESSAGE(JERR_BMP_COMPRESSED, "Sorry, compressed BMPs not yet supported")
JMESSAGE(JERR_BMP_EMPTY, "Empty BMP image")
JMESSAGE(JERR_BMP_NOT, "Not a BMP file - does not start with BM")
JMESSAGE(JERR_BMP_OUTOFRANGE, "Numeric value out of range in BMP file")
JMESSAGE(JTRC_BMP, "%ux%u 24-bit BMP image")
JMESSAGE(JTRC_BMP_MAPPED, "%ux%u 8-bit colormapped BMP image")
JMESSAGE(JTRC_BMP_OS2, "%ux%u 24-bit OS2 BMP image")
JMESSAGE(JTRC_BMP_OS2_MAPPED, "%ux%u 8-bit colormapped OS2 BMP image")
#endif /* BMP_SUPPORTED */

#ifdef GIF_SUPPORTED
JMESSAGE(JERR_GIF_BUG, "GIF output got confused")
JMESSAGE(JERR_GIF_CODESIZE, "Bogus GIF codesize %d")
JMESSAGE(JERR_GIF_COLORSPACE, "GIF output must be grayscale or RGB")
JMESSAGE(JERR_GIF_IMAGENOTFOUND, "Too few images in GIF file")
JMESSAGE(JERR_GIF_NOT, "Not a GIF file")
JMESSAGE(JTRC_GIF, "%ux%ux%d GIF image")
JMESSAGE(JTRC_GIF_BADVERSION,
         "Warning: unexpected GIF version number '%c%c%c'")
JMESSAGE(JTRC_GIF_EXTENSION, "Ignoring GIF extension block of type 0x%02x")
JMESSAGE(JTRC_GIF_NONSQUARE, "Caution: nonsquare pixels in input")
JMESSAGE(JWRN_GIF_BADDATA, "Corrupt data in GIF file")
JMESSAGE(JWRN_GIF_CHAR, "Bogus char 0x%02x in GIF file, ignoring")
JMESSAGE(JWRN_GIF_ENDCODE, "Premature end of GIF image")
JMESSAGE(JWRN_GIF_NOMOREDATA, "Ran out of GIF bits")
#endif /* GIF_SUPPORTED */

#ifdef PPM_SUPPORTED
JMESSAGE(JERR_PPM_COLORSPACE, "PPM output must be grayscale or RGB")
JMESSAGE(JERR_PPM_NONNUMERIC, "Nonnumeric data in PPM file")
JMESSAGE(JERR_PPM_NOT, "Not a PPM/PGM file")
JMESSAGE(JERR_PPM_OUTOFRANGE, "Numeric value out of range in PPM file")
JMESSAGE(JTRC_PGM, "%ux%u PGM image")
JMESSAGE(JTRC_PGM_TEXT, "%ux%u text PGM image")
JMESSAGE(JTRC_PPM, "%ux%u PPM image")
JMESSAGE(JTRC_PPM_TEXT, "%ux%u text PPM image")
#endif /* PPM_SUPPORTED */

#ifdef RLE_SUPPORTED
JMESSAGE(JERR_RLE_BADERROR, "Bogus error code from RLE library")
JMESSAGE(JERR_RLE_COLORSPACE, "RLE output must be grayscale or RGB")
JMESSAGE(JERR_RLE_DIMENSIONS, "Image dimensions (%ux%u) too large for RLE")
JMESSAGE(JERR_RLE_EMPTY, "Empty RLE file")
JMESSAGE(JERR_RLE_EOF, "Premature EOF in RLE header")
JMESSAGE(JERR_RLE_MEM, "Insufficient memory for RLE header")
JMESSAGE(JERR_RLE_NOT, "Not an RLE file")
JMESSAGE(JERR_RLE_TOOMANYCHANNELS, "Cannot handle %d output channels for RLE")
JMESSAGE(JERR_RLE_UNSUPPORTED, "Cannot handle this RLE setup")
JMESSAGE(JTRC_RLE, "%ux%u full-color RLE file")
JMESSAGE(JTRC_RLE_FULLMAP, "%ux%u full-color RLE file with map of length %d")
JMESSAGE(JTRC_RLE_GRAY, "%ux%u grayscale RLE file")
JMESSAGE(JTRC_RLE_MAPGRAY, "%ux%u grayscale RLE file with map of length %d")
JMESSAGE(JTRC_RLE_MAPPED, "%ux%u colormapped RLE file with map of length %d")
#endif /* RLE_SUPPORTED */

#ifdef TARGA_SUPPORTED
JMESSAGE(JERR_TGA_BADCMAP, "Unsupported Targa colormap format")
JMESSAGE(JERR_TGA_BADPARMS, "Invalid or unsupported Targa file")
JMESSAGE(JERR_TGA_COLORSPACE, "Targa output must be grayscale or RGB")
JMESSAGE(JTRC_TGA, "%ux%u RGB Targa image")
JMESSAGE(JTRC_TGA_GRAY, "%ux%u grayscale Targa image")
JMESSAGE(JTRC_TGA_MAPPED, "%ux%u colormapped Targa image")
#else
JMESSAGE(JERR_TGA_NOTCOMP, "Targa support was not compiled")
#endif /* TARGA_SUPPORTED */

JMESSAGE(JERR_BAD_CMAP_FILE,
         "Color map file is invalid or of unsupported format")
JMESSAGE(JERR_TOO_MANY_COLORS,
         "Output file format cannot handle %d colormap entries")
JMESSAGE(JERR_UNGETC_FAILED, "ungetc failed")
#ifdef PNG_SUPPORTED
JMESSAGE(JERR_UNKNOWN_FORMAT,
         "MozJPEG can't read the image (JPEG, PNG and old BMP and Targa are supported)")
#else
JMESSAGE(JERR_UNKNOWN_FORMAT, "MozJPEG can't read the image (PNG support is disabled in this build)")
#endif
JMESSAGE(JERR_UNSUPPORTED_FORMAT, "Unsupported output file format")

#ifdef PNG_SUPPORTED
JMESSAGE(JERR_PNG_ERROR, "Unable to read PNG file: %s")
JMESSAGE(JERR_PNG_PROFILETOOLARGE, "Embedded profile was too large for this tool - dropped.")
#endif

// Original error messages
JMESSAGE(JERR_CJPEG_SHOW_USAGE, "Usage is shown")
JMESSAGE(JERR_CJPEG_SWITCH_ARITHMATIC, "arithmetic coding not supported")
JMESSAGE(JERR_CJPEG_SWITCH_MISSING_DCT, "missing argument for dct")
JMESSAGE(JERR_CJPEG_SWITCH_BAD_DCT, "invalid argument for dct")
JMESSAGE(JERR_CJPEG_SWITCH_VERSION, "")
JMESSAGE(JERR_CJPEG_SWITCH_MISSING_LAMDA1, "missing argument for lamda1")
JMESSAGE(JERR_CJPEG_SWITCH_MISSING_LAMDA2, "missing argument for lamda2")
JMESSAGE(JERR_CJPEG_SWITCH_MISSING_ICC, "missing argument for icc")
JMESSAGE(JERR_CJPEG_SWITCH_MISSING_MAXMEMORY, "missing argument for max memory")
JMESSAGE(JERR_CJPEG_SWITCH_MISSING_DC_SCAN_OPT, "missing argument for dc-scan-opt")
JMESSAGE(JERR_CJPEG_SWITCH_OPTIMIZE, "entropy optimization was not compiled")
JMESSAGE(JERR_CJPEG_SWITCH_MISSING_OUTFILE, "missing argument for output file name")
JMESSAGE(JERR_CJPEG_SWITCH_PROGRESSIVE, "progressive output was not compiled")
JMESSAGE(JERR_CJPEG_SWITCH_MEMDST, "in-memory destination manager was not compiled")
JMESSAGE(JERR_CJPEG_SWITCH_MISSING_QUALITY, "missing argument for quality")
JMESSAGE(JERR_CJPEG_SWITCH_MISSING_QSLOTS, "missing argument for qslots")
JMESSAGE(JERR_CJPEG_SWITCH_MISSING_QTABLES, "missing argument for qtables")
JMESSAGE(JERR_CJPEG_SWITCH_MISSING_QUANT_TABLE, "missing argument for quant-table")
JMESSAGE(JERR_CJPEG_SWITCH_INVALID_QUANT_TABLE, "Value %d in quant table is invalid")
JMESSAGE(JERR_CJPEG_SWITCH_MISSING_RESTART, "missing argument for restart")
JMESSAGE(JERR_CJPEG_SWITCH_INVALID_RESTART, "invlaid value for restart")
JMESSAGE(JERR_CJPEG_SWITCH_MISSING_SAMPLE, "missing argument for sample")
JMESSAGE(JERR_CJPEG_SWITCH_MISSING_SCANS, "missing argument for scans")
JMESSAGE(JERR_CJPEG_SWITCH_SCANS, "multi-scan output was not compiled")
JMESSAGE(JERR_CJPEG_SWITCH_MISSING_SMOOTH, "missing argument for smooth")
JMESSAGE(JERR_CJPEG_SWITCH_INVALID_SMOOTH, "invlaid value for smooth")
JMESSAGE(JERR_CJPEG_SWITCH_MISSING_TRELLIS_DC_VER_WEIGHT, "missing argument for trellis-dc-ver-weight")
JMESSAGE(JERR_CJPEG_SWITCH_UNKNOWN, "unknown option is given")
JMESSAGE(JERR_CJPEG_QUALITY_RATINGS, "can't set quality ratings")
JMESSAGE(JERR_CJPEG_READ_QTABLE, "can't read qtable file")
JMESSAGE(JERR_CJPEG_QSLOT_FAIL, "failed to process qslots")
JMESSAGE(JERR_CJPEG_SAMPLE_FACTORS, "can't set sample factors")
JMESSAGE(JERR_CJPEG_SCANS, "failed to read scan scripts")
JMESSAGE(JERR_CJPEG_INPUT_OPEN_FAIL, "failed to open input file")
JMESSAGE(JERR_CJPEG_INPUT_NOT_SET, "input file is not specified")
JMESSAGE(JERR_CJPEG_ICC_FILE_OPEN_FAIL, "failed to open ICC file")
JMESSAGE(JERR_CJPEG_ICC_FILE_SIZE_FAIL, "failed to determine ICC file size")
JMESSAGE(JERR_CJPEG_ICC_MALLOC, "failed to allocate memory for ICC files")
JMESSAGE(JERR_CJPEG_ICC_READ, "failed to read data from ICC file")
JMESSAGE(JERR_CJPEG_MEM_SRCDST_UNSUPPORTED, "MEM_SRCDST_SUPPORTED is unsupported")

#ifdef JMAKE_ENUM_LIST

  JMSG_LASTADDONCODE
} ADDON_MESSAGE_CODE;

#undef JMAKE_ENUM_LIST
#endif /* JMAKE_ENUM_LIST */

/* Zap JMESSAGE macro so that future re-inclusions do nothing by default */
#undef JMESSAGE
