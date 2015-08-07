#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <signal.h>
#include <semaphore.h>
#include <time.h>

#include "OptionParser.h"
#include "common.h"

using namespace std;
using namespace jrtplib;

#define DPRINTF(...)  
//#define DPRINTF(...)  printf(__VA_ARGS__) 

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE);} while (0)

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

/**************************
 * Configurations
 * ************************/

#define KSEND_PORT   45456
#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN

#define TX_VIDEO_REC_FOR_DEBUG "video_out/tx_dbg.rec"

static int arg_debug          = 0;
static int arg_verbose        = 0;
static int arg_frames         = 0;
static string arg_server      = "127.0.0.1";
static string arg_type      = "gray";
static int arg_width          = 320;
static int arg_height         = 480;
static string arg_frames_file = "";
static bool arg_raw           = false;
static int arg_fps            = 5;

/**************************
 * Types
 * ************************/

typedef struct {
    int payload_type;
    int marker;
    int data_len;
    struct _video_rtp_data data;
} RTP_Packet;

/**************************
 * Global Variables
 * ************************/

typedef struct _video_rtp_data video_rtp_data_type;

int old_width;
int old_height;
unsigned int frame_num;

FILE* g_tx_video_rec_for_debug;

AVCodec* codec;
AVCodecContext* codec_context;
AVFrame* av_inpic;
AVFrame* av_frame; 
struct SwsContext* sws;    

uint8_t* raw_buf;
uint8_t* temp_buf;
uint8_t* out_buf;

uint32_t out_buf_size;
double video_tstamp;

video_rtp_data_type* video_rtp_data;
RTPUDPv4TransmissionParams transparams;  
RTPSessionParams sessparams; 

FILE* g_raw_frames_file;
int g_w_packet_cnt = 0;
int g_tx_packet_cnt = 0;
int g_rtp_tx_size;
volatile int g_frames;

timer_t frame_timer_id;
volatile int frame_timer_started = 0;
timer_t packet_timer_id;

pthread_attr_t g_frame_thread_attr;
pthread_attr_t g_packet_thread_attr;
RTPSession g_vid_sess;  
unsigned long long g_frame_interval_ns;
unsigned long long g_packet_interval_ns;
uint32_t g_timestamp_inc;
double g_timestamp_unit;

#define RTP_PACKET_BUFFER_SIZE  500
RTP_Packet  g_rtp_packet_buffer[RTP_PACKET_BUFFER_SIZE];
sem_t g_buffer_sem;
sem_t g_frame_tx_sem;
sem_t g_packet_tx_sem;
volatile int g_r_pos = 0;
volatile int g_w_pos = 0;
volatile int g_packet_thread_ended = 0;
int g_buffer_overrun = 0;
int g_buffer_underrun = 0;
int g_packet_send_overlapped = 0;
int g_frame_send_overlapped = 0;

#define BUFFER_EMPTY()  (g_r_pos == g_w_pos)
#define PACKET_COUNT()  (\
    (g_w_pos - g_r_pos >=0 ) ?   \
    (g_w_pos - g_r_pos) :       \
    (g_w_pos - g_r_pos + RTP_PACKET_BUFFER_SIZE)    \
    )
#define REMAINING_SIZE() (RTP_PACKET_BUFFER_SIZE -1 - PACKET_COUNT())
#define BUFFER_FULL() (REMAINING_SIZE() == 0)
#define INC(index)  do {index +=1; if (index == RTP_PACKET_BUFFER_SIZE) index = 0;} while(0)

/**************************
 * Declarations
 * ************************/

void close_all();
int get_next_frame(uint8_t* buf, int* read_size);
int transmit_raw_frame(uint8_t* raw_video_frame, const int size);
int encode_tx_video(RTPSession& vid_sess, uint8_t* rgb_data, int vw, int vh);

/**************************
 * Functions
 * ************************/

void print_status()
{
    printf("Frames: %d, Tx Packets: %d, Buffer Level: %d, Overrun: %d, Underrun: %d\n", g_frames, g_tx_packet_cnt, PACKET_COUNT(), g_buffer_overrun, g_buffer_underrun);
    fflush(stdout);
}

void checkerror(int rtperr)
{
    if (rtperr < 0)
    {
        std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
        exit(-1);
    }
}

void set_thread( pthread_attr_t *thread_attr)
{
    int s;

    s = pthread_attr_init(thread_attr);
    if (s != 0)
        handle_error_en(s, "pthread_attr_init");
    s = pthread_attr_setdetachstate(thread_attr, PTHREAD_CREATE_DETACHED);
    if (s != 0)
        handle_error_en(s, "pthread_attr_setdetachstate");
    s = pthread_attr_setinheritsched(thread_attr, PTHREAD_EXPLICIT_SCHED);
    if (s != 0)
        handle_error_en(s, "pthread_attr_setinheritsched");

#if 0
    int stack_size = 409600;
    void *sp;
    s = posix_memalign(&sp, sysconf(_SC_PAGESIZE), stack_size);
    if (s != 0)
        handle_error_en(s, "posix_memalign");

    printf("posix_memalign() allocated at %p\n", sp);

    s = pthread_attr_setstack(thread_attr, sp, stack_size);
    if (s != 0)
        handle_error_en(s, "pthread_attr_setstack");
#endif

}

void packet_send_thread(union sigval)
{
    DPRINTF("\n-------------------------\n");
    DPRINTF("packet_send_thread Test g_packet_tx_sem\n");
    if (sem_trywait(&g_packet_tx_sem) == -1 && errno == EAGAIN) {
        g_packet_send_overlapped = 1;
        return;
    }
    if (g_packet_thread_ended) {
        printf("packet thread already ended!!\n");
        sem_post(&g_packet_tx_sem);
        return;
    }

    DPRINTF("packet_send_thread Got g_packet_tx_sem\n");
    int status = 0;

    while (true) {
        DPRINTF("packet_send_thread Waiting for g_buffer_sem\n");

        // --------- g_buffer_sem ----------

        sem_wait(&g_buffer_sem);
        if (g_packet_thread_ended) {
            sem_post(&g_buffer_sem);
            sem_post(&g_packet_tx_sem);
            return;
        }
        DPRINTF("packet_send_thread Got g_buffer_sem\n");
        if (BUFFER_EMPTY()) {
            if (!frame_timer_started) {
                g_packet_thread_ended = 1;
                timer_delete(packet_timer_id);
                close_all();
                sem_post(&g_buffer_sem);
                sem_post(&g_packet_tx_sem);
                return;
            } else {
                DPRINTF("packet_send_thread: Buffer empty. Releasing g_buffer_sem\n");
                g_buffer_underrun = 1;
                sem_post(&g_buffer_sem);
                sem_post(&g_packet_tx_sem);
                DPRINTF("packet_send_thread :Released g_buffer_sem\n");
                usleep(20);
            }
        } else {
            break;
        }
    }

    RTP_Packet *pkt = &g_rtp_packet_buffer[g_r_pos];
    status = g_vid_sess.SendPacket(pkt->data.frame_buf, pkt->data_len, pkt->payload_type, pkt->marker, g_timestamp_inc);  
    DPRINTF("Sent packet\n");
    g_tx_packet_cnt ++;
    checkerror(status);         

    INC(g_r_pos);
    sem_post(&g_buffer_sem);
    sem_post(&g_packet_tx_sem);
}

#define FSEM(sem)  ({int tmp; sem_getvalue(&sem, &tmp); tmp;}) 
void frame_send_thread(union sigval)
{
    DPRINTF("\n=========================\n");
    DPRINTF("frame_send_thread: Test g_frame_tx_sem:%d \n", FSEM(g_frame_tx_sem));
    if (sem_trywait(&g_frame_tx_sem) == -1 && errno == EAGAIN) {
        g_frame_send_overlapped = 1;
        return;
    }
    // --------- g_frame_tx_sem  ----------
    if (!frame_timer_started) {
        sem_post(&g_frame_tx_sem);
        DPRINTF("frame_send_thread: 264 !frame_send_thread g_frame_tx_sem:%d\n", FSEM(g_frame_tx_sem));
        return;
    }

    DPRINTF("frame_send_thread: Got g_frame_tx_sem: %d\n", FSEM(g_frame_tx_sem));

    int size;
    int ret;

    if (arg_frames && g_frames >= arg_frames) {
        DPRINTF("arg_frames:%d, g_frames:%d\n", arg_frames, g_frames);
        goto frame_send_thread_exit;
    }

    ret = get_next_frame(raw_buf, &size);

    if (ret <=0) { // eof or error
        if (arg_debug) printf("get_next_frame returns %d\n", ret);
        goto frame_send_thread_exit;
    }

    if (arg_raw) {
        transmit_raw_frame(raw_buf, size);
    } else {
        // FIXME: rgb with thread sending
        if (arg_type == "rgb") {
            encode_tx_video(g_vid_sess, raw_buf, arg_width, arg_height);
        } else { // gray
            printf("Not implemented: encoding + gray.\n");
            exit(0);
        }
    }
    g_frames ++;
    DPRINTF("g_frames:%d, frame_timer_started:%d\n", g_frames, frame_timer_started);

    DPRINTF("frame_send_thread 299: Releasing g_frame_tx_sem:%d\n", FSEM(g_frame_tx_sem));
    sem_post(&g_frame_tx_sem);
    DPRINTF("frame_send_thread 301: Released g_frame_tx_sem:%d\n", FSEM(g_frame_tx_sem));

    return;

frame_send_thread_exit:
    timer_delete(frame_timer_id);
    frame_timer_started = 0;
    sem_post(&g_frame_tx_sem);
    DPRINTF("frame_send_thread: 309 Released g_frame_tx_sem:%d\n", FSEM(g_frame_tx_sem));
    DPRINTF("frame_send_thread_exit: ");
    print_status();
}

sigset_t mask;
void set_up_frame_timer(unsigned long long nsecs)
{
   struct sigevent sev;
   struct itimerspec its;
   long long freq_nanosecs;

   /* Create the timer */

   sev.sigev_notify = SIGEV_THREAD;
   sev.sigev_notify_function = frame_send_thread;
   sev.sigev_notify_attributes = &g_frame_thread_attr;
   sev.sigev_signo = SIG;
   sev.sigev_value.sival_ptr = &frame_timer_id;
   if (timer_create(CLOCKID, &sev, &frame_timer_id) == -1)
       errExit("timer_create");

   printf("timer ID is 0x%lx\n", (long) frame_timer_id);

   /* Start the timer */

   freq_nanosecs = nsecs;
   its.it_value.tv_sec = freq_nanosecs / 1000000000;
   its.it_value.tv_nsec = freq_nanosecs % 1000000000;
   its.it_interval.tv_sec = its.it_value.tv_sec;
   its.it_interval.tv_nsec = its.it_value.tv_nsec;

   if (timer_settime(frame_timer_id, 0, &its, NULL) == -1)
        errExit("timer_settime");

   frame_timer_started = 1;
}

void set_up_packet_timer(unsigned long long nsecs)
{
   struct sigevent sev;
   struct itimerspec its;
   long long freq_nanosecs;

   /* Create the timer */

   sev.sigev_notify = SIGEV_THREAD;
   sev.sigev_notify_function = packet_send_thread;
   sev.sigev_notify_attributes = &g_packet_thread_attr;
   sev.sigev_signo = SIG;
   sev.sigev_value.sival_ptr = &packet_timer_id;
   if (timer_create(CLOCKID, &sev, &packet_timer_id) == -1)
       errExit("timer_create");

   printf("timer ID is 0x%lx\n", (long) packet_timer_id);

   /* Start the timer */

   freq_nanosecs = nsecs;
   its.it_value.tv_sec = freq_nanosecs / 1000000000;
   its.it_value.tv_nsec = freq_nanosecs % 1000000000;
   its.it_interval.tv_sec = its.it_value.tv_sec;
   its.it_interval.tv_nsec = its.it_value.tv_nsec;

   if (timer_settime(packet_timer_id, 0, &its, NULL) == -1)
        errExit("timer_settime");
}

//////////////////////////////////////////////////

void init_av(int vw, int vh) 
{
    avcodec_init();
    avcodec_register_all();

    codec = avcodec_find_encoder(CODEC_ID_MPEG4);
    codec_context = avcodec_alloc_context();
    codec_context->bit_rate = 2048000;
    codec_context->width = vw;
    codec_context->height = vh;
    codec_context->time_base = (AVRational){1, 25};
    codec_context->gop_size = 2;
    codec_context->pix_fmt = PIX_FMT_YUV420P;

    int status = (avcodec_open(codec_context, codec));

    if (status < 0) 
    {
        std::cout << "Cant Open Codec [" << status << "]" << std::endl;
        exit(1); 
    }

    sws = sws_getContext(vw, vh, PIX_FMT_RGB24, vw, vh, PIX_FMT_YUV420P, SWS_BILINEAR, NULL, NULL, NULL);

    av_frame = avcodec_alloc_frame();
    av_inpic = avcodec_alloc_frame();
}

int get_next_frame(uint8_t* buf, int* read_size)
{
    if(feof(g_raw_frames_file)) 
        return -1;

    int size;
    if (arg_type == "rgb") {
        size = arg_width * arg_height * 3;
    } else { // gray
        size = arg_width * arg_height;
    }

    size = fread(buf, 1, size, g_raw_frames_file);
    if (size == 0 && !feof(g_raw_frames_file)) diep("fread error");

    *read_size = size;

    return size;
}

int transmit_raw_frame(uint8_t* raw_video_frame, const int size)
{
    int offset = 0;
    int size_left = size;
    bool first_packet = true;
    RTP_Packet packet;

    while(size_left>0)
    { 
        if (first_packet) {
            packet.payload_type = PAYLOAD_TYPE_FRAME_START;
            first_packet = false;
        } else {
            packet.payload_type = PAYLOAD_TYPE_NORMAL;
        }

        int piece_size = size_left > MAX_VDATA_SIZE ? MAX_VDATA_SIZE : size_left;
        packet.marker = size_left > piece_size ? false : true;  
        packet.data_len = piece_size;

        //XXX: optimize memcpy
        memcpy(packet.data.frame_buf, &raw_video_frame[offset], piece_size);
        
        while (true) {
            DPRINTF("\n+++++++++++++\n");
            DPRINTF("transmit_raw_frame waiting for g_buffer_sem\n");
            sem_wait(&g_buffer_sem);
            DPRINTF("transmit_raw_frame got g_buffer_sem\n");
            if (BUFFER_FULL()) {
                g_buffer_overrun = 1;
                DPRINTF("Buffer full: transmit_raw_frame releasing g_buffer_sem\n");
                sem_post(&g_buffer_sem);
                DPRINTF("Buffer full: transmit_raw_frame released g_buffer_sem\n");
                usleep(10*1000);
            } else {
                break;
            }
        }
        memcpy(&g_rtp_packet_buffer[g_w_pos], &packet, sizeof(packet));
        INC(g_w_pos);
        DPRINTF("transmit_raw_frame releasing g_buffer_sem\n");
        sem_post(&g_buffer_sem);
        DPRINTF("transmit_raw_frame released g_buffer_sem\n");

        g_rtp_tx_size += piece_size;
        g_w_packet_cnt ++;

        size_left = size_left - piece_size;
        offset = offset + piece_size;
    }

    if (arg_debug) {
        fwrite(raw_video_frame, 1, size, g_tx_video_rec_for_debug); 
    }
    return 0;
}

int encode_tx_video(RTPSession& vid_sess, uint8_t* rgb_data, int vw, int vh)
{
    uint32_t out_size;

    avpicture_fill((AVPicture*) av_inpic, rgb_data, PIX_FMT_RGB24, vw, vh);
    avpicture_fill((AVPicture*) av_frame, temp_buf, PIX_FMT_YUV420P, vw, vh);

    av_inpic->data[0] = av_inpic->data[0] + (vh *3 * vw);
    av_inpic->linesize[0] = -av_inpic->linesize[0];

    sws_scale(sws, av_inpic->data, av_inpic->linesize, 0, vw, av_frame->data, av_frame->linesize);

    out_size = avcodec_encode_video(codec_context, out_buf, out_buf_size, av_frame);

    int offset = 0;
    int size_left = out_size;

    while(size_left>0)
    { 
        int piece_size = size_left > MAX_VDATA_SIZE ? MAX_VDATA_SIZE : size_left;
        bool markFlag = size_left > piece_size ? false : true;  
        memcpy(video_rtp_data->frame_buf, &out_buf[offset], piece_size);
        //XXX: payload type to be consistent with that of raw frame type
        int status = vid_sess.SendPacket(video_rtp_data, 8 + piece_size, PAYLOAD_TYPE_NORMAL, markFlag, 10);   
        checkerror(status);         
        g_w_packet_cnt ++;

        size_left = size_left - piece_size;
        offset = offset + piece_size;
    }

    if (arg_debug) {
        fwrite(out_buf, 1, out_size, g_tx_video_rec_for_debug); 
    }
    return 0;
}

int init_rtp(RTPSession& sess, const char* server)
{
    uint16_t portbase = KSEND_PORT;
    uint16_t destport = KSERVER_PORT;

    int status = 0;

    uint32_t destip = inet_addr(server);

    if (destip == INADDR_NONE)
    {
        std::cout << "Bad IP address specified" << std::endl;
    }

    destip = ntohl(destip);

    sessparams.SetOwnTimestampUnit(g_timestamp_unit);          
    sessparams.SetAcceptOwnPackets(true);
    sessparams.SetMaximumPacketSize(1500);

    sess.SetDefaultPayloadType(PAYLOAD_TYPE_NORMAL);  
    sess.SetDefaultMark(false);  

    sess.SetDefaultTimestampIncrement(1); 

    transparams.SetPortbase(portbase);

    status = sess.Create(sessparams, &transparams);    
    checkerror(status);

    RTPIPv4Address addr(destip,destport);
    sess.AddDestination(addr);

    video_rtp_data = new video_rtp_data_type;
    return 0;
}

void close_all()
{
    printf("\nCleaning up before exiting...\n");

    if (!arg_raw) {
        // releasing libav resources
        avcodec_close(codec_context);
        av_free(codec_context);
        av_free(av_frame);
        av_free(av_inpic);
        sws_freeContext(sws);

        // closing up the video
        if (arg_debug) {
            out_buf[0] = 0x00;
            out_buf[1] = 0x00;
            out_buf[2] = 0x01;
            out_buf[3] = 0xb7;
            fwrite(out_buf, 1, 4, g_tx_video_rec_for_debug);
            fclose(g_tx_video_rec_for_debug);
        }
    }

    delete raw_buf;
    if (arg_type == "rgb") {
        delete temp_buf;
        delete out_buf;
    }

    fclose(g_raw_frames_file);

    printf("\n");

    if (arg_verbose) {
        print_status();
    }
}

int main(int argc, char** argv)
{
    /*
     * Command line parsing
     */
    using optparse::OptionParser;

    OptionParser parser = OptionParser() .description("Server program to receive Kinect video stream from Raspberry Pi");

    parser.add_option("-v", "--verbose").help("Print verbose output").action("store_true").set_default(arg_verbose);
    parser.add_option("-d", "--debug").help("Print debug output").action("store_true").set_default(arg_debug);
    parser.add_option("-n", "--frames").help("Max number of frames to send before exit (Default: 0, no limit)").set_default(arg_frames).type("int");
    parser.add_option("-s", "--server").help("IP of the server").set_default(arg_server);
    parser.add_option("-t", "--type").help("Input video type (rgb or gray). gray will be transferred uncommpressed").set_default(arg_type);
    parser.add_option("-W", "--width").help("Input video width").set_default(arg_width).type("int");
    parser.add_option("-H", "--height").help("Input video height").set_default(arg_height).type("int");
    parser.add_option("-i", "--frames_file").help("Use raw video frames file as input").set_default(arg_frames_file);
    parser.add_option("-r", "--raw").help("Transmit raw video frames (no encoding)").action("store_true").set_default(arg_raw);
    parser.add_option("-f", "--fps").help("Input video width").set_default(arg_fps).type("int");

    optparse::Values options = parser.parse_args(argc, argv);

    arg_verbose     = options.get("verbose");
    arg_debug       = options.get("debug");
    arg_frames      = atol(options["frames"].c_str());
    arg_server      = options["server"];
    arg_frames_file = options["frames_file"];
    arg_type      = options["type"];
    arg_width       = atol(options["width"].c_str());
    arg_height      = atol(options["height"].c_str());
    arg_raw         = options.get("raw");
    arg_fps         = atol(options["fps"].c_str());

    if (arg_frames_file == "") {
        printf("\033[0;31;40mMust specify input frames file using '-i'\n\033[0m");
        dieh();
    }

    if (arg_type != string("rgb") && arg_type != string("gray")) {
        printf("\033[0;31;40mIncorrect input video type: %s\n\033[0m", arg_type.c_str());
        dieh();
    }

    arg_verbose = arg_verbose || arg_debug;

    if (arg_verbose) {
        printf("frames:         %d\n", arg_frames); 
        printf("server:         %s\n", arg_server.c_str()); 
        printf("type:           %s\n", arg_type.c_str()); 
        printf("width:          %d\n", arg_width); 
        printf("height:         %d\n", arg_height); 
        printf("frames_file:    %s\n", arg_frames_file.c_str()); 
        printf("debug:          %d\n", arg_debug); 
        printf("Sending fps:    %d\n", arg_fps); 
    }

    if (arg_debug) {
        printf("tx video rec for debug:       %s\n", TX_VIDEO_REC_FOR_DEBUG);
    }

    printf("\n");
    /* 
     * Init
     */
    g_raw_frames_file = fopen(arg_frames_file.c_str(), "r");

    if (g_raw_frames_file == NULL) diep("Open image failed");

    if (!arg_raw) {
        init_av(arg_width, arg_height);
        if (arg_verbose) printf("Video library  initialized.\n");
    }

    out_buf_size = 1024000;

    int frame_size;

    if (arg_type == "rgb") {
        raw_buf = new uint8_t[avpicture_get_size(PIX_FMT_RGB24, arg_width, arg_height)];
        temp_buf = new uint8_t[avpicture_get_size(PIX_FMT_YUV420P, arg_width, arg_height)];
        out_buf = new uint8_t[out_buf_size];
        frame_size = arg_width * arg_height * 3;
    } else {
        frame_size = arg_width * arg_height;
        raw_buf = new uint8_t[frame_size];
    }
    if (arg_verbose) printf("Buffers allocated.\n");

    init_rtp(g_vid_sess, arg_server.c_str());
    if (arg_verbose) printf("RTP library initialized.\n");

    if (arg_debug) {
        g_tx_video_rec_for_debug = fopen(TX_VIDEO_REC_FOR_DEBUG, "wb+");
    }

    printf("\n");

    if (sem_init(&g_buffer_sem, 0, 1) == -1) {
        handle_error("sem_init");
    }
    if (sem_init(&g_packet_tx_sem, 0, 1) == -1) {
        handle_error("sem_init");
    }
    if (sem_init(&g_frame_tx_sem, 0, 1) == -1) {
        handle_error("sem_init");
    }

    g_frame_interval_ns = 1000 *1000 *1000 / arg_fps;
    g_packet_interval_ns = g_frame_interval_ns * 1400 / frame_size *10/12;  // *10/12: head room
    g_timestamp_unit = 1.0 / arg_fps *1400 / frame_size;
    g_timestamp_inc = 1;

    set_thread(&g_frame_thread_attr);
    set_thread(&g_packet_thread_attr);

    // frame timer must be started before packet timer
    set_up_frame_timer(g_frame_interval_ns);
    sleep(1); // wait for the buffer to fill
    set_up_packet_timer(g_packet_interval_ns);

    // Sleep forever
    while (!g_packet_thread_ended) {
        print_status();
        usleep(1000*1000);
    }

    return 0;
}
