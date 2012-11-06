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

	Utilities for getting face info from an image

	Modesto Castrill� Santana
	January 2008

****************************************************************************************/
#ifndef	_IMAGEUTILS
#define _IMAGEUTILS

//OpenCV
//OPENCV
#ifndef OPENCVINCLUDED
#include "cv.h"
#include "cvaux.h"
#endif

#include "IplImageUtils.h"

#ifndef BYTE
#define BYTE unsigned char
#endif

class CImageUtils
{

public:
	CImageUtils()
	{
	};
	~CImageUtils()
	{
	};

//! Normalizes an image in relation to the given eyes location
/*!
\param imagegray Single channel image to normalize
\param lex x coordinate of left eye
\param ley y coordinate of left eye
\param rex x coordinate of right eye
\param rey y coordinate of right eye
\param norm_sx normalized image width
\param norm_sy normalized image height
\param norm_loc_leyex normalized left eye x coordinate
\param norm_loc_leyey normalized left eye y coordinate
\param norm_loc_reyex normalized right eye x coordinate
\param norm_loc_reyey normalized right eye y coordinate
\param boMask Set to apply a mask to the resulting normalized image (default false)
\param boEqualize Set to apply a histogram equalization (default false)
*/
	IplImage * GetNormalizedImageGivenEyes(IplImage *imagegray,int lex,int ley,int rex,int rey, int norm_sx=FACEX, int norm_sy=FACEY,int norm_loc_leyex=LEFTEYEX,int norm_loc_leyey=LEFTEYEY,int norm_loc_reyex=RIGHTEYEX,int norm_loc_reyey=RIGHTEYEY, bool boMask=false, bool boEqualize=false)
{
	CIplImageUtils	IplUtils;
	
	CMathUtils	MathUtils;			//! Aux variable for Math utilities

	//Allocates a temp image
	IplImage *imggray=cvCreateImage( cvSize(imagegray->width, imagegray->height), IPL_DEPTH_8U, 1 );

	//Gets angle for rotation according to eye location
	double xs=0,ys=0,angulo;
	float interx,intery;
	//
	angulo=MathUtils.GetAngle4Rotation(lex,ley,rex,rey,&interx,&intery);

	//Rota
	IplUtils.RotateImage(imagegray,imggray,(float)angulo,(float)interx,(float)intery);

	//Normalizes the image to a size of sx*sy pixels using eyes location
	CWarpingImage *cwiImage;
	int dummy;

	//Image warping
	cwiImage= new CWarpingImage(norm_sx,norm_sy,norm_loc_leyex,norm_loc_leyey,norm_loc_reyex,norm_loc_reyey);
	
	//Por si fuera impar pasamos antes a una BYTE * para luego tratar con CWarpingImage
	BYTE *btmp=new BYTE[imggray->width*imggray->height];

	IplUtils.CopyIpl2BYTE(imggray,btmp,imggray->width,imggray->height);

	//Eye locatins are rotated	
	float oixr,oiyr,odxr,odyr;

	float px,py;
	px=(float)lex;
	py=(float)ley;

	MathUtils.Rota2D((float)interx,(float)intery,(float)-angulo,&px,&py);
	oixr=px;
	oiyr=py;

	px=(float)rex;
	py=(float)rey;

	MathUtils.Rota2D((float)interx,(float)intery,(float)-angulo,&px,&py);
	odxr=px;
	odyr=py;

	//Image normalization
	cwiImage->NormalizaOjos(btmp,imggray->width,
			imggray->height,(int)oixr,(int)oiyr,
			(int)odxr,(int)odyr,&dummy,boMask,false);

	IplImage *IfFNormalizada=cvCreateImage( cvSize(norm_sx, norm_sy), IPL_DEPTH_8U, 1 );

	//From BYTE to IplImage depending is masked is active or not
	if (boMask)
	{
		IplUtils.CopyBYTE2Ipl(cwiImage->pbImageMasked,&IfFNormalizada,norm_sx,norm_sy);
	}
	else
	{
		IplUtils.CopyBYTE2Ipl(cwiImage->pbImage,&IfFNormalizada,norm_sx,norm_sy);
		//Histograma ecualizado (¿cómo hacerlo con máscara?)
		if (boEqualize)
			cvEqualizeHist(IfFNormalizada,IfFNormalizada);

	}


	delete cwiImage;
	cvReleaseImage(&imggray);
	delete [] btmp;

	return IfFNormalizada;

}


//! Applies elliptical mask to the image given
	/*!
	Applies elliptical mask to the image given in order to avoid background pixels
		\param h	Image to be masked
		\param eyerx x coordinate of the right (in the image) eye
		\param eyery y coordinate of the right (in the image) eye
		\param eyelx x coordinate of the left (in the image) eye
		\param eyely y coordinate of the left (in the image) eye

	*/
	void ApplyEllipticalMask(IplImage *h, int eyerx,int eyery,int eyelx,int eyely)
	{
		int i,j;
		int		incx,incy,px,py;
		float	sa,la,fval;

		//Makes use of heuristics related to the eye locations
		sa=(float)((eyerx-eyery)*1.15);
		la=(float)(sa*1.37);
		sa*=sa;
		la*=la;

		//Ellipse center is used to crop
		incy=(eyery+(int)((double)(eyerx-eyelx)*0.3));
		incx=((eyerx+eyelx)>>1);

		BYTE *p=(BYTE *)h->imageData,*pp;
		//Scans the whole image
		for (i=0;i<h->height;i++,p+=h->widthStep)
		{
			pp=p;
			for(j=0;j<h->width;j++,pp++){
					//Shifts according to center
					py=i-incy;
					px=j-incx;
					//It is checked if the pointer is sithin or not
					fval=((float)px*((float)px))/(sa)+((float)py*((float)py))/(la);
					if (fval>=1.0) //Applies the masked pixel
						*pp=125;
			}
		}
	}

//! Get the  elliptical mask 
	/*!
	Saves the elliptical mask to the image given in order to avoid background pixels
		\param mask	Image to be masked
		\param eyerx x coordinate of the right (in the image) eye
		\param eyery y coordinate of the right (in the image) eye
		\param eyelx x coordinate of the left (in the image) eye
		\param eyely y coordinate of the left (in the image) eye

	*/
	void GetEllipticalMask(IplImage *mask, int eyerx,int eyery,int eyelx,int eyely)
	{
		int i,j;
		int		incx,incy,px,py;
		float	sa,la,fval;

		//Makes use of heuristics related to the eye locations
		sa=(float)((eyerx-eyery)*1.15);
		la=(float)(sa*1.37);
		sa*=sa;
		la*=la;

		//Ellipse center is used to crop
		incy=(eyery+(int)((double)(eyerx-eyelx)*0.3));
		incx=((eyerx+eyelx)>>1);

		BYTE *p=(BYTE *)mask->imageData,*pp;
		//Scans the whole image
		for (i=0;i<mask->height;i++,p+=mask->widthStep)
		{
			pp=p;
			for(j=0;j<mask->width;j++,pp++){
					//Shifts according to center
					py=i-incy;
					px=j-incx;
					//It is checked if the pointer is sithin or not
					fval=((float)px*((float)px))/(sa)+((float)py*((float)py))/(la);
					if (fval>=1.0)
						*pp=125;
					else
						*pp=0;
			}
		}
	}



};

#endif

