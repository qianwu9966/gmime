/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 *  Authors: Jeffrey Stedfast <fejj@ximian.com>
 *
 *  Copyright 2002 Ximian, Inc. (www.ximian.com)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Street #330, Boston, MA 02111-1307, USA.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <iconv.h>

enum {
	ISO_UNSUPPORTED          = 0,
	
	/* iso-8859-1 */
	ISO_DASH_D_DASH_D_LOWER  = (1 << 0),
	ISO_DASH_D_DASH_D        = (1 << 1),
	ISO_D_DASH_D             = (1 << 2),
	ISO_D_D                  = (1 << 3),
	ISO_UNDER_D_DASH_D       = (1 << 4),
	NO_ISO_D_DASH_D          = (1 << 5),
	
	/* iso-10646-1 */
	/*ISO_DASH_D_DASH_D_LOWER  = (1 << 0),*/
	/*ISO_DASH_D_DASH_D        = (1 << 1),*/
	/*ISO_D_DASH_D             = (1 << 2),*/
	ISO_DASH_D_LOWER           = (1 << 3),
	ISO_DASH_D                 = (1 << 4),
	ISO_D                      = (1 << 5),
	UCS4                       = (1 << 6),
	
	/* iso-2022-jp */
	ISO_DASH_D_DASH_S_LOWER  = (1 << 0),
	ISO_DASH_D_DASH_S        = (1 << 1),
	ISO_D_DASH_S             = (1 << 2),
};


typedef struct {
	char *charset;
	char *format;
	int id;
} CharInfo;


static CharInfo iso8859_tests[] = {
	{ "iso-8859-1",  "iso-%d-%d", ISO_DASH_D_DASH_D_LOWER },
	{ "ISO-8859-1",  "ISO-%d-%d", ISO_DASH_D_DASH_D },
	{ "ISO8859-1",   "ISO%d-%d",  ISO_D_DASH_D },
	{ "ISO88591",    "ISO%d%d",   ISO_D_D },
	{ "ISO_8859-1",  "ISO_%d-%d", ISO_UNDER_D_DASH_D },
	{ "8859-1",      "%d-%d",     NO_ISO_D_DASH_D },
};

static int num_iso8859_tests = sizeof (iso8859_tests) / sizeof (CharInfo);

static CharInfo iso2022_tests[] = {
	{ "iso-2022-jp", "iso-%d-%s", ISO_DASH_D_DASH_S_LOWER },
	{ "ISO-2022-JP", "ISO-%d-%s", ISO_DASH_D_DASH_S },
	{ "ISO2022-JP",  "ISO%d-%s",  ISO_D_DASH_S },
};

static int num_iso2022_tests = sizeof (iso2022_tests) / sizeof (CharInfo);

static CharInfo iso10646_tests[] = {
	{ "iso-10646-1", "iso-%d-%d",  ISO_DASH_D_DASH_D_LOWER },
	{ "ISO-10646-1", "ISO-%d-%d",  ISO_DASH_D_DASH_D },
	{ "ISO10646-1",  "ISO%d-%d",   ISO_D_DASH_D },
	{ "iso-10646",   "iso-%d",     ISO_DASH_D_LOWER },
	{ "ISO-10646",   "ISO-%d",     ISO_DASH_D },
	{ "ISO10646",    "ISO%d",      ISO_D },
	{ "UCS-4BE",     "UCS-4BE",    UCS4 },
};

static int num_iso10646_tests = sizeof (iso10646_tests) / sizeof (CharInfo);


int main (int argc, char **argv)
{
	unsigned int iso8859, iso2022, iso10646;
	CharInfo *info;
	iconv_t cd;
	FILE *fp;
	int i;
	
	fp = fopen ("iconv-detect.h", "w");
	if (fp == NULL)
		exit (255);
	
	fprintf (fp, "/* This is an auto-generated header, DO NOT EDIT! */\n\n");
	
	iso8859 = ISO_UNSUPPORTED;
	info = iso8859_tests;
	/*printf ("#define DEFAULT_ISO_FORMAT(iso,codepage)\t");*/
	for (i = 0; i < num_iso8859_tests; i++) {
		cd = iconv_open (info[i].charset, "UTF-8");
		if (cd != (iconv_t) -1) {
			iconv_close (cd);
			/*printf ("(\"%s\", (iso), (codepage))\n", info[i].format);*/
			fprintf (stderr, "System prefers %s\n", info[i].charset);
			iso8859 = info[i].id;
			break;
		}
	}
	
	if (iso8859 == ISO_UNSUPPORTED) {
		fprintf (stderr, "System doesn't support any ISO-8859-1 formats\n");
		fprintf (fp, "#define ICONV_ISO_D_FORMAT \"%s\"\n", info[0].format);
#ifdef CONFIGURE_IN
		exit (1);
#endif
	} else {
		fprintf (fp, "#define ICONV_ISO_D_FORMAT \"%s\"\n", info[i].format);
	}
	
	iso2022 = ISO_UNSUPPORTED;
	info = iso2022_tests;
	/*printf ("#define ISO_2022_FORMAT(iso,codepage)\t");*/
	for (i = 0; i < num_iso2022_tests; i++) {
		cd = iconv_open (info[i].charset, "UTF-8");
		if (cd != (iconv_t) -1) {
			iconv_close (cd);
			/*printf ("(\"%s\", (iso), (codepage))\n", info[i].format);*/
			fprintf (stderr, "System prefers %s\n", info[i].charset);
			iso2022 = info[i].id;
			break;
		}
	}
	
	if (iso2022 == ISO_UNSUPPORTED) {
		fprintf (stderr, "System doesn't support any ISO-2022 formats\n");
		fprintf (fp, "#define ICONV_ISO_S_FORMAT \"%s\"\n", info[0].format);
#ifdef CONFIGURE_IN
		exit (3);
#endif
	} else {
		fprintf (fp, "#define ICONV_ISO_S_FORMAT \"%s\"\n", info[i].format);
	}
	
	iso10646 = ISO_UNSUPPORTED;
	info = iso10646_tests;
	/*printf ("#define ISO_10646_FORMAT(iso,codepage)\t");*/
	for (i = 0; i < num_iso10646_tests; i++) {
		cd = iconv_open (info[i].charset, "UTF-8");
		if (cd != (iconv_t) -1) {
			iconv_close (cd);
			/*if (info[i].id < ISO_DASH_D_LOWER)
				printf ("(\"%s\", (iso), (codepage))\n", info[i].format);
			else
			printf ("(\"%s\", (iso))\n", info[i].format);*/
			fprintf (stderr, "System prefers %s\n", info[i].charset);
			iso10646 = info[i].id;
			break;
		}
	}
	
	/* we don't need a printf format for iso-10646 because there is only 1 */
	if (iso10646 == ISO_UNSUPPORTED) {
		fprintf (stderr, "System doesn't support any ISO-10646-1 formats\n");
		fprintf (fp, "#define ICONV_10646 \"%s\"\n", info[0].charset);
#ifdef CONFIGURE_IN
		exit (2);
#endif
	} else {
		fprintf (fp, "#define ICONV_10646 \"%s\"\n", info[i].charset);
	}
	
	fclose (fp);
	
	exit (0);
}
