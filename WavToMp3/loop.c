/* loop.c */

#include "types.h"
#include "table2.h"
#include "utils.h"

//#define EXCESS_TO_PART3

#define PART_2_3_LIMIT ((1<<12)-1)     /* 12 bit part_2_3_length */
#define DEC_BUFF_LIMIT 7680            /* decoder buffer limit */

extern int *scalefac_band_long;
extern int cutoff;

int count_bit(int ix[samp_per_frame2], unsigned int start, unsigned int end, unsigned int table );
int bigv_bitcount(int ix[samp_per_frame2], gr_info *gi);
int new_choose_table( int ix[samp_per_frame2], unsigned int begin, unsigned int end );
void bigv_tab_select( int ix[samp_per_frame2], gr_info *gi );
void subdivide(gr_info *gi);
int count1_bitcount( int ix[ samp_per_frame2 ], gr_info *gi );
void calc_runlen( int ix[samp_per_frame2], gr_info *gi );
int quantize(int ix[samp_per_frame2], int stepsize);
int ix_max( int ix[samp_per_frame2], unsigned int begin, unsigned int end );

static int
  *xr,                    /* magnitudes of the spectral values */
  xrabs[samp_per_frame2], /* xr absolute */
  xrmax;                  /* maximum of xrabs array */

/*
 * inner_loop:
 * ----------
 * Selects the quantizer stepsize that allows encoding of the entire
 * spectrum with as many bits as possible up to the maximum allowed
 * of max_bits. Bits used must never exceed max_bits.
 * The lower the value of stepsize, the more bits are required.
 * Each increment of stepsize is an increase of global gain by 1.5dB (2**0.25).
 * The bit calculation functions are executed 7 or 8 times per call.
 */
static int inner_loop(int ix[samp_per_frame2],int max_bits,gr_info *gi)
{
  int step, stepsize, bits;

  step = stepsize = 64;

  /* This loop ends within 1 step of max_bits.
   * It takes a fixed 7 times around because the tablesize is 128=2**7.
   */
  while(step)
  {
    if(!quantize(ix,stepsize))
      bits = 0x7fffffff;     /* outside (our) table range, early fail exit */
    else
    {
      calc_runlen(ix,gi);              /* rzero,count1,big_values */
      bits = count1_bitcount(ix,gi);   /* count1_table selection */
      subdivide(gi);                   /* bigvalues sfb division */
      bigv_tab_select(ix,gi);          /* codebook selection */
      bits += bigv_bitcount(ix,gi);    /* bit count */
    }

    step >>= 1;

    if(bits > max_bits)
      stepsize += step;
    else
      stepsize -= step;
  };

  /* this loop takes it over to the right side of max_bits.
   * it usually only has to go through once, sometimes not
   * at all. The while is just for safety.
   */
  while(bits > max_bits)
  {
    quantize(ix,++stepsize);         /* update ix */
    calc_runlen(ix,gi);              /* rzero,count1,big_values */
    bits = count1_bitcount(ix,gi);   /* count1_table selection */
    subdivide(gi);                   /* bigvalues sfb division */
    bigv_tab_select(ix,gi);          /* codebook selection */
    bits += bigv_bitcount(ix,gi);    /* bit count */
  }

  gi->quantizerStepSize = stepsize;

  return bits;
}

/*
 * L3_iteration_loop:
 * ------------------
 */

void L3_iteration_loop(int            mdct_freq_org[2][2][samp_per_frame2],
                       L3_side_info_t *side_info,
                       int             l3_enc[2][2][samp_per_frame2],
                       int             mean_bits)
{
  gr_info *gi;
  int ch, gr, i;
  int *ix;
  int max_bits;
  int extra_bits;
  int reservoir;
  int resv_max;
  static int main_data_begin=0;

  reservoir = main_data_begin << 3;  /* calculate reservoir at the frame start */
  side_info->main_data_begin = main_data_begin; /* set next frames back pointer */

  for(gr=0; gr<config.mpeg.granules; gr++)
    for(ch=config.mpeg.channels; ch--; )
    {
      /* setup pointers */
      ix = l3_enc[gr][ch];
      xr = mdct_freq_org[gr][ch];
      gi = &side_info->gr[gr].ch[ch].tt;

      /* calculate absolute and maximum */
      for (i=cutoff, xrmax=0; i--;)
        if(xrmax < (xrabs[i] = abs(xr[i])))
          xrmax = xrabs[i];

      gi->part2_3_length    = 0;
      gi->big_values        = 0;
      gi->count1            = 0;
      gi->table_select[0]   = 0;
      gi->table_select[1]   = 0;
      gi->table_select[2]   = 0;
      gi->region0_count     = 0;
      gi->region1_count     = 0;
      gi->count1table_select= 0;

      /* calculate the available bits for the main data */
      extra_bits = reservoir >> 1; /* simple scheme, give half each time */
      reservoir -= extra_bits;
      max_bits = mean_bits + extra_bits;
      if(max_bits > PART_2_3_LIMIT)
      {
        reservoir += max_bits - PART_2_3_LIMIT;
        max_bits = PART_2_3_LIMIT;
      }

      /* quantize the spectrum unless all spectral values zero */
      if(xrmax)
        gi->part2_3_length = inner_loop(ix, max_bits, gi);

      reservoir += max_bits - gi->part2_3_length;  /* adjust for unused bits */

      gi->global_gain = gi->quantizerStepSize + 89;  /* StepSize = 0..127 (1.5dB steps) */

      /* restore sign of quantized spectral values */
      for ( i = 0; i < cutoff; i++ )
        if (xr[i] < 0)
          ix[i] *= -1;
    }

  /* Sort out reservoir at frame end, limit the size and stuff the excess */
  resv_max = DEC_BUFF_LIMIT - config.mpeg.bits_per_frame;
  if(resv_max > config.mpeg.resv_limit)
    resv_max = config.mpeg.resv_limit;
  else if(resv_max < 0)
    resv_max = 0;
  main_data_begin = (reservoir < resv_max) ? (reservoir >> 3) : (resv_max >> 3);
  extra_bits = reservoir - (main_data_begin << 3);

#ifdef EXCESS_TO_PART3
  /* distribute excess bits throughout granules/channels */
  for(gr=0; gr<config.mpeg.granules; gr++)
    for(ch=0; ch<config.mpeg.channels; ch++)
    {
      int spare;
      if (!extra_bits)
        break;
      gi = &side_info->gr[gr].ch[ch].tt;
      spare = PART_2_3_LIMIT - gi->part2_3_length;
      if(spare > extra_bits)
        spare = extra_bits;
      gi->part2_3_length += spare;
      extra_bits -= spare;
    }
#endif

  side_info->resv_drain = extra_bits; /* remaining bits to ancillary data */
}

/*
 * quantize:
 * ---------
 * Function: Quantization of the vector xr ( -> ix).
 * Returns 1 if ixmax < 1000. (our table size)
 */
int quantize(int ix[samp_per_frame2], int stepsize )
{
  /* the -1 together with the 3 bit table shift gives a 4 bit shift.
   * This compensates for using mulr instead of mulsr. The 1 bit shift
   * is equal to a four bit shift in the fourth power of two table.
   */
  int i;
  int scale = pow2_4[stepsize-1]; /* 2**(-stepsize/4) */

  /* A quick check to see if ixmax will be less than 1000 (our table
   * size is 10000) this speeds up the early calls
   */
  if(mul(xrmax, scale) > 9999)
    return 0; /* no point in continuing, stepsize not big enough */

  for(i=0;i<cutoff;i++)
    /* ix = (|xr| * 2**(-stepsize/4)) ** (3/4)
     * The multiply must round it's result to emulate the 'nearest int'
     * (nint) function in the spec.
     */
    ix[i] = pow3_4[mul(xrabs[i], scale)];

  return 1;
 }

/*
 * ix_max:
 * -------
 * Function: Calculate the maximum of ix from 'begin' to 'end-1'
 */
int ix_max( int ix[samp_per_frame2], unsigned int begin, unsigned int end )
{
  register int i;
  register int max = 0;

  for(i=begin;i<end;i++)
    if(max < ix[i])
      max = ix[i];
  return max;
}

/*
 * calc_runlen:
 * ------------
 * Function: Calculation of rzero, count1, big_values
 * (Partitions ix into big values, quadruples and zeros).
 */
void calc_runlen( int ix[samp_per_frame2], gr_info *gi )
{
  int i;

  /* zeros */
  for ( i = cutoff; i > 1; i -= 2 )
    if (ix[i-1] | ix[i-2])
      break;

  /* quadruples */
  gi->count1 = 0 ;
  for ( ; i > 3; i -= 4 )
    if (   ix[i-1] <= 1
        && ix[i-2] <= 1
        && ix[i-3] <= 1
        && ix[i-4] <= 1 )
      gi->count1++;
    else
      break;

  /* what's left are big values */
  gi->big_values = i>>1;
}

/*
 * count1_bitcount:
 * ----------------
 * Determines the number of bits to encode the quadruples.
 */
int count1_bitcount(int ix[samp_per_frame2], gr_info *gi)
{
  int p, i, k;
  int v, w, x, y, signbits;
  int sum0 = 0,
      sum1 = 0;

  for(i=gi->big_values<<1, k=0; k<gi->count1; i+=4, k++)
  {
    v = ix[i];
    w = ix[i+1];
    x = ix[i+2];
    y = ix[i+3];

    p = v + (w<<1) + (x<<2) + (y<<3);

    signbits = 0;
    if(v!=0) signbits++;
    if(w!=0) signbits++;
    if(x!=0) signbits++;
    if(y!=0) signbits++;

    sum0 += signbits;
    sum1 += signbits;

    sum0 += ht[32].hlen[p];
    sum1 += ht[33].hlen[p];
  }

  if(sum0<sum1)
  {
    gi->count1table_select = 0;
    return sum0;
  }
  else
  {
    gi->count1table_select = 1;
    return sum1;
  }
}

/*
 * subdivide:
 * ----------
 * presumable subdivides the bigvalue region which will use separate Huffman tables.
 */
void subdivide(gr_info *gi)
{
  static struct
  {
    unsigned region0_count;
    unsigned region1_count;
  } subdv_table[ 23 ] =
  {
    {0, 0}, /* 0 bands */
    {0, 0}, /* 1 bands */
    {0, 0}, /* 2 bands */
    {0, 0}, /* 3 bands */
    {0, 0}, /* 4 bands */
    {0, 1}, /* 5 bands */
    {1, 1}, /* 6 bands */
    {1, 1}, /* 7 bands */
    {1, 2}, /* 8 bands */
    {2, 2}, /* 9 bands */
    {2, 3}, /* 10 bands */
    {2, 3}, /* 11 bands */
    {3, 4}, /* 12 bands */
    {3, 4}, /* 13 bands */
    {3, 4}, /* 14 bands */
    {4, 5}, /* 15 bands */
    {4, 5}, /* 16 bands */
    {4, 6}, /* 17 bands */
    {5, 6}, /* 18 bands */
    {5, 6}, /* 19 bands */
    {5, 7}, /* 20 bands */
    {6, 7}, /* 21 bands */
    {6, 7}, /* 22 bands */
  };

  int scfb_anz = 0;
  int bigvalues_region;

  if ( !gi->big_values)
  { /* no big_values region */
    gi->region0_count = 0;
    gi->region1_count = 0;
  }
  else
  {
    bigvalues_region = 2 * gi->big_values;
    {
      int thiscount, index;
      /* Calculate scfb_anz */
      while ( scalefac_band_long[scfb_anz] < bigvalues_region )
        scfb_anz++;

      gi->region0_count = subdv_table[scfb_anz].region0_count;
      thiscount = gi->region0_count;
      index = thiscount + 1;
      while ( thiscount && (scalefac_band_long[index] > bigvalues_region) )
      {
        thiscount--;
        index--;
      }
      gi->region0_count = thiscount;

      gi->region1_count = subdv_table[scfb_anz].region1_count;
      index = gi->region0_count + gi->region1_count + 2;
      thiscount = gi->region1_count;
      while ( thiscount && (scalefac_band_long[index] > bigvalues_region) )
      {
        thiscount--;
        index--;
      }
      gi->region1_count = thiscount;
      gi->address1 = scalefac_band_long[gi->region0_count+1];
      gi->address2 = scalefac_band_long[gi->region0_count
                                            + gi->region1_count + 2 ];
      gi->address3 = bigvalues_region;
    }
  }
}

/*
 * bigv_tab_select:
 * ----------------
 * Function: Select huffman code tables for bigvalues regions
 */
void bigv_tab_select( int ix[samp_per_frame2], gr_info *gi )
{
  gi->table_select[0] = 0;
  gi->table_select[1] = 0;
  gi->table_select[2] = 0;

  if ( gi->address1 > 0 )
    gi->table_select[0] = new_choose_table( ix, 0, gi->address1 );

  if ( gi->address2 > gi->address1 )
    gi->table_select[1] = new_choose_table( ix, gi->address1, gi->address2 );

  if ( gi->big_values<<1 > gi->address2 )
    gi->table_select[2] = new_choose_table( ix, gi->address2, gi->big_values<<1 );
}

/*
 * new_choose_table:
 * -----------------
 * Choose the Huffman table that will encode ix[begin..end] with
 * the fewest bits.
 * Note: This code contains knowledge about the sizes and characteristics
 * of the Huffman tables as defined in the IS (Table B.7), and will not work
 * with any arbitrary tables.
 */
int new_choose_table( int ix[samp_per_frame2], unsigned int begin, unsigned int end )
{
  int i, max;
  int choice[2];
  int sum[2];

  max = ix_max(ix,begin,end);
  if(!max)
    return 0;

  choice[0] = 0;
  choice[1] = 0;

  if(max<15)
  {
    /* try tables with no linbits */
    for ( i =14; i--; )
      if ( ht[i].xlen > max )
      {
        choice[0] = i;
        break;
      }

    sum[0] = count_bit( ix, begin, end, choice[0] );

    switch (choice[0])
    {
      case 2:
        sum[1] = count_bit( ix, begin, end, 3 );
        if ( sum[1] <= sum[0] )
          choice[0] = 3;
        break;

      case 5:
        sum[1] = count_bit( ix, begin, end, 6 );
        if ( sum[1] <= sum[0] )
          choice[0] = 6;
        break;

      case 7:
        sum[1] = count_bit( ix, begin, end, 8 );
        if ( sum[1] <= sum[0] )
        {
          choice[0] = 8;
          sum[0] = sum[1];
        }
        sum[1] = count_bit( ix, begin, end, 9 );
        if ( sum[1] <= sum[0] )
          choice[0] = 9;
        break;

      case 10:
        sum[1] = count_bit( ix, begin, end, 11 );
        if ( sum[1] <= sum[0] )
        {
          choice[0] = 11;
          sum[0] = sum[1];
        }
        sum[1] = count_bit( ix, begin, end, 12 );
        if ( sum[1] <= sum[0] )
          choice[0] = 12;
        break;

      case 13:
        sum[1] = count_bit( ix, begin, end, 15 );
        if ( sum[1] <= sum[0] )
          choice[0] = 15;
        break;
    }
  }
  else
  {
    /* try tables with linbits */
    max -= 15;

    for(i=15;i<24;i++)
      if(ht[i].linmax>=max)
      {
        choice[0] = i;
        break;
      }

    for(i=24;i<32;i++)
      if(ht[i].linmax>=max)
      {
        choice[1] = i;
        break;
      }

    sum[0] = count_bit(ix,begin,end,choice[0]);
    sum[1] = count_bit(ix,begin,end,choice[1]);
    if (sum[1]<sum[0])
      choice[0] = choice[1];
  }
  return choice[0];
}

/*
 * bigv_bitcount:
 * --------------
 * Function: Count the number of bits necessary to code the bigvalues region.
 */
int bigv_bitcount(int ix[samp_per_frame2], gr_info *gi)
{
  int bits = 0;
  unsigned int table;

  if((table = gi->table_select[0]) != 0)  /* region0 */
    bits += count_bit(ix, 0, gi->address1, table );
  if((table = gi->table_select[1]) != 0)  /* region1 */
    bits += count_bit(ix, gi->address1, gi->address2, table );
  if((table = gi->table_select[2]) != 0)  /* region2 */
    bits += count_bit(ix, gi->address2, gi->address3, table );
  return bits;
}

/*
 * count_bit:
 * ----------
 * Function: Count the number of bits necessary to code the subregion.
 */
int count_bit(int ix[samp_per_frame2],
              unsigned int start,
              unsigned int end,
              unsigned int table )
{
  unsigned            linbits, ylen;
  register int        i, sum;
  register int        x,y;
  struct huffcodetab *h;

  if(!table)
    return 0;

  h   = &(ht[table]);
  sum = 0;

  ylen    = h->ylen;
  linbits = h->linbits;

  if(table>15)
  { /* ESC-table is used */
    for(i=start;i<end;i+=2)
    {
      x = ix[i];
      y = ix[i+1];
      if(x>14)
      {
        x = 15;
        sum += linbits;
      }
      if(y>14)
      {
        y = 15;
        sum += linbits;
      }

      sum += h->hlen[(x*ylen)+y];
      if(x)
        sum++;
      if(y)
        sum++;
    }
  }
  else
  { /* No ESC-words */
    for(i=start;i<end;i+=2)
    {
      x = ix[i];
      y = ix[i+1];

      sum  += h->hlen[(x*ylen)+y];

      if(x!=0)
        sum++;
      if(y!=0)
        sum++;
    }
  }
  return sum;
}

