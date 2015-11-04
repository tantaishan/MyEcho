/*
 * coder.c
 *
 * 22/02/01
 * Calculation of coefficient tables for sub band windowing
 * analysis filters and mdct.
 */

#include "types.h"
#include "table1.h"
#include "utils.h"

int  x[2][HAN_SIZE],z[512];


/*
 * L3_window_filter_subband:
 * -------------------------
 * Overlapping window on PCM samples
 * 32 16-bit pcm samples are scaled to fractional 2's complement and
 * concatenated to the end of the window buffer #x#. The updated window
 * buffer #x# is then windowed by the analysis window #enwindow# to produce
 * the windowed sample #z#
 * The windowed samples #z# is filtered by the digital filter matrix #filter#
 * to produce the subband samples #s#. This done by first selectively
 * picking out values from the windowed samples, and then multiplying
 * them by the filter matrix, producing 32 subband samples.
 */
void L3_window_filter_subband(short **buffer, int s[SBLIMIT] , int k)
{
  static int off[2]= {0,0};
  int y[64],s1,s2;
  int i,j;

  /* replace 32 oldest samples with 32 new samples */
    for (i=0;i<32;i++) 
		x[k][31-i+off[k]] =( *(*buffer)++)<<16;

    /* shift samples into proper window positions */
    for (i=0;i<HAN_SIZE;i++) 
		z[i] = mul(x[k][(i+off[k])&(HAN_SIZE-1)],ew[i]);

    off[k] += 480;              /* offset is modulo (HAN_SIZE)*/
    off[k] &= HAN_SIZE-1;


  /* sub sample the windowed data */
  for (i=64; i--; )
    for (j=8, y[i] = 0; j--; )
      y[i] += z[i+(j<<6)];

  /* combine sub samples for the simplified matrix calculation */
  for (i=0; i<16; i++)
    y[i+17] += y[15-i];
  for (i=0; i<15; i++)
    y[i+33] -= y[63-i];

  /* simlplified polyphase filter matrix multiplication */
  for (i=16; i--; )
    for (j=0, s[i]= 0, s[31-i]=0; j<32; j += 2)
    {
      s1 = mul(fl[i][j],y[j+16]);
      s2 = mul(fl[i][j+1],y[j+17]);
      s[i] += s1 + s2;
      s[31-i] += s1 - s2;
    }
}

/*
 * L3_mdct_sub:
 * ------------
 */
void L3_mdct_sub(int sb_sample[2][3][18][SBLIMIT],
                 int mdct_freq[2][2][samp_per_frame2])
{
  /* note. we wish to access the array 'mdct_freq[2][2][576]' as
   * [2][2][32][18]. (32*18=576),
   */
  int (*mdct_enc)[18];

  int  ch,gr,band,j,k;
  int mdct_in[36];
  int bu,bd,*m;

  for(gr=0; gr<config.mpeg.granules; gr++)
    for(ch=config.mpeg.channels; ch--; )
    {
      /* set up pointer to the part of mdct_freq we're using */
      mdct_enc = (int (*)[18]) mdct_freq[gr][ch];

      /* Compensate for inversion in the analysis filter
       * (every odd index of band AND k)
       */
      for(band=1; band<=31; band+=2 )
        for(k=1; k<=17; k+=2 )
          sb_sample[ch][gr+1][k][band] *= -1;

      /* Perform imdct of 18 previous subband samples + 18 current subband samples */
      for(band=32; band--; )
      {
        for(k=18; k--; )
        {
          mdct_in[k]    = sb_sample[ch][ gr ][k][band];
          mdct_in[k+18] = sb_sample[ch][gr+1][k][band];
        }

        /* Calculation of the MDCT
         * In the case of long blocks ( block_type 0,1,3 ) there are
         * 36 coefficients in the time domain and 18 in the frequency
         * domain.
         */
        for(k=18; k--; )
        {
          m = &mdct_enc[band][k];
          for(j=36, *m=0; j--; )
            *m += mul(mdct_in[j],cos_l[k][j]);
        }
      }

      /* Perform aliasing reduction butterfly */
      for(band=31; band--; )
        for(k=8; k--; )
        {
          /* must left justify result of multiplication here because the centre
           * two values in each block are not touched.
           */
          bu = muls(mdct_enc[band][17-k],cs[k]) + muls(mdct_enc[band+1][k],ca[k]);
          bd = muls(mdct_enc[band+1][k],cs[k]) - muls(mdct_enc[band][17-k],ca[k]);
          mdct_enc[band][17-k] = bu;
          mdct_enc[band+1][k]  = bd;
        }
    }

  /* Save latest granule's subband samples to be used in the next mdct call */
  for(ch=config.mpeg.channels ;ch--; )
    for(j=18; j--; )
      for(band=32; band--; )
        sb_sample[ch][0][j][band] = sb_sample[ch][config.mpeg.granules][j][band];
}



