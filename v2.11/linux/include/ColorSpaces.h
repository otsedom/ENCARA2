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
 #ifndef _COLSPC
#define _COLSPC


#include "ucs.h"
/**************************************************************

  
			   Working from an rgb or yuv image, being able to 
			   transform to yuv, rngn

						CeTSIA
						ULPGC
			October 2001 - Modesto Castrillón

**************************************************************/

//! Color space transformation utilities
class CRGBrgb{

public:
	//! Class constructor
	/*!
	\param r Red
	\param g Green
	\param b Blue
	*/
	CRGBrgb(int r, int g, int b)
	{
		RGBUpdatergb(r,g,b);
	};

	//! Class constructor
	CRGBrgb(){
	};

	//! Class destructor
	~CRGBrgb(){};

	//! Gets red value
	inline int GetR(){return ir;}
	//! Gets blue value
	inline int GetB(){return ib;}
	//! Gets green value
	inline int GetG(){return ig;}

	//! Given YUV values, update the class RGB values
	/*!
	\param y Y value
	\param u U value
	\param v V value
	*/
	void RGBUpdateyuv(int y, int u, int v)
	{
		yuv2rgb(y,u,v);
	};

	//! Update the class RGB values
	/*!
	\param r Red
	\param g Green
	\param b Blue
	*/
	void RGBUpdatergb(int r, int g, int b)
	{
		ib=b;
		ig=g;
		ir=r;
	};


//Conversiones entre espacios de color
/*************************************************************

			Conversión de RGB a rn gn 

                
 ***********************************************************/
//! Gets red-green normalized coordinates
/*!
\param rn Red normalized
\param gn Green normalized
*/
inline void rgb2rngn(double *rn, double *gn)
{
	double	frgb=(double)(ir+ig+ib);

	if (ir==0) *rn=0;
	else
		*rn=(double)(ir)/frgb;

	if (ig==0) *gn=0;
	else
		*gn=(double)(ig)/frgb;
	
}


/*************************************************************

			Conversión de RGB a i1 i2 i3 

                
 ***********************************************************/
//! Gets I1I2I3 coordinates
/*!
\param i1 I1 coordinate
\param i2 I2 coordinate
\param i3 I3 coordinate
*/
inline void rgb2i1i2i3(int *i1, int *i2, int *i3)
{
	*i1=(ir+ig+ib)/3;
	*i2=ir-ib;
	*i3=ig-(ir+ib)/2;
}

/*************************************************************

			Conversión de YUV a RGB

                
  ***********************************************************/

//! Converts YUV to RGB
/*!
\param y Y value
\param u U value
\param v V value
*/
inline void yuv2rgb(int y, int u, int v)
{
/*	*r=(int)(y+1.140*(float)v);
	*g=(int)(y-0.395*(float)u-0.581*(float)v);
	*b=(int)(y+2.032*(float)u);*/

//Obtenida de internet referenciando 
// Video Desmystified, Keith Jack ISBN 1-878707-09-4
	//http://www.webartz.com/fourcc/fccyvrgb.htm

	ib=(int)(1.164*(float)(y-16)+2.018*(float)(u-128));
	ig=(int)(1.164*(float)(y-16)-0.813*(float)(v-128)-0.391*(float)(u-128));
	ir=(int)(1.164*(float)(y-16)+1.596*(float)(v-128));
}


/*************************************************************

			Conversión de RGB a Yuv (ucs)

                Perceptually Uniform Color Space (UCS)

                 and Its Use for Face Detection

Here, we give a color convertion program to convert a color from NTSC
RGB to the uniform color system proposed by Farnsworth.
(See COLOR SCIENCE, written by Gunter Wyszecki and W. S. Stiles,
John Wiley & Sons, Inc, 1967.)

Although the widely used L*u*v*, L*a*b* color spaces are simple and
easy to use, both of them are just rough approximations of UCS.
A better uniform color system was proposed by Farthworth in 1957. In
this color system, the two colors with an equal distance as perceived
by human viewers, will project with an equal distance here.

La rutina ha sido obtenida de http://www.sys.wakayama-u.ac.jp/~chen/ucs.html
por el trabajo Face Detection From Color Images Using a Fuzzy Pattern Matching Method, PAMI vol.21n.6 Junio 1999

  ***********************************************************/
//! Converts RGB to YUV (ucs)
/*!
\param py Y value
\param puf U value
\param pvf V value
*/
void rgb2yufvf(int *py, int *puf, int *pvf)
{
  int yy, xx, xyz, xyzh,uv;
  
  /* RGB -> CIE's XYZ */
  yy = 77 * ir + 150 * ig + 29 * ib;    /* CIE's Y * 256 */
  *py = yy / 256;                      /* CIE's Y */
  if (*py == 0) {
#define UGRAY   43
#define VGRAY   44
    *puf = UGRAY;                    /* (UGRAY, VGRAY) is "white" */
    *pvf = VGRAY;
    return;
  }
  xx = 147 * ir + 42 * ig + 49 * ib;    /* CIE's X * 256 */
  xyz = 55 * ir + 50 * ig + 87 * ib;    /* CIE's (X + Y + Z) * 256 */
  xyzh = xyz / 2;
  if (xyzh==0){
	  yy=xx=0;
  }
  else{
	xx = (xx * 100 + xyzh) / xyz;      /* CIE's x * 100 = X/(X+Y+Z) * 100 */
	yy = (yy * 100 + xyzh) / xyz;      /* CIE's y * 100 = Y/(X+Y+Z) * 100 */
  }

  /* For all visible color,
   * XN <= x <= XM
   * YN <= y <= YM
   */
#define XN      56
#define XM      267
#define YN      32
#define YM      284

  xx -= XN;
  if (xx < 0)
    xx = 0;
  else if (xx > XM - XN)
    xx = XM - XN;

  yy -= YN;
  if (yy < 0)
    yy = 0;
  else if (yy > YM - YN)
    yy = YM - YN;

  /* Use a table look up to convert (x,y) to Farthworth's
   * (uf,vf)
   */
  uv = xy2uv[yy][xx];
  *pvf = uv / 256;
  *puf = uv % 256;
}


private:
	int	ir,ig,ib;

};

#endif
