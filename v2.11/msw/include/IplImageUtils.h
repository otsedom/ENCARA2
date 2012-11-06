/**
 ** The ENCARA2 project
 ** Copyright (C) 2003-2011
 **    Universidad de Las Palmas de Gran Canaria (ULPGC) (<http://www.ulpgc.es>)
 **    
 **    This software is a project which has been designed, developed and
 **    written, and is maintained by members, students and collaborators of the División
 **    de Inteligencia Artificial y Sistemas (DIAS) at the Instituto Universitario de 
 **    Sistemas Inteligentes y Aplicaciones Numéricas en Ingeniería (SIANI) 
 **    (<http://www.siani.es/>), and also of the Departamento de Informática y Sistemas 
 **    (DIS) (<http://www.dis.ulpgc.es>) both at ULPGC. 
 ** 
 ** This file is part of the ENCARA2 Project.
 ** The ENCARA2 Project is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or any
 ** later version.
 ** 
 ** The ENCARA2 Project is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 ** GNU General Public License for more details.
 ** 
 ** You should have received a copy (COPYING file) of the GNU General Public License
 ** along with the ENCARA2 Project . If not, see <http://www.gnu.org/licenses/>.
 **/
 /****************************************************************************************

	Utilities for IplImage, e.g. BYTE <-> IplImage conversion routines (important if width
	not even, etc. Adapted from ENCARA2

	Modesto Castrill� Santana
	January 2005

****************************************************************************************/
#ifndef	_IPLUTILS
#define _IPLUTILS

//OpenCV
//OPENCV
#ifndef OPENCVINCLUDED
#include "opencv/cv.h"  //añadido opencv/ a partir 2.3.1
#include "opencv/cvaux.h"  //añadido opencv/ a partir 2.3.1
#endif

//memcpy
#include <memory.h>
#include <stdio.h>

#ifndef BYTE
typedef unsigned char       BYTE;
//#define BYTE unsigned char
#endif

class CIplImageUtils
{

public:
	CIplImageUtils()
	{
	};
	~CIplImageUtils()
	{
	};

//! Draws a cross on a gray IplImage
/*!	Draws a cross on a gray IplImage
	
	\param	h	IplImage image
	\param	x	Cross center x coordinate
	\param  y	Cross center y coordinate
*/
void PaintCrossb(IplImage *h,int x, int y)
{
	BYTE *p;
	int i;

	//If the position is contained in the image
	if (x>=0 && x<h->width && y<h->height && y>=0)
	{
		p=(BYTE *)h->imageData;

		//Vertical segment
		p+=(h->widthStep)*(y-4)+x;
		for (i=0;i<9;i++,p+=(h->widthStep))
			*p=100;

		//Horizontal segment	
		p=(BYTE *)h->imageData;
		p+=(h->widthStep)*(y)+x-4;
		for (i=0;i<9;i++,p++)
			*p=100;
	}

}

//! Draws a cross on RGB IplImage
/*!	Draws a cross on RGB IplImage
	
	\param	h	IplImage image
	\param	x	Cross center x coordinate
	\param  y	Cross center y coordinate
*/
void PaintCross(IplImage *h,int x, int y)
{
	BYTE *p;
	int tam=10;
	int tam2=tam/2;
	int i;

	//If the position is contained in the image
	if (x>=0 && x<h->width && y<h->height && y>=0)
	{

		p=(BYTE *)h->imageData+((h->widthStep)*(y-tam2)+x*3);

		//Vertical segment
		for (i=0;i<9;i++,p+=(h->widthStep))
		{
			*p=255;
			*(p+1)=255;
			*(p+2)=255;
		}

		p=(BYTE *)h->imageData+((h->widthStep)*y+x*3-tam2*3);

		//Hoizontal segment
		for (i=0;i<9;i++,p+=3)
		{
			*p=255;
			*(p+1)=255;
			*(p+2)=255;
		}
	}
}

//! Copies from BYTE (unsigned char) to IplImage
/*!	Copies from BYTE (unsigned char) to IplImage
	
	\param	pb			Input image
	\param	h			Output image
	\param 	sx			Image width
	\param 	sy			Image height
*/
void CopyBYTE2Ipl(BYTE *pb,IplImage **h,int sx,int sy)
{
	//Set pointers
	BYTE *s=pb,*d=(BYTE *)(*h)->imageData;

	int Canales=(*h)->nChannels;	//Image channels
	int nCopia=Canales*sx;			//bytes to copy per row
		
	//Copies row by row according to the channels number, taking into account widthStep just in case
	//it was an odd width image
	for (int i=0;i<sy;i++,s+=nCopia,d+=(*h)->widthStep)
		memcpy(d,s,nCopia);

}

//! Copies from IplImage to BYTE 
/*!	Copies from IplImage to BYTE 
	
	\param	pb			Input image
	\param	h			Output image
	\param 	sx			Image width
	\param 	sy			Image height
*/
void CopyIpl2BYTE(IplImage *h,BYTE *pb,int sx,int sy)
{
	//Sets pointers
	BYTE *d=pb,*s=(BYTE *)h->imageData;

    int Canales=h->nChannels;	//Image channels
	int nCopia=Canales*sx;		//bytes to copy per row
	
	//Copies row by row according to the channels number, taking into account widthStep just in case
	//it was an odd width image
	for (int i=0;i<sy;i++,d+=nCopia,s+=h->widthStep)
		memcpy(d,s,nCopia);

}

//! Flips an IplImage, e.g. in an AVI file the first row should be the lowest row to plot
/*!	Flips an IplImage, e.g. in an AVI file the first row should be the lowest row to plot (now available using OpenCV)
	
	\param	h			Input image
*/
void FlipIplImage(IplImage *h)
{
	//Set pointers in first and last row
	BYTE *s,*d=(BYTE *)h->imageData;

	//Clones input image
	IplImage *ipltmp=cvCloneImage(h);
	s=(BYTE *)ipltmp->imageData+ipltmp->widthStep*(ipltmp->height-1);

	//Copies row by row according to the channels number, taking into account widthStep just in case
	//it was an odd width image
	for (int i=0;i<ipltmp->height;i++,s-=ipltmp->widthStep,d+=ipltmp->widthStep)
		memcpy(d,s,ipltmp->widthStep);

	//Modifies origin to avoid a pointing from the bottom row
	h->origin=0;

	//Frees temporal memory
	cvReleaseImage(&ipltmp);

}

//#define RotateImageusingipl //Use it if the ipl rotateimage version is preferred

//! Rotates an IplImage
/*!	Rotates an image around a point (got from yahoogroups provided by Parameswaran)
	
	\param	h			Input image
	\param  hr			Output image
	\param angle		Angle to rotate
	\param xPos			x coordinate of the center of rotation
	\param yPos			y coordinate of the center of rotation
	\param rect			ROI rectangle
	\param boROI		If set, applies ROI
*/
void RotateImage(IplImage *h, IplImage *hr,float angle, float xPos, float yPos, bool boROI=false,CvRect rect=cvRect(0,0,0,0)) 
{

#ifdef RotateImageusingipl //Using ipl functions

	//Sets ROI is indicated
	if (boROI)
	{	
		cvSetImageROI(h,rect);
	}

	//Con ipl (versión previa que parecía encajar bien con el uso del ROI en ocasiones)
	double xs=0,ys=0;
	iplGetRotateShift(xPos,yPos,angle,&xs,&ys);
	iplRotate(h,hr,angle,xs,ys,IPL_INTER_NN);

	if (boROI)
	{
		cvResetImageROI(h);
	}
#else//Rotation without using Sin ipl

	//Sets ROI is indicated
	if (boROI)
	{	
		cvSetImageROI(h,rect);
	//It seems Not necessary with ipl
		cvSetImageROI(hr,rect);
	}
	
	//States transformation matrix
	CvPoint2D32f center;
	CvMat *N = cvCreateMat( 2, 3, CV_32FC1 );;
	
	
	//If there is a ROI the center of rotation is affected
	if (boROI)
	{
		//The center of rotation is affected by the roi
		center.x = (float)xPos-rect.x;
		center.y = (float)yPos-rect.y;
	}
	else
	{
		//The center of rotation
		center.x = (float)xPos;
		center.y = (float)yPos;
	}

	//Gets transformation matrix
	cv2DRotationMatrix( center, angle ,1.0, N );//the third parameter indicates the scale

	//Applies transformation
	cvWarpAffine( h, hr, N,CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);

	cvReleaseMat(&N);

	//Resets ROI
	if (boROI)
	{
		cvResetImageROI(h);
		cvResetImageROI(hr);
	}

#endif


/*	cvCircle( hrrr, cvPoint((int)xPos,(int)yPos), 3, CV_RGB(255,255,255),-1,8,0);
	cvCircle( hr, cvPoint((int)xPos,(int)yPos), 3, CV_RGB(255,255,255),-1,8,0);

#ifdef ProbandoRot
	cvSaveImage("C:\\rotadaipl.jpg",hr);
	cvSaveImage("C:\\rotada3.jpg",hrrr);
	
	cvReleaseImage(&hrr);
	cvReleaseImage(&hrrr);
#endif*/
}

/**************************************************************************************************
		CONTOURS AND COMPONENTS
**************************************************************************************************/

//! Search N biggest connected components.
/*!	Search N biggest connected components in the binary image provided, 
	returning their contours and areas.
	
	\param		h				Binary image
	\param		storage			structure used by the contours search
	\param		cont			contours returned
	\param		farea			areas returned
	\param		Ncomponentes	Maximal number of components to return
*/
int		FindNLargestComponents(IplImage *i,  CvMemStorage* storage, CvSeq ***contour,
					float **farea, int Ncomponentes)
{
    CvSeq* chain = 0; 
    double area;
	int num_conts;


	//Clones input image
	IplImage *itemp=cvCloneImage(i);

	//Searches contours
	num_conts=cvFindContours( itemp, storage, &chain, sizeof(CvContour),
		CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE );

	//If nothing is detected, frees memory
	if (chain==0) 
	{ 
		if (storage!=NULL) 
			cvReleaseMemStorage(&storage); 

		cvReleaseImage(&itemp);
		return 0; 
	}

	//Allocs memory for components data
	*contour=(CvSeq **)new unsigned char[Ncomponentes*sizeof(CvSeq *)];
	*farea=(float *) new unsigned char[Ncomponentes*sizeof(float)];


	//If something was detected initializes
	int j,k;

	for (j=0;j<Ncomponentes;j++)
	{
		(*farea)[j]=0;
		(*contour)[j]=NULL;
	}

	//Sorts the contours according to the areas
    for( CvSeq* c = chain; c != 0; c = c->h_next )
	{
        area = fabs(cvContourArea(c));
		k=0;
		if (area>0.0)
		{
			//Compares the new area with those already stored
			do{

				if (area>(*farea)[k])
				{
					if ((*farea)[k]>0.0)
					{
						//Moves those not properly sorted
						for (j=Ncomponentes-1;j>k;j--)
						{
							if ((*farea)[j-1]>0.0)
							{
								(*farea)[j]=(*farea)[j-1];
								(*contour)[j]=(*contour)[j-1];
							}
						}
					}
					(*farea)[k]=(float)area;
					(*contour)[k]=c;
					break;
				}
				k++;
			}while(k<Ncomponentes);
		}
    }

	if ((*contour)[0]==NULL) 
	{ 
		if (storage!=NULL) 
			cvReleaseMemStorage(&storage);

		cvReleaseImage(&itemp); 

		delete [] *farea;
		delete [] *contour;

		return 0; 
	}

	//Frees memory
	cvReleaseImage(&itemp);

	return 1;
}

//! Search the biggest connected component.
/*!	Searches the biggest connected component, and returns its area.
	
	\param	i			Binary image
	\param		storage			structure used by the contours search
	\param		contMayor1		contour returned
	\param		area1			areas returned
*/
int	FindLargestComponent(IplImage *i, CvMemStorage* storage, CvSeq **contMayor1,float *area1)
{
    CvSeq* chain = 0; 
    double area,max_area;
	int num_conts;

	num_conts=cvFindContours( i, storage, &chain, sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE );

	if (chain==0) { cvReleaseMemStorage(&storage); return 0; }

	max_area=0;
	CvSeq *contour=NULL;
	//Stores the one with maximal area
    for( CvSeq* c = chain; c != 0; c = c->h_next )
		{
        area = fabs(cvContourArea(c));
        if (area > max_area) { max_area=area; contour=c; }
        }

	if (contour==NULL) { cvReleaseMemStorage(&storage); return 0; }

	*contMayor1=contour;
	*area1=(float)max_area;

	return 1;
}





/*******************************************************************************************************
		IplImage I/O
*******************************************************************************************************/

//! Loads an IplImage from disk
/*!	Loads an IplImage from disk
	
	\param	fichero				Filename
*/
IplImage* LoadIplImage(char *fichero)
{
	FILE *rf; 

	IplImage *iSrc;//Image to be returned

	//The file is openned
	rf=fopen(fichero,"rb");

	//Returns NULL if the file is not found
	if (!rf)
		return NULL;

	//Creates a temporal pointer
	IplImage *itemp=(IplImage *) new unsigned char[sizeof(IplImage)];

	//Reads the information to the temporal pointer
	fread(itemp,sizeof(IplImage),1,rf);

	//Creates a header according to the info read
	iSrc=cvCreateImageHeader(cvSize(itemp->width,itemp->height),itemp->depth,itemp->nChannels);

	//Allocs memory for the image data
	cvCreateImageData(iSrc);

	//Reads image data
	fread(iSrc->imageData,iSrc->imageSize,1,rf);

	iSrc->imageDataOrigin=iSrc->imageData;

	//The file is closed
	fclose(rf);

	//Frees memory
	delete [] itemp;

return(iSrc);
}

//! Saves an IplImage to disk
/*!	Saves an IplImage to disk
	
	\param	fichero				Filename
	\param iSrc				IplImage to write
*/
void SaveIplImage(char *fichero, IplImage *iSrc)
{
	FILE *rf;

	//Opens the file
	rf=fopen(fichero,"wb");

	//The header is written
	fwrite(iSrc,sizeof(IplImage),1,rf);

	//The data are written
	fwrite(iSrc->imageData,sizeof(char),iSrc->imageSize,rf);

	//Closes the file
	fclose(rf);
}

//! Copies an IplImage in another bigger (this is not verified). Both are RGB
/*!	Copies an IplImage in another bigger (this is not verified). Both are RGB
	
	\param	src		Input
	\param	dst		Output
	\param	posx	Position x coordinate
	\param posy		Position y coordinate
*/
void Collage(IplImage *src,IplImage *dst,int posx,int posy)

{
	BYTE *s,*d;

	//Set pointers
	s=(BYTE *)src->imageData;
	d=(BYTE *)dst->imageData+dst->widthStep*posy+posx*dst->nChannels;

	//Copies the image
	for (int i=0;i<src->height;i++,s+=src->widthStep,d+=dst->widthStep)
	{
		BYTE *ss,*dd;
		ss=s;
		dd=d;
		//Three times as the images are RGB
		for (int j=0;j<src->width;j++,ss+=3,dd+=3)
			for (int n=0;n<dst->nChannels;n++)
				*(dd+n)=*(ss+n);
						
	}
}

//! Checks if a window is contained in the IplImage boundaries
/*!	Checks if a window is contained in the IplImage boundaries, correcting the window
	if necessary
	
	\param	h	Image
	\param	x	Upper left corner x coordinate
	\param	y	Upper left corner y coordinate
	\param	sx	Window width
	\param  sy	Window height
*/
void WindowTest(IplImage *h, int *x1,int *y1, int *sx, int*sy)
{

	//Checks sizex
	if (*sx<0)
	{
		//Sets width and initial point x
		*sx=0;
	}
	//Checks sizey
	if (*sy<0)
	{
		//Sets width and initial point x
		*sy=0;
	}

	//Checks initial point x
	if (*x1<0)
	{
		//Sets width and initial point x
		*sx=*sx + *x1;
		*x1=0;
	}

	//Checks initial point y
	if (*y1<0)
	{
		//Sets height and initial point y
		*sy=*sy + *y1;
		*y1=0;
	}

	//Too big
	if (*x1>= h->width)
	{
		//Sets width and initial point x
		*x1=h->width-1;
	}
	//Too big
	if (*y1>= h->height)
	{
		//Sets width and initial point x
		*y1=h->height-1;
	}


	//Checks the other corner
	if (*x1 + *sx >= h->width)
		*sx-= (*x1 + *sx) - h->width;
	if (*y1 + *sy >= h->height)
		*sy-= (*y1 + *sy) - h->height;

}

//! Checks if a window is contained in the IplImage boundaries
/*!	Checks if a window is contained in the IplImage boundaries, correcting the window
	if necessary
	
	\param	w	Image width
	\param	h	Image height
	\param	x	Upper left corner x coordinate
	\param	y	Upper left corner y coordinate
	\param	sx	Window width
	\param  sy	Window height
*/
void WindowTest(int w, int h, int *x1,int *y1, int *sx, int*sy)
{

	//Checks sizex
	if (*sx<0)
	{
		//Sets width and initial point x
		*sx=0;
	}
	//Checks sizey
	if (*sy<0)
	{
		//Sets width and initial point x
		*sy=0;
	}

	//Checks initial point x
	if (*x1<0)
	{
		//Sets width and initial point x
		*sx=*sx + *x1;
		*x1=0;
	}

	//Checks initial point y
	if (*y1<0)
	{
		//Sets height and initial point y
		*sy=*sy + *y1;
		*y1=0;
	}

	//Too big
	if (*x1>= w)
	{
		//Sets width and initial point x
		*x1=w-1;
	}
	//Too big
	if (*y1>= h)
	{
		//Sets width and initial point x
		*y1=h-1;
	}


	//Checks the other corner
	if (*x1 + *sx >= w)
		*sx-= (*x1 + *sx) - h;
	if (*y1 + *sy >= h)
		*sy-= (*y1 + *sy) - h;

}


//! Checks if a window, given by corners, is contained in the IplImage boundaries
/*!	Checks if a window, given by corners, is contained in the IplImage boundaries, correcting the window
	if necessary
	
	\param	h				image
	\param	x1	Upper left corner x coordinate
	\param	y1	Upper left corner y coordinate
	\param	x2	Bottom right corner x coordinate
	\param	y2	Bottom right corner y coordinate
*/
void WindowTestP(IplImage *h, int *x1,int *y1, int *x2, int*y2)
{

	//Checks first corner
	if (*x1<0)
		*x1=0;
	if (*y1<0)
		*y1=0;

	//Too big
	if (*x1>= h->width)
	{
		//Sets width and initial point x
		*x1=h->width-1;
	}
	//Too big
	if (*y1>= h->height)
	{
		//Sets width and initial point x
		*y1=h->height-1;
	}
	
	//Checks second corner
	if (*x2 >= h->width)
		*x2 = h->width-1;
	if (*y2 >= h->height)
		*y2 = h->height-1;

	//Checks first corner
	if (*x2<*x1)
		*x2=*x1;
	if (*y2<*y1)
		*y2=*y1;

}

//! Checks if a window, given by corners, is contained in the IplImage boundaries
/*!	Checks if a window, given by corners, is contained in the IplImage boundaries, correcting the window
	if necessary
	
	\param	w	Image width
	\param	h	Image height
	\param	x1	Upper left corner x coordinate
	\param	y1	Upper left corner y coordinate
	\param	x2	Bottom right corner x coordinate
	\param	y2	Bottom right corner y coordinate
*/
void WindowTestP(int w, int h, int *x1,int *y1, int *x2, int*y2)
{

	//Checks first corner
	if (*x1<0)
		*x1=0;
	if (*y1<0)
		*y1=0;

	//Too big
	if (*x1>= w)
	{
		//Sets width and initial point x
		*x1=w-1;
	}
	//Too big
	if (*y1>= h)
	{
		//Sets width and initial point x
		*y1=h-1;
	}
	
	//Checks second corner
	if (*x2 >= w)
		*x2 = w-1;
	if (*y2 >= h)
		*y2 = h-1;

	//Checks first corner
	if (*x2<*x1)
		*x2=*x1;
	if (*y2<*y1)
		*y2=*y1;

}


/*******************************************************************************************
	
	Minimum and maximum search in gray images (there are now OpenCV routines)

		h			image
		min			Minimum returned
		max			Maximum returned

*******************************************************************************************/
void BuscaMinMax(IplImage *h, BYTE *min, BYTE *max)
{
	//Initialization
	*min=255;
	*max=0;

	//Sets pointers
	BYTE *pp,*p=(unsigned char *)h->imageData;

	//Searchs minimum and maximum
	for (int i=0;i<h->height;i++,p+=h->widthStep)
		{
			pp=p;
			for (int j=0;j<h->width;j++,pp++)
			{
				if (*pp<*min)
					*min=*pp;
				if (*pp>*max)
					*max=*pp;
			}
	}
}

/*!	Computes the correlogram for a given window using a gray image
	
	\param	h			Input image
	\param	x			x coordinate of the upper left corner
	\param	y			y coordinate of the upper left corner
	\param 	sx			Window width
	\param 	sy			Window height
	\param  bins		Bins per color dimension
	\param  dist		Distance to consider between pixels
	\param  boHor		If true computes the correlogram in the horizontal direction
*/
int * GetCorrelogram(IplImage *h,int x, int y, int sx,int sy,int bins, int dist, bool boHor=true)
{
	//It must be a gray image
	if (h==NULL) return 0;
	if (h->nChannels>1) return 0;
	if (h->depth!=8) return 0;

	//Bounds the histogram size
	if (bins>100) return 0;

	int *hist=(int *) new unsigned char [bins*bins*sizeof(int)];

	int r,c;

	unsigned char * p= (unsigned char *)h->imageData,*pi,*pid;

	p=p+sy*h->widthStep+sx;

	//Initializes histogram
	memset(hist,0,sizeof(float)*bins*bins); // Clear result histogram 
		
	//Computes histogram
	if (boHor)
	{
		//For each row
		for (r=0;r<sy;r++,p+=h->widthStep)
		{
			pi=p;
			pid=pi+dist;
			//For each column
			for (c=0;c<sx-dist;c++,pi++,pid++)
			{
				hist[*pi + *(pid)*bins]++;
				hist[*pid + *(pi)*bins]++;//simetría
			}
			
		}
		
	}
	else
	{
		//For each row
		for (r=0;r<sy;r++,p+=h->widthStep)
		{
			pi=p;
			pid=pi+dist*h->widthStep;
			//For each column
			for (c=0;c<sx;c++,pi+=h->widthStep,pid+=h->widthStep)
			{
				hist[*pi + *(pid)*bins]++;
				hist[*pid + *(pi)*bins]++;//simetría
			}
			
		}
	}
	

	
	
	return hist;
}


};

#endif

