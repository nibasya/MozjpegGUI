/*
 * cjpeg.cpp
 * This file was originally named as cjpeg.c; which has modification history as below.
 * The file is based on mozjpeg 4.1.1.
 * cjpeg.c is modifeid into cjpeg.cpp:
 * Copyright (C) 2020, 2023, Donadona.
 * Many global variables are now a member of class CJpeg to let conversion handled as an object.
 * Command line options are kept as-it-is to have a compatibility.
 * ERREXIT and other exit() functions are throwing exceptions instead of exiting.
 * Modificatoin to quit compression based on event message are done.
 *
 * cjpeg.c
 *
 * This file was part of the Independent JPEG Group's software:
 * Copyright (C) 1991-1998, Thomas G. Lane.
 * Modified 2003-2011 by Guido Vollbeding.
 * libjpeg-turbo Modifications:
   Copyright (C) 2010, 2013-2014, 2017, 2019-2022, D. R. Commander.
 * mozjpeg Modifications:
 * Copyright (C) 2014, Mozilla Corporation.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains a command-line user interface for the JPEG compressor.
 * It should work on any system with Unix- or MS-DOS-style command lines.
 *
 * Two different command line styles are permitted, depending on the
 * compile-time switch TWO_FILE_COMMANDLINE:
 *      cjpeg [options]  inputfile outputfile
 *      cjpeg [options]  [inputfile]
 * In the second style, output is always to standard output, which you'd
 * normally redirect to a file or pipe to some other program.  Input is
 * either from a named file or from standard input (typically redirected).
 * The second style is convenient on Unix but is unhelpful on systems that
 * don't support pipes.  Also, you MUST use the first style if your system
 * doesn't do binary I/O to stdin/stdout.
 * To simplify script writing, the "-outfile" switch is provided.  The syntax
 *      cjpeg [options]  -outfile outputfile  inputfile
 * works regardless of which command line style is used.
 */

#include "pch.h"

#ifdef CJPEG_FUZZER
#define JPEG_INTERNALS
#endif

#include "cdjpeg.h"             /* Common decls for cjpeg/djpeg applications */
#include "jversion.h"           /* for version message */
#include "jconfigint.h"
#include "../RPTT.h"
#include "../OutputDebugLog.h"
#include <atlbase.h>
#include <atlconv.h>
#include "cjpeg.h"

//#ifdef _DEBUG
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//#endif

#ifdef USE_CCOMMAND             /* command-line reader for Macintosh */
#ifdef __MWERKS__
#include <SIOUX.h>              /* Metrowerks needs this */
#include <console.h>            /* ... and this */
#endif
#ifdef THINK_C
#include <console.h>            /* Think declares it here */
#endif
#endif


/* Create the add-on message string table. */

#define JMESSAGE(code, string)  string,

static const char * const cdjpeg_message_table[] = {
#include "cderror.h"
  NULL
};


/* initialize CJpeg class */
CJpeg::CJpeg() :
	m_input_file(NULL),
	m_is_targa(false), m_is_jpeg(false), m_copy_markers(false),
	m_progname(NULL), m_icc_filename(NULL), m_outfilename(NULL), m_memdst(false),
	m_report(FALSE), m_strict(FALSE),
	m_outbuffer(NULL), m_outsize(0), m_pSyncAbort(NULL), m_Paused(NULL)
{
}

#ifdef CJPEG_FUZZER

#include <setjmp.h>

struct my_error_mgr {
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
};

void my_error_exit(j_common_ptr cinfo)
{
	struct my_error_mgr* myerr = (struct my_error_mgr*)cinfo->err;

	longjmp(myerr->setjmp_buffer, 1);
}

static void my_emit_message_fuzzer(j_common_ptr cinfo, int msg_level)
{
	if (msg_level < 0)
		cinfo->err->num_warnings++;
}

#define HANDLE_ERROR() { \
  if (cinfo.global_state > CSTATE_START) { \
    if (memdst && outbuffer) \
      (*cinfo.dest->term_destination) (&cinfo); \
    jpeg_abort_compress(&cinfo); \
  } \
  jpeg_destroy_compress(&cinfo); \
  if (input_file != stdin && input_file != NULL) \
    fclose(input_file); \
  if (memdst) \
    free(outbuffer); \
  return EXIT_FAILURE; \
}

#endif

/*
 * This routine determines what format the input file is,
 * and selects the appropriate input-reading module.
 *
 * To determine which family of input formats the file belongs to,
 * we may look only at the first byte of the file, since C does not
 * guarantee that more than one character can be pushed back with ungetc.
 * Looking at additional bytes would require one of these approaches:
 *     1) assume we can fseek() the input file (fails for piped input);
 *     2) assume we can push back more than one character (works in
 *        some C implementations, but unportable);
 *     3) provide our own buffering (breaks input readers that want to use
 *        stdio directly);
 * or  4) don't put back the data, and modify the input_init methods to assume
 *        they start reading after the start of file.
 * #1 is attractive for MS-DOS but is untenable on Unix.
 *
 * The most portable solution for file types that can't be identified by their
 * first byte is to make the user tell us what they are.  This is also the
 * only approach for "raw" file types that contain only arbitrary values.
 * We presently apply this method for Targa files.  Most of the time Targa
 * files start with 0x00, so we recognize that case.  Potentially, however,
 * a Targa file could start with any byte value (byte 0 is the length of the
 * seldom-used ID field), so we provide a switch to force Targa input mode.
 */

cjpeg_source_ptr CJpeg::select_file_type(FILE* infile)
{
	OutputDebugLog(L"CJpeg::select_file_type\n");

	int c;
	ASSERT(infile != NULL);

	if (m_is_targa) {
#ifdef TARGA_SUPPORTED
		return jinit_read_targa(&m_cinfo);
#else
		ERREXIT(cinfo, JERR_TGA_NOTCOMP);
#endif
	}

	if ((c = getc(infile)) == EOF)
		ERREXIT(&m_cinfo, JERR_INPUT_EMPTY);
	if (ungetc(c, infile) == EOF)
		ERREXIT(&m_cinfo, JERR_UNGETC_FAILED);

	switch (c) {
#ifdef BMP_SUPPORTED
	case 'B':
		return jinit_read_bmp(&m_cinfo, TRUE);
#endif
#ifdef GIF_SUPPORTED
	case 'G':
		return jinit_read_gif(&m_cinfo);
#endif
#ifdef PPM_SUPPORTED
	case 'P':
		return jinit_read_ppm(&m_cinfo);
#endif
#ifdef PNG_SUPPORTED
	case 0x89:
        m_copy_markers = TRUE;
		return jinit_read_png(&m_cinfo);
#endif
#ifdef TARGA_SUPPORTED
	case 0x00:
		return jinit_read_targa(&m_cinfo);
#endif
	case 0xff:
        m_is_jpeg = TRUE;
        m_copy_markers = TRUE;
		return jinit_read_jpeg(&m_cinfo);
	default:
		ERREXIT(&m_cinfo, JERR_UNKNOWN_FORMAT);
		break;
	}

	return NULL;
}


/*
 * Argument-parsing code.
 * The switch parser is designed to be useful with DOS-style command line
 * syntax, ie, intermixed switches and file names, where only the switches
 * to the left of a given file name affect processing of that file.
 * The main program in this file doesn't actually use this capability...
 */


void CJpeg::usage()
/* complain about bad command line */
{
  _ftprintf(stderr, _T("usage: %s [switches] "), m_progname);
#ifdef TWO_FILE_COMMANDLINE
  _ftprintf(stderr, "inputfile outputfile\n");
#else
  _ftprintf(stderr, _T("[inputfile]\n"));
#endif

  _ftprintf(stderr, _T("Switches (names may be abbreviated):\n"));
  _ftprintf(stderr, _T("  -quality N[,...]   Compression quality (0..100; 5-95 is most useful range,\n"));
  _ftprintf(stderr, _T("                     default is 75)\n"));
  _ftprintf(stderr, _T("  -grayscale     Create monochrome JPEG file\n"));
  _ftprintf(stderr, _T("  -rgb           Create RGB JPEG file\n"));
#ifdef ENTROPY_OPT_SUPPORTED
  _ftprintf(stderr, _T("  -optimize      Optimize Huffman table (smaller file, but slow compression, enabled by default)\n"));
#endif
#ifdef C_PROGRESSIVE_SUPPORTED
  _ftprintf(stderr, _T("  -progressive   Create progressive JPEG file (enabled by default)\n"));
#endif
  _ftprintf(stderr, _T("  -baseline      Create baseline JPEG file (disable progressive coding)\n"));
#ifdef TARGA_SUPPORTED
  _ftprintf(stderr, _T("  -targa         Input file is Targa format (usually not needed)\n"));
#endif
  _ftprintf(stderr, _T("  -revert        Revert to standard defaults (instead of mozjpeg defaults)\n"));
  _ftprintf(stderr, _T("  -fastcrush     Disable progressive scan optimization\n"));
  _ftprintf(stderr, _T("  -dc-scan-opt   DC scan optimization mode\n"));
  _ftprintf(stderr, _T("                 - 0 One scan for all components\n"));
  _ftprintf(stderr, _T("                 - 1 One scan per component (default)\n"));
  _ftprintf(stderr, _T("                 - 2 Optimize between one scan for all components and one scan for 1st component\n"));
  _ftprintf(stderr, _T("                     plus one scan for remaining components\n"));
  _ftprintf(stderr, _T("  -notrellis     Disable trellis optimization\n"));
  _ftprintf(stderr, _T("  -trellis-dc    Enable trellis optimization of DC coefficients (default)\n"));
  _ftprintf(stderr, _T("  -notrellis-dc  Disable trellis optimization of DC coefficients\n"));
  _ftprintf(stderr, _T("  -tune-psnr     Tune trellis optimization for PSNR\n"));
  _ftprintf(stderr, _T("  -tune-hvs-psnr Tune trellis optimization for PSNR-HVS (default)\n"));
  _ftprintf(stderr, _T("  -tune-ssim     Tune trellis optimization for SSIM\n"));
  _ftprintf(stderr, _T("  -tune-ms-ssim  Tune trellis optimization for MS-SSIM\n"));
  _ftprintf(stderr, _T("Switches for advanced users:\n"));
  _ftprintf(stderr, _T("  -noovershoot   Disable black-on-white deringing via overshoot\n"));
  _ftprintf(stderr, _T("  -nojfif        Do not write JFIF (reduces size by 18 bytes but breaks standards; no known problems in Web browsers)\n"));
#ifdef C_ARITH_CODING_SUPPORTED
  _ftprintf(stderr, _T("  -arithmetic    Use arithmetic coding\n"));
#endif
#ifdef DCT_ISLOW_SUPPORTED
  _ftprintf(stderr, _T("  -dct int       Use accurate integer DCT method%s\n"),
          (JDCT_DEFAULT == JDCT_ISLOW ? _T(" (default)") : _T("")));
#endif
#ifdef DCT_IFAST_SUPPORTED
  _ftprintf(stderr, _T("  -dct fast      Use less accurate integer DCT method [legacy feature]%s\n"),
	(JDCT_DEFAULT == JDCT_IFAST ? _T(" (default)") : _T("")));
#endif
#ifdef DCT_FLOAT_SUPPORTED
  _ftprintf(stderr, _T("  -dct float     Use floating-point DCT method [legacy feature]%s\n"),
          (JDCT_DEFAULT == JDCT_FLOAT ? _T(" (default)") : _T("")));
#endif
  _ftprintf(stderr, _T("  -quant-baseline Use 8-bit quantization table entries for baseline JPEG compatibility\n"));
  _ftprintf(stderr, _T("  -quant-table N Use predefined quantization table N:\n"));
  _ftprintf(stderr, _T("                 - 0 JPEG Annex K\n"));
  _ftprintf(stderr, _T("                 - 1 Flat\n"));
  _ftprintf(stderr, _T("                 - 2 Tuned for MS-SSIM on Kodak image set\n"));
  _ftprintf(stderr, _T("                 - 3 ImageMagick table by N. Robidoux (default)\n"));
  _ftprintf(stderr, _T("                 - 4 Tuned for PSNR-HVS on Kodak image set\n"));
  _ftprintf(stderr, _T("                 - 5 Table from paper by Klein, Silverstein and Carney\n"));
  _ftprintf(stderr, _T("                 - 6 Table from paper by Watson, Taylor and Borthwick\n"));
  _ftprintf(stderr, _T("                 - 7 Table from paper by Ahumada, Watson, Peterson\n"));
  _ftprintf(stderr, _T("                 - 8 Table from paper by Peterson, Ahumada and Watson\n"));
  _ftprintf(stderr, _T("  -icc FILE      Embed ICC profile contained in FILE\n"));
  _ftprintf(stderr, _T("  -restart N     Set restart interval in rows, or in blocks with B\n"));
#ifdef INPUT_SMOOTHING_SUPPORTED
  _ftprintf(stderr, _T("  -smooth N      Smooth dithered input (N=1..100 is strength)\n"));
#endif
  _ftprintf(stderr, _T("  -maxmemory N   Maximum memory to use (in kbytes)\n"));
  _ftprintf(stderr, _T("  -outfile name  Specify name for output file\n"));
#if JPEG_LIB_VERSION >= 80 || defined(MEM_SRCDST_SUPPORTED)
  _ftprintf(stderr, _T("  -memdst        Compress to memory instead of file (useful for benchmarking)\n"));
#endif
  _ftprintf(stderr, _T("  -report        Report compression progress\n"));
  _ftprintf(stderr, _T("  -strict        Treat all warnings as fatal\n"));
  _ftprintf(stderr, _T("  -verbose  or  -debug   Emit debug output\n"));
  _ftprintf(stderr, _T("  -version       Print version information and exit\n"));
  _ftprintf(stderr, _T("Switches for wizards:\n"));
  _ftprintf(stderr, _T("  -qtables FILE  Use quantization tables given in FILE\n"));
  _ftprintf(stderr, _T("  -qslots N[,...]    Set component quantization tables\n"));
  _ftprintf(stderr, _T("  -sample HxV[,...]  Set component sampling factors\n"));
#ifdef C_MULTISCAN_FILES_SUPPORTED
  _ftprintf(stderr, _T("  -scans FILE    Create multi-scan JPEG per script FILE\n"));
#endif
  ERREXIT(&m_cinfo, JERR_CJPEG_SHOW_USAGE);
}


int CJpeg::parse_switches(int argc, TCHAR** argv, int last_file_arg_seen, boolean for_real)
/* Parse optional switches.
 * Returns argv[] index of first file-name argument (== argc if none).
 * Any file names with indexes <= last_file_arg_seen are ignored;
 * they have presumably been processed in a previous iteration.
 * (Pass 0 for last_file_arg_seen on the first or only iteration.)
 * for_real is FALSE on the first (dummy) pass; we may skip any expensive
 * processing.
 */
{
	OutputDebugLog(L"CJpeg::parse_switches\n");

	int argn;
	TCHAR* arg;
	boolean force_baseline;
	boolean simple_progressive;
	TCHAR* qualityarg = NULL;      /* saves -quality parm if any */
	TCHAR* qtablefile = NULL;      /* saves -qtables filename if any */
	TCHAR* qslotsarg = NULL;       /* saves -qslots parm if any */
	TCHAR* samplearg = NULL;       /* saves -sample parm if any */
	TCHAR* scansarg = NULL;        /* saves -scans parm if any */

	/* Set up default JPEG parameters. */

	force_baseline = FALSE;       /* by default, allow 16-bit quantizers */
#ifdef C_PROGRESSIVE_SUPPORTED
	simple_progressive = m_cinfo.num_scans == 0 ? FALSE : TRUE;
#else
	simple_progressive = FALSE;
#endif
	m_cinfo.err->trace_level = 0;

	/* Scan command line options, adjust parameters */

	for (argn = 1; argn < argc; argn++) {
		arg = argv[argn];
		if (*arg != _T('-')) {
			/* Not a switch, must be a file name argument */
			if (argn <= last_file_arg_seen) {
				m_outfilename = NULL;     /* -outfile applies to just one input file */
				continue;               /* ignore this name if previously processed */
			}
			break;                    /* else done parsing switches */
		}
		arg++;                      /* advance past switch marker character */

		if (keymatch(arg, _T("arithmetic"), 1)) {
			/* Use arithmetic coding. */
#ifdef C_ARITH_CODING_SUPPORTED
			m_cinfo.arith_code = TRUE;

			/* No table optimization required for AC */
			m_cinfo.optimize_coding = FALSE;
#else
			ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_ARITHMATIC);
#endif
		}
		else if (keymatch(arg, _T("baseline"), 1)) {
			/* Force baseline-compatible output (8-bit quantizer values). */
			force_baseline = TRUE;
			/* Disable multiple scans */
			simple_progressive = FALSE;
			m_cinfo.num_scans = 0;
			m_cinfo.scan_info = NULL;
		}
		else if (keymatch(arg, _T("dct"), 2)) {
			/* Select DCT algorithm. */
			if (++argn >= argc) {      /* advance to next argument */
				ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_MISSING_DCT);
				usage();
			}
			if (keymatch(argv[argn], _T("int"), 1)) {
				m_cinfo.dct_method = JDCT_ISLOW;
			}
			else if (keymatch(argv[argn], _T("fast"), 2)) {
				m_cinfo.dct_method = JDCT_IFAST;
			}
			else if (keymatch(argv[argn], _T("float"), 2)) {
				m_cinfo.dct_method = JDCT_FLOAT;
			}
			else {
				ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_BAD_DCT);
				usage();
			}
		}
		else if (keymatch(arg, _T("debug"), 1) || keymatch(arg, _T("verbose"), 1)) {
			/* Enable debug printouts. */

			_ftprintf(stderr, _T("%s version %s (build %s)\n"),
				PACKAGE_NAME, VERSION, BUILD);
			_ftprintf(stderr, _T("%s\n\n"), _T(JCOPYRIGHT));
			_ftprintf(stderr, _T("Emulating The Independent JPEG Group's software, version %s\n\n"),
				JVERSION);
			m_cinfo.err->trace_level++;
		}
		else if (keymatch(arg, _T("version"), 4)) {
			TCHAR buff[JMSG_STR_PARM_MAX];
			_stprintf_s(buff, JMSG_STR_PARM_MAX, _T("%s version %s (build %s)\n"),
				PACKAGE_NAME, VERSION, BUILD);
			ERREXITS(&m_cinfo, JERR_CJPEG_SWITCH_VERSION, buff);
		}
		else if (keymatch(arg, _T("fastcrush"), 4)) {
			jpeg_c_set_bool_param(&m_cinfo, JBOOLEAN_OPTIMIZE_SCANS, FALSE);
		}
		else if (keymatch(arg, _T("grayscale"), 2) || keymatch(arg, _T("greyscale"), 2)) {
			/* Force a monochrome JPEG file to be generated. */
			jpeg_set_colorspace(&m_cinfo, JCS_GRAYSCALE);
		}
		else if (keymatch(arg, _T("rgb"), 3)) {
			/* Force an RGB JPEG file to be generated. */
			jpeg_set_colorspace(&m_cinfo, JCS_RGB);
		}
		else if (keymatch(arg, _T("lambda1"), 7)) {
			if (++argn >= argc) {       /* advance to next argument */
				ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_MISSING_LAMDA1);
				usage();
			}
			jpeg_c_set_float_param(&m_cinfo, JFLOAT_LAMBDA_LOG_SCALE1,
				static_cast<float>(_ttof(argv[argn])));
		}
		else if (keymatch(arg, _T("lambda2"), 7)) {
			if (++argn >= argc) {       /* advance to next argument */
				ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_MISSING_LAMDA2);
				usage();
			}
			jpeg_c_set_float_param(&m_cinfo, JFLOAT_LAMBDA_LOG_SCALE2,
				static_cast<float>(_ttof(argv[argn])));
		}
		else if (keymatch(arg, _T("icc"), 1)) {
			/* Set ICC filename. */
			if (++argn >= argc) {       /* advance to next argument */
				ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_MISSING_ICC);
				usage();
			}
			m_icc_filename = argv[argn];
		}
		else if (keymatch(arg, _T("maxmemory"), 3)) {
			/* Maximum memory in Kb (or Mb with 'm'). */
			long lval;
			TCHAR ch = _T('x');

			if (++argn >= argc) {       /* advance to next argument */
				ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_MISSING_MAXMEMORY);
				usage();
			}
			if (_stscanf_s(argv[argn], _T("%ld%c"), &lval, &ch, 1) < 1) {
				ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_MISSING_MAXMEMORY);
				usage();
			}
			if (ch == _T('m') || ch == _T('M'))
				lval *= 1000L;
			m_cinfo.mem->max_memory_to_use = lval * 1000L;
		}
		else if (keymatch(arg, _T("dc-scan-opt"), 3)) {
			if (++argn >= argc) {      /* advance to next argument */
				ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_MISSING_DC_SCAN_OPT);
				usage();
			}
			jpeg_c_set_int_param(&m_cinfo, JINT_DC_SCAN_OPT_MODE, _ttoi(argv[argn]));
		}
		else if (keymatch(arg, _T("optimize"), 1) || keymatch(arg, _T("optimise"), 1)) {
			/* Enable entropy parm optimization. */
#ifdef ENTROPY_OPT_SUPPORTED
			m_cinfo.optimize_coding = TRUE;
#else
			ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_OPTIMIZE);
#endif
		}
		else if (keymatch(arg, _T("outfile"), 4)) {
			/* Set output file name. */
			if (++argn >= argc) {      /* advance to next argument */
				ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_MISSING_OUTFILE);
				usage();
			}
			m_outfilename = argv[argn]; /* save it away for later use */
		}
		else if (keymatch(arg, _T("progressive"), 1)) {
			/* Select simple progressive mode. */
#ifdef C_PROGRESSIVE_SUPPORTED
			simple_progressive = TRUE;
			/* We must postpone execution until num_components is known. */
#else
			ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_PROGRESSIVE);
#endif
		}
		else if (keymatch(arg, _T("memdst"), 2)) {
			/* Use in-memory destination manager */
#if JPEG_LIB_VERSION >= 80 || defined(MEM_SRCDST_SUPPORTED)
			m_memdst = TRUE;
#else
			ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_MEMDST);
#endif
		}
		else if (keymatch(arg, _T("quality"), 1)) {
			/* Quality ratings (quantization table scaling factors). */
			if (++argn >= argc) {      /* advance to next argument */
				ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_MISSING_QUALITY);
				usage();
			}
			qualityarg = argv[argn];

		}
		else if (keymatch(arg, _T("qslots"), 2)) {
			/* Quantization table slot numbers. */
			if (++argn >= argc) {       /* advance to next argument */
				ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_MISSING_QSLOTS);
				usage();
			}
			qslotsarg = argv[argn];
			/* Must delay setting qslots until after we have processed any
			 * colorspace-determining switches, since jpeg_set_colorspace sets
			 * default quant table numbers.
			 */
		}
		else if (keymatch(arg, _T("qtables"), 2)) {
			/* Quantization tables fetched from file. */
			if (++argn >= argc) {       /* advance to next argument */
				ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_MISSING_QTABLES);
				usage();
			}
			qtablefile = argv[argn];
			/* We postpone actually reading the file in case -quality comes later. */
		}
		else if (keymatch(arg, _T("report"), 3)) {
			m_report = TRUE;
		}
		else if (keymatch(arg, _T("quant-table"), 7)) {
			int val;
			if (++argn >= argc) {       /* advance to next argument */
				ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_MISSING_QUANT_TABLE);
				usage();
			}
			val = _ttoi(argv[argn]);
			jpeg_c_set_int_param(&m_cinfo, JINT_BASE_QUANT_TBL_IDX, val);
			if (jpeg_c_get_int_param(&m_cinfo, JINT_BASE_QUANT_TBL_IDX) != val) {
				ERREXIT1(&m_cinfo, JERR_CJPEG_SWITCH_INVALID_QUANT_TABLE, val);
				usage();
			}
			jpeg_set_quality(&m_cinfo, 75, TRUE);
		}
		else if (keymatch(arg, _T("quant-baseline"), 7)) {
			/* Force quantization table to meet baseline requirements */
			force_baseline = TRUE;

		}
		else if (keymatch(arg, _T("restart"), 1)) {
			/* Restart interval in MCU rows (or in MCUs with 'b'). */
			long lval;
			TCHAR ch = 'x';

			if (++argn >= argc) {       /* advance to next argument */
				ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_MISSING_RESTART);
				usage();
			}
			if (_stscanf_s(argv[argn], _T("%ld%c"), &lval, &ch, 1) < 1) {
				ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_INVALID_RESTART);
				usage();
			}
			if (lval < 0 || lval > 65535L) {
				ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_INVALID_RESTART);
				usage();
			}
			if (ch == _T('b') || ch == _T('B')) {
				m_cinfo.restart_interval = (unsigned int)lval;
				m_cinfo.restart_in_rows = 0; /* else prior '-restart n' overrides me */
			}
			else {
				m_cinfo.restart_in_rows = (int)lval;
				/* restart_interval will be computed during startup */
			}
		}
		else if (keymatch(arg, _T("revert"), 3)) {
			/* revert to old JPEG default */
			jpeg_c_set_int_param(&m_cinfo, JINT_COMPRESS_PROFILE, JCP_FASTEST);
			jpeg_set_defaults(&m_cinfo);
		}
		else if (keymatch(arg, _T("sample"), 2)) {
			/* Set sampling factors. */
			if (++argn >= argc) {       /* advance to next argument */
				ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_MISSING_SAMPLE);
				usage();
			}
			samplearg = argv[argn];
			/* Must delay setting sample factors until after we have processed any
			 * colorspace-determining switches, since jpeg_set_colorspace sets
			 * default sampling factors.
			 */

		}
		else if (keymatch(arg, _T("scans"), 4)) {
			/* Set scan script. */
#ifdef C_MULTISCAN_FILES_SUPPORTED
			if (++argn >= argc) {       /* advance to next argument */
				ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_MISSING_SCANS);
				usage();
			}
			scansarg = argv[argn];
			/* We must postpone reading the file in case -progressive appears. */
#else
			ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_SCANS);
#endif
		}
		else if (keymatch(arg, _T("smooth"), 2)) {
			/* Set input smoothing factor. */
			int val;

			if (++argn >= argc) {       /* advance to next argument */
				ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_MISSING_SMOOTH);
				usage();
			}
			if (_stscanf_s(argv[argn], _T("%d"), &val) != 1) {
				ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_INVALID_SMOOTH);
				usage();
			}
			if (val < 0 || val > 100) {
				ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_INVALID_SMOOTH);
				usage();
			}
			m_cinfo.smoothing_factor = val;
		}
		else if (keymatch(arg, _T("strict"), 2)) {
			m_strict = TRUE;
		}
		else if (keymatch(arg, _T("targa"), 1)) {
			/* Input file is Targa format. */
			m_is_targa = TRUE;

		}
		else if (keymatch(arg, _T("notrellis-dc"), 11)) {
			/* disable trellis quantization */
			jpeg_c_set_bool_param(&m_cinfo, JBOOLEAN_TRELLIS_QUANT_DC, FALSE);

		}
		else if (keymatch(arg, _T("notrellis"), 1)) {
			/* disable trellis quantization */
			jpeg_c_set_bool_param(&m_cinfo, JBOOLEAN_TRELLIS_QUANT, FALSE);

		}
		else if (keymatch(arg, _T("trellis-dc-ver-weight"), 12)) {
			if (++argn >= argc) {      /* advance to next argument */
				ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_MISSING_TRELLIS_DC_VER_WEIGHT);
				usage();
			}
			jpeg_c_set_float_param(&m_cinfo, JFLOAT_TRELLIS_DELTA_DC_WEIGHT, static_cast<float>(_ttof(argv[argn])));

		}
		else if (keymatch(arg, _T("trellis-dc"), 9)) {
			/* enable DC trellis quantization */
			jpeg_c_set_bool_param(&m_cinfo, JBOOLEAN_TRELLIS_QUANT_DC, TRUE);

		}
		else if (keymatch(arg, _T("tune-psnr"), 6)) {
			jpeg_c_set_int_param(&m_cinfo, JINT_BASE_QUANT_TBL_IDX, 1);
			jpeg_c_set_float_param(&m_cinfo, JFLOAT_LAMBDA_LOG_SCALE1, 9.0);
			jpeg_c_set_float_param(&m_cinfo, JFLOAT_LAMBDA_LOG_SCALE2, 0.0);
			jpeg_c_set_bool_param(&m_cinfo, JBOOLEAN_USE_LAMBDA_WEIGHT_TBL, FALSE);
			jpeg_set_quality(&m_cinfo, 75, TRUE);

		}
		else if (keymatch(arg, _T("tune-ssim"), 6)) {
			jpeg_c_set_int_param(&m_cinfo, JINT_BASE_QUANT_TBL_IDX, 1);
			jpeg_c_set_float_param(&m_cinfo, JFLOAT_LAMBDA_LOG_SCALE1, 11.5);
			jpeg_c_set_float_param(&m_cinfo, JFLOAT_LAMBDA_LOG_SCALE2, 12.75);
			jpeg_c_set_bool_param(&m_cinfo, JBOOLEAN_USE_LAMBDA_WEIGHT_TBL, FALSE);
			jpeg_set_quality(&m_cinfo, 75, TRUE);

		}
		else if (keymatch(arg, _T("tune-ms-ssim"), 6)) {
			jpeg_c_set_int_param(&m_cinfo, JINT_BASE_QUANT_TBL_IDX, 3);
			jpeg_c_set_float_param(&m_cinfo, JFLOAT_LAMBDA_LOG_SCALE1, 12.0);
			jpeg_c_set_float_param(&m_cinfo, JFLOAT_LAMBDA_LOG_SCALE2, 13.0);
			jpeg_c_set_bool_param(&m_cinfo, JBOOLEAN_USE_LAMBDA_WEIGHT_TBL, TRUE);
			jpeg_set_quality(&m_cinfo, 75, TRUE);

		}
		else if (keymatch(arg, _T("tune-hvs-psnr"), 6)) {
			jpeg_c_set_int_param(&m_cinfo, JINT_BASE_QUANT_TBL_IDX, 3);
			jpeg_c_set_float_param(&m_cinfo, JFLOAT_LAMBDA_LOG_SCALE1, 14.75);
			jpeg_c_set_float_param(&m_cinfo, JFLOAT_LAMBDA_LOG_SCALE2, 16.5);
			jpeg_c_set_bool_param(&m_cinfo, JBOOLEAN_USE_LAMBDA_WEIGHT_TBL, TRUE);
			jpeg_set_quality(&m_cinfo, 75, TRUE);

		}
		else if (keymatch(arg, _T("noovershoot"), 11)) {
			jpeg_c_set_bool_param(&m_cinfo, JBOOLEAN_OVERSHOOT_DERINGING, FALSE);

		}
		else if (keymatch(arg, _T("nojfif"), 6)) {
			m_cinfo.write_JFIF_header = 0;
		}
		else {
			ERREXIT(&m_cinfo, JERR_CJPEG_SWITCH_UNKNOWN);
			usage();                  /* bogus switch */
		}
	}

	/* Post-switch-scanning cleanup */

	if (for_real) {

		/* Set quantization tables for selected quality. */
		/* Some or all may be overridden if -qtables is present. */
		if (qualityarg != NULL)     /* process -quality if it was present */
			if (!set_quality_ratings(&m_cinfo, qualityarg, force_baseline)) {
				ERREXIT(&m_cinfo, JERR_CJPEG_QUALITY_RATINGS);
				usage();
			}

		if (qtablefile != NULL)     /* process -qtables if it was present */
			if (!read_quant_tables(&m_cinfo, qtablefile, force_baseline)) {
				ERREXIT(&m_cinfo, JERR_CJPEG_READ_QTABLE);
				usage();
			}

		if (qslotsarg != NULL)      /* process -qslots if it was present */
			if (!set_quant_slots(&m_cinfo, qslotsarg)) {
				ERREXIT(&m_cinfo, JERR_CJPEG_QSLOT_FAIL);
				usage();
			}
		/* set_quality_ratings sets default subsampling, so the explicit
		   subsampling must be set after it */
		if (samplearg != NULL)      /* process -sample if it was present */
			if (!set_sample_factors(&m_cinfo, samplearg)) {
				ERREXIT(&m_cinfo, JERR_CJPEG_SAMPLE_FACTORS);
				usage();
			}

#ifdef C_PROGRESSIVE_SUPPORTED
		if (simple_progressive)     /* process -progressive; -scans can override */
			jpeg_simple_progression(&m_cinfo);
#endif

#ifdef C_MULTISCAN_FILES_SUPPORTED
		if (scansarg != NULL)       /* process -scans if it was present */
			if (!read_scan_script(&m_cinfo, scansarg)) {
				ERREXIT(&m_cinfo, JERR_CJPEG_SCANS);
				usage();
			}
#endif
	}

	return argn;                  /* return index of next arg (file name) */
}

void CJpeg::error_exit(j_common_ptr cinfo)
{
	OutputDebugLog(L"CJpeg::error_exit\n");

	/* Always display the message */
	(*cinfo->err->output_message) (cinfo);

	/* Let the memory manager delete any temp files before we die */
	jpeg_destroy(cinfo);

	char buffer[JMSG_LENGTH_MAX];
	cinfo->err->format_message(cinfo, buffer);
	
	throw CJpegException(cinfo->err->msg_code, CString(CA2T(buffer)));
}

void CJpeg::output_message(j_common_ptr cinfo)
{
	char buffer[JMSG_LENGTH_MAX];

	/* Create the message */
	(*cinfo->err->format_message) (cinfo, buffer);

	OutputDebugLog(L"CJpeg: ");
	OutputDebugLog(CString(CA2T(buffer)));
	OutputDebugLog(L"\n");

	_RPT1(_CRT_WARN, "%s\n",buffer);
}


void CJpeg::my_emit_message(j_common_ptr cinfo, int msg_level)
{
	if (msg_level < 0) {
		/* Treat warning as fatal */
		cinfo->err->error_exit(cinfo);
	}
	else {
		if (cinfo->err->trace_level >= msg_level)
			cinfo->err->output_message(cinfo);
	}
}


/*
 * The main program.
 */

int CJpeg::cjpeg_main(int argc, TCHAR** argv)
{
#ifdef CJPEG_FUZZER
	struct my_error_mgr myerr;
	struct jpeg_error_mgr& jerr = myerr.pub;
#else
	struct jpeg_error_mgr jerr;
#endif
	struct cdjpeg_progress_mgr progress;
	int file_index;
	cjpeg_source_ptr src_mgr;
	FILE* icc_file = NULL;
	JOCTET* icc_profile = NULL;
	long icc_len = 0;
	JDIMENSION num_scanlines;
	TCHAR errBuffer[JMSG_LENGTH_MAX];

	/* On Mac, fetch a command line. */
#ifdef USE_CCOMMAND
	argc = ccommand(&argv);
#endif

	m_progname = argv[0];
	if (m_progname == NULL || m_progname[0] == 0)
		m_progname = _T("cjpeg");         /* in case C library doesn't provide it */

	  /* Initialize the JPEG compression object with default error handling. */
	m_cinfo.err = jpeg_std_error(&jerr);
	m_cinfo.err->error_exit = CJpeg::error_exit;
	m_cinfo.err->output_message = CJpeg::output_message;
	jpeg_create_compress(&m_cinfo);
	/* Add some application-specific error messages (from cderror.h) */
	jerr.addon_message_table = cdjpeg_message_table;
	jerr.first_addon_message = JMSG_FIRSTADDONCODE;
	jerr.last_addon_message = JMSG_LASTADDONCODE;

	/* Initialize JPEG parameters.
	 * Much of this may be overridden later.
	 * In particular, we don't yet know the input file's color space,
	 * but we need to provide some value for jpeg_set_defaults() to work.
	 */
	OutputDebugLog(L"CJpeg::cjpeg_main initialize JPEG parameters\n");

	m_cinfo.in_color_space = JCS_RGB; /* arbitrary guess */
	jpeg_set_defaults(&m_cinfo);

	/* Scan command line to find file names.
	 * It is convenient to use just one switch-parsing routine, but the switch
	 * values read here are ignored; we will rescan the switches after opening
	 * the input file.
	 */

	file_index = parse_switches(argc, argv, 0, FALSE);

	if (m_strict)
		jerr.emit_message = my_emit_message;

	/* Open the input file. */
	if (file_index < argc) {
		if (_tfopen_s(&m_input_file, argv[file_index], READ_BINARY) != 0) {
			_stprintf_s(errBuffer, JMSG_LENGTH_MAX, _T("failed to open input file:%s"), argv[file_index]);
			ERREXITS(&m_cinfo, JERR_CJPEG_INPUT_OPEN_FAIL, errBuffer);
		}
	}
	else {
		ERREXIT(&m_cinfo, JERR_CJPEG_INPUT_NOT_SET);
	}
	
	OutputDebugLog(L"CJpeg::cjpeg_main start reading file\n");

	if (m_icc_filename != NULL) {
		_tfopen_s(&icc_file, m_icc_filename, READ_BINARY);
		if (icc_file == NULL) {
			_stprintf_s(errBuffer, JMSG_LENGTH_MAX, _T("failed to open %s"), m_icc_filename);
			ERREXITS(&m_cinfo, JERR_CJPEG_ICC_FILE_OPEN_FAIL, errBuffer);
		}
		if (fseek(icc_file, 0, SEEK_END) < 0 ||
			(icc_len = ftell(icc_file)) < 1 ||
			fseek(icc_file, 0, SEEK_SET) < 0) {
			fclose(icc_file);
			_stprintf_s(errBuffer, JMSG_LENGTH_MAX, _T("failed to determine ICC file size: %s"), m_icc_filename);
			ERREXITS(&m_cinfo, JERR_CJPEG_ICC_FILE_SIZE_FAIL, errBuffer);
		}
		if ((icc_profile = (JOCTET*)malloc(icc_len)) == NULL) {
			fclose(icc_file);
			_stprintf_s(errBuffer, JMSG_LENGTH_MAX, _T("failed to allocate memory for ICC: %s"), m_icc_filename);
			ERREXITS(&m_cinfo, JERR_CJPEG_ICC_MALLOC, errBuffer);
		}
		if (fread(icc_profile, icc_len, 1, icc_file) < 1) {
			free(icc_profile);
			fclose(icc_file);
			_stprintf_s(errBuffer, JMSG_LENGTH_MAX, _T("failed to read from ICC file: %s"), m_icc_filename);
			ERREXITS(&m_cinfo, JERR_CJPEG_ICC_READ, errBuffer);
		}
		fclose(icc_file);
	}
#ifdef CJPEG_FUZZER
	jerr.error_exit = my_error_exit;
	jerr.emit_message = my_emit_message_fuzzer;
	if (setjmp(myerr.setjmp_buffer))
		HANDLE_ERROR()
#endif

	if (m_report) {
		start_progress_monitor((j_common_ptr)&m_cinfo, &progress);
		progress.report = m_report;
	}

	/* Figure out the input file format, and set up to read it. */
	src_mgr = select_file_type(m_input_file);
	src_mgr->input_file = m_input_file;
#ifdef CJPEG_FUZZER
	src_mgr->max_pixels = 1048576;
#endif

	/* Read the input file header to obtain file size & colorspace. */
	(*src_mgr->start_input) (&m_cinfo, src_mgr);

	/* Now that we know input colorspace, fix colorspace-dependent defaults */
#if JPEG_RAW_READER
	if (!is_jpeg)
#endif
		jpeg_default_colorspace(&m_cinfo);

	/* Adjust default compression parameters by re-parsing the options */
	file_index = parse_switches(argc, argv, 0, TRUE);

	/* Specify data destination for compression */
#if JPEG_LIB_VERSION >= 80 || defined(MEM_SRCDST_SUPPORTED)
	jpeg_mem_dest(&m_cinfo, &m_outbuffer, &m_outsize);
#else
	ERREXIT(&m_cinfo, JERR_CJPEG_MEM_SRCDST_UNSUPPORTED);
#endif

#ifdef CJPEG_FUZZER
	if (setjmp(myerr.setjmp_buffer))
		HANDLE_ERROR()
#endif

	/* Start compressor */
	OutputDebugLog(L"CJpeg::cjpeg_main start compressor\n");

	jpeg_start_compress(&m_cinfo, TRUE);

	/* Copy metadata */
	OutputDebugLog(L"CJpeg::cjpeg_main copy metadata\n");

	if (m_copy_markers) {
		jpeg_saved_marker_ptr marker;

		/* In the current implementation, we don't actually need to examine the
		 * option flag here; we just copy everything that got saved.
		 * But to avoid confusion, we do not output JFIF and Adobe APP14 markers
		 * if the encoder library already wrote one.
		 */
		for (marker = src_mgr->marker_list; marker != NULL; marker = marker->next) {
			if (m_cinfo.write_JFIF_header &&
				marker->marker == JPEG_APP0 &&
				marker->data_length >= 5 &&
				GETJOCTET(marker->data[0]) == 0x4A &&
				GETJOCTET(marker->data[1]) == 0x46 &&
				GETJOCTET(marker->data[2]) == 0x49 &&
				GETJOCTET(marker->data[3]) == 0x46 &&
				GETJOCTET(marker->data[4]) == 0)
				continue;                       /* reject duplicate JFIF */
			if (m_cinfo.write_Adobe_marker &&
				marker->marker == JPEG_APP0 + 14 &&
				marker->data_length >= 5 &&
				GETJOCTET(marker->data[0]) == 0x41 &&
				GETJOCTET(marker->data[1]) == 0x64 &&
				GETJOCTET(marker->data[2]) == 0x6F &&
				GETJOCTET(marker->data[3]) == 0x62 &&
				GETJOCTET(marker->data[4]) == 0x65)
				continue;                       /* reject duplicate Adobe */
			jpeg_write_marker(&m_cinfo, marker->marker, marker->data,
				marker->data_length);
		}
	}
	if (icc_profile != NULL)
		jpeg_write_icc_profile(&m_cinfo, icc_profile, (unsigned int)icc_len);

	/* Process data */
	OutputDebugLog(L"CJpeg::cjpeg_main process data\n");

	int count = 0;
	while (m_cinfo.next_scanline < m_cinfo.image_height) {
		while (*m_Paused) {
			Sleep(500);
			if (WaitForSingleObject(m_pSyncAbort->m_hObject, 0) == WAIT_OBJECT_0) {
				break;
			}
		}
		if (count > 100) {
			if (WaitForSingleObject(m_pSyncAbort->m_hObject, 0) == WAIT_OBJECT_0) {
				jpeg_destroy_compress(&m_cinfo);
				fclose(m_input_file);
				m_input_file = NULL;
				if (icc_profile != NULL)
					free(icc_profile);
				return FALSE;
			};
			count = 0;
		}
		count++;
		num_scanlines = (*src_mgr->get_pixel_rows) (&m_cinfo, src_mgr);
#if JPEG_RAW_READER
		if (is_jpeg)
			(void)jpeg_write_raw_data(&m_cinfo, src_mgr->plane_pointer, num_scanlines);
		else
#endif
			(void)jpeg_write_scanlines(&m_cinfo, src_mgr->buffer, num_scanlines);
	}

	/* Finish compression and release memory */
	OutputDebugLog(L"CJpeg::cjpeg_main finish compression and release memory\n");

	(*src_mgr->finish_input) (&m_cinfo, src_mgr);
	jpeg_finish_compress(&m_cinfo);
	jpeg_destroy_compress(&m_cinfo);

	/* Close files, if we opened them */
	if (m_input_file != stdin) {
		fclose(m_input_file);
		m_input_file = NULL;
	}

	if (m_report)
		end_progress_monitor((j_common_ptr)&m_cinfo);

	free(icc_profile);

	/* All done. */
	OutputDebugLog(L"CJpeg::cjpeg_main complete\n");

	return TRUE;                     /* suppress no-return-value warnings */
}


CString CJpeg::Version()
{
	CStringA buff;
	buff.Format(" %s version % s(build % s)", PACKAGE_NAME, VERSION, BUILD);

	return CString(CA2T(buff));
}
