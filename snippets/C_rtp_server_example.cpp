#include <jrtplib3/rtppacket.h>
#include <jrtplib3/rtpsession.h>
#include <jrtplib3/rtpudpv4transmitter.h>
#include <jrtplib3/rtpipv4address.h>
#include <jrtplib3/rtpsessionparams.h>
#include <jrtplib3/rtperrors.h>
#include <jrtplib3/rtpsourcedata.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <signal.h>

#include "OptionParser.h"
#include "common.h"

using namespace std;
using namespace jrtplib;

#define RX_VIDEO_DATA "video_out/kserver_rx.data"

extern "C" {
#  define UINT64_C(c)   c ## UL
#include "libavcodec/avcodec.h"
#include "libavutil/mathematics.h"
}

/*
 * Configurations
 */
static int arg_debug          = 0;
static int arg_verbose        = 0;
static int arg_frames         = 0;
static bool arg_raw           = false;

static double g_timestamp_unit      = 1.0/1100; 

/*
 * Variables
 */
static FILE * g_rec_video = NULL;
int g_dec_frames = 0;
int g_marker_count = 0;
int g_rtp_rx_packet_cnt = 0;
int g_rtp_rx_size  = 0;
uint32_t g_first_timestamp = 0;

bool g_found_frame_start = false;

/**************************
 * Declarations
 * ************************/

void clean_up();

/**************************
 * Functions
 * ************************/
void intHandler(int)
{
    clean_up();
    exit(0);
}

video_rtp_data_type video_rtp_data;

AVCodec *codec;
AVCodecContext *avc_ctx= NULL;
AVFrame *picture;
AVPacket avpkt;

#ifdef RTP_SUPPORT_THREAD

//
// This function checks if there was a RTP error. If so, it displays an error
// message and exists.
//

void checkerror(int rtperr)
{
	if (rtperr < 0)
	{
		std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
		exit(-1);
	}
}

int decode_video_packet(uint8_t* mpeg4_buf, int mpeg4_size, uint8_t** yuvbuf, int* linesize, int* got_picture)
{
    int this_picture;

    *got_picture = false;
    avpkt.data = mpeg4_buf;
    avpkt.size = mpeg4_size;
    if (avpkt.size == 0)
        return -1;

    while (avpkt.size > 0) {
        int len;
        len = avcodec_decode_video2(avc_ctx, picture, &this_picture, &avpkt);
        if (len < 0) {
            fprintf(stderr, "Error while decoding frame\n");
            exit(1);
        }
        if(this_picture){
            assert( *got_picture == false ); //should be no multiple pictures in one packet
            *yuvbuf = picture->data[0];
            *linesize = picture->linesize[0];
            *got_picture = true;
        }
        avpkt.size -= len;
        avpkt.data += len;
    }

    avpkt.data = NULL;
    avpkt.size = 0;
    avcodec_decode_video2(avc_ctx, picture, &this_picture, &avpkt);
    if(this_picture){
        assert( *got_picture == false ); //should be no multiple pictures in one packet
        *yuvbuf = picture->data[0];
        *linesize = picture->linesize[0];
        *got_picture = true;
    }

    return 0;
}


//
// The new class routine
//

class MyRTPSession : public RTPSession
{
protected:
	void OnPollThreadStep();
	void ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack);
};

void MyRTPSession::OnPollThreadStep()
{
	BeginDataAccess();
		
	// check incoming packets
	if (GotoFirstSourceWithData())
	{
		do
		{
			RTPPacket *pack;
			RTPSourceData *srcdat;
			
			srcdat = GetCurrentSourceInfo();
			
			while ((pack = GetNextPacket()) != NULL)
			{
                if (g_rtp_rx_packet_cnt == 0) {
                    g_first_timestamp = pack->GetTimestamp();
                }
                g_rtp_rx_packet_cnt ++;
				ProcessRTPPacket(*srcdat,*pack);
				DeletePacket(pack);
			}
		} while (GotoNextSourceWithData());
	}
		
	EndDataAccess();
}

void MyRTPSession::ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack)
{    
    // You can inspect the packet and the source's info here
    int line_size;
    uint8_t* yuvbuf;
    int got_picture = 0;
    int payload_size = rtppack.GetPayloadLength();
    uint8_t payload_type = rtppack.GetPayloadType();

    if (!g_found_frame_start) {
        if (payload_type != PAYLOAD_TYPE_FRAME_START) {
            return;
        } else {
            g_found_frame_start = true;
        }
    }

    g_rtp_rx_size += payload_size;

    bzero(&video_rtp_data, sizeof(video_rtp_data_type));					

    memcpy(&video_rtp_data, rtppack.GetPayloadData(), payload_size);
    fwrite(video_rtp_data.frame_buf, 1, payload_size, g_rec_video);

    if (!arg_raw) {
        decode_video_packet(video_rtp_data.frame_buf, sizeof(video_rtp_data.frame_buf), &yuvbuf, &line_size, &got_picture);
    }

    if(got_picture){
        g_dec_frames++;
    }
    if (rtppack.HasMarker() == true) { 
        g_marker_count++; 
        if (arg_debug) {
            std::cout << "Got packet " << rtppack.GetExtendedSequenceNumber() << " from SSRC " << srcdat.GetSSRC() << std::endl;
            printf("rx-frames: %i, rx-packets: %d, rx-size:%d, timestamp: %d, frames decoded: %d\n", 
                    g_marker_count, g_rtp_rx_packet_cnt, g_rtp_rx_size,
                    rtppack.GetTimestamp() - g_first_timestamp, g_dec_frames);
        }

        if (arg_frames && g_marker_count >= arg_frames) {
            clean_up();
            exit(0);
        }
    }
}

int init_decode_video()
{
    /* must be called before using avcodec lib */
    avcodec_init();
    /* register all the codecs */
    avcodec_register_all();

    av_init_packet(&avpkt);
    memset(video_rtp_data.padding, 0, FF_INPUT_BUFFER_PADDING_SIZE);
    codec = avcodec_find_decoder(CODEC_ID_MPEG4);
    if (!codec) {
        fprintf(stderr, "codec not found\n");
        exit(1);
    }

    avc_ctx= avcodec_alloc_context();
    picture= avcodec_alloc_frame();

    if(codec->capabilities&CODEC_CAP_TRUNCATED)
        avc_ctx->flags|= CODEC_FLAG_TRUNCATED; /* we do not send complete frames */

    /* open it */
    if (avcodec_open(avc_ctx, codec) < 0) {
        fprintf(stderr, "could not open codec\n");
        exit(1);
    }
    return 0;
}

void close_decode_video()
{
    avcodec_close(avc_ctx);
    av_free(avc_ctx);
    av_free(picture);
}

#if 0
void close_rtp(RTPSession& sess)
{
    jrtplib::RTPTime::Wait(jrtplib::RTPTime(1,0));
    sess.BYEDestroy(RTPTime(10,0),0,0);
}
#endif

//
// The main routine
// 

int main(int argc, char** argv)
{
    signal(SIGINT, intHandler);

    /*
     * Command line parsing
     */
    using optparse::OptionParser;

    OptionParser parser = OptionParser() .description("Server program to receive Kinect video stream from Raspberry Pi");

    parser.add_option("-v", "--verbose").help("Print verbose output").action("store_true").set_default(arg_verbose);
    parser.add_option("-d", "--debug").help("Print debug output").action("store_true").set_default(arg_debug);
    parser.add_option("-n", "--frames").help("Max number of frames to receive before exit. Default: 0 = no limit").set_default(arg_frames).type("int");
    parser.add_option("-r", "--raw").help("Transmit raw video frames (no encoding)").action("store_true").set_default(arg_raw);

    optparse::Values options = parser.parse_args(argc, argv);

    arg_verbose = options.get("verbose");
    arg_debug   = options.get("debug");
    if (arg_debug) arg_verbose = 1;
    arg_frames  = atol(options["frames"].c_str());
    arg_raw         = options.get("raw");

    /* 
     * Init
     */
    g_rec_video = fopen(RX_VIDEO_DATA, "wb+");	
    if (!arg_raw ) {
        init_decode_video();
        if (arg_verbose) printf("Video library Initialized\n");
    }

	MyRTPSession sess;
	uint16_t portbase = KSERVER_PORT;
	int status;

	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessparams;
	
	// IMPORTANT: The local timestamp unit MUST be set, otherwise
	//            RTCP Sender Report info will be calculated wrong
	sessparams.SetOwnTimestampUnit(g_timestamp_unit);
	transparams.SetPortbase(portbase);
	status = sess.Create(sessparams,&transparams);	
	checkerror(status);
    if (arg_verbose) printf("RTP Initialized\n");
	
    /* 
     * Waiting for RTP
     */
    pause();
	
	sess.BYEDestroy(RTPTime(10,0),0,0);

    clean_up();

	return 0;
}

void clean_up()
{
    printf("Cleaning up...\n");

    if (!arg_raw) {
        char out_buf[4];
        out_buf[0] = 0x00;
        out_buf[1] = 0x00;
        out_buf[2] = 0x01;
        out_buf[3] = 0xb7;
        fwrite(out_buf, 1, 4, g_rec_video);

        close_decode_video();
    }
    fclose(g_rec_video);
}

#else

int main(void)
{
	std::cerr << "Thread support is required for this example" << std::endl;
	return 0;
}

#endif // RTP_SUPPORT_THREAD

