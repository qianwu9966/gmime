/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*  GMime
 *  Copyright (C) 2000-2007 Jeffrey Stedfast
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation; either version 2.1
 *  of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301, USA.
 */


#ifndef __GMIME_COMMON_H__
#define __GMIME_COMMON_H__

#include <glib.h>

G_BEGIN_DECLS

int g_mime_strcase_equal (gconstpointer v, gconstpointer v2);

guint g_mime_strcase_hash (gconstpointer key);

#ifndef HAVE_STRNCASECMP
int strncasecmp (const char *s1, const char *s2, size_t n);
#endif

#ifndef HAVE_STRCASECMP
int strcasecmp (const char *s1, const char *s2);
#endif

G_END_DECLS

#endif /* __GMIME_COMMON_H__ */
