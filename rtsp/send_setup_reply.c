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
#include <string.h>

#include <config.h>
#include <fenice/rtsp.h>
#include <fenice/utils.h>

int send_setup_reply(RTSP_buffer * rtsp, RTSP_session * session, char *address, RTP_session * sp2)
{
	char r[1024];
	char temp[30];
	/* build a reply message */
	
	sprintf(r, "%s %d %s\nCSeq: %d\nServer: %s/%s\n", RTSP_VER, 200, get_stat(200), rtsp->rtsp_cseq, PACKAGE,
		VERSION);
	add_time_stamp(r, 0);
	strcat(r, "Session: ");
	sprintf(temp, "%d", session->session_id);
	strcat(r, temp);
	strcat(r, "\n");
	if (sp2->isMulticast == NO_MULTICAST) {
		strcat(r, "Transport: RTP/AVP/UDP;unicast;client_port=");
		sprintf(temp, "%d", sp2->cli_ports.RTP);
		strcat(r, temp);
		strcat(r, "-");
		sprintf(temp, "%d", sp2->cli_ports.RTCP);
		strcat(r, temp);

		sprintf(temp, ";source=%s", address);
		strcat(r, temp);

		strcat(r, ";server_port=");
	} else {		//IS MULTICAST
		strcat(r, "Transport: RTP/AVP/UDP;multicast;");
		sprintf(temp, ";destination=%s", address);
		strcat(r, temp);

		strcat(r, ";port=");
	}
	sprintf(temp, "%d", sp2->ser_ports.RTP);
	strcat(r, temp);
	strcat(r, "-");
	sprintf(temp, "%d", sp2->ser_ports.RTCP);
	strcat(r, temp);

	sprintf(temp, ";ssrc=%u", session->rtp_session->ssrc);
	strcat(r, temp);

	strcat(r, "\r\n\r\n");

	
	bwrite(r, (unsigned short) strlen(r), rtsp);

#ifdef VERBOSE
	printf("SETUP response sent.\n");
#endif
	return ERR_NOERROR;
}