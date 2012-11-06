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

	Utilities to warp BYTE images. Readapted from ENCARA2 .
	
	Modesto Castrillón Santana (originally written by Jorge Cabrera)
	January 2005

****************************************************************************************/

#ifndef	_WARPINGIMAGE
#define _WARPINGIMAGE

#include "ENCARA2_2lib_defs.h"


#ifndef BYTE
typedef unsigned char       BYTE;
//#define BYTE unsigned char
#endif

//! Utilities to warp BYTE images
/*!
Utilities to warp BYTE images. Originally written by Jorge Cabrera, adapted by Modesto Castrillón
*/
class CWarpingImage
{

public:
	//! Class constructor
	/*
	\param iX Width
	\param iY Height
	*/
	CWarpingImage(int iX,int iY);
	//! Class constructor partocularized for normalized faces
	/*
	\param iX Width
	\param iY Height
	\param OIx Left eye x coordinate
	\param OIy Left eye y coordinate
	\param ODx Right eye x coordinate
	\param ODy Right eye y coordinate
	*/
	CWarpingImage(int iX,int iY,int OIx,int OIy,int ODx,int ODy);
	//! Class destructor
	~CWarpingImage();

	
  	//! Rotates an image
	/*!
	Rotates an image around a point using three displacements (ref. Digital image processing, Wolberg))
	\param BYTEcara		Input image
	\param BYTEcaraR	Output image
	\param foveax		Input image width
	\param foveay		Input image height
	\param cx			Center of rotation x coordinate
	\param cy			Center of rotation y coordinate
	\param ix			Upper left x coordinate of the window to rotate
	\param iy			Upper left y coordinate of the window to rotate
	\param dx			Bottom right x coordinate of the window to rotate
	\param dy			Bottom right y coordinate of the window to rotate
	\param th			Angle to rotate (radians)
	*/
	void Rotar(BYTE *BYTEcara,BYTE **BYTEcaraR,int foveax,int foveay,int cx, int cy,int ix,int iy,int dx, int dy,float th);
	//! Rotates an image using integer aritmecy.
	/*!
	Rotates an image around a point using three displacements (ref. Digital image processing, Wolberg)). The use of
	integer aritmecy provokes aliasing
	\param BYTEcara		Input image
	\param BYTEcaraR	Output image
	\param foveax		Input image width
	\param foveay		Input image height
	\param cx			Center of rotation x coordinate
	\param cy			Center of rotation y coordinate
	\param ix			Upper left x coordinate of the window to rotate
	\param iy			Upper left y coordinate of the window to rotate
	\param dx			Bottom right x coordinate of the window to rotate
	\param dy			Bottom right y coordinate of the window to rotate
	\param th			Angle to rotate (radians)
	*/
	void RotarInt(BYTE *BYTEcara,BYTE **BYTEcaraR,int foveax,int foveay,int cx, int cy,int ix,int iy,int dx, int dy,float th);
	//! Rotates a point
	/*!
	Rotates a point around a center of rotation
	\param cx			Center of rotation x coordinate
	\param cy			Center of rotation y coordinate
	\param px			x coordinate of the point to rotate
	\param py			y coordinate of the point to rotate
	\param th			Angle to rotate (radians)
	*/
	void Rotarxy(int cx, int cy,int *px,int *py,float th);
	//! Normalizes image according to eye positions
	/*!
	\param ppPixels Input image
	\param isx Input image width
	\param isy Input image height
	\param iOjoIX Left eye x coordinate
	\param iOjoIY Left eye y coordinate
	\param iOjoDX Right eye x coordinate
	\param iOjoDY Right eye y coordinate
	\param mean	Normalized image mean value
	*/
	void NormalizaOjos(BYTE **ppPixels,int isx, int isy,int iOjoIX,int iOjoIY,int iOjoDX,int iOjoDY,int *mean);
 
  //! Normalizes image according to eye and chin positions
	/*!
	\param ppPixels Input image
	\param isx Input image width
	\param isy Input image height
	\param iOjoIX Left eye x coordinate
	\param iOjoIY Left eye y coordinate
	\param iOjoDX Right eye x coordinate
	\param iOjoDY Right eye y coordinate
	\param iBarbillaX Chin x coordinate
	\param iBarbillaY Chin eye y coordinate
	\param OJOIX Normalized left eye x coordinate
	\param OJOIY Normalized left eye y coordinate
	\param OJODX Normalized right eye x coordinate
	\param OJODY Normalized right eye y coordinate
	\param BARBX Normalized chin x coordinate
	\param BARBY Normalized chin eye y coordinate
	\param mean	Normalized image mean value
	*/	
	void NormalizaOjos(BYTE **ppPixels,int isx, int isy,int iOjoIX,int iOjoIY,int iOjoDX,int iOjoDY,
	int iBarbillaX, int iBarbillaY,int OJOIX,int OJOIY,int OJODX,int OJODY, int BARBX, int BARBY,int *mean);

	//! Normalizes image according to eye positions
	/*!
	Normalizes image according to eye positions and applies an elliptical mask if requested
	\param pPixels Input image
	\param isx Input image width
	\param isy Input image height
	\param iOjoIX Left eye x coordinate
	\param iOjoIY Left eye y coordinate
	\param iOjoDX Right eye x coordinate
	\param iOjoDY Right eye y coordinate
	\param mean	Normalized image mean value
	\param boMascaraEliptica If set applies a mask to face images
	\param boIlluminationNormalization If set indicates that the illumination is normalized
	*/
	void NormalizaOjos(BYTE *pPixels,int isx, int isy,
		int iOjoIX,int iOjoIY,int iOjoDX,int iOjoDY,int *mean, 
		bool boMascaraEliptica=true,bool boIlluminationNormalization=false);
	
	//! Scales an image
	/*!	
	\param pPixels Input image
	\param isx Input image width
	\param isy Input image height
	*/
	void TransformaEscala(BYTE *pPixels,int isx, int isy);


	BYTE	*pbImage;	//! Image
	BYTE	*pbImageMasked;	//! Image masked (only available if it was set)
	
	
private:
	void ResuelveEscaladoTraslacion(int OIx, int OIy,int ODx, int ODy, float*out,int OInx, int OIny,int ODnx, int ODny);
	void resample_rows( BYTE * INi, BYTE * OUTi, int INlen, int OUTlen);	//Warping images
	void resample_columns( BYTE * INi, BYTE * OUTi, int INlen, int OUTlen,int INjmp, int OUTjmp);
	void Remap(BYTE *pbInImage,int sx,int sy);
	void EllipticalMask();

	int		iSx,iSy;
	int 	OJOIX,OJOIY,OJODX,OJODY; //Eye normalized positions

	BYTE	*pbEllipticalMask;

};

#endif



