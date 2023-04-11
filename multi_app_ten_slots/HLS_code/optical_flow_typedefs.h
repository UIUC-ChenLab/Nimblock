/*===============================================================*/
/*                                                               */
/*                          kernel.h                             */
/*                                                               */
/*        Defines types and constants for host function          */
/*                                                               */
/*===============================================================*/

#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__
//#include "ap_fixed.h"
const int MAX_HEIGHT = 436;
const int MAX_WIDTH = 256;

#define SDSOC

// basic typedefs
#ifdef SDSOC
	#include "ap_fixed.h"
	//typedef ap_fixed<17,9> input_t;
	typedef ap_fixed<16,8> input_t;
	typedef ap_fixed<32,13> pixel_t;
	typedef ap_fixed<32,11> outer_pixel_t;
	//typedef ap_fixed<48,27> outer_pixel_t;
	typedef ap_fixed<64,24> calc_pixel_t;
	//typedef ap_fixed<96,56> calc_pixel_t;
	typedef ap_fixed<16,8> vel_pixel_t;
	//typedef ap_fixed<32,13> vel_pixel_t;
	//typedef ap_fixed<16,8> input_t;
        //typedef ap_fixed<32,13> pixel_t;
        //typedef float outer_pixel_t;
	//typedef float calc_pixel_t;
	//typedef float vel_pixel_t;
	
#endif
#ifdef OCL
	#include "ap_fixed.h"
	typedef ap_fixed<48,40> pixel_t;
#endif
#ifdef SW
	typedef float pixel_t;
#endif
typedef struct{
	pixel_t x;
	pixel_t y;
	pixel_t z;
	pixel_t pad; //add a padding here to make the compiler happy
}gradient_t;

typedef struct{
    outer_pixel_t val[6];
}outer_t; 

typedef struct{
    outer_pixel_t val[6];
}tensor_t;

typedef struct{
    vel_pixel_t x;
    vel_pixel_t y;
}velocity_t;

#ifndef SW
  #include "ap_int.h"
  // for data packing
  typedef ap_uint<64> frames_t;
#endif

#ifdef OCL
  #include <string>
  // change the target device here
  const std::string TARGET_DEVICE = "xilinx:aws-vu9p-f1:4ddr-xpr-2pr:4.0";
#endif

#endif
