/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 2007 Brian Pepple <bpepple@fedoraproject.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __AVATAR_H__
#define __AVATAR_H__

#include <glib-object.h>

#include <gdk-pixbuf/gdk-pixbuf.h>

G_BEGIN_DECLS

#define TYPE_AVATAR (avatar_get_gtype ())

typedef struct _TwituxAvatar TwituxAvatar;

struct _TwituxAvatar {
	guchar    *data;
	gsize      len;
	gchar     *format;
	GdkPixbuf *pixbuf;
	guint      refcount;
};

GType          avatar_get_gtype               (void) G_GNUC_CONST;
TwituxAvatar * avatar_new                     (guchar *avatar,
													  gsize len,
													  gchar *format);
GdkPixbuf    * avatar_get_pixbuf              (TwituxAvatar *avatar);
GdkPixbuf    * avatar_create_pixbuf_with_size (TwituxAvatar *avatar,
													  gint          size);

TwituxAvatar * avatar_ref                     (TwituxAvatar *avatar);
void           avatar_unref                   (TwituxAvatar *avatar);

G_END_DECLS

#endif /* __AVATAR_H__ */
