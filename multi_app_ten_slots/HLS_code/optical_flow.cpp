/*===============================================================*/
/*                                                               */
/*                      optical_flow.cpp                         */
/*                                                               */
/*             Hardware function for optical flow                */
/*                                                               */
/*===============================================================*/

#include "optical_flow.h"
// use HLS video library
#include <hls_video.h>

// use HLS fixed point
#include "ap_fixed.h"
#include "ap_int.h"

int read_input_module(
		frames_t frames[MAX_HEIGHT][MAX_WIDTH],
		input_t frame1_a[MAX_HEIGHT][MAX_WIDTH],
		input_t frame2_a[MAX_HEIGHT][MAX_WIDTH],
		input_t frame3_a[MAX_HEIGHT][MAX_WIDTH],
		input_t frame3_b[MAX_HEIGHT][MAX_WIDTH],
		input_t frame4_a[MAX_HEIGHT][MAX_WIDTH],
		input_t frame5_a[MAX_HEIGHT][MAX_WIDTH]
){

#pragma HLS interface s_axilite port=return bundle=ctrl
#pragma HLS interface m_axi port=frames offset=slave bundle=data
#pragma HLS interface m_axi port=frame1_a offset=slave bundle=data
#pragma HLS interface m_axi port=frame2_a offset=slave bundle=data
#pragma HLS interface m_axi port=frame3_a offset=slave bundle=data
#pragma HLS interface m_axi port=frame3_b offset=slave bundle=data
#pragma HLS interface m_axi port=frame4_a offset=slave bundle=data
#pragma HLS interface m_axi port=frame5_a offset=slave bundle=data

	// stream in and organize the inputs
	  static frames_t buf;
	  for (int r=0; r<MAX_HEIGHT; r++)
	  {
	    for (int c=0; c<MAX_WIDTH; c++)
	    {
	      //#pragma HLS pipeline II=1
	      // one wide read
	      buf = frames[r][c];
	      // assign values to the FIFOs
	      frame1_a[r][c] = ((input_t)(buf(7 ,  0)) >> 8);
	      frame2_a[r][c] = ((input_t)(buf(15,  8)) >> 8);
	      frame3_a[r][c] = ((input_t)(buf(23, 16)) >> 8);
	      frame3_b[r][c] = ((input_t)(buf(23, 16)) >> 8);
	      frame4_a[r][c] = ((input_t)(buf(31, 24)) >> 8);
	      frame5_a[r][c] = ((input_t)(buf(39, 32)) >> 8);
	    }
	  }
	  return 0;
}


// define these constants so they can be used in pragma
const int max_width = MAX_WIDTH; 
const int default_depth = MAX_WIDTH;
int gradient_xy_calc_module(
	input_t frame[MAX_HEIGHT][MAX_WIDTH],
    pixel_t gradient_x[MAX_HEIGHT][MAX_WIDTH],
    pixel_t gradient_y[MAX_HEIGHT][MAX_WIDTH],
	ap_int<64> *dummy_port_1,
	ap_int<64> *dummy_port_2,
	ap_int<64> *dummy_port_3,
	ap_int<64> *dummy_port_4)
{

#pragma HLS interface s_axilite port=return bundle=ctrl
#pragma HLS interface m_axi port=frame offset=slave bundle=data
#pragma HLS interface m_axi port=gradient_x offset=slave bundle=data
#pragma HLS interface m_axi port=gradient_y offset=slave bundle=data
#pragma HLS interface m_axi port=dummy_port_1 offset=slave bundle=data
#pragma HLS interface m_axi port=dummy_port_2 offset=slave bundle=data
#pragma HLS interface m_axi port=dummy_port_3 offset=slave bundle=data
#pragma HLS interface m_axi port=dummy_port_4 offset=slave bundle=data

  // our own line buffer
  static pixel_t buf[5][MAX_WIDTH];
  #pragma HLS array_partition variable=buf complete dim=1

  // small buffer
  pixel_t smallbuf[5];
  #pragma HLS array_partition variable=smallbuf complete dim=0

  // window buffer
  hls::Window<5,5,input_t> window;

  const int GRAD_WEIGHTS[] =  {1,-8,0,8,-1};

  GRAD_XY_OUTER: for(int r=0; r<MAX_HEIGHT+2; r++)
  {
    GRAD_XY_INNER: for(int c=0; c<MAX_WIDTH+2; c++)
    {
      #pragma HLS pipeline II=1
      // read out values from current line buffer
      for (int i = 0; i < 4; i ++ )
        smallbuf[i] = buf[i+1][c];
      // the new value is either 0 or read from frame
      if (r<MAX_HEIGHT && c<MAX_WIDTH)
        smallbuf[4] = (pixel_t)(frame[r][c]);
      else if (c < MAX_WIDTH)
        smallbuf[4] = 0;
      // update line buffer
      if(r<MAX_HEIGHT && c<MAX_WIDTH)
      {
        for (int i = 0; i < 4; i ++ )
          buf[i][c] = smallbuf[i];
        buf[4][c] = smallbuf[4];
      }
      else if(c<MAX_WIDTH)
      {
        for (int i = 0; i < 4; i ++ )
          buf[i][c] = smallbuf[i];
        buf[4][c] = smallbuf[4];
      }

      // manage window buffer
      if(r<MAX_HEIGHT && c<MAX_WIDTH)
      {
        window.shift_pixels_left();

        for (int i = 0; i < 5; i ++ )
          window.insert_pixel(smallbuf[i],i,4);
      }
      else
      {
        window.shift_pixels_left();
        window.insert_pixel(0,0,4);
        window.insert_pixel(0,1,4);
        window.insert_pixel(0,2,4);
        window.insert_pixel(0,3,4);
        window.insert_pixel(0,4,4);
      }

      // compute gradient
      pixel_t x_grad = 0;
      pixel_t y_grad = 0;
      if(r>=4 && r<MAX_HEIGHT && c>=4 && c<MAX_WIDTH)
      {
        GRAD_XY_XYGRAD: for(int i=0; i<5; i++)
        {
          x_grad += window.getval(2,i)*GRAD_WEIGHTS[i];
          y_grad += window.getval(i,2)*GRAD_WEIGHTS[i];
        }
        gradient_x[r-2][c-2] = x_grad/12;
        gradient_y[r-2][c-2] = y_grad/12;
      }
      else if(r>=2 && c>=2)
      {
        gradient_x[r-2][c-2] = 0;
        gradient_y[r-2][c-2] = 0;
      }
    }
  }
  return 0;
}

int gradient_z_calc_module(input_t frame1[MAX_HEIGHT][MAX_WIDTH],
    input_t frame2[MAX_HEIGHT][MAX_WIDTH],
    input_t frame3[MAX_HEIGHT][MAX_WIDTH],
    input_t frame4[MAX_HEIGHT][MAX_WIDTH],
    input_t frame5[MAX_HEIGHT][MAX_WIDTH],
    pixel_t gradient_z[MAX_HEIGHT][MAX_WIDTH],
	ap_int<64> *dummy_port_1,
	ap_int<64> *dummy_port_2)
{
#pragma HLS interface s_axilite port=return bundle=ctrl
#pragma HLS interface m_axi port=frame1 offset=slave bundle=data
#pragma HLS interface m_axi port=frame2 offset=slave bundle=data
#pragma HLS interface m_axi port=frame3 offset=slave bundle=data
#pragma HLS interface m_axi port=frame4 offset=slave bundle=data
#pragma HLS interface m_axi port=frame5 offset=slave bundle=data
#pragma HLS interface m_axi port=gradient_z offset=slave bundle=data
#pragma HLS interface m_axi port=dummy_port_1 offset=slave bundle=data
#pragma HLS interface m_axi port=dummy_port_2 offset=slave bundle=data

  const int GRAD_WEIGHTS[] =  {1,-8,0,8,-1};
  GRAD_Z_OUTER: for(int r=0; r<MAX_HEIGHT; r++)
  {
    GRAD_Z_INNER: for(int c=0; c<MAX_WIDTH; c++)
    {
      #pragma HLS pipeline II=1
      gradient_z[r][c] =((pixel_t)(frame1[r][c]*GRAD_WEIGHTS[0]
                        + frame2[r][c]*GRAD_WEIGHTS[1]
                        + frame3[r][c]*GRAD_WEIGHTS[2]
                        + frame4[r][c]*GRAD_WEIGHTS[3]
                        + frame5[r][c]*GRAD_WEIGHTS[4]))/12;
    }
  }
  return 0;
}

int gradient_weight_y_module(pixel_t gradient_x[MAX_HEIGHT][MAX_WIDTH],
    pixel_t gradient_y[MAX_HEIGHT][MAX_WIDTH],
    pixel_t gradient_z[MAX_HEIGHT][MAX_WIDTH],
    gradient_t filt_grad[MAX_HEIGHT][MAX_WIDTH],
	ap_int<64> *dummy_port_1,
	ap_int<64> *dummy_port_2)
{
#pragma HLS interface s_axilite port=return bundle=ctrl
#pragma HLS interface m_axi port=gradient_x offset=slave bundle=data
#pragma HLS interface m_axi port=gradient_y offset=slave bundle=data
#pragma HLS interface m_axi port=gradient_z offset=slave bundle=data
#pragma HLS interface m_axi port=filt_grad offset=slave bundle=data
#pragma HLS interface m_axi port=dummy_port_1 offset=slave bundle=data
#pragma HLS interface m_axi port=dummy_port_2 offset=slave bundle=data

  hls::LineBuffer<7,MAX_WIDTH,gradient_t> buf;

  const pixel_t GRAD_FILTER[] = {0.0755, 0.133, 0.1869, 0.2903, 0.1869, 0.133, 0.0755};
  GRAD_WEIGHT_Y_OUTER: for(int r=0; r<MAX_HEIGHT+3; r++)
  {
    GRAD_WEIGHT_Y_INNER: for(int c=0; c<MAX_WIDTH; c++)
    {
      //#pragma HLS pipeline II=16
      #pragma HLS dependence variable=buf inter false

      if(r<MAX_HEIGHT)
      {
        buf.shift_pixels_up(c);
        gradient_t tmp;
        tmp.x = gradient_x[r][c];
        tmp.y = gradient_y[r][c];
        tmp.z = gradient_z[r][c];
        //tmp.pad = 0;
        buf.insert_bottom_row(tmp,c);
      }
      else
      {
        buf.shift_pixels_up(c);
        gradient_t tmp;
        tmp.x = 0;
        tmp.y = 0;
        tmp.z = 0;
        //tmp.pad = 0;
        buf.insert_bottom_row(tmp,c);
      }

      gradient_t acc;
      acc.x = 0;
      acc.y = 0;
      acc.z = 0;
      //acc.pad = 0; //Mang: to keep the compiler happy
      if(r >= 6 && r<MAX_HEIGHT)
      {
        GRAD_WEIGHT_Y_ACC: for(int i=0; i<7; i++)
        {
//		  #pragma HLS ALLOCATION instances=mul limit=1 operation
//		  #pragma HLS ALLOCATION instances=shl limit=1 operation
          acc.x += buf.getval(i,c).x*GRAD_FILTER[i];
          acc.y += buf.getval(i,c).y*GRAD_FILTER[i];
          acc.z += buf.getval(i,c).z*GRAD_FILTER[i];
          //acc.pad += 0;
        }
        filt_grad[r-3][c] = acc;
      }
      else if(r>=3)
      {
        filt_grad[r-3][c] = acc;
      }
    }
  }
  return 0;
}

int gradient_weight_x_module(gradient_t y_filt[MAX_HEIGHT][MAX_WIDTH],
                       gradient_t filt_grad[MAX_HEIGHT][MAX_WIDTH],
						ap_int<64> *dummy_port_1,
						ap_int<64> *dummy_port_2,
						ap_int<64> *dummy_port_3,
						ap_int<64> *dummy_port_4)
{
	#pragma HLS interface s_axilite port=return bundle=ctrl
	#pragma HLS interface m_axi port=y_filt offset=slave bundle=data
	#pragma HLS interface m_axi port=filt_grad offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_1 offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_2 offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_3 offset=slave bundle=data
	#pragma HLS interface m_axi port=dummy_port_4 offset=slave bundle=data

  hls::Window<1,7,gradient_t> buf;
  const pixel_t GRAD_FILTER[] = {0.0755, 0.133, 0.1869, 0.2903, 0.1869, 0.133, 0.0755};
  GRAD_WEIGHT_X_OUTER: for(int r=0; r<MAX_HEIGHT; r++)
  {
    GRAD_WEIGHT_X_INNER: for(int c=0; c<MAX_WIDTH+3; c++)
    {
      //#pragma HLS pipeline II=1
	  //#pragma HLS ALLOCATION instances=xor limit=4 operation
	  //#pragma HLS ALLOCATION instances=and limit=4 operation
      buf.shift_pixels_left();
      gradient_t tmp;
      if(c<MAX_WIDTH)
      {
        tmp = y_filt[r][c];
      }
      else
      {
        tmp.x = 0;
        tmp.y = 0;
        tmp.z = 0;
      }
      buf.insert_pixel(tmp,0,6);

      gradient_t acc;
      acc.x = 0;
      acc.y = 0;
      acc.z = 0;
      if(c >= 6 && c<MAX_WIDTH)
      {
        GRAD_WEIGHT_X_ACC: for(int i=0; i<7; i++)
        {
		  //#pragma HLS ALLOCATION instances=mul limit=8 operation
          acc.x += buf.getval(0,i).x*GRAD_FILTER[i];
          acc.y += buf.getval(0,i).y*GRAD_FILTER[i];
          acc.z += buf.getval(0,i).z*GRAD_FILTER[i];
        }
        filt_grad[r][c-3] = acc;
      }
      else if(c>=3)
      {
        filt_grad[r][c-3] = acc;
      }
    }
  }
  return 0;
}

int outer_product_module(gradient_t gradient[MAX_HEIGHT][MAX_WIDTH],
     outer_t outer_product[MAX_HEIGHT][MAX_WIDTH],
	ap_int<64> *dummy_port_1,
	ap_int<64> *dummy_port_2,
	ap_int<64> *dummy_port_3,
	ap_int<64> *dummy_port_4)
{
#pragma HLS interface s_axilite port=return bundle=ctrl
#pragma HLS interface m_axi port=gradient bundle=data offset=slave
#pragma HLS interface m_axi port=outer_product bundle=data offset=slave
#pragma HLS interface m_axi port=dummy_port_1 offset=slave bundle=data
#pragma HLS interface m_axi port=dummy_port_2 offset=slave bundle=data
#pragma HLS interface m_axi port=dummy_port_3 offset=slave bundle=data
#pragma HLS interface m_axi port=dummy_port_4 offset=slave bundle=data

  OUTER_OUTER: for(int r=0; r<MAX_HEIGHT; r++)
  {
	OUTER_INNER: for(int c=0; c<MAX_WIDTH; c++)
	{
	  #pragma HLS pipeline II=1
	  gradient_t grad = gradient[r][c];
	  outer_pixel_t x = (outer_pixel_t) grad.x;
	  outer_pixel_t y = (outer_pixel_t) grad.y;
	  outer_pixel_t z = (outer_pixel_t) grad.z;
	  outer_t out;
	  out.val[0] = (x*x);
	  out.val[1] = (y*y);
	  out.val[2] = (z*z);
	  out.val[3] = (x*y);
	  out.val[4] = (x*z);
	  out.val[5] = (y*z);
	  outer_product[r][c] = out;
	}
  }
  return 0;
}

int tensor_weight_y_module(outer_t outer[MAX_HEIGHT][MAX_WIDTH],
                     tensor_t tensor_y[MAX_HEIGHT][MAX_WIDTH],
						ap_int<64> *dummy_port_1,
						ap_int<64> *dummy_port_2,
						ap_int<64> *dummy_port_3,
						ap_int<64> *dummy_port_4)
{

#pragma HLS interface s_axilite port=return bundle=ctrl
#pragma HLS interface m_axi port=outer offset=slave bundle=data
#pragma HLS interface m_axi port=tensor_y offset=slave bundle=data
#pragma HLS interface m_axi port=dummy_port_1 offset=slave bundle=data
#pragma HLS interface m_axi port=dummy_port_2 offset=slave bundle=data
#pragma HLS interface m_axi port=dummy_port_3 offset=slave bundle=data
#pragma HLS interface m_axi port=dummy_port_4 offset=slave bundle=data

  hls::LineBuffer<3,MAX_WIDTH,outer_t> buf;
  const pixel_t TENSOR_FILTER[] = {0.3243, 0.3513, 0.3243};
  TENSOR_WEIGHT_Y_OUTER: for(int r=0; r<MAX_HEIGHT+1; r++)
  {
    TENSOR_WEIGHT_Y_INNER: for(int c=0; c<MAX_WIDTH; c++)
    {
      #pragma HLS pipeline II=1

      outer_t tmp;
      #pragma HLS data_pack variable=tmp
      #pragma HLS data_pack variable=buf.val[0]
      buf.shift_pixels_up(c);
      if(r<MAX_HEIGHT)
      {
        tmp = outer[r][c];
      }
      else
      {
        TENSOR_WEIGHT_Y_TMP_INIT: for(int i=0; i<6; i++)
          tmp.val[i] = 0;
      }
      buf.insert_bottom_row(tmp,c);

      tensor_t acc;
      TENSOR_WEIGHT_Y_ACC_INIT: for(int k =0; k<6; k++)
        acc.val[k] = 0;

      if (r >= 2 && r < MAX_HEIGHT)
      {
        TENSOR_WEIGHT_Y_TMP_OUTER: for(int i=0; i<3; i++)
        {
          tmp = buf.getval(i,c);
          pixel_t k = TENSOR_FILTER[i];
          TENSOR_WEIGHT_Y_TMP_INNER: for(int component=0; component<6; component++)
          {
			#pragma HLS pipeline
			#pragma HLS ALLOCATION instances=mul limit=10 operation
            acc.val[component] += tmp.val[component]*k;
          }
        }
      }
      if(r >= 1)
      {
        tensor_y[r-1][c] = acc;
      }
    }
  }
  return 0;
}

int tensor_weight_x_module(tensor_t tensor_y[MAX_HEIGHT][MAX_WIDTH],
                     tensor_t tensor[MAX_HEIGHT][MAX_WIDTH],
						ap_int<64> *dummy_port_1,
						ap_int<64> *dummy_port_2,
						ap_int<64> *dummy_port_3,
						ap_int<64> *dummy_port_4)
{

#pragma HLS interface s_axilite port=return bundle=ctrl
#pragma HLS interface m_axi port=tensor_y offset=slave bundle=data
#pragma HLS interface m_axi port=tensor offset=slave bundle=data
#pragma HLS interface m_axi port=dummy_port_1 offset=slave bundle=data
#pragma HLS interface m_axi port=dummy_port_2 offset=slave bundle=data
#pragma HLS interface m_axi port=dummy_port_3 offset=slave bundle=data
#pragma HLS interface m_axi port=dummy_port_4 offset=slave bundle=data

  hls::Window<1,3,tensor_t> buf;
  const pixel_t TENSOR_FILTER[] = {0.3243, 0.3513, 0.3243};
  //const float TENSOR_FILTER[] = {0.3243, 0.3513, 0.3243};
  TENSOR_WEIGHT_X_OUTER: for(int r=0; r<MAX_HEIGHT; r++)
  {
    TENSOR_WEIGHT_X_INNER: for(int c=0; c<MAX_WIDTH+1; c++)
    {
      #pragma HLS pipeline II=1
      buf.shift_pixels_left();
      tensor_t tmp;
      if(c<MAX_WIDTH)
      {
        tmp = tensor_y[r][c];
      }
      else
      {
        TENSOR_WEIGHT_X_TMP_INIT: for(int i=0; i<6; i++)
          tmp.val[i] = 0;
      }
      buf.insert_pixel(tmp,0,2);

      tensor_t acc;
      TENSOR_WEIGHT_X_ACC_INIT: for(int k =0; k<6; k++)
        acc.val[k] = 0;
      if (c >= 2 && c < MAX_WIDTH)
      {
        TENSOR_WEIGHT_X_TMP_OUTER: for(int i=0; i<3; i++)
        {
          tmp = buf.getval(0,i);
          TENSOR_WEIGHT_X_TMP_INNER: for(int component=0; component<6; component++)
          {
			#pragma HLS pipeline
		    #pragma HLS ALLOCATION instances=mul limit=10 operation
            acc.val[component] += tmp.val[component]*TENSOR_FILTER[i];
          }
        }
      }
      if(c>=1)
      {
        tensor[r][c-1] = acc;
      }
    }
  }
  return 0;
}

int flow_calc_module(tensor_t tensors[MAX_HEIGHT][MAX_WIDTH],
               velocity_t outputs[MAX_HEIGHT][MAX_WIDTH],
				ap_int<64> *dummy_port_1,
				ap_int<64> *dummy_port_2,
				ap_int<64> *dummy_port_3,
				ap_int<64> *dummy_port_4)
{
#pragma HLS interface s_axilite port=return bundle=ctrl
#pragma HLS interface m_axi port=tensors offset=slave bundle=data
#pragma HLS interface m_axi port=outputs offset=slave bundle=data
#pragma HLS interface m_axi port=dummy_port_1 offset=slave bundle=data
#pragma HLS interface m_axi port=dummy_port_2 offset=slave bundle=data
#pragma HLS interface m_axi port=dummy_port_3 offset=slave bundle=data
#pragma HLS interface m_axi port=dummy_port_4 offset=slave bundle=data

  static outer_pixel_t buf[2];
  FLOW_OUTER: for(int r=0; r<MAX_HEIGHT; r++)
  {
    FLOW_INNER: for(int c=0; c<MAX_WIDTH; c++)
    {
      //#pragma HLS pipeline II=1
      tensor_t tmp_tensor = tensors[r][c];
      if(r>=2 && r<MAX_HEIGHT-2 && c>=2 && c<MAX_WIDTH-2)
      {
		  #pragma HLS ALLOCATION instances=mul limit=4 operation
		  #pragma HLS ALLOCATION instances=sdiv limit=4 operation
		  #pragma HLS ALLOCATION instances=lshr limit=4 operation
	      calc_pixel_t t1 = (calc_pixel_t) tmp_tensor.val[0];
	      calc_pixel_t t2 = (calc_pixel_t) tmp_tensor.val[1];
	      calc_pixel_t t3 = (calc_pixel_t) tmp_tensor.val[2];
	      calc_pixel_t t4 = (calc_pixel_t) tmp_tensor.val[3];
	      calc_pixel_t t5 = (calc_pixel_t) tmp_tensor.val[4];
	      calc_pixel_t t6 = (calc_pixel_t) tmp_tensor.val[5];

          calc_pixel_t denom = t1*t2-t4*t4;
	      calc_pixel_t numer0 = t6*t4-t5*t2;
	      calc_pixel_t numer1 = t5*t4-t6*t1;

	      if(denom != 0)
        {
          buf[0] = numer0 / denom;
          buf[1] = numer1 / denom;
	      }
	      else
	      {
		      buf[0] = 0;
		      buf[1] = 0;
	      }
      }
      else
      {
        buf[0] = buf[1] = 0;
      }

      outputs[r][c].x = (vel_pixel_t)buf[0];
      outputs[r][c].y = (vel_pixel_t)buf[1];

    }
  }
  return 0;
}

/*static pixel_t gradient_x[MAX_HEIGHT][MAX_WIDTH];
static pixel_t gradient_y[MAX_HEIGHT][MAX_WIDTH];
static pixel_t gradient_z[MAX_HEIGHT][MAX_WIDTH];
static gradient_t y_filtered[MAX_HEIGHT][MAX_WIDTH];
static gradient_t filtered_gradient[MAX_HEIGHT][MAX_WIDTH];
static outer_t out_product[MAX_HEIGHT][MAX_WIDTH];
static tensor_t tensor_y[MAX_HEIGHT][MAX_WIDTH];
static tensor_t tensor[MAX_HEIGHT][MAX_WIDTH];

// FIFOs for streaming in, just for clarity
static input_t frame1_a[MAX_HEIGHT][MAX_WIDTH];
static input_t frame2_a[MAX_HEIGHT][MAX_WIDTH];
static input_t frame4_a[MAX_HEIGHT][MAX_WIDTH];
static input_t frame5_a[MAX_HEIGHT][MAX_WIDTH];

//Need to duplicate frame3 for the two calculations
static input_t frame3_a[MAX_HEIGHT][MAX_WIDTH];
static input_t frame3_b[MAX_HEIGHT][MAX_WIDTH];

void optical_flow_modular(
		frames_t   frames[MAX_HEIGHT][MAX_WIDTH],
		velocity_t outputs[MAX_HEIGHT][MAX_WIDTH])
{
  read_input_module(frames, frame1_a, frame2_a, frame3_a, frame3_b, frame4_a, frame5_a);
  gradient_xy_calc_module(frame3_a, gradient_x, gradient_y);
  gradient_z_calc_module(frame1_a, frame2_a, frame3_b, frame4_a, frame5_a, gradient_z);
  gradient_weight_y_module(gradient_x, gradient_y, gradient_z, y_filtered);
  gradient_weight_x_module(y_filtered, filtered_gradient);
  outer_product_module(filtered_gradient, out_product);
  tensor_weight_y_module(out_product, tensor_y);
  tensor_weight_x_module(tensor_y, tensor);
	for (int i = 0; i < 10; i++)
	{
	  for (int j = 0; j < 10; j++)
	  {
		  float pixel = tensor[i][j].val[0].to_float();
		  //printf("%f\n\r ", pixel);
	  }
	}
  flow_calc_module(tensor, outputs);
//	for (int i = 0; i < 10; i++)
//	{
//	  for (int j = 0; j < 10; j++)
//	  {
//		  float pixel = outputs[i][j].x.to_float();
//		  printf("%f ", pixel);
//		  pixel = outputs[i][j].y.to_float();
//		  printf("%f\n\r ", pixel);
//	  }
//	}
}*/
