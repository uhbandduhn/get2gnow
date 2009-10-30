/* -*- Mode: C; shift-width: 8; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * get2gnow is:
 * 	Copyright (c) 2006-2009 Kaity G. B. <uberChick@uberChicGeekChick.Com>
 * 	Released under the terms of the Reciprocal Public License (RPL).
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
/********************************************************************************
 *                      My art, code, & programming.                            *
 ********************************************************************************/
#define _GNU_SOURCE
#define _THREAD_SAFE

#include "program.h"
#include "template.h"


/********************************************************************************
 *      Project, system, & library headers.  eg #include <gdk/gdkkeysyms.h>     *
 ********************************************************************************/


/********************************************************************************
 *                        defines, macros, methods, & etc                       *
 ********************************************************************************/


/********************************************************************************
 *                        objects, structs, and enum typedefs                   *
 ********************************************************************************/


/********************************************************************************
 *                prototypes for private methods & functions                    *
 ********************************************************************************/


/********************************************************************************
 *               object methods, handlers, callbacks, & etc.                    *
 ********************************************************************************/


/********************************************************************************
 *              Debugging information static objects, and local defines         *
 ********************************************************************************/
#define DEBUG_DOMAINS "OnlineServices:UI:Networking:Updates:Requests:Notification:Settings:Setup:Users:Timelines:update.c"


/********************************************************************************
 *              creativity...art, beauty, fun, & magic...programming            *
 ********************************************************************************/


gboolean update_notify_on_timeout(gpointer data){
	UserStatus *status=(UserStatus *)data;
	if(!(status && G_STR_N_EMPTY(status->notification) )){
		return FALSE;
	}
	
	NotifyNotification *notify_notification;
	GError             *error=NULL;
	
	if(!gtk_status_icon_is_embedded(main_window->private->status_icon))
		notify_notification=notify_notification_new(PACKAGE_TARNAME, status->notification, PACKAGE_TARNAME, NULL);
	else
		notify_notification=notify_notification_new_with_status_icon(PACKAGE_TARNAME, status->notification, PACKAGE_TARNAME, main_window->private->status_icon);
	
	notify_notification_set_timeout(notify_notification, 10000);
	
	if(gconfig_if_bool(PREFS_NOTIFY_BEEP, TRUE))
		update_viewer_beep();
	
	notify_notification_show(notify_notification, &error);
	
	g_object_unref(G_OBJECT(notify_notification));
	
	if(error){
		debug("Error displaying status->notification: %s.", error->message);
		g_error_free(error);
	}
	return FALSE;
}/*main_window_notify_on_timeout - only used as a callback to g_timer_add_seconds_full - see 'src/parser.c'. */


/********************************************************************************
 *                                    eof                                       *
 ********************************************************************************/

