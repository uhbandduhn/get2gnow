/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 2009 Kaity G. B. <uberChick@uberChicGeekChick.Com>
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
 * Authors: Kaity G. B. <uberChick@uberChicGeekChick.Com>
 */

#ifndef __MAIN_H__
#define __MAIN_H__

#include "config.h"

G_BEGIN_DECLS

#ifndef G_STR_EMPTY
#define G_STR_EMPTY(x) ((x) == NULL || (x)[0] == '\0')
#endif


/* Proxy configuration */
#define PROXY				"/system/http_proxy"
#define PROXY_USE			PROXY "/use_http_proxy"
#define PROXY_HOST			PROXY "/host"
#define PROXY_PORT			PROXY "/port"
#define PROXY_USE_AUTH			PROXY "/use_authentication"
#define PROXY_USER			PROXY "/authentication_user"
#define PROXY_PASS			PROXY "/authentication_password"

/* File storage */
#define DIRECTORY			PACKAGE_NAME
#define CACHE_IMAGES			DIRECTORY "/avatars"

G_END_DECLS

#endif /* _MAIN_H_ */
