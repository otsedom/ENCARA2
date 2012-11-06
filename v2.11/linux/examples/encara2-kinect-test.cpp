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
char ENCARAdataDir[256]; //Detector data path
CENCARA2_2Detector *ENCARAFaceDetector=NULL; //Detector
CFacialDataperImage FacialData; //Detection data
//ENCARA2 variables end

int main( int argc, const char** argv )
{
    VideoCapture capture;
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
    capture.open(CV_CAP_OPENNI); 

   if( !capture.isOpened() )
   {
	fprintf(stderr,"\nCannot open capture object.\n");
	return 0;
   }

   if( capture.isOpened() )
    {
        for(;;)
        {
            //Kinect
	    capture.grab();
	    capture.retrieve(frame,CV_CAP_OPENNI_BGR_IMAGE);
            //frame = iplImg;
            if( frame.empty() )
                break;
           
            //Loading ENCARA2
            if (ENCARAFaceDetector==NULL)
            {
		ENCARAFaceDetector=new CENCARA2_2Detector(ENCARAdataDir,frame.size().width,frame.size().height);
            }
	    else
	    {
	    	//ENCARA2 processing
		IplImage h=frame;
				
		IplImage *imagen = cvCloneImage(&h);
            	ENCARAFaceDetector->ApplyENCARA2(imagen,2);
		
            	//ENCARA2 detection output
		IplImage iframe=frame;
            	ENCARAFaceDetector->PaintFacialData(&iframe,CV_RGB(0,255,0));

		//Liberamos imagen temporal
		cvReleaseImage(&imagen);
	    }
           
	    imshow("Detections",frame);

            if( waitKey( 10 ) >= 0 )
                goto _cleanup_;
        }

        waitKey(0);
_cleanup_:
        //Deallocating ENCARA
	delete ENCARAFaceDetector;
    }
    else
    {
	fprintf(stderr,"\nCannot open capture object.\n");
    }
   
    return 0;
}


