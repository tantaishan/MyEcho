/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*      File             : copy.h
*
********************************************************************************
*/
 
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "copy.h"
const char copy_id[] = "@(#)$Id $" copy_h;

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "count.h"

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
 *
 *   FUNCTION:   Copy
 *
 *   PURPOSE:   Copy vector x[] to y[]
 *
 *
 *************************************************************************/
/*
**************************************************************************
*
*  Function    : Copy
*  Purpose     : Copy vector x[] to y[]
*
**************************************************************************
*/
void Copy (
    const Word16 x[],   /* i : input vector (L)  */
    Word16 y[],         /* o : output vector (L) */
    Word16 L            /* i : vector length     */
)
{
    Word16 i;

    for (i = 0; i < L; i++)
    {
        y[i] = x[i];            move16 (); 
    }

    return;
}