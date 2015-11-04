/* wave.c
 *
 * Data input functions.
 */

#include "types.h"

/*
 * wave_close:
 * -----------
 */
void wave_close(void)
{
  fclose(config.wave.file);
}

/*
 * wave_open:
 * ----------
 * Opens and verifies the header of the Input Wave file. The file pointer is
 * left pointing to the start of the samples.
 */
void wave_open(int raw, int mono_from_stereo)
{

  static char *channel_mappings[] = {NULL,"mono","stereo"};
  static char *file_type[] = {"RIFF-WAVE PCM","RAW DATA PCM"};

  /* Wave file headers can vary from this, but we're only intereseted in this format */
  struct wave_header
  {
    char riff[4];             /* "RIFF" */
    unsigned int size;       /* length of rest of file = size of rest of header(36) + data length */
    char wave[4];             /* "WAVE" */
    char fmt[4];              /* "fmt " */
    unsigned int fmt_len;    /* length of rest of fmt chunk = 16 */
    unsigned short tag;       /* MS PCM = 1 */
    unsigned short channels;  /* channels, mono = 1, stereo = 2 */
    unsigned int samp_rate;  /* samples per second = 44100 */
    unsigned int byte_rate;  /* bytes per second = samp_rate * byte_samp = 176400 */
    unsigned short byte_samp; /* block align (bytes per sample) = channels * bits_per_sample / 8 = 4 */
    unsigned short bit_samp;  /* bits per sample = 16 for MS PCM (format specific) */
    char data[4];             /* "data" */
    unsigned int length;     /* data length (bytes) */
  } header;

  /* open input file */
  if((config.wave.file = fopen(config.infile,"rb")) == NULL)
    error("Unable to open file");

  if(raw)
  {
      fseek(config.wave.file, 0, SEEK_END); /* find length of file */
      header.length = ftell(config.wave.file);
      fseek(config.wave.file, 0, SEEK_SET);
      header.channels = (mono_from_stereo) ? 1 : 2;
      header.samp_rate = config.wave.samplerate; /* 44100 if not set by user */
      header.bit_samp = 16;     /* assumed */
      header.byte_samp = header.channels * header.bit_samp / 8;
      header.byte_rate = header.samp_rate * header.byte_samp;
  }
  else /* wave */
  {
      if (fread(&header, sizeof(header), 1, config.wave.file) != 1)
        error("Invalid Header");
      if(strncmp(header.riff,"RIFF",4) != 0) error("Not a MS-RIFF file");
      if(strncmp(header.wave,"WAVE",4) != 0) error("Not a WAVE audio");
      if(strncmp(header.fmt, "fmt ",4) != 0) error("Can't find format chunk");
      if(header.tag != 1)                    error("Unknown WAVE format");
      if(header.channels > 2)                error("More than 2 channels");
      if(header.bit_samp != 16)              error("Not 16 bit");
      //if(strncmp(header.data,"data",4) != 0) error("Can't find data chunk");
  }

  config.wave.channels      = header.channels;
  config.wave.samplerate    = header.samp_rate;
  config.wave.bits          = header.bit_samp;
  config.wave.total_samples = header.length / header.byte_samp;
  config.wave.length        = header.length / header.byte_rate;

  if((config.wave.channels == 1) || mono_from_stereo)
  {
    config.mpeg.channels = 1;
    config.mpeg.mode = MODE_MONO;
  }

  printf("%s, %s %dHz %dbit, Length: %2d:%2d:%2d\n",
          file_type[raw],
          channel_mappings[config.wave.channels],
          config.wave.samplerate,
          config.wave.bits,
          config.wave.length/3600,(config.wave.length/60)%60,config.wave.length%60);
}

/*
 * wave_get:
 * ---------
 * Reads samples from the file in longs. A long can
 * hold one stereo or two mono samples.
 * Returns the address of the start of the next frame or NULL if there are no
 * more. The last frame will be padded with zero's.
 */
int wave_get(short buffer[2][1152])
{
  int j,p;
  static short temp_buf[2304];

  p = fread(temp_buf,sizeof(short),2304,config.wave.file);
  for(j=0;j<1152;j++)
  {
    buffer[0][j] = temp_buf[2*j];
    buffer[1][j] = temp_buf[2*j+1];
  }
    return p;
}


