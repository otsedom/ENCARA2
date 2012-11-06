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
 #ifndef	_MATHUTILS
#define _MATHUTILS

#include "math.h"

//! Class to generate gaussians
class CGaussiana
{

public:
	//! Class constructor
	CGaussiana()
	{
	};
	//! Class destructor
	~CGaussiana(){
	};

	//! Precalculates gaussian
	/*
	Precalculates bidimensional gaussian. Size limited to 100x100
	\param sigmax One dimension variance
	\param sigmay Second dimension variance
	*/
	CGaussiana(int sigmax,int sigmay){
		int i,j,n;
		int sigmax2=sigmax/2;
		int sigmay2=sigmay/2;

		//Inicialización
		for(i=0;i<100;i++){
			for(j=0;j<100;j++){
				mgauss[j][i]=0.0;
			}
		}
	
		//Obtengo un cuarto de la máscara gaussiana, el resto se utilizará por simetría
		n=sigmax2;
		if (sigmay>sigmax) n=sigmay2;
		for (i=0;i<n;i++){
			gaussx[i]=Gaussiana((float)sigmax2,(float)i);
			gaussy[i]=Gaussiana((float)sigmay2,(float)i);
		}
		for(i=0;i<sigmax2;i++){
			for(j=0;j<sigmay2;j++){
				mgauss[j][i]=gaussy[j];
			}
		}
		for(i=0;i<sigmay2;i++){
			for(j=0;j<sigmax2;j++){
				mgauss[i][j]*=gaussx[j];
			}
		}
	};

	float		gaussx[100],gaussy[100],mgauss[100][100];




//?? Uso con simetría, ya viene restada
float Gaussiana(float sigma,float t)
{
	float aux,num,den;

	num=(float)exp(-t*t/(sigma*sigma));
	den=sigma*(float)sqrt(2.0*3.141592);

	aux=(float)(num/den);
	return(aux);
}

//Distribución normal 1D, retorna la probabilidad para un x
//De Forsyth & Ponce "An introduction to probability"
float Gaussiana(float average,float variance,float x)
{
	float aux,num,den;

	num=(float)exp(-( (x-average)*(x-average) )/(2*variance));
	den=(float)sqrt(variance*2.0*3.141592);//multiplica la desviación estándard

	aux=(float)(num/den);
	return(aux);
}

//Distribución normal 2D simétrica, retorna la probabilidad para un x
//De Forsyth & Ponce "An introduction to probability"
float Gaussiana(float averagex,float averagey,float variance,float x,float y)
{
	float aux,num,den;

	num=(float)exp(-( (x-averagex)*(y-averagey) )/(2*variance));
	den=(float)sqrt(variance*2.0*3.141592);//multiplica la desviación estándard

	aux=(float)(num/den);
	return(aux);
}

};


//! Math utilities
class CMathUtils
{

public:
	//! Class constructor
	CMathUtils()
	{
	};


//! Computes euclidean distance
/*!
\param x1 First point x coordinate
\param y1 First point y coordinate
\param x2 Second point x coordinate
\param y2 Second point y coordinate
*/
inline float DistanciaEuclidea(int x1,int y1, int x2, int y2)
{
	float dist;

	dist=(float)sqrt((float)((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)));

	return dist;
}

//! Determines if a point is contained in a window
/*!
\param px Point x coordinate
\param py Point y coordinate
\param x1 Upper left x coordinate
\param y1 Upper left y coordinate
\param x2 Bottom right x coordinate
\param y2 Bottom right y coordinate
*/
bool PuntoEnVentana(int px,int py,int x1,int y1,int x2,int y2)
{
	if (px>x1 && px<x2)
	{
		if (py>y1 && py<y2)
			return true;
		else
			return false;
	}
	return false;
}


//! Rotates a point
/*! 
Rotates a point around a center of projection
\param cx x coordinate of the center of projection
\param cy y coordinate of the center of projection
\param angle Angle to rotate (in degrees)
\param x Rotated x coordinate
\param y Rotated y coordinate
*/
void Rota2D(float cx,float cy,float angle, float*	x,float *y)
{
	//Pasamos a radianes
	double rad_angle=-angle*3.141592/180.0;
	float c=(float)cos(rad_angle);
	float s=(float)sin(rad_angle);

	//Movemos origen
	float lx=*x-cx;
	float ly=*y-cy;

	//Rotamos
	float aux=(c* (lx)+s* (ly));
	ly=(c*(ly)-s*(lx));
	lx=aux;

	//Retrasladamos
	*x=lx+cx;
	*y=ly+cy;
}

//! Determines if a window is contained in another window
/*!
\param px1 Upper left x coordinate of the tested window
\param py1 Upper left y coordinate of the tested window
\param px2 Bottom right x coordinate of the tested window
\param py2 Bottom right y coordinate of the tested window
\param x1 Upper left x coordinate of the container window
\param y1 Upper left y coordinate of the container window
\param x2 Bottom right x coordinate of the container window
\param y2 Bottom right y coordinate of the container window
*/
bool VentanaEnVentana(int px1,int py1,int px2,int py2,int x1,int y1,int x2,int y2)
{
	if ( PuntoEnVentana(px1,py1,x1,y1,x2,y2)
		&& PuntoEnVentana(px1,py2,x1,y1,x2,y2)
		&& PuntoEnVentana(px2,py1,x1,y1,x2,y2)
		&& PuntoEnVentana(px2,py2,x1,y1,x2,y2)
		)
		return true;
	else return false;

	
}

//! Determines if a there is an overlap between two windows
/*!
\param Ax1 Upper left x coordinate of window A
\param Ay1 Upper left y coordinate of window A
\param Ax2 Bottom right x coordinate of window A
\param Ay2 Bottom right y coordinate of window A
\param Bx1 Upper left x coordinate of window B
\param By1 Upper left y coordinate of window B
\param Bx2 Bottom right x coordinate of window B
\param By2 Bottom right y coordinate of window B
\param areainter Area of intersection
*/
bool WindowsOverlap(int Ax1,int Ay1,int Ax2,int Ay2,int Bx1,int By1,int Bx2,int By2, long *areainter)
{
	int x1i,x2i,y1i,y2i; 

	//Calculates intersection rectangle and intersection area
	x1i=Ax1;
	if (Bx1>x1i)
		x1i=Bx1;
	y1i=Ay1;
	if (By1>y1i)
		y1i=By1;
	x2i=Ax2;
	if (Bx2<x2i)
		x2i=Bx2;
	y2i=Ay2;
	if (By2<y2i)
		y2i=By2;

	//Computes the area of intersection
	if (x2i>x1i && y2i>y1i) 
	{
		*areainter=(x2i-x1i)*(y2i-y1i);
		return true;
	}
	else
	{
		*areainter=0;
		return false;
	}
}


//! Determines the angle to rotate an image according to eyes location
/*!
\param lex x coordidate of left eye
\param ley y coordidate of left eye
\param rex x coordidate of right eye
\param rey y coordidate of right eye
\param interx x coordidate of inbetween position 
\param intery y coordidate of inbetween position
*/
double GetAngle4Rotation(int lex,int ley,int rex,int rey,float *interx,float *intery)
{
	double angulo;

	//Gets inbetween point
	*interx=((float)(rex + lex))/2.f;
	*intery=((float)(ley + rey))/2.f;

	//Computes the angle in radians
	angulo=atan2((float)(*intery - rey),(float)(rex - *interx));
	
	//Conversion to degrees
	angulo=(-angulo*180.0/3.141592);

	return angulo;
}


//! Computes norm L1
/*!
\param A First vector
\param B Second vector
\param dim Vectors dimension
*/
float L1(int *A,int *B,int dim)
{
	float l1=0.f;

	for (int i=0;i<dim;i++)
		l1+=(float)abs(A[i]-B[i]);

	return l1;
}



//! Computes norm L1, 0 idénticos
/*!
\param A First vector
\param B Second vector
\param dim Vectors dimension
*/
float L1(float *A,float *B,int dim)
{
	float l1=0.f;

	for (int i=0;i<dim;i++)
		l1+=(float)fabs(A[i]-B[i]);

	return l1;
}

//! Computes norm L2, 0 idénticos
/*!
\param A First vector
\param B Second vector
\param dim Vectors dimension
*/
float L2(float *A,float *B,int dim)
{
	float l2=0.f;

	for (int i=0;i<dim;i++)
		l2+=(float)(A[i]-B[i])*(A[i]-B[i]);

	return sqrt(l2);
}

//! Computes histogram intersection, 0 means no overlap
/*!
\param A First vector
\param B Second vector
\param dim Vectors dimension
*/
float HistIntersection(float *A,float *B,int dim)
{
	float inter=0.f;

	for (int i=0;i<dim;i++)
	{
		if (A[i]<B[i])
			inter+=A[i];
		else
			inter+=B[i];
	}
		
	return inter;
}

//! Computes histogram Bhattacharyya distance, 0 means no overlap
/*!
\param A First vector
\param B Second vector
\param dim Vectors dimension
*/
float HistBhattacharyya(float *A,float *B,int dim)
{
	float val=0.f;
	float num=0.f,den=0.f;

	float sumh1=0.f,sumh2=0.f;
	for (int i=0;i<dim;i++)
	{
		sumh1+=A[i];
		sumh2+=B[i];
	}
	den=sqrt(sumh1*sumh2);

	for (int i=0;i<dim;i++)
	{
		num+=sqrt(A[i]*B[i])/den;		
	}

	val=sqrt(1-num);
		
	return val;
}

//! Computes log likelihood ratio (LBP)
/*!
\param A First vector (sample)
\param B Second vector (model)
\param dim Vectors dimension
*/
float LogLikelihood(float *A,float *B,int dim)
{
	float ll=0.f;

	for (int i=0;i<dim;i++)
	{
		if (B[i]>0.f)
			ll+=(float)A[i]*(float)log(B[i]);
	}

	return ll;
}


};



#endif

