/* -*- Mode: C; shift-width: 8; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * get2gnow is:
 * 	Copyright(c) 2009 Kaity G. B. <uberChick@uberChicGeekChick.Com>
 * 	Released under the terms of the RPL
 *
 * For more information or to find the latest release, visit our
 * website at: http://uberChicGeekChick.Com/?projects=get2gnow
 *
 * Writen by an uberChick, other uberChicks please meet me & others @:
 * 	http://uberChicks.Net/
 *
 * I'm also disabled. I live with a progressive neuro-muscular disease.
 * DYT1+ Early-Onset Generalized Dystonia, a type of Generalized Dystonia.
 * 	http://Dystonia-DREAMS.Org/
 *
 *
 *
 * Unless explicitly acquired and licensed from Licensor under another
 * license, the contents of this file are subject to the Reciprocal Public
 * License("RPL") Version 1.5, or subsequent versions as allowed by the RPL,
 * and You may not copy or use this file in either source code or executable
 * form, except in compliance with the terms and conditions of the RPL.
 *
 * All software distributed under the RPL is provided strictly on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, AND
 * LICENSOR HEREBY DISCLAIMS ALL SUCH WARRANTIES, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, QUIET ENJOYMENT, OR NON-INFRINGEMENT. See the RPL for specific
 * language governing rights and limitations under the RPL.
 *
 * The User-Visible Attribution Notice below, when provided, must appear in each
 * user-visible display as defined in Section 6.4(d):
 * 
 * Initial art work including: design, logic, programming, and graphics are
 * Copyright(C) 2009 Kaity G. B. and released under the RPL where sapplicable.
 * All materials not covered under the terms of the RPL are all still
 * Copyright(C) 2009 Kaity G. B. and released under the terms of the
 * Creative Commons Non-Comercial, Attribution, Share-A-Like version 3.0 US license.
 * 
 * Any & all data stored by this Software created, generated and/or uploaded by any User
 * and any data gathered by the Software that connects back to the User.  All data stored
 * by this Software is Copyright(C) of the User the data is connected to.
 * Users may lisences their data under the terms of an OSI approved or Creative Commons
 * license.  Users must be allowed to select their choice of license for each piece of data
 * on an individual bases and cannot be blanketly applied to all of the Users.  The User may
 * select a default license for their data.  All of the Software's data pertaining to each
 * User must be fully accessible, exportable, and deletable to that User.
 */


#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <glib/gprintf.h>


#include "main.h"
#include "config.h"
#include "app.h"
#include "tweets.h"
#include "network.h"
#include "gconfig.h"
#include "preferences.h"
#include "tweet-list.h"
#include "tweet-view.h"
#include "profile-viewer.h"
#include "ui-utils.h"
#include "users.h"


typedef struct SelectedTweet {
	OnlineService *service;
	unsigned long int id;
	unsigned long int user_id;
	gchar *user_name;
	gchar *tweet;
	gchar *reply_to_string;
} SelectedTweet;//SelectedTweet
unsigned long int in_reply_to_status_id=0;
OnlineService *in_reply_to_service=NULL;
static SelectedTweet *selected_tweet=NULL;

#define	DEBUG_DOMAINS	"OnlineServices:Networking:Tweets:Requests:Users"
#include "debug.h"


static void tweets_include_and_begin_to_send(gchar *tweet, gboolean a_response, gboolean release);


void set_selected_tweet(OnlineService *service, unsigned long int id, unsigned long int user_id, const gchar *user_name, const gchar *tweet){
	/*	id=strtoul( char id, NULL, 10 );	*/
	if(selected_tweet)
		unset_selected_tweet();
	
	debug("SelectedTweet created from '%s', tweet id: #%lu from '%s' on '%s'.", service->key, id, user_name);
	selected_tweet=g_new0(SelectedTweet, 1);
	selected_tweet->service=service;
	selected_tweet->id=id;
	selected_tweet->user_id=user_id;
	selected_tweet->user_name=g_strdup(user_name);
	selected_tweet->tweet=g_uri_unescape_string(tweet, NULL);
	if(!G_STR_EMPTY(selected_tweet->user_name))
		if(!( (gconfig_if_bool(PREFS_TWEETS_NO_PROFILE_LINK, FALSE)) && online_services->connected > 1 ))
			selected_tweet->reply_to_string=g_strdup_printf("@%s ( http://%s/%s ) ", selected_tweet->user_name, selected_tweet->service->uri, selected_tweet->user_name);
		else
			selected_tweet->reply_to_string=g_strdup_printf("@%s ", selected_tweet->user_name);
	else
		selected_tweet->reply_to_string=NULL;
}/*set_selected_tweet*/

OnlineService *selected_tweet_get_service(void){
	return ( (selected_tweet && selected_tweet->service) ?selected_tweet->service :NULL );
}/*selected_tweet_get_service*/

gchar *selected_tweet_get_user_name(void){
	return ( (selected_tweet && selected_tweet->user_name) ?selected_tweet->user_name :NULL );
}/*selected_tweet_get_user_name*/

unsigned long int selected_tweet_get_user_id(void){
	return ( (selected_tweet && selected_tweet->user_id) ?selected_tweet->user_id :0 );
}/*selected_tweet_get_user_name*/

gchar *selected_tweet_get_reply_to_string(void){
	return ( (selected_tweet && selected_tweet->reply_to_string) ?selected_tweet->reply_to_string :NULL );
}/*selected_tweet_get_reply_to_string*/

void unset_selected_tweet(void){
	if(!selected_tweet) return;
	debug("Un-Setting selected_tweet.");
	
	if(selected_tweet->user_name)
		g_free(selected_tweet->user_name);
	selected_tweet->user_name=NULL;
	
	if(selected_tweet->reply_to_string)
		g_free(selected_tweet->reply_to_string);
	selected_tweet->reply_to_string=NULL;
	
	if(selected_tweet->tweet)
		g_free(selected_tweet->tweet);
	selected_tweet->tweet=NULL;
	
	selected_tweet->service=NULL;
	
	g_free(selected_tweet);
	selected_tweet=NULL;
}/*unset_selected_tweet*/

void tweets_hotkey(GtkWidget *widget, GdkEventKey *event){
	switch( event->state ){
		case GDK_MOD1_MASK:
			switch(event->keyval){
				case GDK_S: case GDK_s:
					return tweet_view_send(NULL);
				case GDK_Return: return tweets_retweet();
				case GDK_A: case GDK_a:
					return g_signal_emit_by_name(app_get_menu("accounts"), "activate");
				case GDK_T: case GDK_t:
					return g_signal_emit_by_name(app_get_menu("tweets"), "activate");
				case GDK_F: case GDK_f:
					return g_signal_emit_by_name(app_get_menu("friends"), "activate");
				case GDK_V: case GDK_v:
					return g_signal_emit_by_name(app_get_menu("timelines"), "activate");
				case GDK_H: case GDK_h:
					return g_signal_emit_by_name(app_get_menu("help"), "activate");
				/*
				case GDK_: case GDK_:
					return g_signal_emit_by_name(app_get_menu(""), "activate");
				*/
			}
			return;
		case GDK_SHIFT_MASK:
			if(event->keyval==GDK_Return)
				return tweets_new_dm();
			return;
		case GDK_CONTROL_MASK:
			switch( event->keyval ){
				case GDK_Return:
					return tweet_view_sexy_insert_char('\n');
				case GDK_N: case GDK_n:
					return tweets_new_tweet();
				case GDK_Q: case GDK_q:
					return gtk_main_quit();
				case GDK_R: case GDK_r:
					return tweets_reply();
				case GDK_F: case GDK_f:
					return tweets_retweet();
				case GDK_D: case GDK_d:
					return tweets_new_dm();
				case GDK_S: case GDK_s:
					return tweets_save_fave();
				case GDK_I: case GDK_i:
					return tweets_user_view_profile();
				case GDK_H: case GDK_h:
					return tweets_user_view_tweets();
				/*
				case GDK_: case GDK_:
					return tweets_();
				*/
			}
			return;
		default:
			switch( event->keyval ){
				case GDK_F1: return help_show( app_get_window() );
				case GDK_F5: return app_refresh_timeline( GTK_WIDGET(app_get_window()), app_get()); 
				case GDK_greater: return tweets_retweet();
				case GDK_at: return tweets_reply();
				case GDK_asciitilde: return tweets_new_dm();
			}
			tweet_list_key_pressed(GTK_WIDGET(app_get_tweet_list()), event, app_get_tweet_list());
			return;
	}
}/*tweets_hotkey*/

void tweets_new_tweet(void){
	if(in_reply_to_status_id) in_reply_to_status_id=0;
	if(in_reply_to_service) in_reply_to_service=NULL;
	tweet_view_show_tweet(online_services_connected_get_first(online_services), 0, 0, "", "", "", "", "", NULL);
	tweet_view_sexy_set((gchar *)"");
	unset_selected_tweet();
}/*tweets_new_tweet*/

void tweets_reply(void){
	if(!(selected_tweet && selected_tweet->reply_to_string)) return;
	gchar *tweet=g_strdup(selected_tweet->reply_to_string);
	tweets_include_and_begin_to_send(tweet, TRUE, TRUE);
}/*tweets_reply*/

void tweets_retweet(void){
	if(!(selected_tweet && selected_tweet->reply_to_string))
		return;
	gchar *tweet=g_strdup_printf("RT: %s%s", selected_tweet->reply_to_string, selected_tweet->tweet);
	tweets_include_and_begin_to_send(tweet, TRUE, TRUE);
}/*tweets_retweet*/

static void tweets_include_and_begin_to_send(gchar *tweet, gboolean in_response, gboolean release){
	if(!(tweet)) return;
	if(in_response) {
		in_reply_to_status_id=selected_tweet->id;
		in_reply_to_service=selected_tweet->service;
	}
	tweet_view_sexy_prefix_string(tweet);
	if(release) g_free(tweet);
}/*tweets_include_and_begin_to_send*/

void tweets_new_dm(void){
	tweet_view_new_dm();
}/*tweets_new_dm*/
	
void tweets_save_fave(void){
	if(!selected_tweet)
		return;
	gchar *fave_tweet_id=g_strdup_printf( "%lu", selected_tweet->id );
	user_request_main(selected_tweet->service, Fave, app_get_window(), fave_tweet_id);
	g_free(fave_tweet_id);
}/*tweets_save_fave*/

void tweets_user_view_tweets(void){
	if(!selected_tweet->user_name) return;
	user_request_main(selected_tweet->service, ViewTweets, app_get_window(), selected_tweet->user_name);
}/*tweets_user_view_tweets*/

void tweets_user_view_profile(void){
	if(!selected_tweet->user_name) return;
	user_request_main(selected_tweet->service, ViewProfile, app_get_window(), selected_tweet->user_name);
}/*tweets_user_view_profile*/

void tweets_user_follow(void){
	if(!selected_tweet->user_name) return;
	user_request_main(selected_tweet->service, Follow, app_get_window(), selected_tweet->user_name);
}/*tweets_user_follow*/

void tweets_user_unfollow(void){
	if(!selected_tweet->user_name) return;
	user_request_main(selected_tweet->service, UnFollow, app_get_window(), selected_tweet->user_name);
}/*tweets_user_unfollow*/

void tweets_user_block(void){
	if(!selected_tweet->user_name) return;
	user_request_main(selected_tweet->service, Block, app_get_window(), selected_tweet->user_name);
}/*tweets_user_block*/

void tweets_user_unblock(void){
	if(!selected_tweet->user_name) return;
	user_request_main(selected_tweet->service, UnBlock, app_get_window(), selected_tweet->user_name);
}/*tweets_user_unblock*/


/********************************************************
 *                       eof                            *
 ********************************************************/

