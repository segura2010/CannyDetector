/*
  g++ canny.cpp -o canny -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_objdetect
*/

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

/// Global variables

Mat src, src_gray;
Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold;
int const max_lowThreshold = 100, maxRGB = 255;
int ratio = 3;
int kernel_size = 3;
char* window_name = "Edge Map";

int red = 0, blue = 0, green = 0, minActivate = 150;
int save = 0;


void doCanny()
{
  /// Reduce noise with a kernel 3x3
  blur( src_gray, detected_edges, Size(3,3) );

  /// Canny detector
  Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );

  /// Using Canny's output as a mask, we display our result
  dst = Scalar::all(0);

  src.copyTo( dst, detected_edges);

  // Apply color
  int size = dst.cols * dst.rows * dst.channels();
  uchar *p = dst.data;
  for(int i=0; i<size; i+=3)
  {
    //printf("\n[%d] = %d", i, *p);
    if(*p >= minActivate && *(p+1) >= minActivate && *(p+2) >= minActivate)
    {
      *p = blue;
      *(p+1) = green;
      *(p+2) = red;
    }
    else
    {
      *p = 255;
      *(p+1) = 255;
      *(p+2) = 255;
    }
    p += 3;
  }

  // printf("\nB: %d, G: %d, R: %d, CHANneLS: %d \n", blue, green, red, dst.channels());

  imshow( window_name, dst );
}


/**
 * @function CannyThreshold
 * @brief Trackbar callback - Canny thresholds input with a ratio 1:3
 */
void CannyThreshold(int, void*)
{
  doCanny(); 
}


/**
 * @brief Select R callback - Canny thresholds input with a ratio 1:3
 */
void setColorRGR(int, void*)
{
  doCanny();
}

// Save Image
void saveImage(int, void*)
{
  if(save == 1)
  {
    imwrite("out.jpeg", dst);
  }
}

/** @function main */
int main( int argc, char** argv )
{
  /// Load an image
  src = imread( argv[1] );

  if( !src.data )
  { return -1; }

  /// Create a matrix of the same type and size as src (for dst)
  dst.create( src.size(), src.type() );

  /// Convert the image to grayscale
  cvtColor( src, src_gray, CV_BGR2GRAY );

  /// Create a window
  namedWindow( window_name, CV_WINDOW_AUTOSIZE );

  /// Create a Trackbar for user to enter threshold
  createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold );

  // RGB Trackbars
  createTrackbar( "RED:", window_name, &red, maxRGB, setColorRGR );
  createTrackbar( "GREEN:", window_name, &green, maxRGB, setColorRGR );
  createTrackbar( "BLUE:", window_name, &blue, maxRGB, setColorRGR );
  createTrackbar( "MIN:", window_name, &minActivate, maxRGB, setColorRGR ); // MIN

  // Save Button
  createTrackbar( "SAVE", window_name, &save, 1, saveImage );

  /// Show the image
  CannyThreshold(0, 0);

  /// Wait until user exit program by pressing a key
  waitKey(0);

  return 0;
  }