/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2009 Brian Pepple <bpepple@fedoraproject.org>
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
 *
 * Authors: Kaity G. B. <uberChick@uberChicGeekChick.com>
 */

#ifndef __TWEETS_H__
#define __TWEETS_H__

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <libsoup/soup.h>


#include "app.h"
#include "tweet-list.h"

#define TWEETS_RETURN_MODIFIERS_STATUSBAR_MSG "HotKeys: press [Return] and '@' to reply, '>' to re-tweet, [Ctrl+N] to tweet, and/or [Ctrl+D] or <Shift>+[Return] to DM."

extern unsigned long int in_reply_to_status_id;

G_BEGIN_DECLS

void set_selected_tweet(unsigned long int id, const gchar *user_name, const gchar *tweet);
gchar *tweets_get_selected_user_name(void);
void unset_selected_tweet(void);
void tweets_show_submenu_entries(gboolean show);

/* G_CALLBACK_FUNC for Greet-Tweet-Know's "Tweets" menu & the 'extended_tweet_?_button'.
 *	When handled by Greet-Tweet-Know's gtkbuilder.[ch]
 *	UI builder methods.
 */
void tweets_new_tweet(void);
void tweets_reply(void);
void tweets_retweet(void);
void tweets_new_dm(void);
void tweets_make_fave(void);
void tweets_view_selected_timeline(void);
void tweets_view_selected_profile(void);

G_END_DECLS

#endif

