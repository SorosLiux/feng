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
 *	- Federico Ridolfo	<federico.ridolfo@polito.it>
 *	- Marco Penno		<marco.penno@polito.it>
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

#include <fenice/mpeg.h>
#include <fenice/mpeg_utils.h>
#include <fenice/MediaParser.h>
#include <fenice/mediaparser_module.h>
#include <fenice/utils.h>
#include <fenice/types.h>

static MediaParserInfo info = {
	"MPV",
	"V"
};

FNC_LIB_MEDIAPARSER(mpv);

/*see ISO/IEC 11172-2:1993 and ISO/IEC 13818-2:1995 (E)*/
/*prefix*/
#define START_CODE 0x000001

/*value*/
#define PICTURE_START_CODE 0x00
#define SLICE_START_CODE /*0x01 to 0xAF*/
#define USER_DATA_START_CODE 0xB2
#define SEQ_START_CODE 0xB3
#define SEQ_ERROR_CODE 0xB4
#define EXT_START_CODE 0xB5 
#define SEQ_END_CODE 0xB7
#define GOP_START_CODE 0xB8

typedef struct _MPV_DATA{
	video_spec_head1 vsh1;
	video_spec_head2 vsh2;
	char temp_ref;
	char hours;
	char minutes;
	char seconds;
	char picture;
	standard std;
}mpv_data;

static int seq_head(uint8 *dst, uint32 dst_remained, uint8 *src, uint32 src_remained, mpv_data *mpeg_video);
static int seq_ext(uint8 *dst, uint32 dst_remained, uint8 *src, uint32 src_remained, mpv_data *mpeg_video);
static int ext_and_user_data(uint8 *dst, uint32 dst_remained, uint8 *src, uint32 src_remained, mpv_data *mpeg_video);
static int gop_head(uint8 *dst, uint32 dst_remained, uint8 *src, uint32 src_remained, mpv_data *mpeg_video);
static int picture_coding_ext(uint8 *dst, uint32 dst_remained, uint8 *src, uint32 src_remained, mpv_data *mpeg_video);
static int picture_head(uint8 *dst, uint32 dst_remained, uint8 *src, uint32 src_remained, mpv_data *mpeg_video);
static int picture_data(uint8 *dst, uint32 dst_remained, uint8 *src, uint32 src_remained, mpv_data *mpeg_video);
static int slice(uint8 *dst, uint32 dst_remained, uint8 *src, uint32 src_remained);

/*mediaparser_module interface implementation*/
static int init(MediaProperties *properties, void **private_data)
{
	*private_data=malloc(sizeof(mpv_data));
	return 0;
}

static int get_frame2(uint8 *dst, uint32 dst_nbytes, int64 *timestamp, InputStream *istream, MediaProperties *properties, void *private_data)
{
	return 0;
}


/*see RFC 2250: RTP Payload Format for MPEG1/MPEG2 Video*/
/*
 * A possible use is:
 *
 get_frame2();
 do {
	ret=packetize(dst, dst_nbytes, src, src_nbytes, properties, *private_data);
	src+=ret;
	src_nbytes-=ret;
	rtp_marker_bit = !(src_nbytes>0);
	// use dst
 } while(src_nbytes);

 *
 */
static int packetize(uint8 *dst, uint32 dst_nbytes, uint8 *src, uint32 src_nbytes, MediaProperties *properties, void *private_data)
{
	int ret=0;
	int dst_remained=dst_nbytes;
	int src_remained=src_nbytes;
	uint8 final_byte;
	mpv_data *mpeg_video;

	mpeg_video=(mpv_data *)private_data;
		
	do{
		ret=next_start_code2(dst,dst_remained,src,src_remained);
		if(ret==-1)
			return min(dst_nbytes,src_nbytes); /*if src_nbytes => marker=1 because src contains a frame*/
		dst_remained-=ret;
		src_remained-=ret;
		dst+=ret;
		src+=ret;
		final_byte=src[0];

		if(final_byte==SEQ_START_CODE) {
			ret=seq_head(dst,dst_remained,src,src_remained,mpeg_video);
			dst_remained-=ret;
			src_remained-=ret;
			dst+=ret;
			src+=ret;
			final_byte=src[0];
			if(final_byte==EXT_START_CODE) {
				/*means MPEG2*/
				mpeg_video->std=MPEG_2;
				ret=seq_ext(dst,dst_remained,src,src_remained,mpeg_video);
				dst_remained-=ret;
				src_remained-=ret;
				dst+=ret;
				src+=ret;
				final_byte=src[0];
			}
			else
				mpeg_video->std=MPEG_1;
		}
		else if(final_byte==GOP_START_CODE) {
			ret=gop_head(dst,dst_remained,src,src_remained,mpeg_video);
			dst_remained-=ret;
			src_remained-=ret;
			dst+=ret;
			src+=ret;
			final_byte=src[0];
		}
		else if(final_byte==PICTURE_START_CODE) {
			ret=picture_head(dst,dst_remained,src,src_remained,mpeg_video);
			dst_remained-=ret;
			src_remained-=ret;
			dst+=ret;
			src+=ret;
			final_byte=src[0];
			
			if(final_byte==USER_DATA_START_CODE) {
				ret=ext_and_user_data(dst,dst_remained,src,src_remained,mpeg_video);
				dst_remained-=ret;
				src_remained-=ret;
				dst+=ret;
				src+=ret;
				final_byte=src[0];
			}
			 do {
				ret=slice(dst,dst_remained,src,src_remained);
			 	dst_remained-=ret;
			 	src_remained-=ret;
				dst+=ret;
				src+=ret;
				final_byte=src[0];
			 } while(ret!=-1 && final_byte>=0x01 && final_byte<=0xAF /*SLICE_START_CODE*/);
			 
		}
		else if(final_byte>=0x01 && final_byte<=0xAF /*SLICE_START_CODE*/) {
			if(ret!=0 ){ 
				/*means that the end of a fragmented slice is arreached*/
				return ret;
			}
			else {
			 	do {
					ret=slice(dst,dst_remained,src,src_remained);
			 		dst_remained-=ret;
			 		src_remained-=ret;
					dst+=ret;
					src+=ret;
					final_byte=src[0];
				 } while(ret!=-1 && final_byte>=0x01 && final_byte<=0xAF /*SLICE_START_CODE*/);
			}
		
		}
		else if(final_byte==SEQ_END_CODE) {
			/*adding SEQ_END_CODE*/	
			if(dst_remained>=4) {
				dst[0]=0x00;
				dst[1]=0x00;
				dst[2]=0x01;
				dst[3]=SEQ_END_CODE;
			}
			else {
				/*i must send another packet??!?*/
			}
			/*in this case the video sequence should be finished, so min(dst_nbytes,src_nbytes) = src_nbytes*/
		}
	}while(ret!=-1);
	
	return min(dst_nbytes,src_nbytes); /*if src_nbytes => marker=1 because src contains a frame*/
}

static int uninit(void *private_data)
{
	return 0;
}


/*usefull function to parse*/
static int seq_head(uint8 *dst, uint32 dst_remained, uint8 *src, uint32 src_remained, mpv_data *mpeg_video)
{
	int count=0;
	/*
	 *read bitstream and increment dst and src, decrement dst_remained and src_remained
	 * */
	
	return count+next_start_code2(dst,dst_remained,src,src_remained);
}

static int seq_ext(uint8 *dst, uint32 dst_remained, uint8 *src, uint32 src_remained, mpv_data *mpeg_video)
{
	int count=0;
	/*
	 *read bitstream and increment dst and src, decrement dst_remained and src_remained
	 * */
	
	return count+next_start_code2(dst,dst_remained,src,src_remained);
}

static int ext_and_user_data(uint8 *dst, uint32 dst_remained, uint8 *src, uint32 src_remained, mpv_data *mpeg_video)
{
	int count=0;
	/*
	 *read bitstream and increment dst and src, decrement dst_remained and src_remained
	 * */
	
	return count+next_start_code2(dst,dst_remained,src,src_remained);
}

static int gop_head(uint8 *dst, uint32 dst_remained, uint8 *src, uint32 src_remained, mpv_data *mpeg_video)
{
	int count=0;
	/*
	 *read bitstream and increment dst and src, decrement dst_remained and src_remained
	 * */
	
	return count+next_start_code2(dst,dst_remained,src,src_remained);
}

static int picture_coding_ext(uint8 *dst, uint32 dst_remained, uint8 *src, uint32 src_remained, mpv_data *mpeg_video)
{
	int count=0;
	/*
	 *read bitstream and increment dst and src, decrement dst_remained and src_remained
	 * */
	
	return count+next_start_code2(dst,dst_remained,src,src_remained);
}

static int picture_head(uint8 *dst, uint32 dst_remained, uint8 *src, uint32 src_remained, mpv_data *mpeg_video)
{
	int ret;
	int count=0;
	uint8 final_byte;
	/*
	 *read bitstream and increment dst, src and count, decrement dst_remained and src_remained
	 * */
	/*---*/


	/*---*/
	ret=next_start_code2(dst,dst_remained,src,src_remained);
	dst_remained-=ret;
	src_remained-=ret;
	dst+=ret;
	src+=ret;
	count+=ret;
	final_byte=src[0];
	if(final_byte==EXT_START_CODE) 	
		count+=picture_coding_ext(dst,dst_remained,src,src_remained,mpeg_video);

	return count+next_start_code2(dst,dst_remained,src,src_remained); 
}


static int picture_data(uint8 *dst, uint32 dst_remained, uint8 *src, uint32 src_remained, mpv_data *mpeg_video)
{
	int count=0;
	/*
	 *read bitstream and increment dst and src, decrement dst_remained and src_remained
	 * */
	
	return count+next_start_code2(dst,dst_remained,src,src_remained);
}

static int slice(uint8 *dst, uint32 dst_remained, uint8 *src, uint32 src_remained)
{
	return next_start_code2(dst,dst_remained,src,src_remained);
}
