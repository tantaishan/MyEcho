/*
 * ===================================================================
 *  TS 26.104
 *  R99   V3.5.0 2003-03
 *  REL-4 V4.4.0 2003-03
 *  REL-5 V5.1.0 2003-03
 *  3GPP AMR Floating-point Speech Codec
 * ===================================================================
 *
 */

/*
 * interf_enc.h
 *
 *
 * Project:
 *    AMR Floating-Point Codec
 *
 * Contains:
 *    Defines interface to AMR encoder
 *
 */
#pragma once

/*
 * include files
 */
#include"sp_enc.h"

/*
 * Function prototypes
 */
/*
 * Encodes one frame of speech
 * Returns packed octets
 */
/*
#ifdef __cplusplus
   extern "C" {
#endif*/

	void print_hello();
#ifdef __cplusplus
/*extern __declspec(dllexport)*/ int Encoder_Interface_Encode( void *st, enum Mode mode, short *speech,

#ifndef ETSI
      unsigned char *serial,  /* max size 31 bytes */

#else
      short *serial, /* size 500 bytes */
#endif

      int forceSpeech );   /* use speech mode */

/*
 * Reserve and init. memory
 */
/*extern __declspec(dllexport)*/ void *Encoder_Interface_init( int dtx );

/*
 * Exit and free memory
 */
/*extern __declspec(dllexport)*/ void Encoder_Interface_exit( void *state );


#endif
/*
#ifdef __cplusplus
}
#endif*/


