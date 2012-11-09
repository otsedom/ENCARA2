#include "cv.h"
#include "highgui.h"

// Encara2's includes: begin
#include "ENCARA2_2lib.h"
// Encara2's includes: end

#include <iostream>
#include <cstdio>

#ifdef _EiC
#define WIN32
#endif

using namespace std;
using namespace cv;


//ENCARA2 variables
IplImage* pIplImage;
char ENCARAdataDir[256]; //Detector data path
CENCARA2_2Detector *ENCARAFaceDetector=NULL; //Detector
CFacialDataperImage *FacialData; //Detection data
//ENCARA2 variables end

int main( int argc, const char** argv )
{
    CvCapture* capture = 0;
    Mat frame, frameCopy, image;
    
   //Enough number of arguments
   if (argc>=3)
   {
	if (strcmp(argv[1],"--path "))
	{
		fprintf(stderr,"\nENCARA2 path introduced: %s\n\n",argv[2]);
		strcpy(ENCARAdataDir,argv[2]);
	}
	else
	{
		fprintf(stderr,"\nUse as encara2-test -path \"path\"\n");
		return 0;
	}
   }
   else
   {
	fprintf(stderr,"\nUse as encara2-test -path \"path\"\n");
	return 0;
   }

    //Camera activation
    capture = cvCaptureFromCAM(CV_CAP_V4L);//check that the required packages are required, particularly in case that openni is installed

    cvNamedWindow( "result", 1 );

    if( capture )
    {
        for(;;)
        {
            IplImage* iplImg = cvQueryFrame( capture );
            frame = iplImg;
            if( frame.empty() )
                break;
            if( iplImg->origin == IPL_ORIGIN_TL )
                frame.copyTo( frameCopy );
            else
                flip( frame, frameCopy, 0 );

            //Loading ENCARA2
            if (ENCARAFaceDetector==NULL)
            {
		ENCARAFaceDetector=new CENCARA2_2Detector(ENCARAdataDir,frame.size().width,frame.size().height);
            }
	    else
	    {
	    	//ENCARA2 processing
            	ENCARAFaceDetector->ApplyENCARA2(iplImg,2);
		
            	//ENCARA2 detection output
            	ENCARAFaceDetector->PaintFacialData(iplImg,CV_RGB(0,255,0));

	    }
           
	    cvShowImage("result",iplImg);

            if( waitKey( 10 ) >= 0 )
                goto _cleanup_;
        }

        waitKey(0);
_cleanup_:
        cvReleaseCapture( &capture );
	//Deallocating ENCARA
	delete ENCARAFaceDetector;
    }
    else
    {
	fprintf(stderr,"\nCannot open capture object.\n");
    }
   
    cvDestroyWindow("result");

    return 0;
}


