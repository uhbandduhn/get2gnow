/* -*- Mode: C; shift-width: 8; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * get2gnow is:
 * 	Copyright (c) 2009 Kaity G. B. <uberChick@uberChicGeekChick.Com>
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
 * License ("RPL") Version 1.5, or subsequent versions as allowed by the RPL,
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
 * user-visible display as defined in Section 6.4 (d):
 * 
 * Initial art work including: design, logic, programming, and graphics are
 * Copyright (C) 2009 Kaity G. B. and released under the RPL where sapplicable.
 * All materials not covered under the terms of the RPL are all still
 * Copyright (C) 2009 Kaity G. B. and released under the terms of the
 * Creative Commons Non-Comercial, Attribution, Share-A-Like version 3.0 US license.
 * 
 * Any & all data stored by this Software created, generated and/or uploaded by any User
 * and any data gathered by the Software that connects back to the User.  All data stored
 * by this Software is Copyright (C) of the User the data is connected to.
 * Users may lisences their data under the terms of an OSI approved or Creative Commons
 * license.  Users must be allowed to select their choice of license for each piece of data
 * on an individual bases and cannot be blanketly applied to all of the Users.  The User may
 * select a default license for their data.  All of the Software's data pertaining to each
 * User must be fully accessible, exportable, and deletable to that User.
 */

#define _GNU_SOURCE
#define _THREAD_SAFE


#define _GNU_SOURCE
#define _THREAD_SAFE

#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <gio/gio.h>
#include <glib-object.h>
#include <libsexy/sexy.h>
#include <libxml/parser.h>

#include "config.h"
#include "program.h"

#include "network.h"
#include "parser.h"
#include "gconfig.h"
#include "online-services.h"
#include "online-service.types.h"
#include "online-service.h"
#include "online-service-request.h"
#include "preferences.h"

#include "main-window.h"
#include "update-viewer.h"

#include "label.h"

#define	DEBUG_DOMAINS	"UI:Sexy:URLs:URIs:Links:TimelinesSexyTreeView:OnlineServices:Networking:Updates:XPath:Auto-Magical:label.c"
#include "debug.h"

#define	GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE((obj), TYPE_LABEL, LabelPrivate))

struct _LabelPrivate{
	OnlineService	*service;
	gdouble		update_id;
};

static void label_class_init(LabelClass *klass);
static void label_init(Label *label);
static void label_finalize(Label *label);

static gboolean url_check_word(char *word, int len);
static void label_url_activated_cb(Label *label, const gchar *uri);
static gssize find_first_non_user_name(const gchar *str);
static gchar *label_format_service_hyperlink(OnlineService *service, const gchar *url_prefix, gchar *services_resource, gboolean expand_hyperlinks, gboolean make_hyperlinks, gboolean titles_strip_uris);
static gchar *label_find_user_title(OnlineService *service, const gchar *uri, const gchar *services_resource, gboolean expand_hyperlinks, gboolean make_hyperlinks, gboolean titles_strip_uris);
static gchar *label_find_uri_title(OnlineService *service, const gchar *uri, const gchar *services_resource, gboolean expand_hyperlinks, gboolean make_hyperlinks, gboolean titles_strip_uris);

static gchar *label_get_uri_dom_xpath_element_content(SoupMessage *xml, const gchar *xpath, const gchar *max_nodes_name);


G_DEFINE_TYPE (Label, label, SEXY_TYPE_URL_LABEL);

static void label_class_init(LabelClass *klass){
	GObjectClass   *object_class=G_OBJECT_CLASS(klass);
	g_type_class_add_private(object_class, sizeof(LabelPrivate));
	object_class->finalize=(GObjectFinalizeFunc)label_finalize;
}

static void label_init(Label *label){
	if(!( label && IS_LABEL(label) )) return;
	GET_PRIVATE(label)->update_id=0.0;
	GET_PRIVATE(label)->service=NULL;
	
	gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
	g_object_set(label, "xalign", 0.0, "yalign", 0.0, "xpad", 0, "ypad", 0, NULL);
	g_signal_connect(label, "url-activated", (GCallback)label_url_activated_cb, NULL);
}/*label_init(gobject);*/

Label *label_new(void){
	return g_object_new(TYPE_LABEL, NULL);
}

static void label_finalize(Label *label){
	GET_PRIVATE(label)->update_id=0.0;
	G_OBJECT_CLASS(label_parent_class)->finalize(G_OBJECT(label));
}

static void label_url_activated_cb(Label *label, const gchar *uri){
	LabelPrivate *this=GET_PRIVATE(label);
	if( g_strrstr(uri, this->service->uri) && !g_strrstr(uri, "search") ){
		gchar *services_resource=g_strrstr( (g_strrstr(uri, this->service->uri)), "/");
		debug("MySexyLabel: Inserting: <%s@%s> in to current update.", &services_resource[1], this->service->uri );
		if(!in_reply_to_service) in_reply_to_service=this->service;
		if(!in_reply_to_status_id) in_reply_to_status_id=this->update_id;
		gchar *user_profile_link=NULL;
		if( online_services_has_connected(1) > 0 && !gconfig_if_bool(PREFS_UPDATES_NO_PROFILE_LINK, TRUE) )
			user_profile_link=g_strdup_printf(" ( http://%s%s )", this->service->uri, services_resource );
		gchar *users_at=g_strdup_printf("@%s%s ", &services_resource[1], (user_profile_link ?user_profile_link :""));
		update_viewer_sexy_insert_string(users_at);
		uber_free(users_at);
		if(user_profile_link) uber_free(user_profile_link);
	}else if(g_app_info_launch_default_for_uri(uri, NULL, NULL))
		debug("**NOTICE:** Opening URI: <%s>.", uri );
	else
		debug("**ERROR:** Can't handle URI: <%s>.", uri );
}

void label_set_text(OnlineService *service, Label *label, gdouble update_id, const gchar *text, gboolean expand_hyperlinks, gboolean make_hyperlinks){
	if(!( label && IS_LABEL(label) )) return;
	
	if(G_STR_EMPTY(text)){
		GET_PRIVATE(label)->service=NULL;
		GET_PRIVATE(label)->update_id=0.0;
		gtk_label_set_text(GTK_LABEL(label), "");
		return;
	}
	
	GET_PRIVATE(label)->service=service;
	GET_PRIVATE(label)->update_id=update_id;
	debug("Rendering sexy markup for <%s>'s update's ID: %f; update's text: %s", service->key, update_id, text);
	gchar *sexy_text=label_msg_format_urls(service, text, expand_hyperlinks, make_hyperlinks);
	sexy_url_label_set_markup(SEXY_URL_LABEL(label), sexy_text);
	debug("Rendered sexy markup: %s", sexy_text);
	uber_free(sexy_text);
}/*label_set_text*/

static gboolean url_check_word (char *word, int len){
#define	D(x)	{ (x), ((sizeof(x))-1) }
	static const struct {
		const char	*s;
		guint8		len;
	}
	
	prefix[] = {
		D("@"),
		D("!"),
		D("#"),
		D("ftp."),
		D("www."),
		D("irc."),
		D("ftp://"),
		D("irc://"),
		D("http://"),
		D("https://"),
	};
#undef D
	for(int i=0; i<G_N_ELEMENTS(prefix); i++){
		if(len<prefix[i].len) continue;
		
		/* This is pretty much case in-sensitive g_str_has_prefix(). */
		int j;
		for(j=0; j<prefix[i].len; j++)
			if(g_ascii_tolower(word[j])!=prefix[i].s[j]) break;
			
		if(j && j==prefix[i].len) return TRUE;
	}
	
	return FALSE;
}

static gssize find_first_non_user_name(const gchar *str){
	for(gssize i=0; str[i]; ++i)
		if(!(g_ascii_isalnum(str[i]) || str[i] == '_'))
			return i;
	return -1;
}

gchar *label_msg_format_urls(OnlineService *service, const gchar *message, gboolean expand_hyperlinks, gboolean make_hyperlinks){
	if(G_STR_EMPTY(message)) return g_strdup("");
	
	gboolean titles_strip_uris=gconfig_if_bool(PREFS_URLS_EXPANSION_REPLACE_WITH_TITLES, TRUE);
	
	gchar *result=NULL, *temp=NULL;
	gchar **words=g_strsplit_set(message, " \t\n", 0);
	for(gint i=0; words[i]; i++) {
		if(!url_check_word(words[i], strlen(words[i]))) continue;
		
		gboolean searching=FALSE;
		if(words[i][0]!='@' && !(searching=(words[i][0]=='!' || words[i][0]=='#')) ){
			debug("Rendering URI for display including title.  URI: '%s'.", words[i]);
			temp=label_find_uri_title(service, words[i], NULL, expand_hyperlinks, make_hyperlinks, titles_strip_uris);
			debug("Rendered URI for display.  %s will be replaced with %s.", words[i], temp);
			g_free(words[i]);
			words[i]=temp;
			continue;
		}
		
		gchar *url_prefix=g_strdup_printf("http%s://%s%s/%s%s%s%s", (words[i][0]=='@'&&service->https ?"s" :""), ( (words[i][0]=='#' && service->micro_blogging_service==Twitter) ?"search." :"" ), service->uri, (searching ?"search" :""), (searching && service->micro_blogging_service!=Twitter ?"/notice" :"" ), (searching ?"?q=" :""), (words[i][0]=='#' ?"%23" :(words[i][0]=='!' ?"%21" :"") ) );
		debug("Rendering OnlineService: <%s>'s %s %c link for: <%s@%s>.", service->key, (words[i][0]=='@' ?"user profile" :"search results"), words[i][0], words[i], service->uri);
		temp=label_format_service_hyperlink(service, url_prefix, words[i], expand_hyperlinks, make_hyperlinks, titles_strip_uris );
		debug("Rendered OnlineService: <%s>'s %s %c link for: <%s@%s> will be replaced by: %s.", service->key, (words[i][0]=='@' ?"user profile" :"search results"), words[i][0], words[i], service->uri, temp);
		uber_free(url_prefix);
		g_free(words[i]);
		words[i]=temp;
	}
	result=g_strjoinv(" ", words);
	g_strfreev(words);
	
	return result;
}/*label_msg_format_urls*/

static gchar *label_format_service_hyperlink(OnlineService *service, const gchar *url_prefix, gchar *services_resource, gboolean expand_hyperlinks, gboolean make_hyperlinks, gboolean titles_strip_uris){
	gchar *user_at_link=NULL;
	gssize end=0;
	gchar delim;
	
	if((end=(find_first_non_user_name(&services_resource[1])+1) ) ){
		delim=services_resource[end];
		services_resource[end]='\0';
	}
	
	gchar *title=NULL;
	gchar *uri=g_strdup_printf("%s%s", url_prefix, &services_resource[1]);
	if(!gconfig_if_bool(PREFS_URLS_EXPANSION_USER_PROFILES, TRUE))
		title=g_strdup("");
	else
		title=label_find_user_title(service, uri, services_resource, expand_hyperlinks, make_hyperlinks, titles_strip_uris);
	
	const gchar *hyperlink_suffix=( G_STR_N_EMPTY(title) ?"" :services_resource );
	if(!make_hyperlinks)
		user_at_link=g_strdup_printf( "<u>%s%s</u>", title, hyperlink_suffix );
	else if(G_STR_N_EMPTY(title))
		user_at_link=g_strdup(title);
	else
		user_at_link=g_strdup_printf( "<a href=\"%s\">%s%s</a>", uri, title, hyperlink_suffix );
	
	if(end){
		gchar *user_at_link2=g_strdup_printf(
				"%s%c%s",
				user_at_link, delim, &services_resource[end+1]
		);
		g_free(user_at_link);
		user_at_link=user_at_link2;
		user_at_link2=NULL;
	}
	uber_free(uri);
	uber_free(title);
	
	return user_at_link;
}/*label_format_service_hyperlink(service, "https://identi.ca/", "@user"||"#search||"!tag", expand_hyperlinks, make_hyperlinks, titles_strip_uris);*/

static gchar *label_find_user_title(OnlineService *service, const gchar *uri, const gchar *services_resource, gboolean expand_hyperlinks, gboolean make_hyperlinks, gboolean titles_strip_uris){
	gchar *title=NULL;
	gchar *title_test=NULL;
	
	if(!make_hyperlinks)
		title_test=g_strdup_printf("<u>%s</u>", uri);
	else
		title_test=g_strdup_printf("<a href=\"%s\">%s</a>", uri, uri);
	
	title=label_find_uri_title(service, uri, services_resource, expand_hyperlinks, make_hyperlinks, titles_strip_uris);
	
	if(g_str_equal(title, title_test)){
		g_free(title);
		title=g_strdup("");
	}
	g_free(title_test);
	
	return title;
}/*label_find_user_title(service, uri, "@user"||"#search||"!tag", expand_hyperlinks, make_hyperlinks);*/

static gchar *label_find_uri_title(OnlineService *service, const gchar *uri, const gchar *services_resource, gboolean expand_hyperlinks, gboolean make_hyperlinks, gboolean titles_strip_uris){
	gchar *temp=NULL;
	if(!make_hyperlinks)
		temp=g_strdup_printf("<u>%s</u>", uri);
	else
		temp=g_strdup_printf("<a href=\"%s\">%s</a>", uri, uri);
	
	if(gconfig_if_bool(PREFS_URLS_EXPANSION_DISABLED, FALSE) || !expand_hyperlinks || g_str_has_prefix(uri, "ftp://")  || g_str_has_prefix(uri, "irc://") )
		return temp;
	
	SoupMessage *msg=NULL;
	gchar *content_type=NULL;
	
	main_window_statusbar_printf("Please wait while %s's title is found.", uri);
	debug("Attempting to determine content-type for: %s.", uri);
	if(!(content_type=online_service_get_uri_content_type(service, uri, &msg))){
		debug("Unable to determine the content-type from uri: '%s'.", uri);
		return temp;
	}
	
	
	if(!g_str_equal(content_type, "text/html")){
		debug("Non-XHTML content-type from uri: '%s'.", uri);
		uber_free(content_type);
		return temp;
	}
	uber_free(content_type);
	
	
	gboolean searching=(services_resource && (services_resource[0]=='#' || services_resource[0]=='!' ));
	gchar *uri_title=NULL;
	if(!(uri_title=label_get_uri_dom_xpath_element_content(msg, "html->head->title", "body"))){
		if(searching)
			uri_title=g_strdup(service->uri);
		else if(services_resource && services_resource[0]=='@')
			uri_title=g_strdup_printf("<%s@%s>", &services_resource[1], service->uri);
		else
			return temp;
	}
	uber_free(temp);
	
	gchar *escaped_title=NULL;
	escaped_title=parser_escape_text(uri_title);
	uber_free(uri_title);
	
	debug("Attempting to display link info. title: %s for uri: '%s'.", escaped_title, uri);
	gchar *hyperlink_suffix1=NULL;
	if(services_resource && searching && !g_strrstr(escaped_title, services_resource))
		hyperlink_suffix1=g_strdup_printf("'s search results for %s", services_resource);
	
	gchar *hyperlink_suffix2=NULL;
	if(!titles_strip_uris)
		hyperlink_suffix2=g_strdup_printf(" &lt;- %s", uri);
	
	if(!make_hyperlinks)
		temp=g_strdup_printf("<u>%s%s%s</u>", escaped_title, (hyperlink_suffix1 ?hyperlink_suffix1 :""), (hyperlink_suffix2 ?hyperlink_suffix2 :""));
	else
		temp=g_strdup_printf("<a href=\"%s\">%s%s%s</a>", uri, escaped_title, (hyperlink_suffix1 ?hyperlink_suffix1 :""), (hyperlink_suffix2 ?hyperlink_suffix2 :"") );
	if(hyperlink_suffix1) g_free(hyperlink_suffix1);
	if(hyperlink_suffix2) g_free(hyperlink_suffix2);
	
	g_free(escaped_title);
	
	return temp;
}/*label_find_uri_title(service, uri, "@user"||"#search||"!tag", expand_hyperlinks, make_hyperlinks, titles_strip_uris);*/

static gchar *label_get_uri_dom_xpath_element_content(SoupMessage *xml, const gchar *xpath, const gchar *max_nodes_name){
	xmlDoc		*doc=NULL;
	xmlNode		*root_element=NULL;
	debug("Parsing xml document before searching for xpath: '%s' content.", xpath);
	if(!(doc=parse_xml_doc(xml, &root_element))){
		debug("Unable to parse xml doc.");
		xmlCleanupParser();
		return NULL;
	}
	
	xmlNode	*current_node=NULL;
	gchar	*xpath_content=NULL;
	
	gchar	**xpathv=g_strsplit(xpath, "->", -1);
	guint	xpath_depth=0, xpath_target_depth=g_strv_length(xpathv)-1;
	
	debug("Searching for xpath: '%s' content.", xpath);
	for(current_node=root_element; current_node; current_node=current_node->next){
		if(current_node->type != XML_ELEMENT_NODE ) continue;
		
		IF_DEBUG
			debug("**NOTICE:** Looking for XPath: %s; current depth: %d; targetted depth: %d.  Comparing against current node: %s.", xpathv[xpath_depth], xpath_depth, xpath_target_depth, current_node->name);
		if( xpath_depth>xpath_target_depth || g_str_equal(max_nodes_name, current_node->name) ) break;
		
		if(!g_str_equal(current_node->name, xpathv[xpath_depth])){
			if(xpath_depth==xpath_target_depth && !current_node->next){
				IF_DEBUG
					debug("**NOTICE:** Current node: %s, at depth: %d, is malformated and doesn't close.  Moving back 'up' one layer in the DOM.", current_node->name, xpath_depth);
				current_node=current_node->parent;
				xpath_depth--;
			}
			continue;
		}
		
		if(xpath_depth<xpath_target_depth){
			if(!(current_node->children && current_node->children->next)) continue;
			
			current_node=current_node->children;
			xpath_depth++;
			continue;
		}
		
		xpath_content=(gchar *)xmlNodeGetContent(current_node);
		break;
	}
	
	xmlFreeDoc(doc);
	xmlCleanupParser();
	g_strfreev(xpathv);
	
	if(!( ((xpath_content)) && (xpath_content=g_strstrip(xpath_content)) && G_STR_N_EMPTY(xpath_content) )){
		if(xpath_content) g_free(xpath_content);
		return NULL;
	}
	
	return xpath_content;
}/*label_get_uri_dom_xpath_element_content(soup_xml_response, "targetted->xpaths->element", "max_xpath_element");*/

