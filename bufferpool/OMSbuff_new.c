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

#include <stdio.h>
#include <stdlib.h>

#include <fenice/bufferpool.h>

OMSBuffer *OMSbuff_new(uint32 buffer_size)
{
	OMSSlot *head;
	OMSBuffer *buffer;
	uint32 index;
	
	if(!(head = (OMSSlot *)calloc(buffer_size, sizeof(OMSSlot))))
		return NULL;
	for(index=0; index<buffer_size-1; index++)
		(head[index]).next = &(head[index+1]);
	
	(head[index]).next=head; /*end of the list back to the head*/
	buffer = (OMSBuffer *)malloc(sizeof(OMSBuffer));
	buffer->write_pos = &(head[index]);
	buffer->buffer_head = head;
	buffer->added_head = NULL;
	buffer->refs = 0;
	// buffer->fd = -1;
	// buffer->fd = NULL;
	buffer->min_size = buffer_size-1;

	return buffer;
}
