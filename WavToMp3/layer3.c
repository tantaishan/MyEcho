/* layer3.c */

#include "types.h"

//#define NO_RESERVOIR

int *scalefac_band_long;

/* Scalefactor bands. */
static int sfBandIndex[4][3][23] =
{
  { /* MPEG-2.5 11.025 kHz */
    {0,6,12,18,24,30,36,44,54,66,80,96,116,140,168,200,238,284,336,396,464,522,576},
    /* MPEG-2.5 12 kHz */
    {0,6,12,18,24,30,36,44,54,66,80,96,116,140,168,200,238,284,336,396,464,522,576},
    /* MPEG-2.5 8 kHz */
    {0,12,24,36,48,60,72,88,108,132,160,192,232,280,336,400,476,566,568,570,572,574,576}
  },
  {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  },
  { /* Table B.2.b: 22.05 kHz */
    {0,6,12,18,24,30,36,44,54,66,80,96,116,140,168,200,238,284,336,396,464,522,576},
    /* Table B.2.c: 24 kHz */
    {0,6,12,18,24,30,36,44,54,66,80,96,114,136,162,194,232,278,332,394,464,540,576},
    /* Table B.2.a: 16 kHz */
    {0,6,12,18,24,30,36,44,54,66,80,96,116,140,168,200,238,284,336,396,464,522,576}
  },
  { /* Table B.8.b: 44.1 kHz */
    {0,4,8,12,16,20,24,30,36,44,52,62,74,90,110,134,162,196,238,288,342,418,576},
    /* Table B.8.c: 48 kHz */
    {0,4,8,12,16,20,24,30,36,42,50,60,72,88,106,128,156,190,230,276,330,384,576},
    /* Table B.8.a: 32 kHz */
    {0,4,8,12,16,20,24,30,36,44,54,66,82,102,126,156,194,240,296,364,448,550,576}
  }
};

/*
 * L3_compress:
 * ------------
 */
void L3_compress(void)
{
  int          frames_processed;
  int           ch;
  int           i;
  int           gr;
  static short    buffer[2][1152];
  short          *buffer_window[2];
  int          remainder;
  int          bytes_per_frame;
  int          lag;
  int           mean_bits;
  int           sideinfo_len;
  static int    l3_enc[2][2][samp_per_frame2];
  static int   l3_sb_sample[2][3][18][SBLIMIT];
  static int   mdct_freq[2][2][samp_per_frame2];
  static L3_side_info_t side_info;

  if(config.mpeg.type == MPEG1)
  {
    config.mpeg.granules = 2;
    config.mpeg.samples_per_frame = samp_per_frame;
    config.mpeg.resv_limit = ((1<<9)-1)<<3;
    sideinfo_len = (config.mpeg.channels == 1) ? 168 : 288;
  }
  else /* mpeg 2/2.5 */
  {
    config.mpeg.granules = 1;
    config.mpeg.samples_per_frame = samp_per_frame2;
    config.mpeg.resv_limit = ((1<<8)-1)<<3;
    sideinfo_len = (config.mpeg.channels == 1) ? 104 : 168;
  }
  scalefac_band_long = sfBandIndex[config.mpeg.type][config.mpeg.samplerate_index];

#ifdef NO_RESERVOIR
  config.mpeg.resv_limit = 0;
#endif

  { /* find number of whole bytes per frame and the remainder */
    int x = config.mpeg.samples_per_frame * config.mpeg.bitr * (1000/8);
    bytes_per_frame = x / config.wave.samplerate;
    remainder  = x % config.wave.samplerate;
  }

  config.mpeg.total_frames =  /* round up */
        (config.wave.total_samples + config.mpeg.samples_per_frame - 1) /
                      config.mpeg.samples_per_frame;

  printf("%d frames\n",config.mpeg.total_frames);

  frames_processed = lag = 0;

  open_bit_stream(config.outfile);

  while(wave_get(buffer))
  {
    frames_processed++;
    if(((frames_processed & 7)==0) || (frames_processed >= config.mpeg.total_frames))
      printf("\015[%d] %d%%", frames_processed,(frames_processed*100)/config.mpeg.total_frames);

     buffer_window[0] = buffer[0];
     buffer_window[1] = buffer[1];
    /* sort out padding */
    config.mpeg.padding = (lag += remainder) >= config.wave.samplerate;
    if (config.mpeg.padding)
      lag -= config.wave.samplerate;
    config.mpeg.bits_per_frame = 8*(bytes_per_frame + config.mpeg.padding);

    /* bits per channel per granule */
    mean_bits = (config.mpeg.bits_per_frame - sideinfo_len) >>
                      (config.mpeg.granules + config.mpeg.channels - 2);

    /* polyphase filtering */
    for(gr=0; gr<config.mpeg.granules; gr++)
      for(ch=0; ch<config.mpeg.channels; ch++)
        for(i=0;i<18;i++)
          L3_window_filter_subband(&buffer_window[ch], &l3_sb_sample[ch][gr+1][i][0] ,ch);

    /* apply mdct to the polyphase output */
    L3_mdct_sub(l3_sb_sample, mdct_freq);

    /* bit and noise allocation */
    L3_iteration_loop(mdct_freq, &side_info, l3_enc, mean_bits);

    /* write the frame to the bitstream */
    L3_format_bitstream(l3_enc, &side_info);
  }

  close_bit_stream();
}


