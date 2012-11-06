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
 /* 

		Class definition

*/

//Utilidades IplImage
#include "IplImageUtils.h"

//#include "cv.h"

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
