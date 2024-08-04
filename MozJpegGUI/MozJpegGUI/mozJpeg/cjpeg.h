
#pragma once

#include "cdjpeg.h"             /* Common decls for cjpeg/djpeg applications */

class CJpeg {
public:
	CJpeg();
	cjpeg_source_ptr select_file_type(FILE* infile);
	void usage();
	int parse_switches(int argc, TCHAR** argv, int last_file_arg_seen, boolean for_real);
	static void error_exit(j_common_ptr cinfo);
	static void output_message(j_common_ptr cinfo);
	static void my_emit_message(j_common_ptr cinfo, int msg_level);
	int cjpeg_main(int argc, TCHAR** argv);

	jpeg_compress_struct m_cinfo;

	boolean m_is_targa;        /* records user -targa switch */
	boolean m_is_jpeg;
	boolean m_copy_markers;
	unsigned char* m_outbuffer;
	unsigned long m_outsize;
	FILE* m_input_file;
	CSyncObject* m_pSyncAbort;
	bool* m_Paused;

	TCHAR* m_progname;    /* program name for error messages */	// const
	TCHAR* m_icc_filename;      /* for -icc switch */
	TCHAR* m_outfilename;       /* for -outfile switch */
	boolean m_memdst;                 /* for -memdst switch */
	boolean m_report;                 /* for -report switch */
	boolean m_strict;                 /* for -strict switch */
	static CString Version();
};

class CJpegException {
public:
	CJpegException(int e, CString str):e(e), str(str){};
	int e;
	CString str;
};
