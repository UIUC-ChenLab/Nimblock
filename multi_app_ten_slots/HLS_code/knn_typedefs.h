/*===============================================================*/
/*                                                               */
/*                          typedefs.h                           */
/*                                                               */
/*           Constant definitions and typedefs for host.         */
/*                                                               */
/*===============================================================*/

#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

// dataset information
#define NUM_TRAINING 18000
#define CLASS_SIZE 1800
#define NUM_TEST 2000
#define DIGIT_WIDTH 4
#define PR

// typedefs
typedef unsigned long long DigitType;
typedef unsigned char      LabelType;

#ifdef SDSOC
  #include "ap_int.h"
  // sdsoc wide vector type
  typedef ap_uint<256>  WholeDigitType;
#endif

#ifdef PR
  #include "ap_int.h"
  // sdsoc wide vector type
  typedef ap_uint<256>  WholeDigitType;
#endif

// parameters
#define K_CONST 3
#define PAR_FACTOR 10

#endif
