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
 *	- Eugenio Menegatti 	<m.eu@libero.it>
 *	- Stefano Cau
 *	- Giuliano Emma
 *	- Stefano Oldrini
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

#include <string.h>
#include <fenice/utils.h>
#include <fenice/mediainfo.h>
#include <fenice/types.h>

int get_frame(media_entry *me, double *mtime)
{
	int recallme=1;	
	OMSSlot *slot;
	int res = ERR_NOERROR;

	while(recallme && res!=ERR_EOF){
		slot=OMSbuff_getslot(me->pkt_buffer);
		
	        if (strcmp(me->description.encoding_name,"MPA")==0) {
        	        recallme = 0;
                	me->description.delta_mtime=me->description.pkt_len;/* For MPA, L16 and GSM delta_mtime is fixed to pkt_len */
			res = read_MP3(me,slot->data,&slot->data_size,mtime);
        	}	
		else if (strcmp(me->description.encoding_name,"L16")==0) {
                	recallme = 0;
                	me->description.delta_mtime=me->description.pkt_len;
                	res = read_PCM(me,slot->data,&slot->data_size,mtime);
        	}
		else if (strcmp(me->description.encoding_name,"GSM")==0) {
                	recallme = 0;
                	me->description.delta_mtime=me->description.pkt_len;
                	res = read_GSM(me,slot->data,&slot->data_size,mtime);
        	}
		else if (strcmp(me->description.encoding_name,"H26L")==0) {
                	res = read_H26L(me,slot->data,&slot->data_size,mtime,&recallme);
        	}
		else if (strcmp(me->description.encoding_name,"MPV")==0) {
                	res = read_MPEG_video(me,slot->data,&slot->data_size,mtime,&recallme);
			slot->marker = !recallme;
        	}
		else if (strcmp(me->description.encoding_name,"MP2T")==0) {
                	res = read_MPEG_system(me,slot->data,&slot->data_size,mtime,&recallme);
        	}
		else if (strcmp(me->description.encoding_name,"MP4V-ES")==0) {
                	res = read_MPEG4ES_video(me,slot->data,&slot->data_size,mtime,&recallme);
			if(recallme==0)
				slot->marker=1;
        	}
		
		else 
			res=ERR_UNSUPPORTED_PT;
		slot->timestamp=*mtime;
	}
	return res;
}
