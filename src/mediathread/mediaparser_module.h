/* * 
 *  This file is part of Feng
 * 
 * Copyright (C) 2008 by LScube team <team@streaming.polito.it>
 * See AUTHORS for more details 
 *  
 * Feng is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 * 
 * Feng is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
 * General Public License for more details. 
 * 
 * You should have received a copy of the GNU General Public License
 * along with Feng; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *  
 * */

#ifndef FN_MEDIAPARSER_MODULE_H
#define FN_MEDIAPARSER_MODULE_H

#include "mediaparser.h"

#define INIT_PROPS properties->media_type = info.media_type;

#define FNC_LIB_MEDIAPARSER(x) const MediaParser fnc_mediaparser_##x =\
{\
	&info, \
	x##_init, \
        x##_parse, \
	x##_uninit \
}

#endif // FN_MEDIAPARSER_MODULE_H
