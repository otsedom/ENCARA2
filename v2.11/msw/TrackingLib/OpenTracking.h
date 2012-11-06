/* 

		Class definition

*/

//Utilidades IplImage
#include "IplImageUtils.h"

#include "cv.h"

//Cazando gazapos
#include "DebugLog.h"

class COpenTracking
{
	public:
		COpenTracking (IplImage *pimg, int x, int y, int iROI_x, int iROI_y, int iPatternSize_x, int iPatternSize_y);
		~COpenTracking ();
		void Track (IplImage *pimg);
		void SetPattern(IplImage *pimg, int ix, int iy);
		IplImage *GetPattern(); //! Gets a copy of the current pattern
		int GetLastMin();		//! Returns the min achieved in ROI
		void SetPatternandROI(IplImage *pimg, int ix, int iy, int iROI_x,int iROI_y);
		void GetSearchArea(int *esqx,int *esqy, int *sx, int *sy);//Gets the search area

		unsigned char *pbPattern;
		int *piBuffer; 
		unsigned char *pbImageAux;
		int x,y; // Tracking position x and y
		int iValue;//Min achieved in ROI
		int iSecondMin;
		int threshold;
		int image_width, image_height;
		int ROIx, ROIy;
		int Patx, Paty;
		bool boLost; //Pattern lost
		float Reliability; //Fiabilidad del seguimiento, 0 sería igual al threshold, 0.5 a mitad de camino, 1.0 idéntico
		bool boInit; // ¿Patrón inicializado?

	private:
		void SecondMin(int *buffer, int sx,int sy);
		void FreeAux();
		void AllocAux();
		void CopyPattern(IplImage *pimg);
		void FitROI(int *esqx,int *esqy, int *sx, int *sy);
		
		bool	boimg_aux;
		IplImage *pimg_aux;
		int buffer_width, buffer_height;
		bool	boBuffer;

		int		Min4Pattern;//Stores the min achieved for the first test
};


// +++++++++++++++++++++++++++++++++++++++++++++++
// The Tracking function, H file
// Juan Mendez
// CeTSIA, ULPGC
// jmendez@dis.ulpgc.es
// Oct-2000
// +++++++++++++++++++++++++++++++++++++++++++++++


void TrackingMMX(unsigned char*,int,int,int,int,int,int,unsigned char*,int,int*,int*,int*,int*);
bool Similarity(int , int, int, unsigned char*, int, int, int, bool* , unsigned char**);


void MX2_AbsSum24(int*,unsigned char*,unsigned char*,int,int//		printf("val=%d, x=%d, y=%d\n", val,pos_x,pos_y);
);
void C_AbsSum24M(int*,unsigned char*,unsigned char*,int,int//		printf("val=%d, x=%d, y=%d\n", val,pos_x,pos_y);
);
