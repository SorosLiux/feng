/* * 
 *  $Id$
 *  
 *  This file is part of Fenice
 *
 *  Fenice -- Open Media Server
 *
 *  Copyright (C) 2004 by
 *  	
 *	- Giampaolo Mancini	<giampaolo.mancini@polito.it>
 *	- Francesco Varano	<francesco.varano@polito.it>
 *	- Marco Penno		<marco.penno@polito.it>
 *	- Federico Ridolfo	<federico.ridolfo@polito.it>
 * 
 *  Fenice is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Fenice is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Fenice; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *  
 * */

#include <fenice/demuxer.h>
#include <fenice/utils.h>
#include <fenice/fnc_log.h>

static void free_track(Track * t)
{
	close_is(t->i_stream);
	free(t->track_info);
	t->track_info=NULL;
	free_parser(t->parser);
	OMSbuff_free(t->buffer);
	if(t->private_data!=NULL) {
		free(t->private_data);
		t->private_data=NULL;
	}
	t->read_timestamp=NULL;
}

Resource * r_open(resource_name n)
{
	Resource *r;
	
	if((r=(Resource *)malloc(sizeof(Resource)))==NULL) {
		fnc_log(FNC_LOG_FATAL,"Memory allocation problems.\n");
		return NULL;
	}
	if((r->info=(ResourceInfo *)malloc(sizeof(ResourceInfo)))==NULL) {
		free(r);
		r=NULL;
		fnc_log(FNC_LOG_FATAL,"Memory allocation problems.\n");
		return NULL;
	}
	if((r->i_stream=create_inputstream(n))==NULL) {
		free(r->info);
		r->info=NULL;
		free(r);
		r=NULL;
		fnc_log(FNC_LOG_FATAL,"Memory allocation problems.\n");
		return NULL;
	}
	r->private_data=NULL;
	/*initializate tracks[MAX_TRACKS]??? TODO*/
	
	return r;
}

void r_close(Resource *r)
{
	int i;

	if(r!=NULL) {
		close_is(r->i_stream);
		free(r->info);
		r->info=NULL;
		if(r->private_data!=NULL) {
			free(r->private_data);
			r->private_data=NULL;
		}
		for(i=0;i<MAX_TRACKS;i++) 
			if(r->tracks[i]!=NULL)	/*r_close_track ??? TODO*/
				free_track(r->tracks[i]);
		free(r);
		r=NULL;
	}
}

msg_error get_resource_info(resource_name n , ResourceInfo *r)
{
	//...
	return RESOURCE_OK;
}

Selector * r_open_tracks(resource_name n, uint8 *track_name, Capabilities *capabilities)
{
	Selector *s;
	//...
	return s;
}

void r_close_tracks(Selector *s)
{
	//...
}

msg_error r_seek(Resource *r)
{
	//...
	return RESOURCE_OK;
}

Resource *init_resource(resource_name name)
{
	Resource *r;
	//...
	return r;
}

msg_error add_resource_info(Resource *r, .../*infos*/)
{
	//...
	return RESOURCE_OK;
}

msg_error add_track(Resource *r, const char *name, .../*infos*/)
{
	//...
	return RESOURCE_OK;
}
