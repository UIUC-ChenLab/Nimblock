/*===============================================================*/
/*                                                               */
/*                         rendering.h                           */
/*                                                               */
/*                 C++ kernel for 3D Rendering                   */
/*                                                               */
/*===============================================================*/

#ifndef __RENDERING_H__
#define __RENDERING_H__

#include "3dr_typedefs.h"

#pragma SDS data access_pattern(input:SEQUENTIAL, output:SEQUENTIAL)
void rendering_modular(bit32 input[3*NUM_3D_TRI], bit32 output[NUM_FB]);
int bundle1_module (
	bit32 input[3*NUM_3D_TRI],
	//Triangle_2D triangle_2d[NUM_3D_TRI],
	CandidatePixel fragment2[NUM_3D_TRI][500],
	bit16 size_fragment[NUM_3D_TRI] );

int zculling_module (
  //bit16 counter[NUM_3D_TRI],
  CandidatePixel fragments[NUM_3D_TRI][500],
  bit16 size[NUM_3D_TRI],
  Pixel pixels[NUM_3D_TRI][500],
  bit16 size_pixels[NUM_3D_TRI]);

int bundle2_module(
  bit16 size_pixels[NUM_3D_TRI],
  Pixel pixels[NUM_3D_TRI][500],
  bit32 output[NUM_FB]);
#endif

