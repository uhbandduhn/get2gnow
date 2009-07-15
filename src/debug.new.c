/* -*- Mode: C; shift-width: 8; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * get2gnow is:
 * 	Copyright (c) 2006-2009 Kaity G. B. <uberChick@uberChicGeekChick.Com>
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
/********************************************************************************
 *                      My art, code, & programming.                            *
 ********************************************************************************/
#define _GNU_SOURCE
#define _THREAD_SAFE



/********************************************************************************
 *      Project, system, & library headers.  eg #include <gdk/gdkkeysyms.h>     *
 ********************************************************************************/
#include <errno.h>
#include <stdarg.h>
#include <string.h>

#include <glib.h>
#include <glib/gprintf.h>

#include "config.h"
#include "program.h"

#include "cache.h"
/********************************************************************************
 *               object methods, handlers, callbacks, & etc.                    *
 ********************************************************************************/
static gboolean debug_reinit(void);


/********************************************************************************
 *                 prototypes for private method & function                     *
 ********************************************************************************/
static FILE *debug_log_rotate(void);
static void debug_environment_check(void);
static void debug_domains_check(const gchar *domains);
static void debug_output(FILE *debug_output_fp, const gchar *debug_domain, const gchar *msg, va_list args);

/********************************************************************************
 *              Debugging information static objects, and local defines         *
 ********************************************************************************/
static gchar **debug_environment=NULL;
static gboolean all_domains=FALSE;
static gboolean debug_devel=FALSE;

static gchar **debug_domains=NULL;
static gchar *debug_last_domains=NULL;
static gchar *debug_last_domain=NULL;

static gchar *debug_environmental_variable=NULL;
static FILE *debug_log_fp=NULL;
static const gchar *debug_log_filename=NULL;
static const gchar *debug_log_filename_swp=NULL;

static gboolean debug_pause=FALSE;
static gboolean debug_output_enabled=FALSE;
static guint debug_timeout_id=0;

/*
 * Set DEBUG_DOMAINS to a colon separated list of debugging areas.
 * Each value is checked against value in the environmental variable:
 *	GETTEXT_PACKAGE_DEBUG, in this case its: GET2GNOW_DEBUG.
 */
#define DEBUG_DOMAINS "All"
#include "debug.h"


/********************************************************************************
 *              creativity...art, beauty, fun, & magic...programming            *
 ********************************************************************************/
gboolean debug_if_devel(void){
	return debug_devel;
}/*IF_DEVEL==if(debug_if_devel())*/

gboolean debug_check_devel(const gchar *debug_environmental_value){
#ifndef GNOME_ENABLE_DEBUG
	if(!(debug_environmental_value && g_str_equal(debug_environmental_value, "ARTISTIC" ) ))
		return (debug_devel=FALSE);
	#define GNOME_ENABLE_DEBUG
	
#else
	if(!(debug_environmental_value && g_str_equal(debug_environmental_value, "ARTISTIC" ) ))
		return (debug_devel=FALSE);
#endif
	
	if(debug_environment) g_strfreev(debug_environment);
	
	debug_output_enabled=TRUE;
	all_domains=TRUE;
	debug_environment=g_strsplit("UI:All", ":", -1);
	
	return (debug_devel=TRUE);
}/*debug_check_devel(g_getenv("GET2GNOW_DEBUG"));*/

void debug_init(void){
	/*re-checks GET(2GNOW_DEBUG every 10 minutes.*/
	debug_timeout_id=g_timeout_add_seconds(600, (GSourceFunc)debug_reinit, NULL);
	
	gchar *debug_package=g_utf8_strup(PACKAGE_TARNAME, -1);
	debug_environmental_variable=g_strdup_printf("%s_DEBUG", debug_package);
	g_free(debug_package);
	
	debug_log_filename=cache_file_touch("debug.log");
	debug_log_filename_swp=g_strdup_printf("%s.swp", debug_log_filename);
	debug_log_fp=fopen(debug_log_filename, "w");
	
	debug_environment_check();
}/*debug_init();*/

static gboolean debug_reinit(void){
	debug_pause=TRUE;
	debug_timeout_id=g_timeout_add(600000, (GSourceFunc)debug_reinit, NULL);
	debug_environment_check();
	return (debug_pause=FALSE);
}/*static void debug_refresh(void);*/

void debug_deinit(void){
	program_timeout_remove(&debug_timeout_id, "DEBUG environment testing.");
	
	fclose(debug_log_fp);
	g_free((gchar *)debug_log_filename);
	g_free((gchar *)debug_log_filename_swp);
	
	g_free(debug_environmental_variable);
	g_strfreev(debug_environment);
	
	if(debug_last_domains) g_free(debug_last_domains);
	if(debug_last_domain) g_free(debug_last_domain);
	if(debug_domains) g_strfreev(debug_domains);
}/*debug_deinit();*/

static void debug_environment_check(void){
	const gchar *debug_environmental_value=g_getenv(debug_environmental_variable);
	if(debug_check_devel(debug_environmental_value)) return;
	
	if(debug_environment) g_strfreev(debug_environment);
	if(!debug_environmental_value){
		debug_environment=g_strsplit("UI:All", ":", -1);
		debug_output_enabled=FALSE;
		return;
	}
	
	debug_environment=g_strsplit(debug_environmental_value, ":", -1);
	debug_output_enabled=TRUE;
	
	for(guint i=0; debug_environment[i]; i++)
		if(!strcasecmp("All", debug_environment[i]))
			all_domains=TRUE;
}/*debug_environment_check();*/

static void debug_domains_check(const gchar *domains){
	//if( (debug_last_domains) && g_str_equal(domains, debug_last_domains) ) return;
	
	if(debug_last_domains) g_free(debug_last_domains);
	debug_last_domains=g_strdup(domains);
	
	if(debug_domains) g_strfreev(debug_domains);
	debug_domains=g_strsplit(domains, ":", -1);
	
	if(debug_last_domain) uber_free(debug_last_domain);
}/*debug_domains_check(domain);*/

static FILE *debug_log_rotate(void){
	struct stat debug_log_stat;
	
	if(stat(debug_log_filename, &debug_log_stat)){
		g_error("Failed to stat log file: %s.", debug_log_filename);
		return NULL;
	}
	
	/*Not a mega-byte but close.. cause get2gnow has tons of debugging output.*/
	if(debug_log_stat.st_size <= 1000000)
		return debug_log_fp;
	
	fclose(debug_log_fp);
	debug_log_fp=fopen(debug_log_filename, "r");
	FILE *debug_log_fp_swap=fopen(debug_log_filename_swp, "w");
	
	/*Save 1/10 of the log so far.*/
	gchar *debug_log_swap_contents=NULL;
	fread(debug_log_swap_contents, sizeof(gchar), 25000, debug_log_fp);
	fwrite(debug_log_swap_contents, sizeof(gchar), 25000, debug_log_fp_swap);
	fclose(debug_log_fp_swap);
	fclose(debug_log_fp);
	g_remove(debug_log_filename);
	g_rename(debug_log_filename_swp, debug_log_filename);
	debug_log_fp=fopen(debug_log_filename, "a");
	return debug_log_fp;
}/*debug_log_rotate();*/

void debug_printf(const gchar *domains, const gchar *msg, ...){
	if(!(domains && msg)) return;
	while(debug_pause){}
	
	FILE *debug_output_fp=NULL;
	
	debug_domains_check(domains);
	gboolean debug_x_found=FALSE;
	const gchar *debug_domain=NULL;
	for(guint x=0; debug_domains[x] && !debug_x_found; x++){
		for(guint y=0; debug_environment[y] && !debug_x_found; y++) {
			if(!(all_domains && strcasecmp(debug_domains[x], debug_environment[y]) ))
				continue;
			debug_x_found=TRUE;
			debug_output_fp=stdout;
			debug_domain=debug_environment[y];
			break;
		}
	}
	if(!(debug_domain && debug_output_fp)){
		debug_domain=debug_domains[(g_strv_length(debug_domains)-1)];
		debug_output_fp=debug_log_rotate();
	}
	
	va_list args;
	va_start(args, msg);
	debug_output(debug_output_fp, debug_domain, msg, args);
	va_end(args);
}/*debug_printf(DEBUG_DOMAINS, "message" __format, format_args...);*/

static void debug_output(FILE *debug_output_fp, const gchar *debug_domain, const gchar *msg, va_list args){
	static gboolean output_started=FALSE;
	
	time_t current_time=time(NULL);
	const gchar *datetime=ctime(&current_time);
	
	if(!output_started){
		output_started=TRUE;
		g_fprintf(debug_output_fp, "\n");
	}
	
	if(g_str_has_prefix(msg, "**ERROR:**")){
		g_fprintf(stderr, "\n**%s %s %s**", _(GETTEXT_PACKAGE), _(debug_domain), _("error"));
		g_vfprintf(stderr, msg, args);
		g_fprintf(stderr, " @ %s", datetime);
	}
	
	if(!( debug_last_domain && g_str_equal(debug_last_domain, debug_domain) )){
		if(debug_last_domain) g_free(debug_last_domain);
		debug_last_domain=g_strdup(debug_domain);
		g_fprintf(debug_output_fp, "\n%s:\n", debug_domain);
	}
	
	g_fprintf(debug_output_fp, "\t\t");
	g_vfprintf(debug_output_fp, msg, args);
	g_fprintf(debug_output_fp, " @ %s", datetime);
}/*debug_output(message, args);*/

gboolean debug_if_domain(const gchar *domains){
	if(G_STR_EMPTY(domains))
		return FALSE;
	
	debug_domains_check(domains);
	for(guint x=0; debug_domains[x]; x++){
		for(guint y=0; debug_environment[y]; y++) {
			if(!(all_domains && strcasecmp(debug_domains[x], debug_environment[y]) ))
				continue;
			
			return TRUE;
		}
	}
	return FALSE;
}/*macro:DEBUG_IF==if(debug_if_domain(DEBUG_DOMAINS))*/


/********************************************************************************
 *                                    eof                                       *
 ********************************************************************************/
