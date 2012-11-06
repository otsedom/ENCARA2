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

	Elements necessary for blob information. Originally developed by Josep Isern González,
	adapted by Modesto Castrillón Santana


	Modesto Castrillón Santana
	Septiembre 2003

****************************************************************************************/
#if ! defined ( SKINBLOBINFO_H )
#define SKINBLOBINFO_H

//Espacios de color
#include "ColorSpaces.h"	

//iplutils
#include "IplImageUtils.h"

//Ellipse structure used
typedef struct
{
	int	  cx,cy;	// Ellipse center
	float la;		// Length major axis
	float lb;		// Length minor axis
	float orient;	// Orientation
	int   area;		// Area
} StEllipse;

//Image rotatios structure
typedef struct{
	CvPoint2D32f center;
    float angle;
	bool boFromElipse;
	CvBox2D32f	elipse;
} stRotacionImagen;

//! Skib blon information
/*!
Contains information related to skin blobs
*/
class CSkinBlobInfo 
{

public:
	//! Class constructor
	/*!
	\param sx Width of the image to analyze
	\param sy Height of the image to analyze
	*/
	CSkinBlobInfo(int sx,int sy)
	{
		blob = cvCreateImage( cvSize(sx, sy), IPL_DEPTH_8U, 1 );	
		blobs = cvCreateImage( cvSize(sx, sy), IPL_DEPTH_8U, 1 );
		blobs_noDil = cvCreateImage( cvSize(sx, sy), IPL_DEPTH_8U, 1 );
		blobrot = cvCreateImage( cvSize(sx, sy), IPL_DEPTH_8U, 1 );	


		//Toma un conjunto de color para comenzar
		//ColorSet=ConjuntoA;
		ColorSet=ConjuntoB;

		RNmin[ConjuntoA]=RNINFmA;		//Colores normalizados escogidos según la hora
		RNmax[ConjuntoA]=RNSUPmA;
		GNmin[ConjuntoA]=GNINFmA;
		GNmax[ConjuntoA]=GNSUPmA;

		RNmin[ConjuntoB]=RNINFmB;		//Colores normalizados escogidos según la hora
		RNmax[ConjuntoB]=RNSUPmB;
		GNmin[ConjuntoB]=GNINFmB;
		GNmax[ConjuntoB]=GNSUPmB;

		//loja esttucturas datos blobs
		rect=(CvRect *) new unsigned char [NBLOBMAX*sizeof(CvRect)];
		elipse=(CvBox2D32f	*) new unsigned char [NBLOBMAX*sizeof(CvBox2D32f)];
		lastelipse=(CvBox2D32f	*) new unsigned char [NBLOBMAX*sizeof(CvBox2D32f)];
		elipseinicial=(CvBox2D32f	*) new unsigned char [NBLOBMAX*sizeof(CvBox2D32f)];
		boElipseEstimada=(bool	*) new unsigned char [NBLOBMAX*sizeof(bool)];

		//Inicializaciones 
		Inicializa();
		
	}


	//! Class destructor
	~CSkinBlobInfo()
	{
		cvReleaseImage(&blob);
		cvReleaseImage(&blobs);
		cvReleaseImage(&blobs_noDil);
		cvReleaseImage(&blobrot);
	
		//Libera datos blobs
		delete [] rect;
		delete [] elipse;
		delete [] lastelipse;
		delete [] elipseinicial;
		delete [] boElipseEstimada;
	}


	//! Detects pixels within a color range
	/*! 
	Given an input image detects thos pixels within a color range defined by RNmin, RNmax, GNmin and GNmax
	\param h Input image
	\param r_n Output image containing red normalized values
	\param g_n Output image containing red normalized values
	*/
int DetectaPixelsRangoColor(IplImage *h,IplImage **r_n, IplImage **g_n)
{
	//Contador de pixels tomados en el rango
	int cont=0;
	Inicializa();

	//Si hay imagen no nula
	if (h)
	{
		//Obtiene imagen de blob
		CRGBrgb crgbcolor;
		double rn,gn;
		char *p,*pp,*q,*pq;
		float *pr,*pg,*ppr,*ppg;
		pp=h->imageData; //trata imagen rgb

		pq=blobs_noDil->imageData;
	
		ppr=(float *)(*r_n)->imageData;
		ppg=(float *)(*g_n)->imageData;
		for (int n=0;n<h->height;n++,p+=h->widthStep,pq+=blobs_noDil->widthStep,ppr+=(*r_n)->width,ppg+=(*g_n)->width)
		{
			p=pp;
			q=pq;
			pr=ppr;
			pg=ppg;
			for (int m=0;m<h->width;m++,p+=3, q++,pr++,pg++)
			{
#ifdef INPUTIMAGERGB
				crgbcolor.RGBUpdatergb((BYTE)*p,(BYTE)*(p+1),(BYTE)*(p+2));
#else//INPUTIMAGEBGR caso normal de OpenCV
				crgbcolor.RGBUpdatergb((BYTE)*(p+2),(BYTE)*(p+1),(BYTE)*p);
#endif
				//Convierte a rn gn
				crgbcolor.rgb2rngn(&rn,&gn);
				*pr=(float)rn;
				*pg=(float)gn;
#ifdef USARBF	//Usa una RBF para clasificar
				if(Blob->ClassifyPointsRBF(rn,gn))
					*q=(char)255;
				else
					*q=0;
#else

				//Busca en rango combinación rn con gn (creando una imagen comprobando sólo el rango rn y otra con ambos
				if (rn>RNmin[ColorSet] && rn<RNmax[ColorSet]) //Está en rango de rn
				{
				if (gn>GNmin[ColorSet] && gn<GNmax[ColorSet] )//Está en rango de gn
					{
						*q=(char)255;	//ACtualiza imagen de blobs
						cont++;
					}
					else
						*q=0;
				}
				else					//Por defecto no es blob
				{
					*q=0;
				}
#endif
			}
		}
		//Realiza una dilatación
		cvDilate(blobs_noDil,blobs,NULL,1);
	}
	return cont;
}

	//! Detects pixels within a color range in a given window
	/*! 
	Given an input image detects returns a blob image where white pixels  are within a color range defined by RNmin, RNmax, GNmin and GNmax
	\param h Input image
	\param blobs Output image containing blobs
	\param blobs_noDil Output image containing blobs without dilation
	\param r_n Output image containing red normalized values
	\param g_n Output image containing red normalized values
	\param thal Lower value for first color component
	\param thau Higher value for first color component
	\param thbl Lower value for second color component
	\param thbu Higher value for second color component
	\param x1 Upper left x coordinate
	\param y1 Upper left y coordinate
	\param sx Window width
	\param sy Window height
	*/
	int DetectaPixelsRangoColorenVentana(IplImage *h, 
											IplImage **blobs,
											IplImage **blobs_noDil,
											IplImage **r_n, IplImage **g_n,
											double thal,double thau,
											double thbl,double thbu,
											int x1,int y1,int sx,int sy)
	{

	//Contador de pixels tomados en el rango
		int cont=0;
		Inicializa();


		if (h)
		{
			//Como detecta en una ventana pone el resto a cero
			//iplSet(*blobs_noDil,0);
			cvSetZero(*blobs_noDil);
			//iplSetFP(*r_n,0);
			cvSetZero(*r_n);
			//iplSetFP(*g_n,0);
			cvSetZero(*g_n);

			//Obtiene imagen de blob
			//Detección blob con espacio rn gn
			//
			CRGBrgb crgbcolor;
			double rn,gn;
			BYTE *p,*q,*pt,*qt;
			float *pr,*pg;

			p=(BYTE *)h->imageData+x1*h->nChannels+y1*h->widthStep;; //trata imagen rgb
		
			//Se sitúa en función de la ventana indicada
			q=(BYTE *)(*blobs_noDil)->imageData+x1+y1*(*blobs_noDil)->widthStep;
			pr=(float *)(*r_n)->imageData+x1+y1*(*r_n)->widthStep/(*r_n)->depth;
			pg=(float *)(*g_n)->imageData+x1+y1*(*g_n)->widthStep/(*r_n)->depth;
			float *prt,*pgt;

			for (int n=0;n<sy;n++,p+=h->widthStep,pr+=(*r_n)->widthStep/(*r_n)->depth,pg+=(*g_n)->widthStep/(*r_n)->depth,q+=(*blobs_noDil)->widthStep)
			{
				prt=pr;
				pgt=pg;
				qt=q;
				pt=p;
				for (int m=0;m<sx;m++,pt+=3,prt++,pgt++, qt++)
				{
	#ifdef INPUTIMAGERGB
					crgbcolor.RGBUpdatergb((BYTE)*pt,(BYTE)*(pt+1),(BYTE)*(pt+2));
	#else //INPUTIMAGEBGR caso normal de OpenCV
					crgbcolor.RGBUpdatergb((BYTE)*(pt+2),(BYTE)*(pt+1),(BYTE)*pt);
	#endif
					crgbcolor.rgb2rngn(&rn,&gn);
					*prt=(float)rn;
					*pgt=(float)gn;

					//Busca en rango combinación rn con gn (creando una imagen comprobando sólo el rango rn y otra con ambos
					if (rn>thal && rn<thau && gn<thbu && gn>thbl)
					{
						*qt=(char)255;
						cont++;
					}
					else  *qt=0;
									
				}
			}
		}

		return cont;
		
	}


	//! Removes neck are for a given blob
	/*!

	\param h Input image
	\param hrot
	\param hblob
	\param hblobrot
	\param hblobs
	*/
	/**************************************************************************************************************
		Para el blob actual correspondiente a la imagen h, detecta si hay cuello contenido en el mismo y lo elimina.
		Calcula la elpse asociada.
		Almacena en HistoriaRotaciones las posibles  dos rotaciones efectuadas

			h			Input Image
			hblob		Input single blob image
		
	**************************************************************************************************************/
	void DetectaBlobCaraSinCuello(IplImage *h,IplImage *hrot,IplImage *hblob,
								IplImage *hblobrot,IplImage *hblobs)
	{
		int i;

		//Si tenemos una imagen válida
		if (h)
		{
			//Inicializaciones
			IplImage *i2=cvCreateImage(cvSize(h->width,h->height),8,1);

			//Initializes
			cvSetZero(i2);
			cvSetZero(hblob);
			cvSetZero(hblobrot);

			//Asume el blob actual
			// Ajusta Rectángulo al contorno actual...
 			rect[iBlobActual] = cvContourBoundingRect(contour[iBlobActual]);

			//Crea una nueva imagen con el blob correspondiente al contorno actual pintado
	#ifdef OPENCV31
			cvDrawContours(i2,contour[iBlobActual],127,127,0,CV_FILLED);	//CV_FILLED rellena también el interior
	#else
			cvDrawContours(i2,contour[iBlobActual],CV_RGB(127,127,127),CV_RGB(127,127,127),0,CV_FILLED);	//CV_FILLED rellena también el interior
	#endif

			//Calcula el blob descontando las islas del blob (para evitar el efecto de CV_FILLED)
			//iplAnd(hblobs,i2,hblob);
			cvAnd(hblobs,i2,hblob);

			//Ajusta elipse con puntos del blob
			CalculaElipseDadoBlob(hblob,rect[iBlobActual],&(elipse[iBlobActual]));
			boElipseEstimada[iBlobActual]=true;
			
			//Histórico rotaciones
			HistoriaRotaciones[nRot].center=elipse[iBlobActual].center;
			HistoriaRotaciones[nRot].angle=elipse[iBlobActual].angle;
			HistoriaRotaciones[nRot].boFromElipse=true;
			HistoriaRotaciones[nRot].elipse=elipse[iBlobActual];
			nRot++;

			//Coherencia temporal de la elipse
			bool boCoherenciatemporalorient=true;

			lastelipse[iBlobActual]=elipse[iBlobActual];
			//Fin coherencia temporal

			//ELIMINACIÓN DEL CUELLO	

			//Una vez calculada la elipse estudiamos si es viable eliminar el cuello
			//Chequeamos dimensiones elipse para eliminar el cuello si es posible
			//Indica si sufre el proceso dos rotaciones
			boElipsereducida=false;

			//Para repetirlo mientras las dimensiones no estén en cierto rango
			if (boCoherenciatemporalorient && elipse[iBlobActual].size.height>(float)15.0 && 
				elipse[iBlobActual].size.width>elipse[iBlobActual].size.height*1.45 /*&& elipse->size.width<elipse->size.height*2.9*/)
			{
				//Rotates using ipl
				//Rotar antes de buscar el cuello
				CIplImageUtils iplutils;

				cvSetImageROI(hblob,rect[iBlobActual]);
				iplutils.RotateImage(hblob,hblobrot,elipse[iBlobActual].angle,elipse[iBlobActual].center.x,elipse[iBlobActual].center.y);
				cvResetImageROI(hblob);

				cvSetImageROI(h,rect[iBlobActual]);
				iplutils.RotateImage(h,hrot,elipse[iBlobActual].angle,elipse[iBlobActual].center.x,elipse[iBlobActual].center.y);
				cvResetImageROI(h);

				//

				//Prueba comprobación cuello
				//Buscamos la fila con máximo ancho de elementos del blob
				int imax=0,imaxm,maxtmp;
				char *ptmp,*pptmp;
				
				//A partir del centro de la elipse buscamos hacia abajo una fila que sean máximo ancho local
				imax=0;
				ptmp=hblobrot->imageData+(int)(elipse[iBlobActual].center.y+elipse[iBlobActual].size.height*0.1)*hblobrot->widthStep;
				ifilab=(int)(elipse[iBlobActual].center.y+elipse[iBlobActual].size.height*0.1);
					
				//Hacia abajo busca la fila la más ancha
				for (i=(int)(elipse[iBlobActual].center.y+elipse[iBlobActual].size.height*0.1);i<hblobrot->height;i++)
				{
					maxtmp=0;
					int ini,fin;
					ini=hblobrot->width;
					fin=0;

					for (int j=0;j<hblobrot->width;j++,ptmp++)
					{
						if (*ptmp!=0)
						{
							if (fin==0) ini=j;
							fin=j;
						}
					}
					maxtmp=fin-ini;
					if (maxtmp>imax)
					{
						imax=maxtmp;
						ifilab=i;
					}
				}
				ptmp=hblobrot->imageData;
				ptmp+=(int)(elipse[iBlobActual].center.y+elipse[iBlobActual].size.height*0.1)*hblobrot->widthStep;
				imaxm=hblobrot->width;
				
				ifilamb=(int)(elipse[iBlobActual].center.y+elipse[iBlobActual].size.height*0.1);
				
				//Hacia abajo la más estrecha antes de la más ancha
				for (i=(int)(elipse[iBlobActual].center.y+elipse[iBlobActual].size.height*0.1);i<ifilab;i++,ptmp+=hblobrot->widthStep)
				{
					maxtmp=0;
					int ini,fin;
					ini=hblobrot->width;
					fin=0;

					//Puntero temporal
					pptmp=ptmp;

					//Recorro la fila
					for (int j=0;j<hblobrot->width;j++,pptmp++)
					{
						if (*pptmp!=0)
						{
							if (fin==0) ini=j;
							fin=j;
						}
					}
					maxtmp=fin-ini;
					if (ini!=fin)
					{
						if (maxtmp<imaxm)
							{
							imaxm=maxtmp;
							ifilamb=i;
						}
					}
				}


				//Ajusta elipse con puntos del blob tras eliminar después de ifilamb
				//Eliminar a partir de ifilamb (y un poco más)
				int barbilla=(int)(ifilab*0.7+ifilamb*0.3);

				//El valor estimado de la barbilla debe estar en unos límites width es el eje largo de la elipse
				int iPuntoSupElipse=(int)(elipse[iBlobActual].center.y-elipse[iBlobActual].size.width);
				int BarbillaMin=iPuntoSupElipse+(int)(2.0*elipse[iBlobActual].size.height*RATIOEJESELIPSEMIN);
				int BarbillaMax=iPuntoSupElipse+(int)(2.0*elipse[iBlobActual].size.height*RATIOEJESELIPSEMAX);
				//Heurística relación ancho y alto de cara (no es siempre así)
				if (BarbillaMin>barbilla) barbilla=BarbillaMin;//No demasiado corto
				//No demasiado largo
				if (barbilla>BarbillaMax)
					barbilla=BarbillaMax;

				if ( barbilla>rect[iBlobActual].y && barbilla<h->height ) //Control para no irnos fuera
				{
					memset(hblobrot->imageData+(barbilla)*hblobrot->widthStep,0,(h->height-barbilla)*hblobrot->widthStep);
					//Nueva elipse (ojo hay que actualizar este angulo 
					//ojo con elipseinicial y forma de guardarla

					//Ajusto la altura del rectàngulo si he quitado el cuello
					rect[iBlobActual].height=barbilla-rect[iBlobActual].y;

					CalculaElipseDadoBlob(hblobrot,rect[iBlobActual],&(elipseinicial[iBlobActual]));

					//Intercambiar elipses
					//Coherencia evitando cambios drásticos en orientación
					if (!(fabs(elipseinicial[iBlobActual].angle-elipse[iBlobActual].angle)>25))
					{
						CvBox2D32f elipaux;

						//La primera calculada
						elipaux=elipse[iBlobActual];

						elipse[iBlobActual]=elipseinicial[iBlobActual];
						elipseinicial[iBlobActual]=elipaux;
						//Vale la elipse recortada
						boElipsereducida=true;

						//Histórico rotaciones
						HistoriaRotaciones[nRot].center=elipse[iBlobActual].center;
						HistoriaRotaciones[nRot].angle=elipse[iBlobActual].angle;
						HistoriaRotaciones[nRot].boFromElipse=true;
						HistoriaRotaciones[nRot].elipse=elipse[iBlobActual];

						nRot++;

						
					}
				}
				//Fin prueba cuello
			}

			//FIN ELIMINACIÓN DEL CUELLO
			cvReleaseImage(&i2);
		}
	}


	//Variables
	//Imágenes utilizadas para el cálculo de blobs
	//! Selected blob
	IplImage	*blob;	
	//! Selected blob rotated
	IplImage	*blobrot;
	//! Blobs
	IplImage	*blobs;		
	//! Blobs not dilated
	IplImage	*blobs_noDil;	

	//! Blobs rectangles
	CvRect		*rect;		
	//! Estimated ellipse
	CvBox2D32f	*elipse;		
	
	//Componentes y contornos
	//! Contours
	CvSeq   ** contour;	
	//! Area
	float   *area;				
	//! Blob actual en proceso
	int		iBlobActual;		
	
	//Rotation history
	//Histórico rotaciones imagen de entrada
	//! Rotation history data
	stRotacionImagen HistoriaRotaciones[5],
	//! Aux
	RotacionImagenaux1; 
	//! Number of rotations applied to input image
	int	nRot;					

	
private:

		
	void Inicializa()
	{
		for (int i=0;i<NBLOBMAX;i++)
			boElipseEstimada[i]=false;
	}

	
	//! Computes an ellipse for a blob
	/*!
	\param hblob Input blob image
	\param rect rect Blob container
	\param elipse Ellipse estimated for the blob
	*/
	void CalculaElipseDadoBlob(IplImage *hblob,CvRect rect, CvBox2D32f *elipse)
	{
	//Inicializaciones
	long	iArea=0;
	CvPoint2D32f *puntos32fBlob=new CvPoint2D32f[rect.height*rect.width];
	int		isx=hblob->width;
	int		isy=hblob->height;

	//Coloco el puntero al comienzo del rectàngulo que incluye la ventana que tratamos
	BYTE *pp,*p=(BYTE *)hblob->imageData+rect.x+rect.y*hblob->widthStep;

	//Evitando salirnos fuera de la imagen
	if (rect.y+rect.height>=isy)
		rect.height=isy-rect.y;
	if (rect.x+rect.width>=isx)
		rect.width=isx-rect.x;
				
	for (int i=0;i<rect.height;i++,p+=hblob->widthStep)
	{
		pp=p;
		for (int j=0;j<rect.width;j++,pp++){
			if (*pp!=0){
				puntos32fBlob[iArea].x=(float)rect.x+j;
				puntos32fBlob[iArea].y=(float)rect.y+i;
				iArea++;
			}
		}
	}
	//Calcular elipse con método momentos de Josep.
	StEllipse Elipse[1];
	Elipse[0].orient=0.0;
	Elipse[0].cx=0;
	Elipse[0].cy=0;
	Elipse[0].la=0.0;
	Elipse[0].lb=0.0;

	if (iArea)
	{
		EllipseCalculate(puntos32fBlob,iArea,Elipse);
	}


	//Escribe estructura de salida

	//Paso a grados la orientación
	float	angulo=(float)(Elipse[0].orient*180.0/3.141592);

	//centro

	(*elipse).center.x=(float)Elipse[0].cx;
	(*elipse).center.y=(float)Elipse[0].cy;

	//Rotamos el ángulo contrario
	(*elipse).angle=-angulo;
	(*elipse).size.width=Elipse[0].la;
	(*elipse).size.height=Elipse[0].lb;

	//Con rutinas de OpenCV sería
			
	/*CvBox2D32f elipseB;

	cvFitEllipse( puntos32fBlob, iArea, &elipseB );

	//Pruebita
	(*elipse).angle=elipseB.angle-90;
	(*elipse).size.width=elipseB.size.width*0.65;
	(*elipse).size.height=elipseB.size.height*0.65;

				
				cvEllipse( h, cvPoint((int)elipse->center.x,(int)elipse->center.y), 
					cvSize((int)elipse->size.width,(int)elipse->size.height), elipse->angle, 0, 360, CV_RGB(255,0,0));
	*/

	delete [] puntos32fBlob;

	}


	//! Computes an ellipse for a blob
	/*!
	Rutina Josep adaptada
	\param puntos Region points
	\param npuntos Number of points
	\param ellipse Ellipse estimated for the points
	*/
	void EllipseCalculate(CvPoint2D32f *puntos, long npuntos ,StEllipse *ellipse)

	{
		int	  x,y,i;
		float  xy,xc,yc,xx,yy;//long

		//Inicializaciones
		xy=xc=yc=xx=yy=0;

		// Se calcula los xi e yi para el cálculo de los momentos centrales
		for (i=0;i<npuntos; i++){
			x=(int)puntos[i].x;
			y=(int)puntos[i].y;
		
			xc+=x;
			yc+=y;
			xy+=x*y;
			xx+=x*x;
			yy+=y*y;
		}

		// Centro con respecto a la ventana del blob
		double cx=(double)xc/(double)npuntos;
		double cy=(double)yc/(double)npuntos;

		// Cálculo de la Orientacion
		ellipse->orient=(float)(.5*atan2(2.*((double)xy-(double)xc*(double)yc/(double)npuntos),
								((double)yy-(double)yc*(double)yc/(double)npuntos)
								-((double)xx-(double)xc*(double)xc/(double)npuntos)));
		// Cálculo de los ejes
		double Imin=0.;
		double Imax=0.;
		double c=cos(ellipse->orient);
		double s=sin(ellipse->orient);
		for (i=0;i<npuntos; i++){
			x=(int)puntos[i].x;
			y=(int)puntos[i].y;
		
			Imin+=pow((x-cx)*c-(y-cy)*s,2.);
			Imax+=pow((x-cx)*s+(y-cy)*c,2.);
		
		}
		// Cálculo de la longitud de los ejes
		//Prueba
		if (Imax>Imin)
		{
			ellipse->la=(float)(pow(4/PI,0.25)*pow(pow(Imax,3.)/Imin,0.125));
			ellipse->lb=(float)(pow(4/PI,0.25)*pow(pow(Imin,3.)/Imax,0.125));
		}
		else
		{
			ellipse->lb=(float)(pow(4/PI,0.25)*pow(pow(Imax,3.)/Imin,0.125));
			ellipse->la=(float)(pow(4/PI,0.25)*pow(pow(Imin,3.)/Imax,0.125));
		}
		// Se calcula el Centro de Masas de la Región con respecto a la imagen
		ellipse->cx=(int)(cx);
		ellipse->cy=(int)(cy);
		ellipse->area=npuntos;
	}		

	//Color range
	double		RNmin[2],RNmax[2],GNmin[2],GNmax[2];	//! Colores normalizados escogidos según la hora

	enum	JuegosColores ColorSet;						//Indica el juego de colores activo

	int		iNBlobsCurrentFrame;						//Blobs analizados en la imagen actual
	int		iBlobSelected;								//Blob que tuvo detección

	CvBox2D32f	*lastelipse;							//Elipse estimada
	CvBox2D32f	*elipseinicial;							//Si boElipsereducida contiene la primera elipse asumida
	bool		*boElipseEstimada;						//Indica si para este blob se ha estimado la elipse

	
	bool		boElipsereducida;					//Indica si al calcular la elipse del blob se calculan dos
	int			ifilab,ifilamb;						//Utilizadas para heurísticas del cuello, son de la clase para poder pintarlas en depuración

};

#endif
