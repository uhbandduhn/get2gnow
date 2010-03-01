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


#include <string.h>
#include <strings.h>

#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <libsoup/soup-message.h>

#include "config.h"
#include "program.h"

#include "online-services.rest-uris.defines.h"
#include "online-services.h"
#include "online-service.typedefs.h"
#include "online-service.h"
#include "update-ids.h"
#include "network.h"

#include "users.types.h"
#include "users.h"

#include "main-window.h"
#include "uberchick-tree-view.h"
#include "preferences.defines.h"
#include "images.h"
#include "gconfig.h"

#include "best-friends.h"
#include "parser.h"

#define	DEBUG_DOMAINS	"Parser:Requests:OnlineServices:Updates:UI:Refreshing:Dates:Times:parser.c"
#include "debug.h"



xmlDoc *parse_xml_dom_content(SoupMessage *xml, xmlNode **root_element){
	xmlDoc *doc=NULL;
	
	SoupURI	*suri=NULL;
	gchar *uri=NULL;
	if(!( (((suri=soup_message_get_uri(xml)))) && ((uri=soup_uri_to_string(suri, FALSE))) && (G_STR_N_EMPTY(uri)) )){
		debug("*WARNING*: Unknown XML document URI.");
		if(uri) g_free(uri);
		uri=g_strdup("[*unknown*]");
	}
	if(suri) soup_uri_free(suri);
	
	
	if(!( xml->response_headers && xml->response_body && xml->response_body->data && xml->response_body->length )){
		debug("*ERROR:* Cannot parse empty or xml resonse from: %s.", uri);
		g_free(uri);
		return NULL;
	}
	
	
	gchar **content_v=NULL;
	debug("Parsing xml document's content-type & DOM information from: %s", uri);
	gchar *content_info=NULL;
	if(!(content_info=g_strdup((gchar *)soup_message_headers_get_one(xml->response_headers, "Content-Type")))){
		debug("*ERROR:* Failed to determine content-type for:  %s.", uri);
		g_free(uri);
		return NULL;
	}
	
	debug("Parsing content info: [%s] from: %s", content_info, uri);
	content_v=g_strsplit(content_info, "; ", -1);
	g_free(content_info);
	gchar *content_type=NULL;
	if(!( ((content_v[0])) && (content_type=g_strdup(content_v[0])) )){
		debug("*ERROR:*: Failed to determine content-type for:  %s.", uri);
		g_strfreev(content_v);
		g_free(uri);
		return NULL;
	}
	
	if(!( g_strrstr(content_type, "text") || g_strrstr(content_type, "xml") )){
		debug("*ERROR:* <%s>'s Content-Type: [%s] is not contain text or xml content and cannot be parsed any further.", uri, content_type );
		uber_free(content_type);
		return NULL;
	}
	
	debug("Parsed Content-Type: [%s] for: %s", content_type, uri);
	
	gchar *charset=NULL;
	if(!( ((content_v[1])) && (charset=g_strdup(content_v[1])) )){
		debug("*ERROR:* Failed to determine charset for:  %s.", uri);
		g_free(content_type);
		g_strfreev(content_v);
		g_free(uri);
		return NULL;
	}
	g_strfreev(content_v);
	
	
	debug("Parsing charset: [%s] for: %s", charset, uri);
	content_v=g_strsplit(charset, "=", -1);
	g_free(charset);
	if(!(content_v && content_v[1])){
		debug("Defaulting to charset: [utf-8] for: %s", uri);
		charset=g_strdup("utf-8");
	}else{
		charset=g_strdup(content_v[1]);
		debug("Setting parsed charset: [%s] for: %s", charset, uri);
	}
	g_strfreev(content_v);
	
	
	gchar *encoding=NULL;
	if(!g_str_equal(charset, "utf-8"))
		encoding=g_ascii_strup(charset, -1);
	else
		encoding=g_utf8_strup(charset, -1);
	
	content_v=g_strsplit(content_type, "/", -1);
	gchar *dom_base_entity=NULL;
	if(!( ((content_v[1])) && (dom_base_entity=g_strdup(content_v[1])) )){
		debug("**ERROR**: Failed to determine DOM base entity URL for: %s.", uri);
		g_free(uri);
		g_free(content_type);
		g_strfreev(content_v);
		return NULL;
	}
	g_strfreev(content_v);
	
	
	debug("Parsed xml document's information. URI: [%s] content-type: [%s]; charset: [%s]; encoding: [%s]; dom element: [%s]", uri, content_type, charset, encoding, dom_base_entity);
	
	
	debug("Parsing %s document.", dom_base_entity);
	if(!( (doc=xmlReadMemory(xml->response_body->data, xml->response_body->length, dom_base_entity, encoding, (XML_PARSE_NOENT | XML_PARSE_RECOVER | XML_PARSE_NOERROR | XML_PARSE_NOWARNING) )) )){
		debug("*ERROR:* Failed to parse %s document.", dom_base_entity);
		g_free(uri);
		g_free(content_type);
		g_free(dom_base_entity);
		g_free(charset);
		return NULL;
	}
	
	g_free(dom_base_entity);
	g_free(encoding);
	g_free(content_type);
	g_free(charset);
	
	debug("XML document parsed.  Getting XML nodes.");
	
	/* Get first element */
	if(!( *root_element=xmlDocGetRootElement(doc) )){
		debug("failed getting first element of xml data");
		xmlFreeDoc(doc);
		return NULL;
	}
	
	debug("XML document parsed & XML root node found: returning xmlDoc.");
	return doc;
}/*parse_dom_content();*/

const gchar *parser_xml_node_type_to_string(xmlElementType type){
	switch(type){
		case XML_ELEMENT_NODE: return _("XML_ELEMENT_NODE");
		case XML_ATTRIBUTE_NODE: return _("XML_ATTRIBUTE_NODE");
		case XML_TEXT_NODE: return _("XML_TEXT_NODE");
		case XML_CDATA_SECTION_NODE: return _("XML_CDATA_SECTION_NODE");
		case XML_ENTITY_REF_NODE: return _("XML_ENTITY_REF_NODE");
		case XML_ENTITY_NODE: return _("XML_ENTITY_NODE");
		case XML_PI_NODE: return _("XML_PI_NODE");
		case XML_COMMENT_NODE: return _("XML_COMMENT_NODE");
		case XML_DOCUMENT_NODE: return _("XML_DOCUMENT_NODE");
		case XML_DOCUMENT_TYPE_NODE: return _("XML_DOCUMENT_TYPE_NODE");
		case XML_DOCUMENT_FRAG_NODE: return _("XML_DOCUMENT_FRAG_NODE");
		case XML_NOTATION_NODE: return _("XML_NOTATION_NODE");
		case XML_HTML_DOCUMENT_NODE: return _("XML_HTML_DOCUMENT_NODE");
		case XML_DTD_NODE: return _("XML_DTD_NODE");
		case XML_ELEMENT_DECL: return _("XML_ELEMENT_DECL");
		case XML_ATTRIBUTE_DECL: return _("XML_ATTRIBUTE_DECL");
		case XML_ENTITY_DECL: return _("XML_ENTITY_DECL");
		case XML_NAMESPACE_DECL: return _("XML_NAMESPACE_DECL");
		case XML_XINCLUDE_START: return _("XML_XINCLUDE_START");
		case XML_XINCLUDE_END: return _("XML_XINCLUDE_END");
		case XML_DOCB_DOCUMENT_NODE: return _("XML_DOCB_DOCUMENT_NODE");
		default: return _("XML_TYPE_UNKNOWN");
	}
}/*parser_xml_node_type_to_string(node->type);*/

/* Parse a timeline XML file */
guint parse_timeline(OnlineService *service, SoupMessage *xml, const xmlDoc *doc, const xmlNode *root_element, const gchar *uri, UberChickTreeView *uberchick_tree_view, UpdateMonitor monitoring){
	const gchar	*timeline=g_strrstr(uri, "/");
	
	xmlNode		*current_node=NULL;
	UserStatus 	*status=NULL;
	
	/* Count new tweets */
	gboolean	notify=gconfig_if_bool(PREFS_NOTIFY_ALL, TRUE);;
	
	gboolean	save_oldest_id=(uberchick_tree_view_has_loaded(uberchick_tree_view)?FALSE:TRUE);
	gboolean	notify_best_friends=gconfig_if_bool(PREFS_NOTIFY_BEST_FRIENDS, TRUE);
	
	guint		new_updates=0, notified_updates=0;
	gint		update_expiration=0, best_friends_expiration=0;
	gconfig_get_int_or_default(PREFS_UPDATES_ARCHIVE_BEST_FRIENDS, &best_friends_expiration, 86400);
	
	gdouble		newest_update_id=0.0, unread_update_id=0.0, oldest_update_id=0.0;
	update_ids_get(service, timeline, &newest_update_id, &unread_update_id, &oldest_update_id);
	gdouble	last_notified_update=newest_update_id;
	newest_update_id=0.0;
	
	switch(monitoring){
		case	Searches: case	Groups:
			debug("*ERROR:* Unsupported timeline.  parse_timeline requested to parse %s.  This method sould not have been called.", (monitoring==Groups?"Groups":"Searches"));
			return 0;
			
		case	DMs:
			/*Direct Messages are kept for 4 weeks, by default.*/
			debug("Parsing DMs.");
			gconfig_get_int_or_default(PREFS_UPDATES_ARCHIVE_DMS, &update_expiration, 2419200);
			if(!notify) notify=gconfig_if_bool(PREFS_NOTIFY_DMS, TRUE);
			if(!oldest_update_id) save_oldest_id=TRUE;
			else save_oldest_id=FALSE;
			break;
		
		case	Replies:
			/*By default Replies, & @ Mentions, from the last 7 days are loaded.*/
			debug("Parsing Replies and/or @ Mentions.");
			gconfig_get_int_or_default(PREFS_UPDATES_ARCHIVE_REPLIES, &update_expiration, 604800);
			if(!notify) notify=gconfig_if_bool(PREFS_NOTIFY_REPLIES, TRUE);
			if(!oldest_update_id) save_oldest_id=TRUE;
			else save_oldest_id=FALSE;
			break;
		
		case	Faves:
			/*Favorite/Star'd updates are kept for 4 weeks, by default.*/
			debug("Parsing Faves.");
			gconfig_get_int_or_default(PREFS_UPDATES_ARCHIVE_FAVES, &update_expiration, 2419200);
			if(!oldest_update_id) save_oldest_id=TRUE;
			else save_oldest_id=FALSE;
			break;
		
		case	BestFriends:
			/*Best Friends' updates are kept for 1 day, by default.*/
			debug("Parsing best friends updates.");
			notify=notify_best_friends;
			gconfig_get_int_or_default(PREFS_UPDATES_ARCHIVE_BEST_FRIENDS, &update_expiration, 86400);
			if(!save_oldest_id) save_oldest_id=TRUE;
			break;
		
		case	Homepage:	case	ReTweets:
		case	Timelines:	case	Users:
			debug("Parsing updates from someone I'm following.");
			if(!notify) notify=gconfig_if_bool(PREFS_NOTIFY_FOLLOWING, TRUE);
			if(!save_oldest_id) save_oldest_id=TRUE;
			break;
			
		case	Archive:
			debug("Parsing my own updates or favorites.");
			break;
		
		case	None:	default:
			/* These cases are never reached - they're just here to make gcc happy. */
			return 0;
	}
	if(!oldest_update_id && notify && ( monitoring!=DMs || monitoring!=Replies ) ) notify=FALSE;
	
	guint		update_notification_delay=uberchick_tree_view_get_notify_delay(uberchick_tree_view);
	const gint	update_notification_interval=10;
	const gint	notify_priority=(uberchick_tree_view_get_page(uberchick_tree_view)+1)*100;
	
	if(!(doc=parse_xml_doc(xml, &root_element))){
		debug("Failed to parse xml document, <%s>'s timeline: %s.", service->key, timeline);
		xmlCleanupParser();
		return 0;
	}
	
	/* get updates or direct messages */
	debug("Parsing %s.", root_element->name);
	for(current_node=root_element; current_node; current_node=current_node->next) {
		if(current_node->type != XML_ELEMENT_NODE ) continue;
		
		if( g_str_equal(current_node->name, "statuses") || g_str_equal(current_node->name, "direct-messages") ){
			if(!current_node->children) continue;
			current_node=current_node->children;
			continue;
		}
		
		if(!( g_str_equal(current_node->name, "status") || g_str_equal(current_node->name, "direct_message") ))
			continue;
		
		if(!current_node->children){
			debug("*WARNING:* Cannot parse %s. Its missing children nodes.", current_node->name);
			continue;
		}
		
		debug("Parsing %s.", (g_str_equal(current_node->name, "status") ?"status update" :"direct message" ) );
		
		status=NULL;
		debug("Creating Status *.");
		if(!( (( status=user_status_parse(service, current_node->children, monitoring ))) && status->id )){
			if(status) user_status_free(status);
			continue;
		}
		
		new_updates++;
		gboolean notify_of_new_update=FALSE;
		/* id_oldest_tweet is only set when monitoring DMs or Replies */
		debug("Adding UserStatus from: %s, ID: %s, on <%s> to UberChickTreeView.", status->user->user_name, status->id_str, service->key);
		uberchick_tree_view_store_update(uberchick_tree_view, status);
		if( monitoring!=BestFriends && monitoring!=DMs && ( best_friends_is_user_best_friend(service, status->user->user_name) || ( status->retweet && best_friends_is_user_best_friend(service, status->retweeted_user_name) ) ) )
			if( (best_friends_check_update_ids( service, status->user->user_name, status->id)) && notify_best_friends)
				notify_of_new_update=TRUE;
			
		
		if( notify && !notify_of_new_update && !save_oldest_id && status->id > last_notified_update && strcasecmp(status->user->user_name, service->user_name) )
			notify_of_new_update=TRUE;
		
		if(!newest_update_id && status->id) newest_update_id=status->id;
		if(save_oldest_id && status->id) oldest_update_id=status->id;
		
		if(!notify_of_new_update)
			user_status_free(status);
		else{
			g_timeout_add_seconds_full(notify_priority, update_notification_delay, (GSourceFunc)user_status_notify_on_timeout, status, (GDestroyNotify)user_status_free);
			update_notification_delay+=update_notification_interval;
			notified_updates++;
		}
	}
	
	if(new_updates && newest_update_id){
		/*TODO implement
		 *cache_save_page(service, timeline, xml->response_body);
		 * this only once it won't ending up causing bloating.
		 */
		debug("Processing <%s>'s requested URI's: [%s] new update IDs", service->guid, timeline);
		debug("Saving <%s>'s; update IDs for [%s];  newest ID: %f; unread ID: %f; oldest ID: %f.", service->guid, timeline, newest_update_id, unread_update_id, oldest_update_id );
		update_ids_set(service, timeline, newest_update_id, unread_update_id, oldest_update_id);
	}
	
	return new_updates;
}/*parse_timeline(service, xml, timeline, uberchick_tree_view, monitoring);*/
