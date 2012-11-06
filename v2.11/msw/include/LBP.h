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
 /*********************************************************************
 * Copyright (C) 2002 Maenpaa
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.Redistributions of source code must retain all copyright
 *   notices, this list of conditions and the following disclaimer.
 *
 * 2.Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following
 *   disclaimer in the documentation and/or other materials provided
 *   with the distribution.
 *
 * 3.The name(s) of the author(s) may not be used to endorse or
 *   promote products derived from this software without specific
 *   prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

#ifndef _LBP_H
#define _LBP_H

#ifdef ENCARA2INLINUX
#include <values.h>
#endif

#include <math.h>

//M_PI defined? Needed for windows
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//LBP codes
#define LBP_ORIGINAL	0//0-255 considering the binary code
#define LBP_UNIFORM		1//Rotation invariant and considering only those significant codes. Uses only the first ten positions of the histogram
#define LBP_SIMPLIFIED	2//After the VISAPP paper, simply adds the bits. Applies an offset to see the resulting LBP image
#define LBP_SIMPLIFIED_COMPACT	3//After the VISAPP paper, simply adds the bits. Uses only the first nine positions of the histogram


//Look table para uniform patterns
static const unsigned char uni_lookup[256]={
	0,1,1,2,1,9,2,3,1,9,9,9,2,9,3,4,	//0x00 0x0F (11)
	1,9,9,9,9,9,9,9,2,9,9,9,3,9,4,5,	//0x10 0x1F (5)
	1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,	//0x20 0x2F (1)
	2,9,9,9,9,9,9,9,3,9,9,9,4,9,5,6,	//0x30 0x3F (5)
	1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,	//0x40 0x4F (1)
	9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,	//0x50 0x5F
	2,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,	//0x60 0x6F (1)
	3,9,9,9,9,9,9,9,4,9,9,9,5,9,6,7,	//0x70 0x7F (5)
	1,2,9,3,9,9,9,4,9,9,9,9,9,9,9,5,	//0x80 0x8F (5)
	9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,	//0x90 0x9F
	9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,	//0xA0 0xAF
	9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,	//0xB0 0xBF
	2,3,9,4,9,9,9,5,9,9,9,9,9,9,9,6,	//0xC0 0xCF (5)
	9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,	//0xD0 0xDF 
	3,4,9,5,9,9,9,6,9,9,9,9,9,9,9,7,	//0xE0 0xEF (5)
	4,5,9,6,9,9,9,7,5,6,9,7,6,7,7,8		//0xF0 0xFF (11)
};

#include <string.h>

/* Compare a value pointed to by 'ptr' to the 'center' value and
 * increment pointer. Comparison is made by masking the most
 * significant bit of an integer (the sign) and shifting it to an
 * appropriate position. */
#define compab_mask_inc(ptr,shift) { value |= ((unsigned int)(*center - *ptr - 1) & 0x80000000) >> (31-shift); ptr++; }

/* Compare a value 'val' to the 'center' value. */
#define compab_mask(val,shift) { value |= ((unsigned int)(*center - (val) - 1) & 0x80000000) >> (31-shift); }

/* Predicate 1 for the 3x3 neighborhood */
#define predicate 1

/* The number of bits */
#define bits 8

typedef struct
{
	int x,y;
} integerpoint;

typedef struct
{
	double x,y;
} doublepoint;


class CLBP
{
public:

	CLBP()
	{
	};
	~CLBP()
	{
	};



////////////////////////////////////////////////////////////////////////////////////////////////
//			ORIGINAL CODE PROVIDED BY AUTHORS READY FOR AN int IMAGE
///////////////////////////////////////////////////////////////////////////////////////////////

	/*
	* Get a bilinearly interpolated value for a pixel.
	*/
#ifdef ENCARA2INLINUX
	inline double interpolate_at_ptr(int* upperLeft, int i, int columns)
#else
	double interpolate_at_ptr(int* upperLeft, int i, int columns)
#endif
	{
		double dx = 1-offsets[i].x;
		double dy = 1-offsets[i].y;
		return
			*upperLeft*dx*dy +
			*(upperLeft+1)*offsets[i].x*dy +
			*(upperLeft+columns)*dx*offsets[i].y +
			*(upperLeft+columns+1)*offsets[i].x*offsets[i].y;
	}
	

	/*
	* Get a bilinearly interpolated value for a pixel.
	*/
#ifdef ENCARA2INLINUX
	inline double interpolate_at_ptr(unsigned char* upperLeft, int i, int columns)
#else
	double interpolate_at_ptr(unsigned char* upperLeft, int i, int columns)
#endif
	{
		double dx = 1-offsets[i].x;
		double dy = 1-offsets[i].y;
		return
			*upperLeft*dx*dy +
			*(upperLeft+1)*offsets[i].x*dy +
			*(upperLeft+columns)*dx*offsets[i].y +
			*(upperLeft+columns+1)*offsets[i].x*offsets[i].y;
	}
/*
 * Calculate the point coordinates for circular sampling of the neighborhood.
 */
void calculate_points(void)
{
	double step = 2 * M_PI / bits, tmpX, tmpY;
	int i;
	for (i=0;i<bits;i++)
		{
			tmpX = predicate * cos(i * step);
			tmpY = predicate * sin(i * step);
			points[i].x = (int)tmpX;
			points[i].y = (int)tmpY;
			offsets[i].x = tmpX - points[i].x;
			offsets[i].y = tmpY - points[i].y;
			if (offsets[i].x < 1.0e-10 && offsets[i].x > -1.0e-10) // rounding error 
				offsets[i].x = 0;
			if (offsets[i].y < 1.0e-10 && offsets[i].y > -1.0e-10) // rounding error 
				offsets[i].y = 0;
			
			if (tmpX < 0 && offsets[i].x != 0)
				{
					points[i].x -= 1;
					offsets[i].x += 1;
				}
			if (tmpY < 0 && offsets[i].y != 0)
				{
					points[i].y -= 1;
					offsets[i].y += 1;
				}
		}
}

/*
 * Calculate the LBP histogram for an integer-valued image. This is an
 * optimized version of the basic 8-bit LBP operator. Note that this
 * assumes 4-byte integers. In some architectures, one must modify the
 * code to reflect a different integer size.
 * 
 * img: the image data, an array of rows*columns integers arranged in
 * a horizontal raster-scan order
 * rows: the number of rows in the image
 * columns: the number of columns in the image
 * result: an array of 256 integers. Will hold the 256-bin LBP histogram.
 * interpolated: if != 0, a circular sampling of the neighborhood is
 * performed. Each pixel value not matching the discrete image grid
 * exactly is obtained using a bilinear interpolation. You must call
 * calculate_points (only once) prior to using the interpolated version.
 * return value: result
 */
void lbp_histogram(int* img, int rows, int columns, int* result, int interpolated)
{
	int leap = columns*predicate;	//Distance among rows

	//Set up a circularly indexed neighborhood using nine pointers.

	//For predicate 1, i.e. 8 neighbours would be p0 p1 p2
	//					      p7 ce p3
	//					      p6 p5 p4
	int
		*p0 = img,			
		*p1 = p0 + predicate,
		*p2 = p1 + predicate,
		*p3 = p2 + leap,
		*p4 = p3 + leap,
		*p5 = p4 - predicate,
		*p6 = p5 - predicate,
		*p7 = p6 - leap,
		*center = p7 + predicate;

	unsigned int value;//8 bits code obtained

	int pred2 = predicate << 1;

	int r,c;

	//Initializes histogram
	memset(result,0,sizeof(int)*256); // Clear result histogram 
		
	if (!interpolated)
	{
		//For each row
		for (r=0;r<rows-pred2;r++)
		{
			//For each column
			for (c=0;c<columns-pred2;c++)
			{
				value = 0;

				//Unrolled loop 
				compab_mask_inc(p0,0);
				compab_mask_inc(p1,1);
				compab_mask_inc(p2,2);
				compab_mask_inc(p3,3);
				compab_mask_inc(p4,4);
				compab_mask_inc(p5,5);
				compab_mask_inc(p6,6);
				compab_mask_inc(p7,7);
				center++;

				result[value]++; // Increase histogram bin value 
			}
			p0 += pred2;
			p1 += pred2;
			p2 += pred2;
			p3 += pred2;
			p4 += pred2;
			p5 += pred2;
			p6 += pred2;
			p7 += pred2;
			center += pred2;
		}
	}
	else
		{
			p0 = center + points[5].x + points[5].y * columns;
			p2 = center + points[7].x + points[7].y * columns;
			p4 = center + points[1].x + points[1].y * columns;
			p6 = center + points[3].x + points[3].y * columns;
				
			for (r=0;r<rows-pred2;r++)
				{
					for (c=0;c<columns-pred2;c++)
						{
							value = 0;

							// Unrolled loop 
							compab_mask_inc(p1,1);
							compab_mask_inc(p3,3);
							compab_mask_inc(p5,5);
							compab_mask_inc(p7,7);

							// Interpolate corner pixels 
							compab_mask((int)(interpolate_at_ptr(p0,5,columns)+0.5),0);
							compab_mask((int)(interpolate_at_ptr(p2,7,columns)+0.5),2);
							compab_mask((int)(interpolate_at_ptr(p4,1,columns)+0.5),4);
							compab_mask((int)(interpolate_at_ptr(p6,3,columns)+0.5),6);
							p0++;
							p2++;
							p4++;
							p6++;
							center++;
								
							result[value]++;
						}
					p0 += pred2;
					p1 += pred2;
					p2 += pred2;
					p3 += pred2;
					p4 += pred2;
					p5 += pred2;
					p6 += pred2;
					p7 += pred2;
					center += pred2;
				}
		}

		
}


////////////////////////////////////////////////////////////////////////////////////////////////
//			END ORIGINAL CODE PROVIDED BY AUTHORS
///////////////////////////////////////////////////////////////////////////////////////////////

//Functions adapted for unsigned char IplImage by Modesto Castrillón (mcastrillon@iusiani.ulpgc.es)


/*!	Computes LBP histogram for a window given an IplImage
	
	\param	h			Input image
	\param	x			x coordinate of the upper left corner
	\param	y			y coordinate of the upper left corner
	\param 	sx			Window width
	\param 	sy			Window height
*/
int * GetLBPHistogram(IplImage *h,int x, int y, int sx,int sy)
{
	//It must be a gray image
	if (h->nChannels>1) return 0;

	int *hist=NULL;

	if (h->depth==8)//only for unsigned char images
	{
		//Allocs the histogram
		hist=(int *) new unsigned char [256*sizeof(int)];

		//Computes histogram
		lbp_histogram(h,x,y,sx,sy,hist,0);
	}

	return hist;

}


/*!	Computes LBP histogram for a window
	
	\param	h			Input image
	\param	interpolated	If set, the points are interpolated using the neighbors
	\param  Type		LBP_ORIGINAL, LBP_UNIFORM or LBP_SIMPLIFIED
*/
IplImage * GetLBPImage(IplImage *h, int interpolated=0, int Type=LBP_ORIGINAL, IplImage *hlbp=NULL)
{
	IplImage *haux;

	//It must be a gray image
	if (h->nChannels>1)
	{
		if (h->nChannels<3) return 0; //It is no RGB
		else 
		{
			haux=cvCreateImage( cvSize(h->width,h->height), IPL_DEPTH_8U, 1 );	
			//Converts to a single gray channel
			cvCvtColor(h,haux,CV_BGR2GRAY );		
		}
	}else haux=h;

	IplImage *lbpimg=NULL;


	if (h->depth==8)//unsigned char images
	{
		//Computes lbp image
		lbpimg=lbp_image(haux,interpolated,Type,hlbp);
	}
	else//Nothing is done in this version for other types
	{
	}

	//deallocs if haux was newallocated
	if (h->nChannels>1 && h->nChannels<3) cvReleaseImage(&haux);


	return lbpimg;

}


/*!	Computes LBP histogram for a window
	
	\param	h			Input image
	\param	x			x coordinate of the upper left corner
	\param	y			y coordinate of the upper left corner
	\param 	sx			Window width
	\param 	sy			Window height
	\param  Xtracue		If different from NULL, a joint histogram is computed with this additional cue (e.g. color)
	\param  bins		Number of bins for the additional cue in the joint histogram

*/
int * GetLBPHistogramFromLBP(IplImage *lbp,int x, int y, int sx,int sy, IplImage *Xtracue=NULL,int bins=10)
{
	//It must be a gray image
	if (lbp==NULL) return 0;
	if (lbp->nChannels>1) return 0;
	if (lbp->depth!=8) return 0;

	//The number of bins should be optimized according to the code used
	int *hist=(int *) new unsigned char [256*sizeof(int)];

	//Computes histogram
	if (Xtracue==NULL)
	{		
		lbp_histogram_from_lbp_image(lbp,x,y,sx,sy,hist);
	}
	else
	{
		lbp_histogram_from_lbp_image(lbp,Xtracue,x,y,sx,sy,hist,bins);
	}
	
	return hist;
}



/*!	Computes normalized LBP histogram for a window
	
	\param	h			Input image
	\param	x			x coordinate of the upper left corner
	\param	y			y coordinate of the upper left corner
	\param 	sx			Window height
	\param 	sy			Window width
	\param Xtracue		Color likelihood image
	\param mask			Mask image
	\param bins			Number of bins in the joint histogram
*/
float * GetLBPHistogramFromLBPf(IplImage *lbp,int x, int y, int sx,int sy, IplImage *Xtracue=NULL,IplImage * mask=NULL,int bins=10)
{
	//It must be a gray image
	if (lbp==NULL) return 0;
	if (lbp->nChannels>1) return 0;
	if (lbp->depth!=8) return 0;

	//Ojo, si es invariante ante rotaciones no hacen falta 256 posiciones !!!
	float *hist=NULL;
	
	hist=(float *) new unsigned char [256*sizeof(float)];

	//Computes histogram
	if (Xtracue==NULL)
	{		
		lbp_histogram_from_lbp_image(lbp,x,y,sx,sy,hist);
	}
	else
	{
		if (mask==NULL)
			lbp_histogram_from_lbp_image(lbp,Xtracue,x,y,sx,sy,hist,bins);
		else
			lbp_histogram_from_lbp_image(lbp,Xtracue,mask,x,y,sx,sy,hist,bins);
	}
	
	return hist;
}

/*!	Computes normalized LBP histogram for a window
	
	\param	h			Input image
	\param	x			x coordinate of the upper left corner
	\param	y			y coordinate of the upper left corner
	\param 	sy			Window height
	\param 	sx			Window width
	\param 	hist		Returned histogram
	\param Xtracue		Color likelihood image
	\param mask			Mask image
	\param bins			Number of bins in the joint histogram
*/
void GetLBPHistogramFromLBPf(IplImage *lbp,int x, int y, int sx,int sy, float **hist,IplImage *Xtracue=NULL,IplImage * mask=NULL,int bins=10)
{
	//It must be a gray image
	if (lbp==NULL) return;
	if (lbp->nChannels>1) return;
	if (lbp->depth!=8) return;

	//Ojo, si es invariante ante rotaciones no hacen falta 256 posiciones !!!
	if (*hist==NULL)
		*hist=(float *) new unsigned char [256*sizeof(float)];

	//Computes histogram
	if (Xtracue==NULL)
	{		
		lbp_histogram_from_lbp_image(lbp,x,y,sx,sy,*hist);
	}
	else
	{
		if (mask==NULL)
			lbp_histogram_from_lbp_image(lbp,Xtracue,x,y,sx,sy,*hist,bins);
		else
			lbp_histogram_from_lbp_image(lbp,Xtracue,mask,x,y,sx,sy,*hist,bins);
	}
	
}


void GetLBPHistogramFromLBPGridf(IplImage *lbp,int dim, float **hist,int *ndims,int LBPcode)
{
	//Obtenemos el número de histogramas en base a la dimensión y al tamaño de la imagen
	int hhor=lbp->width/dim;
	int hver=lbp->height/dim;

	//Devuelve el número de ventanas de tamaño dim que caben en la imagen
	*ndims=hhor*hver;

	//Offset en cada dimensión en base a los histogramas por dimensión
	int offh=(lbp->width-hhor*dim)/2;
	int offv=(lbp->height-hver*dim)/2;

	//Alojamos el histograma temporal
	float *histaux=(float *) new unsigned char [256*sizeof(float)];	

	//alojamos el que resultará tras concatenar
	int ncomps;
	switch (LBPcode)
	{
		case LBP_UNIFORM:
			ncomps=10;
			break;
		case LBP_SIMPLIFIED_COMPACT:
			ncomps=9;
			break;
		case LBP_ORIGINAL:
		case LBP_SIMPLIFIED:
		default:
			ncomps=256;
			break;
	}
	*hist=(float *) new unsigned char [ncomps*sizeof(float)*(*ndims)];

	float *dst=*hist;

	int ofh;
	
	//Vamos copiando para cada ventanita el histograma en el histograma concatenado global
	for (int i=0;i<hver;i++)
	{
		ofh=offh;
		for (int j=0;j<hhor;j++,ofh+=dim,dst+=ncomps)
		{
			//Obtengo histograma (devuelve histograma de 256 posiciones)
			GetLBPHistogramFromLBPf(lbp,ofh,offv,dim,dim,&histaux);

			//Copio al histograma concatenado en base al tipo LBP escogido
			memcpy(dst,histaux,ncomps*sizeof(float));//sólo copia en base a las componentes

		}
		offv+=dim;//Nueva fila
	}

	delete [] histaux;
}


private:

/*Adapted from original to IplImage by Modesto
 * Calculate the LBP histogram for an unsigned char-valued IplImage in a given window.
 * This is an optimized version of the basic 8-bit LBP operator. 
 * 
 * img: the image data, an array of rows*columns integers arranged in
 * a horizontal raster-scan order
 * rows: the number of rows in the window
 * columns: the number of columns in the window
 * result: an array of 256 integers. Will hold the 256-bin LBP histogram.
 * interpolated: if != 0, a circular sampling of the neighborhood is
 * performed. Each pixel value not matching the discrete image grid
 * exactly is obtained using a bilinear interpolation. You must call
 * calculate_points (only once) prior to using the interpolated version.
 * return value: result
 */
void lbp_histogram(IplImage* img, int offx, int offy,int columns, int rows, int* result, int interpolated)
{

	int leap = img->widthStep*predicate;	//Distance among rows (tested for radius 1)

	//Set up a circularly indexed neighborhood using nine pointers.

	//For predicate 1, i.e. 8 neighbours would be p0 p1 p2
	//					      p7 ce p3
	//					      p6 p5 p4
	//int
	unsigned char
		*p0 = (unsigned char *)img->imageData + img->widthStep*offy +offx ,			
		*p1 = p0 + predicate,
		*p2 = p1 + predicate,
		*p3 = p2 + leap,
		*p4 = p3 + leap,
		*p5 = p4 - predicate,
		*p6 = p5 - predicate,
		*p7 = p6 - leap,
		*center = p7 + predicate;

	unsigned int value;//8 bits code obtained

	int pred2 = predicate << 1;

	int r,c;

	//Initializes histogram
	memset(result,0,sizeof(int)*256); // Clear result histogram 
		
	if (!interpolated)
	{
		//For each row
		for (r=0;r<rows-pred2;r++)
		{
			//For each column
			for (c=0;c<columns-pred2;c++)
			{
				value = 0;

				//Unrolled loop 
				compab_mask_inc(p0,0);
				compab_mask_inc(p1,1);
				compab_mask_inc(p2,2);
				compab_mask_inc(p3,3);
				compab_mask_inc(p4,4);
				compab_mask_inc(p5,5);
				compab_mask_inc(p6,6);
				compab_mask_inc(p7,7);
				center++;

				result[value]++; // Increase histogram bin value 
			}
			p0 += img->widthStep-(columns-pred2);	//Pruebas con iplimage
			p1 += img->widthStep-(columns-pred2);
			p2 += img->widthStep-(columns-pred2);
			p3 += img->widthStep-(columns-pred2);
			p4 += img->widthStep-(columns-pred2);
			p5 += img->widthStep-(columns-pred2);
			p6 += img->widthStep-(columns-pred2);
			p7 += img->widthStep-(columns-pred2);
			center += img->widthStep-(columns-pred2);
		}
	}
	else//This part has not been checked for unsigned char images
		{
			p0 = center + points[5].x + points[5].y * columns;
			p2 = center + points[7].x + points[7].y * columns;
			p4 = center + points[1].x + points[1].y * columns;
			p6 = center + points[3].x + points[3].y * columns;
				
			for (r=0;r<rows-pred2;r++)
				{
					for (c=0;c<columns-pred2;c++)
						{
							value = 0;

							// Unrolled loop 
							compab_mask_inc(p1,1);
							compab_mask_inc(p3,3);
							compab_mask_inc(p5,5);
							compab_mask_inc(p7,7);

							// Interpolate corner pixels 
							compab_mask((int)(interpolate_at_ptr(p0,5,columns)+0.5),0);
							compab_mask((int)(interpolate_at_ptr(p2,7,columns)+0.5),2);
							compab_mask((int)(interpolate_at_ptr(p4,1,columns)+0.5),4);
							compab_mask((int)(interpolate_at_ptr(p6,3,columns)+0.5),6);
							p0++;
							p2++;
							p4++;
							p6++;
							center++;
								
							result[value]++;
						}
					p0 += pred2;
					p1 += pred2;
					p2 += pred2;
					p3 += pred2;
					p4 += pred2;
					p5 += pred2;
					p6 += pred2;
					p7 += pred2;
					center += pred2;
				}
		}

		
}

/*
 * Calculate the non normalized LBP histogram in a window given the precomputed LBP image.
 * 
 * img: the lbp image data, an array of rows*columns integers arranged in
 * a horizontal raster-scan order
 * offx, offy: upper left corner of the window
 * rows: the number of rows in the window
 * columns: the number of columns in the window
 * result: an array of 256 integers. Will hold the 256-bin LBP histogram.
 * return value: result
 */
void lbp_histogram_from_lbp_image(IplImage* lbpimg, int offx, int offy,int columns, int rows, int* result)
{
	unsigned char * pi= (unsigned char *)lbpimg->imageData;

	pi=pi+offy*lbpimg->widthStep+offx;
	
	int r,c;

	//Initializes histogram
	memset(result,0,sizeof(int)*256); // Clear result histogram 
		
	//Computes histogram
	//For each row
	for (r=0;r<rows;r++)
	{
		//For each column
		for (c=0;c<columns;c++,pi++)
		{	
				result[*pi]++; // Increase histogram bin value 
		}
		pi+=lbpimg->widthStep-columns;
	}
		
}


/*
 * Calculate the joint histogram combining the LBP uniform image and 
 * an additional cue, e.g. color likelihood
 * 
 * img: the lbp image data, an array of rows*columns integers arranged in
 * a horizontal raster-scan order
 * Xtracue: colorliklihood image
 * rows: the number of rows in the image
 * columns: the number of columns in the image
 * result: an array of 256 integers. Will hold the 10 (rotation invariant LBP codes) histogram
 * 
 * return value: result
 */
void lbp_histogram_from_lbp_image(IplImage* lbpimg, IplImage *Xtracue, int offx, int offy,int columns, int rows, int* result, int bins)
{
	unsigned char * pi= (unsigned char *)lbpimg->imageData;
	unsigned char * pc= (unsigned char *)Xtracue->imageData;

	//LBP image pointer, each pixel has its corresponding LBP code
	pi=pi+offy*lbpimg->widthStep+offx;
	//extra cue image pointer
	pc=pc+offy*Xtracue->widthStep+offx;
	
	int r,c;

	int histstep=255/bins;	//steps for the likelihood images according to the number of bins indicated

	//Initializes histogram
	memset(result,0,sizeof(int)*256); // Clear result histogram 
		
	//Computes histogram
	//For each row
	for (r=0;r<rows;r++)
	{
		//For each column
		for (c=0;c<columns;c++,pi++,pc++)
		{	
			result[*pi + ((*pc)/histstep)*bins]++; // Increase histogram bin value 
		}
		pi+=lbpimg->widthStep-columns;
		pc+=Xtracue->widthStep-columns;
	}
		
}

/*
 * Calculate the LBP histogram in a window given the precomputed nonuniform LBP image.
 * 
 * The resulting histogram is float and normalized according to the total number of samples (window size)
 * img: the lbp image data, an array of rows*columns integers arranged in
 * a horizontal raster-scan order
 * rows: the number of rows in the image
 * columns: the number of columns in the image
 * result: an array of 256 integers. Will hold the 256-bin LBP histogram.
 * return value: result
 */
void lbp_histogram_from_lbp_image(IplImage* lbpimg, int offx, int offy,int columns, int rows, float* result)
{
	unsigned char * pi= (unsigned char *)lbpimg->imageData;

	pi=pi+offy*lbpimg->widthStep+offx;
	
	int r,c;

	int ncounted=rows*columns;//Number of pixels counted, useful to normalize the histogram

	//Initializes histogram
	memset(result,0,sizeof(float)*256); // Clear result histogram 
		
	//Computes histogram
	//For each row
	for (r=0;r<rows;r++)
	{
		//For each column
		for (c=0;c<columns;c++,pi++)
		{				
			result[*pi]++; // Increase histogram bin value 			
		}
		pi+=lbpimg->widthStep-columns;
	}

	//Histogram normalization
	if (ncounted)
		for (r=0;r<256;r++)
			result[r]/=ncounted;	
}

/*
 * Calculate the joint histogram combining the LBP uniform image and 
 * an additional cue, e.g. color likelihood
 * 
 * img: the lbp image data, an array of rows*columns integers arranged in
 * a horizontal raster-scan order
 * Xtracue: colorliklihood image
 * rows: the number of rows in the image
 * columns: the number of columns in the image
 * result: an array of 256 floats. Will hold 10 (LBP codes) * bins histogram. In this version it is normalized
 * 
 * return value: result
 */
void lbp_histogram_from_lbp_image(IplImage* lbpimg, IplImage *Xtracue, int offx, int offy,int columns, int rows, float* result, int bins)
{
	unsigned char * pi= (unsigned char *)lbpimg->imageData;
	unsigned char * pc= (unsigned char *)Xtracue->imageData;

	//LBP image pointer, each pixel has its corresponding LBP code
	pi=pi+offy*lbpimg->widthStep+offx;
	//extra cue image pointer
	pc=pc+offy*Xtracue->widthStep+offx;
	
	int r,c;

	int histstep=255/bins;	//steps for the likelihood images according to the number of bins indicated

	int ncounted=rows*columns;//Number of pixles counted, useful to normalize the histogram

	//Initializes histogram
	memset(result,0,sizeof(float)*256); // Clear result histogram 
		
	//Computes histogram
	//For each row
	for (r=0;r<rows;r++)
	{
		//For each column
		for (c=0;c<columns;c++,pi++,pc++)
		{	
			result[*pi + ((*pc)/histstep)*bins]++; // Increase histogram bin value 
		}
		pi+=lbpimg->widthStep-columns;
		pc+=Xtracue->widthStep-columns;
	}
	
	if (ncounted)
		for (r=0;r<256;r++)
			result[r]/=(float)ncounted;
}

/*
 * Calculate the joint histogram combining the LBP uniform image and 
 * an additional cue, e.g. color likelihood
 * 
 * img: the lbp image data, an array of rows*columns integers arranged in
 * a horizontal raster-scan order
 * Xtracue: colorliklihood image
 * mask: A mask image indicating the pixels to use
 * rows: the number of rows in the image
 * columns: the number of columns in the image
 * result: an array of 256 floats. Will hold 10 (LBP codes) * bins histogram. In this version it is normalized
 * 
 * return value: result
 */
void lbp_histogram_from_lbp_image(IplImage* lbpimg, IplImage *Xtracue, IplImage *mask, int offx, int offy,int columns, int rows, float* result, int bins)
{
	unsigned char * pi= (unsigned char *)lbpimg->imageData;
	unsigned char * pc= (unsigned char *)Xtracue->imageData;
	unsigned char * pm= (unsigned char *)mask->imageData;

	//LBP image pointer, each pixel has its corresponding LBP code
	pi=pi+offy*lbpimg->widthStep+offx;
	//extra cue image pointer
	pc=pc+offy*Xtracue->widthStep+offx;
	//extra cue image pointer
	pm=pm+offy*mask->widthStep+offx;
	
	int r,c;

	int histstep=255/bins;	//steps for the likelihood images according to the number of bins indicated

	int ncounted=0;//Number of pixles counted, useful to normalize the histogram

	//Initializes histogram
	memset(result,0,sizeof(float)*256); // Clear result histogram 
		
	//Computes histogram
	//For each row
	for (r=0;r<rows;r++)
	{
		//For each column
		for (c=0;c<columns;c++,pi++,pc++,pm++)
		{	
			if (*pm)//Counts only those pixels that are not null in the mask
			{
				//OJO si usas LBP_ORIGINAL porque se sale del rango 255 al combinar ambos histogramas
				result[*pi + ((*pc)/histstep)*bins]++; // Increase histogram bin value 
				ncounted++;
			}
		}
		pi+=lbpimg->widthStep-columns;
		pc+=Xtracue->widthStep-columns;
		pm+=mask->widthStep-columns;
	}
	
	if (ncounted)
		for (r=0;r<256;r++)
			result[r]/=(float)ncounted;
	
}


/*Modificación propuesta por David

Mira, te paso la nueva definición de la función que nos permite la obtención más precisa en las esquinas y el acotamiento a zonas dentro de las imágenes. te lo paso por si quieres trabajar con ella o cualquier cosa.

void lbp_histogram_from_lbp_image(IplImage* lbpimg, int offx, int offy,int columns, int rows, float* result)
{
    unsigned char * pi=((unsigned char *)lbpimg->imageData)+offy+(offx*lbpimg->widthStep);
   
    int ncounter=0;//Number of pixles counted, useful to normalize the histogram

    //Initializes histogram
    memset(result,0,sizeof(float)*256); // Clear result histogram
       

    for(int mx=0;mx<rows;mx++){//en x (abajo)
        for(int my=0;my<columns;my++){//en y (->)
            if((mx+offx<lbpimg->height)&&(my+offy<lbpimg->width)){     //procesamos sí y solo sí el bin cae dentro de la imagen
                pi=((unsigned char *)lbpimg->imageData)+(offy+my)+((offx+mx)*lbpimg->widthStep);
                ncounter++;  //incrementamos ncounter
                result[*pi]++;
            }
        }
    }

    if (ncounter)
        for (int r=0;r<256;r++)
            result[r]/=ncounter;
   
}

*/

/*
 * Calculate the LBP image for a BYTE image. This is an
 * optimized version of the basic 8-bit LBP operator. Note that this
 * assumes 4-byte integers. In some architectures, one must modify the
 * code to reflect a different integer size.
 * 
 * img: the image data, 
 * interpolated: if != 0, a circular sampling of the neighborhood is
 * performed. Each pixel value not matching the discrete image grid
 * exactly is obtained using a bilinear interpolation. You must call
 * calculate_points (only once) prior to using the interpolated version.
 * return value: result
 */

IplImage *lbp_image(IplImage* img, int interpolated, int LBPType=LBP_ORIGINAL, IplImage *hlbp=NULL)
{
	//int leap = columns*predicate;	//Distance among rows

	//Creating the lbp image if it is not provided
	IplImage *lbpimg;

	if (hlbp==NULL)
		lbpimg=cvCreateImage( cvSize(img->width, img->height), IPL_DEPTH_8U, 1 );	
	else
		lbpimg=hlbp;

	cvSetZero(lbpimg);

	int leap = img->widthStep*predicate;	//Distance among rows

	//Set up a circularly indexed neighborhood using nine pointers.

	//For predicate 1, i.e. 8 neighbours would be p0 p1 p2
	//											  p7 ce p3
	//											  p6 p5 p4
	//int
	unsigned char
		*p0 = (unsigned char *)img->imageData,			
		*p1 = p0 + predicate,
		*p2 = p1 + predicate,
		*p3 = p2 + leap,
		*p4 = p3 + leap,
		*p5 = p4 - predicate,
		*p6 = p5 - predicate,
		*p7 = p6 - leap,
		*center = p7 + predicate;

	//Output pointer leaves outer ring as null
	unsigned char *po=(unsigned char *)lbpimg->imageData;
	po=po+lbpimg->widthStep+1;

	unsigned char value;//8 bits code obtained

	int pred2 = predicate << 1;

	int r,c;

	int rowshift=img->widthStep-(img->width-pred2);

	
	if (!interpolated)
	{
		//For each row
		for (r=0;r<img->height-pred2;r++)
		{
			//For each column
			for (c=0;c<img->width-pred2;c++,po++)
			{
				value = 0;

				//Unrolled loop 
				compab_mask_inc(p0,0);
				compab_mask_inc(p1,1);
				compab_mask_inc(p2,2);
				compab_mask_inc(p3,3);
				compab_mask_inc(p4,4);
				compab_mask_inc(p5,5);
				compab_mask_inc(p6,6);
				compab_mask_inc(p7,7);
				

				//Storing uniform or non uniform patterns (Iy would be faster to separate into two different funtions reducing the number of comparisons)
				switch (LBPType)
				{
				case LBP_UNIFORM:
					*po=uni_lookup[value];//Diez valores posibles 0-9
					break;
				case LBP_SIMPLIFIED://Adds the number of 1s instead of adding their multiples (ver Qian Tao)
					*po= ((value & 0x01)+ ( (value>>1) & 0x01)+ ( (value>>2) & 0x01)+ ( (value>>3) & 0x01)
						+ ( (value>>4) & 0x01)+ ( (value>>5) & 0x01)+ ( (value>>6) & 0x01)+ ( (value>>7) & 0x01))<<4;//desplaza para visualizar 
					break;
				case LBP_SIMPLIFIED_COMPACT://Adds the number of 1s instead of adding their multiples (ver Qian Tao)
					*po= ((value & 0x01)+ ( (value>>1) & 0x01)+ ( (value>>2) & 0x01)+ ( (value>>3) & 0x01)
						+ ( (value>>4) & 0x01)+ ( (value>>5) & 0x01)+ ( (value>>6) & 0x01)+ ( (value>>7) & 0x01));//se queda con los 9 valores posibles 0-8
					break;

				default:
				case LBP_ORIGINAL:
					*po=value;
					break;
				}

				//Moving to the next pixel
				center++;
									
			}
			//Offset for the next row of the input image
			p0 += rowshift;
			p1 += rowshift;
			p2 += rowshift;
			p3 += rowshift;
			p4 += rowshift;
			p5 += rowshift;
			p6 += rowshift;
			p7 += rowshift;
			center += rowshift;

			//shifting pointer of the output image 
			po+=rowshift;
		}
	}
	else//This alternative not been checked for unsigned char images
		{
			p0 = center + points[5].x + points[5].y * img->width;
			p2 = center + points[7].x + points[7].y * img->width;
			p4 = center + points[1].x + points[1].y * img->width;
			p6 = center + points[3].x + points[3].y * img->width;
				
			for (r=0;r<img->height-pred2;r++)
				{
					for (c=0;c<img->width-pred2;c++,po++)
						{
							value = 0;

							// Unrolled loop 
							compab_mask_inc(p1,1);
							compab_mask_inc(p3,3);
							compab_mask_inc(p5,5);
							compab_mask_inc(p7,7);

							// Interpolate corner pixels 
							compab_mask((int)(interpolate_at_ptr(p0,5,img->width)+0.5),0);//ojo con columns si uso BYTE
							compab_mask((int)(interpolate_at_ptr(p2,7,img->width)+0.5),2);
							compab_mask((int)(interpolate_at_ptr(p4,1,img->width)+0.5),4);
							compab_mask((int)(interpolate_at_ptr(p6,3,img->width)+0.5),6);
							p0++;
							p2++;
							p4++;
							p6++;
							center++;
								
							*po=value;
						}
					p0 += pred2;
					p1 += pred2;
					p2 += pred2;
					p3 += pred2;
					p4 += pred2;
					p5 += pred2;
					p6 += pred2;
					p7 += pred2;
					center += pred2;

					po+=2;
				}
		}

		return lbpimg;
}




private:

	integerpoint points[bits];
	doublepoint offsets[bits];

};

#endif
