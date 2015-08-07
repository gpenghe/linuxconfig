#include <stdio.h>              // for printf()
#include <stdlib.h>             // for exit()
#include <string.h>             // for strncpy()
#include <assert.h>             // for assert()
#include <stdint.h>             // for uint32_t etc.

#include <iostream>             // for << and >>
#include <string>               // for string
#include <vector>               // for vector
#
#include "OptionParser.h"

#include "flc.h"

#define OFF      0
#define ON       1
#define FLASHING 2

/**************************
 * Command arguments
 * ************************/

static std::string arg_target_address = "all";
static int arg_verbose                = 0;

static std::string arg_audio_file     = "";
static int arg_audio_freq             = 8000;
static int arg_audio_channels         = 2;
static int arg_audio_sample_bits      = 8;
static int arg_audio_sample_bits_tx   = 8;

static int  arg_led_status            = OFF;
static int  arg_output_status         = OFF;
static bool arg_disable               = false;

static std::string arg_device         = "";

/**************************
 * Globals
 * ************************/

static uint8_t g_pkt_buffer[65536];

/**************************
 * Functions
 * ************************/

int init_flc_packet_header(FLC_PacketHeader* header)
{
    char* p8 = (char*) &header->sentinel;
    uint16_t* p16;

    *p8 ++ = 'A';
    *p8 ++ = 'A';
    *p8 ++ = 'A';
    *p8 ++ = 'A';

    *p8 ++ = 'F';
    *p8 ++ = 'L';
    *p8 ++ = 'C';
    *p8 ++ = ((char)1) << 4;

    p16 = (uint16_t*) p8;
    *p16 ++ = sizeof(FLC_PacketHeader) + 2 /*CRC*/;
    *p16 ++ = 0;    // flag

    return (char*)p16 -(char*)header;
}

int main(int argc, char* argv[])
{
    /*
     * Command line parsing start
     */

    using optparse::OptionParser;

    std::string usage = "Usage: %prog [options] <device_name>";
    OptionParser parser = OptionParser().usage(usage).description("Send out FLC packets through a serial line.");

    parser.add_option("-t", "--target-address").help("Address of the target (\"all\" for broadcast)").set_default(arg_target_address);
    parser.add_option("-v", "--verbose").help("Print verbose output").action("store_true").set_default(arg_verbose);

    parser.add_option("-a", "--audio-file").help("Audio file to send out").set_default(arg_audio_file);
    parser.add_option("-f", "--audio-freq").help("Audio sampling frequency (in Hz)").set_default(arg_audio_freq).type("int");
    parser.add_option("-c", "--audio-channels").help("Audio channel numbers (1 or 2)").set_default(arg_audio_channels).type("int");
    parser.add_option("-b", "--audio-sample-bits").help("Audio sampling bits").set_default(arg_audio_sample_bits).type("int");
    parser.add_option("-x", "--audio-sample-bits-tx").help("Audio bits being transferred for each sample").set_default(arg_audio_sample_bits_tx).type("int");

    parser.add_option("-l", "--led-status").help("Set device LED status (0, 1 or 2)").set_default(arg_led_status).type("int");
    parser.add_option("-o", "--output-status").help("Set device output status (0 or 1)").set_default(arg_output_status).type("int");

    parser.add_option("-d", "--disable").help("Disable target device").action("store_true").set_default(arg_disable);

    optparse::Values options = parser.parse_args(argc, argv);
    std::vector<std::string> args = parser.args();

    arg_target_address       = options["target_address"];
    arg_verbose              = atol(options["verbose"].c_str());

    arg_audio_file           = options["audio_file"];
    arg_audio_freq           = atol(options["audio_freq"].c_str());
    arg_audio_channels       = atol(options["audio_channels"].c_str());
    arg_audio_sample_bits    = atol(options["audio_sample_bits"].c_str());
    arg_audio_sample_bits_tx = atol(options["audio_sample_bits_tx"].c_str());

    arg_led_status           = atol(options["led_status"].c_str());
    arg_output_status        = atol(options["output_status"].c_str());
    arg_disable              = bool(atol(options["disable"].c_str()));

    if (args.empty()) {
        fprintf(stderr, "Fatal Error: device not provided.\nA target device name must be provided. It can be either a serial port name, like \"/dev/ttyUSB0\", or a file name like \"foo.out\", or \"-\" for stdout.");
        parser.print_usage();
        exit(-1);
    }
    arg_device = args[0];

    if (arg_verbose) {
        printf("target_address:         %s\n", arg_target_address.c_str());
        printf("audio_file:             %s\n", arg_audio_file.c_str());
        printf("audio_freq:             %d\n", arg_audio_freq);
        printf("audio_channels:         %d\n", arg_audio_channels);
        printf("audio_sample_bits:      %d\n", arg_audio_sample_bits);
        printf("audio_sample_bits_tx:   %d\n", arg_audio_sample_bits_tx);
        printf("led_status:             %d\n", arg_led_status);
        printf("output_status:          %d\n", arg_output_status);
        printf("disable:                %d\n", arg_disable);
        printf("Output to:              %s\n", arg_device.c_str());
    }

     /*
     * Command line parsing end
     */

    uint8_t *p8;
    int n;
    FLC_PacketHeader* header;

    header = (FLC_PacketHeader*)g_pkt_buffer;
    n = init_flc_packet_header(header);
    assert( n == sizeof(FLC_PacketHeader) );

    header->msg_len = sizeof(FLC_PacketHeader) + 2 /*CRC*/;

    p8 = g_pkt_buffer;
    p8 += n;
    *p8 ++ = 0; // CRC[0]
    *p8 ++ = 0; // CRC[1]
    
    FILE* fp;
    if (arg_device == std::string("-")) {
        fwrite(g_pkt_buffer, 1, header->msg_len, stdout);
    } else {
        fp = fopen(arg_device.c_str(), "w");
        assert(fp);
        fwrite(g_pkt_buffer, 1, header->msg_len, fp);
    }

    return 0;
}
