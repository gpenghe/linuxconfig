#include "config.h"

#include "util.h"
#include "BgCodebook.h"
#include "Analytics.h"
#include "drawing.h"

#ifdef __GNUC__
#include <unistd.h>
#endif

#include <getopt.h>

#include "comm.h"

// #define CODEBOOK_EX
#define IMAGE2WORLD_OPT
// #define LEARN_BACKGROUND

/****************************************************
 * ICS Parameters
 ****************************************************/
#define Width 640
#define Height 480
#define halfw Width/2  //only process half size of frame
#define halfh Height/2

// some boundary part of depth frame is not valid, we can skip them in processing
//#define min_row 5
//#define max_row halfh-20
//#define min_column 24
//#define max_column halfw-10
// Changed per Hui's e-mail Tue 2/10/2015 1:11 PM -AF
#define min_row 16
#define max_row halfh-11
#define min_column 20
#define max_column halfw-20

// 2D visualization parameters
#define psize_m 0.02 // length of one pixel in "m"

// changed by Hui on 02/10,2015, based on Alan's new measurements (02/09,2015) about the dimension of Berlin lab 
// changed 2015-02-11 for smaller lobby_h (x axis) so other people sitting in the lab are not 'waiting' -AF
// lobby_h was 5.1 will be 4.5
#define lobby_w 5.04 // length of lobby width in "m"
#define lobby_h 4.50 // length of lobby height in "m"
#define origin_x 2.18 // x position of origin (kinect position) in lobby figure
#define origin_y 5.1 // y position of origin (kinect position) in lobby figure

#define show_w lobby_w/psize_m
#define show_h lobby_h/psize_m

// background learning variables/parameters (currently background learning is just done once at begining)
#define f_bg_start 300 // the frame no. when background learning starts (depth frames at the begining seem very nosiy, we just skip them)
#define f_bg_end 600 // the frame no. when background learning stops
#define ini_box_boundary 10 // initial box boundary to learn a box
#define min_box_element 5 // a learned box will be removed if the number of supported point smaller than this threshold

// background subtration variables/parameters
#define down_offset 100 // define lowest bounday (a pixel belongs to a background box if it's within the box's lowest and highest bounday)
#define up_offset 100 // define highest bounday

// geometry constraint to filter false foreground, more constraint can be applied for a specific case (like min and max depth)
// Tuning 2015-02-10 14:02 -AF
// max_X was 7.0, now 5.0
#define max_X 5.0 // the maximum world X coordinate of one foreground point
#define min_X 0.0 // the minimum world X coordinate of one foreground point (depends on specific lobby layout, maybe there's no such constraint can be used)
#define max_Z 2.0 // the maximum world Z coordinate of one foreground point (acoording to normal people height)
#define min_Z 0.1 // the minimum world Z coordinate

// foreground segmentation variables/parameters
#define depth_discontinuity_thrd 150 // threshold for depth discontinuity to segment foreground
#define size_thrd 800 // minimum area (pixel number) for each segmented foreground blob

// tracking variables/parameters
// Tuning 2015-02-10 18:57 -AF
// n_local_p was 4, now 8
#define track_cf_thrd 2                 // confidence (number of consecutive associated detections) to be a valid tracker
#define max_tracking_dis 1.0            // in m,  maximum allowed x-y plane distance for associated detection and predicted blobs
#define n_local_p 8                     // number of locations to compute moving direction and speed

// intention recognition parameters
// Tuning 2015-02-11 09:12 -AF
// speed_smoothness_weight was 0.35, now 0.5
// walking_speed_thrd was 0.6, now 0.4
// X_close_door was 1.2, now .7
// minY_door_position was -1.0, now -1.5
// maxY_door_position was 1.0, now +1.5
#define speed_smoothness_weight 0.5     // larger value will consider more on previous speed value
#define walking_speed_thrd 0.4          // people in waiting status with high probability if moving speed is smaller than this value
#define waiting_speed_thrd 0.15         // people's status will be set to waiting if speed smaller than this value
#define minY_door_position -1.5         // min elevator door world Y coordinates, decide whether people approaching elevator
#define maxY_door_position 1.5          // max elevator door world Y coordinates
#define X_close_door 0.7                // people are near door location if people's X world coordinates smaller this value

/****************************************************
 * Types and Variables
 ****************************************************/

CvPoint2D32f local_p[n_local_p]; //to store the most recent "n_local_p" locations
double dt_array[n_local_p]; // to store time between two frames to compute speed

// origin position in 2D plane figure
int or_x;
int or_y;

typedef struct _ICSContext {

#ifdef CFG_OPENNI_V1
    xn::Context openniV1Context;
#ifndef CFG_OPT_NO_RGB
    xn::ImageMetaData imageMD;
    xn::ImageGenerator imageGenerator;
#endif
    xn::DepthGenerator depthGenerator;
    xn::DepthMetaData depthMD;
#endif

}ICSContext;

#ifdef CODEBOOK_EX
typedef struct {
    int min;
    int max;
} Limit;

typedef struct {
    int count;
    Limit limits[16];
} CodeBookEx;

CodeBookEx codebooks_ex[320*240];
#endif

bool com_send = false;
bool com_send2 = false;

typedef unsigned short int int16;

bool learn_bg = true; // whether to learn background

// whether lobby is empty or not
int g_lobby_status = 0;

// passenger status vector
int g_passenger_status_vector[4];
int g_last_passenger_status_vector[4];
bool g_last_passenger_status_vector_saved = false;

// elevator control signal
int g_enter_hallcall = 0;
int g_last_hallcall_status = 0;
int nf2show = 0;
int nf2show_2 = 0;

int g_last_wa = 0;
int g_leaving = 0;

int close_door_time = -100;
int diftime = 100;

bool g_show_detail_info = false;

bool g_file_test = false;

IplImage* Tshow;
IplImage* Tshow_copy; // added by Hui on 02/10,2015

// Transform Matrix from Hui's e-mail Wed 2/11/2015 5:58 AM. This fixes (x,y) transposition in data acquisition. Modified to include FLOAT_TO_FP for later use.
// This matrix is for the kinect angled down more than initially. -AF
FIXED_POINT transf_matrix[12] = {
    FLOAT_TO_FP(0.113680157513151f/1000),
    FLOAT_TO_FP(0.802116581341557f/1000),
    FLOAT_TO_FP(0.613843684866222f/1000),
    FLOAT_TO_FP(108.808157426950f/1000),
    FLOAT_TO_FP(-1.09963101884009f/1000),
    FLOAT_TO_FP(-0.0251517127905826f/1000),
    FLOAT_TO_FP(-0.0203876890115233f/1000),
    FLOAT_TO_FP(-1.05388491630521f/1000),
    FLOAT_TO_FP(-0.0632876194123213f/1000),
    FLOAT_TO_FP(0.771183716904325f/1000),
    FLOAT_TO_FP(-0.734225200800951f/1000),
    FLOAT_TO_FP(2433.09839103153f/1000)
};

// Previous values for Berlin demo (kinect not angled down so much) -AF 
//FIXED_POINT transf_matrix[12] = {
//    FLOAT_TO_FP(-0.00837164307101012f/1000),
//    FLOAT_TO_FP(0.542389871021722f/1000),
//    FLOAT_TO_FP(0.831936175839380f/1000),
//    FLOAT_TO_FP(139.919377175003f/1000),
//    FLOAT_TO_FP(-1.09505468434722f/1000),
//    FLOAT_TO_FP(-0.0473322549028341f/1000),
//    FLOAT_TO_FP(0.00365587568631198f/1000),
//    FLOAT_TO_FP(-30.8900873660321f/1000),
//    FLOAT_TO_FP(0.00609964436605688f/1000),
//    FLOAT_TO_FP(0.967137324119811f/1000),
//    FLOAT_TO_FP(-0.602185719989463f/1000),
//    FLOAT_TO_FP(2547.54485475742f/1000)
//};

ofstream comd;

#ifdef USE_OZONE
HANDLE hPort;
int doorState;
int elevatorState;
int currentFloor;
#endif

// Ozone Commands
#define HALL_CALL             0x01
#define CAR_CALL              0x02
#define CANCEL_HALL_CALL      0x03
#define CANCEL_CAR_CALL       0x04
#define DOOR_CLOSE                  0x05
#define DOOR_STAY_OPEN        0x06
// Ozone Request
#define REQUEST_STATUS        0x07 // get change of event

CvScalar colors[4] =
{
    cvScalar(0,225,255),    // waiting
    cvScalar(0,255,0),      // approaching
    cvScalar(0,0,0),        // leaving
    cvScalar(0,0,0)         // passing by
};

CvFont font;

int psize = 2; // one pixel for 2 cm (0.02 m)

FILE * fp_log_file = NULL;

#ifdef CFG_SEND_ICS_SERVER
CommHandle g_comm = NULL;
#endif

CBgCodebook* codebooks;

int cmd_verbose          = 0;
const char* cmd_log_file = "ics.log";
char* cmd_bg_file        = NULL;
char* cmd_depth_file     = NULL;

#ifndef CFG_OPT_NO_RGB
IplImage* imgRGB8u;
IplImage* rgb2;
IplImage* bgr2;
#ifndef CFG_OPENNI
FILE* fp_rgb_in;
#endif
#endif
IplImage*  imgDepth16u;
IplImage* depth2;
IplImage* pFgMask;
IplImage* depthShow;
#ifndef CFG_OPENNI
int frame_count;
FILE* fp_depth_in;
#endif

#ifdef PLATFORM_WINDOWS     // Added for windows timing code. Used in Berlin demo. -AF
LARGE_INTEGER litmp; 
LONGLONG start_time, end_time; 
double dfFreq;
#endif


/**************************************************
 *                   FUNCTIONS                    *
 **************************************************/

static void handle_ctrl_c(int signo)
{
    assert(signo == SIGINT);
    fprintf(stderr, "\nCtrl-C caught. Exiting...\n");
    if (fp_log_file) {
        fclose(fp_log_file);
        fp_log_file = NULL;
    }

    fflush(stdout);
    exit(0);
}

void get_passenger_status(TrackSet* trackset)
{
    for(int i=0; i<4; i++)
        g_passenger_status_vector[i] = 0;

    TrackSet::iterator it_track = trackset->begin();
    for(; it_track!=trackset->end(); it_track++)
    {
        if( (*it_track).cf>=track_cf_thrd )
        {
            int st = (*it_track).smoothed_status;

            if( st>=0 )
                g_passenger_status_vector[st] = g_passenger_status_vector[st]+1;

        }

    }

#ifdef CFG_SEND_ICS_SERVER
    if (g_last_passenger_status_vector_saved) {
        for (int i=0; i<4; i++) {
            if (g_last_passenger_status_vector[i] != g_passenger_status_vector[i]) {
                comm_send_passenger_intention(g_comm, i, g_passenger_status_vector[i]);
            }
        }
    }
    memcpy(g_last_passenger_status_vector, g_passenger_status_vector, sizeof(g_passenger_status_vector));
    g_last_passenger_status_vector_saved = true;
#endif

    if (g_file_test) {
        int wa = g_passenger_status_vector[0];        // Waiting
        int ap = g_passenger_status_vector[1];        // Approaching
        int lea = g_passenger_status_vector[2];       // Leaving
        int pas = g_passenger_status_vector[3];       // Passing

        // response function
        if( g_lobby_status == 1 )         // Lobby not empty
        {
            //  if passenger approaching or waiting, then enter hall call
            if( ap > 0 || wa > 0)
            {
                g_enter_hallcall = 1;
            }

            // if passenger leaving or passing by, and no passenger approaching or waiting,
            // then:
            //  1. not entering hall call
            //  2. set g_leaving
            if( (lea > 0 || pas > 0) && ap==0 && wa==0 )
            {
                g_enter_hallcall = 0;

                /*if( g_last_hallcall_status == 1)
                  g_leaving = 1;*/
                if( g_last_wa > 0)
                    g_leaving = 1;

            }

        }
        else
        {
            g_enter_hallcall = 0;
        }

        g_last_wa = wa;
    }
}

void show_button(int fn)
{
    int bd = 10; // border? 
    // show control window
    int yoffset = bd+lobby_h;
    cvRectangle(Tshow, cvPoint(bd, yoffset+10), cvPoint(bd+100, yoffset+10+40), cvScalar(255,0,0), 1);

    // show and send control signal
    if( g_enter_hallcall==1 )
    {
        showDoorOpen(Tshow, bd+70,yoffset+30);

        if( g_last_hallcall_status == 0)
        {
            diftime = fn - close_door_time;
            nf2show = 8;

            if( diftime<20 )
            {
                fprintf(fp_log_file, "Hold Door Open !\n");
                comd<<fn<<": "<<"Hold Door Open !"<<endl; // save command to log file
#ifdef USE_OZONE
                OzoneTransmit(hPort, DOOR_STAY_OPEN, 0x00, 0x00);
#endif
                fprintf(fp_log_file, "%d\n", DOOR_STAY_OPEN);
            }
            else
            {
                fprintf(fp_log_file, "Enter Hall Call !\n");
                comd<<fn<<": "<<"Enter Hall Call !"<<endl; // save command to log file

#ifdef USE_OZONE
                OzoneTransmit(hPort, HALL_CALL, 0x01, 0x01);
#endif
                fprintf(fp_log_file, "%d\n", HALL_CALL);
            }

        }

        if( diftime<20 )
        {
            showButtonOff(Tshow, bd+20, yoffset+30);

            if( nf2show>0 && nf2show%2==1 )
                notShowDoor(Tshow, bd+70,yoffset+30);

        }
        else
        {
            showButtonOn(Tshow, bd+20, yoffset+30);

            if( nf2show>0 && nf2show%2==1 )
                notShowButton(Tshow, bd+20, yoffset+30);
        }

        if(nf2show>0)
            nf2show--;

    }
    else
    {
        showButtonOff(Tshow, bd+20, yoffset+30);
        showDoorClose(Tshow, bd+70,yoffset+30);

        if( g_leaving == 1 )
        {
            nf2show_2 = 8;
            g_leaving = 0;

            fprintf(fp_log_file, "Cancel Hall Call !\n");
            comd<<fn<<": "<<"Cancel Hall Call !"<<endl; // save command to log file
        }

        if( nf2show_2>0 )
        {
            if( nf2show_2%2==0 )
                showButtonCancel(Tshow, bd+20, yoffset+30);
            else
                notShowButton(Tshow, bd+20, yoffset+30);

            nf2show_2--;
        }

        if( g_last_hallcall_status == 1)
        {
            close_door_time = fn;

            fprintf(fp_log_file, "Close Door !\n");
            comd<<fn<<": "<<"Close Door !"<<endl; // save command to log file
#ifdef USE_OZONE
            OzoneTransmit(hPort, DOOR_CLOSE, 0x00, 0x00);
#endif
            fprintf(fp_log_file, "%d\n", DOOR_CLOSE);
        }

    }

    g_last_hallcall_status = g_enter_hallcall;

}

void get_depth(ICSContext* ctx, IplImage* d)
{
    double tt = -1;
    // read data
#ifdef CFG_OPENNI_V2
    int changedStreamDummy;
    Status rc;
    VideoStream* pStream = &depth;
#define SAMPLE_READ_WAIT_TIMEOUT 5000 //5000ms
    rc = OpenNI::waitForAnyStream(&pStream, 1, &changedStreamDummy, SAMPLE_READ_WAIT_TIMEOUT);
    if (rc != STATUS_OK) {
        printf("Wait failed! (timeout is %d ms)\n%s\n", SAMPLE_READ_WAIT_TIMEOUT, OpenNI::getExtendedError());
        continue;
    }

    rc = depth.readFrame(&frame);
    if (rc != STATUS_OK) {
        printf("Read failed!\n%s\n", OpenNI::getExtendedError());
        continue;
    }

    if (frame.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_1_MM && frame.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_100_UM) {
        printf("Unexpected frame format\n");
        continue;
    }

    DepthPixel* pDepth = (DepthPixel*)frame.getData();

#endif

#ifdef CFG_OPENNI_V1
#ifndef CFG_OPT_NO_RGB
    ctx->imageGenerator.GetMetaData(ctx->imageMD);
#endif
    ctx->depthGenerator.GetMetaData(ctx->depthMD);
#endif

#ifdef CFG_OPENNI

#ifdef PLATFORM_WINDOWS
    QueryPerformanceCounter(&litmp);
    end_time = litmp.QuadPart;

    tt = ((double)(end_time - start_time))/dfFreq;

    start_time = end_time;
#else	// Linux or RPi
    //FIXME: Linux timing method
#endif
#else // faked time interval for file input
    tt = 0.066667; // fixed 15 fps
#endif

    // Copy RGB
#ifndef CFG_OPT_NO_RGB
#ifdef CFG_OPENNI_V1
    memcpy(imgRGB8u->imageData, ctx->imageMD.Data(), Width*Height*3);
#ifdef RECORD_RGB_VIDEO
    fwrite(ctx->imageMD.Data(), Width*Height*3, 1, fp_rgb_out);
#endif
#elif defined CFG_OPENNI_V2
#error "Currently OpenNI2 implementation doesn't support RGB video!"
#else
    fread(imgRGB8u->imageData, Width*Height*3, 1, fp_rgb_in);
#endif

    // removed flips to correct Berlin kinect image -AF
    // cvFlip(imgRGB8u, imgRGB8u);
    // cvFlip(imgRGB8u, imgRGB8u, 1);
    cvResize(imgRGB8u, bgr2, CV_INTER_NN);
    cvCvtColor(bgr2, rgb2, CV_RGB2BGR);  // rgb2 is the one we need
#endif

    // Copy Depth data
#ifdef CFG_OPENNI_V1
    memcpy(imgDepth16u->imageData, ctx->depthMD.Data(), Width*Height*2);
#ifdef RECORD_DEPTH_VIDEO
    fwrite(imgDepth16u->imageData, Width*Height*2, 1, fp_depth_out);
#endif
#elif defined CFG_OPENNI_V2
    memcpy(imgDepth16u->imageData, pDepth, Width*Height*2);
#else
    fread(imgDepth16u->imageData, Width*Height*2, 1, fp_depth_in);
#endif

    // removed flips to correct Berlin kinect image -AF
    // cvFlip(imgDepth16u, imgDepth16u);
    // cvFlip(imgDepth16u, imgDepth16u, 1);
    cvResize(imgDepth16u, d, CV_INTER_NN);
    cvSmooth(d, d, CV_MEDIAN);
    cvConvertScale(d, depthShow, 255/8000.0, 0);

    for(int i=1; i<n_local_p; i++)
    {
        dt_array[i-1] = dt_array[i];
    }
    dt_array[n_local_p-1] = tt;
}

// added by Hui on 02/10, 2015
void display_results(TrackSet* trackset)
{
    char text[100];

    cvCopy(Tshow_copy, Tshow);

    TrackSet::iterator it_track = trackset->begin();
    for(; it_track!=trackset->end(); it_track++)
    {
        if( (*it_track).cf>=track_cf_thrd && (*it_track).rects3D.size()>=(n_local_p) )
        {
            int n = 0;

            // get most recent x-y locations
            RECTS3D::reverse_iterator rit = (*it_track).rects3D.rbegin();				
            for(; rit!=(*it_track).rects3D.rend(); rit++)
            {
                local_p[n].x = float((*rit).wx);
                local_p[n].y = float((*rit).wy);

                n++;
                if(n==n_local_p)
                    break;
            }

            // average movement between two frames
            double dis = 0;
            for(int i=1; i<n_local_p; i++)
            {
                dis = dis + sqrt( (local_p[i].x-local_p[i-1].x)*(local_p[i].x-local_p[i-1].x) + (local_p[i].y-local_p[i-1].y)*(local_p[i].y-local_p[i-1].y) );
            }
            dis = dis/(n_local_p-1);

            // average time interval between two frames
            double dtsum = 0;
            for(int i=0; i<n_local_p; i++)
            {
                dtsum = dtsum + dt_array[i];
            }
            double dt = dtsum/n_local_p;

            // get current speed
            double speed = dis/dt;

            // speed smoothness
            if( (*it_track).last_speed >=0 )
                speed = (*it_track).last_speed*speed_smoothness_weight + speed*(1-speed_smoothness_weight);

            double y0, dx, dy;// dx0, dx1, dx2;

            y0 = 0.0;       // initialize -AF
            dy = 0.0;       // initialize -AF

            // fitted line 
            float line[4];
            dx = local_p[0].x - local_p[n-1].x;
            /*dx0 = local_p[0].x - local_p[1].x;
            dx1 = local_p[1].x - local_p[2].x;
            dx2 = local_p[2].x - local_p[3].x;*/

            // high probability of waiting
            if(speed<walking_speed_thrd)
            {
                if( (*it_track).rects3D.front().wx<X_close_door && ((*it_track).cur_status<0 || (*it_track).cur_status==2) )
                {
                    (*it_track).cur_status = 2; // leaving from elevator car (close to door at begining)
                }
                else
                {
                    if( (*it_track).cur_status<0  )
                    {
                        (*it_track).cur_status = 1; // approaching, just entering field of view
                    }
                    else
                    {
                        (*it_track).cur_status = 0; //waiting
                    }

                    if(speed<waiting_speed_thrd)
                        (*it_track).cur_status = 0; //waiting

                    //if( local_p[0].x<0.5 && dx<0)
                    //	(*it_track).cur_status = 1; // approaching

                }

            }
            else
            {
                // fit line to get moving direction
                fit_line(local_p, n_local_p, line);

                if( fabs(line[0])<0.00001 )
                {
                    y0 = 100;
                }
                else
                {
                    y0 = line[3] - line[2]*line[1]/line[0];
                }

                // check whether within the elevator door area
                if( y0>minY_door_position && y0<maxY_door_position )
                {
                    if( (*it_track).rects3D.front().wx<X_close_door && ((*it_track).cur_status<0 || (*it_track).cur_status==2) ) 
                    {
                        (*it_track).cur_status = 2; // leaving out from elevator car
                    }
                    else
                    {
                        if(dx<=0)
                        {
                            (*it_track).cur_status = 1; // approaching
                        }
                        else
                        {
                            (*it_track).cur_status = 2; // leaving
                        }

                    }
                }
                else
                {
                    (*it_track).cur_status = 3; // passing by
                }

            }

            Rect3D rt3d = (*it_track).rects3D.back();
            // CvRect rt = rt3d.rect;

            //if( (*it_track).cur_status==3 && speed<1)
            //{
            //	double cx = rt3d.wx;
            //	double cy = rt3d.wy;

            //	double dis = sqrt( cx*cx + (cy-0.5)*(cy-0.5) );

            //	if( dis<0.8 )
            //		(*it_track).cur_status = 1; //approaching

            //}

            //if( (*it_track).cur_status==-1 && dx<0 )
            //{
            //	(*it_track).cur_status = 1; //approaching
            //}

            // smooth status based on last three status, important to make the algorithm robust
            (*it_track).smoothed_status = smooth_status((*it_track).cur_status, (*it_track).last_status, (*it_track).last_last_status);

#ifndef CFG_OPT_NO_RGB
            if( g_show_detail_info )
            {
                // show locaiton in rgb
                sprintf(text, "(%2.1f, %2.1f, %2.1f)", rt3d.wx, rt3d.wy, rt3d.wz);
                cvPutText( rgb2, text, cvPoint(rt.x-5, rt.y+rt.height/2-10), &font, cvScalar(0,0,255) );
                // show info in rgb
                sprintf(text, "(%2.1f, %2.2f, %2.1f)", dx, speed, y0);
                cvPutText( rgb2, text, cvPoint(rt.x-5, rt.y+rt.height/2+10), &font, cvScalar(0,0,255) );
                // show tracking id in rgb
                sprintf(text, "id=%d", (*it_track).id);
                cvPutText( rgb2, text, cvPoint(rt.x, rt.y+rt.height+5), &font, cvScalar(255,0,0) );
            }
#endif
            // show passenger position
            /*int tshow_y = int(rt3d.wx/psize + or_y);
            int tshow_x = int(rt3d.wy/psize + or_x);*/
            /*int tshow_y = int(rt3d.wy/psize_m + or_y);
            int tshow_x = int(or_x - rt3d.wx/psize_m);*/
            int tshow_y = int(or_y - rt3d.wx/psize_m); // Berlin lab
            int tshow_x = int(or_x - rt3d.wy/psize_m);

            if(tshow_x<0)
                tshow_x = 1;

#ifndef CFG_OPT_NO_RGB
            // show rect in rgb and position in 2d 
            if((*it_track).smoothed_status>=0)
            {
                cvRectangle( rgb2, cvPoint(rt.x, rt.y), cvPoint(rt.x+rt.width, rt.y+rt.height), cvScalar(0,0,255), 2);

                cvCircle(Tshow, cvPoint(tshow_x, tshow_y), 4, colors[(*it_track).smoothed_status], -1);
            }
#endif

            // show ETA for approaching passenger
            if( (*it_track).smoothed_status==1 && rt3d.wx > X_close_door )
            {
                double cx = rt3d.wx;
                double cy = rt3d.wy;

                double dis = sqrt( cx*cx + (cy)*(cy) );

                double eta = dis/speed;

                sprintf(text, "ETA=%2.1f", eta);
                cvPutText( Tshow, text, cvPoint(tshow_x-25, tshow_y+15), &font, cvScalar(0,255,0) );

            }

            // show direction in 2d
            if(speed>=walking_speed_thrd && (*it_track).smoothed_status>=1)
            {
                int x2, y2;			
                double vec = speed*10;
                if( fabs(line[0])<0.00001 )
                {
                    x2 = tshow_x;

                    dy = local_p[0].y - local_p[n-1].y;
                    if(dy>0)
                        y2 = int(tshow_y-vec);
                    else
                        y2 = int(tshow_y+vec);	
                }
                else
                {
                    double a = atan(line[1]/line[0]);

                    double tx = vec*cos(a);
                    double ty = vec*sin(a);

                    if(dx>0)
                    {
                        tx = -tx;
                        ty = -ty;
                    }
                    y2 = tshow_y + int(tx);
                    x2 = tshow_x + int(ty);
                }

                cvLine(Tshow, cvPoint(tshow_x, tshow_y), cvPoint(x2, y2), colors[(*it_track).smoothed_status], int(1.5));
            }

            // update last status
            (*it_track).last_speed = speed;
            (*it_track).last_last_status = (*it_track).last_status;
            (*it_track).last_status = (*it_track).cur_status;

        }

    }  // for(; it_track!=trackset.end(); it_track++)

}

void load_bg_from_file()
{
#ifdef CODEBOOK_EX
    memset(codebooks_ex, 0, sizeof(CodeBookEx)*halfw*halfh);
#endif

    ifstream ifbg;
    ifbg.open(cmd_bg_file);
    if(!ifbg)
        cout<< "Error in opening bg file !"<<endl;

    string str;
    int i, nbox, d;
    char *pstr;
    char* p;

    while( !ifbg.eof() )
    {
        getline(ifbg, str);
        pstr = (char*) str.c_str();

        // index
        p=strtok(pstr, " ");
        if(p == '\0')  // the last line of bg file is empty
            break;

        i  = atoi(p);

        // number of boxes
        p = strtok(NULL, " ");
        nbox = atoi(p);

        for(int k=0; k<nbox; k++)
        {
            CodeBox newbox;

            p = strtok(NULL, " ");
            d = atoi(p);
            newbox.min = d;

            p = strtok(NULL, " ");
            d = atoi(p);
            newbox.max = d;

            codebooks[i].m_codebook.boxes.push_back(newbox);

        }

        p = strtok(NULL, " ");
        if(p!=NULL)
            cout<< "Error in parsing bg file !"<<endl;

    }

#ifdef CODEBOOK_EX
    // Add tolerances to the codebook
    // Merge boxes
    for(int i=0; i<halfw*halfh; i++)
    {
        CBgCodebook* cb = &codebooks[i];
        cb->add_tolerance();
        cb->merge();
    }

    for(int i=0; i<halfw*halfh; i++)
    {
        CodeBook *cb = &codebooks[i].m_codebook;
        codebooks_ex[i].count       = (int)(cb->boxes.size());
        list<CodeBox>::iterator it  = cb->boxes.begin();
        for(int j=0 ; it!=cb->boxes.end(); it++, j++)
        {
            codebooks_ex[i].limits[j].min   = (*it).min;
            codebooks_ex[i].limits[j].max   = (*it).max;
        }
    }
#endif
}

void parse_cmd_line(int argc, char* argv[])
{
    int c;
    static struct option long_options[] =
    {
        {"verbose",          no_argument,        NULL, 'v'},
        {"log",              required_argument,  NULL, 'l'},
        {"background-file",  required_argument,  NULL, 'b'},
        {"depth-video-file", optional_argument,  NULL, 'd'},
        {"test",             required_argument,  NULL,  0 },
        {0, 0, 0, 0}
    };

    while (1)
    {
        int option_index = 0;
        c = getopt_long (argc, argv, "vl:b:d:",
                long_options, &option_index);
        if (c == -1)
            break;

        switch (c)
        {
            case 0:
                printf ("option %s", long_options[option_index].name);
                if (optarg)
                    printf (" with arg %s", optarg);
                printf ("\n");
                break;

            case 'b':
                cmd_bg_file = optarg;
                break;

            case 'd':
                cmd_depth_file = optarg;
                break;

            case 'l':
                cmd_log_file = optarg;
                break;

            case 'v':
                cmd_verbose = 1;
                break;

            default:
                printf ("?? getopt returned character code 0%o ??\n", c);
        }
    }

    if (!cmd_bg_file) {
        printf("background file must provided.\n");
        exit(-1);
    }

    if (cmd_verbose)
    {
        printf("Background file: %s\n", cmd_bg_file);
        if (cmd_depth_file) {
            printf("Test depth video file: %s\n", cmd_depth_file);
        }
        printf("Saving output to            %s\n", cmd_log_file);
    }

#ifndef CFG_OPENNI
    g_file_test = true;
#endif
}

#ifdef CFG_OPENNI_V1
void CheckOpenNIError( XnStatus result, string status )
{
    if( result != XN_STATUS_OK ) {
        cerr << status << " Error: " << xnGetStatusString( result ) << endl;
        printf("Fatal Error: Check whether Kinect is connected or not.\n");
        exit(-1);
    }
}
#endif

#ifdef CFG_OPENNI_V2
using namespace openni;
Device device;
VideoStream depth;
VideoFrameRef frame;

int kinect_init()
{
    Status rc = OpenNI::initialize();
    if (rc != STATUS_OK)
    {
        printf("Initialize failed\n%s\n", OpenNI::getExtendedError());
        return 1;
    }

    rc = device.open(ANY_DEVICE);
    if (rc != STATUS_OK)
    {
        printf("Couldn't open device\n%s\n", OpenNI::getExtendedError());
        return 2;
    }

    if (device.getSensorInfo(SENSOR_DEPTH) != NULL)
    {
        rc = depth.create(device, SENSOR_DEPTH);
        if (rc != STATUS_OK)
        {
            printf("Couldn't create depth stream\n%s\n", OpenNI::getExtendedError());
            return 3;
        }

        const SensorInfo* sinfo = device.getSensorInfo(SENSOR_DEPTH);
        const Array<VideoMode>& modes = sinfo->getSupportedVideoModes();

        for (int i=0; i<modes.getSize(); i++) {
            printf("%i: %ix%i, %i fps, %i format\n",
                i,
                modes[i].getResolutionX(),
                modes[i].getResolutionY(),
                modes[i].getFps(),
                modes[i].getPixelFormat()
                );
        }

        //rc = depth.setVideoMode(modes[0]); // 320x240, 30fps, format: 100
        //rc = depth.setVideoMode(modes[4]); // 640x480, 30fps, format: 100
        rc = depth.setVideoMode(modes[4]); // 640x480, 30fps, format: 100

        if (rc != openni::STATUS_OK) {
            printf("Failed to set depth resolution\n");
            return -1;
        }
    }

    rc = depth.start();
    if (rc != STATUS_OK)
    {
        printf("Couldn't start the depth stream\n%s\n", OpenNI::getExtendedError());
        return 4;
    }

    return 0;
}
#endif

void init_image_data(ICSContext* ctx)
{
    // image data for processing
#ifndef CFG_OPT_NO_RGB
    imgRGB8u = cvCreateImage(cvSize(Width, Height),IPL_DEPTH_8U,3);
    rgb2 = cvCreateImage( cvSize(halfw, halfh), 8, 3 );
    bgr2 = cvCreateImage(cvSize(halfw, halfh), IPL_DEPTH_8U, 3);
#endif

    imgDepth16u = cvCreateImage(cvSize(Width,Height),IPL_DEPTH_16U,1);
    depth2 = cvCreateImage( cvSize(halfw, halfh), 16, 1 );
    pFgMask = cvCreateImage( cvSize(halfw+2, halfh+2), 8, 1 );
    depthShow = cvCreateImage( cvSize(halfw, halfh), 8, 1 );

#ifdef CFG_OPENNI_V2
    kinect_init();
#elif defined CFG_OPENNI_V1
    // Kinect initilization
    XnStatus result = XN_STATUS_OK;

    xn::ScriptNode scriptNode;
    xn::EnumerationErrors errors;

    // FIXME: detect whether xml file exists
    result = ctx->openniV1Context.InitFromXmlFile("enics_cfg.xml", scriptNode, &errors);

    CheckOpenNIError( result, string("initialize context with ") + "enics_cfg.xml" );

    result = ctx->depthGenerator.Create( ctx->openniV1Context );
    CheckOpenNIError( result, "Create depth generator" );
#ifndef CFG_OPT_NO_RGB
    result = ctx->imageGenerator.Create( ctx->openniV1Context );
    CheckOpenNIError( result, "Create image generator" );
#endif

    XnMapOutputMode mapMode;
    mapMode.nXRes = Width;
    mapMode.nYRes = Height;
    mapMode.nFPS = 15;
    printf("Setting map mode to %dx%d @%dfps\n", Width, Height, mapMode.nFPS);
    result = ctx->depthGenerator.SetMapOutputMode( mapMode );
#ifndef CFG_OPT_NO_RGB
    result = ctx->imageGenerator.SetMapOutputMode( mapMode );
    //correct view port
    ctx->depthGenerator.GetAlternativeViewPointCap().SetViewPoint( ctx->imageGenerator );
#endif
    //read data
    result = ctx->openniV1Context.StartGeneratingAll();
    result = ctx->openniV1Context.WaitNoneUpdateAll();
#else // use file input
    frame_count = 300;
    assert(cmd_depth_file);
    fp_depth_in	= fopen(cmd_depth_file, "rb");
    assert(fp_depth_in);
#ifndef CFG_OPT_NO_RGB
    assert(false && "Not implemented"); // FIXME
    fp_rgb_in		= fopen("rgb_640x480x3_300frames.rgb", "rb");
    assert(fp_rgb_in);
#endif
#endif
}

#ifdef CODEBOOK_EX
// >0: not included
// <=0: included
inline bool bg_match(int index, int val)
{
    bool match = false;
    CodeBookEx* cbx = &codebooks_ex[index];
    int box_cnt = cbx->count;
//#define SMART_MATCH
#ifdef SMART_MATCH
    int64_t prod = 1;
    if (val < cbx->limits[0].min ||
        val > cbx->limits[box_cnt-1].max) {
            match = false;
    } else {
        // FIXME: Assume no overflow here
        for(int i=0; i<box_cnt; i++)
        {
            prod *= (int64_t)(cbx->limits[i].min - val) *
                (cbx->limits[i].max - val);
        }
        if (prod <= 0) match = true;

    }
#else
    for(int i=0; i<box_cnt; i++)
    {
        if(val >= cbx->limits[i].min && val <= cbx->limits[i].max)
        {
            match = true;
            break;
        }
    }

#endif
    return match;
}
#endif

void init_gui()
{
    // display parameters
    Tshow = cvCreateImage( cvSize((int)(show_w), (int)(show_h)), 8, 3 );
    Tshow->origin = 0;
    cvSet(Tshow, cvScalar(255,255,255));

#if 0
    // show lobby
    cvRectangle(Tshow, cvPoint(bd, bd), cvPoint(lobby_w+bd, lobby_h+bd), cvScalar(100,100,100), 1);
    cvLine(Tshow, cvPoint(lobby_w+bd, bd), cvPoint(lobby_w+bd, bd+130/psize), cvScalar(255,255,255), 1);
    cvLine(Tshow, cvPoint(lobby_w+bd, bd), cvPoint(lobby_w+bd+30, bd), cvScalar(100,100,100), 1);
    cvLine(Tshow, cvPoint(lobby_w+bd, bd+130/psize), cvPoint(lobby_w+bd+30, bd+130/psize), cvScalar(100,100,100), 1);

    // show elevator door
    int ele_x1 = bd + lobby_w;
    int ele_y1 = bd + 250/psize-20;
    cvRectangle(Tshow, cvPoint(ele_x1, ele_y1), cvPoint(ele_x1+5, ele_y1+106/psize), cvScalar(255,0, 0), -1);

    // show field of view
    cvLine(Tshow, cvPoint(20, 126-20), cvPoint(40+bd, bd), cvScalar(200,200,200), 1);
    cvLine(Tshow, cvPoint(20, 126-20), cvPoint(293-15,300), cvScalar(200,200,200), 1);
    cvLine(Tshow, cvPoint(293-15+28,300), cvPoint(lobby_w+bd, lobby_h+bd-80), cvScalar(200,200,200), 1);
#endif

    // origin position in 2D plane figure
    or_x = int(origin_x/psize_m); // changed by Hui on 02/10, 2015, or_x is global variable and will be used later
    or_y = int(origin_y/psize_m);

    //cvCircle(Tshow, cvPoint(or_x, or_y), 5, cvScalar(255,0,0),-1); // show origin
    //// draw lobby layout and Kinect field of view (specific for different location)
    //cvLine(Tshow,cvPoint(0,int(4.5/psize_m)),cvPoint(int(1/psize_m),int(4.5/psize_m)),cvScalar(100,100,100));
    //cvLine(Tshow,cvPoint(int(1/psize_m),0),cvPoint(int(1/psize_m),int(4.5/psize_m)),cvScalar(100,100,100));
    //cvLine(Tshow,cvPoint(int(2.6/psize_m),0),cvPoint(int(2.6/psize_m),int(4.5/psize_m)),cvScalar(100,100,100));
    //cvLine(Tshow,cvPoint(int(5.2/psize_m),int(4.5/psize_m)),cvPoint(int(2.6/psize_m),int(4.5/psize_m)),cvScalar(100,100,100));

    // changed by Hui on 02/10,2015, based on new lobby layout measurements by Alan on 02/09/2015
    // draw elevator door
    cvRectangle(Tshow, cvPoint(int((origin_x-0.45)/psize_m), int(show_h-5)),cvPoint(int((origin_x+0.45)/psize_m), int(show_h)),cvScalar(255,0,0),-1);
    // draw lobby layout (specific for different location)
    cvLine(Tshow,cvPoint(0,int(3.0/psize_m)),cvPoint(int(1.06/psize_m),int(3.0/psize_m)),cvScalar(100,100,100));
    cvLine(Tshow,cvPoint(int(1.06/psize_m),0),cvPoint(int(1.06/psize_m),int(3.0/psize_m)),cvScalar(100,100,100));
    cvLine(Tshow,cvPoint(int(1.84/psize_m),0),cvPoint(int(1.84/psize_m),int(1.1/psize_m)),cvScalar(100,100,100));
    cvLine(Tshow,cvPoint(int(1.84/psize_m),int(1.1/psize_m)),cvPoint(int(2.66/psize_m),int(1.1/psize_m)),cvScalar(100,100,100));
    cvLine(Tshow,cvPoint(int(2.66/psize_m),int(3.0/psize_m)),cvPoint(int(2.66/psize_m),int(1.1/psize_m)),cvScalar(100,100,100));
    cvLine(Tshow,cvPoint(int(2.66/psize_m),int(3.0/psize_m)),cvPoint(int(5.04/psize_m),int(3.0/psize_m)),cvScalar(100,100,100));

    Tshow_copy = cvCloneImage(Tshow);
}

inline void forground_filtering(IplImage* img)
{
    // for 2D morphological operation
    IplConvKernel* element_3_3 = cvCreateStructuringElementEx( 3, 3, 1, 1,  CV_SHAPE_RECT);
    IplConvKernel* element_5_5 = cvCreateStructuringElementEx( 5, 5, 2, 2,  CV_SHAPE_RECT);
    IplConvKernel* element_5_10 = cvCreateStructuringElementEx( 5, 10, 2, 5,  CV_SHAPE_RECT);

    cvDilate(pFgMask, pFgMask, element_3_3, 1);
    cvErode(pFgMask, pFgMask, element_3_3, 1);

    cvErode(pFgMask, pFgMask, element_5_5, 1);
    cvDilate(pFgMask, pFgMask, element_5_5, 1);

    cvDilate(pFgMask, pFgMask, element_5_10, 1);
    cvErode(pFgMask, pFgMask, element_5_10, 1);

#ifdef CFG_SHOW_GUI
    cvShowImage( "Filtered Foreground", pFgMask );
#endif
}

void init_communication()
{
#ifdef CFG_SEND_ICS_SERVER
    const char* svr_ip = "127.0.0.1";
    const char* svr_port = "7770";
    g_comm = comm_init(svr_ip, svr_port);
    printf("Sending test OSC messages to %s:%s\n", svr_ip, svr_port);
    comm_send_data(g_comm, 99, 99, 99, 99);
#endif
}

int main(int argc, char* argv[])
{
    ICSContext icsContext;

    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = handle_ctrl_c;
    sigaction(SIGINT, &sa, NULL);

    init_communication();

    cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 0.4, 0.4,  0, 0, 6);

    parse_cmd_line(argc, argv);

    fp_log_file = fopen(cmd_log_file, "w+"); // TODO: Note for Windows
    if (!fp_log_file)
    {
        printf("logfile %s created failed.\n", cmd_log_file);
        exit(-1);
    }

    init_image_data(&icsContext);

    init_gui();

#ifdef PLATFORM_WINDOWS
    // check computational time
    QueryPerformanceFrequency(&litmp); // get fregquency
    dfFreq = (double)litmp.QuadPart;
#endif

    CAnalytics* analytics = new CAnalytics();

    TrackSet trackset;  // initialize track

    // initialize the frame to frame time
    for(int i=0; i<n_local_p; i++)
    {
        dt_array[i] = 0.025;
    }

    // initialize background model
    int len = halfw*halfh;
    codebooks = new CBgCodebook[len];

#ifndef LEARN_BACKGROUND
    load_bg_from_file();
#endif

#ifdef CFG_SHOW_GUI
    g_show_detail_info = true;
#endif

#ifdef CFG_OPENNI
    int f_prc_n = 0;
#else
    int f_prc_n = 350; // gph: make hall call printing code happy (print 6)
#endif

#ifdef USE_OZONE
    // communication to ozone
    hPort = SerialPortOpen();
    //assert(hPort);
#endif

    // log file saving the command sent to ozone
    comd.open("command.txt");
#ifdef RECORD_DEPTH_VIDEO
    FILE* fp_depth_out = fopen("depth_640x480_16bit.depth", "wb+");
    assert(fp_depth_out);
#endif

#ifdef RECORD_RGB_VIDEO
    FILE* fp_rgb_out = fopen("rgb_640x480x3.rgb", "wb+");
    assert(fp_rgb_out);
#endif

#ifdef PLATFORM_WINDOWS
    QueryPerformanceCounter(&litmp);
    start_time = litmp.QuadPart;
#endif

    tick();

#ifdef CFG_OPENNI
#ifdef CFG_OPENNI_V2
    while (!wasKeyboardHit())
#else // CFG_OPENNI_V1
    XnStatus result;
    while( !(result = icsContext.openniV1Context.WaitAnyUpdateAll( )) )
#endif
#else // file input
    for(int i=0; i<frame_count; i++)
#endif
    {
        f_prc_n++;

        get_depth(&icsContext, depth2);
        // point to depth data
        int16* p = (int16*)depth2->imageData;

        XnPoint3D wo[1]; // in world coordinates
#ifndef IMAGE2WORLD_OPT
        XnPoint3D co[1]; // in kinect coordinates
#endif

        cvZero(pFgMask);
        // unsigned char *fg = (unsigned char*)pFgMask->imageData;

        int index, pv;
        int x, y, z;

        for(int j=min_row; j<max_row; j=j+2)
        {
            for(int i=min_column; i<max_column; i=i+2)
            {
                index = i + j*halfw;

                // depth value of current pixel
                pv = *(p+index);

                if(pv==0)
                    continue;

                // do background box matching
#ifdef CODEBOOK_EX
                if( bg_match(index, pv) == false )
#else
                if( false == codebooks[index].bgSub_codebook(pv, down_offset, up_offset) )
#endif

                {
                    // get 3D depth frame coordinates
                    x = i*2;
                    y = j*2;
                    z = ((int16*)(imgDepth16u->imageData + imgDepth16u->widthStep*y))[x];

#ifdef IMAGE2WORLD_OPT
                    convert_image2world(z, x, y, wo, transf_matrix);
#else
                    convert_image2camera(z, x, y, co);

                    convert_camera2world(co, wo, transf_matrix);
#endif

                    // do geometry filtering to remove noise
                    if( wo->X>min_X && wo->X<max_X && wo->Z>min_Z && wo->Z<max_Z)
                    {
                        ((uchar*)(pFgMask->imageData + pFgMask->widthStep*(j+1)))[i+1] = 255;

                    }
                }

            }
        }

#ifdef CFG_SHOW_GUI
        if( g_show_detail_info )
            cvShowImage( "Raw Foreground", pFgMask );
#endif

        forground_filtering(pFgMask);

        // foreground blobs extraction based on depth segmentation
        RECTSD rectsd;
        analytics->fg_depth_segment_v2(pFgMask, depth2, depth_discontinuity_thrd, size_thrd, &rectsd);

#ifdef CFG_SHOW_GUI
        cvShowImage( "Filtered Foreground", pFgMask );
#endif

        // get 3D world coordinates of segmented rects
        RECTS3D rects3D;
        RECTSD::iterator it = rectsd.begin();

        for(; it!=rectsd.end(); it++)
        {
            CvRect rt = (*it).rect;

            // get average depth coordinates
            double x, y, n;
            x = 0;
            y = 0;
            n = 0;
            for(int i=rt.x; i<=rt.x+rt.width; i++)
            {
                for(int j=rt.y; j<=rt.y+rt.height; j++)
                {
                    if( ((uchar*)(pFgMask->imageData + pFgMask->widthStep*j))[i] == 255 )
                    {
                        x = x+i;
                        y = y+j;
                        n++;
                    }

                }
            }

            int xc = cvRound(x/n)*2;
            int yc = cvRound(y/n)*2;

            int zc = int((*it).d);

#ifdef IMAGE2WORLD_OPT
            convert_image2world(zc, xc, yc, wo, transf_matrix);
#else
            // get Kinect coordinates
            convert_image2camera(zc, xc, yc, co);

            // do geometry filtering based on prior knowledge
            convert_camera2world(co, wo, transf_matrix);
#endif

            // do geometry filtering based on prior knowledge
            if( wo->X>min_X && wo->X<max_X && wo->Z>min_Z && wo->Z<max_Z)
            {
                Rect3D rect3D;
                rect3D.rect = rt;
                rect3D.wx = wo->X;
                rect3D.wy = wo->Y;
                rect3D.wz = wo->Z;

                fprintf(fp_log_file, "x:%.2f, y:%.2f\n", rect3D.wx, rect3D.wy);
                rects3D.push_back(rect3D);
            }
        }


        // foreground blob tracking
        analytics->track(&trackset, rects3D, max_tracking_dis);

        // delete tracker if its confidence become 0
        analytics->trackset_delete(&trackset);

        // whether empty or non-empty
        if(rects3D.size()>0)
            g_lobby_status = 1;
        else
        {
            g_lobby_status = 0;
        }

        // display results
        if( g_lobby_status==1 )
        {
            display_results(&trackset);
        }
        else
        {
            // delete all the trackers if the lobby is empty
            analytics->trackset_delete_all(&trackset);
            analytics->trackID = 0;
        }

        // summarize how many people for each intention for response function generation
        get_passenger_status(&trackset);

        if (g_file_test) {
            show_button(f_prc_n);
        }

#ifdef CFG_SHOW_GUI
        // show result
#ifndef CFG_OPT_NO_RGB
        cvShowImage(  "RGB Frame", rgb2 );
#endif
        cvShowImage(  "Depth Frame", depthShow );
        //cvShowImage(  "Depth", depth2 );
        //cvShowImage(  "Foreground", pFgMask );
        cvShowImage(  "Track", Tshow );
#endif

        RECTSD::iterator it_d = rectsd.begin();
        for(; it_d!=rectsd.end(); )
        {
            it_d = rectsd.erase(it_d);
        }

        RECTS3D::iterator it_3d =  rects3D.begin();
        for(; it_3d!=rects3D.end(); )
        {
            it_3d = rects3D.erase(it_3d);

        }

#ifdef CFG_SHOW_GUI
        // hack... slow down frame rate for more stable (larger) change in distance measurements
        cvWaitKey(1);   // 0== wait forever, N == N ms. Hui suggested 80, but this slowed the messages-AF
#endif

    }
    printf("Processing time: %.3f s\n", double(tock())/1000000);

    comd.close();

#ifdef RECORD_DEPTH_VIDEO
    fclose(fp_depth_out);
    fclose(fp_rgb_out);
#endif

#ifdef USE_OZONE
    SerialPortClose(hPort);
#endif

#ifdef CFG_SHOW_GUI
    cvReleaseImage(&Tshow);
#endif

    fclose(fp_log_file);

    return 0;
}
