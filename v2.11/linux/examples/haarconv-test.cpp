#include <cv.h>

// for printf's
#include <string>


int main(int argc, char** argv)
{
	if( argc == 5 )
	{
		CvHaarClassifierCascade* cascade_old =
		cvLoadHaarClassifierCascade( argv[1], cvSize( atoi(argv[3]), atoi
		(argv[4]) ) );
		printf("1. classifier loaded from _Directory_\n");
		cvSave( argv[2], cascade_old );
		printf("2. classifier saved to xml _File_\n");
		cvReleaseHaarClassifierCascade( &cascade_old );
		
		// test conversion
		CvHaarClassifierCascade* cascade_new =
		(CvHaarClassifierCascade*) cvLoad( argv[2] );
		printf("3. classifier loaded from new xml_File_\n");
		cvReleaseHaarClassifierCascade( &cascade_new );
	}
	else
	{
		printf("Convert classifiers for Haar-like feature from directory structure into xml (as\nintroduced with OpenCV beta4).\n\n");
		printf("Usage: haarconv <dir> <xml> <sizeX><sizeY>\n\n");
		printf("<dir> classifier directory containing the stage directories 0..n\n");
		printf("<xml> xml file where the classifier should be saved to\n");
		printf("<sizeX/Y> sample size the classifier hasbeen trained for\n");bin

		printf("\nExample: haarconv oldtree newfile.xml 25 25");
		printf("\n\nSee other utilities for OpenCV's object recognition framework at http://www.inflomatik.info\n");
	}
	
	return 0;
} 
