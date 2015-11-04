#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>

#define MPEG2_5 0
#define MPEG2   2
#define MPEG1   3

#define MODE_STEREO       0
#define MODE_MS_STEREO    1
#define MODE_DUAL_CHANNEL 2
#define MODE_MONO         3

#define LAYER_3 1
#define LAYER_2 2
#define LAYER_1 3
#define samp_per_frame  1152
#define samp_per_frame2 576
#define PI              3.14159265358979
#define HAN_SIZE        512
#define SBLIMIT         32

typedef struct {
    FILE *file;
    int  type;
    int  channels;
    int  bits;
    int samplerate;
    int total_samples;
    int length;
} wave_t;

typedef struct {
    FILE *file;
    int  type;
    int  layr;
    int  mode;
    int  bitr;
    int  psyc;
    int  emph;
    int  padding;
    int samples_per_frame;
    int bits_per_frame;
    int bits_per_slot;
    int total_frames;
    int  bitrate_index;
    int  samplerate_index;
    int crc;
    int ext;
    int mode_ext;
    int copyright;
    int original;
    int channels;
    int granules;
    int resv_limit;
} mpeg_t;

typedef struct {
    time_t start_time;

    char* infile;
    wave_t wave;

    char* outfile;
    mpeg_t mpeg;
} config_t;
extern config_t config;


#define HUFFBITS unsigned int
#define HTN     34
#define MXOFF   250

struct huffcodetab {
  unsigned int xlen;    /*max. x-index+                         */
  unsigned int ylen;    /*max. y-index+                         */
  unsigned int linbits; /*number of linbits                     */
  unsigned int linmax;  /*max number to be stored in linbits    */
  HUFFBITS *table;      /*pointer to array[xlen][ylen]          */
  unsigned char *hlen;  /*pointer to array[xlen][ylen]          */
};

extern struct huffcodetab ht[HTN];/* global memory block                */
                                /* array of all huffcodtable headers    */
                                /* 0..31 Huffman code table 0..31       */
                                /* 32,33 count1-tables                  */

/* Side information */
typedef struct {
        unsigned part2_3_length;
        unsigned big_values;
        unsigned count1;
        unsigned global_gain;
        unsigned table_select[3];
        unsigned region0_count;
        unsigned region1_count;
        unsigned count1table_select;
        unsigned address1;
        unsigned address2;
        unsigned address3;
        int quantizerStepSize;
} gr_info;

typedef struct {
  int main_data_begin;
  struct
  {
    struct
    {
      gr_info tt;
    } ch[2];
  } gr[2];
  int resv_drain;
} L3_side_info_t;

/* function prototypes */

void error(char* s);

/* bitstream.c */
void open_bit_stream(char *bs_filenam);
void close_bit_stream(void);
void L3_format_bitstream(int l3_enc[2][2][samp_per_frame2],
                         L3_side_info_t  *l3_side);

/* l3loop.c */
void L3_iteration_loop(int            mdct_freq_org[2][2][samp_per_frame2],
                       L3_side_info_t *side_info,
                       int             l3_enc[2][2][samp_per_frame2],
                       int             mean_bits);

/* layer3.c */
void L3_compress(void);

/* wave.c */
void wave_open(int raw, int mono_from_stereo);
int wave_get(short buffer[2][1152]);
void wave_close(void);

/* coder.c */
void L3_window_filter_subband(short **buffer, int s[SBLIMIT],
                 int k);
void L3_mdct_sub(int sb_sample[2][3][18][SBLIMIT],
                 int mdct_freq[2][2][samp_per_frame2]);

/* utils.c */
//long mul(int x, int y);
//long muls(int x, int y);


#endif

