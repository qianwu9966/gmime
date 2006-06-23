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
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include <gmime/gmime.h>

#ifndef G_OS_WIN32
#define ENABLE_ZENTIMER
#include "zentimer.h"
#endif

/*#define TEST_RAW_HEADER*/
#define TEST_PRESERVE_HEADERS
#define TEST_GET_BODY
#define PRINT_MIME_STRUCT
/*#define TEST_WRITE_TO_STREAM*/

void
print_depth (int depth)
{
	int i;
	
	for (i = 0; i < depth; i++)
		fprintf (stdout, "   ");
}

void
print_mime_struct (GMimeObject *part, int depth)
{
	const GMimeContentType *type;
	gboolean has_md5;
	
	print_depth (depth);
	type = g_mime_object_get_content_type (part);
	has_md5 = g_mime_object_get_header (part, "Content-Md5") != NULL;
	fprintf (stdout, "Content-Type: %s/%s%s", type->type, type->subtype,
		 has_md5 ? "; md5sum=" : "\n");
	
	if (GMIME_IS_MULTIPART (part)) {
		GList *subpart;
		
		subpart = GMIME_MULTIPART (part)->subparts;
		while (subpart) {
			print_mime_struct (subpart->data, depth + 1);
			subpart = subpart->next;
		}
	} else if (GMIME_IS_MESSAGE_PART (part)) {
		GMimeMessagePart *mpart = (GMimeMessagePart *) part;
		
		if (mpart->message)
			print_mime_struct (mpart->message->mime_part, depth + 1);
	} else if (has_md5) {
		/* validate the Md5 sum */
		if (g_mime_part_verify_content_md5 ((GMimePart *) part))
			fprintf (stdout, "GOOD\n");
		else
			fprintf (stdout, "BAD\n");
	}
}

void
test_parser (GMimeStream *stream)
{
	GMimeParser *parser;
	GMimeMessage *message;
	gboolean is_html;
	char *text;
	
	fprintf (stdout, "\nTesting MIME parser...\n\n");
	
	parser = g_mime_parser_new ();
	g_mime_parser_init_with_stream (parser, stream);
	
	ZenTimerStart (NULL);
	message = g_mime_parser_construct_message (parser);
	ZenTimerStop (NULL);
	ZenTimerReport (NULL, "gmime::parser_construct_message");
	
	g_object_unref (parser);
	
	ZenTimerStart (NULL);
	text = g_mime_message_to_string (message);
	ZenTimerStop (NULL);
	ZenTimerReport (NULL, "gmime::message_to_string");
	/*fprintf (stdout, "Result should match previous MIME message dump\n\n%s\n", text);*/
	g_free (text);
	
#ifdef TEST_RAW_HEADER
	{
		char *raw;
		
		raw = g_mime_message_get_headers (message);
		fprintf (stdout, "\nTesting raw headers...\n\n%s\n", raw);
		g_free (raw);
	}
#endif
	
#ifdef TEST_PRESERVE_HEADERS
	{
		GMimeStream *stream;
		
		fprintf (stdout, "\nTesting preservation of headers...\n\n");
		stream = g_mime_stream_file_new (stdout);
		g_mime_header_write_to_stream (GMIME_OBJECT (message)->headers, stream);
		g_mime_stream_flush (stream);
		GMIME_STREAM_FILE (stream)->fp = NULL;
		g_mime_stream_unref (stream);
		fprintf (stdout, "\n");
	}
#endif
	
#ifdef TEST_GET_BODY
	{
		/* test of get_body */
		char *body;
		
		body = g_mime_message_get_body (message, FALSE, &is_html);
		fprintf (stdout, "Testing get_body (looking for html...%s)\n\n%s\n\n",
			 body && is_html ? "found" : "not found",
			 body ? body : "No message body found");
		
		g_free (body);
	}
#endif
	
#ifdef TEST_WRITE_TO_STREAM
	stream = g_mime_stream_fs_new (2);
	g_mime_object_write_to_stream (GMIME_OBJECT (message), stream);
	g_mime_stream_flush (stream);
	GMIME_STREAM_FS (stream)->fd = -1;
	g_mime_stream_unref (stream);
#endif
	
#ifdef PRINT_MIME_STRUCT
	/* print mime structure */
	print_mime_struct (message->mime_part, 0);
#endif
	
	g_mime_object_unref (GMIME_OBJECT (message));
}



/* you can only enable one of these at a time... */
/*#define STREAM_BUFFER*/
/*#define STREAM_MEM*/
/*#define STREAM_MMAP*/

int main (int argc, char **argv)
{
	char *filename = NULL;
	GMimeStream *stream;
	int fd;
	
	g_mime_init (0);
	
	if (argc > 1)
		filename = argv[1];
	else
		return 0;
	
	fd = open (filename, O_RDONLY);
	if (fd == -1)
		return 0;
	
#ifdef STREAM_MMAP
	stream = g_mime_stream_mmap_new (fd, PROT_READ, MAP_PRIVATE);
	g_assert (stream != NULL);
#else
	stream = g_mime_stream_fs_new (fd);
#endif /* STREAM_MMAP */
	
#ifdef STREAM_MEM
	istream = g_mime_stream_mem_new ();
	g_mime_stream_write_to_stream (stream, istream);
	g_mime_stream_reset (istream);
	g_mime_stream_unref (stream);
	stream = istream;
#endif
	
#ifdef STREAM_BUFFER
	istream = g_mime_stream_buffer_new (stream,
					    GMIME_STREAM_BUFFER_BLOCK_READ);
	g_mime_stream_unref (stream);
	stream = istream;
#endif
	
	test_parser (stream);
	
	g_mime_stream_unref (stream);
	
	g_mime_shutdown ();
	
	return 0;
}
