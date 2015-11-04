/* main.c
 * Command line interface.
 *
 * This fixed point version of shine is based on Gabriel Bouvigne's original
 * source, version 0.1.2
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <Windows.h>

#include "types.h"

config_t config;
int cutoff;

/*
 * error:
 * ------
 */
void error(char *s)
{
  printf("[ERROR] %s\n",s);
  exit(1);
}

/*
 * print_usage:
 * ------------
 */
static void print_usage()
{
  printf("\nUSAGE   :  Shine [options] <infile> <outfile>\n"
         "options : -h            this help message\n"
         "          -b <bitrate>  set the bitrate [32-320], default 128kbit\n"
         "          -c            set copyright flag, default off\n"
         "          -o            reset original flag, default on\n"
         "          -r [sample rate]  raw cd data file instead of wave\n"
         "          -m            mono from stereo, raw mono with -r\n"
         "The sample rate is optional and defaults to 44100.\n\n");
  exit(0);
}

/*
 * set_defaults:
 * -------------
 */
static void set_defaults()
{
  config.mpeg.type = MPEG1;
  config.mpeg.layr = LAYER_3;
  config.mpeg.mode = MODE_DUAL_CHANNEL;
  config.mpeg.bitr = 128;
  config.mpeg.psyc = 0;
  config.mpeg.emph = 0;
  config.mpeg.crc  = 0;
  config.mpeg.ext  = 0;
  config.mpeg.mode_ext  = 0;
  config.mpeg.copyright = 0;
  config.mpeg.original  = 1;
  config.mpeg.channels = 2;
  config.mpeg.granules = 2;
  cutoff = 418; /* 16KHz @ 44.1Ksps */
  config.wave.samplerate = 44100;
}

/*
 * parse_command:
 * --------------
 */
static int parse_command(int argc, char **argv, int *raw, int *mono_from_stereo)
{
  int i = 0, x;

  if(argc<3) return 0;

  while(argv[++i][0]=='-')
    switch(argv[i][1])
    {
      case 'b':
        config.mpeg.bitr = atoi(argv[++i]);
        break;

      case 'c':
        config.mpeg.copyright = 1;
        break;

      case 'o':
        config.mpeg.original = 0;
        break;

      case 'r':
        *raw = 1;
        x = atoi(argv[i+1]);
        if(x > 7999)
        {
          i++;
          config.wave.samplerate = x;
        }
        break;

      case 'm':
        *mono_from_stereo = 1;
        break;

      default :
        return 0;
    }

  if((argc-i)!=2) return 0;
  config.infile  = argv[i++];
  config.outfile = argv[i];
  return 1;
}

/*
 * find_samplerate_index:
 * ----------------------
 */
static int find_samplerate_index(int freq)
{
  static int sr[4][3] = {{11025, 12000,  8000},   /* mpeg 2.5 */
                          {    0,     0,     0},   /* reserved */
                          {22050, 24000, 16000},   /* mpeg 2 */
                          {44100, 48000, 32000}};  /* mpeg 1 */
  int i, j;

  for(j=0; j<4; j++)
    for(i=0; i<3; i++)
      if((freq == sr[j][i]) && (j != 1))
      {
        config.mpeg.type = j;
        return i;
      }

  error("Invalid samplerate");
  return 0;
}

/*
 * find_bitrate_index:
 * -------------------
 */
static int find_bitrate_index(int bitr)
{
  static int br[2][15] =
    {{0, 8,16,24,32,40,48,56, 64, 80, 96,112,128,144,160},   /* mpeg 2/2.5 */
     {0,32,40,48,56,64,80,96,112,128,160,192,224,256,320}};  /* mpeg 1 */
  int i;

  for(i=1; i<15; i++)
    if(bitr==br[config.mpeg.type & 1][i]) return i;

  error("Invalid bitrate");
  return 0;
}

int set_cutoff(void)
{
  static int cutoff_tab[3][2][15] =
  {
    { /* 44.1k, 22.05k, 11.025k */
      {100,104,131,157,183,209,261,313,365,418,418,418,418,418,418}, /* stereo */
      {183,209,261,313,365,418,418,418,418,418,418,418,418,418,418}  /* mono */
    },
    { /* 48k, 24k, 12k */
      {100,104,131,157,183,209,261,313,384,384,384,384,384,384,384}, /* stereo */
      {183,209,261,313,365,384,384,384,384,384,384,384,384,384,384}  /* mono */
    },
    { /* 32k, 16k, 8k */
      {100,104,131,157,183,209,261,313,365,418,522,576,576,576,576}, /* stereo */
      {183,209,261,313,365,418,522,576,576,576,576,576,576,576,576}  /* mono */
    }
  };

  return cutoff_tab[config.mpeg.samplerate_index]
                   [config.mpeg.mode == MODE_MONO]
                   [config.mpeg.bitrate_index];
}

/*
 * check_config:
 * -------------
 */
static void check_config()
{
  static char *mode_names[4]    = { "stereo", "j-stereo", "dual-ch", "mono" };
  static char *layer_names[4]   = { "", "III", "II", "I" };
  static char *version_names[4] = { "MPEG 2.5", "", "MPEG 2", "MPEG 1" };
  static char *psy_names[3]     = { "none", "MUSICAM", "Shine" };
  static char *demp_names[4]    = { "none", "50/15us", "", "CITT" };

  config.mpeg.samplerate_index = find_samplerate_index(config.wave.samplerate);
  config.mpeg.bitrate_index    = find_bitrate_index(config.mpeg.bitr);
  cutoff = set_cutoff();

  printf("%s layer %s, %s  Psychoacoustic Model: %s\n",
           version_names[config.mpeg.type],
           layer_names[config.mpeg.layr],
           mode_names[config.mpeg.mode],
           psy_names[config.mpeg.psyc] );
  printf("Bitrate=%d kbps  ",config.mpeg.bitr );
  printf("De-emphasis: %s   %s %s\n",
           demp_names[config.mpeg.emph],
           (config.mpeg.original) ? "Original" : "",
           (config.mpeg.copyright) ? "(C)" : "" );
}

/*
 * main:
 * -----
 */
__declspec (dllexport) int WavToMp3(const char* intfile, const char* outfile)
{
	time_t end_time;
	int raw = 0;
	int mono_from_stereo = 0;

	//argc=3;
	//argv[1]="2.wav";
	//argv[2]="1111.mp3";

	time(&config.start_time);
	set_defaults();
	//config.infile  = argv[i++];
	//if (!parse_command(argc, argv, &raw, &mono_from_stereo))
	//  print_usage();
	config.infile = intfile;
	config.outfile = outfile;

	wave_open(raw, mono_from_stereo); /* prints wave (input) configuration */

	check_config(); /* prints mpeg (output) configuration */

	printf("Encoding \"%s\" to \"%s\"\n", config.infile, config.outfile);

	L3_compress();

	wave_close();

	time(&end_time);
	end_time -= config.start_time;
	printf(" Finished in %2d:%2d:%2d\n",
		end_time / 3600, (end_time / 60) % 60, end_time % 60);

	return 0;
}
