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
 /*!
 *	ENCARA2 ver 2 interface
 *
 *	Performs real-time multiple face detection according to the paper:
 *
 *		ENCARA2: Real-time Detection of Multiple Faces at Di®erent Resolutions in Video Streams,
 *		Journal of Video and Image Communication, Elsevier 2007.
 *			
 *s
 *	It makes use of OpenCV, combining Viola-Jones based detectors, skin color and tracking
 *	
 *	Modesto Castrillón Santana
 *	January 2005-07
 */

#define NewFeatures //Sucesivas detecciones de los elementos faciales

#define AJUSTEBOCANARIZ 1//1 lo aplica, 0 no lo aplica (versión previa que parece un pelín más rápida)

//#define USINGSIZEANDNOTPCAERROR //Si sólo tienes los ojos, si definido escoge los ojos en función del tamaño y no del menor error de reconstrucción

//#define NewFeatures2 //AÚN En desarrollo para la primera detección de los delementos faciales

//#define DBGMode//Depurando



#include "ENCARA2_2lib.h"


/*
  	Multiple face detection by means of ENCARA2.2
 
 		h					Image
 		escala				1 no dimension reduction, 2 scales down the image to half size
 */
void CENCARA2_2Detector::ApplyENCARA2(IplImage *h, int escala)
{
	if (boDetectorAvailable)
	{
		//Are the dimensions with current configuration incoherent?
		if (width!=h->width || height!=h->height)
		{
			//Sets the proper configuration
			SetWorkingSize(h->width,h->height);
		}


		//Is the input image a gray image?
		if (h->nChannels!=3)
			boImagenGrises=true;

		//Saves previous gray image
		cvCopyImage(imagegrayIN,imagegrayINprev);
		
		//Update working structures
		if (!boImagenGrises)//Normal color image
		{
			cvCopyImage(h,image);
			cvCvtColor(h,imagegray, CV_BGR2GRAY );			
		}
		else
		{	//No conversion is needed as the image is already a gray image
			cvCopyImage(h,imagegray);
		}

		//Saves a copy of the gray image to use it as temporal
		cvCopy(imagegray,imagegrayIN);

		//Locates faces in the image
		FindFaces(escala);
	}
}

/*
  	Multiple face detection by means of ENCARA2.2
 
 		h					Image
 		iDetector			Identifies the detector to apply
		px1,py1,px2,py2		Object containers
		scale	1 uses the input image scale, 2 halves it

		The arrays returned must be freed after use
 */
void CENCARA2_2Detector::ApplyViolaBasedDetector(IplImage *h, int iDetector, int **px1, int **py1, int **px2,int **py2,int scale)
{
	int ii;
	CViolaDetector	*Detector;
	bool	DetectorAvailable=false;

	px1=px2=py1=py2=NULL;



	switch (iDetector)
	{
	case OPENCV_FACE:
			//Is the detector available?
			if (ViolaFaceDetector->boCascada)
			{
				DetectorAvailable=true;
				Detector=ViolaFaceDetector;
			}
			break;
	case OWN_HS:
			//Is the detector available?
			if (ViolaHSDetector->boCascada)
			{
				DetectorAvailable=true;
				Detector=ViolaHSDetector;
			}
		break;
	case OWN_LEYE:
			//Is the detector available?
			if (ViolaLeftEyeDetector->boCascada)
			{
				DetectorAvailable=true;
				Detector=ViolaLeftEyeDetector;
			}
		break;
	case OWN_REYE:
			//Is the detector available?
			if (ViolaRightEyeDetector->boCascada)
			{
				DetectorAvailable=true;
				Detector=ViolaRightEyeDetector;
			}
		break;
	case OWN_BOTHEYES:
			//Is the detector available?
			if (ViolaEyePairDetector->boCascada)
			{
				DetectorAvailable=true;
				Detector=ViolaEyePairDetector;
			}
		break;
	case OWN_NOSE:
			//Is the detector available?
			if (ViolaNoseDetector->boCascada)
			{
				DetectorAvailable=true;
				Detector=ViolaNoseDetector;
			}
		break;
	case OWN_MOUTH:
			//Is the detector available?
			if (ViolaMouthDetector->boCascada)
			{
				DetectorAvailable=true;
				Detector=ViolaMouthDetector;
			}
		break;
	default:
		break;
	}

	//Processes
	if (DetectorAvailable)
	{
			ViolaFaceDetector->ProcessImage(image,scale);

			//Are there detections
			if (Detector->NumObjects)
			{
				*px1= (int *) new unsigned char[Detector->NumObjects*sizeof(int)];
				*py1= (int *) new unsigned char[Detector->NumObjects*sizeof(int)];
				*px2= (int *) new unsigned char[Detector->NumObjects*sizeof(int)];
				*py2= (int *) new unsigned char[Detector->NumObjects*sizeof(int)];

				for (ii=0;ii<Detector->NumObjects;ii++)
					{							
						//Copies detected containers
						(*px1)[ii]=Detector->x1[ii];
						(*py1)[ii]=Detector->y1[ii];
						(*px2)[ii]=Detector->x2[ii];
						(*py2)[ii]=Detector->y2[ii];
					}
			}
	}



	switch (iDetector)
	{
	case OPENCV_FACE:
			//Is the detector available?
			if (ViolaFaceDetector->boCascada)
			{
			}

		break;
	}
}

/****************************************************************************************

	Assumes that image to process is image, and performs a search of multiple faces
	making use of temporal coherence, i.e. using previous detection data
				
		escala				Scale to process using Viola based detectors, admits 1 and 2
	
****************************************************************************************/
void CENCARA2_2Detector::FindFaces(int escala)
{
	//Auxiliar vars
	int		NumFaces;
	
	//Eyes have not been searched
	boOjosBuscadosenGrises=false;

	//No search in the whole image has been performed by default
	boAlreadySearchedintheWholeImage=false;

	//Rotation counter
	Blob->nRot=0;

	if (boHayImagenesdeTrabajo)
	{

		if (boDebugLog)
		{
			char cadena[256];
			sprintf(cadena,"\n\nNueva imagen (%d) --------------------------------\n",nProcesadas);
			DebugLog->WriteToLogFile(cadena);

		}

		//Inicializo t para contabilizar el tiempo de poceso
		clock_t tini=clock();
	
		//Inicializaciones para utilización de los detectores de caras y boca
		CvSeq* faces=NULL;
		//Imagen con resolución menor
		IplImage* image2;		

		//Si hay imagen disponible
#ifdef DEMOVERSION
		if (imagegray && nProcesadas<500) //limits the number of images to process continously
#else
		if (imagegray)
#endif
		{			
			//Uses previous detections as a clue
			//Hace copia de los datos faciales obtenidos en la imagen precedente
			CFacialData		**Facesaux;

			DatosFacialesPrevios.Libera();
			Facesaux=DatosFacialesPrevios.Faces;
			DatosFacialesPrevios.Faces=DatosFaciales.Faces;
			DatosFaciales.Faces=Facesaux;
			DatosFacialesPrevios.NumFaces=DatosFaciales.NumFaces;
			DatosFacialesPrevios.nSubjects=DatosFaciales.nSubjects;//útil para el contador de etiquetas de nombres
			ids_sofar=DatosFaciales.nSubjects;//Para tener constancia del valor si se pierden todas las caras
			DatosFaciales.NumFaces=0;//DatosFacialesPrevios had no face
			//DatosFaciales.Libera();

			//Increases the number of processed frames
			nProcesadas++;
			
			//Transforma la imagen de entrada según la escala elegida
			if (escala==2)
			{
				image2 = cvCreateImage( cvSize(width/2,height/2), image->depth, image->nChannels );
				//Reduce la dimensión de la imagen a tratar
				cvPyrDown( image, image2 );
			}
			else
			{
				image2 = cvCreateImage( cvSize(width,height), image->depth, image->nChannels );
				//No reduce ya que la escala es 1
				cvCopyImage( image, image2 );
			}
			
			//A priori no hay detecciones
			NumFaces=0;

			if (boDebugLog)
			{
				char cadena[256];
				
				sprintf(cadena,"Antes SearchUsingPreviousFrameDetections (%ld)\n",clock() - tini );
			
				DebugLog->WriteToLogFile(cadena);
			}

			//Si hubo detecciones en la imagen previa las busca en la imagen actual en zonas cercanas
			//combinando el tracking o la detección en una zona limitada
			//Encloses the search of new faces from time to time			
			SearchUsingPreviousFrameDetections(&NumFaces,escala,image2);


			if (boDebugLog)
			{
				char cadena[256];
				
				sprintf(cadena,"Antes SearchIntheWholeImage (%ld)\n",clock() - tini );
				
				DebugLog->WriteToLogFile(cadena);
			}
	
			//Searches in the whole image (its execution depends on the current configuration and if any face was detected using previous info)
			SearchIntheWholeImage(&NumFaces,escala,image2);

			if (boDebugLog)
			{
				char cadena[256];
				
				sprintf(cadena,"Antes RemoveRedundantFaces (%ld)\n",clock() - tini );
				
				DebugLog->WriteToLogFile(cadena);
			}
			
			//Deletes REDUNDANT faces
			RemoveRedundantFaces(&NumFaces);

			if (boDebugLog)
			{
				char cadena[256];
				
				sprintf(cadena,"Antes SearchFacialElements (%ld)\n",clock() - tini );
				
				DebugLog->WriteToLogFile(cadena);
			}

			//UNA VEZ QUE HEMOS BUSCADO TODO LO POSIBLE
			//Si hubo detección, busca los elementos faciales
			SearchFacialElements(NumFaces);

			if (boDebugLog)
			{
				char cadena[256];
				
				sprintf(cadena,"Antes FuseLostandNew (%ld)\n",clock() - tini );
				
				DebugLog->WriteToLogFile(cadena);
			}
			
			//Fuses lost and new faces based on histograms
			FuseLostandNew();
	
			cvReleaseImage( &image2 );	

		}//if(imagegray)

		if (boDebugLog)
		{
			char cadena[256];
			sprintf(cadena,"Termina proceso imagen\n");
			DebugLog->WriteToLogFile(cadena);
		}

		if (boDebugLog)
		{
			char cadena[256];
			
			sprintf(cadena,"Final (%ld)\n",clock() - tini );
			
			DebugLog->WriteToLogFile(cadena);
		}

		t=clock()-tini;//Inicializa valor por defecto de detección
	}

	return;

}



/*******************************************************************************************************************


			COLOR

********************************************************************************************************************/

/***************************************************************************************
Computes skin color representation using the center of the face container. It assumes that
the normalized red and green images are available

	hrn		Red normalize dinput image
	hgn		Green normalized input image
	x1,y1	Left upper corner of window
	sx,sy	WIndow size
	rnmix	Lower red normalized value
	rnmax	Upper red normalized value
	gnmin	Lower green normalized value
	gnmax	Upper green normalized value
	boColorAlreadyInitialized Indicates if the color has been initialized

****************************************************************************************/
void CENCARA2_2Detector::AprendeColor(IplImage *hrn,IplImage *hgn,int x1, int y1, int sx, int sy,
								   float *rnmin,float *rnmax,float *gnmin,float *gnmax,
								   bool *boColorAlreadyInitialized)
{
	float  MediaRUpdated, MediaGUpdated, DesvR, DesvG;	//Color data


	//Initialization
	 MediaRUpdated=0.0;
	 MediaGUpdated=0.0;
	 DesvR=0.0;
	 DesvG=0.0;
	 
	 int icont=0;

	 BYTE *prn=(BYTE *)hrn->imageData+y1*hrn->widthStep+x1*sizeof(float);
	 BYTE *pgn=(BYTE *)hgn->imageData+y1*hgn->widthStep+x1*sizeof(float);
	 float *qfr,*pfr=(float *)prn;
	 float *qfg,*pfg=(float *)pgn;


	 //Comptes average and variance
	//float pointers, thus no care with widthstep needed
	for (int i=0;i<sy;i++,pfr+=hrn->width,pfg+=hgn->width)
	{
		qfr=pfr;
		qfg=pfg;
		for (int j=0;j<sx;j++,qfr++,qfg++)
		{
			MediaRUpdated+=*qfr;
			MediaGUpdated+=*qfg;
			icont++;
			DesvR+=(*qfr)*(*qfr);
			DesvG+=(*qfg)*(*qfg);
		}
	}

	//General
	MediaRUpdated/=icont;
	MediaGUpdated/=icont;

	DesvR/=icont;
	DesvR-= MediaRUpdated* MediaRUpdated;
	DesvG/=icont;
	DesvG-= MediaGUpdated* MediaGUpdated;
	if (DesvR>0.0)						//Si no ocurre probablemente es una imagen de grises representada en RGB
	DesvR=(float)sqrt( DesvR);
	if (DesvG>0)
	DesvG=(float)sqrt( DesvG);
		
	
	//Computes range according to variance
	float Drn,Dgn;
	if (DesvR>0.0)
	Drn=(float)(sqrt(DesvR)*0.15);
	else
		Drn=0.0;
	if (DesvG>0.0)
	Dgn=(float)(sqrt(DesvG)*0.15);
	else
		Dgn=0.0;

	/*//Útil para seguimiento de una única cara
	Ultimorn_media=MediaRUpdated;
	Ultimogn_media=MediaGUpdated;
	Ultimorn_desv=DesvR;
	Ultimogn_desv=DesvG;*/

	if (!(*boColorAlreadyInitialized))
	{
		//Initializes color range
		*rnmin=MediaRUpdated-Drn;
		*rnmax=MediaRUpdated+Drn;
		*gnmin=MediaGUpdated-Dgn;
		*gnmax=MediaGUpdated+Dgn;	

		*boColorAlreadyInitialized=true;
	}
	else
	{
		//Does not accept large color changes
		if (fabs(*rnmin-MediaRUpdated+Drn)+
			fabs(*rnmax-MediaRUpdated-Drn)+
			fabs(*gnmin-MediaRUpdated+Dgn)+
			fabs(*gnmax-MediaRUpdated-Dgn)<0.15)
		{
			*rnmin=(float)(*rnmin*0.8+0.2*(MediaRUpdated-Drn));
			*rnmax=(float)(*rnmax*0.8+0.2*(MediaRUpdated+Drn));
			*gnmin=(float)(*gnmin*0.8+0.2*(MediaGUpdated-Dgn));
			*gnmax=(float)(*gnmax*0.8+0.2*(MediaGUpdated+Dgn));	
		}
	}

}

/******************************************************
Given a space color (A;B) given a rectangular area on that 
space, returns an image with white for pixels on that range
computed in a window of the original image


  h			Input Image
  blobs		Blob (output) image containing all pixels in color range
  blobs_noDil		Blob (output) image containing all pixels in color range without dilation
  thal,thau	Upper and lower threshold for A color component
  thbl,thbu Upper and lower threshold for B color component

*******************************************************/
void CENCARA2_2Detector::CalculaImagenColorNormalizado_Ventana(IplImage *h,IplImage *hrn,IplImage *hgn,int x1,int y1,int sx,int sy)
{
	//If there is an image
	if (h)
	{
		CRGBrgb crgbcolor;
		double rn,gn;
		char *p,*pt;
		float *pr,*pg,*prt,*pgt;
		p=h->imageData+x1*h->nChannels+y1*h->widthStep; //trata imagen rgb por ello *3
		
		pr=(float *)hrn->imageData+x1+y1*width;//No hace falta widthStep pq son float
		pg=(float *)hgn->imageData+x1+y1*width;

		for (int n=0;n<sy;n++,p+=h->widthStep,pr+=width,pg+=width)
		{
			pt=p;//input image
			prt=pr;
			pgt=pg;
			for (int m=0;m<sx;m++,pt+=3,prt++,pgt++)
			{
				//According to input image format
#ifdef INPUTIMAGERGB
				crgbcolor.RGBUpdatergb((BYTE)*pt,(BYTE)*(pt+1),(BYTE)*(pt+2));
#else //INPUTIMAGEBGR caso normal de OpenCV
				crgbcolor.RGBUpdatergb((BYTE)*(pt+2),(BYTE)*(pt+1),(BYTE)*pt);
#endif
				//Convierte a rn gn
				crgbcolor.rgb2rngn(&rn,&gn);
				*prt=(float)rn;
				*pgt=(float)gn;

			}
		}			

	}	

	
}

/*******************************************************************************************************************
	FACIAL ELEMENTS SEARCH
*******************************************************************************************************************/



/*************************************************************************************************
	
	Searches the eye pair container given a window

*************************************************************************************************/
void CENCARA2_2Detector::BuscaParOjosconViola(CvHaarClassifierCascade* cascade,
										   IplImage *h,int x1,int y1,int x2,int y2,int *NumOjos,
											int *ojosx1,int *ojosy1,int *ojosx2,int *ojosy2)

{

	int nOjos;

	IplImage *hh,*h2;
	bool boPiramide=false;
	int  escala;

	CvSeq* ojos=NULL;

	*NumOjos=0;

	//Localizamos zona para buscar los ojos
	cvSetImageROI(h,cvRect((int)(x1),(int)(y1),(int)(x2-x1),(int)(y2-y1)));

	if (x2-x1>70)
	{
		hh=h;
		escala=1;
	}
	else
	{
		if (x2-x1>15)
		{
			h2=cvCloneImage(h);
			hh=cvCreateImage( cvSize((int)(x2-x1)*2,(int)(y2-y1)*2), h->depth, h->nChannels );
			cvPyrUp(h2,hh);
			escala=2;
			cvReleaseImage(&h2);
			boPiramide=true;
		}
		else
		{
			cvResetImageROI(h);
			return;
		}
	}
						
	cvClearMemStorage( ViolaStorage );
				
	//Setting the last parameter is faster
	ojos = cvHaarDetectObjects(hh, cascade, ViolaStorage, 1.2);

	if (ojos)
	{
		nOjos=(ojos ? ojos->total : 0);

        //Almacena datos si hubo detección ajustando en función de la ventana suministrada
		if (nOjos)
		{
			long area=0,areamax=0;
			for (int i=0;i<nOjos;i++)
			{
				CvRect r = *(CvRect*)cvGetSeqElem( ojos, i );

				area=(r.width)*(r.height);

				//Optará por la mayor lo bastante ancha en relación al recuadro
				if (area>areamax && r.width>(int)((float)(x2-x1)*0.3))
				{
					*ojosx1=x1+r.x/escala;
					*ojosy1=y1+r.y/escala;
					*ojosx2=*ojosx1+r.width/escala;
					*ojosy2=*ojosy1+r.height/escala;
					areamax=area;
					*NumOjos=1;
				}
			}
		}
		cvClearSeq(ojos);
		ojos=NULL;
	}

	cvResetImageROI(h);

	//Si usó pirámide libera
	if (boPiramide)
		cvReleaseImage(&hh);

}

/*****************************************************************
Búsqueda de ojos, intercambiable, en este momento busca los grises
menores en una zona, para luego ajustar según si están muy cerca 
(ojijuntos) buscando uno de nuevo, o buscar el punto alrededor 
concreto con menor correlación.
c	
	hgray	imagen
	contour	imagen de contorno zona ojos(salida)
	odx,ody	ojo derecho
	oix,oiy	ojo izquierdo
	h_odx,h_ody	ojo derecho por Hough
	h_oix,h_oiy	ojo izquierdo por Hough
	vd,vi	valores del mínimo localizado para cada posible ojo
	laxis saxis	valores ejes de la elipse actual
	x1,y1,x2,y2 ventana que contiene al par ocular

	******************************************************************/
int CENCARA2_2Detector::BuscaOjos_Grises(IplImage *hgray,
							int *odx,int *ody,unsigned char *vd,int *oix,int *oiy,unsigned char *vi,int x1,int y1, int x2, int y2)
{

	//Inicializaciones
	int ret=-1; //Valor retornado si los posibles ojos se aceptan


	//En cada mitad de la ventana del par ocular busca cada ojo
	int vx1=x1,vy1=y1;

	int ancho=(int)(x2-x1);
	int alto=(int)(y2-y1);

	IplUtils.WindowTest(hgray,&vx1,&vy1,&ancho,&alto);

	//The area is divided in two parts
	ancho/=2;

	if(ancho>0 && alto>0)
	{

		double min,max;
		CvPoint cvmin;

		cvSetImageROI(hgray,cvRect(vx1,vy1,ancho,alto));

		cvMinMaxLoc(hgray,&min,&max,&cvmin);
		
		*oix=vx1+cvmin.x;
		*oiy=vy1+cvmin.y;

		cvResetImageROI(hgray);

		cvSetImageROI(hgray,cvRect(vx1+ancho,vy1,ancho,alto));

		cvMinMaxLoc(hgray,&min,&max,&cvmin);
		
		*odx=vx1+ancho+cvmin.x;
		*ody=vy1+cvmin.y;

		cvResetImageROI(hgray);

		return OK;

	} else	return NO_OK;

/*	
	BYTE *p=(BYTE*)hgray->imageData;
	int sx=hgray->widthStep;
	int sy=hgray->height;

	

	//Buscando en posición estándar
	//izquierdo
	rOISearch.x=x1;
	rOISearch.y=y1;
	rOISearch.width=ancho;
	rOISearch.height=alto;

	ControlVentanaBusqueda(&rOISearch);

	//Derecho			
	rODSearch.x=x1+ancho;
	rODSearch.y=y1;
	rODSearch.width=(int)ancho;
	rODSearch.height=(int)alto;

	ControlVentanaBusqueda(&rODSearch);


	
//En la imagen de gris busca el mínimo en la misma ventana
BuscaMinimob(p,sx,sy,vi,oix,oiy,rOISearch.x,rOISearch.y,rOISearch.width,rOISearch.height);
		//En la imagen de gris busca el mínimo en la misma ventana
BuscaMinimob(p,sx,sy,vd,odx,ody,rODSearch.x,rODSearch.y,rODSearch.width,rODSearch.height);*/



}

/*****************************************************************
Búsqueda de ojos, intercambiable, en este momento busca los grises
menores en una zona, para luego ajustar según si están muy cerca 
(ojijuntos) buscando uno de nuevo, o buscar el punto alrededor 
concreto con menor correlación.
c	
	hgray	imagen
	contour	imagen de contorno zona ojos(salida)
	odx,ody	ojo derecho
	oix,oiy	ojo izquierdo
	h_odx,h_ody	ojo derecho por Hough
	h_oix,h_oiy	ojo izquierdo por Hough
	vd,vi	valores del mínimo localizado para cada posible ojo
	laxis saxis	valores ejes de la elipse actual
	Cmasa	centro de la elipse
	distN	distancia en x entre punto promedio y un ojo

	******************************************************************/
int CENCARA2_2Detector::BuscaOjos_Grises(IplImage *hgray,
							int *odx,int *ody,unsigned char *vd,int *oix,int *oiy,unsigned char *vi,CvPoint Cmasa,float laxis, float saxis)
{
	CvRect		rOISearch,rODSearch;	//zonas búsqueda ojos en imagen de entrada 

	//Inicializaciones
	int ret=-1; //Valor retornado si los posibles ojos se aceptan
	BYTE *p=(BYTE*)hgray->imageData;
	int sx=hgray->widthStep;
	int sy=hgray->height;

	//Buscando en posición estándar
	//izquierdo
	rOISearch.x=Cmasa.x-(int)(saxis*0.75);
	rOISearch.y=Cmasa.y-(int)(laxis*0.5);
	rOISearch.width=(int)(saxis*0.75)-1;
	rOISearch.height=(int)(laxis*0.5);

	ControlVentanaBusqueda(&rOISearch);

	//Derecho			
	rODSearch.x=Cmasa.x+1;
	rODSearch.y=Cmasa.y-(int)(laxis*0.5);
	rODSearch.width=(int)(saxis*0.75)-1;
	rODSearch.height=(int)(laxis*0.5);

	ControlVentanaBusqueda(&rODSearch);

	if (boUsaProyecciones)//Usa las proyecciones para definir la zona de los posibles ojos
	{
		//Si la y para buscar es más pequeña que el primer mínimo de proyección cuidado
		//Si la proyección posible de los ojos, no está en la ventana
		//Si tenemos pareja de mínimos
	    //Busca un poco más abajo que el primer mínimo (podría utilizar por hemisferio)
		if (facevm_y[1]!=-1)
		{
			int dif=facevm_y[1]-facevm_y[0];

			//Confirmar que la segunda proyección está dentro del rango de búsqueda
			if (facevm_y[1]<rOISearch.y || facevm_y[1]>rOISearch.y+rOISearch.width)
			{
				//rOISearch.y= facevm_y[0]+dif*0.3;
				rOISearch.y= facevmI_y[0]+(int)(dif*0.3);
				
			}
			else
			{
				if (rOISearch.y<facevm_y[0])
				{
					//rOISearch.y=facevm_y[0]+dif*0.3;
					rOISearch.y= facevmI_y[0]+(int)(dif*0.3);
					
					
				}
			}
			//Si la y para buscar es más pequeña que el primer mínimo de proyección cuidado
			//Si la proyección posible de los ojos, no está en la ventana
			if (facevm_y[1]<rODSearch.y || facevm_y[1]>rODSearch.y+rODSearch.width)
			{
				//rODSearch.y= facevm_y[0]+dif*0.3;
				rODSearch.y= facevmD_y[0]+(int)(dif*0.3);
				
			}
			else
			{
				if (rODSearch.y<facevm_y[0])
				{
					//rODSearch.y=facevm_y[0]+dif*0.3;
					rODSearch.y= facevmD_y[0]+(int)(dif*0.3);
					
				}
			}

		
		}
	}

//En la imagen de gris busca el mínimo en la misma ventana
BuscaMinimob(p,sx,sy,vi,oix,oiy,rOISearch.x,rOISearch.y,rOISearch.width,rOISearch.height);
		//En la imagen de gris busca el mínimo en la misma ventana
BuscaMinimob(p,sx,sy,vd,odx,ody,rODSearch.x,rODSearch.y,rODSearch.width,rODSearch.height);

//Test para mejorar la localización de los ojos, realiza nueva búsqueda cuando son ojijuntos

//Si están muy cerca (ojijuntos) hacer una búsqueda para el más cernano al centro, excluyendo la zona donde antes encontró
int iIntereyes=*odx- *oix;

if (boTestOjijunto)
{
//La distancia interocular debe tener cierta relación con la elipse
if (!(iIntereyes>saxis*0.80 && iIntereyes<saxis*1.5))
{
	int icend,iceni;
	//Distancia al centro
	icend= *odx-Cmasa.x;
	iceni=Cmasa.x- *oix;
	//De momento buscamos sólo el más cercano pero quizás habría que buscar ambos
	//Recortamos de la ventana en función saxis
	if (iceni<icend)
	{//Buscamos de nuevo el izquierdo
		rOISearch.width=(int)((*oix-(int)(saxis*0.1))-rOISearch.x);
		//Si los ojos no son horizontales, 
		// y el izquierdo tiene mayor y recorta un poco en altura (agujero nariz)
		if (!(MY_ABS(*ody - *oiy)<=(int)(saxis/6.0+0.5)) && *oiy>*ody)
			rOISearch.height-=(int)(saxis*0.15);
		BuscaMinimob(p,sx,sy,vi,oix,oiy,rOISearch.x,rOISearch.y,rOISearch.width,rOISearch.height);

		}
	else
	{//Buscamos el derecho
		int desp=(*odx-rODSearch.x)+(int)(saxis*0.1);
		rODSearch.x+=desp;
		rODSearch.width-=desp;
			//Si los ojos no son horizontales, 
		// y el derecho tiene mayor y recorta un poco en altura (agujero nariz)
		if (!(MY_ABS(*ody - *oiy)<=(int)(saxis/6.0+0.5)) && *ody>*oiy)
			rODSearch.height-=(int)(saxis*0.15);
		BuscaMinimob(p,sx,sy,vd,odx,ody,rODSearch.x,rODSearch.y,rODSearch.width,rODSearch.height);

	}
}
}

return OK;
}


/*************************************************************************************************
	
	Searches the eyes given a likely window. The window is divided in two zones to perform the search
	
		h				Image	
		x1,x2,y1,y2		Window to search
		ojoDx, ...		returning values

*************************************************************************************************/
int CENCARA2_2Detector::BuscaOjosconViola(CvHaarClassifierCascade* cascadeD,
										   CvHaarClassifierCascade* cascadeI,
										   IplImage *h,int x1,int y1,int x2,int y2,
											int *ojoDx,int *ojoDy,int *ojoIx,int *ojoIy)
{
	CvSeq* ojo=NULL;
	int nojos=0;

	//Checks image limits
	if (x1<0)
		x1=0;

	if (y1<0)
		y1=0;

	if (x2>h->width)
		x2=h->width-1;

	if (y2>h->height)
		y2=h->height-1;

	int despx;
	
	//after controlling limits
	int ancho=x2-x1,alto=y2-y1;

	if (ancho>8 && alto>4)
	{
		despx=(int)(x1+(int)(ancho*0.5));

		//Localizamos zona para buscar el ojo derecho (mitad derecha de la ventana)
		cvSetImageROI(h,cvRect(despx,y1,(int)(ancho*0.5),alto));

		cvClearMemStorage( ViolaStorage );
		
		//Setting the last parameter is faster
		ojo = cvHaarDetectObjects(h, cascadeD, ViolaStorage, 1.2);
	
		if (ojo)
		{
			nojos=(ojo ? ojo->total : 0);

			//Almacena datos si hubo detección ajustando en función de la ventana suministrada
			if (nojos)
			{
				long area=0, areamax=0;
				for (int i=0;i<nojos;i++)
				{
					CvRect r = *(CvRect*)cvGetSeqElem( ojo, i );

					area=(r.width)*(r.height);

					//Optará por la mayor
					if (area>areamax)
					{
						//Tomamos el punto medio de la zona detectada
						*ojoDx=(int)(despx+r.x+(r.width)*0.5);
						*ojoDy=(int)(y1+r.y+(r.height)*0.5);
						areamax=area;
						
					}
				}
			}

			cvClearSeq(ojo);
			ojo=NULL;			
		}
		
		cvResetImageROI(h);

		//Sólo si detectó el derecho busca el otro
		if (nojos)
		{
			despx=x1;

			//Localizamos zona para buscar el ojo izquierdo
			cvSetImageROI(h,cvRect(despx,(int)(y1),(int)(ancho*0.5),alto));
							
			cvClearMemStorage( ViolaStorage );				
						
			//Setting the last parameter is faster
			ojo = cvHaarDetectObjects(h, cascadeI, ViolaStorage, 1.2);

			if (ojo)
			{
				nojos=(ojo ? ojo->total : 0);

				//Almacena datos si hubo detección ajustando en función de la ventana suministrada
				if (nojos)
				{
					long area=0, areamax=0;
					for (int i=0;i<nojos;i++)
					{
						CvRect r = *(CvRect*)cvGetSeqElem( ojo, i );

						area=(r.width)*(r.height);

						//Optará por la mayor
						if (area>areamax)
						{
							//Tomamos el punto medio de la zona detectada
							*ojoIx=(int)(despx+r.x+(r.width)*0.5);
							*ojoIy=(int)(y1+r.y+(r.height)*0.5);
							areamax=area;
						}
					}
				}

				cvClearSeq(ojo);
				ojo=NULL;
			}

			cvResetImageROI(h);


		}
	}


	return nojos;
}


/*************************************************************************************************
	
	Searches mouth container based on Viola

		h				Image
		x1,y1,sx,sy		Search area
		MOUTHX1,...		Output data
		eyedist			Inter eyes distance
		interx, intery	Inbetween eyes position

*************************************************************************************************/
int CENCARA2_2Detector::BuscaBocaconViola(IplImage *h,int x1,int y1,int sx,int sy,
										  int *MOUTHX1,int *MOUTHY1,int *MOUTHX2,int *MOUTHY2,
										  float eyesdist,int interx,int intery)
{
	CvSeq* boca=NULL;
	int nboca=0,ret=0;
	long areamax=0;

	if (sx>1 && sy>0)
	{						
		cvSetImageROI(h,cvRect(x1,y1,(int)sx,(int)sy));

		cvClearMemStorage( ViolaStorage );
	   			
		//Setting the last parameter is faster
		if (ViolaMouthDetector->cascade)
			boca = cvHaarDetectObjects(h, ViolaMouthDetector->cascade, ViolaStorage, 1.2);
	
		if (boca)
		{
			nboca=(boca ? boca->total : 0);

			//Almacena datos si hubo detección ajustando en función de la ventana suministrada
			if (nboca)
			{
				long area=0, areamax=0,posy=0;
				float distinter;
				for (int i=0;i<nboca;i++)
				{
					CvRect r = *(CvRect*)cvGetSeqElem( boca, i );

					area=(r.width)*(r.height);

					distinter=MathUtils.DistanciaEuclidea(interx,intery,(int)(x1+r.x+(r.width)*0.5),(int)(y1+r.y+(r.height)*0.5));

					//El centroide del la zona debe estar a cierta distancia de la posición interocular
					if (eyesdist*0.65<distinter && eyesdist*1.4>distinter && r.width>eyesdist*0.75)
					{
						//Optará por la mayor que cumpla dichas condiciones
						if (area>areamax)
						{
							areamax=area;

							//Tomamos la zona detectada
							*MOUTHX1=(int)(x1+r.x);
							*MOUTHY1=(int)(y1+r.y);
							*MOUTHX2=(int)(x1+r.x+(r.width));
							*MOUTHY2=(int)(y1+r.y+(r.height));
							posy=(int)(y1+r.y+(r.height/2));

							ret=1;
						}
					}
				}
			}
			cvClearSeq(boca);
			boca=NULL;
		}
		cvResetImageROI(h);
	}

	return ret;
}

/*************************************************************************************************
	
	Searches nose container based on Viola

*************************************************************************************************/
int CENCARA2_2Detector::BuscaNarizconViola(IplImage *h,int x1,int y1,int sx,int sy,
										   int *narizx1,int *narizy1,int *narizx2,int *narizy2,
										   float eyesdist,int interx,int intery)
{
	CvSeq* nariz=NULL;
	int nnariz=0,ret=0;

	if (sx>1 && sy>0)
	{						
		cvSetImageROI(h,cvRect(x1,y1,(int)sx,(int)sy));

		cvClearMemStorage( ViolaStorage );
	   			
		//Setting the last parameter is faster
		if (ViolaNoseDetector->cascade)
			nariz = cvHaarDetectObjects(h, ViolaNoseDetector->cascade, ViolaStorage, 1.2);
	
		if (nariz)
		{
			nnariz=(nariz ? nariz->total : 0);

			//Almacena datos si hubo detección ajustando en función de la ventana suministrada
			if (nnariz)
			{
				long area=0, areamax=0;
				float distinter;
				for (int i=0;i<nnariz;i++)
				{
					CvRect r = *(CvRect*)cvGetSeqElem( nariz, i );

					area=(r.width)*(r.height);

					distinter=MathUtils.DistanciaEuclidea(interx,intery,(int)(x1+r.x+(r.width)*0.5),(int)(y1+r.y+(r.height)*0.5));

					//El centroide del la zona debe estar a cierta distancia de la posición interocular
					if (eyesdist*0.25<distinter && eyesdist*0.85>distinter && r.width>eyesdist*0.3 /*&& eyesdist*0.85>distinter && r.width>eyesdist*0.5*/)
					{
						//Optará por la mayor que cumpla dichas condiciones
						if (area>areamax)
						{
							//Tomamos la zona detectada
							*narizx1=(int)(x1+r.x);
							*narizy1=(int)(y1+r.y);
							*narizx2=(int)(x1+r.x+(r.width));
							*narizy2=(int)(y1+ r.y+(r.height));
							areamax=area;

							ret=1;
						}
					}
				}
			}
			cvClearSeq(nariz);
			nariz=NULL;
		}
		cvResetImageROI(h);
	}

	return ret;
}


/*****************************************************************
Búsqueda boca,por relaciones geométricas localiza una zona
tras detectar los ojos, para luego buscar casi en horizontal hacia 
los extremos. Aceptable para ausencia de expresión

	hgray	imagen
	bdx,bdy	boca derecha
	bix,biy	boca izquierda
	bdsupx,bdsupy	boca arriba derecha
	bdinfx,bdinfy boca abajo derecha
	interx, intery	punto promedio conocidos dos ojos
	distN	distancia interocular
	
******************************************************************/
void CENCARA2_2Detector::BuscaBoca_Grises(IplImage *hgray, int * bdx, int *bdy, int *bix, int * biy,int interx, int intery, int distN)
{
	unsigned char vauxd,vauxi;
	int valorparada=140;
	int	distN2=distN/2;
	int distancho=(int)((float)distN*0.4);
	CvRect		rBISearch,rBDSearch;

	//Minimo comisura de la boca derecho
	rBDSearch.x=interx;
	rBDSearch.y=intery+distN;
	//rBDSearch.width=(int)distN2;
	rBDSearch.width=(int)distancho;
	rBDSearch.height=(int)distN2;

	ControlVentanaBusqueda(&rBDSearch);

	//Minimo comisura de la boca izquierdo
	rBISearch.x=interx-distN2;
	rBISearch.y=intery+distN;
	//rBISearch.width=(int)distN2;
	rBISearch.width=(int)distancho;
	rBISearch.height=(int)distN2;

	ControlVentanaBusqueda(&rBISearch);

	//Busca en dos zonas posibles mínimos			
	BuscaMinimob((BYTE*)(hgray->imageData),width,height,&vauxd,bdx,bdy,rBDSearch.x,rBDSearch.y,rBDSearch.width,rBDSearch.height);

	BuscaMinimob((BYTE*)(hgray->imageData),width,height,&vauxi,bix,biy,rBISearch.x,rBISearch.y,rBISearch.width,rBISearch.height);

	bool boCambio;
	int nposx,nposy,val,val1,val2,val3;
	BYTE *p=(BYTE*)hgray->imageData;

	//Desde el mínimo encontrado hacia la derecha
	//Busca hacia la derecha (3 vecinos) valores cercanos
	//Busca comisura de la boca derecho
	do{
		boCambio=false;
		//Si está en la imagen
		//y no pasa la altura del ojo derecho
		if (*bdx +1 <width && *bdy +1 <height && *bdy >0
			&& *bdx<interx+distN2)
		{
			//toma el menor de los 3 vecinos
			nposx=*bdx;
			nposy=*bdy;
			val=*(p + (nposy * width) + nposx);
			val1=*(p + ((nposy-1) * width) + nposx +1);
			val2=*(p + (nposy * width) + nposx +1);
			val3=*(p + ((nposy +1) * width) + nposx +1);
			//Menor val1
			if (val1<val2 && val1<val3)
			{
				if (val1<80 || (val1<valorparada && val1-val<(val/2)))
				{
					nposx=*bdx +1;
					nposy=*bdy -1;
					boCambio=true;
				}
			}
			//Menor val2
			if (val2<=val1 && val2<val3)
			{
				if (val2<80 || (val2<valorparada && val2-val<(val/2)))
				{
					nposx=*bdx +1;
					nposy=*bdy;
					boCambio=true;
				}
			}
			//Menor val3
			if (val3<val2 && val3<val1)
			{
				if (val3<80 || (val3<valorparada && val3-val<(val/2)))
				{
					nposx=*bdx +1;
					nposy=*bdy +1;
					boCambio=true;
				}
			}
			*bdx=nposx;
			*bdy=nposy;
		}
	}while(boCambio);

	//Busca extremo de la boca izquierdo
	do{
		boCambio=false;
		//Si está en la imagen
		//y no pasa la altura del ojo izquierdo
		if (*bix  > 0 && *biy +1 <height && *biy >0
			&& *bix>interx-distN2)
		{
			//toma el menor de los 3 vecinos
			nposx=*bix;
			nposy=*biy;
			val=*(p + (nposy * width) + nposx);
			val1=*(p + ((nposy-1) * width) + nposx -1);
			val2=*(p + (nposy * width) + nposx -1);
			val3=*(p + ((nposy +1) * width) + nposx -1);
			if (val1<val2 && val1<val3)
			{
				if (val1<80 || (val1<valorparada && val1-val<(val/2)))
				{
					nposx=*bix -1;
					nposy=*biy -1;
					boCambio=true;
				}
			}
			if (val2<=val1 && val2<=val3)
			{
				if (val2<80 || (val2<valorparada && val2-val<(val/2)))
				{
					boCambio=true;
					nposx=*bix -1;
					nposy=*biy;
				}
			}
			if (val3<val2 && val3<val1)
			{
				if (val3<80 || (val3<valorparada && val3-val<(val/2)))
				{
					boCambio=true;
					nposx=*bix -1;
					nposy=*biy +1;
				}
			}
			*bix=nposx;
			*biy=nposy;
		}
	}while(boCambio);

}


/***************************************************************************************

	Searches eyes according to face container data using color

		i		Face index

*****************************************************************************************/
int CENCARA2_2Detector::BuscaOjosUsandoENCARA(int i)
{
int tipo=APARIENCIARARA;

if (!boImagenGrises)
{
	//Resetea rotaciones
	Blob->nRot=0;

	//Busca ojos con ENCARA considerando el color 
											
	//Estima dimensiones para la cara según su contenedor asociado
	int medioalto=(int)((DatosFaciales.Faces[i]->y2-DatosFaciales.Faces[i]->y1)*0.25);
	int vx1=DatosFaciales.Faces[i]->x1,vy1=DatosFaciales.Faces[i]->y1 - (int)(medioalto*0.5),
	vsx=(DatosFaciales.Faces[i]->x2-DatosFaciales.Faces[i]->x1),
	vsy=(DatosFaciales.Faces[i]->y2-DatosFaciales.Faces[i]->y1)+medioalto;

	//Comprueba dimensiones de la ventana de búsqueda
	IplUtils.WindowTest(image,&vx1,&vy1,&vsx,&vsy);										

	//Detecta ahora con el valor promedio en la ventana de detección correspondiente a esa cara 
	Blob->DetectaPixelsRangoColorenVentana(image,&(Blob->blobs),&(Blob->blobs_noDil),
		&(imageINr_norm),&(imageINg_norm),
		DatosFaciales.Faces[i]->rn_min,DatosFaciales.Faces[i]->rn_max,
		DatosFaciales.Faces[i]->gn_min,DatosFaciales.Faces[i]->gn_max,
		vx1,vy1,vsx,vsy);


	//Dilatamos
	cvDilate(Blob->blobs_noDil,Blob->blobs,NULL,1);

	//Sólo va a tratar un blob en esa zona, el mayor
	Blob->iBlobActual=0;			//Current blob being processed
	//Busca la componente mayor

	CvMemStorage* storageblob = cvCreateMemStorage(0); 

								
	//Para el blob mayor, estima la elipse, lo rota y localiza los elementos faciales
	int r=IplUtils.FindNLargestComponents(Blob->blobs,storageblob, &(Blob->contour),&(Blob->area),1);
	if (r)  //Analiza la mayor
	{
		//Busca el blob candidato de la cara
		Blob->DetectaBlobCaraSinCuello(image,imagerot,Blob->blob,Blob->blobrot,Blob->blobs);

		//No lo acepta si el ángulo es excesivo, ViolaJones no detecta en esa orientación tan extraña
		if (fabs(Blob->elipse[Blob->iBlobActual].angle)<18)
		{
			int nrotaciones=Blob->nRot,pasada=0;

			//Si hay acumuladas dos rotaciones probará con una y luego con las dos a menos que detecte ya con una
			if (nrotaciones==2)
			{
				//Guarda la última en un temporal
				Blob->nRot--;
				Blob->RotacionImagenaux1=Blob->HistoriaRotaciones[Blob->nRot];
			}
													
							
			//Hace tantas pasadas como rotaciones acumuladas para probar en todos los casos
			//Aquí para reloaded podemos plantear hacer una búsqueda sin rotar
			do{
				//Si estamos en la segunda rotación recuperamos
				if (pasada>0)
				{
					Blob->HistoriaRotaciones[Blob->nRot]=Blob->RotacionImagenaux1;
					Blob->nRot=nrotaciones;
					//Ya hemos rotado una vez, usa esas como partida para rotar de nuevo
					cvCopyImage(imagerot,dummy);
					cvCopyImage(Blob->blobrot,dummygray);
				}
				else
				{
					//Parte de las originales para aplicar una única rotación
					cvCopyImage(image,dummy);
					cvCopyImage(Blob->blob,dummygray);
					Blob->nRot=1;
				}

				//La roto
				//double xs=0,ys=0;
				//iplGetRotateShift(Blob->HistoriaRotaciones[pasada].elipse.center.x,
				//	Blob->HistoriaRotaciones[pasada].elipse.center.y,
				//	Blob->HistoriaRotaciones[pasada].elipse.angle,&xs,&ys);
				//cv2DRotationMatrix??

				//Computamos la rotación sólo en la zona del blob
				//cvSetImageROI(dummy,Blob->rect[Blob->iBlobActual]);
				//iplRotate(dummy,imagerot,Blob->HistoriaRotaciones[pasada].elipse.angle,xs,ys,IPL_INTER_NN);
				CIplImageUtils iplutils;
				iplutils.RotateImage(dummy,imagerot,Blob->HistoriaRotaciones[pasada].elipse.angle,Blob->HistoriaRotaciones[pasada].elipse.center.x,Blob->HistoriaRotaciones[pasada].elipse.center.y);
				//cvResetImageROI(dummy);
															
				//Rotar la imagen de blob
				//cvSetImageROI(dummygray,Blob->rect[Blob->iBlobActual]);
				//iplRotate(dummygray,Blob->blobrot,Blob->HistoriaRotaciones[pasada].elipse.angle,xs,ys,IPL_INTER_NN);
				//Sin ipl, en pruebas
				iplutils.RotateImage(dummygray,Blob->blobrot,Blob->HistoriaRotaciones[pasada].elipse.angle,Blob->HistoriaRotaciones[pasada].elipse.center.x,Blob->HistoriaRotaciones[pasada].elipse.center.y);
				
				//cvResetImageROI(dummygray);

				//Convierto la rotada a grises
				//iplColorToGray(imagerot,imagegray);
				cvCvtColor(imagerot,imagegray, CV_BGR2GRAY );

				//Caracteristicas con imagen de blob y original rotada
				tipo=BuscaCaracteristicasFacialesconColor(imagegray,imagegrayIN,
								Blob->rect[Blob->iBlobActual],
								Blob->HistoriaRotaciones[pasada].elipse);

											
				pasada++;


			}while(pasada<nrotaciones && tipo!=FRONTAL);

		}
													
		cvReleaseMemStorage( &storageblob );
	}//if r
											
												

	//Libera información de contornos		
	delete [] Blob->contour;
	delete [] Blob->area;

										
	}//No es imagen de grises

return tipo;
}

/****************************************************************************************

	Searches facial elements using the skin color blob ellipse. Designed for multiple faces

		hgray					Image
		hgrayIN					Original input image (without transformations)
		rect					Blob container
		elipse					Ellipse 			
			
****************************************************************************************/
	
int CENCARA2_2Detector::BuscaCaracteristicasFacialesconColor(IplImage *hgray,IplImage *hgrayIN,
	CvRect rect,CvBox2D32f elipse)
{
	int odx,ody,oix,oiy;

	//Por si queremos contabilizar el tiempo empleado
	clock_t taux=clock();

	//Inicializaciones
	BYTE	vd,vi;
	static int	ret=-100; //Inicializado a valor no válido
	int retojogrises;

	//Guardamos geometría de la elipse
	CvPoint Cmasa;
	float orient, laxis,  saxis;
	Cmasa.x=(int)elipse.center.x;
	Cmasa.y=(int)elipse.center.y;
	orient=(float)(elipse.angle*3.141592/180.0);
	saxis=elipse.size.height;
	laxis=elipse.size.width;

	//Evitando elipses con relación extraña entre las dimensiones o pequeñas
	//Por encima de 1.8 parece indicar que detecta el cuello
	//Restringimos orientación de elipse permitida
	if ( saxis>(float)15.0 && laxis>saxis*1.1 && laxis<saxis*2.9 && orient>-1.0 && orient<1.0)
	{

		//Se van a buscar los ojos
		boOjosBuscadosenGrises=true; 

		//Calcula proyecciones integral en y para definir la zona de búsqueda de los elementos faciales
		if (boUsaProyecciones)
			Proyecciones_Grises(hgray,Cmasa,laxis,saxis,false);

		//Busca los ojos en una zona localizada del blob detectado
		retojogrises=BuscaOjos_Grises(hgray,&odx,&ody,&vd,&oix,&oiy,&vi,Cmasa,laxis,saxis);

		//Decide comprobar primero los ojos por grises
		if (retojogrises==OK)
			ret=TestBuenaPosPardeOjos(hgray,odx,ody,vd,oix,oiy,vi,Cmasa,saxis,laxis);
		else ret=OJOSNOPARECIDOS;

		//Si no ha detectado un par candidato, chequea con los disponibles por seguimiento
		//Multiples candiodatos lo contiene
		/*if (boUsaCorrelacion &&	boCorrelaOjos && ret!=POSIBLEFRONTAL)
		{
			//Rellenar con ENCARA
		}*/

		float DistanciaEntreOjos;

		//Al hacer varias pruebas sobre hgray, necesitamos poder sdeshacer los cambios hechos dentro de las rutinas
		int nRotAnterior;

		IplImage *hgraytmp=cvCreateImage( cvSize(width,height), IPL_DEPTH_8U, 1 );	
		cvCopyImage(hgray,hgraytmp);

		//Cálculo de posiciones en imagen  de entrada (para pintar)
		//Para ojos sólo se necesitan como mucho dos
		//Obtiene las posiciones del iris en coordenadas imagen de entrada
		GetPositiononInputImage(odx,ody,&odx_in,&ody_in);
		GetPositiononInputImage(oix,oiy,&oix_in,&oiy_in);

		//Si hay candidatos, comprueba la apariencia
		if (ret==POSIBLEFRONTAL)
		{

			nRotAnterior=Blob->nRot;

			ret=TestAparienciaParaParOjos(hgraytmp,hgrayIN,&oix,&oiy,&odx,&ody,
										&DistanciaEntreOjos,
										UMBRALSUAVE,false,false,false);


			if (ret==POSIBLEFRONTAL)
				ret=FRONTAL;

		
			//Si no se acepta desprecio la posible rotación hecha 
			if (ret!=FRONTAL)
			{
				//Anulo el incremento en el índice de rotaciones
				Blob->nRot=nRotAnterior;
				//Recupero la imagen de gris sin rotar
				//iplCopy(hgray,hgraytmp);
				cvCopy(hgray,hgraytmp);
			}
			else//Ha actualizado, debe adaptarse
			{
				//Cálculo de posiciones en imagen  de entrada (para pintar)
				//Para ojos sólo se necesitan como mucho dos
				//Obtiene las posiciones del iris en coordenadas imagen de entrada
				GetPositiononInputImage(odx,ody,&odx_in,&ody_in);
				GetPositiononInputImage(oix,oiy,&oix_in,&oiy_in);

			}


		}

		
		//Si usamos multiples candidatos combina el resultado del seguimiento con
		//el resultado de la detecciòn en la zona de color

/*		if (boMultiplesCandidatos && boUsaCorrelacion)
		{
			//Si hemos probado por grises y no funcionó
			if (ret!=POSIBLEFRONTAL && ret!=FRONTAL)
			{
				ret=TestMultiplesCandidatos(hgray,hgraytmp,hgrayIN,&DistanciaEntreOjos,rect,&nRotAnterior);
			}
		}*/

		//Liberamos la copia temporal de hgray
		cvReleaseImage(&hgraytmp);	
}
else
{
	
	if (laxis<7.0 && saxis<7.0)
		ret=NOBLOBLOCATED;
	else	
		ret=SMALL;
}

return ret;
	}


/****************************************************************************************

	Searches facial elements using the eye pair container
    
		hgray					Image
		hgrayIN					Original input image (without transformations)
		x1,y1,x2,y2				Eye pair container
					
			
****************************************************************************************/
int CENCARA2_2Detector::BuscaCaracteristicasFacialessinColor(IplImage *hgray,IplImage *hgrayIN,
		int x1,int y1,int x2,int y2)
	{		
		int odx,ody,oix,oiy;

		//Por si queremos contabilizar el tiempo empleado
		clock_t taux=clock();

		//Inicializaciones
		BYTE	vd,vi;
		static int	ret=-100; //Inicializado a valor no válido
		int retojogrises;

		//Se van a buscar los ojos
		boOjosBuscadosenGrises=true; 

		//Aplicar antes proyecciones para delimitar mejor



		//Fin proyecciones
		//Busca los ojos en una zona localizada del blob detectado
		retojogrises=BuscaOjos_Grises(hgray,&odx,&ody,&vd,&oix,&oiy,&vi,x1,y1,x2,y2);

		//Decide comprobar primero los ojos por grises
		if (retojogrises==OK)
			ret=POSIBLEFRONTAL;
		else ret=OJOSNOPARECIDOS;

		
		float DistanciaEntreOjos;

		//Al hacer varias pruebas sobre hgray, necesitamos poder sdeshacer los cambios hechos dentro de las rutinas
		int nRotAnterior;

		IplImage *hgraytmp=cvCreateImage( cvSize(width,height), IPL_DEPTH_8U, 1 );	
		cvCopyImage(hgray,hgraytmp);

		//Cálculo de posiciones en imagen  de entrada (para pintar)
		//Para ojos sólo se necesitan como mucho dos
		//Obtiene las posiciones del iris en coordenadas imagen de entrada
		GetPositiononInputImage(odx,ody,&odx_in,&ody_in);
		GetPositiononInputImage(oix,oiy,&oix_in,&oiy_in);

		//Si hay candidatos, comprueba la apariencia sólo si el ojo izquierdo está a la izquierda en coordenadas de imagen de entrada.
		if (ret==POSIBLEFRONTAL && oix_in<odx_in)
		{

				nRotAnterior=Blob->nRot;
		
				ret=TestAparienciaParaParOjos(hgraytmp,hgray,&oix,&oiy,&odx,&ody,
											&DistanciaEntreOjos,
											UMBRALSUAVE,false,false,false);

		
				//Si no se acepta desprecio la posible rotación hecha 
				if (ret!=POSIBLEFRONTAL)
				{
					//Anulo el incremento en el índice de rotaciones
					Blob->nRot=nRotAnterior;
					//Recupero la imagen de gris sin rotar
					cvCopyImage(hgraytmp,hgray);
					
				}
				else//Ha actualizado, debe adaptarse
				{
					//Cálculo de posiciones en imagen  de entrada (para pintar)
					//Para ojos sólo se necesitan como mucho dos
					//Obtiene las posiciones del iris en coordenadas imagen de entrada
					GetPositiononInputImage(odx,ody,&odx_in,&ody_in);
					GetPositiononInputImage(oix,oiy,&oix_in,&oiy_in);
					
					ret=FRONTAL;
				}
			}

			
			//Si usamos multiples candidatos combina el resultado del seguimiento con
			//el resultado de la detecciòn en la zona de color

			/*if (boMultiplesCandidatos && boUsaCorrelacion && 0)
			{
				//Si hemos probado por grises y no funcionó
				if (ret!=POSIBLEFRONTAL && ret!=FRONTAL)
				{
					//Usa el rect del par ocular
					ret=TestMultiplesCandidatos(hgray,hgraytmp,hgrayIN,&DistanciaEntreOjos,cvRect(x1,y1,x2-x1,y2-y1),&nRotAnterior);
				}
			}*/

			//Liberamos la copia temporal de hgray
			cvReleaseImage(&hgraytmp);
				

	return ret;
	}


/****************************************************************************************************************/

/****************************************************************************************************

	Update detected face data

	hIN							Input image
	hgrayIN						Input image in gray format
	DatosFaciales				Facial data
	i							Current face
	eyex1,..					Eye pair container
	reyex,...					Eyes coordinates
	mouthx1,..					Mouth container
	mlx,..						Mouth corners
	nosex1,..					Nose container
	boEyeContainerAvailable		States if the eyes container is valid
	boEyesAvailable				States if the eyes coordinates are valid
	boLEyeAvailable				States if the left eye coordinates are valid
	boREyeAvailable				States if the right eye  coordinates are valid
	boMouthContainerAvailable	States if the mouth container is valid
	boMouthAvailable			States if the mouth corners are is valid
	boNoseAvailable				States if the nose container is valid

****************************************************************************************************/
void CENCARA2_2Detector::ActualizaDatosCarasenImagen(IplImage *hIN,IplImage *hgrayIN,
												  CFacialDataperImage	*DatosFaciales,int i,
												  int eyex1,int eyey1,int eyex2,int eyey2,
												  int reyex,int reyey,int leyex,int leyey,
												  int mouthx1,int mouthy1,int mouthx2,int mouthy2,
												  int mlx,int mly,int mrx,int mry,												  
												  int nosex1,int nosey1,int nosex2,int nosey2,
												  int nx,int ny,
												  bool boEyeContainerAvailable,bool boEyesAvailable,
												  bool boLEyeAvailable,bool boREyeAvailable,
												  bool boMouthContainerAvailable,bool boMouthAvailable,
												  bool boNoseContainerAvailable, bool boNoseAvailable)

{

	//Facial expressions
	DatosFaciales->Faces[i]->boSmiling=false; //By default (it should be externally activated currently)

	//Toma como ancho el múltiplo de 8 más cercano a la mitad del ancho de la detección
	int TrackWidth=((DatosFaciales->Faces[i]->x2-DatosFaciales->Faces[i]->x1)/16+1)*8;
	while (TrackWidth<=PATSIZEX || TrackWidth<=PATSIZEY) TrackWidth+=8;

	

	//Estimates a head location for non face detections
	if ( !(DatosFaciales->Faces[i]->hs || DatosFaciales->Faces[i]->fb || DatosFaciales->Faces[i]->lb) )
	{
		EstimateHSfromFace(DatosFaciales,i);
	}


	//Salva el patrón de la cara escalado si no ha sido seguido (y consecuentemente actualizado su patrón)
	//No tiene sentido para no secuencias
	if (!DatosFaciales->Faces[i]->headtracked && DatosFaciales->Faces[i]->persistence>3)
	{
		//Stores the resolution to track (tries to fit the pattern size to our fixed pattern size)
		DatosFaciales->Faces[i]->escalaCaraf=(float)(DatosFaciales->Faces[i]->x2-DatosFaciales->Faces[i]->x1)/(float)(PATSIZEX);

		//No la admito mayor o igual a 8 porque entonces la imagen que queda para buscar es m´s pequeña
		//que el ROI mínimo
		if (DatosFaciales->Faces[i]->escalaCaraf>=8.f)
			DatosFaciales->Faces[i]->escalaCaraf=7.5f;
			
		//Temps
		IplImage *temp,*temp2;

		cvResetImageROI(hIN);

		temp=cvCloneImage(hIN);

		DatosFaciales->Faces[i]->escalaCara=1;

		//Escoge el centro de la cara como zona para tomar el patrón
		int px=(DatosFaciales->Faces[i]->x2+DatosFaciales->Faces[i]->x1)/2,
			py=(DatosFaciales->Faces[i]->y2+DatosFaciales->Faces[i]->y1)/2;

		//Ajusta la escala entera a múltiplo de 2
		while (DatosFaciales->Faces[i]->escalaCara*2<DatosFaciales->Faces[i]->escalaCaraf)
		{
			temp2 = cvCreateImage( cvSize(temp->width/2,temp->height/2), 8, 3 );

			//Reduce la dimensión de la imagen a tratar
			cvPyrDown( temp, temp2 );
			cvReleaseImage(&temp);

			DatosFaciales->Faces[i]->escalaCara*=2;

			temp=cvCloneImage(temp2);
			cvReleaseImage(&temp2);

			px/=2;
			py/=2;
		}

		//Obtiene el patrón si la imagen es lo suficientemente grande
		if (DatosFaciales->Faces[i]->boHeadTracker)
		{
			DatosFaciales->Faces[i]->HeadTracker->SetPattern(temp,px,py);
		}
		else
		{
			if (boDebugLog)
			{
				char cadena[256];
				sprintf(cadena,"Aloja HeadTracker\n");

				DebugLog->WriteToLogFile(cadena);
			}

			//Aloja el tracker
			DatosFaciales->Faces[i]->HeadTracker=new COpenTracking(temp,px,py,40,40,PATSIZEX,PATSIZEY);
			DatosFaciales->Faces[i]->boHeadTracker=true;			
		}

		

		cvReleaseImage(&temp);
	}
	//Fin Salva patrón de la cabeza

	
#ifdef SAVEIMAGES
	if ( 1/*DatosFaciales->Faces[i]->persistence>=3 && !DatosFaciales->Faces[i]->fb
		&& DatosFaciales->Faces[i]->framestodie==-1*/)
	{
		//Usamos el ROI para reducir operaciones
		int ancho=DatosFaciales->Faces[i]->x2-DatosFaciales->Faces[i]->x1,
			alto=DatosFaciales->Faces[i]->y2-DatosFaciales->Faces[i]->y1,alto2=alto/8;

		int esqx=DatosFaciales->Faces[i]->x1,
			esqy=DatosFaciales->Faces[i]->y1-alto2,
			sx=ancho,sy=(int)(1.25*(float)alto);

		IplUtils.WindowTest(hgrayIN,&esqx,&esqy,&sx,&sy);

		cvSetImageROI(hIN,cvRect(esqx,esqy,sx,sy));

#ifndef ENCARA2INLINUX
		sprintf(cPath,"%s\\..\\CarasDetectadas\\Raro_%d.jpg",cWorkDataDir,(unsigned)time(NULL));
#else
		sprintf(cPath,"%s/../CarasDetectadas/Raro_%d.jpg",cWorkDataDir,(unsigned)time(NULL));
#endif

		IplImage *ipltmp  = cvCreateImage( cvSize(100,(int)((sy*100.0)/sx)), hIN->depth, hIN->nChannels );
		//IplImage *ipltmp  = cvCreateImage( cvSize(100,126), hIN->depth, hIN->nChannels );

		cvResize(hIN,ipltmp);
		
		cvSaveImage(cPath,ipltmp);

		cvResetImageROI(hIN);

		cvReleaseImage(&ipltmp);

	}

#endif

	//Si se dispone de los ojos
	if (boEyeContainerAvailable)
	{
		//Coordenadas contenedor ojos
		DatosFaciales->Faces[i]->eyepairx1=eyex1;
		DatosFaciales->Faces[i]->eyepairy1=eyey1;
		DatosFaciales->Faces[i]->eyepairx2=eyex2;
		DatosFaciales->Faces[i]->eyepairy2=eyey2;

		DatosFaciales->Faces[i]->eyepair=true;
	}
	else
		DatosFaciales->Faces[i]->eyepair=false;

	//	LK TRACKER only if we have detected a little bit and it was not tracked
	if (DatosFaciales->Faces[i]->persistence>3 && 
		(!DatosFaciales->Faces[i]->headtracked || 
		(DatosFaciales->Faces[i]->headtracked && DatosFaciales->Faces[i]->cornerCount<20) || 
		!DatosFaciales->Faces[i]->boLKTracker) && 0)
	{
		SaveGoodFeaturestoTrack(hgrayIN,DatosFaciales,i);
	}
	else
		DatosFaciales->Faces[i]->boLKTracker=false;


	
	//
	DatosFaciales->Faces[i]->boTorsoContainer=false;

	//EYES DETECTED
	if (boEyesAvailable || boLEyeAvailable || boREyeAvailable)
	{
		//Actualiza los ojos o el ojo disponible
		DatosFaciales->SetEyes(i,boEyesAvailable,boLEyeAvailable,leyex,leyey,boREyeAvailable,reyex,reyey,TrackWidth,hIN,hgrayIN);

#ifdef SAVEIMAGESNORMALIZED
		//Salva la imagen del rostro en ciertas condiciones
		if ( !(persistence%13) &&  (boEyesDetectedAtLeastOnceforThisFace && !boHS && !boFullBody) )
		{
			char cPath[256];			
			
#ifndef ENCARA2INLINUX
			sprintf(cPath,"%s\\..\\CarasDetectadas\\%d.jpg",cWorkDataDir,(unsigned)time(NULL));
#else
			sprintf(cPath,"%s/../CarasDetectadas/%d.jpg",cWorkDataDir,(unsigned)time(NULL));
#endif
		
			cvSaveImage(cPath,DatosFaciales->Faces[i]->iplNormalized);
		}
#endif

		DatosFaciales->Faces[i]->nose=false;
		DatosFaciales->Faces[i]->mouth=false;
	}
	else
	{
		//Indica que no hay datos de los elementos faciales disponibles a menos que un ojo pueda haber sido seguido
		DatosFaciales->Faces[i]->leyetracked=false;
		DatosFaciales->Faces[i]->reyetracked=false;
		DatosFaciales->Faces[i]->leye=false;
		DatosFaciales->Faces[i]->reye=false;
		DatosFaciales->Faces[i]->nose=false;
	}

	//Boca
	if (boMouthContainerAvailable || boMouthAvailable/*&& !DatosFaciales->Faces[i]->mouth*/)
	{
		int mlaux_x,mlaux_y,mraux_x,mraux_y;

		if (boMouthContainerAvailable)//Por detección
		{
			DatosFaciales->Faces[i]->mouthcontainer=true;
			DatosFaciales->Faces[i]->mouthx1=mouthx1;
			DatosFaciales->Faces[i]->mouthy1=mouthy1;
			DatosFaciales->Faces[i]->mouthx2=mouthx2;
			DatosFaciales->Faces[i]->mouthy2=mouthy2;

			//Dado el recuadro estima una posición para la boca
			mlaux_x=(int)((mouthx1+mouthx2)/2-TrackWidth*0.2);
			mlaux_y=(int)(mouthy1*0.65+mouthy2*0.35);
			mraux_x=(int)((mouthx1+mouthx2)/2+TrackWidth*0.2);
			mraux_y=(int)(mouthy1*0.65+mouthy2*0.35);
		}
		else//Por seguimiento
		{
			mlaux_x=mlx;
			mlaux_y=mly;
			mraux_x=mrx;
			mraux_y=mry;
		}

		DatosFaciales->SetMouth(i,mlaux_x,mlaux_y,mraux_x,mraux_y,DatosFaciales->Faces[i]->mouthtracked,TrackWidth,hIN);

		/*DatosFaciales->Faces[i]->mouth=true;
		DatosFaciales->Faces[i]->ml_x=mlaux_x;
		DatosFaciales->Faces[i]->ml_y=mlaux_y;
		DatosFaciales->Faces[i]->mr_x=mraux_x;
		DatosFaciales->Faces[i]->mr_y=mraux_y;


		//Al disponer de la boca carga el patrón a seguir si está activado el seguimiento
		if (boCorrelaBoca && !DatosFaciales->Faces[i]->mouthtracked)
		{	
			//Obtiene el patrón de la boca a seguir
			if (!DatosFaciales->Faces[i]->boLMouthTracker)
			{
				//Aloja el tracker de la boca
				DatosFaciales->Faces[i]->LMouthTracker=new COpenTracking(hIN,DatosFaciales->Faces[i]->ml_x,DatosFaciales->Faces[i]->ml_y,TrackWidth,TrackWidth,PATSIZEX,PATSIZEY);
				DatosFaciales->Faces[i]->boLMouthTracker=true;
			}
			else
			{
				DatosFaciales->Faces[i]->LMouthTracker->SetPattern(hIN,DatosFaciales->Faces[i]->ml_x,DatosFaciales->Faces[i]->ml_y);
			}

			if (!DatosFaciales->Faces[i]->boRMouthTracker)
			{
				//Aloja el tracker de la boca
				DatosFaciales->Faces[i]->RMouthTracker=new COpenTracking(hIN,DatosFaciales->Faces[i]->mr_x,DatosFaciales->Faces[i]->mr_y,TrackWidth,TrackWidth,PATSIZEX,PATSIZEY);
				DatosFaciales->Faces[i]->boRMouthTracker=true;
			}
			else
			{
				DatosFaciales->Faces[i]->RMouthTracker->SetPattern(hIN,DatosFaciales->Faces[i]->mr_x,DatosFaciales->Faces[i]->mr_y);
			}
		}*/	
	}
	else
	{
		DatosFaciales->Faces[i]->mouthcontainer=false;
		DatosFaciales->Faces[i]->mouth=false;
	}
	
	//Nariz
	if (boNoseContainerAvailable || boNoseAvailable)
	{
		int naux_x,naux_y;
		if (boNoseContainerAvailable)//Por detección
		{
			DatosFaciales->Faces[i]->nosecontainer=true;
			DatosFaciales->Faces[i]->nosex1=nosex1;
			DatosFaciales->Faces[i]->nosey1=nosey1;
			DatosFaciales->Faces[i]->nosex2=nosex2;
			DatosFaciales->Faces[i]->nosey2=nosey2;

			naux_x=(nosex1+nosex2)/2;
			naux_y=(nosey1+nosey2)/2;
		}
		else//Por seguimiento
		{
			naux_x=nx;
			naux_y=ny;
		}

		DatosFaciales->SetNose(i,naux_x,naux_y,DatosFaciales->Faces[i]->nosetracked,TrackWidth,hIN);
		
		
		/*DatosFaciales->Faces[i]->nose=true;
		DatosFaciales->Faces[i]->np_x=naux_x;
		DatosFaciales->Faces[i]->np_y=naux_y;
	
		//Al disponer de la nariz carga el patrón a seguir si está activado el seguimiento y no se ha seguido
		if (boCorrelaNariz && !DatosFaciales->Faces[i]->nosetracked)
		{	
			//Obtiene el patrón de la nariz a seguir
			if (!DatosFaciales->Faces[i]->boNoseTracker)
			{
			
				//Aloja el tracker de la nariz
				DatosFaciales->Faces[i]->NoseTracker=new COpenTracking(hIN,DatosFaciales->Faces[i]->np_x,DatosFaciales->Faces[i]->np_y,TrackWidth,TrackWidth,PATSIZEX,PATSIZEY);
				DatosFaciales->Faces[i]->boNoseTracker=true;
			}
			else
				DatosFaciales->Faces[i]->NoseTracker->SetPattern(hIN,DatosFaciales->Faces[i]->np_x,DatosFaciales->Faces[i]->np_y);
		}

		*/
	}
	else
	{
		DatosFaciales->Faces[i]->nosecontainer=false;
		DatosFaciales->Faces[i]->nose=false;
	}
	

	//Estimates Body if at least two features were allocated
	//if (boEyesAvailable	&& (DatosFaciales->Faces[i]->nose || DatosFaciales->Faces[i]->mouth) )
	if (boEyesAvailable	&& (DatosFaciales->Faces[i]->leye && DatosFaciales->Faces[i]->reye) )
	{
		//Skin histogram
		//Creates histogram
		if (!DatosFaciales->Faces[i]->SkinHist)
		{
			int h_bins = 30, s_bins = 32;
			int hist_size[] = {h_bins, s_bins};
			float h_ranges[] = { 0, 180 }; /* hue varies from 0 (~0°red) to 180 (~360°red again) */
			float s_ranges[] = { 0, 255 }; /* saturation varies from 0 (black-gray-white) to 255 (pure spectrum color) */
			float* ranges[] = { h_ranges, s_ranges };

			DatosFaciales->Faces[i]->SkinHist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );

			cvClearHist(DatosFaciales->Faces[i]->SkinHist);
		}

		//Siempre lo calcula
		cvSetImageROI(hIN,cvRect(DatosFaciales->Faces[i]->e_lx,DatosFaciales->Faces[i]->e_ly,
			(DatosFaciales->Faces[i]->e_rx-DatosFaciales->Faces[i]->e_lx),
			(int)((DatosFaciales->Faces[i]->e_rx-DatosFaciales->Faces[i]->e_lx)*1.5)));	

		//Calculamos el histograma
		IplImage* h_plane = cvCreateImage( cvGetSize(hIN), 8, 1 );
		IplImage* s_plane = cvCreateImage( cvGetSize(hIN), 8, 1 );
		IplImage* v_plane = cvCreateImage( cvGetSize(hIN), 8, 1 );
		IplImage* planes[] = { h_plane, s_plane };
		IplImage* hsv = cvCreateImage( cvGetSize(hIN), 8, 3 );

		cvCvtColor( hIN, hsv, CV_BGR2HSV );
		cvCvtPixToPlane( hsv, h_plane, s_plane, v_plane, 0 );
		cvCalcHist( planes,DatosFaciales->Faces[i]->SkinHist, 0, 0 );//el 1 en la tercera posición indica que se acumula, un 0 fuerza inicialización

		//Elimina ruido
		/*float min,max,dif;
		cvGetMinMaxHistValue(DatosFaciales->Faces[i]->SkinHist,&min,&max);
		dif = max- min;
			
		cvThreshHist(DatosFaciales->Faces[i]->SkinHist,min + dif*0.18);		*/
		
		cvReleaseImage(&h_plane);
		cvReleaseImage(&s_plane);
		cvReleaseImage(&v_plane);
		cvReleaseImage(&hsv);

		cvResetImageROI(hIN);		

		//Torso histogram

		//Needed for histogram computation
		DatosFaciales->Faces[i]->boTorsoContainer=EstimateBreastfromFace(hIN,DatosFaciales,i);

		if (DatosFaciales->Faces[i]->boTorsoContainer)
		{
			//Creates torso color histogram
			if (!DatosFaciales->Faces[i]->TorsoHist)
			{
				int h_bins = 30, s_bins = 32;
				int hist_size[] = {h_bins, s_bins};
				float h_ranges[] = { 0, 180 }; /* hue varies from 0 (~0°red) to 180 (~360°red again) */
				float s_ranges[] = { 0, 255 }; /* saturation varies from 0 (black-gray-white) to 255 (pure spectrum color) */
				float* ranges[] = { h_ranges, s_ranges };

				DatosFaciales->Faces[i]->TorsoHist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );

				cvClearHist(DatosFaciales->Faces[i]->TorsoHist);
			}
			//Falta comparar similutud con previo ...

			int tsx=DatosFaciales->Faces[i]->torsox2-DatosFaciales->Faces[i]->torsox1,
				tsy=DatosFaciales->Faces[i]->torsoy2-DatosFaciales->Faces[i]->torsoy1;
			

			//Siempre lo calcula
			cvSetImageROI(hIN,cvRect(DatosFaciales->Faces[i]->torsox1,DatosFaciales->Faces[i]->torsoy1,
				tsx,tsy));	

			//Calculamos el histograma de color
			IplImage* h_plane = cvCreateImage( cvGetSize(hIN), 8, 1 );
			IplImage* s_plane = cvCreateImage( cvGetSize(hIN), 8, 1 );
			IplImage* v_plane = cvCreateImage( cvGetSize(hIN), 8, 1 );
			IplImage* planes[] = { h_plane, s_plane };
			IplImage* hsv = cvCreateImage( cvGetSize(hIN), 8, 3 );

			cvCvtColor( hIN, hsv, CV_BGR2HSV );
			cvCvtPixToPlane( hsv, h_plane, s_plane, v_plane, 0 );
			cvCalcHist( planes,DatosFaciales->Faces[i]->TorsoHist,DatosFaciales->Faces[i]->boTorsoHistAlreadyInitialized, 0 );//el tercer parámetro acumula si es true

			
			cvReleaseImage(&h_plane);
			cvReleaseImage(&s_plane);
			cvReleaseImage(&v_plane);
			cvReleaseImage(&hsv);

			cvResetImageROI(hIN);		

			DatosFaciales->Faces[i]->boTorsoHistAlreadyInitialized=true;

		}	
	}


	//STABLE FACES OR EXEMPLARS
	//Eyes and at least another feature (nose or mouth) should be available
	if (boEyesAvailable && DatosFaciales->Faces[i]->eyestracked 
		&& (DatosFaciales->Faces[i]->nose || DatosFaciales->Faces[i]->mouth) 
		&& boLearningLive)
	{
		//If it has not been saved an exemplar for this detection threadmarked as exemplar
		if (!DatosFaciales->Faces[i]->boStableTrackedFace)
		{
			DatosFaciales->Faces[i]->boStableTrackedFace=true;
			DatosFaciales->Faces[i]->boStableTrackedFaceSaved=false;
		}
		else //it was already saved the first exemplar marked, the persistence of this stable pattern for current detection thread is increased
		{
			//if the pattern has not been stored, it is stored
			if (!DatosFaciales->Faces[i]->boStableTrackedFaceSaved)
			{
				DatosFaciales->Faces[i]->boStableTrackedFaceSaved=true;

				

				//Allocs new exemplar				
				DatosFaciales->Faces[i]->Exemplars.AllocNewExemplar();
				
				//Updates exemplar information
				DatosFaciales->Faces[i]->Exemplars.SetExemplarData(hIN,DatosFaciales->Faces[i]->iplFaceNormalized,
					DatosFaciales->Faces[i]->iplFaceContextNormalized,
					DatosFaciales->Faces[i]->iplFaceContextNormalizedMasked,
					DatosFaciales->Faces[i]->iplFaceNormalizedIN,
					cvRect(DatosFaciales->Faces[i]->torsox1,DatosFaciales->Faces[i]->torsoy1,
					DatosFaciales->Faces[i]->torsox2-DatosFaciales->Faces[i]->torsox1,
					DatosFaciales->Faces[i]->torsoy2-DatosFaciales->Faces[i]->torsoy1),
					DatosFaciales->Faces[i]->boTorsoHistAlreadyInitialized,
					DatosFaciales->Faces[i]->TorsoHist,
					PCARepFaceQuore);
			}
			else//Actualizamos las imágenes si encajan mejor con una cara según el error PCA
			{
				DatosFaciales->Faces[i]->Exemplars.SetExemplarImages(DatosFaciales->Faces[i]->Exemplars.iNImages-1,DatosFaciales->Faces[i]->iplFaceNormalized,
					DatosFaciales->Faces[i]->iplFaceContextNormalized,
					DatosFaciales->Faces[i]->iplFaceContextNormalizedMasked,
					DatosFaciales->Faces[i]->iplFaceNormalizedIN,
					PCARepFaceQuore);

			}

			//Quizás meterlo en SetExemplarImages !!!!!
			//Updates avg and persistence of the exemplar
			//DatosFaciales->Faces[i]->Exemplars.UpdateAvg(DatosFaciales->Faces[i]->iplFaceNormalized);

			DatosFaciales->Faces[i]->Exemplars.IncreasePersistence();
		}
	}
	else DatosFaciales->Faces[i]->boStableTrackedFace=false; //No stable face available
	

	if (boDebugLog)
	{
		char cadena[256];
		sprintf(cadena,"Actualizando cara fin\n");

		DebugLog->WriteToLogFile(cadena);
	}
}


/***********************************************************************************************************

			Containers estimation routines

***********************************************************************************************************/

void CENCARA2_2Detector::EstimateFacefromEyes(CFacialDataperImage	*DatosFaciales,int i,int oix,int oiy, int odx, int ody)
{
	DatosFaciales->EstimateFacefromEyes(i,oix,oiy,odx,ody,imagegrayIN->width,imagegrayIN->height);
}

void CENCARA2_2Detector::EstimateFacefromEyeandMouth(CFacialDataperImage	*DatosFaciales,int i,bool boLE,int ox,int oy, int mx, int my)
{
	DatosFaciales->EstimateFacefromEyeandMouth(i,boLE,ox,oy,mx,my,imagegrayIN->width,imagegrayIN->height);
}

void CENCARA2_2Detector::EstimateFacefromHS(CFacialDataperImage	*DatosFaciales,int i)
{
	DatosFaciales->EstimateFacefromHS(i,imagegrayIN->width,imagegrayIN->height);
}


void CENCARA2_2Detector::EstimateFacefromFB(CFacialDataperImage	*DatosFaciales,int i)
{
	//Estimo el contenedor de la cara
	//Estimo el contenedor de la cara
	int sx=DatosFaciales->Faces[i]->fbx2-DatosFaciales->Faces[i]->fbx1,
		sy=DatosFaciales->Faces[i]->fby2-DatosFaciales->Faces[i]->fby1;
	int wd=(int)((float)sx*0.3);
	int hd=(int)((float)sy*0.1);

	DatosFaciales->Faces[i]->x1=DatosFaciales->Faces[i]->fbx1+wd;
	DatosFaciales->Faces[i]->x2=DatosFaciales->Faces[i]->fbx2-wd;
	DatosFaciales->Faces[i]->y1=DatosFaciales->Faces[i]->fby1+hd;
	DatosFaciales->Faces[i]->y2=(int)(DatosFaciales->Faces[i]->fby2-(hd*7.2));

	IplUtils.WindowTestP(imagegrayIN,&DatosFaciales->Faces[i]->x1,&DatosFaciales->Faces[i]->y1,
		&DatosFaciales->Faces[i]->x2,&DatosFaciales->Faces[i]->y2);
}


void CENCARA2_2Detector::EstimateHSfromFace(CFacialDataperImage	*DatosFaciales,int i)
{

	DatosFaciales->EstimateHSfromFace(i,imagegrayIN->width,imagegrayIN->height);

}

bool CENCARA2_2Detector::EstimateBreastfromFace(IplImage *h,CFacialDataperImage	*DatosFaciales,int i)
{

	DatosFaciales->EstimateBreastfromFace(i,h->width,h->height);


	return DatosFaciales->EstimateBreastfromFace(i,h->width,h->height);
}


/***************************************************************************************************************

		UTILITIES

***************************************************************************************************************/
/*************************************************************************************************
Given the last face detection container, estimates a search window for the face detection
*************************************************************************************************/
void CENCARA2_2Detector::VentanaCoherenciaTemporal(IplImage *imageIN,int *esqx,int *esqy,int *winw,int *winh,int x1,int y1,int x2,int y2)
{
	//Tamaño de la nueva ventana que representa la zona a buscar
	//se corresponde a la anterior detectada modificado de forma heurística
	//en base al ancho y alto previo
	*winw=(int)(2.5*(x2-x1));
	*winh=(int)(2.0*(y2-y1));// era (int)(2.5*(y2-y1));

	//Centro detección previa
	int interx=(x2+x1)/2;
	int intery=(y2+y1)/2;

	//Comprueba que no supera los límites
	if ((*winw)>imageIN->width) *winw=imageIN->width;
	if ((*winh)>imageIN->height) *winh=imageIN->height;

	//Ajusta las esquinas según el nuevo tamaño de ventanita
	*esqx=(int)(interx - (*winw)*0.5);
	//Si la ventana se sale por un lado ajusta
	if (*esqx<0)
	{
		*winw =*winw + *esqx;
		*esqx=0;
	}

	*esqy=(int)(intery - (*winh)*0.5);
	//Si la ventana se sale por un lado ajusta
	if (*esqy<0)
	{
		*winh =*winh + *esqy;
		*esqy=0;
	}

	//Ajusta por el otro extremo
	while (*esqx+*winw>=imageIN->width)
		(*winw)--;

	while (*esqy+*winh>=imageIN->height)
		(*winh)--;

	while ((*esqx)<=0)
	{
		(*winw)--;
		(*esqx)++;
	}
	if ((*esqy)<=0)
	{
		(*winh)--;
		(*esqy)++;
	}


}


/*************************************************************************************************
Given the last face detection container, estimates a search window for the face detection
*************************************************************************************************/
void CENCARA2_2Detector::VentanaCoherenciaTemporalTorso(IplImage *imageIN,int *esqx,int *esqy,int *winw,int *winh,int x1,int y1,int x2,int y2)
{

	//Tamaño de la nueva ventana que representa la zona a buscar
	//se corresponde a la anterior detectada modificado de forma heuristica
	
	*winw=(int)(3*(x2-x1));
	*winh=(int)(2*(y2-y1));

	//Comprueba que no supera los límites
	if ((*winw)>width) *winw=imageIN->width;
	if ((*winh)>height) *winh=imageIN->height;

	//Ajusta las esquinas según el nuevo tamaño de ventanita
	*esqx=(int)(x1 - (*winw)*0.33);
	//Si la ventana se sale por un lado ajusta
	if (*esqx<0)
	{
		*winw =*winw + *esqx;
		*esqx=0;
	}

	*esqy=(int)(y2 - (*winh)*0.2);
	//Si la ventana se sale por un lado ajusta
	if (*esqy<0)
	{
		*winh =*winh + *esqy;
		*esqy=0;
	}

	//Ajusta por el otro extremo
	while (*esqx+*winw>=imageIN->width)
		(*winw)--;

	while (*esqy+*winh>=imageIN->height)
		(*winh)--;

	while ((*esqx)<0)
	{
		(*winw)--;
		(*esqx)++;
	}
	if ((*esqy)<0)
	{
		(*winh)--;
		(*esqy)++;
	}


}


/*************************************************************************************************
Given the last face detection container, estimates a search window for the HS detection
*************************************************************************************************/
bool CENCARA2_2Detector::VentanaCoherenciaTemporalHS(IplImage *imageIN,int *esqx,int *esqy,int *winw,int *winh,int x1,int y1,int x2,int y2)
{

	//Tamaño de la nueva ventana que representa la zona a buscar
	//se corresponde a la anterior detectada modificado de forma heuristica
	//siendo el doble del ancho y 1.5 veces al alto
	*winw=(int)(5*(x2-x1));
	*winh=(int)(5*(y2-y1));

	//Centro detección previa
	int interx=(x2+x1)/2;
	int intery=(y2+y1)/2;


	long AreaBusqueda=(*winw)*(*winh);

	//Comprueba que no supera los límites
	if ((*winw)>width) *winw=imageIN->width;
	if ((*winh)>height) *winh=imageIN->height;

	//Ajusta las esquinas según el nuevo tamaño de ventanita
	*esqx=(int)(interx - (*winw)*0.5);
	//Si la ventana se sale por un lado ajusta
	if (*esqx<0)
	{
		*winw =*winw + *esqx;
		*esqx=0;
	}

	*esqy=(int)(intery - (*winh)*0.35);
	//Si la ventana se sale por un lado ajusta
	if (*esqy<0)
	{
		*winh =*winh + *esqy;
		*esqy=0;
	}

	//Ajusta por el otro extremo
	while (*esqx+*winw>=imageIN->width)
		(*winw)--;

	while (*esqy+*winh>=imageIN->height)
		(*winh)--;

	while ((*esqx)<=0)
	{
		(*winw)--;
		(*esqx)++;
	}
	if ((*esqy)<=0)
	{
		(*winh)--;
		(*esqy)++;
	}

	//Compara el área estimada de búsqueda inicial con la posible final, y retorna false si es mucho menor
	if ( (float)AreaBusqueda/(float)((*winw)*(*winh)) > 1.8)
		return false;
	else
		return true;

}

/*************************************************************************************************
Given the last face detection container, estimates a search window for the Fullbody detection
*************************************************************************************************/
bool CENCARA2_2Detector::VentanaCoherenciaTemporalFB(IplImage *imageIN,int *esqx,int *esqy,int *winw,int *winh,int x1,int y1,int x2,int y2)
{

	//Tamaño de la nueva ventana que representa la zona a buscar
	//se corresponde a la anterior detectada modificado de forma heuristica
	//siendo el doble del ancho y 1.5 veces al alto
	*winw=(int)(4*(x2-x1));
	*winh=(int)(10*(y2-y1));

	long AreaBusqueda=(*winw)*(*winh);

	//Comprueba que no supera los límites
	if ((*winw)>width) *winw=imageIN->width;
	if ((*winh)>height) *winh=imageIN->height;

	//Ajusta las esquinas según el nuevo tamaño de ventanita
	*esqx=(int)(x1 - (*winw)*0.375);
	//Si la ventana se sale por un lado ajusta
	if (*esqx<0)
	{
		*winw =*winw + *esqx;
		*esqx=0;
	}

	*esqy=(int)(y1 - (*winh)*0.25);
	//Si la ventana se sale por un lado ajusta
	if (*esqy<0)
	{
		*winh =*winh + *esqy;
		*esqy=0;
	}

	//Ajusta por el otro extremo
	while (*esqx+*winw>=imageIN->width)
		(*winw)--;

	while (*esqy+*winh>=imageIN->height)
		(*winh)--;

	while ((*esqx)<=0)
	{
		(*winw)--;
		(*esqx)++;
	}
	if ((*esqy)<=0)
	{
		(*winh)--;
		(*esqy)++;
	}

	//Compara el área estimada de búsqueda inicial con la posible final, y retorna false si es mucho menor
	if ( (float)AreaBusqueda/(float)((*winw)*(*winh)) > 1.5)
		return false;
	else
		return true;

}

/*****************************************************************************************************************

	Adjust cvRect to fit in current image dimensions

****************************************************************************************************************/
void	CENCARA2_2Detector::ControlVentanaBusqueda(CvRect *rSearch)
{

	//Límite izquierdo
	if (rSearch->x<0)
	{
		rSearch->width+=rSearch->x;
		rSearch->x=0;
	}
	//Límite derecho
	if (rSearch->x>=width)
	{
		rSearch->width=0;
	}
	if (rSearch->x+rSearch->width>=width)
	{

		rSearch->width=(width - rSearch->x)-1;
	}

	//Límite superior
	if (rSearch->y<0)
	{
		rSearch->height+=rSearch->y;
		rSearch->y=0;
	}

	//Límite inferior
	if (rSearch->y>=height)
	{
		rSearch->height=0;
	}
	if (rSearch->y+rSearch->height>=height)
	{

		rSearch->height=(height - rSearch->y)-1;
	}

}


/**************************************************************************************************************
	Given position in input image, gets position in working image

		inputx inputy		Coordinates in original image
		outx,outy			Coordinates in working image
  *************************************************************************************************************/
void CENCARA2_2Detector::GetPositiononInputImage(int inputx,int inputy,int *outx,int *outy)
{

	float tx,ty;

	tx=(float)inputx;
	ty=(float)inputy;
	for (int i=Blob->nRot-1; i>=0;i--)
		{
			MathUtils.Rota2D(Blob->HistoriaRotaciones[i].center.x,
						Blob->HistoriaRotaciones[i].center.y,
						Blob->HistoriaRotaciones[i].angle,
						&tx,&ty);

		}
	*outx=(int)(tx+0.5);
	*outy=(int)(ty+0.5);
}


/**************************************************************************************************************
	Given position in working image, gets position in input image

		inputx inputy		Coordinates in working image
		outx,outy			Coordinates in original image
  *************************************************************************************************************/
void CENCARA2_2Detector::GetPositiononWorkImage(int inputx,int inputy,int *outx,int *outy)
{

	float tx,ty;

	tx=(float)inputx;
	ty=(float)inputy;
	for (int i=0;i<Blob->nRot;i++)
	{
		MathUtils.Rota2D(Blob->HistoriaRotaciones[i].center.x,
						Blob->HistoriaRotaciones[i].center.y,
						-Blob->HistoriaRotaciones[i].angle,
						&tx,&ty);

	}
	*outx=(int)(tx+0.5);
	*outy=(int)(ty+0.5);
}

/***********************************************************************************************************


		Utilities LK Tracker

		Given a face detection selects in that window the best features to track


***********************************************************************************************************/
void CENCARA2_2Detector::SaveGoodFeaturestoTrack(IplImage *hgrayIN,CFacialDataperImage	*DatosFaciales,int i)
{
	//Focalizamos la zona de interés
	int isx=(DatosFaciales->Faces[i]->x2-DatosFaciales->Faces[i]->x1)/2,
		isy=DatosFaciales->Faces[i]->y2-DatosFaciales->Faces[i]->y1;

	DatosFaciales->Faces[i]->cornerx=(int)(DatosFaciales->Faces[i]->x1+isx*0.5);
	DatosFaciales->Faces[i]->cornery=DatosFaciales->Faces[i]->y1;

	//Corners offset
	IplUtils.WindowTest(hgrayIN,&DatosFaciales->Faces[i]->cornerx,&DatosFaciales->Faces[i]->cornery,&isx,&isy);

	cvSetImageROI(hgrayIN,cvRect(DatosFaciales->Faces[i]->cornerx,DatosFaciales->Faces[i]->cornery,isx,isy));
		
	//Creamos imágenes auxiliares
	IplImage *imagecrop=cvCreateImage( cvSize(isx,isy), hgrayIN->depth, 1 );
	cvCopyImage(hgrayIN,imagecrop);

	//Temporales necesarias
	IplImage *imagecropf=cvCreateImage( cvSize(isx,isy), IPL_DEPTH_32F, 1 );
	IplImage *imagecrop2=cvCreateImage( cvSize(isx,isy), IPL_DEPTH_32F, 1);

	int win_size=10;

	if (DatosFaciales->Faces[i]->cornerCount<=0)
		DatosFaciales->Faces[i]->cornerCount=30;

	//Buscamos las características
	cvGoodFeaturesToTrack( imagecrop,imagecropf,imagecrop2,
					DatosFaciales->Faces[i]->corners, &DatosFaciales->Faces[i]->cornerCount,
					0.02, 10);//quality level and min distance

	/*cvFindCornerSubPix( imagecrop, DatosFaciales->Faces[i]->corners, DatosFaciales->Faces[i]->cornerCount,
		cvSize(win_size,win_size), cvSize(-1,-1),
                cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));*/

	
	//Reseteamos ROI
	cvResetImageROI(hgrayIN);

	//Liberamos temporales
	cvReleaseImage(&imagecrop);
	cvReleaseImage(&imagecropf);
	cvReleaseImage(&imagecrop2);

	//The tracker is available for this detection
	if (DatosFaciales->Faces[i]->cornerCount>0)
		DatosFaciales->Faces[i]->boLKTracker=true;
}




/********************************************************************************************************

	Locates better the eye pattern to track

*****************************************************************************************************/
bool CENCARA2_2Detector::AjustaZonaOjo(IplImage *image,int *posx,int *posy,float DistanciaInterocular)
{

//Intenta localizar un buen patrón para el iris, consistente en una zona oscura
BYTE *p,*pp,*baux;
int i,j;
BYTE min,max;

//Buscamos mínimo y máximo en la zona pasada
IplUtils.BuscaMinMax(image,&min,&max);
					
//Creamos imágenes auxiliares para trabajo del tamaño del patrón
IplImage *iplaux  = cvCreateImage( cvSize(EYEPATTERN, EYEPATTERN), IPL_DEPTH_8U, 1 );
IplImage *iplaux2  = cvCreateImage( cvSize(EYEPATTERN, EYEPATTERN), IPL_DEPTH_8U, 1 );
IplImage *iplaux3  = cvCreateImage( cvSize(EYEPATTERN, EYEPATTERN), IPL_DEPTH_8U, 1 );

//Copiamos el patrón en una imagen de trabajo
cvCopyImage(image,iplaux);

float nden=2;
int niter=0;
float fradioestimado=((float)DistanciaInterocular)/(float)10.0;
int areaEstimadaCorrecta=(int)(fradioestimado*fradioestimado*3.141592);
int area;

//Aplicamos umbral pero de forma inteligente para adaptarse al supuesto tamaño del ojo, en función de la última buena detección
	do{
		int idiff=(int)((float)(max-min)/nden);
							
		cvThreshold( iplaux,iplaux3, min+idiff, 255, CV_THRESH_BINARY_INV);
		area=0;
		p=(unsigned char *)iplaux3->imageData;

		//Calculamos el áreade la zona que supera el úmbral
		for (i=0;i<EYEPATTERN;i++,p+=iplaux3->widthStep)
		{
			pp=p;
			for (j=0;j<EYEPATTERN;j++,pp++)
			{
				if (*pp) area++;
			}
		}
							
		if (area>areaEstimadaCorrecta) nden+=0.5;
		else nden-=0.5;
		niter++;
	}while( (nden>2 && nden<8) && (area>80 || area<10) && niter<5);


	//Elimino todos los blobs menos el de mayor tamaño
	CvMemStorage* storage = cvCreateMemStorage(0);  
	CvSeq *contMayor;
	float farea;
	int r=IplUtils.FindLargestComponent(iplaux3,storage, &contMayor,&farea);

	if (r)  
	{

	//Imagen nueva con el blob mayor pintado
		cvSetZero(iplaux2);
#ifdef OPENCV31
		cvDrawContours(iplaux2,contMayor,127,127,0,CV_FILLED);
#else
		cvDrawContours(iplaux2,contMayor,CV_RGB(127,127,127),CV_RGB(127,127,127),0,CV_FILLED);

#endif
		cvReleaseMemStorage( &storage );
	}

	
	//Si al área es mínima retorno sin ajustar
	if (area<areaEstimadaCorrecta/2)
	{
#ifdef AJUSTAIRISPORDEFECTO
		//Por si se desplaza a un lado
		int cont=0,nx=0,ny=0;
		p=(unsigned char *)iplaux2->imageData;

		//Calcula centroide
		for (i=0;i<EYEPATTERN;i++,p+=iplaux2->widthStep)
		{
			pp=p;
			for (j=0;j<EYEPATTERN;j++,pp++)
			{
				if (*pp)
				{
					cont++;
					nx +=j;
					ny +=i;
				}
			}
		}

		//Libera imàgenes
		cvReleaseImage(&iplaux);
		cvReleaseImage(&iplaux2);
		cvReleaseImage(&iplaux3);

		if (cont)
		{
			*posx =nx/cont;
			*posy =ny/cont;

			if (abs(*posx - EYEPATTERN/2)<2 && abs(*posy - EYEPATTERN/2)<2)
				return true;
			else
				return false;
		}
		else 
			return true;

#else
		//Libera imàgenes
		cvReleaseImage(&iplaux);
		cvReleaseImage(&iplaux2);
		cvReleaseImage(&iplaux3);
		
		return true;
#endif
	}


//Elimino filas y columnas donde no hay suficientes pixels según el radio
	p=(unsigned char *)iplaux2->imageData;
//columnas

	for (i=0;i<EYEPATTERN;i++,p++)
	{
		int cont=0;
		baux=p;
		for (int j=0;j<EYEPATTERN;j++,baux+=iplaux2->widthStep)
			if (*baux)
				cont++;

		if (cont<fradioestimado/2)
		{
			baux=p;
			for (int j=0;j<EYEPATTERN;j++,baux+=iplaux2->widthStep)
				*baux=0;
		}
	}
//filas
	p=(unsigned char *)iplaux2->imageData;
//columnas
	for (i=0;i<EYEPATTERN;i++,p+=iplaux2->widthStep)
	{
		int cont=0;
		baux=p;
		for (int j=0;j<EYEPATTERN;j++,baux++)
			if (*baux)
				cont++;

		if (cont<fradioestimado/2)
		{
			baux=p;
			for (int j=0;j<EYEPATTERN;j++,baux++)
				*baux=0;
		}
	}

	//Fin eliminación zonas muy delgadas

	//Buscamos el centroide del blob escogido para recolocar
	*posx=0;
	*posy=0;
	int cont=0;
	p=(unsigned char *)iplaux2->imageData;
	for (i=0;i<EYEPATTERN;i++,p+=iplaux2->widthStep)
	{
		pp=p;
		for (j=0;j<EYEPATTERN;j++,pp++)
		{
			if (*pp)
			{
				cont++;
				*posx +=j;
				*posy +=i;
			}
		}
	}

	//Libero imàgenes
	cvReleaseImage(&iplaux);
	cvReleaseImage(&iplaux2);
	cvReleaseImage(&iplaux3);

	if (cont==0) 
		return true;

	*posx /=cont;
	*posy /=cont;

	if (abs(*posx - EYEPATTERN/2)<2 && abs(*posy - EYEPATTERN/2)<2)
		return true;
	else return false;

}

//Min and max, already contained in OpenCV
/******************************************************************************************
							Busca el mínimo en una imagen 

		pbImagen	Imagen donde buscar
		isx,isy		dimensiones de la imagen
		min, mx,my	valor y coordenadas del mínimo
		cx,cy		esquina superior izquierda ventana de búsqueda
		scx,scy		Tamaño de la zona de búsqueda

********************************************************************************************/
void CENCARA2_2Detector::BuscaMinimob(BYTE *pbImagen,int isx,int isy,unsigned char *min,int *mx,int *my,
							   int cx,int cy,int scx,int scy)
{
	int i,j;
	BYTE *p,*q;
	int fx=cx+scx,fy=cy+scy;

	int imin=1000;

	//Cuidando no estar fuera de la imagen
	if (cy<0) cy=0;
	if (cx<0) cx=0;
	if (cx+scx>=isx) fx=isx;
	if (cy+scy>=isy) fy=isy;

	p=pbImagen;

	//Busco mínimos en la ventana indicada
	for (i=cy,p+=cy*isx;i<fy;i++,p+=isx)
		for (j=cx,q=p+cx;j<fx;j++,q++)
		{
			//Si no es una posición nula
			if ((unsigned char) *q<imin)
			{
				imin=(unsigned char) *q;
				
				*mx=j;
				*my=i;
			}
		}

	*min=(unsigned char) imin;
}

/******************************************************************************************
							Busca el maximo en una imagen 

		pbImagen	Imagen donde buscar
		isx,isy		dimensiones de la imagen
		max, mx,my	valor y coordenadas del máximo
		cx,cy		esquina superior izquierda ventana de búsqueda
		scx,scy		Tamaño de la zona de búsqueda

 ********************************************************************************************/
void CENCARA2_2Detector::BuscaMaximob(BYTE *pbImagen,int isx,int isy,unsigned char *max,int *mx,int *my,int cx,int cy,int scx,int scy)
{
	int i,j;
	BYTE *p,*q;
	int fx=cx+scx,fy=cy+scy;

	*max=0;

	//Cuidando no estar fuera de la imagen
	if (cy<0) cy=0;
	if (cx<0) cx=0;
	if (cx+scx>=isx) fx=isx;
	if (cy+scy>=isy) fy=isy;

//	TRACE("Busca en %d,%d %d,%d\n",cx,cy,fx,fy);
	
		
	p=pbImagen;
	//Busco máximos en la ventana indicada (pensado para buscar donde se ha localizado color)
	for (i=cy,p+=cy*isx;i<fy;i++,p+=isx)
		for (j=cx,q=p+cx;j<fx;j++,q++)
		{
			//Si no es una posición nula
			if (*q>*max)
			{
				*max=*q;
				*mx=j;
				*my=i;
			}
		}

}




/*************************************************************************************************
	INTEGRAL PROJECTIONS
*************************************************************************************************/

/*************************************************************************************************
		Realiza la proyección en y (tras rotar) para localizar los elementos posteriormente

			hgray	imagen de grises a tratar
			contour	imagen de contorno zona ojos(salida)
			laxis saxis	valores ejes de la elipse actual
			Cmasa	centro de la elipse
			
************************************************************************************************/
void CENCARA2_2Detector::Proyecciones_Grises(IplImage *hgray, 
CvPoint Cmasa,float laxis, float saxis,bool boSingleFace)
{
	//Calculamos mínimos en la vertical
	int i,j;

	//Inicializo variables de mínimos
	for (i=0;i<MAXNUMPROYECCIONES;i++)
	{
		facevm_y[i]=-1;
		facevm_val[i]=100000;
		facevmD_y[i]=-1;
		facevmD_val[i]=100000;
		facevmI_y[i]=-1;
		facevmI_val[i]=100000;
		facehm_x[i]=-1;
		facehm_val[i]=100000;
	}

//Primero buscamos cejas y ojos
//límites para buscar ceja y ojos conforme a dimensiones de la elipse
//en y
int miny=Cmasa.y-(int)(laxis*0.6);
//Buscamos mucho hacia abajo por si hubiera barba o se corta el blob en la boca ...
int maxy=Cmasa.y+(int)(saxis);


//Pruebita para ajustar y máxima según correlaciones recientes sólo para el caso de búsqueda de una cara
/*if (boFirstEyesDetected && TestDeteccionFACEYOjosReciente(2000) && boSingleFace)
{
	int tmp=(int)((corr_ody+corr_oiy)/2+saxis*0.4);
	if (tmp>miny)
		maxy=tmp;
}*/
//Fin Pruebita

int dimy=maxy-miny;
//en x posiciones estándar para cara frontal
int minx=(int)(Cmasa.x-saxis*0.7);
int mminx=(int)(Cmasa.x-saxis*0.25);
int maxx=(int)(Cmasa.x+saxis*0.7);//eran0.75
int mmaxx=(int)(Cmasa.x+saxis*0.25);
int OImin=minx;
int OImax=mminx;
int ODmax=maxx;
int ODmin=mmaxx;
int dimx=maxx-minx;
BYTE *p,*pp;

long *lproy_ver=(long *)new unsigned char[dimy*sizeof(long)];
long *lproy_verD=(long *)new unsigned char[dimy*sizeof(long)];
long *lproy_verI=(long *)new unsigned char[dimy*sizeof(long)];
long *lproy_veraux=(long *)new unsigned char[dimy*sizeof(long)];
long *lproy_verDaux=(long *)new unsigned char[dimy*sizeof(long)];
long *lproy_verIaux=(long *)new unsigned char[dimy*sizeof(long)];
long *lproy_hor=(long *)new unsigned char[dimx*sizeof(long)];

int npasadas=1;

//Hace varias pasadas busca filas mínimo/busca x en segunda fila
for (int pas=0;pas<npasadas;pas++)
{

	//Número de mínimos encontrados
	nvm=nhm=nvmD=nvmI=0;
	p=(unsigned char *)hgray->imageData+hgray->widthStep*miny;
	//Para cada fila calcula la proyección, evitando la zona central, que si es una cara
	//frontal puede corresponder al puente nasal
	for (i=0;i<dimy;i++,p+=(hgray->widthStep))
	{
		BYTE *q=p+OImin;
		//Zona delimitada en x por las ventanas de búsqueda
		//Ahora no evito puente nasal (si las ventanas están bien colocadas)
		lproy_verIaux[i]=0;
		lproy_verDaux[i]=0;
		for(j=OImin;j<OImax;j++,q++)
			lproy_verIaux[i]+=*q;

		q=p+ODmin;
		for(j=ODmin;j<ODmax;j++,q++)
			lproy_verDaux[i]+=*q;

		lproy_veraux[i]=lproy_verIaux[i]+lproy_verDaux[i];

	}
	//Suaviza
	lproy_ver[0]=lproy_veraux[0];
	lproy_ver[dimy-1]=lproy_veraux[dimy-1];
	lproy_verD[0]=lproy_verDaux[0];
	lproy_verD[dimy-1]=lproy_verDaux[dimy-1];
	lproy_verI[0]=lproy_verIaux[0];
	lproy_verI[dimy-1]=lproy_verIaux[dimy-1];

	
	for (i=1;i<dimy-1;i++)
	{
		lproy_ver[i]=(lproy_veraux[i-1]+lproy_veraux[i]+lproy_veraux[i+1])/3;
		lproy_verD[i]=(lproy_verDaux[i-1]+lproy_verDaux[i]+lproy_verDaux[i+1])/3;
		lproy_verI[i]=(lproy_verIaux[i-1]+lproy_verIaux[i]+lproy_verIaux[i+1])/3;
	}


	//Pasamos proyecciones, filas/columnas a comprobar, desplazamiento de la primera,
	//umbral de distancia mínimo entre dos mínimos, el umbral de distancia máximo para elegir los 
	//dos mínimos, y el último valor si es positivo indica
	//el número de mínimos a los que se restringe
	//Buscamos los mínimos de la suma completa
	ProyeccionesBuscaMinimo(facevm_y,facevm_val,lproy_ver,dimy,miny,(int)(saxis/8),(int)(saxis/2),&nvm,2);
	ProyeccionesBuscaMinimo(facevmI_y,facevmI_val,lproy_verI,dimy,miny,(int)(saxis/8),(int)(saxis/2),&nvmI,2);
	ProyeccionesBuscaMinimo(facevmD_y,facevmD_val,lproy_verD,dimy,miny,(int)(saxis/8),(int)(saxis/2),&nvmD,2);

	if (nvm)
	{
		int idist;

		if (nvm==2)
		{
			//Nos colocamos desde por encima de las supuestas cejas
			//tan arriba para combatir gafas ...
			idist=(facevm_y[1]-facevm_y[0]);
			p=(unsigned char *)hgray->imageData+((facevm_y[1]+facevm_y[0])/2*(hgray->widthStep)+OImin);
		}
		else
		{
			//Si por alguna razón hubiera solo un mínimo, supone que es el ojo
			idist=(int)(saxis/10);
			p=(unsigned char *)hgray->imageData+((facevm_y[0]-(int)(idist/2))*(hgray->widthStep)+OImin);
		}
		//Para cada columna calcula la proyección, en un rango definido
		//por la distancia con el mínimo anterior (cejas)
		for (i=0;i<dimx;i++,p++)
		{
			BYTE *q=p;
		//Zona delimitada en x por las ventanas de búsqueda
			//Ahora no evito puente nasal (si las ventanas están bien colocadas)
			lproy_hor[i]=0;
			for(j=0;j<idist;j++,q+=hgray->widthStep)
				lproy_hor[i]+=*q;

		}


	//Busca los mínimos en X en zona concreta
	int cont=0;


	do{

		//Primero
		//Evitamos el posible inicio en subida
		bool boCrece=true;

		while (boCrece && cont<dimx-1){
			if (lproy_hor[cont+1]>=lproy_hor[cont])
				cont++;
			else
				boCrece=false;
		}

		//Bajamos hasta el mínimo
		bool boDecrece=true;

		while (boDecrece && cont<dimx-1){
			if (lproy_hor[cont+1]<=lproy_hor[cont])
				cont++;
			else
				boDecrece=false;
		}

		//Comprueba con el anterior si están demasiado cerca
		if (cont<dimx-1 && !(OImin+cont>OImax && OImin+cont<ODmin) )//Si no está al final
		{
			if (nhm && minx+cont-facehm_x[nhm-1]<saxis*0.75)
			{
				if (lproy_hor[cont]<facehm_val[nhm-1])
				{
					facehm_x[nhm-1]=OImin+cont;
					facehm_val[nhm-1]=lproy_hor[cont];
				}
			}
			else
			{
				
				facehm_x[nhm]=OImin+cont;
				facehm_val[nhm++]=lproy_hor[cont];
				
			}
		}

	}while(cont<dimx-1 && nhm<10);


	}

	//sucesivas vueltas
	OImin=facehm_x[0]-(int)(saxis*0.22);
	OImax=facehm_x[0]+(int)(saxis*0.22);
	ODmin=facehm_x[1]-(int)(saxis*0.22);
	ODmax=facehm_x[1]+(int)(saxis*0.22);

}



delete [] lproy_ver;
delete [] lproy_veraux;
delete [] lproy_verD;
delete [] lproy_verDaux;
delete [] lproy_verI;
delete [] lproy_verIaux;
delete [] lproy_hor;

//Buscamos para la segunda fila (pensamos que corresponde a los ojos,
//los mínimos en x


//Tras buscar los dos primeros mínimos, 
//buscamos la nariz, boca (y barbilla si lo hacemos hasta el 5)
//Siempre y cuando hayamos encontrado al menos uno
if (nvm)
{
	if (nvm==2)
		miny=facevm_y[1]+(facevm_y[1]-facevm_y[0]);//aquí usaremos la y del segundo mínimo guardado
	else
		miny=facevm_y[0]+(int)(saxis*0.3);

	//Evitamos que se salga
	if (miny>hgray->height) miny=hgray->height-1;

	//maxy=Cmasa.y+(int)(laxis*1.2);
	maxy=miny+(int)(saxis*2.0);
	//Evitamos que se salga
	if (maxy>hgray->height) maxy=hgray->height;
	dimy=maxy-miny;
	//Busca entre posiciones x de ojos
	if (nhm==2)
	{
		minx=facehm_x[0];
		maxx=facehm_x[1];
	}
	else
	{
		minx=Cmasa.x-(int)(saxis*0.3);
		maxx=Cmasa.x+(int)(saxis*0.3);
	}
	dimx=maxx-minx;
	p=(unsigned char *)hgray->imageData;

	lproy_ver=(long *)new unsigned char[dimy*sizeof(long)];

	//Posiciona
	p+=hgray->widthStep*miny+minx;

	//Calcula la proyección
	for (i=0;i<dimy;i++,p+=hgray->widthStep)
	{
		pp=p;
	//Zona delimitada en x por las ventanas de búsqueda
		lproy_ver[i]=0;
		for(j=minx;j<maxx;j++,pp++)
			lproy_ver[i]+=*pp;
		
	}

	//Buscamos los mínimos
	int cont=0;
	int nvmg2=0;
	do{

	//Primero
	//Evitamos el inicio en subida
	bool boCrece=true;

	while (boCrece && cont<dimy-1){
		if (lproy_ver[cont+1]>=lproy_ver[cont])
			cont++;
		else
			boCrece=false;
	}

	//Bajamos hasta el mínimo
	bool boDecrece=true;

	while (boDecrece && cont<dimy-1){
		if (lproy_ver[cont+1]<=lproy_ver[cont])
			cont++;
		else
			boDecrece=false;
	}

	if (cont<dimy-1) //Si no está al final
	{
		//Comprueba con el anterior si están cerca
		if (nvmg2 && miny+cont-facevm_y[nvm-1]<saxis/3)
		{
			if (lproy_ver[cont]<facevm_val[nvm-1])
			{
				facevm_y[nvm-1]=miny+cont;
				facevm_val[nvm-1]=lproy_ver[cont];
			}
		}
		else
		{
			if (miny+cont-facevm_y[1]>saxis/2)//suficientemente lejos de los ojos
			{
				facevm_y[nvm]=miny+cont;
				facevm_val[nvm++]=lproy_ver[cont];
				nvmg2++;
			}
		}	
	}

	}while(cont<dimy-1 && nvm<4);


/*//Desde el mínimo de la boca, busca un máximo hacia arriba (nariz)
cont=facevm_y[2]-miny;
int max=0;
do{
	cont--;
	if (lproy_ver[cont]>max) 
	{
		max=lproy_ver[cont];
		facevx_y=miny+cont;
	}
}while(cont && cont+miny-facevm_y[1]>saxis/4);

  */
//Hasta aquí funcionaba aceptablemente

//Si ha encontrado más de dos (después de lod dos del grupo 1, toma los dos menores
/*min1=1000000,min2=1000000;
idxmin1,idxmin2;
ymin1,ymin2;
if (nvmg2>3)
{
	for (i=2;i<nvm;i++)
	{
		if(facevm_val[i]<min1)
		{
			min2=min1;
			idxmin2=idxmin1;
			ymin2=ymin1;
			min1=facevm_val[i];
			ymin1=facevm_y[i];
			idxmin1=i;
		}
		else
		{ //Segundo mínimo
			if (facevm_val[i]<min2)
			{
				min2=facevm_val[i];
				ymin2=facevm_y[i];
				idxmin2=i;
			}
		}
	}

	if (idxmin1<idxmin2)
	{
		facevm_val[2]=min1;
		facevm_y[2]=ymin1;
		facevm_val[3]=min2;
		facevm_y[3]=ymin2;
	}
	else
	{
		facevm_val[2]=min2;
		facevm_y[2]=ymin2;
		facevm_val[3]=min1;
		facevm_y[3]=ymin1;
	}
	nvm=4;

	TRACE("Revisado çnvm %d\n",nvm);
	for (i=0;i<nvm;i++)
		TRACE("facevm_y[%d]=%ld,facevm_val[%d]=%ld\n",i,facevm_y[i],i,facevm_val[i]);

}*/

delete [] lproy_ver;
}


}



/*******************************************************************************************
	
	Para el vector de proyecciones pasado, localiza los mínimos
		
		pos				Salida, posiciones de mínimos
		val				Salida, valores de mínimos
		proy			Vector de proyecciones
		dim				Dimensión del vector de proyecciones
		min				Desplazamiento en la imagen del inicio de la zona proyectada
		th				Umbral de distancia mínima entre dos mínimos
		th2				Umbral de distancia máxima entre dos mínimos
		n				Número de mínimos localizados
		nmax			Número máximo de mínimos alocalizar

*******************************************************************************************/
void CENCARA2_2Detector::ProyeccionesBuscaMinimo(int *pos,int *val,long *proy,int dim,int min,
										  int th,int th2,int *n,int nmax)

{
//Inicializamos contadores de mínimos y de fila recorrida
int cont=0;//Contador de fila
*n=0;//Contador de mínimos

//Buscamos los mínimos
//Localizamos mínimos a partir de las proyecciones pasadas en proy
do{

	//Comienza eliminando zonas de crecimiento monótono, sube hasta el máximo
	bool boCrece=true;
	while (boCrece && cont<dim-1){
		if (proy[cont+1]>=proy[cont])
			cont++;
		else
			boCrece=false;
	}

	//Bajamos hasta el mínimo (a menos que estemos al final del vector)
	bool boDecrece=true;
	while (boDecrece && cont<dim-1){
		if (proy[cont+1]<=proy[cont])
			cont++;
		else
			boDecrece=false;
	}

	//Comprueba con el anterior mínimo almacenado está demasiado cerca (siempre que no esté al final del vector)
	if (cont<dim-1) 
	{
		//Chequea si el mínimo es de interés
		if (*n && min+cont-pos[*n-1]<th)//Si está cerca del anterior mínimo almacenado
		{
			if (proy[cont]<val[*n-1])//Está cerca pero es menor, borramos el anterior
			{
				pos[*n]=min+cont;
				val[*n]=proy[cont];
			}
		}
		else//No tenemos almacenado, o no está demasiado cerca
		{
			
			pos[*n]=min+cont;
			val[(*n)++]=proy[cont];
		}
	}

}while(cont<dim-1 && *n<10);//Recorre mientras no lleguemos al final del vector de proyecciones o no tengamos demasiadas

//Si queremos localizar un número de mínimos
if (nmax)
{
	//Si ha encontrado más de los requeridos, toma los nmax menores, pero no demasiado
	//distanciados para el tamaño del blob (para no escoger por ejemplo la boca

	//Inicializaciones
	long min1=1000000,min2=1000000;
	int idxmin1=0,idxmin2=0,i;
	int ymin1=0,ymin2;
	//Auxiliares
	int auxnvm=*n;
	bool boMinOK=false;

	//Si el número localizado es mayor que el solicitado
	if (*n>nmax)
	{

		do{
			for (i=0;i<auxnvm;i++)
			{
				if(val[i]<min1)
				{ //Almacena el valor de min1 en min2, no importa que la primera vez no tenga valor min1
					min2=min1;
					idxmin2=idxmin1;
					ymin2=ymin1;
					min1=val[i];
					ymin1=pos[i];
					idxmin1=i;
				}
				else
				{ //Segundo mínimo
					if (val[i]<min2)
					{
						min2=val[i];
						ymin2=pos[i];
						idxmin2=i;
					}
				}
			}

			//Si están denmasiado distanciados evita el inferior (puede ser la boca, bigote ...) y vuelve a buscar
			if (abs(ymin1-ymin2)>th2)
			{
				if (idxmin1<idxmin2)
					auxnvm=idxmin2;
				else auxnvm=idxmin1;
				min1=1000000;
				min2=1000000;
			}
			else 
				boMinOK=true;
		}while(!boMinOK);


		//Coloca ocupando las posiciones primeras en la estructura de mínimos
		if (idxmin1<idxmin2)
		{
			val[0]=min1;
			pos[0]=ymin1;
			val[1]=min2;
			pos[1]=ymin2;
		}
		else
		{
			val[0]=min2;
			pos[0]=ymin2;
			val[1]=min1;
			pos[1]=ymin1;
		}
		//Machaco los que ya no valen
		for (i=2;i<*n;i++)
		{
			pos[i]=-1;
			val[i]=0;
			pos[i]=-1;
			val[i]=100000;
		}

		*n=nmax;
	}
}
}




	/****************************************************************************************

				Pinta los datos de las caras detectadas (versión múltiples caras)

				imagen					Imagen donde pintar
				col
								
	****************************************************************************************/
	void CENCARA2_2Detector::PaintFacialData(IplImage *h,CvScalar color,bool Masked)
	{
		if (boDetectorAvailable)
		{
			DatosFaciales.PaintDetections(h,color,&stcvFont,Masked);
		}
	}

	
/********************************************************************************************************************

	Tests

********************************************************************************************************************/
/*********************************************************************************************************

  Checks eyes and face appearance

	hgray			Image

  *******************************************************************************************************/
int CENCARA2_2Detector::TestAparienciaParaParOjos(IplImage *hgray,IplImage *hgrayIN,
										   int *oix,int *oiy, int *odx, int *ody,
										   float *DistanciaEntreOjos,
										   int umbralAutoOjos,bool boICorrOK,bool boDCorrOK,bool boBuscaRestoElementos)
{
int ret=POSIBLEFRONTAL;//Empieza siendo inocente ;)



//Supone que en cualquier caso los ojos localizados deben estar a la izquierda el de menor x y a la derecha el de mayor
if (*odx<*oix)
{
	ret=APARIENCIARARA;
	return ret;
}

*DistanciaEntreOjos=MathUtils.DistanciaEuclidea(*odx,*ody,*oix,*oiy);

//Si los ojos candidatos detectados cumplen las exigencias de distancia mìinima y màxima
if (*DistanciaEntreOjos>=MINIMALINTEREYE && *DistanciaEntreOjos<=iMaximoInterOjo)
{	
	//Para minimizar el test de apariencia, rotamos según posición de los ojos candidatos
		if (MY_ABS(*oiy - *ody)>3) 
		{
			//Hace una copia de la imagan en tratamiento
			cvCopyImage(hgray,dummygray);
			cvCopyImage(imagerot,dummy);

			double xs=0,ys=0,angulo;
			float interx,intery;

			//Gets angle
			angulo=MathUtils.GetAngle4Rotation(*oix,*oiy,*odx,*ody,&interx,&intery);

			//Recupero las coordenadas de los ojos respecto a la imagen original
			//Posición en coordenadas de imagen de entrada	
			int tmpdx,tmpdy,tmpix,tmpiy;

			GetPositiononInputImage(*odx,*ody,&tmpdx,&tmpdy);
			GetPositiononInputImage(*oix,*oiy,&tmpix,&tmpiy);
					
			//Roto. Podrìa utilizarse el roi
			CIplImageUtils iplutils;
			iplutils.RotateImage(dummygray,hgray,(float)angulo,(float)interx,(float)intery);
			iplutils.RotateImage(dummy,imagerot,(float)angulo,(float)interx,(float)intery);

			//Añado al histórico rotaciones
			Blob->HistoriaRotaciones[Blob->nRot].center.x=(float)interx;
			Blob->HistoriaRotaciones[Blob->nRot].center.y=(float)intery;
			Blob->HistoriaRotaciones[Blob->nRot].angle=(float)angulo;
			Blob->HistoriaRotaciones[Blob->nRot].boFromElipse=false;
			
			Blob->nRot++;

			//Calculo nuevas posiciones para la imagen tras dos rotaciones
			//Recupera posiciones según la correlación en 
			//coordenadas imagen de trabajo (tras rotar)
			GetPositiononWorkImage(tmpdx,tmpdy,odx,ody);
			GetPositiononWorkImage(tmpix,tmpiy,oix,oiy);

		}

		if (*odx<*oix)
		{
			ret=APARIENCIARARA;
			return ret;
		}

	
		//TESTS APARIENCIA OJOS
		if (boTestPCAOjos)
		{
			//Obtiene errores de reconstrucción para los ojos según sus coordenadas en la imagen de entrada
			float *minaux;
			CvPoint leftE=cvPoint(*oix,*oiy);
			CvPoint rightE=cvPoint(*odx,*ody);

			IplImage *auxcara=cvCreateImage( cvSize(FACEQUOREX,FACEQUOREY), 8, 1 );

			minaux=GetPCAErrorforNormalizedCoordinates(hgray,leftE,rightE,-1,NULL,NULL,NULL,auxcara);

			//Chequea el error de reconstrucción devuelto para los dos ojos y la parte central de la cara
			if (minaux[1]<umbralAutoOjos && minaux[2]<umbralAutoOjos && minaux[3]<FACETHRESHOLD)//Era 17 el Face threshold
			{
				//Test SVM Cara (TBD)
				if (SVMFace && PCARepFaceQuore->PCAspace)
				{
					//Proyecta la imagen
					float *coefs=PCARepFaceQuore->PCAspace->Proyectar(auxcara);

					//Falta usar el SVM

					delete [] coefs;
				}				

				ret=POSIBLEFRONTAL;				
			}
			else 
				ret=NOFACE;

			cvReleaseImage(&auxcara);

		}


}
else
	ret=OJIJUNTO;


	return ret;
}

/******************************************************************************************************************

	Checks face appearance (if third parameter is set)

*******************************************************************************************************************/
int CENCARA2_2Detector::TestAparienciaTracking(IplImage *hgrayIN,float fdist, bool boTestsApariencia)
{
int ret2=-100;
int retAppOjos=POSIBLEFRONTAL,retApp=POSIBLEFRONTAL;
int odx,ody,oix,oiy;


	//Puntero temporal, utilizado para indicar la imagen desde
	//la que tomar la cara para normalizar
	IplImage *iplp=hgrayIN;

	//Variables utilizadas
	oiy=corr_oiy;
	ody=corr_ody;
	odx=corr_odx;
	oix=corr_oix;

	//Los ojos deben estar bien pasados
	if (odx<=oix)
		return NOFACE;
	
	
	//Rotamos si hace falta porque los ojos no están muy horizontales
	if (MY_ABS(corr_ody - corr_oiy)>3) 
	{
		//Gets angle for rotation according to eye location
		double xs=0,ys=0,angulo;
		float interx,intery;
		//
		angulo=MathUtils.GetAngle4Rotation(corr_oix,corr_oiy,corr_odx,corr_ody,&interx,&intery);

		//without ipl(en pruebas
		CIplImageUtils iplutils;
		iplutils.RotateImage(hgrayIN,dummygray,(float)angulo,(float)interx,(float)intery);
					
		//Quizás hace falta calcular la x porque puede estar bastante movida

		//Histórico rotaciones
		Blob->HistoriaRotaciones[Blob->nRot].center.x=(float)interx;
		Blob->HistoriaRotaciones[Blob->nRot].center.y=(float)intery;
		Blob->HistoriaRotaciones[Blob->nRot].angle=(float)angulo;
		Blob->HistoriaRotaciones[Blob->nRot].boFromElipse=false;
		
		Blob->nRot++;

		//Calculo nuevas posiciones para la imagen tras dos rotaciones
		//Recupera posiciones según la correlación en 
		//coordenadas imagen de trabajo (tras rotar)
		GetPositiononWorkImage(corr_odx,corr_ody,&odx,&ody);
		GetPositiononWorkImage(corr_oix,corr_oiy,&oix,&oiy);


		//Tras rotar actualiza el puntero de la imagen a normalizar
		//en este caso no es imagegrayIN
		iplp=dummygray; 
	}

	//Checks good eye configuration
	if (odx<=oix)
		return NOFACE;

	if (boTestsApariencia)
	{
		if (boTestPCAOjos)
		{
			//Obtiene errores de reconstrucción para los ojos
			float *minaux;
			CvPoint leftE=cvPoint(oix,oiy);
			CvPoint rightE=cvPoint(odx,ody);

			IplImage *auxcara=cvCreateImage( cvSize(FACEQUOREX,FACEQUOREY), 8, 1 );

			minaux=GetPCAErrorforNormalizedCoordinates(iplp,leftE,rightE,-1,NULL,NULL,NULL,auxcara);

			//Chequea el error de recosntrucción devuelto para los dos ojos y la parte central de la cara
			if (minaux[1]<UMBRALSUAVE && minaux[2]<UMBRALSUAVE && minaux[3]<FACETHRESHOLD)
			{
				//Test SVM Cara (TBD)
				if (SVMFace && PCARepFaceQuore->PCAspace)
				{
					//Proyecta la imagen
					float *coefs=PCARepFaceQuore->PCAspace->Proyectar(auxcara);

					//Falta usar el SVM

					delete [] coefs;
				}				

				retApp=POSIBLEFRONTAL;				
			}
			else 
				retApp=NOFACE;

			cvReleaseImage(&auxcara);

		}
	}// if (boTestsApariencia)
	
	return retApp; //Detectó como frontal
	

}

/*********************************************************************************************************

		Verifica la buena disposiciòn de un par de ojos
				
				hgray		Imagen de análisis
				dx,dy,ix,iy	Posiciones de los ojos candidatos (derecho e izquierdo)
				vd,vi		Valor del mínimo en ese punto
				Cmasa		Centro de la elipse
				sa,la		Dimensiones ejes de la elipse


  *******************************************************************************************************/
int CENCARA2_2Detector::TestBuenaPosPardeOjos(IplImage *hgray,int dx, int dy, int vd,int ix, int iy, int vi, CvPoint Cmasa,float sa,float la)
{

int			iDespOjos;
int			ret,lastret=0;

//Control posición ojos, deben estar casi horizontales
 
	if (MY_ABS(dy-iy)<=(int)(sa/6.0+0.5))
	{
		int iIntereyes=(int)MathUtils.DistanciaEuclidea(dx,dy,ix,iy);//dx-ix;

		//TRACE("iIntereyes %d sa %f\n",iIntereyes,sa);

		//La distancia interocular debe tener cierta relación con la elipse
		if (iIntereyes>sa*0.70 && iIntereyes<sa*1.5 && iIntereyes>=MINIMALINTEREYE && iIntereyes<=iMaximoInterOjo){
	
			int iborded,ibordei,icend,iceni;
			//Distancia al borde
			iborded=Cmasa.x+(int)sa-dx;
			//iborded=x2-dx;
			ibordei=ix-Cmasa.x+(int)sa;
			//ibordei=ix-x1;
			//Distancia al centro
			icend=dx-Cmasa.x;
			iceni=Cmasa.x-ix;


			//Si la distancia al borde no guarda una relación con la interdistancia ocular,
			//A similar distancia del centro y si está uno cerca de un borde y el otro central, son probablemente laterales
					
			if( ( (iIntereyes>=3*ibordei && (iceni>1.5*icend)) 
				|| (iIntereyes>=3*iborded && (icend>1.5*iceni)) 
				|| (iborded>2*(ibordei) && icend<sa/10)
				|| ((iborded)*2<(ibordei) && iceni<sa/10) 
				|| (icend>3*iceni) 
				|| (iceni>3*icend) 
				|| (iborded>2*(ibordei) && (iceni>2*icend)) 
				|| (ibordei>2*(iborded) && (icend>2*iceni)) ) 
				&& iIntereyes<sa*0.7)
				{
				//TRACE("ALgo lateral ibordei %d, iborded %d, iceni %d, icend %d\n",ibordei,iborded,iceni,icend);
				//Optimizar
				if (iborded>ibordei || (iceni>icend))
					ret=ALGOLATERALIZQ;
				else
					ret=ALGOLATERALDER;	
				}
			else{
				//Posibilidades de ser frontal

				ret=POSIBLEFRONTAL;
				
				}
			}
		else{//Ojijunto 

//			TRACE("Ojijunto sa %f la %f, Intereyes %d\n",sa,la,iIntereyes);
			//Ha detectado los ojos muy juntos, quizás sería un avance buscar nuevas zonas de búsqueda ya que tenemos un ojo
			//Prueba 
			if (lastret==ALGOLATERALIZQ || lastret==ALGOLATERALDER)
				ret=lastret;//Coherencia temporal !!!
			else{
				//Nos lavamos las manos mientras buscamos coherencia temporal
				iDespOjos=0;

							
				if (dx<Cmasa.x+5.0 && dy<Cmasa.y)
					ret=MASLATERALIZQ;
				else{
					if (ix>Cmasa.x-5.0 && dy<Cmasa.y)
						ret=MASLATERALDER;
					else
						ret=OJIJUNTO;
				}


			}//Ojijunto
		}

	}
	else //se llega aquí si no son horizontales, sería cuestión de ver si uno puede ser ojo, y buscar en su simétrico
		ret=NOHORIZONTALES;				
	
	return ret;
}



/******************************************************************************************************************

	Given previous eye detection, checks distances with current eye detections

*******************************************************************************************************************/
int CENCARA2_2Detector::TestPosOjosconPrevia(int oix_in,int oiy_in,int odx_in,int ody_in,CFacialDataperImage	*DatosFaciales,int i)
{
	int			UltimoOjoDxOK,UltimoOjoDyOK,UltimoOjoIxOK,UltimoOjoIyOK;		//Last eyes detected positions

//Calculo la distancia de la detección previa
UltimoOjoDxOK=DatosFaciales->Faces[i]->e_rx;
UltimoOjoDyOK=DatosFaciales->Faces[i]->e_ry;
UltimoOjoIxOK=DatosFaciales->Faces[i]->e_lx;
UltimoOjoIyOK=DatosFaciales->Faces[i]->e_ly;

//Desplazamiento máximo permitido depende del tamaño de la cara
float DistPrev=MathUtils.DistanciaEuclidea(UltimoOjoDxOK,UltimoOjoDyOK,UltimoOjoIxOK,UltimoOjoIyOK);
int idif=(int) (DistPrev*0.25);

//Si no están en posiciones cercanas, comprueba que el desplazamiento ha sido similar,
//Puede ocurrir que se hayan trasladado y/o acercado alejado
int idiftr=(int)(DistPrev*0.125);//Suponiendo traslación similar, máxima diferencia permitida


int difDx=odx_in-UltimoOjoDxOK;
int difDy=ody_in-UltimoOjoDyOK;
int difIx=oix_in-UltimoOjoIxOK;
int difIy=oiy_in-UltimoOjoIyOK;

//Chequea posiciones similares
	if (abs(difDx)<idif && abs(difDy)<idif &&
		abs(difIx)<idif && abs(difIy)<idif)
	{
		return POSIBLEFRONTAL;
	}
	else 
	{
		//Chequea desplazamiento análogo simétrico o no
		if (abs(difDx-difIx)<idiftr && abs(difDy-difIy)<idiftr )
		{
			return POSIBLEFRONTAL;
		}
		else
			return NOCOHERENTEPREVIA;

	}
}


/******************************************************************************************************************

	Part of FindFaces in charge of searching using previous frame info

	image2 Imagen en su resolución reducida

*******************************************************************************************************************/
void CENCARA2_2Detector::SearchUsingPreviousFrameDetections(int *NumFaces,int escala,IplImage *image2)
{
	int esqx,esqy,winw,winh;
	bool	*boNueva;
	int		NumFacesAux=0;
	int i,yy;

	//Si hubo detecciones en la imagen previa y está activado su uso, las busca en la imagen actual en zonas cercanas
	//combinando el tracking o la detección en una zona limitada
	if (DatosFacialesPrevios.NumFaces && boUsePreviousDetection)
	{
		//Gestión de la detección en la nueva imagen para comprobar si se vuelve a detectar esa cara
		bool *boDetectadodePrevia=(bool *)new unsigned char[DatosFacialesPrevios.NumFaces*sizeof(bool)];
		bool *boStilltoDie=(bool *)new unsigned char[DatosFacialesPrevios.NumFaces*sizeof(bool)];
		int DeteccionesdePrevia=0,NumFacesAuxNuevas=0;
		int anchoprevio,altoprevio;
		int anchoprevio2,altoprevio2;//Con resolución bajada
		int anchopreviohs,altopreviohs;
		int anchoprevio2hs,altoprevio2hs;//Con resolución bajada
		long AreaPrevia;
		long AreaPrevia2;//Para resolución bajada

		//Factor de escalado aplicado a la imagen a procesar
		float factor=(float)(1.0/(float)escala);
	
		//For each previous detection
		for (i=0;i<DatosFacialesPrevios.NumFaces;i++)
		{
			//A priori no hemos detectado nada
			boDetectadodePrevia[i]=false;
			boStilltoDie[i]=false;

			//Useful measures
			anchoprevio=DatosFacialesPrevios.Faces[i]->x2-DatosFacialesPrevios.Faces[i]->x1;
			altoprevio=DatosFacialesPrevios.Faces[i]->y2-DatosFacialesPrevios.Faces[i]->y1;
			anchopreviohs=DatosFacialesPrevios.Faces[i]->hsx2-DatosFacialesPrevios.Faces[i]->hsx1;
			altopreviohs=DatosFacialesPrevios.Faces[i]->hsy2-DatosFacialesPrevios.Faces[i]->hsy1;
			anchoprevio2=(int)(anchoprevio*factor);
			altoprevio2=(int)(altoprevio*factor);
			anchoprevio2hs=(int)(anchopreviohs*factor);
			altoprevio2hs=(int)(altopreviohs*factor);

			AreaPrevia=(long)(anchoprevio*altoprevio);
			AreaPrevia2=(long)(altoprevio2*anchoprevio2);


			//Initializes eye tracking
			DatosFacialesPrevios.Faces[i]->eyestracked=false;
			DatosFacialesPrevios.Faces[i]->leyetracked=false;
			DatosFacialesPrevios.Faces[i]->reyetracked=false;
			DatosFacialesPrevios.Faces[i]->mouthtracked=false;
			DatosFacialesPrevios.Faces[i]->nosetracked=false;

			
			//---CORRELATION---
		
			//Primero intenta localizar por correlación si se detectaron para esta detección 
			//previa los ojos (o uno de ellos)
			if (boUsaCorrelacion)
			{
#ifdef NewFeatures
				//Busca las características utiliando seguimiento y detección. Entre todos los candidatos escoge la configuración más probable en base a coordenadas normalizadas
				bool boLEye,boREye,boNose,boMouth,boLMouth,boRMouth;
				CvPoint LEye,REye,Nose,Mouth,LMouth,RMouth;

				//Debugging
				clock_t tdbg=clock();			
			
				SearchEyesMouthandNose(image,image2,&DatosFacialesPrevios,i,
					&boLEye,&LEye,&boREye,&REye,&boNose,&Nose,&boLMouth,&LMouth,&boRMouth,&RMouth,&boMouth,&Mouth,escala);

			if (boDebugLog)
			{
				char cadena[256];
				
				sprintf(cadena,"Tras SearchEyesMouthandNose (%ld)\n",clock() - tdbg );
			
				DebugLog->WriteToLogFile(cadena);
			}

				if (boLEye && boREye)
				{
					corr_odx=REye.x;
					corr_ody=REye.y;
					corr_oix=LEye.x;
                    corr_oiy=LEye.y;

					float fdist=MathUtils.DistanciaEuclidea(corr_odx,corr_ody,corr_oix,corr_oiy);
					int retprev=-1;

					if (fdist>=MINIMALINTEREYE && fdist<=iMaximoInterOjo)
					{
						//Test por posición con la anterior pasada por parámetro
						retprev=TestPosOjosconPrevia(corr_oix,corr_oiy,corr_odx,corr_ody,&DatosFacialesPrevios,i);

						//Si por posición es correcta o todo por correlación es coherente
						if (retprev==POSIBLEFRONTAL 
							//|| (ComisurasBocaCorrLost==OK && OjosCorrLost==OK)
							)
						{
							//Comprobamos su apariencia (quizás excesivo)

						}		
					}

					//Los acepta si el test de apariencia triunfa o si además ha detectado otra característica
					if (retprev==POSIBLEFRONTAL || (boMouth || boNose))
					{
						DeteccionesdePrevia++;
						boDetectadodePrevia[i]=true;
						
						DatosFacialesPrevios.Faces[i]->eyestracked=true;

						DatosFacialesPrevios.Faces[i]->reye=true;
						DatosFacialesPrevios.Faces[i]->leye=true;

						//Color detection flag
						DatosFacialesPrevios.Faces[i]->boColorBasedFound=false;
						//Tracking flag
						DatosFacialesPrevios.Faces[i]->headtracked=false;

						//Coordenadas de ojos  machaca las previas
						DatosFacialesPrevios.Faces[i]->e_rx=corr_odx;
						DatosFacialesPrevios.Faces[i]->e_ry=corr_ody;
						DatosFacialesPrevios.Faces[i]->e_lx=corr_oix;
						DatosFacialesPrevios.Faces[i]->e_ly=corr_oiy;

						//Not lost
						DatosFacialesPrevios.Faces[i]->framestodie=-1;

						//Estimate Face container from eyes location
						EstimateFacefromEyes(&DatosFacialesPrevios,i,corr_oix,corr_oiy,corr_odx,corr_ody);	
					}
					else
					{//?????????
						//The appearance was not accepted	
						/*DatosFacialesPrevios.Faces[i]->leyetracked=true;
						DatosFacialesPrevios.Faces[i]->reyetracked=true;

						//Saves eyes, just in case no other eye detection works
						DatosFacialesPrevios.Faces[i]->reye=true;
						DatosFacialesPrevios.Faces[i]->leye=true;

						//Coordenadas de ojos  machaca las previas
						DatosFacialesPrevios.Faces[i]->e_rx=corr_odx;
						DatosFacialesPrevios.Faces[i]->e_ry=corr_ody;
						DatosFacialesPrevios.Faces[i]->e_lx=corr_oix;
						DatosFacialesPrevios.Faces[i]->e_ly=corr_oiy;*/
					}					
				}
				else
				{
					//Checks if just one eye was tracked
					if (!boLEye && boREye)
					{
						corr_odx=REye.x;
						corr_ody=REye.y;
						DatosFacialesPrevios.Faces[i]->reyetracked=true;

						//Saves eyes, just in case no other eye detection works
						DatosFacialesPrevios.Faces[i]->reye=true;
						DatosFacialesPrevios.Faces[i]->leye=false;
					
						//Coordenadas de ojos  machaca las previas
						DatosFacialesPrevios.Faces[i]->e_rx=corr_odx;
						DatosFacialesPrevios.Faces[i]->e_ry=corr_ody;
					
					}
					if (boLEye && !boREye)
					{						
						corr_oix=LEye.x;
						corr_oiy=LEye.y;
						DatosFacialesPrevios.Faces[i]->leyetracked=true;

						//Saves eyes, just in case no other eye detection works
						DatosFacialesPrevios.Faces[i]->leye=true;
						DatosFacialesPrevios.Faces[i]->reye=false;
					
						//Coordenadas de ojos  machaca las previas
						DatosFacialesPrevios.Faces[i]->e_lx=corr_oix;
						DatosFacialesPrevios.Faces[i]->e_ly=corr_oiy;
					}
				}

				if (boNose)
				{
					DatosFacialesPrevios.Faces[i]->nosetracked=true;

				
					//Copia la nueva detección donde antes de forma temporal
					DatosFacialesPrevios.Faces[i]->np_x=Nose.x;
					DatosFacialesPrevios.Faces[i]->np_y=Nose.y;								
				}

				//Si parece haberlo encontrado
				if (boMouth)
				{
					DatosFacialesPrevios.Faces[i]->mouthtracked=true;
				
					//Copia la nueva detección donde antes de form temporal
					DatosFacialesPrevios.Faces[i]->ml_x=LMouth.x;
					DatosFacialesPrevios.Faces[i]->ml_y=LMouth.y;
					DatosFacialesPrevios.Faces[i]->mr_x=RMouth.x;
					DatosFacialesPrevios.Faces[i]->mr_y=RMouth.y;

#ifdef VALECONUNJOJOYLABOCA
					//Probando si ha detectado uno de los ojos y la boca ...
					if ( (boLEye && !boREye) ||	(!boLEye && boREye))
					{
						DeteccionesdePrevia++;
						boDetectadodePrevia[i]=true;	

						int ox,oy;

						if (boLEye)
						{
							ox=corr_oix;
							oy=corr_oiy;
						}
						else
						{
							ox=corr_odx;
							oy=corr_ody;
						}

						//Estimate Face container from eyes location
						EstimateFacefromEyeandMouth(&DatosFacialesPrevios,i,boLEye,ox,oy,(LMouth.x+RMouth.x)/2,(LMouth.y+RMouth.y)/2);	
					}
#endif
				}
 
			//FIN
		
#else
				float despLEye,despREye,despNose,despLMouth,despRMouth;

				//Inicializa variables seguimiento
				NarizCorrLost=BocaCorrLost=OjosCorrLost=BOTH_LOST;
				
				//Eyes using TrackingLib (if any is available)
				if (boCorrelaOjos && (DatosFacialesPrevios.Faces[i]->leye || DatosFacialesPrevios.Faces[i]->reye))
				{
					bool ReyeLost=true,LeyeLost=true;

					//Left eye
					if (DatosFacialesPrevios.Faces[i]->boLEyeTracker && DatosFacialesPrevios.Faces[i]->leye)
					{
						//Tracks left eye
						DatosFacialesPrevios.Faces[i]->LEyeTracker->Track(image);//Resolución original
						LeyeLost=DatosFacialesPrevios.Faces[i]->LEyeTracker->boLost;
					}

					//Right eye
					if (DatosFacialesPrevios.Faces[i]->boREyeTracker && DatosFacialesPrevios.Faces[i]->reye)
					{
						//Tracks right eye
						DatosFacialesPrevios.Faces[i]->REyeTracker->Track(image);//Resolución original
						ReyeLost=DatosFacialesPrevios.Faces[i]->REyeTracker->boLost;
					}

					//Si parece haberlo encontrado
					if (!LeyeLost && !ReyeLost)
					{
						OjosCorrLost=OK;

						corr_odx=DatosFacialesPrevios.Faces[i]->REyeTracker->x;
						corr_ody=DatosFacialesPrevios.Faces[i]->REyeTracker->y;
						corr_oix=DatosFacialesPrevios.Faces[i]->LEyeTracker->x;
						corr_oiy=DatosFacialesPrevios.Faces[i]->LEyeTracker->y;

						despREye=MathUtils.DistanciaEuclidea(corr_odx,corr_ody,
							DatosFacialesPrevios.Faces[i]->e_rx,
							DatosFacialesPrevios.Faces[i]->e_ry);

						despLEye=MathUtils.DistanciaEuclidea(corr_oix,corr_oiy,
							DatosFacialesPrevios.Faces[i]->e_lx,
							DatosFacialesPrevios.Faces[i]->e_ly);
					}
					else
					{
						if (!LeyeLost)
						{
							OjosCorrLost=RIGHT_LOST;

							corr_oix=DatosFacialesPrevios.Faces[i]->LEyeTracker->x;
							corr_oiy=DatosFacialesPrevios.Faces[i]->LEyeTracker->y;

							despLEye=MathUtils.DistanciaEuclidea(corr_odx,corr_ody,
								DatosFacialesPrevios.Faces[i]->e_rx,
								DatosFacialesPrevios.Faces[i]->e_ry);
						}
						if (!ReyeLost)
						{
							OjosCorrLost=LEFT_LOST;

							corr_odx=DatosFacialesPrevios.Faces[i]->REyeTracker->x;
							corr_ody=DatosFacialesPrevios.Faces[i]->REyeTracker->y;

							despREye=MathUtils.DistanciaEuclidea(corr_odx,corr_ody,
								DatosFacialesPrevios.Faces[i]->e_lx,
								DatosFacialesPrevios.Faces[i]->e_ly);
						}							
					}					
				}

				
				//Si localizó los ojos corr_odx,corr_ody,corr_oix,corr_oiy realiza test de apariencia frontal
				if (OjosCorrLost==OK )
				{
					float fdist=MathUtils.DistanciaEuclidea(corr_odx,corr_ody,corr_oix,corr_oiy);
					int retprev=-1;

					if (fdist>=MINIMALINTEREYE && fdist<=iMaximoInterOjo)
					{
						//Test por posición con la anterior pasada por parámetro
						retprev=TestPosOjosconPrevia(corr_oix,corr_oiy,corr_odx,corr_ody,&DatosFacialesPrevios,i);

						//Si por posición es correcta o todo por correlación es coherente
						if (retprev==POSIBLEFRONTAL 
							//|| (ComisurasBocaCorrLost==OK && OjosCorrLost==OK)
							)
						{
							//Comprobamos su apariencia (quizás excesivo)
							retprev=TestAparienciaTracking(imagegrayIN,fdist,true);
						}		
					}

					if (retprev==POSIBLEFRONTAL)
					{
						DeteccionesdePrevia++;
						boDetectadodePrevia[i]=true;
						
						DatosFacialesPrevios.Faces[i]->eyestracked=true;

						DatosFacialesPrevios.Faces[i]->reye=true;
						DatosFacialesPrevios.Faces[i]->leye=true;

						//Color detection flag
						DatosFacialesPrevios.Faces[i]->boColorBasedFound=false;
						//Tracking flag
						DatosFacialesPrevios.Faces[i]->headtracked=false;

						//Coordenadas de ojos  machaca las previas
						DatosFacialesPrevios.Faces[i]->e_rx=corr_odx;
						DatosFacialesPrevios.Faces[i]->e_ry=corr_ody;
						DatosFacialesPrevios.Faces[i]->e_lx=corr_oix;
						DatosFacialesPrevios.Faces[i]->e_ly=corr_oiy;

						//Not lost
						DatosFacialesPrevios.Faces[i]->framestodie=-1;

						//recuadro en base a ojos seguidos
						//Determina el ojo màs alto por si la cara està inclinada
						int eyeoffy,eyeoffxl=0,eyeoffxr=0,yval=corr_oiy;

						if (corr_ody<yval)
						{
							yval=corr_ody;
							eyeoffxl=(int)((corr_ody+corr_oiy)*0.5-yval);
						}
						else
							eyeoffxr=(int)((corr_ody+corr_oiy)*0.5-yval);

						eyeoffy=corr_oiy-yval;

						DatosFacialesPrevios.Faces[i]->x1=(int)(corr_oix-fdist*0.5-eyeoffxr);
						DatosFacialesPrevios.Faces[i]->y1=(int)(corr_oiy-eyeoffy-fdist*0.5);
						DatosFacialesPrevios.Faces[i]->x2=DatosFacialesPrevios.Faces[i]->x1+(int)(fdist*2.0+eyeoffxr+eyeoffxl);
						DatosFacialesPrevios.Faces[i]->y2=DatosFacialesPrevios.Faces[i]->y1+(int)(fdist*2.25+eyeoffy);

						IplUtils.WindowTestP(imagegrayIN,&DatosFacialesPrevios.Faces[i]->x1,&DatosFacialesPrevios.Faces[i]->y1,
								&DatosFacialesPrevios.Faces[i]->x2,&DatosFacialesPrevios.Faces[i]->y2);
					}
					else
					{//?????????
						//The appearance was not accepted	
						/*DatosFacialesPrevios.Faces[i]->leyetracked=true;
						DatosFacialesPrevios.Faces[i]->reyetracked=true;

						//Saves eyes, just in case no other eye detection works
						DatosFacialesPrevios.Faces[i]->reye=true;
						DatosFacialesPrevios.Faces[i]->leye=true;

						//Coordenadas de ojos  machaca las previas
						DatosFacialesPrevios.Faces[i]->e_rx=corr_odx;
						DatosFacialesPrevios.Faces[i]->e_ry=corr_ody;
						DatosFacialesPrevios.Faces[i]->e_lx=corr_oix;
						DatosFacialesPrevios.Faces[i]->e_ly=corr_oiy;*/
					}					
				}
				else
				{
					//Checks if just one eye was tracked
					if (OjosCorrLost==LEFT_LOST)
					{
						DatosFacialesPrevios.Faces[i]->reyetracked=true;

						//Saves eyes, just in case no other eye detection works
						DatosFacialesPrevios.Faces[i]->reye=true;
						DatosFacialesPrevios.Faces[i]->leye=false;
					
						//Coordenadas de ojos  machaca las previas
						DatosFacialesPrevios.Faces[i]->e_rx=corr_odx;
						DatosFacialesPrevios.Faces[i]->e_ry=corr_ody;
					
					}
					if (OjosCorrLost==RIGHT_LOST)
					{
						DatosFacialesPrevios.Faces[i]->leyetracked=true;

						//Saves eyes, just in case no other eye detection works
						DatosFacialesPrevios.Faces[i]->leye=true;
						DatosFacialesPrevios.Faces[i]->reye=false;
					
						//Coordenadas de ojos  machaca las previas
						DatosFacialesPrevios.Faces[i]->e_lx=corr_oix;
						DatosFacialesPrevios.Faces[i]->e_ly=corr_oiy;
					}
				}

				//Mouth using TrackingLib
				if (boCorrelaBoca)
				{
					if (DatosFacialesPrevios.Faces[i]->boLMouthTracker && DatosFacialesPrevios.Faces[i]->boRMouthTracker)
					{
						//Si hubo detección en la previa
						if (DatosFacialesPrevios.Faces[i]->mouth || 
							DatosFacialesPrevios.Faces[i]->mouthcontainer)
						{
							//Busca el patrón (sería bueno separarlo en dos)
							DatosFacialesPrevios.Faces[i]->LMouthTracker->Track(image);//Resolución original
							DatosFacialesPrevios.Faces[i]->RMouthTracker->Track(image);
						
							//Si parece haberlo encontrado
							if (!DatosFacialesPrevios.Faces[i]->LMouthTracker->boLost &&
								!DatosFacialesPrevios.Faces[i]->RMouthTracker->boLost)
							{
								DatosFacialesPrevios.Faces[i]->mouthtracked=true;

								despRMouth=MathUtils.DistanciaEuclidea(DatosFacialesPrevios.Faces[i]->RMouthTracker->x,
									DatosFacialesPrevios.Faces[i]->RMouthTracker->y,
									DatosFacialesPrevios.Faces[i]->mr_x,
									DatosFacialesPrevios.Faces[i]->mr_y);

								despLMouth=MathUtils.DistanciaEuclidea(DatosFacialesPrevios.Faces[i]->LMouthTracker->x,
									DatosFacialesPrevios.Faces[i]->LMouthTracker->y,
									DatosFacialesPrevios.Faces[i]->ml_x,
									DatosFacialesPrevios.Faces[i]->ml_y);

								//Copia la nueva detección donde antes de form temporal
								DatosFacialesPrevios.Faces[i]->ml_x=DatosFacialesPrevios.Faces[i]->LMouthTracker->x;
								DatosFacialesPrevios.Faces[i]->ml_y=DatosFacialesPrevios.Faces[i]->LMouthTracker->y;
								DatosFacialesPrevios.Faces[i]->mr_x=DatosFacialesPrevios.Faces[i]->RMouthTracker->x;
								DatosFacialesPrevios.Faces[i]->mr_y=DatosFacialesPrevios.Faces[i]->RMouthTracker->y;
							}
						}
					}
				}

			
				//Nose
				if (boCorrelaNariz)
				{
					if (DatosFacialesPrevios.Faces[i]->boNoseTracker)
					{
						//Si hubo detección en la previa
						if (DatosFacialesPrevios.Faces[i]->nose || 
							DatosFacialesPrevios.Faces[i]->nosecontainer)
						{

					
							//Busca el patrón
							DatosFacialesPrevios.Faces[i]->NoseTracker->Track(image);//Resolución original

							
							//Si parece haberlo encontrado
							if (!DatosFacialesPrevios.Faces[i]->NoseTracker->boLost)
							{
								DatosFacialesPrevios.Faces[i]->nosetracked=true;

								despNose=MathUtils.DistanciaEuclidea(DatosFacialesPrevios.Faces[i]->NoseTracker->x,
									DatosFacialesPrevios.Faces[i]->NoseTracker->y,
									DatosFacialesPrevios.Faces[i]->np_x,
									DatosFacialesPrevios.Faces[i]->np_y);

								//Copia la nueva detección donde antes de forma temporal
								DatosFacialesPrevios.Faces[i]->np_x=DatosFacialesPrevios.Faces[i]->NoseTracker->x;
								DatosFacialesPrevios.Faces[i]->np_y=DatosFacialesPrevios.Faces[i]->NoseTracker->y;								
							}
						}
					}
				}

				//Test geométricos
				//Prueba tests saltos posiciones, un salto exagerado de unos elementos con respecto a otros lo anula
				float avgdesp=1.f;//Para que nunca pueda ser 0 ya que el doble de 0 es 0
				int	ndesp=0;

				if (DatosFacialesPrevios.Faces[i]->leyetracked || DatosFacialesPrevios.Faces[i]->eyestracked)
				{
					avgdesp+=despLEye;
					ndesp++;
				}
				if (DatosFacialesPrevios.Faces[i]->reyetracked || DatosFacialesPrevios.Faces[i]->eyestracked)
				{
					avgdesp+=despREye;
					ndesp++;
				}
				if (DatosFacialesPrevios.Faces[i]->nosetracked)
				{
					avgdesp+=despNose;
					ndesp++;
				}
				if (DatosFacialesPrevios.Faces[i]->mouthtracked)
				{
					avgdesp+=despRMouth;
					avgdesp+=despLMouth;
					ndesp+=2;
				}

				//Cancela el seguimiento de elementos cuyo desplazamiento se pase mucho de la media 
				//La boca se mueve bastante en y, por lo que casca con este test si se toma toda la distancia
				if (ndesp>2 && 0)
				{
					avgdesp/=ndesp;

					if (DatosFacialesPrevios.Faces[i]->leyetracked && despLEye>2*avgdesp)
					{
						DatosFacialesPrevios.Faces[i]->leyetracked=false;
						DatosFacialesPrevios.Faces[i]->eyestracked=false;
						ndesp--;
					}
					if (DatosFacialesPrevios.Faces[i]->reyetracked && despREye>2*avgdesp)
					{
						DatosFacialesPrevios.Faces[i]->reyetracked=false;
						DatosFacialesPrevios.Faces[i]->eyestracked=false;
						ndesp--;
					}
					if (DatosFacialesPrevios.Faces[i]->nosetracked && despNose>2*avgdesp)
					{
						DatosFacialesPrevios.Faces[i]->nosetracked=false;
						ndesp--;
					}
					if (DatosFacialesPrevios.Faces[i]->mouthtracked && (despLMouth>2*avgdesp || despRMouth>2*avgdesp) )
					{
						DatosFacialesPrevios.Faces[i]->mouthtracked=false;
						ndesp-=2;
					}
				}
				//fin desplazamientos

			
				//Test posición cercana a la esperada dados ojos y nariz o boca
				if (DatosFacialesPrevios.Faces[i]->eyestracked && (DatosFacialesPrevios.Faces[i]->mouthtracked || DatosFacialesPrevios.Faces[i]->nosetracked))
				{
					int intereyesx,intereyesy;
					int intermouthx,intermouthy;

					//Vector en la dirección entre los ojos
					float n_eyesx,n_eyesy;
					//Vector perpendicular
					float np_eyesx,np_eyesy;

					n_eyesx=(float)(DatosFacialesPrevios.Faces[i]->e_rx-DatosFacialesPrevios.Faces[i]->e_lx);
					n_eyesy=(float)(DatosFacialesPrevios.Faces[i]->e_ry-DatosFacialesPrevios.Faces[i]->e_ly);

					float disteyes=MathUtils.DistanciaEuclidea(DatosFacialesPrevios.Faces[i]->e_lx,
						DatosFacialesPrevios.Faces[i]->e_ly,
						DatosFacialesPrevios.Faces[i]->e_rx,
						DatosFacialesPrevios.Faces[i]->e_ry);

					//Normalizamos
					n_eyesx/=disteyes;
					n_eyesy/=disteyes;

					intereyesx=(DatosFacialesPrevios.Faces[i]->e_lx+DatosFacialesPrevios.Faces[i]->e_rx)/2;
					intereyesy=(DatosFacialesPrevios.Faces[i]->e_ly+DatosFacialesPrevios.Faces[i]->e_ry)/2;
					
					//Obtiene la recta en la dirección perpendicular a los ojos
					float linex,liney;
					float mod,distproy;

					linex=(float)DatosFacialesPrevios.Faces[i]->e_rx;
					liney=(float)DatosFacialesPrevios.Faces[i]->e_ry;

					//Calculamos un punto en la dirección boca nariz
					MathUtils.Rota2D((float)intereyesx,(float)intereyesy,90,&linex,&liney);

					np_eyesx=(float)(linex-intereyesx);
					np_eyesy=(float)(liney-intereyesy);

					//normaliza un vector en esa dirección
					mod=MathUtils.DistanciaEuclidea((int)linex,(int)liney,intereyesx,intereyesy);

					np_eyesx/=mod;
					np_eyesy/=mod;					

					//¿Está la boca alejada?
					if (DatosFacialesPrevios.Faces[i]->mouthtracked)
					{
						intermouthx=(DatosFacialesPrevios.Faces[i]->ml_x+DatosFacialesPrevios.Faces[i]->mr_x)/2;
						intermouthy=(DatosFacialesPrevios.Faces[i]->ml_y+DatosFacialesPrevios.Faces[i]->mr_y)/2;

						mod=MathUtils.DistanciaEuclidea(intereyesx,intereyesy,intermouthx,intermouthy);
						distproy=(float)fabs((intermouthx-intereyesx)*n_eyesx+(intermouthy-intereyesy)*n_eyesy);

						if (distproy>0.3*disteyes)
							DatosFacialesPrevios.Faces[i]->mouthtracked=false;
					}

					//¿Está la nariz alejada?
					if (DatosFacialesPrevios.Faces[i]->nosetracked)
					{
						mod=MathUtils.DistanciaEuclidea(intereyesx,intereyesy,DatosFacialesPrevios.Faces[i]->np_x,DatosFacialesPrevios.Faces[i]->np_y);
						distproy=(float)fabs((DatosFacialesPrevios.Faces[i]->np_x-intereyesx)*n_eyesx+(DatosFacialesPrevios.Faces[i]->np_y-intereyesy)*n_eyesy);

						if (distproy>0.3*disteyes)
							DatosFacialesPrevios.Faces[i]->nosetracked=false;

					}
					//Falta comprobar que están bien colocados formando el triángulo

				}	
#endif
			}//Correlación
			//---END CORRELATION---

			if (boDebugLog)
			{
				char cadena[256];
				sprintf(cadena,"Tras buscar por correlación ha obtenido %d\n",(int)boDetectadodePrevia[i]);

				DebugLog->WriteToLogFile(cadena);
			}

			
			//---VIOLA CARAS---
			//Viola para caras en las distintas versiones (por defecto, propio y profile)
			//Si no ha localizado la cara por correlación busca por Viola
			if (!boDetectadodePrevia[i])
			{			
				int minw,minh,lscale;
				float lfactor;
				IplImage *ima4searching;

				//Fija el ROI
				if (anchoprevio<60)//No aplicar escalado en la búsqueda para caras pequeñas
				{
					ima4searching=image;

					//La cara es pequeña no aplica factor de escalado
					lfactor=1.f;
					lscale=1;

					//Tamaño mínimo a buscar
					minw=anchoprevio2;
					minh=altoprevio2;
				}
				else
				{
					ima4searching=image;

					
					lfactor=factor;
					lscale=escala;

					//Tamaño mínimo a buscar
					minw=anchoprevio2/2;
					minh=altoprevio2/2;
				}

				//Aplica factor de escalado correspondiente para seleccionar el ROI
				VentanaCoherenciaTemporal(ima4searching,&esqx,&esqy,&winw,&winh,
					(int)(DatosFacialesPrevios.Faces[i]->x1*lfactor),(int)(DatosFacialesPrevios.Faces[i]->y1*lfactor),
					(int)(DatosFacialesPrevios.Faces[i]->x2*lfactor),(int)(DatosFacialesPrevios.Faces[i]->y2*lfactor));


				//Define un ROI dependiendo de la detección anterior
				//Coloca el ROI al doble en área que la anterior detectada pero en la imagen de menor resolución,
				//por ello muntiplicamos por factor
				
				//Controla límites
				//Fija el ROI
				IplUtils.WindowTest(ima4searching,&esqx,&esqy,&winw,&winh);		
				cvSetImageROI(ima4searching,cvRect(esqx,esqy,winw,winh));
		
				IplImage *temproi=cvCreateImage( cvSize((int)(winw),(int)(winh)), 8, 3 );

				//Copia a una temporal
				cvCopyImage(ima4searching,temproi);

				//Resetea el ROI
				cvResetImageROI(ima4searching);

				//Aplica los distintos detectores de caras
				//Variables locales temporales
				bool boDetectadaCara=false;
				int px1,px2,py1,py2;

				//Aplica Viola por defecto
				if (ViolaFaceDetector->boCascada && boBuscaconViolaCaras)
				{
					//Busca en una imagen de resolución reducida (si así se solicitó)
					//Pasamos 1 como escala porque ya pasamos una imagen reducida
					//Busco no mucho menores que la detección previa
					ViolaFaceDetector->ProcessImage(temproi,1,false,cvSize(minw,minh));
					//ViolaFaceDetector->ProcessImage(temproi,1);//El cambio de escala ya lo asume factor
					
					//Si hubo detección actualiza temporales
					if (ViolaFaceDetector->NumObjects)
					{
						//Escoge la de tamaño más parecido
						int esc=ViolaFaceDetector->EscogeParecida(
							(int)(DatosFacialesPrevios.Faces[i]->x1*lfactor),(int)(DatosFacialesPrevios.Faces[i]->y1*lfactor),
							(int)(DatosFacialesPrevios.Faces[i]->x2*lfactor),(int)(DatosFacialesPrevios.Faces[i]->y2*lfactor),1);

						if (esc!=-1)//Corrige la escala más abajo
						{
							boDetectadaCara=true;
							px1=ViolaFaceDetector->x1[esc];
							py1=ViolaFaceDetector->y1[esc];
							px2=ViolaFaceDetector->x2[esc];
							py2=ViolaFaceDetector->y2[esc];
						}
					}
				}

				//Viola propio
/*				if (!boDetectadaCara && boUseAlternativeDetectors)
				{
					if (ViolaBasedOwnFaceDetector->boCascada && boBuscaconViolaCaras)
					{
						ViolaBasedOwnFaceDetector->ProcessImage(temproi,1);//El cambio de escala ya lo asume factor

						//Si hubo detección actualiza temporales
						if (ViolaBasedOwnFaceDetector->NumObjects)
						{
							//Escoge la de tamaño más parecido
							int esc=ViolaBasedOwnFaceDetector->EscogeParecida(
								(int)(DatosFacialesPrevios.Faces[i]->x1*lfactor),(int)(DatosFacialesPrevios.Faces[i]->y1*lfactor),
								(int)(DatosFacialesPrevios.Faces[i]->x2*lfactor),(int)(DatosFacialesPrevios.Faces[i]->y2*lfactor),1);

							if (esc!=-1)//Corrige la escala más abajo
							{
								boDetectadaCara=true;
								px1=ViolaBasedOwnFaceDetector->x1[esc];
								py1=ViolaBasedOwnFaceDetector->y1[esc];
								px2=ViolaBasedOwnFaceDetector->x2[esc];
								py2=ViolaBasedOwnFaceDetector->y2[esc];
							}
						}
					}
				}
*/
				//Viola profile
				if (!boDetectadaCara && boUseAlternativeDetectors)
				{
					if (ViolaProfileDetector->boCascada && boBuscaconViolaProfile)
					{
						ViolaProfileDetector->ProcessImage(temproi,1);//El cambio de escala ya lo asume factor

						//Si hubo detección actualiza temporales
						if (ViolaProfileDetector->NumObjects)
						{
							//Escoge la de tamaño más parecido
							int esc=ViolaProfileDetector->EscogeParecida(
								(int)(DatosFacialesPrevios.Faces[i]->x1*lfactor),(int)(DatosFacialesPrevios.Faces[i]->y1*lfactor),
								(int)(DatosFacialesPrevios.Faces[i]->x2*lfactor),(int)(DatosFacialesPrevios.Faces[i]->y2*lfactor),1);

							if (esc!=-1)//Corrige la escala más abajo
							{
								boDetectadaCara=true;
								px1=ViolaProfileDetector->x1[esc];
								py1=ViolaProfileDetector->y1[esc];
								px2=ViolaProfileDetector->x2[esc];
								py2=ViolaProfileDetector->y2[esc];
							}
						}
					}
				}

				//Si hubo detección con cualquiera de las técnicas la almacena
				if (boDetectadaCara)
				{
					//Almacena corrigiendo el desplazamiento de la detección por el ROI, y la escala
					
					DeteccionesdePrevia++;
					boDetectadodePrevia[i]=true;

					//Color detection flag
					DatosFacialesPrevios.Faces[i]->boColorBasedFound=false;
					//Tracking flag
					DatosFacialesPrevios.Faces[i]->headtracked=false;

					//Indico que la detección no ha sido de HS (baja resolución)
					DatosFacialesPrevios.Faces[i]->hs=false;

					//Copia las coordenadas relativas con la escala adecuada asumiendo que tomamos
					//la primera, es decir, la mayor
					DatosFacialesPrevios.Faces[i]->x1=(esqx+px1)*lscale;
					DatosFacialesPrevios.Faces[i]->y1=(esqy+py1)*lscale;
					DatosFacialesPrevios.Faces[i]->x2=(esqx+px2)*lscale;
					DatosFacialesPrevios.Faces[i]->y2=(esqy+py2)*lscale;

					IplUtils.WindowTestP(imagegrayIN,&DatosFacialesPrevios.Faces[i]->x1,&DatosFacialesPrevios.Faces[i]->y1,
								&DatosFacialesPrevios.Faces[i]->x2,&DatosFacialesPrevios.Faces[i]->y2);

					DatosFacialesPrevios.Faces[i]->framestodie=-1;
					
				}
	
				cvReleaseImage(&temproi);
			}
			//---FACE VIOLA END---
		
			//Fin Viola Caras

			if (boDebugLog)
			{
				char cadena[256];
				sprintf(cadena,"Antes Viola HS y FB, ha obtenido %d\n",(int)boDetectadodePrevia[i]);

				DebugLog->WriteToLogFile(cadena);
			}

			//---VIOLA HS---
			//A priori no hemos encontrado empleando HS ni Fullbody
			DatosFacialesPrevios.Faces[i]->hs=false;
			DatosFacialesPrevios.Faces[i]->fb=false;
			DatosFacialesPrevios.Faces[i]->lb=false;
			
			//Si no ha detectado todavía usa el detector HS
			//La escala a usar podría depender del tamaño de la cara que buscamos
			if (!boDetectadodePrevia[i] && boBuscaconViolaHS)
			{
				int minw,minh,lscale;
				float lfactor;
				IplImage *ima4searching;

				//Fija el ROI
				if (anchopreviohs<80)//No aplicar escalado en la búsqueda para caras pequeñas
				{
					ima4searching=image;

					lfactor=1.f;
					lscale=1;

					//Tamaño mínimo a buscar
					minw=anchoprevio2hs;
					minh=altoprevio2hs;
				}
				else//Era gande, aplica la pirámide para bajar la resolución
				{
					ima4searching=image2;

					lfactor=factor;
					lscale=escala;

					//Tamaño mínimo a buscar
					minw=anchoprevio2hs/2;
					minh=altoprevio2hs/2;
				}

				//Selecciona el ROI en base al factor de escalado correspondiente
				//Establece ventana de coherencia con detección previa (ojo detección HS)
				if (VentanaCoherenciaTemporalHS(ima4searching,&esqx,&esqy,&winw,&winh,
					(int)(DatosFacialesPrevios.Faces[i]->x1*lfactor),(int)(DatosFacialesPrevios.Faces[i]->y1*lfactor),
					(int)(DatosFacialesPrevios.Faces[i]->x2*lfactor),(int)(DatosFacialesPrevios.Faces[i]->y2*lfactor)))
				{	
					//Ajusta el ROI
					cvSetImageROI(ima4searching,cvRect((int)(esqx),
						(int)(esqy),(int)(winw),(int)(winh)));
							
					IplImage *temproi=cvCreateImage( cvSize((int)(winw),(int)(winh)), 8, 3 );

					//Copia a una temporal
					cvCopyImage(ima4searching,temproi);

					//Resetea el ROI
					cvResetImageROI(ima4searching);

					//Aplica los distintos detectores de caras
					//Variables locales temporales
					bool boDetectadaCara=false;
					int px1,px2,py1,py2;

					//Aplica Viola HS propio
					if (ViolaHSDetector->boCascada)
					{
						//Process searching not much smaller than previous detection
						ViolaHSDetector->ProcessImage(temproi,1,false,cvSize(minw,minh));
//						ViolaHSDetector->ProcessImage(temproi,1);//El cambio de escala ya lo asume factor

						//Si hubo detección actualiza temporales
						if (ViolaHSDetector->NumObjects)
						{
							//Escoge la de tamaño más parecido (lógicamente teniendo en cuenta las coordenadas anteriores HS)
							int esc=ViolaHSDetector->EscogeParecida(
								(int)(DatosFacialesPrevios.Faces[i]->hsx1*lfactor),(int)(DatosFacialesPrevios.Faces[i]->hsy1*lfactor),
								(int)(DatosFacialesPrevios.Faces[i]->hsx2*lfactor),(int)(DatosFacialesPrevios.Faces[i]->hsy2*lfactor),1);

							if (esc!=-1)
							{
								boDetectadaCara=true;
								px1=ViolaHSDetector->x1[esc];
								py1=ViolaHSDetector->y1[esc];
								px2=ViolaHSDetector->x2[esc];
								py2=ViolaHSDetector->y2[esc];
							}
						}
					}

					//Viola HS Hannes
					/*if (!boDetectadaCara && boUseAlternativeDetectors)
					{
						if (ViolaUpperBodyDetector->boCascada)
						{
							ViolaUpperBodyDetector->ProcessImage(temproi,1);//El cambio de escala ya lo asume factor

							//Si hubo detección actualiza temporales
							if (ViolaUpperBodyDetector->NumObjects)
							{
								//Escoge la de tamaño más parecido (lógicamente teniendo en cuenta las coordenadas anteriores HS)
								int esc=ViolaUpperBodyDetector->EscogeParecida(
									(int)(DatosFacialesPrevios.Faces[i]->hsx1*lfactor),(int)(DatosFacialesPrevios.Faces[i]->hsy1*lfactor),
									(int)(DatosFacialesPrevios.Faces[i]->hsx2*lfactor),(int)(DatosFacialesPrevios.Faces[i]->hsy2*lfactor),1);

								if (esc!=-1)
								{
									boDetectadaCara=true;
									px1=ViolaUpperBodyDetector->x1[esc];
									py1=ViolaUpperBodyDetector->y1[esc];
									px2=ViolaUpperBodyDetector->x2[esc];
									py2=ViolaUpperBodyDetector->y2[esc];
								}
							}
						}
					}*/
					cvReleaseImage(&temproi);

					//Si hubo detección HS la almacena y corrige la escala
					if (boDetectadaCara)
					{
						DeteccionesdePrevia++;
						boDetectadodePrevia[i]=true;

						//Color detection flag
						DatosFacialesPrevios.Faces[i]->boColorBasedFound=false;
						//Tracking flag
						DatosFacialesPrevios.Faces[i]->headtracked=false;
					
						//Indico que la detección ha sido de HS
						DatosFacialesPrevios.Faces[i]->hs=true;
						
						DatosFacialesPrevios.Faces[i]->hsx1=(esqx+px1)*lscale;
						DatosFacialesPrevios.Faces[i]->hsy1=(esqy+py1)*lscale;
						DatosFacialesPrevios.Faces[i]->hsx2=(esqx+px2)*lscale;
						DatosFacialesPrevios.Faces[i]->hsy2=(esqy+py2)*lscale;

						IplUtils.WindowTestP(imagegrayIN,&DatosFacialesPrevios.Faces[i]->x1,&DatosFacialesPrevios.Faces[i]->y1,
								&DatosFacialesPrevios.Faces[i]->x2,&DatosFacialesPrevios.Faces[i]->y2);


						EstimateFacefromHS(&DatosFacialesPrevios,i);

						DatosFacialesPrevios.Faces[i]->framestodie=-1;
					}
					
				}
			}
			//---END VIOLA HS---


			

			//---VIOLA FULL BODY---
			//Si no ha detectado todavía usa el detector FullBody
			//La escala a usar podría depender del tamaño de la cara que buscamos
			if (!boDetectadodePrevia[i] && ViolaFullBodyDetector->boCascada && boBuscaconViolaFullBody)
			{
				//Establece ventana de coherencia con detección previa 
				if (VentanaCoherenciaTemporalFB(image2,&esqx,&esqy,&winw,&winh,
					(int)(DatosFacialesPrevios.Faces[i]->x1*factor),(int)(DatosFacialesPrevios.Faces[i]->y1*factor),
					(int)(DatosFacialesPrevios.Faces[i]->x2*factor),(int)(DatosFacialesPrevios.Faces[i]->y2*factor)) )
				{					
					//Ajusta el ROI
					cvSetImageROI(image2,cvRect((int)(esqx),
						(int)(esqy),(int)(winw),(int)(winh)));
							
					IplImage *temproi=cvCreateImage( cvSize((int)(winw),(int)(winh)), 8, 3 );

					//Copia a una temporal
					cvCopyImage(image2,temproi);

					//Resetea el ROI
					cvResetImageROI(image2);

					//Busca conjunto propio
					ViolaFullBodyDetector->ProcessImage(temproi,1); //Ya factor incluye la reducción de escala

					cvReleaseImage(&temproi);
											
					//Si hubo detección
					if (ViolaFullBodyDetector->NumObjects)
					{
						//Escoge la de tamaño más parecido (lógicamente teniendo en cuenta las coordenadas anteriores HS)
						int esc=ViolaFullBodyDetector->EscogeParecida(
							(int)(DatosFacialesPrevios.Faces[i]->hsx1*factor),(int)(DatosFacialesPrevios.Faces[i]->hsy1*factor),
							(int)(DatosFacialesPrevios.Faces[i]->hsx2*factor),(int)(DatosFacialesPrevios.Faces[i]->hsy2*factor),1);

						if (esc!=-1)
						{
							DeteccionesdePrevia++;
							boDetectadodePrevia[i]=true;

							//Color detection flag
							DatosFacialesPrevios.Faces[i]->boColorBasedFound=false;
							//Tracking flag
							DatosFacialesPrevios.Faces[i]->headtracked=false;
						
							//Indico que la detección ha sido de HS
							DatosFacialesPrevios.Faces[i]->fb=true;
							DatosFacialesPrevios.Faces[i]->fbx1=(esqx+ViolaFullBodyDetector->x1[esc])*escala;
							DatosFacialesPrevios.Faces[i]->fby1=(esqy+ViolaFullBodyDetector->y1[esc])*escala;
							DatosFacialesPrevios.Faces[i]->fbx2=(esqx+ViolaFullBodyDetector->x2[esc])*escala;
							DatosFacialesPrevios.Faces[i]->fby2=(esqy+ViolaFullBodyDetector->y2[esc])*escala;

							IplUtils.WindowTestP(imagegrayIN,&DatosFacialesPrevios.Faces[i]->fbx1,&DatosFacialesPrevios.Faces[i]->fby1,
									&DatosFacialesPrevios.Faces[i]->fbx2,&DatosFacialesPrevios.Faces[i]->fby2);


							EstimateFacefromFB(&DatosFacialesPrevios,i);

							DatosFacialesPrevios.Faces[i]->framestodie=-1;
						}
					}				
				}
			}
			//---END VIOLA FULLBODY---

			if (boDebugLog)
			{
				char cadena[256];
				sprintf(cadena,"Antes Histogramas, ha obtenido %d\n",(int)boDetectadodePrevia[i]);

				DebugLog->WriteToLogFile(cadena);
			}

			//---HISTOGRAM BASED (en pruebas, falta buscar el torso si es posible)
			if (!boDetectadodePrevia[i] && DatosFacialesPrevios.Faces[i]->persistence>2 
				&& DatosFacialesPrevios.Faces[i]->boTorsoHistAlreadyInitialized
				&& !boImagenGrises)
			{
				//Sets search area (debería ser distintas para el torso)
				VentanaCoherenciaTemporal(image,&esqx,&esqy,&winw,&winh,
					DatosFacialesPrevios.Faces[i]->x1,DatosFacialesPrevios.Faces[i]->y1,
					DatosFacialesPrevios.Faces[i]->x2,DatosFacialesPrevios.Faces[i]->y2);

				//Comprueba dimensiones de la ventana de búsqueda
				IplUtils.WindowTest(image,&esqx,&esqy,&winw,&winh);	//Estáusando resolución original			
				

				IplImage *h_plane,*s_plane,*v_plane;
				h_plane=cvCreateImage( cvSize(image->width,image->height), 8, 1 );
				s_plane=cvCreateImage( cvSize(image->width,image->height), 8, 1 );
				v_plane=cvCreateImage( cvSize(image->width,image->height), 8, 1 );
				

				IplImage *hsv=cvCreateImage( cvSize(image->width,image->height), 8, 3 );
				IplImage *skin=cvCreateImage( cvSize(image->width,image->height), 8, 1 );
				IplImage *torso=cvCreateImage( cvSize(image->width,image->height), 8, 1 );

				//Sets ROI
				cvSetImageROI(image,cvRect(esqx,esqy,winw,winh));
				cvSetImageROI(skin,cvRect(esqx,esqy,winw,winh));
				cvSetImageROI(hsv,cvRect(esqx,esqy,winw,winh));
				cvSetImageROI(h_plane,cvRect(esqx,esqy,winw,winh));
				cvSetImageROI(s_plane,cvRect(esqx,esqy,winw,winh));
				cvSetImageROI(v_plane,cvRect(esqx,esqy,winw,winh));
				IplImage *planes[]={h_plane,s_plane};
				cvSetZero(skin);
				cvCvtColor(image,hsv,CV_BGR2HSV);

				cvCvtPixToPlane(hsv,h_plane,s_plane,v_plane,0);

				cvCalcBackProject(planes,skin,DatosFacialesPrevios.Faces[i]->SkinHist);

				cvThreshold(skin,skin,40,255,CV_THRESH_BINARY);

				CvMemStorage* storage = cvCreateMemStorage(0);  
				CvSeq *contMayor;
				float farea;
				int r=IplUtils.FindLargestComponent(skin,storage, &contMayor,&farea);
				//Podríamos probara  buscar varios blobs solapando con la det. previa ...

				if (r)  
				{
					if (contMayor!=NULL)
					{
						CvRect re = cvContourBoundingRect(contMayor);

						cvDrawContours(skin,contMayor,CV_RGB(128,128,128),CV_RGB(128,128,128),-1,CV_FILLED);

						//heurística para impedir que el límite inferior se baje demasiado
						int altoblob=re.height;

						//If it is higher checks previous ratio
						if (altoblob>altoprevio)
						{
							if (DatosFacialesPrevios.Faces[i]->boColorBasedFound)
							{
								//If height has become much bigger and previous detection was by color
								if ((float)altoblob/(float)re.width>(float)altoprevio/(float)anchoprevio)
									altoblob=(int)((float)(altoprevio*re.width)/(float)anchoprevio);
							}
							else
							{	//Nor previously by color
								if ((float)altoblob/(float)re.width>1.4*(float)altoprevio/(float)anchoprevio)
									altoblob=(int)(1.4*(float)(altoprevio*re.width)/(float)anchoprevio);
							}
						}

						
						int iAreaActual=re.width*altoblob;

						float ratioareas;
						if (AreaPrevia>iAreaActual)
							ratioareas=(float)iAreaActual/(float)AreaPrevia;
						else
							ratioareas=(float)AreaPrevia/(float)iAreaActual;

						int cx_previo=(DatosFacialesPrevios.Faces[i]->x2+DatosFacialesPrevios.Faces[i]->x1)/2,
							cy_previo=(DatosFacialesPrevios.Faces[i]->y2+DatosFacialesPrevios.Faces[i]->y1)/2,
							cx_actual=re.x+re.width/2,
							cy_actual=re.y+altoblob/2;
				
						//If sizes are similar, they are not too distant
						//and the width is not too large in comparison to height
						if (ratioareas>0.8 && 
							MathUtils.DistanciaEuclidea(cx_previo,cy_previo,cx_actual,cy_actual)<
							(anchoprevio)*0.4)
						{
							DeteccionesdePrevia++;
							boDetectadodePrevia[i]=true;

							//Tracking flag
							DatosFacialesPrevios.Faces[i]->headtracked=false;
						
							//Copia la nueva detección donde antes de form temporal
							DatosFacialesPrevios.Faces[i]->x1=re.x;
							DatosFacialesPrevios.Faces[i]->y1=re.y;
							DatosFacialesPrevios.Faces[i]->x2=re.x+re.width;
							DatosFacialesPrevios.Faces[i]->y2=re.y+altoblob;

							IplUtils.WindowTestP(imagegrayIN,&DatosFacialesPrevios.Faces[i]->x1,&DatosFacialesPrevios.Faces[i]->y1,
									&DatosFacialesPrevios.Faces[i]->x2,&DatosFacialesPrevios.Faces[i]->y2);

							DatosFacialesPrevios.Faces[i]->boColorBasedFound=true;

							DatosFacialesPrevios.Faces[i]->framestodie=-1;

						}

					}
					cvReleaseMemStorage( &storage );
				}

				cvResetImageROI(image);
				cvResetImageROI(skin);//¿necesario?
				cvResetImageROI(hsv);
				cvResetImageROI(h_plane);
				cvResetImageROI(s_plane);
				cvResetImageROI(v_plane);


				//Torso
				if (DatosFacialesPrevios.Faces[i]->TorsoHist)
				{
					//Sets search area (debería ser distintas para el torso)
					VentanaCoherenciaTemporalTorso(image,&esqx,&esqy,&winw,&winh,
						DatosFacialesPrevios.Faces[i]->x1,DatosFacialesPrevios.Faces[i]->y1,
						DatosFacialesPrevios.Faces[i]->x2,DatosFacialesPrevios.Faces[i]->y2);

					//Comprueba dimensiones de la ventana de búsqueda
					IplUtils.WindowTest(image,&esqx,&esqy,&winw,&winh);

					if (winh>DatosFacialesPrevios.Faces[i]->y2-DatosFacialesPrevios.Faces[i]->y1)
					{

						//Sets ROI
						cvSetImageROI(image,cvRect(esqx,esqy,winw,winh));
						cvSetImageROI(torso,cvRect(esqx,esqy,winw,winh));
						cvSetImageROI(hsv,cvRect(esqx,esqy,winw,winh));
						cvSetImageROI(h_plane,cvRect(esqx,esqy,winw,winh));
						cvSetImageROI(s_plane,cvRect(esqx,esqy,winw,winh));
						cvSetImageROI(v_plane,cvRect(esqx,esqy,winw,winh));
						IplImage *planes[]={h_plane,s_plane};
						cvSetZero(torso);
						cvCvtColor(image,hsv,CV_BGR2HSV);

						cvCvtPixToPlane(hsv,h_plane,s_plane,v_plane,0);

						cvCalcBackProject(planes,torso,DatosFacialesPrevios.Faces[i]->TorsoHist);

						cvThreshold(torso,torso,40,255,CV_THRESH_BINARY);

						storage = cvCreateMemStorage(0);  
						r=IplUtils.FindLargestComponent(torso,storage, &contMayor,&farea);
						//Podríamos probara  buscar varios blobs solapando con la det. previa ...
						if (r)  
						{
							if (contMayor!=NULL)
							{
//								CvRect re = cvContourBoundingRect(contMayor);

								cvDrawContours(torso,contMayor,CV_RGB(128,128,128),CV_RGB(128,128,128),-1,CV_FILLED);

#ifndef ENCARA2INLINUX
								//cvNamedWindow("Torso", 1);
								//cvShowImage("Torso",torso);
#endif

							}
							cvReleaseMemStorage( &storage );
						}

						cvResetImageROI(image);
						cvResetImageROI(torso);//¿necesario?
						cvResetImageROI(hsv);
						cvResetImageROI(h_plane);
						cvResetImageROI(s_plane);
						cvResetImageROI(v_plane);

					}//large height
				}//TorsoHist available

				cvReleaseImage(&skin);
				cvReleaseImage(&torso);
				cvReleaseImage(&hsv);
				cvReleaseImage(&h_plane);
				cvReleaseImage(&s_plane);
				cvReleaseImage(&v_plane);
			}

			//---END HISTOGRAM BASED

			if (boDebugLog)
			{
				char cadena[256];
				sprintf(cadena,"Antes Color, ha obtenido %d\n",(int)boDetectadodePrevia[i]);

				DebugLog->WriteToLogFile(cadena);
			}

			//---COLOR BASED---
			//Searches based on color only if previous detection was not HS or FB and it is not a gray image
			//and eyes have been detceted at least once for this detection thread
			if (!boDetectadodePrevia[i] && DatosFacialesPrevios.Faces[i]->persistence>2 
				&& DatosFacialesPrevios.Faces[i]->boEyesDetectedAtLeastOnceforThisFace
				&& !(DatosFacialesPrevios.Faces[i]->hs || DatosFacialesPrevios.Faces[i]->fb || DatosFacialesPrevios.Faces[i]->lb)
				&& !boImagenGrises)
			{
				//Sets search area
				VentanaCoherenciaTemporal(image,&esqx,&esqy,&winw,&winh,
					DatosFacialesPrevios.Faces[i]->x1,DatosFacialesPrevios.Faces[i]->y1,
					DatosFacialesPrevios.Faces[i]->x2,DatosFacialesPrevios.Faces[i]->y2);

				//Comprueba dimensiones de la ventana de búsqueda
				IplUtils.WindowTest(image,&esqx,&esqy,&winw,&winh);//Usa resolución original
									
				//Resetea rotaciones
				Blob->nRot=0;

				//Detecta ahora con el valor promedio en la ventana de detección correspondiente a esa cara 
				Blob->DetectaPixelsRangoColorenVentana(image,&(Blob->blobs),&(Blob->blobs_noDil),
					&(imageINr_norm),&(imageINg_norm),
					DatosFacialesPrevios.Faces[i]->rn_min,DatosFacialesPrevios.Faces[i]->rn_max,
					DatosFacialesPrevios.Faces[i]->gn_min,DatosFacialesPrevios.Faces[i]->gn_max,
					esqx,esqy,winw,winh);

                //Dilatamos
				cvDilate(Blob->blobs_noDil,Blob->blobs,NULL,1);

				//Sólo va a tratar un blob en esa zona, el mayor
				Blob->iBlobActual=0;			//Current blob being processed
				//Busca la componente mayor

				CvMemStorage* storageblob = cvCreateMemStorage(0);  
														
				//Para el blob mayor, estima la elipse, lo rota y localiza los elementos faciales
				int r=IplUtils.FindNLargestComponents(Blob->blobs,storageblob, &(Blob->contour),&(Blob->area),1);

				if (r)  //Analiza la mayor
				{
					//Busca el blob candidato de la cara eliminando el cuello
					Blob->DetectaBlobCaraSinCuello(image,imagerot,Blob->blob,Blob->blobrot,Blob->blobs);

					Blob->rect[Blob->iBlobActual] = cvContourBoundingRect(Blob->contour[Blob->iBlobActual]);

					//heurística para impedir que el límite inferior se baje demasiado
					int altoblob=Blob->rect[Blob->iBlobActual].height;

					//If it is higher checks previous ratio
					if (altoblob>altoprevio)
					{
						if (DatosFacialesPrevios.Faces[i]->boColorBasedFound)
						{
							//If height has become much bigger and previous detection was by color
							if ((float)altoblob/(float)Blob->rect[Blob->iBlobActual].width>(float)altoprevio/(float)anchoprevio)
								altoblob=(int)((float)(altoprevio*Blob->rect[Blob->iBlobActual].width)/(float)anchoprevio);
						}
						else
						{	//Nor previously by color
							if ((float)altoblob/(float)Blob->rect[Blob->iBlobActual].width>1.4*(float)altoprevio/(float)anchoprevio)
								altoblob=(int)(1.4*(float)(altoprevio*Blob->rect[Blob->iBlobActual].width)/(float)anchoprevio);
						}
					}


					int iAreaActual=Blob->rect[Blob->iBlobActual].width*altoblob;

					float ratioareas;
					if (AreaPrevia>iAreaActual)
						ratioareas=(float)iAreaActual/(float)AreaPrevia;
					else
						ratioareas=(float)AreaPrevia/(float)iAreaActual;

					int cx_previo=(DatosFacialesPrevios.Faces[i]->x2+DatosFacialesPrevios.Faces[i]->x1)/2,
						cy_previo=(DatosFacialesPrevios.Faces[i]->y2+DatosFacialesPrevios.Faces[i]->y1)/2,
						cx_actual=Blob->rect[Blob->iBlobActual].x+Blob->rect[Blob->iBlobActual].width/2,
						cy_actual=Blob->rect[Blob->iBlobActual].y+altoblob/2;

			
					//If sizes are similar, they are not too distant
					//and the width is not too large in comparison to height
					if (ratioareas>0.8 && 
						MathUtils.DistanciaEuclidea(cx_previo,cy_previo,cx_actual,cy_actual)<
						(anchoprevio)*0.4	&& (Blob->rect[Blob->iBlobActual].width)<1.25*(altoblob) )
					{
						DeteccionesdePrevia++;
						boDetectadodePrevia[i]=true;

						//Tracking flag
						DatosFacialesPrevios.Faces[i]->headtracked=false;
					
						//Copia la nueva detección donde antes de form temporal
						DatosFacialesPrevios.Faces[i]->x1=Blob->rect[Blob->iBlobActual].x;
						DatosFacialesPrevios.Faces[i]->y1=Blob->rect[Blob->iBlobActual].y;
						DatosFacialesPrevios.Faces[i]->x2=Blob->rect[Blob->iBlobActual].x+Blob->rect[Blob->iBlobActual].width;
						DatosFacialesPrevios.Faces[i]->y2=Blob->rect[Blob->iBlobActual].y+altoblob;

						IplUtils.WindowTestP(imagegrayIN,&DatosFacialesPrevios.Faces[i]->x1,&DatosFacialesPrevios.Faces[i]->y1,
								&DatosFacialesPrevios.Faces[i]->x2,&DatosFacialesPrevios.Faces[i]->y2);

						DatosFacialesPrevios.Faces[i]->boColorBasedFound=true;

						DatosFacialesPrevios.Faces[i]->framestodie=-1;

					}
					cvReleaseMemStorage( &storageblob );

					//Libera información de contornos ya que sólo se aloja si hay r	
					delete [] Blob->contour;
					delete [] Blob->area;
				}//if (r)
				
			}
			//---END SKIN BASED---

			
			if (boDebugLog)
			{
				char cadena[256];
				sprintf(cadena,"Antes head tracking, ha obtenido %d\n",(int)boDetectadodePrevia[i]);

				DebugLog->WriteToLogFile(cadena);
			}
				
			//---HEAD TRACKING---
			//Busca por tracking del patrón siempre fijo pero que almacena último tamaño
			//Pero sólo si tenemos el patrón y el sujeto ha sido algo "persistente"
			if (!boDetectadodePrevia[i] && DatosFacialesPrevios.Faces[i]->boHeadTracker &&
				!(DatosFacialesPrevios.Faces[i]->headtracked && DatosFacialesPrevios.Faces[i]->iconttracked>5) )
				//&& (DatosFacialesPrevios.Faces[i]->persistence>2 || DatosFacialesPrevios.Faces[i]->boEyesDetectedAtLeastOnceforThisFace))
			{
				//Reduce la dimensión según la escala
				IplImage *temp2,*temp3;
				int escalaaux=1;

				temp3=cvCloneImage(image);//Usa resolución original para empezar

				//Ajusta la escala entera a múltiplo de 2
				while (escalaaux*2<DatosFacialesPrevios.Faces[i]->escalaCaraf)
				{
					temp2 = cvCreateImage( cvSize(temp3->width/2,temp3->height/2), 8, 3 );
						//Reduce la dimensión de la imagen a tratar
					cvPyrDown( temp3, temp2 );
					cvReleaseImage(&temp3);

					escalaaux*=2;

					temp3=cvCloneImage(temp2);
					cvReleaseImage(&temp2);
				}

				//Busca el patrón
                DatosFacialesPrevios.Faces[i]->HeadTracker->Track(temp3);

				cvReleaseImage(&temp3);

				//Si parece haberlo encontrado
				if (!DatosFacialesPrevios.Faces[i]->HeadTracker->boLost)
				{
					//Recupera la posición
					DeteccionesdePrevia++;
					boDetectadodePrevia[i]=true;

					//Color detection flag
					DatosFacialesPrevios.Faces[i]->boColorBasedFound=false;

					int anchoprevio=(int)((float)(DatosFacialesPrevios.Faces[i]->x2-DatosFacialesPrevios.Faces[i]->x1)/(float)escalaaux);
					int anchonuevo2=(int)((float)anchoprevio*0.4+PATSIZEX*0.2);
					int altoprevio=(int)((float)(DatosFacialesPrevios.Faces[i]->y2-DatosFacialesPrevios.Faces[i]->y1)/(float)escalaaux);
					int altonuevo2=(int)((float)altoprevio*0.4+PATSIZEY*0.2);

					//Copia la nueva detección donde antes de form temporal
					DatosFacialesPrevios.Faces[i]->x1=(DatosFacialesPrevios.Faces[i]->HeadTracker->x-anchonuevo2)*escalaaux;
					DatosFacialesPrevios.Faces[i]->y1=(DatosFacialesPrevios.Faces[i]->HeadTracker->y-altonuevo2)*escalaaux;
					DatosFacialesPrevios.Faces[i]->x2=(DatosFacialesPrevios.Faces[i]->HeadTracker->x+anchonuevo2)*escalaaux;
					DatosFacialesPrevios.Faces[i]->y2=(DatosFacialesPrevios.Faces[i]->HeadTracker->y+altonuevo2)*escalaaux;

					IplUtils.WindowTestP(imagegrayIN,&DatosFacialesPrevios.Faces[i]->x1,&DatosFacialesPrevios.Faces[i]->y1,
								&DatosFacialesPrevios.Faces[i]->x2,&DatosFacialesPrevios.Faces[i]->y2);

					//Counts consecutive tracks (and limits it) Not so trustful
					if (DatosFacialesPrevios.Faces[i]->headtracked)
						DatosFacialesPrevios.Faces[i]->iconttracked++;
					else
						DatosFacialesPrevios.Faces[i]->iconttracked=1;

					DatosFacialesPrevios.Faces[i]->headtracked=true;

					DatosFacialesPrevios.Faces[i]->framestodie=-1;
				
				}
			}
			//---END HEAD TRACKING


			if (boDebugLog)
			{
				char cadena[256];
				sprintf(cadena,"Tras head tracking\n");

				DebugLog->WriteToLogFile(cadena);
			}

			//---LK Tracking---
			if(!boDetectadodePrevia[i] && DatosFacialesPrevios.Faces[i]->boLKTracker
				&& DatosFacialesPrevios.Faces[i]->cornerCount>8 && 0)
			{
				int win_size = 10;//Tamaño de la ventana 
				char status[30];//Nos dirá si el punto fue encontrado
				int flags=0;//Puede utilizarse para optimizar un poco en el cálculo de pirámides

				//Ajusto la ventana según el tipo de detección
				if (!DatosFacialesPrevios.Faces[i]->hs && !DatosFacialesPrevios.Faces[i]->fb && !DatosFacialesPrevios.Faces[i]->lb)
					VentanaCoherenciaTemporal(image,&esqx,&esqy,&winw,&winh,
						DatosFacialesPrevios.Faces[i]->x1,DatosFacialesPrevios.Faces[i]->y1,
						DatosFacialesPrevios.Faces[i]->x2,DatosFacialesPrevios.Faces[i]->y2);
				else
				{
					if (DatosFacialesPrevios.Faces[i]->hs)
						VentanaCoherenciaTemporalHS(image,&esqx,&esqy,&winw,&winh,
							DatosFacialesPrevios.Faces[i]->x1,DatosFacialesPrevios.Faces[i]->y1,
							DatosFacialesPrevios.Faces[i]->x2,DatosFacialesPrevios.Faces[i]->y2);
					else
						VentanaCoherenciaTemporalFB(image,&esqx,&esqy,&winw,&winh,
							DatosFacialesPrevios.Faces[i]->x1,DatosFacialesPrevios.Faces[i]->y1,
							DatosFacialesPrevios.Faces[i]->x2,DatosFacialesPrevios.Faces[i]->y2);

				}

				//Falta asignar el ROI claro
				esqx=esqy=0;
				//cvSetImageROI(imagegrayINprev,cvRect(esqx,esqy,winw,winh));
				//cvSetImageROI(imagegrayIN,cvRect(esqx,esqy,winw,winh));
				//cvSetImageROI(prev_pyramid,cvRect(esqx,esqy,winw,winh));
				//cvSetImageROI(pyramid,cvRect(esqx,esqy,winw,winh));

				//Desplazamos esqeuinas previas
				for (int icorner=0;icorner<DatosFacialesPrevios.Faces[i]->cornerCount;icorner++)
				{
					DatosFacialesPrevios.Faces[i]->corners[icorner].x+=(DatosFacialesPrevios.Faces[i]->cornerx-esqx);
					DatosFacialesPrevios.Faces[i]->corners[icorner].y+=(DatosFacialesPrevios.Faces[i]->cornery-esqy);
				}

				cvCalcOpticalFlowPyrLK( imagegrayINprev, imagegrayIN, prev_pyramid, pyramid,
					DatosFacialesPrevios.Faces[i]->corners, 
					DatosFacialesPrevios.Faces[i]->corners_new, 
					DatosFacialesPrevios.Faces[i]->cornerCount, 
					cvSize(win_size,win_size), 3, status, 0,
					cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03), flags );

				DatosFacialesPrevios.Faces[i]->cornerx=esqx;
				DatosFacialesPrevios.Faces[i]->cornery=esqy;

				cvResetImageROI(imagegrayIN);
				cvResetImageROI(imagegrayINprev);
				cvResetImageROI(prev_pyramid);
				cvResetImageROI(pyramid);

				//Contabilizamos puntos no perdidos
				int npointstracked=0;
				int offx=0,offy=0,
					minx=10000,maxx=0,miny=10000,maxy=0,pminx=10000,pmaxx=0,pminy=10000,pmaxy=0;

				//GoodFeaturestoTrack update
				for (int np=0;np<DatosFacialesPrevios.Faces[i]->cornerCount;np++)
				{
					if (status[np])
					{
						//Contabiliza cambio de posición, minimos y máximos para la escala
						//de los elementos seguidos
						offx+=(int)(DatosFacialesPrevios.Faces[i]->corners_new[np].x-
							DatosFacialesPrevios.Faces[i]->corners[np].x);
						offy+=(int)(DatosFacialesPrevios.Faces[i]->corners_new[np].y-
							DatosFacialesPrevios.Faces[i]->corners[np].y);

						//Mínimos y máximos para la nueva
						if (DatosFacialesPrevios.Faces[i]->corners_new[np].x<minx)
							minx=(int)DatosFacialesPrevios.Faces[i]->corners_new[np].x;
						if (DatosFacialesPrevios.Faces[i]->corners_new[np].x>maxx)
							maxx=(int)DatosFacialesPrevios.Faces[i]->corners_new[np].x;
						if (DatosFacialesPrevios.Faces[i]->corners_new[np].y<miny)
							miny=(int)DatosFacialesPrevios.Faces[i]->corners_new[np].y;
						if (DatosFacialesPrevios.Faces[i]->corners_new[np].y>maxy)
							maxy=(int)DatosFacialesPrevios.Faces[i]->corners_new[np].y;

						//Mínimos y máximos para la precedente (considerando sólo los puntos seguidos)
						if (DatosFacialesPrevios.Faces[i]->corners[np].x<pminx)
							pminx=(int)DatosFacialesPrevios.Faces[i]->corners[np].x;
						if (DatosFacialesPrevios.Faces[i]->corners[np].x>pmaxx)
							pmaxx=(int)DatosFacialesPrevios.Faces[i]->corners[np].x;
						if (DatosFacialesPrevios.Faces[i]->corners[np].y<pminy)
							pminy=(int)DatosFacialesPrevios.Faces[i]->corners[np].y;
						if (DatosFacialesPrevios.Faces[i]->corners[np].y>pmaxy)
							pmaxy=(int)DatosFacialesPrevios.Faces[i]->corners[np].y;

						//Actualiza
						DatosFacialesPrevios.Faces[i]->corners[npointstracked].x=
							DatosFacialesPrevios.Faces[i]->corners_new[np].x;
						DatosFacialesPrevios.Faces[i]->corners[npointstracked].y=
							DatosFacialesPrevios.Faces[i]->corners_new[np].y;

						npointstracked++;

					}
				}

				//Decidimos si ha funcionado o no
				if (npointstracked>(int)(DatosFacialesPrevios.Faces[i]->cornerCount*0.6) && npointstracked>8)
				{
					//Recupera la posición
					DeteccionesdePrevia++;
					boDetectadodePrevia[i]=true;

					//Color detection flag
					DatosFacialesPrevios.Faces[i]->boColorBasedFound=false;

					float ratio,ratiox=(float)(maxx-minx)/(float)(pmaxx-pminx),
						ratioy=(float)(maxy-miny)/(float)(pmaxy-pminy);

					//Toma el más cercano a 1
					if (fabs((float)(ratiox-1.0))<fabs((float)(ratioy-1.0)))
						ratio=ratiox;
					else
						ratio=ratioy;

					int anchonuevo=(int)((float)(DatosFacialesPrevios.Faces[i]->x2-
						DatosFacialesPrevios.Faces[i]->x1)*ratio);
					int altonuevo=(int)((float)(DatosFacialesPrevios.Faces[i]->y2-
						DatosFacialesPrevios.Faces[i]->y1)*ratio);

					DatosFacialesPrevios.Faces[i]->cornerCount=npointstracked;

					//Copia la nueva detección donde antes de form temporal
					DatosFacialesPrevios.Faces[i]->x1=(DatosFacialesPrevios.Faces[i]->x2+DatosFacialesPrevios.Faces[i]->x1)/2-anchonuevo/2;
					DatosFacialesPrevios.Faces[i]->y1=(DatosFacialesPrevios.Faces[i]->y2+DatosFacialesPrevios.Faces[i]->y1)/2-altonuevo/2;
					DatosFacialesPrevios.Faces[i]->x2=(DatosFacialesPrevios.Faces[i]->x2+DatosFacialesPrevios.Faces[i]->x1)/2+anchonuevo/2;
					DatosFacialesPrevios.Faces[i]->y2=(DatosFacialesPrevios.Faces[i]->y2+DatosFacialesPrevios.Faces[i]->y1)/2+altonuevo/2;

					IplUtils.WindowTestP(imagegrayIN,&DatosFacialesPrevios.Faces[i]->x1,&DatosFacialesPrevios.Faces[i]->y1,
								&DatosFacialesPrevios.Faces[i]->x2,&DatosFacialesPrevios.Faces[i]->y2);



					DatosFacialesPrevios.Faces[i]->headtracked=true;

					DatosFacialesPrevios.Faces[i]->framestodie=-1;
				}
			}
			//---END HEAD TRACKING---

		}//for previously detected faces


		if (boDebugLog)
		{
			char cadena[256];
			sprintf(cadena,"\nBefore Search for new faces\n");

			DebugLog->WriteToLogFile(cadena);
		}

		//---SEARCH FOR NEW FACES---
		//Normalmente si hubo detecciones previas busca por coherencia, pero cada cierto
		//tiempo busca en toda la imagen para detectar nuevas "entradas" (sólo si alguna 
		//se detectó por coherencia)
		NumFacesAux=NumFacesAuxNuevas=0;
		if (DeteccionesdePrevia || !(boSpeed))//Si hay previas debe chequear siempre aunque no las haya localizado
		{
			//Cada cuatro frames o si estamos en el modo de no velocidad (busca siempre)
			if ( (DeteccionesdePrevia && ((nConsecutivasDetectandoporCoherenciaTemporal+9)%10)==0) || 
				(!DeteccionesdePrevia && ((nConsecutivasDetectandoporCoherenciaTemporal+4)%5)==0) || !(boSpeed))
			{

				if (boDebugLog)
				{
					char cadena[256];
					sprintf(cadena,"\nSearch for new faces\n");

					DebugLog->WriteToLogFile(cadena);
				}

				//A seacrh in the whole image is performed, indicates that to avoid a new search
				boAlreadySearchedintheWholeImage=true;

				//Escoge una de las técnicas aleatoriamente
				int tecnica=(int)(((double)rand()/(double)RAND_MAX)*4.0);
				
				switch(tecnica)
				{
				case 2:
				case 0://Usa Viola Caras por defecto
					if (ViolaFaceDetector->boCascada && boBuscaconViolaCaras)
						{
							ViolaFaceDetector->ProcessImage(image2,1);//Uso la imagen escalada y evito una nueva pirámide

							//Si ha detectado con Viola
							if (ViolaFaceDetector->NumObjects)
							{	
								NumFacesAux=ViolaFaceDetector->NumObjects;

								DatosFacialesAux.Init(NumFacesAux);

															
								//Las inicializa como nuevas
								for (int ii=0;ii<NumFacesAux;ii++)
								{												
									DatosFacialesAux.Faces[ii]->hs=false;	//No fue detección HS
									DatosFacialesAux.Faces[ii]->fb=false;	//No fue detección FullBody
									DatosFacialesAux.Faces[ii]->lb=false;	//No fue detección LowerBody

									//Copia los contenedores detectados
									DatosFacialesAux.Faces[ii]->x1=ViolaFaceDetector->x1[ii]*escala;
									DatosFacialesAux.Faces[ii]->y1=ViolaFaceDetector->y1[ii]*escala;
									DatosFacialesAux.Faces[ii]->x2=ViolaFaceDetector->x2[ii]*escala;
									DatosFacialesAux.Faces[ii]->y2=ViolaFaceDetector->y2[ii]*escala;

									IplUtils.WindowTestP(imagegrayIN,&DatosFacialesAux.Faces[ii]->x1,&DatosFacialesAux.Faces[ii]->y1,
									&DatosFacialesAux.Faces[ii]->x2,&DatosFacialesAux.Faces[ii]->y2);
								
									DatosFacialesAux.Faces[ii]->framestodie=-1;
								}
							}//Detectó algo
							else NumFacesAux=0;
						}//Hay detector cascada
						else NumFacesAux=0; //No había detector no pudo buscar
					break;
				case 1:
					if (ViolaHSDetector->boCascada && boBuscaconViolaHS)
					{
						ViolaHSDetector->ProcessImage(image2,1);//Uso la imagen escalada, evito de nuevo hacer una pirámide

						//Si ha detectado con Viola
						if (ViolaHSDetector->NumObjects)
						{									
							NumFacesAux=ViolaHSDetector->NumObjects;

							DatosFacialesAux.Init(NumFacesAux);
							
							//Las inicializa como nuevas
							for (int ii=0;ii<NumFacesAux;ii++)
							{												
								DatosFacialesAux.Faces[ii]->hs=true;	//No fue detección HS
								DatosFacialesAux.Faces[ii]->fb=false;	//No fue detección FullBody
								DatosFacialesAux.Faces[ii]->lb=false;	//No fue detección LowerBody
								//Copia los contenedores detectados
								DatosFacialesAux.Faces[ii]->hsx1=ViolaHSDetector->x1[ii]*escala;
								DatosFacialesAux.Faces[ii]->hsy1=ViolaHSDetector->y1[ii]*escala;
								DatosFacialesAux.Faces[ii]->hsx2=ViolaHSDetector->x2[ii]*escala;
								DatosFacialesAux.Faces[ii]->hsy2=ViolaHSDetector->y2[ii]*escala;

								IplUtils.WindowTestP(imagegrayIN,&DatosFacialesAux.Faces[ii]->hsx1,&DatosFacialesAux.Faces[ii]->hsy1,
								&DatosFacialesAux.Faces[ii]->hsx2,&DatosFacialesAux.Faces[ii]->hsy2);

								EstimateFacefromHS(&DatosFacialesAux,ii);

								DatosFacialesAux.Faces[ii]->framestodie=-1;																
							}
						}//Detectó algo
						else NumFacesAux=0;
					}//Hay detector cascada
					else NumFacesAux=0; //No había detector no pudo buscar

					//OpenCV HS detector (Hannes)
					/*if (ViolaUpperBodyDetector->boCascada && boBuscaconViolaHS && boUseAlternativeDetectors)
						{
							ViolaUpperBodyDetector->ProcessImage(image2,1);//Ya disponemos de la imagen escalada, evito nueva pirámide

							//Si ha detectado con Viola
							if (ViolaUpperBodyDetector->NumObjects)
							{									
								NumFacesAux=ViolaUpperBodyDetector->NumObjects;

								DatosFacialesAux.Init(NumFacesAux);
								
								//Las inicializa como nuevas
								for (int ii=0;ii<NumFacesAux;ii++)
								{												
									DatosFacialesAux.Faces[ii]->hs=true;	//No fue detección HS
									DatosFacialesAux.Faces[ii]->fb=false;	//No fue detección FullBody
									DatosFacialesAux.Faces[ii]->lb=false;	//No fue detección LowerBody
									//Copia los contenedores detectados
									DatosFacialesAux.Faces[ii]->hsx1=ViolaUpperBodyDetector->x1[ii]*escala;
									DatosFacialesAux.Faces[ii]->hsy1=ViolaUpperBodyDetector->y1[ii]*escala;
									DatosFacialesAux.Faces[ii]->hsx2=ViolaUpperBodyDetector->x2[ii]*escala;
									DatosFacialesAux.Faces[ii]->hsy2=ViolaUpperBodyDetector->y2[ii]*escala;

									IplUtils.WindowTestP(imagegrayIN,&DatosFacialesAux.Faces[ii]->hsx1,&DatosFacialesAux.Faces[ii]->hsy1,
									&DatosFacialesAux.Faces[ii]->hsx2,&DatosFacialesAux.Faces[ii]->hsy2);


									EstimateFacefromHS(&DatosFacialesAux,ii);

									DatosFacialesAux.Faces[ii]->framestodie=-1;
									
								}
							}//Detectó algo
							else NumFacesAux=0;
						}//Hay detector cascada
						else NumFacesAux=0; //No había detector no pudo buscar
*/
					break;
				case 3:
					if (ViolaProfileDetector->boCascada && boBuscaconViolaCaras)
						{
							ViolaProfileDetector->ProcessImage(image2,1);//Uso la imagen escalada y evito una nueva pirámide

							//Si ha detectado con Viola
							if (ViolaProfileDetector->NumObjects)
							{	
								NumFacesAux=ViolaProfileDetector->NumObjects;

								DatosFacialesAux.Init(NumFacesAux);
								
								//Las inicializa como nuevas
								for (int ii=0;ii<NumFacesAux;ii++)
								{												
									DatosFacialesAux.Faces[ii]->hs=false;	//No fue detección HS
									DatosFacialesAux.Faces[ii]->fb=false;	//No fue detección FullBody
									DatosFacialesAux.Faces[ii]->lb=false;	//No fue detección LowerBody
									//Copia los contenedores detectados
									DatosFacialesAux.Faces[ii]->x1=ViolaProfileDetector->x1[ii]*escala;
									DatosFacialesAux.Faces[ii]->y1=ViolaProfileDetector->y1[ii]*escala;
									DatosFacialesAux.Faces[ii]->x2=ViolaProfileDetector->x2[ii]*escala;
									DatosFacialesAux.Faces[ii]->y2=ViolaProfileDetector->y2[ii]*escala;

									IplUtils.WindowTestP(imagegrayIN,&DatosFacialesAux.Faces[ii]->x1,&DatosFacialesAux.Faces[ii]->y1,
									&DatosFacialesAux.Faces[ii]->x2,&DatosFacialesAux.Faces[ii]->y2);
								
									DatosFacialesAux.Faces[ii]->framestodie=-1;
								}
							}//Detectó algo
							else NumFacesAux=0;
						}//Hay detector cascada
						else NumFacesAux=0; //No había detector no pudo buscar
					
					break;
				default:
					NumFacesAux=0; //No buscó
				}

				//Deletes redundant detections (only useful if there were new detections, i.e. NumFaceAux!=0)
				if (NumFacesAux && DeteccionesdePrevia)
				{
					//Si ha detectado algo las compara con las ya existentes para ver si puede añadir
					NumFacesAuxNuevas=NumFacesAux;
					boNueva=(bool *)new unsigned char[NumFacesAux*sizeof(bool)];

					//Chequea si estas detecciones, las nuevas, no han sido ya contempladas por coherencia
					for (int ii=0;ii<NumFacesAux;ii++)
					{
						boNueva[ii]=true;//A priori new detections are accepted

						if (!DatosFacialesAux.Faces[ii]->fb)
						{
							int Ax1,Ax2,Ay1,Ay2;
							int Bx1,Bx2,By1,By2;
							long area1,area2,areainter;
							
							//Calculates new detection area
							Ax1=DatosFacialesAux.Faces[ii]->x1;
							Ax2=DatosFacialesAux.Faces[ii]->x2;
							Ay1=DatosFacialesAux.Faces[ii]->y1;
							Ay2=DatosFacialesAux.Faces[ii]->y2;
							
							area1=(Ax2-Ax1)*(Ay2-Ay1);

							//Compares with those already stored in DatosFacialesPrevios, i. e., those detected by
							//temporal coherence
							for (int iii=0;iii<DatosFacialesPrevios.NumFaces;iii++)
							{	
								//Compara las detecciones de las previas con la nueva
								//Si la previa no se detectó, o fue tracking, se actualiza
								if (boNueva[ii] && !DatosFacialesPrevios.Faces[iii]->fb)
								{		
									//Calculates are of the currently selected temporl coherent detected face
									Bx1=DatosFacialesPrevios.Faces[iii]->x1;
									Bx2=DatosFacialesPrevios.Faces[iii]->x2;
									By1=DatosFacialesPrevios.Faces[iii]->y1;
									By2=DatosFacialesPrevios.Faces[iii]->y2;
									
									area2=(Bx2-Bx1)*(By2-By1);

									//Do the windows intersect?
									MathUtils.WindowsOverlap(Ax1,Ay1,Ax2,Ay2,Bx1,By1,Bx2,By2,&areainter);

									float distcenters=MathUtils.DistanciaEuclidea((Ax1+Ax2)/2,(Ay1+Ay2)/2,(Bx1+Bx2)/2,(By1+By2)/2);

									//Checks if one is contained or if intersection area is too big to consider them independent detections
									if ( (MathUtils.VentanaEnVentana(Ax1,Ay1,Ax2,Ay2,Bx1,By1,Bx2,By2) && (float)areainter/(float)area2>0.6) ||
											(MathUtils.VentanaEnVentana(Bx1,By1,Bx2,By2,Ax1,Ay1,Ax2,Ay2) && (float)areainter/(float)area1>0.6) ||
											((float)areainter/(float)area1>0.6 &&  distcenters < (Ax2-Ax1)/2 ) || 
										(float)areainter/(float)area2>0.6  &&  distcenters < (Bx2-Bx1)/2 )
									{
										//If temporal coherence detection was found, or just not found using tracking or color
										//new detection is preferred, threfore previous info is rewritten
										if (DatosFacialesPrevios.Faces[iii]->headtracked ||
											DatosFacialesPrevios.Faces[iii]->boColorBasedFound ||
											!boDetectadodePrevia[iii])
										{
											if (DatosFacialesAux.Faces[ii]->hs)
											{
												DatosFacialesPrevios.Faces[iii]->hsx1=DatosFacialesAux.Faces[ii]->hsx1;
												DatosFacialesPrevios.Faces[iii]->hsx2=DatosFacialesAux.Faces[ii]->hsx2;
												DatosFacialesPrevios.Faces[iii]->hsy1=DatosFacialesAux.Faces[ii]->hsy1;
												DatosFacialesPrevios.Faces[iii]->hsy2=DatosFacialesAux.Faces[ii]->hsy2;

												EstimateFacefromHS(&DatosFacialesPrevios,iii);
											}
											else//no hs
											{
												DatosFacialesPrevios.Faces[iii]->x1=DatosFacialesAux.Faces[ii]->x1;
												DatosFacialesPrevios.Faces[iii]->x2=DatosFacialesAux.Faces[ii]->x2;
												DatosFacialesPrevios.Faces[iii]->y1=DatosFacialesAux.Faces[ii]->y1;
												DatosFacialesPrevios.Faces[iii]->y2=DatosFacialesAux.Faces[ii]->y2;
											}

											//If coherence based was not found, indicates it was, just to preserve persistence
											if (!boDetectadodePrevia[iii])
											{
												boDetectadodePrevia[iii]=true;
												DeteccionesdePrevia++;
											}

											//Copies type of detection
											DatosFacialesPrevios.Faces[iii]->fb=DatosFacialesAux.Faces[ii]->fb;
											DatosFacialesPrevios.Faces[iii]->lb=DatosFacialesAux.Faces[ii]->lb;
											DatosFacialesPrevios.Faces[iii]->hs=DatosFacialesAux.Faces[ii]->hs;

											//Cancela el detector ya que hemos modificado la ventanita
											DatosFacialesPrevios.Faces[iii]->boLKTracker=false;
											DatosFacialesPrevios.Faces[iii]->headtracked=false;
											DatosFacialesPrevios.Faces[iii]->boColorBasedFound=false;
										}

										//Cancels new detection
										boNueva[ii]=false;
										NumFacesAuxNuevas--;
									}
								}
							}
						}
					}//for redundancy check
				}//if hubo nuevas detecciones(creo que sobra)
			}//if busca en toda la imagen		
		}
		
		//---END SEARCH FOR NEW FACES---
		

		if (boDebugLog)
			{
				char cadena[256];
				sprintf(cadena,"Stores faces\n");

				DebugLog->WriteToLogFile(cadena);
			}
		
		
		//STORES in temporal memory faces detected by temporal coherence (if any)
		int NumFacesStillToDie=0;

		//Counts faces not detected but with persistence longer than one frame
		for (i=0;i<DatosFacialesPrevios.NumFaces;i++)
		{
			//Only those not detected faces
			if (!boDetectadodePrevia[i])
			{
				//Whose eyes where located at least once
				if (DatosFacialesPrevios.Faces[i]->boEyesDetectedAtLeastOnceforThisFace)
				{
					//Sets its life for new lost faces
					if (DatosFacialesPrevios.Faces[i]->framestodie==-1)
					{
						DatosFacialesPrevios.Faces[i]->framestodie=8;
						NumFacesStillToDie++;
						boStilltoDie[i]=true;
					}
					else
					{
						//Checks if if it is still alive
						if (DatosFacialesPrevios.Faces[i]->framestodie>0)
						{
							NumFacesStillToDie++;
							boStilltoDie[i]=true;
						}//In any other case it will be removed below
						else //Copies detection threads which will be removed from the list of active detection threads
						{
							int infaces=DatosFacialesLost.NumFaces;

							if (infaces)//there are already faces in the lost list
							{
								CFacialDataperImage	DF;
								DF.Init(infaces);
								//Copies previous stored to temporary variable
								for (yy=0;yy<infaces;yy++)
								{
									DF.Faces[yy]->CopyFacialData(DatosFacialesLost.Faces[yy]);

								}
								DatosFacialesLost.Libera();
								infaces++;
								DatosFacialesLost.Init(infaces);
								//Recover previously stored
								for ( yy=0;yy<infaces-1;yy++)
								{
									DatosFacialesLost.Faces[yy]->CopyFacialData(DF.Faces[yy]);
								}
								DF.Libera();
							}
							else
							{//Initializes and copies detection thread to die
								infaces++;
								DatosFacialesLost.Init(infaces);
							}
							//Copies new added
							DatosFacialesLost.Faces[infaces-1]->CopyFacialData(DatosFacialesPrevios.Faces[i]);
							
						}
					}
				}
			}
			else //It was detected
				DatosFacialesPrevios.Faces[i]->framestodie=-1;
		
		}//for

		//Allocs memory for all faces detected, coherent and news
		*NumFaces=DeteccionesdePrevia+NumFacesAuxNuevas+NumFacesStillToDie;

		//Faces in current frame index
		int iaux=0;

		if (*NumFaces)
		{
			//Something has been detected using temporal coherence
			nConsecutivasDetectandoporCoherenciaTemporal++;
		
			//Structure to store facial data in current frame
			DatosFaciales.Init(*NumFaces,DatosFacialesPrevios.nSubjects);

			//Primero copiamos las que se localizaron por coherencia con la previa
			//Copiamos las que hemos colocado temporalmente en DatosFacialesPrevios
			for (i=0;i<DatosFacialesPrevios.NumFaces;i++)
			{
				//It was detected
				if (boDetectadodePrevia[i])
				{
					DatosFaciales.Faces[iaux]->CopyFacialData(DatosFacialesPrevios.Faces[i]);

					//Increases persistence 
					DatosFaciales.Faces[iaux]->persistence++;

					

					//Increases detected faces counter
					iaux++;
				}
				else//It was not detected
				{
					//Extends temporal coherence more than one frame
					if (boStilltoDie[i])
					{
						DatosFaciales.Faces[iaux]->CopyFacialData(DatosFacialesPrevios.Faces[i]);

						//Reduces its life
						DatosFaciales.Faces[iaux]->framestodie--;

						//Increases detected faces counter
						iaux++;
					}
				}
			}//for

			//Copies new detections achieved using the whole image
			if (NumFacesAuxNuevas)
			{
				for (i=0;i<NumFacesAux;i++)
				{
					//Si la detectó y la considera no redundante
					if (boNueva[i])
					{
						DatosFaciales.Faces[iaux]->CopyFacialData(DatosFacialesAux.Faces[i]);
					
						iaux++;
					}
				}							
			}
			
			if (NumFacesAux  && DeteccionesdePrevia)
			{
			//Libera auxiliar
				DatosFacialesAux.Libera();

				//Si chewqueó redundancias
				if (DeteccionesdePrevia)
					delete [] boNueva;
			}

		}//if (*NumFaces)

		if (DatosFacialesPrevios.NumFaces)
		{
			delete [] boDetectadodePrevia;
			delete [] boStilltoDie;
		}
		
		//Liberamos los datos previos almacenados si los hubiera dado que ya no tienen validez
		DatosFacialesPrevios.Libera();

	}//If there were detections in previous frame
	else *NumFaces=0; //No hubo detecciones previas
}

/******************************************************************************************************************

	Part of FindFaces in charge of searching in the whole image

*******************************************************************************************************************/
void CENCARA2_2Detector::SearchIntheWholeImage(int *NumFaces,int escala,IplImage *image2)
{

	int		NumFacesYaEncontradas;
	int		ii;
	int		FacesvsHS=4;
	

	//Se aplica si no hemos detectado a partir de las previas,  ni hemos ya buscado en toda la imagen, o si no se exige velocidad
	if ((!(*NumFaces) && !boAlreadySearchedintheWholeImage))
	{		
		nConsecutivasDetectandoporCoherenciaTemporal=0;

		//Una de cada dos veces busca caras o siempre si es la primera imagen (válido para imágenes no secuencias)
		if ((nProcesadas%FacesvsHS || (nProcesadas==1) || !boSpeed ) )
		{
			//Busca con Viola por defecto, profile o el propio alternando el método
			int tipo=(int)(((double)rand()/(double)RAND_MAX)*3.0);

			NumFacesYaEncontradas=0;
			*NumFaces=0;

			if (boDebugLog)
			{
				char cadena[512];
				sprintf(cadena,"WI Antes búsqueda caras %d\n",*NumFaces);

				DebugLog->WriteToLogFile(cadena);
			}


			//Using ViolaFaceDetector
			if (tipo==0 ||  tipo==1 ||!boSpeed)
			{
				if (ViolaFaceDetector->boCascada && boBuscaconViolaCaras)
				{

					if (boDebugLog)
					{
						char cadena[512];
						sprintf(cadena,"WI Antes búsqueda Viola\n");

						DebugLog->WriteToLogFile(cadena);
					}
					ViolaFaceDetector->ProcessImage(image2,1);//Uso la imagen escalada y evitamos na nueva pirámide

					//Si ha detectado con Viola
					if (ViolaFaceDetector->NumObjects)
					{

						*NumFaces=ViolaFaceDetector->NumObjects;
						DatosFaciales.Init(*NumFaces,ids_sofar);

						for (ii=0;ii<*NumFaces;ii++)
						{							
							DatosFaciales.Faces[ii]->hs=false;	//No fue detección HS
							DatosFaciales.Faces[ii]->fb=false;
							DatosFaciales.Faces[ii]->lb=false;
							//Copia los contenedores detectados
							DatosFaciales.Faces[ii]->x1=ViolaFaceDetector->x1[ii]*escala;
							DatosFaciales.Faces[ii]->y1=ViolaFaceDetector->y1[ii]*escala;
							DatosFaciales.Faces[ii]->x2=ViolaFaceDetector->x2[ii]*escala;
							DatosFaciales.Faces[ii]->y2=ViolaFaceDetector->y2[ii]*escala;

							
							IplUtils.WindowTestP(imagegrayIN,&DatosFaciales.Faces[ii]->x1,&DatosFaciales.Faces[ii]->y1,
							&DatosFaciales.Faces[ii]->x2,&DatosFaciales.Faces[ii]->y2);

							DatosFaciales.Faces[ii]->framestodie=-1;
						}
					}
					else *NumFaces=0;
				}
			}//ViolaFaceDetector

			if (boDebugLog)
			{
				char cadena[512];
				sprintf(cadena,"WI Antes búsqueda caras propio %d\n",*NumFaces);

				DebugLog->WriteToLogFile(cadena);
			}
			
			//Stores previous detection number
			NumFacesYaEncontradas=*NumFaces;

			//Busca caras con ViolaProfileDetector
			if (tipo==2 || !boSpeed)
			{
				if (ViolaProfileDetector->boCascada && boBuscaconViolaCaras)
				{
					ViolaProfileDetector->ProcessImage(image2,1);//Uso la imagen escalada y evitamos na nueva pirámide

					//Si ha detectado con Viola
					if (ViolaProfileDetector->NumObjects)
					{
						//If there are already detections, they are stored temporaly
						if (*NumFaces)
						{							
							DatosFacialesAux.Init(NumFacesYaEncontradas,ids_sofar);

							for (ii=0;ii<NumFacesYaEncontradas;ii++)
							{
								DatosFacialesAux.Faces[ii]->CopyFacialData(DatosFaciales.Faces[ii]);
							}		
						}

						//Frees memory
						DatosFaciales.Libera();

						DatosFaciales.Init(ViolaProfileDetector->NumObjects+NumFacesYaEncontradas,ids_sofar);

						//Guarda las previas (caso detección de sólo una imagen
						for (ii=0;ii<NumFacesYaEncontradas;ii++)
						{
							DatosFaciales.Faces[ii]->CopyFacialData(DatosFacialesAux.Faces[ii]);
						}

						for (ii=NumFacesYaEncontradas;ii<ViolaProfileDetector->NumObjects+NumFacesYaEncontradas;ii++)
						{
							DatosFaciales.Faces[ii]->hs=false;	//No fue detección HS
							DatosFaciales.Faces[ii]->fb=false;
							DatosFaciales.Faces[ii]->lb=false;
							//Copia los contenedores detectados
							DatosFaciales.Faces[ii]->x1=ViolaProfileDetector->x1[ii-NumFacesYaEncontradas]*escala;
							DatosFaciales.Faces[ii]->y1=ViolaProfileDetector->y1[ii-NumFacesYaEncontradas]*escala;
							DatosFaciales.Faces[ii]->x2=ViolaProfileDetector->x2[ii-NumFacesYaEncontradas]*escala;
							DatosFaciales.Faces[ii]->y2=ViolaProfileDetector->y2[ii-NumFacesYaEncontradas]*escala;

							IplUtils.WindowTestP(imagegrayIN,&DatosFaciales.Faces[ii]->x1,&DatosFaciales.Faces[ii]->y1,
							&DatosFaciales.Faces[ii]->x2,&DatosFaciales.Faces[ii]->y2);

							DatosFaciales.Faces[ii]->framestodie=-1;
						}


						//If aux memory was used it is released
						if (*NumFaces)
							DatosFacialesAux.Libera();

						//Updates number of faces
						*NumFaces=ViolaProfileDetector->NumObjects+NumFacesYaEncontradas;
					}
					else *NumFaces=NumFacesYaEncontradas;					
				}
			}//ViolaProfileDetector
		}//Busca caras
		
		//Upper part or FullBody or Lower Body
		if ( ((!(nProcesadas%FacesvsHS)) || (nProcesadas==1) || !boSpeed) )
		{

			//Organiza aleatorios según configuración
			float nrand=2.0;
			int tipolb=1;

			if (boBuscaconViolaFullBody)
			{
				nrand+=1.f;
				tipolb++;//to index the rand value for lb
			}

			if (boSearchLegs)
			{
				nrand+=1.f;
				tipolb++;//to index the rand value for lb
			}





			//Calcula aleatorio para casos en los que no usemos las frecuencia
			int tipo=(int)(((double)rand()/(double)RAND_MAX)*nrand);
		
			//Stores previous detection number
			NumFacesYaEncontradas=*NumFaces;

			if (boDebugLog)
			{
				char cadena[512];
				sprintf(cadena,"WI Antes búsqueda HS %d\n",*NumFaces);

				DebugLog->WriteToLogFile(cadena);
			}
			
			if ((tipo==0 || tipo==1) || !boSpeed)//My HS detector ViolaHSDetector
			{
				
				//Salvadas anteriores
				if (ViolaHSDetector->boCascada && boBuscaconViolaHS)
				{
					
					ViolaHSDetector->ProcessImage(image2,1);//Uso la imagen escalada y evitamos na nueva pirámide

					//NumFaces depends on previous and new detections
					if (ViolaHSDetector->NumObjects)
					{
						//If there are already detections, they are stored temporaly
						if (*NumFaces)
						{
							NumFacesYaEncontradas=*NumFaces;
							DatosFacialesAux.Init(NumFacesYaEncontradas,ids_sofar);

							for (ii=0;ii<NumFacesYaEncontradas;ii++)
							{
								DatosFacialesAux.Faces[ii]->CopyFacialData(DatosFaciales.Faces[ii]);			
							}		
						}

						//Frees memory
						DatosFaciales.Libera();

						DatosFaciales.Init(ViolaHSDetector->NumObjects+NumFacesYaEncontradas,ids_sofar);

						//Guarda las previas (caso de teción de sólo una imagen
						for (ii=0;ii<NumFacesYaEncontradas;ii++)
						{
							DatosFaciales.Faces[ii]->CopyFacialData(DatosFacialesAux.Faces[ii]);
						}

						//Copia las detecciones FullBody
						for (ii=NumFacesYaEncontradas;ii<ViolaHSDetector->NumObjects+NumFacesYaEncontradas;ii++)
						{
							DatosFaciales.Faces[ii]->hs=true;	//No fue detección HS
							DatosFaciales.Faces[ii]->fb=false;
							DatosFaciales.Faces[ii]->lb=false;

							//Copia los contenedores detectados
							DatosFaciales.Faces[ii]->hsx1=ViolaHSDetector->x1[ii-NumFacesYaEncontradas]*escala;
							DatosFaciales.Faces[ii]->hsy1=ViolaHSDetector->y1[ii-NumFacesYaEncontradas]*escala;
							DatosFaciales.Faces[ii]->hsx2=ViolaHSDetector->x2[ii-NumFacesYaEncontradas]*escala;
							DatosFaciales.Faces[ii]->hsy2=ViolaHSDetector->y2[ii-NumFacesYaEncontradas]*escala;

							IplUtils.WindowTestP(imagegrayIN,&DatosFaciales.Faces[ii]->hsx1,&DatosFaciales.Faces[ii]->hsy1,
							&DatosFaciales.Faces[ii]->hsx2,&DatosFaciales.Faces[ii]->hsy2);

							EstimateFacefromHS(&DatosFaciales,ii);

							DatosFaciales.Faces[ii]->framestodie=-1;
						}

						//If aux memory was used it is released
						if (*NumFaces)
							DatosFacialesAux.Libera();

						//New number of detections
						*NumFaces=ViolaHSDetector->NumObjects+NumFacesYaEncontradas;

					}
					else
						*NumFaces=NumFacesYaEncontradas;
				}
				else//No classifier available
				{
					//Si está procesando una no hace falta copiar porque no hemos liberado y deben estar
					*NumFaces=NumFacesYaEncontradas;
				}
			}

			if (boDebugLog)
			{
				char cadena[512];
				sprintf(cadena,"WI Tras búsqueda HS %d\n",*NumFaces);

				DebugLog->WriteToLogFile(cadena);
			}
			
			//Stores previous detection number
			NumFacesYaEncontradas=*NumFaces;
			if (tipo==2 || !boSpeed) //FullBody detector
			{
				//Salvadas anteriores
				if (ViolaFullBodyDetector->boCascada && boBuscaconViolaFullBody)
				{
					ViolaFullBodyDetector->ProcessImage(image2,1);//Uso la imagen escalada y evitamos na nueva pirámide

					//NumFaces depends on previous and new detections
					if (ViolaFullBodyDetector->NumObjects)
					{
						//If there are already detections, they are stored temporaly
						if (*NumFaces)
						{
							NumFacesYaEncontradas=*NumFaces;
							DatosFacialesAux.Init(NumFacesYaEncontradas,ids_sofar);

							for (ii=0;ii<NumFacesYaEncontradas;ii++)
							{
								DatosFacialesAux.Faces[ii]->CopyFacialData(DatosFaciales.Faces[ii]);							
							}		
						}

						//Frees memory
						DatosFaciales.Libera();
						
						DatosFaciales.Init(ViolaFullBodyDetector->NumObjects+NumFacesYaEncontradas,ids_sofar);

						//Guarda las previas (caso de teción de sólo una imagen
						for (ii=0;ii<NumFacesYaEncontradas;ii++)
						{
							DatosFaciales.Faces[ii]->CopyFacialData(DatosFacialesAux.Faces[ii]);
						}

						//Copia las detecciones FullBody
						for (ii=NumFacesYaEncontradas;ii<ViolaFullBodyDetector->NumObjects+NumFacesYaEncontradas;ii++)
						{
							DatosFaciales.Faces[ii]->hs=false;	//No fue detección HS
							DatosFaciales.Faces[ii]->fb=true;
							DatosFaciales.Faces[ii]->lb=false;

							//Copia los contenedores detectados
							DatosFaciales.Faces[ii]->fbx1=ViolaFullBodyDetector->x1[ii-NumFacesYaEncontradas]*escala;
							DatosFaciales.Faces[ii]->fby1=ViolaFullBodyDetector->y1[ii-NumFacesYaEncontradas]*escala;
							DatosFaciales.Faces[ii]->fbx2=ViolaFullBodyDetector->x2[ii-NumFacesYaEncontradas]*escala;
							DatosFaciales.Faces[ii]->fby2=ViolaFullBodyDetector->y2[ii-NumFacesYaEncontradas]*escala;

							IplUtils.WindowTestP(imagegrayIN,&DatosFaciales.Faces[ii]->fbx1,&DatosFaciales.Faces[ii]->fby1,
							&DatosFaciales.Faces[ii]->fbx2,&DatosFaciales.Faces[ii]->fby2);

							EstimateFacefromFB(&DatosFaciales,ii);

							DatosFaciales.Faces[ii]->framestodie=-1;
						}

						//If aux memory was used it is released
						if (NumFacesYaEncontradas)
							DatosFacialesAux.Libera();

						//New number of detections
						*NumFaces=ViolaFullBodyDetector->NumObjects+NumFacesYaEncontradas;
					}
					else
						*NumFaces=NumFacesYaEncontradas;
				}
				else//No classifier available
				{
					//Si está procesando una no hace falta copiar porque no hemos liberado y deben estar
					*NumFaces=NumFacesYaEncontradas;
				}		
			}	


			//
			//Stores previous detection number
			NumFacesYaEncontradas=*NumFaces;
			if (tipo==tipolb || !boSpeed) //LowerBody detector
			{
				//Salvadas anteriores
				if (ViolaLowerBodyDetector->boCascada && boSearchLegs)
				{
					ViolaLowerBodyDetector->ProcessImage(image2,1);//Uso la imagen escalada y evitamos na nueva pirámide

					//NumFaces depends on previous and new detections
					if (ViolaLowerBodyDetector->NumObjects)
					{
						//If there are already detections, they are stored temporaly
						if (*NumFaces)
						{
							NumFacesYaEncontradas=*NumFaces;
							DatosFacialesAux.Init(NumFacesYaEncontradas,ids_sofar);

							for (ii=0;ii<NumFacesYaEncontradas;ii++)
							{
								DatosFacialesAux.Faces[ii]->CopyFacialData(DatosFaciales.Faces[ii]);							
							}		
						}

						//Frees memory
						DatosFaciales.Libera();
						
						DatosFaciales.Init(ViolaLowerBodyDetector->NumObjects+NumFacesYaEncontradas,ids_sofar);

						//Guarda las previas (caso de teción de sólo una imagen
						for (ii=0;ii<NumFacesYaEncontradas;ii++)
						{
							DatosFaciales.Faces[ii]->CopyFacialData(DatosFacialesAux.Faces[ii]);
						}

						//Copia las detecciones LowerBody
						for (ii=NumFacesYaEncontradas;ii<ViolaLowerBodyDetector->NumObjects+NumFacesYaEncontradas;ii++)
						{
							DatosFaciales.Faces[ii]->hs=false;	//No fue detección HS
							DatosFaciales.Faces[ii]->fb=false;
							DatosFaciales.Faces[ii]->lb=true;

							//Copia los contenedores detectados
							DatosFaciales.Faces[ii]->lbx1=ViolaLowerBodyDetector->x1[ii-NumFacesYaEncontradas]*escala;
							DatosFaciales.Faces[ii]->lby1=ViolaLowerBodyDetector->y1[ii-NumFacesYaEncontradas]*escala;
							DatosFaciales.Faces[ii]->lbx2=ViolaLowerBodyDetector->x2[ii-NumFacesYaEncontradas]*escala;
							DatosFaciales.Faces[ii]->lby2=ViolaLowerBodyDetector->y2[ii-NumFacesYaEncontradas]*escala;

							IplUtils.WindowTestP(imagegrayIN,&DatosFaciales.Faces[ii]->lbx1,&DatosFaciales.Faces[ii]->lby1,
							&DatosFaciales.Faces[ii]->lbx2,&DatosFaciales.Faces[ii]->lby2);

							EstimateFacefromFB(&DatosFaciales,ii);

							DatosFaciales.Faces[ii]->framestodie=-1;
						}

						//If aux memory was used it is released
						if (NumFacesYaEncontradas)
							DatosFacialesAux.Libera();

						//New number of detections
						*NumFaces=ViolaLowerBodyDetector->NumObjects+NumFacesYaEncontradas;
					}
					else
						*NumFaces=NumFacesYaEncontradas;
				}
				else//No classifier available
				{
					//Si está procesando una no hace falta copiar porque no hemos liberado y deben estar
					*NumFaces=NumFacesYaEncontradas;
				}		
			}	



		}

		

	}//if (! (*NumFaces) )

	if (boDebugLog)
	{
		char cadena[512];
		sprintf(cadena,"WI Terminando whole image %d\n",*NumFaces);

		DebugLog->WriteToLogFile(cadena);
	}
}



/******************************************************************************************************************

	Part of FindFaces in charge of removing redundant faces

*******************************************************************************************************************/
void CENCARA2_2Detector::RemoveRedundantFaces(int *NumFaces)
{
	int NumFacesAux;
	int i;

	//Deletes REDUNDANT faces, useful only if there are more than one face detected
	//Copies detections to a temporarly structure
	if (*NumFaces>1)
	{
		DatosFacialesAux.Init(*NumFaces,DatosFaciales.nSubjects);

		for (int ii=0;ii<*NumFaces;ii++)
		{
			DatosFacialesAux.Faces[ii]->CopyFacialData(DatosFaciales.Faces[ii]);
		}		

		DatosFaciales.Libera();

		//Gives more confident to frontal face detection with higher persistence
		boRedundante= (bool *)new unsigned char[*NumFaces*sizeof(bool)];
		NumFacesAux=*NumFaces;
		for (i=0;i<NumFacesAux;i++)
			boRedundante[i]=false;

		for (i=0;i<NumFacesAux-1;i++)
		{			
			int Ax1,Ax2,Ay1,Ay2;
			int Bx1,Bx2,By1,By2;
			int Inx1,Inx2,Iny1,Iny2;
			long areaA,areaB,areaIn;

			//Si no ha sido marcado y no fue FullBody
			if (!boRedundante[i] && !DatosFacialesAux.Faces[i]->fb && !DatosFacialesAux.Faces[i]->lb)
			{
				int sx=DatosFacialesAux.Faces[i]->x2-DatosFacialesAux.Faces[i]->x1,
						sy=DatosFacialesAux.Faces[i]->y2-DatosFacialesAux.Faces[i]->y1;
				
				Ax1=DatosFacialesAux.Faces[i]->x1;
				Ax2=DatosFacialesAux.Faces[i]->x2;
				Ay1=DatosFacialesAux.Faces[i]->y1;
				Ay2=DatosFacialesAux.Faces[i]->y2;
				
				areaA=sx*sy;

				for (int j=i+1;j<NumFacesAux;j++)
				{
					if (!boRedundante[i] && !boRedundante[j] && !DatosFacialesAux.Faces[j]->fb && !DatosFacialesAux.Faces[j]->lb)
					{
						int sx=DatosFacialesAux.Faces[j]->x2-DatosFacialesAux.Faces[j]->x1,
						sy=DatosFacialesAux.Faces[j]->y2-DatosFacialesAux.Faces[j]->y1;
						
						Bx1=DatosFacialesAux.Faces[j]->x1;
						Bx2=DatosFacialesAux.Faces[j]->x2;
						By1=DatosFacialesAux.Faces[j]->y1;
						By2=DatosFacialesAux.Faces[j]->y2;
						
						areaB=sy*sx;

						//área de intersección
						Inx1=Ax1;
						if (Bx1>Inx1) Inx1=Bx1;
						Iny1=Ay1;
						if (By1>Iny1) Iny1=By1;
						Inx2=Ax2;
						if (Bx2<Inx2) Inx2=Bx2;
						Iny2=Ay2;
						if (By2<Iny2) Iny2=By2;

						if (Inx2>Inx1 && Iny2>Iny1)
							areaIn=(Inx2-Inx1)*(Iny2-Iny1);
						else areaIn=0;

						//Chequea ahora si una está contenida en otra con bastante solape a la mayor o si hay mucho solape
						// Si una es interior a otra las suponemos solapadas
						// O si el solape es alto
						if ( (MathUtils.VentanaEnVentana(Ax1,Ay1,Ax2,Ay2,Bx1,By1,Bx2,By2) && (float)areaIn/(float)areaB>0.6) ||
							(MathUtils.VentanaEnVentana(Bx1,By1,Bx2,By2,Ax1,Ay1,Ax2,Ay2) && (float)areaIn/(float)areaA>0.6) ||
							(float)areaIn/(float)areaA>0.6 || (float)areaIn/(float)areaB>0.6 )
						{
							//si ambos son HS o ambos caras
							if ( (DatosFacialesAux.Faces[j]->hs && DatosFacialesAux.Faces[i]->hs) 
								|| (!DatosFacialesAux.Faces[j]->hs && !DatosFacialesAux.Faces[i]->hs))
							{
								//Anula la de menor persistencia o siendo iguales la de área mayor
								if (DatosFacialesAux.Faces[i]->persistence<DatosFacialesAux.Faces[j]->persistence
									|| (areaA>areaB && DatosFacialesAux.Faces[i]->persistence==DatosFacialesAux.Faces[j]->persistence) )
									boRedundante[i]=true;
								else
									boRedundante[j]=true;

							}
							else //en otro caso una es HS y la otra de cara anula la HS
							{
								if (DatosFacialesAux.Faces[i]->hs)
									boRedundante[i]=true;
								else
									boRedundante[j]=true;
							}
							
							(*NumFaces)--;
						}
					}
				}
			}
		}


		//Copies only non redundant faces 
		if (*NumFaces)
		{
			DatosFaciales.Init(*NumFaces,DatosFacialesAux.nSubjects);
			int iaux=0;

			for (int ii=0;ii<NumFacesAux;ii++)
			{
				if (!boRedundante[ii])
				{
					
					DatosFaciales.Faces[iaux]->CopyFacialData(DatosFacialesAux.Faces[ii]);
					iaux++;
				}
				
			}		
		}
		delete [] boRedundante;

		DatosFacialesAux.Libera();

	}

	
	//End redundants supression

}

//#define DBGFUSE
/******************************************************************************************************************

	Part of FindFaces in charge of finding similarities of new faces with lost ones

*******************************************************************************************************************/
void CENCARA2_2Detector::FuseLostandNew()
{
	CFacialDataperImage *DF,*DFL;
	int i,j,idnew,idlost, idnewT,idlostT,idnewF,idlostF,n,m,exnew,exlost;

	DF=GetFacialData();
	DFL=GetFacialDataLost();

	//Compares histograms
	bool boCheckTorsoHistogram=false;//Fuse using torso
	bool boCheckFaceHistogram=false;//Fuse using face

	//If there are data that could be potentially fused, i.e. new and old detection threads
	if (DF->NumFaces && DFL->NumFaces && (boCheckTorsoHistogram || boCheckFaceHistogram))
	{
		float temp,maxT=-1,minF=10000;

		for (i=0;i<DF->NumFaces;i++)
		{
			for (j=0;j<DFL->NumFaces;j++)
			{				
				for (n=0;n<DF->Faces[i]->Exemplars.iNImages;n++)
				{
					for (m=0;m<DFL->Faces[j]->Exemplars.iNImages;m++)
					{

						//Torso histogram
						if (boCheckTorsoHistogram)
						{
							//Calcula parecido si los histogramas están disponibles
							if (DF->Faces[i]->Exemplars.boHistogramAvailable[n] && 
								DFL->Faces[j]->Exemplars.boHistogramAvailable[m])
							{	
								//el tercer parámetro elige entre distintas medidas, // 0=Interseccion, 1=EMD, 2=Jeffrey Divergence
								//temp=(float)histmanag.ParecidoHistogramas(&DF->Faces[i]->Exemplars.THist[n],&DFL->Faces[j]->Exemplars.THist[m],2);
								//Modificar a función de OpenCV
								temp=(float)cvCompareHist(DF->Faces[i]->Exemplars.THist[n],DFL->Faces[j]->Exemplars.THist[m],2);
								if (temp>maxT)
								{
									maxT=temp;
									idnewT=i;
									idlostT=j;
								}
							}
						}

						//Face histogram
						if (boCheckFaceHistogram)
						{		
							//Sólo los compara si la perdida no ha coincidido en el tiempo con la nueva
							//Y los ejemplares que se comparan han tenido cierta persistencia
							if (DFL->Faces[j]->timestamplast<DF->Faces[i]->timestamp &&
								DF->Faces[i]->Exemplars.persistence[n]>10 &&
								DFL->Faces[j]->Exemplars.persistence[m]>10)
							{
								temp=MathUtils.L1(DF->Faces[i]->Exemplars.FaceHist[n],DFL->Faces[j]->Exemplars.FaceHist[m],256);//SI es uniforme o simplificado basta con 10

								if (temp<minF)
								{
									minF=temp;
									idnewF=i;
									idlostF=j;

									exnew=n;
									exlost=m;
								}
							}
						}
					}
				}
			}
		}//for (i=0;i<DF->NumFaces;i++)

		//Torso o Face
		//¿Hay alguien lo bastante parecido? En ese caso asocio la nueva hebra a la antigua
		if (maxT>UMBRAL_COMPARACION_TORSO_HISTOGRAMAS || minF<UMBRAL_COMPARACION_FACE_HISTOGRAMAS)
		{
			//Prefiere el de la cara, y si no el del torso
			if (minF<UMBRAL_COMPARACION_FACE_HISTOGRAMAS)
			{
				idnew=idnewF;
				idlost=idlostF;
				if (boDebugLog)
				{
					char cadena[256];
					sprintf(cadena,"FUSE %f\n",minF);

					DebugLog->WriteToLogFile(cadena);
				}
			}
			else
			{
				idnew=idnewT;
				idlost=idlostT;
				if (boDebugLog)
				{
					char cadena[256];
					sprintf(cadena,"FUSE %f\n",maxT);

					DebugLog->WriteToLogFile(cadena);
				}
			}

#ifdef DBGFUSE
								cvNamedWindow("NSI", 1);
								cvShowImage("NSI",DF->Faces[idnew]->Exemplars.StableImage[exnew]);
								cvNamedWindow("NAvg", 1);
								cvShowImage("NAvg",DF->Faces[idnew]->Exemplars.StableImageAvg[exnew]);
								cvNamedWindow("NIN", 1);
								cvShowImage("NIN",DF->Faces[idnew]->Exemplars.StableImageIN[exnew]);

								cvNamedWindow("LSI", 1);
								cvShowImage("LSI",DFL->Faces[idlost]->Exemplars.StableImage[exlost]);
								cvNamedWindow("LAvg", 1);
								cvShowImage("LAvg",DFL->Faces[idlost]->Exemplars.StableImageAvg[exlost]);
								cvNamedWindow("LINL", 1);
								cvShowImage("LINL",DFL->Faces[idlost]->Exemplars.StableImageIN[exlost]);

								//Imagen para escribir similariddad
								IplImage *iplt=cvCloneImage(DF->Faces[idnew]->Exemplars.StableImageIN[0]);
								cvSetZero(iplt);
								char cadena[128];
								sprintf(cadena,"(n %d,p %d)=",idnew,idlost);
								cvPutText( iplt,cadena,cvPoint(5,10),&stcvFont,CV_RGB(255,255,255));
								sprintf(cadena,"%f",minF);
								cvPutText( iplt,cadena,cvPoint(5,25),&stcvFont,CV_RGB(255,255,255));

								cvNamedWindow("Similaridad", 1);
								cvShowImage("Similaridad",iplt);
								

								cvReleaseImage(&iplt);
#endif



			//Uses previous Label
			strcpy(DF->Faces[idnew]->Label,DFL->Faces[idlost]->Label);

			//accumulated persistence
			DF->Faces[idnew]->persistence+=DFL->Faces[idlost]->persistence;

			//sets the initial timestamp according to the first detectiomn thread
			DF->Faces[idnew]->timestamp=DFL->Faces[idlost]->timestamp;

			//If the eyes have not been detected for the new detection thread, their info is copied
			if (!DF->Faces[idnew]->boEyesDetectedAtLeastOnceforThisFace)
			{
				DF->Faces[idnew]->boEyesDetectedAtLeastOnceforThisFace=DFL->Faces[idlost]->boEyesDetectedAtLeastOnceforThisFace;

				//sets the last timestamp according to the first detectiomn thread (only if the eyes has not been detected for the new one)
				DF->Faces[idnew]->timestamplast=DFL->Faces[idlost]->timestamplast;				
			}

			//Exemplars temporal structure
			CExemplars sttemp;

			//In the order to be fused
			sttemp.FuseExemplars(&DFL->Faces[idlost]->Exemplars,&DF->Faces[idnew]->Exemplars);
			
			//Exchanges new exemplars with temporary structure that should be fused
			DF->Faces[idnew]->Exemplars.ExchangeExemplarsData(&sttemp);

			//Removes exemplars information
			sttemp.FreeMem();

			//Removes from lost
			DatosFacialesAux.Init(DFL->NumFaces-1);

			int jj=0,ii;
			//Removes the detection thread fused from the lost list
			for (ii=0;ii<DFL->NumFaces;ii++)
			{
				if (ii!=idlost)
				{
					DatosFacialesAux.Faces[jj]->CopyFacialData(DFL->Faces[ii]);
					jj++;
				}				
			}	
			DFL->Libera();

			//Copia los que restan en el nuevo lost
			DFL->Init(jj);
			
			for (ii=0;ii<jj;ii++)
			{
				DFL->Faces[ii]->CopyFacialData(DatosFacialesAux.Faces[ii]);			
			}	

			DatosFacialesAux.Libera();
		}//maxT

		
	}

}


/******************************************************************************************************************

	Part of FindFaces in charge of finding facial elements

	It will search for candidates for each facial element, using Viola-Jones based detectors and
	tracking if the elements were detected in previous frame. The selected candidate configuration will
	be chosen according to the most coherent one for a normalized face

*******************************************************************************************************************/
void CENCARA2_2Detector::SearchEyesMouthandNose(IplImage *ima,IplImage *ima2,
												CFacialDataperImage	*DatosFaciales,int i,
												bool *boLEye, CvPoint *LEye,
												bool *boREye, CvPoint *REye,
												bool *boNose, CvPoint *Nose,
												bool *boLMouth, CvPoint *LMouth,
												bool *boRMouth, CvPoint *RMouth,
												bool *boMouth, CvPoint *Mouth,
												int scaledown,int scaleup,
												bool boUsaTrackers,bool boRotateImage)
{
	//Used to estimate patterns size
	int TrackWidth=((DatosFaciales->Faces[i]->x2-DatosFaciales->Faces[i]->x1)/16+1)*8;

	IplImage *imgdbg=cvCloneImage(ima);

	CvPoint *LEyeCandidates=NULL,
			*REyeCandidates=NULL,
			*NoseCandidates=NULL,
			*LMouthCandidates=NULL,
			*RMouthCandidates=NULL,
			*MouthCandidates=NULL;
	long 	*LEyeWidth=NULL,*LEyeHeight=NULL,
			*REyeWidth=NULL,*REyeHeight=NULL,
			*NoseWidth=NULL,*NoseHeight=NULL,
			*MouthWidth=NULL,*MouthHeight=NULL;

	int despx,despy,roiw,roih;//For ROIs

	bool LEyeTrackedAvailable,REyeTrackedAvailable,NoseTrackedAvailable,MouthTrackedAvailable;

	bool boPreviousEyesAvailable=DatosFaciales->Faces[i]->leye && DatosFaciales->Faces[i]->reye;

	int x1,y1,x2,y2,elx,ely,erx,ery;
	
	int Scale2Search;//Scale used for Viola-Jones detectors

	int LocalScaleDown=1;//A priori no duplica el tamaño de la imagen

	float mineyepairPCAerror=20;//Minimum PCA error accepted for eye pair

	if (scaleup!=-1)//Pyrup, duplica el tamaño de la imagen
	{
		LocalScaleDown=scaleup;

		Scale2Search=1;//No scaledown porque hacemos un scale up

		x1=DatosFaciales->Faces[i]->x1*scaleup;
		x2=DatosFaciales->Faces[i]->x2*scaleup;
		y1=DatosFaciales->Faces[i]->y1*scaleup;
		y2=DatosFaciales->Faces[i]->y2*scaleup;

		if (boPreviousEyesAvailable)
		{
			elx=DatosFaciales->Faces[i]->e_lx*scaleup;//de momento para este caso no tienen un valor útil
			ely=DatosFaciales->Faces[i]->e_ly*scaleup;
			erx=DatosFaciales->Faces[i]->e_rx*scaleup;
			ery=DatosFaciales->Faces[i]->e_ry*scaleup;
		}
	}
	else//Pyrdown
	{
		x1=DatosFaciales->Faces[i]->x1;
		x2=DatosFaciales->Faces[i]->x2;
		y1=DatosFaciales->Faces[i]->y1;
		y2=DatosFaciales->Faces[i]->y2;

		if (boPreviousEyesAvailable)
		{
			elx=DatosFaciales->Faces[i]->e_lx;
			ely=DatosFaciales->Faces[i]->e_ly;
			erx=DatosFaciales->Faces[i]->e_rx;
			ery=DatosFaciales->Faces[i]->e_ry;
		}

		if (x2-x1<80)
			Scale2Search=1;
		else
		{
			Scale2Search=scaledown;
			//¿Debe calcular la imagen de menor resolución
			if (scaledown==2 && !ima)
			{
				Scale2Search=1;//De momento
			}
		}
	}

	int ancho=x2-x1,
		alto=y2-y1;


	//Imagen rotada en función de detección previa
	IplImage *imaR=NULL,*ima2R=NULL;//Si no se rota no habrá imágenes de rotación

	float fdist;
	float interx=-1,intery=-1;
	double angulo=0.f;	
	
	if (boPreviousEyesAvailable)
	{
		//Intereye distance
		fdist=MathUtils.DistanciaEuclidea(elx,ely,erx,ery);

		//Gets angle for rotation according to eye location
		angulo=MathUtils.GetAngle4Rotation(elx,ely,erx,ery,&interx,&intery);

		if (boRotateImage)//De momento no para detectar, sólo para repeticiones
		{
			double xs=0,ys=0;

			//Rotation at full resolution
			
			//Computamos la rotación sólo en la zona del blob
			imaR=cvCloneImage(ima);
			
			CIplImageUtils iplutils;
			iplutils.RotateImage(ima,imaR,(float)angulo,interx,intery,true,cvRect(x1,y1,ancho,alto));
			
			//Rotated image at half resolution
			ima2R = cvCreateImage( cvSize(imaR->width/2,imaR->height/2), imaR->depth, imaR->nChannels );
			cvPyrDown( imaR, ima2R );//Would be faster rotating?

		}
	}


	//LEFT EYE
	int nLEyecandidates=0;
	
	//ROI offset for this search
	if (scaleup==-1)//Given the previous face area, used for detection+tracking. Relative to previos inbetween eye position
	{
		if (boPreviousEyesAvailable)
		{
			despx=(int)(interx-fdist*1.1f);
			despy=(int)(intery-0.6f*fdist);
			roiw=(int)(fdist*1.2f);
			roih=(int)(fdist*1.2f);
		}
		else//sin ojos previos
		{
			despx=x1;
			despy=y1+(int)(alto*0.1);
			roiw=(int)(ancho*0.6);
			roih=(int)(alto*0.6);
		}
	}
	else//Given the previous detection as an image, used for detection
	{
		despx=0;
		despy=(int)(ima->height*0.1);
		roiw=(int)(ima->width*0.5);
		roih=(int)(ima->height*0.4);
	}

	//Checks window boundaries
	IplUtils.WindowTest(ima,&despx,&despy,&roiw,&roih);

#ifdef DBGMode
	if (boDebugLog)
	{
		char cadena[512];
		sprintf(cadena,"LE EScala %d (%d,%d)-(%d,%d)\n",Scale2Search,despx,despy,roiw,roih);
		DebugLog->WriteToLogFile(cadena);
	}
#endif

	clock_t tdbg=clock();	

	//Gets candidates , using also rotated images if available (if not NULL should be given as parameter)
	LEyeTrackedAvailable=GetCandidates(ima, ima2, imaR, ima2R, Scale2Search,
		(float)angulo, interx, intery, //sólo utilizados si se proporciona la imagen rotada
		DatosFaciales->Faces[i]->boLEyeTracker && boUsaTrackers,
		DatosFaciales->Faces[i]->LEyeTracker,
		DatosFaciales->Faces[i]->leye,
		ViolaLeftEyeDetector,
		despx,despy,roiw,roih,ancho,
		&nLEyecandidates,&LEyeWidth,&LEyeHeight,&LEyeCandidates);		
	
	if (boDebugLog)
	{
		char cadena[256];
		
		sprintf(cadena,"Tras BuscarLEye (%ld)\n",clock() - tdbg );
	
		DebugLog->WriteToLogFile(cadena);
	}

	

	//Datos depuración
#ifdef DBGMode
	int idbg;

	for (idbg=0;idbg<nLEyecandidates;idbg++)
		if (boDebugLog)
		{
			char cadena[512];
			sprintf(cadena,"LEyeCandidates[%d]=(%d,%d)\n",idbg,LEyeCandidates[idbg].x,LEyeCandidates[idbg].y);
			DebugLog->WriteToLogFile(cadena);
		}
#endif

	//RIGHT EYE
	int nREyecandidates=0;

	//ROI offset for this search
	if (scaleup==-1)//Given the previous face area, used for detection+tracking. Relative to previos inbetween eye position
	{
		if (boPreviousEyesAvailable)
		{
			despx=(int)(interx-fdist*0.1f);
			despy=(int)(intery-0.6f*fdist);
			roiw=(int)(fdist*1.2f);
			roih=(int)(fdist*1.2f);
		}
		else
		{
			despx=(int)(x1+ancho*0.4);
			despy=y1+(int)(alto*0.1);
			roiw=(int)(ancho*0.6);
			roih=(int)(alto*0.4);
		}
	}
	else//Given the previous detection as an image, used for detection
	{
		despx=(int)(ima->width*0.5);;
		despy=(int)(ima->height*0.1);
		roiw=(int)(ima->width*0.5);
		roih=(int)(ima->height*0.4);
	}
	
	//Checks window boundaries
	IplUtils.WindowTest(ima,&despx,&despy,&roiw,&roih);

#ifdef DBGMode
	if (boDebugLog)
	{
		char cadena[512];
		sprintf(cadena,"RE EScala %d (%d,%d)-(%d,%d)\n",Scale2Search,despx,despy,roiw,roih);
		DebugLog->WriteToLogFile(cadena);
	}
#endif

	tdbg=clock();			
			
	//Gets candidates , NULL should be the rotated image (according to last eyes)
	REyeTrackedAvailable=GetCandidates(ima,  ima2, imaR, ima2R, Scale2Search,
		(float)angulo, interx, intery,
		DatosFaciales->Faces[i]->boREyeTracker && boUsaTrackers,
		DatosFaciales->Faces[i]->REyeTracker,
		DatosFaciales->Faces[i]->reye,
		ViolaRightEyeDetector,
		despx,despy,roiw,roih,ancho,
		&nREyecandidates,&REyeWidth,&REyeHeight,&REyeCandidates);

	if (boDebugLog)
	{
		char cadena[256];
		
		sprintf(cadena,"Tras REye (%ld)\n",clock() - tdbg );
	
		DebugLog->WriteToLogFile(cadena);
	}

	

	//Datos depuración
#ifdef DBGMode	
	for (idbg=0;idbg<nREyecandidates;idbg++)
		if (boDebugLog)
		{
			char cadena[512];
			sprintf(cadena,"REyeCandidates[%d]=(%d,%d)\n",idbg,REyeCandidates[idbg].x,REyeCandidates[idbg].y);
			DebugLog->WriteToLogFile(cadena);
		}
#endif

	//NOSE
	int nNosecandidates=0;

	//ROI offset for this search
	if (scaleup==-1)//Given the previous face area, used for detection+tracking. Relative to previos inbetween eye position
	{
		//Si hay datos de la nariz anterior, usa esa información para estimar donde buscarla con Viola
		if (DatosFaciales->Faces[i]->nose && AJUSTEBOCANARIZ)//Es algo que he añadido nuevo mayo 08
		{
			DatosFaciales->Faces[i]->NoseTracker->GetSearchArea(&despx,&despy,&roiw,&roih);
		}
		else//En otro caso usa info de los ojos
		{
			if (boPreviousEyesAvailable)
			{
				despx=(int)(interx-0.6f*fdist);
				despy=(int)(intery);
				roiw=(int)(fdist*1.2f);
				roih=(int)(fdist);
			}
			else
			{
				despx=(int)(x1+ancho*0.1);
				despy=(int)(y1+alto*0.2);
				roiw=(int)(ancho*0.8);
				roih=(int)(alto*0.6);
			}
		}
	}
	else//Given the previous detection as an image, used for detection
	{
		despx=(int)(ima->width*0.1);;
		despy=(int)(ima->height*0.2);
		roiw=(int)(ima->width*0.8);
		roih=(int)(ima->height*0.6);
	}

	//Checks window boundaries
	IplUtils.WindowTest(ima,&despx,&despy,&roiw,&roih);

#ifdef DBGMode
	if (boDebugLog)
	{
		char cadena[512];
		sprintf(cadena,"N EScala %d (%d,%d)-(%d,%d)\n",Scale2Search,despx,despy,roiw,roih);
		DebugLog->WriteToLogFile(cadena);
	}
#endif

	tdbg=clock();			
			
	//Gets candidates , NULL should be the rotated image (according to last eyes)
	NoseTrackedAvailable=GetCandidates(ima, ima2, imaR, ima2R,Scale2Search,
		(float)angulo, interx, intery,
		DatosFaciales->Faces[i]->boNoseTracker && boUsaTrackers,
		DatosFaciales->Faces[i]->NoseTracker,
		DatosFaciales->Faces[i]->nose || DatosFaciales->Faces[i]->nosecontainer,
		ViolaNoseDetector,
		despx,despy,roiw,roih,ancho,
		&nNosecandidates,&NoseWidth,&NoseHeight,&NoseCandidates);

	if (boDebugLog)
	{
		char cadena[256];
		
		sprintf(cadena,"Tras Nose (%ld)\n",clock() - tdbg );
	
		DebugLog->WriteToLogFile(cadena);
	}

	

	//Datos depuración	
#ifdef DBGMode
	for (idbg=0;idbg<nNosecandidates;idbg++)
		if (boDebugLog)
		{
			char cadena[512];
			sprintf(cadena,"NoseCandidates[%d]=(%d,%d)\n",idbg,NoseCandidates[idbg].x,NoseCandidates[idbg].y);
			DebugLog->WriteToLogFile(cadena);
		} 
#endif

	//MOUTH
	int nMouthcandidates=0;
	int nLMouthcandidates=0;
	int nRMouthcandidates=0;

	//ROI offset for this search
	if (scaleup==-1)//Given the previous face area, used for detection+tracking. Relative to previos inbetween eye position
	{
		//Si hay datos de la nariz anterior, usa esa información para estimar donde buscarla con Viola
		if (DatosFaciales->Faces[i]->mouth && AJUSTEBOCANARIZ)//Es algo que he añadido nuevo mayo 08
		{
			int auxx,auxy,dumx,dumy;
			DatosFaciales->Faces[i]->LMouthTracker->GetSearchArea(&despx,&despy,&roiw,&roih);
			DatosFaciales->Faces[i]->RMouthTracker->GetSearchArea(&auxx,&auxy,&dumx,&dumy);
			if (auxy<despy) despy=auxy;
			roiw=auxx+dumx-despx;
			roih=auxy+dumy-despy;

		}
		else//En otro caso usa info de los ojos
		{
			if (boPreviousEyesAvailable)
			{
				despx=(int)(interx-0.675f*fdist);
				despy=(int)(intery+0.5f*fdist);
				roiw=(int)(fdist*1.35f);
				roih=(int)(fdist*1.2f);
			}
			else
			{
				despx=(int)(x1+ancho*0.1);
				despy=(int)(y1+alto*0.6);
				roiw=(int)(ancho*0.8);
				roih=(int)(alto*0.4);
			}
		}
	}
	else//Given the previous detection as an image, used for detection
	{
		despx=(int)(ima->width*0.1);
		despy=(int)(ima->height*0.6);
		roiw=(int)(ima->width*0.8);
		roih=(int)(ima->height*0.4);
	}

	//Checks window boundaries
	IplUtils.WindowTest(ima,&despx,&despy,&roiw,&roih);
	
#ifdef DBGMode
	if (boDebugLog)
	{
		char cadena[512];
		sprintf(cadena,"M EScala %d (%d,%d)-(%d,%d)\n",Scale2Search,despx,despy,roiw,roih);
		DebugLog->WriteToLogFile(cadena);
	}
#endif

	tdbg=clock();			
			
	//(Un poco diferente porque son dos trackers y un detector)
	MouthTrackedAvailable=GetCandidates(ima,ima2, imaR, ima2R, Scale2Search,
		(float)angulo, interx, intery,
		DatosFaciales->Faces[i]->boLMouthTracker && boUsaTrackers,
		DatosFaciales->Faces[i]->LMouthTracker,
		DatosFaciales->Faces[i]->boRMouthTracker && boUsaTrackers,
		DatosFaciales->Faces[i]->RMouthTracker,
		DatosFaciales->Faces[i]->mouth || DatosFaciales->Faces[i]->mouthcontainer,
		ViolaMouthDetector,
		despx,despy,roiw,roih,ancho,
		&nLMouthcandidates,&LMouthCandidates,
		&nRMouthcandidates,&RMouthCandidates,
		&nMouthcandidates,&MouthWidth,&MouthHeight,&MouthCandidates);

	if (boDebugLog)
	{
		char cadena[256];
		
		sprintf(cadena,"Tras Mouth (%ld)\n",clock() - tdbg );
	
		DebugLog->WriteToLogFile(cadena);
	}

	

	//Datos depuración
#ifdef DBGMode	
	for (idbg=0;idbg<nMouthcandidates;idbg++)
		if (boDebugLog)
		{
			char cadena[512];
			sprintf(cadena,"MouthCandidates[%d]=(%d,%d)\n",idbg,MouthCandidates[idbg].x,MouthCandidates[idbg].y);
			DebugLog->WriteToLogFile(cadena);
		} 
#endif

	//Buscar ahora la combinación con mayor probabilidad
	//Hay muchas posibilidades: Todos detectados, sólo tres, sólo dos ...
	//Para todas las combinaciones de ojos
	int l,r,n,m;
	float maxlik=0.f,aux;
	int bestl=-1,bestr=-1,bestn=-1,bestm=-1;
	int tested=0;

	CMathUtils	MUtils;
	float	dist_leye_nose,dist_reye_nose,dist_eyes_avg,dist_mouth_nose;


	//Candidates are available

	//Are both eyes available?
	if (nLEyecandidates && nREyecandidates)
	{
		for (l=0;l<nLEyecandidates;l++)
		{
			for (r=0;r<nREyecandidates;r++)
			{
				//Depuración
				/*if (boDebugLog)
				{
					char cadena[512];
					sprintf(cadena,"Tratando par (%d,%d)\n",l,r);
					DebugLog->WriteToLogFile(cadena);
				} */

				//Trata el par si se cumplen ciertas restricciones
				if (LEyeCandidates[l].x<REyeCandidates[r].x && MathUtils.DistanciaEuclidea(LEyeCandidates[l].x,LEyeCandidates[l].y,REyeCandidates[r].x,REyeCandidates[r].y)>=MINIMALINTEREYE)
				{

					//Se dispone de algún elemento faciel además de los ojos
					if ( (nMouthcandidates || nNosecandidates))
					{
						//sólo con nariz
						for (n=0;n<nNosecandidates;n++)
						{						
							//Cuequear combinaciones y almacenar probabilidad
							aux=GetLikelihoodforNormalizedCoordinates(LEyeCandidates[l],REyeCandidates[r],
														   true,NoseCandidates[n],
														   false,NoseCandidates[n]);
							

#ifdef DBGMode
							char cadena[128];
							IplImage *Candidate=cvCloneImage(ima);
							cvCircle(Candidate,cvPoint(LEyeCandidates[l].x,LEyeCandidates[l].y),2, CV_RGB(0,255,0),-1,8,0);
							sprintf(cadena,"LEye %d, %d",LEyeCandidates[l].x,LEyeCandidates[l].y);
							cvPutText(Candidate,cadena,cvPoint(40,50),&stcvFont,CV_RGB(255,255,255));
							cvCircle(Candidate,cvPoint(REyeCandidates[r].x,REyeCandidates[r].y),2, CV_RGB(0,255,0),-1,8,0);
							sprintf(cadena,"REye %d, %d",REyeCandidates[r].x,REyeCandidates[r].y);
							cvPutText(Candidate,cadena,cvPoint(40,70),&stcvFont,CV_RGB(255,255,255));
							cvCircle(Candidate,cvPoint(NoseCandidates[n].x,NoseCandidates[n].y),2, CV_RGB(0,255,0),-1,8,0);
							sprintf(cadena,"Nose %d, %d",NoseCandidates[n].x,NoseCandidates[n].y);
							cvPutText(Candidate,cadena,cvPoint(40,90),&stcvFont,CV_RGB(255,255,255));
							
							sprintf(cadena,"Probabilidad %f",aux);
							cvPutText(Candidate,cadena,cvPoint(40,20),&stcvFont,CV_RGB(255,255,255));
							sprintf(cadena,"%s\\Frame%04d_%d_%d_%d_-1.jpg",cWorkDataDir,nProcesadas,l,r,n);
							cvSaveImage(cadena,Candidate);
							cvReleaseImage(&Candidate);


							//Depuración
							if (boDebugLog)
							{
								char cadena[512];
								sprintf(cadena,"Likelihood(%d,%d,%d,-1)=%f\n",l,r,n,aux);
								DebugLog->WriteToLogFile(cadena);
							} 
#endif
							if (aux>maxlik)
							{
								maxlik=aux;
								bestl=l;
								bestr=r;
								bestm=-1;
								bestn=n;
							}
						}
						//sólo con boca
						for (m=0;m<nMouthcandidates;m++)
						{
							//Cuequear combinaciones y almacenar probabilidad
							aux=GetLikelihoodforNormalizedCoordinates(LEyeCandidates[l],REyeCandidates[r],
														   false,MouthCandidates[m],
														   true,MouthCandidates[m]);
							

#ifdef DBGMode
							char cadena[128];
							IplImage *Candidate=cvCloneImage(ima);
							cvCircle(Candidate,cvPoint(LEyeCandidates[l].x,LEyeCandidates[l].y),2, CV_RGB(0,255,0),-1,8,0);
							sprintf(cadena,"LEye %d, %d",LEyeCandidates[l].x,LEyeCandidates[l].y);
							cvPutText(Candidate,cadena,cvPoint(40,50),&stcvFont,CV_RGB(255,255,255));
							cvCircle(Candidate,cvPoint(REyeCandidates[r].x,REyeCandidates[r].y),2, CV_RGB(0,255,0),-1,8,0);
							sprintf(cadena,"REye %d, %d",REyeCandidates[r].x,REyeCandidates[r].y);
							cvPutText(Candidate,cadena,cvPoint(40,70),&stcvFont,CV_RGB(255,255,255));
							cvCircle(Candidate,cvPoint(MouthCandidates[m].x,MouthCandidates[m].y),2, CV_RGB(0,255,0),-1,8,0);
							sprintf(cadena,"Mouth %d, %d",MouthCandidates[m].x,MouthCandidates[m].y);
							cvPutText(Candidate,cadena,cvPoint(40,110),&stcvFont,CV_RGB(255,255,255));
							
							sprintf(cadena,"Probabilidad %f",aux);
							cvPutText(Candidate,cadena,cvPoint(40,20),&stcvFont,CV_RGB(255,255,255));
							sprintf(cadena,"%s\\Frame%04d_%d_%d_-1_%d.jpg",cWorkDataDir,nProcesadas,l,r,m);
							cvSaveImage(cadena,Candidate);
							cvReleaseImage(&Candidate);


							//Depuración
							if (boDebugLog)
							{
								char cadena[512];
								sprintf(cadena,"Likelihood(%d,%d,-1,%d)=%f\n",l,r,m,aux);
								DebugLog->WriteToLogFile(cadena);
							} 
#endif
							if (aux>maxlik)
							{
								maxlik=aux;
								bestl=l;
								bestr=r;
								bestm=m;
								bestn=-1;
							}
						}//con boca

						//Con ambos
						if (nMouthcandidates && nNosecandidates)
						{
							for (n=0;n<nNosecandidates;n++)
							{
								for (m=0;m<nMouthcandidates;m++)
								{

									//No permite boca y nariz demasiado pegadas en comparación a los ojos
									dist_leye_nose=MUtils.DistanciaEuclidea(LEyeCandidates[l].x,LEyeCandidates[l].y,
										NoseCandidates[n].x,NoseCandidates[n].y);
									dist_reye_nose=MUtils.DistanciaEuclidea(REyeCandidates[r].x,REyeCandidates[r].y,
										NoseCandidates[n].x,NoseCandidates[n].y);
									dist_mouth_nose=MUtils.DistanciaEuclidea(MouthCandidates[m].x,MouthCandidates[m].y,
										NoseCandidates[n].x,NoseCandidates[n].y);

									dist_eyes_avg=(dist_leye_nose+dist_reye_nose)/2;

									//Cuequear combinaciones y almacenar probabilidad si las distancias parecen rudamente encajar
									if ( !(dist_eyes_avg>2*dist_mouth_nose) && !(dist_eyes_avg*2<dist_mouth_nose) )
										aux=GetLikelihoodforNormalizedCoordinates(LEyeCandidates[l],REyeCandidates[r],
														   true,NoseCandidates[n],
														   true,MouthCandidates[m]);
									else
										aux=-1;
									

#ifdef DBGMode
								char cadena[128];
								IplImage *Candidate=cvCloneImage(ima);
								cvCircle(Candidate,cvPoint(LEyeCandidates[l].x,LEyeCandidates[l].y),2, CV_RGB(0,255,0),-1,8,0);
								sprintf(cadena,"LEye %d, %d",LEyeCandidates[l].x,LEyeCandidates[l].y);
								cvPutText(Candidate,cadena,cvPoint(40,50),&stcvFont,CV_RGB(255,255,255));
								cvCircle(Candidate,cvPoint(REyeCandidates[r].x,REyeCandidates[r].y),2, CV_RGB(0,255,0),-1,8,0);
								sprintf(cadena,"REye %d, %d",REyeCandidates[r].x,REyeCandidates[r].y);
								cvPutText(Candidate,cadena,cvPoint(40,70),&stcvFont,CV_RGB(255,255,255));
								cvCircle(Candidate,cvPoint(NoseCandidates[n].x,NoseCandidates[n].y),2, CV_RGB(0,255,0),-1,8,0);
								sprintf(cadena,"Nose %d, %d",NoseCandidates[n].x,NoseCandidates[n].y);
								cvPutText(Candidate,cadena,cvPoint(40,90),&stcvFont,CV_RGB(255,255,255));
								cvCircle(Candidate,cvPoint(MouthCandidates[m].x,MouthCandidates[m].y),2, CV_RGB(0,255,0),-1,8,0);
								sprintf(cadena,"Mouth %d, %d",MouthCandidates[m].x,MouthCandidates[m].y);
								cvPutText(Candidate,cadena,cvPoint(40,110),&stcvFont,CV_RGB(255,255,255));
								
								sprintf(cadena,"Probabilidad %f",aux);
								cvPutText(Candidate,cadena,cvPoint(40,20),&stcvFont,CV_RGB(255,255,255));
								sprintf(cadena,"%s\\Frame%04d_%d_%d_%d_%d.jpg",cWorkDataDir,nProcesadas,l,r,n,m);
								cvSaveImage(cadena,Candidate);
								cvReleaseImage(&Candidate);


								//Depuración
								if (boDebugLog)
								{
									char cadena[512];
									sprintf(cadena,"Likelihood(%d,%d,%d,%d)=%f\n",l,r,n,m,aux);
									DebugLog->WriteToLogFile(cadena);
								} 
#endif
									if (aux>maxlik)
									{
										maxlik=aux;
										bestl=l;
										bestr=r;
										bestm=m;
										bestn=n;
									}

								}
							}
						}
					}//La nariz o la boca están disponibles

					//sólo se dispone de los dos ojos o las combinaciones con boca y/o nariz no resultaron adecuadas
					if (!(maxlik>0.f))
					{
					
						if (boDebugLog)
						{
							char cadena[512];
							sprintf(cadena,"Chequea combinación ojos (%d,%d) (%d,%d)\n",LEyeCandidates[l].x,LEyeCandidates[l].y,REyeCandidates[r].x,REyeCandidates[r].y);
							DebugLog->WriteToLogFile(cadena);
						} 
						// El que menor error de reconstrucción tenga
						//Error de reconstrucción
#ifndef USINGSIZEANDNOTPCAERROR
						float *minaux;
						minaux=GetPCAErrorforNormalizedCoordinates(imagegray,LEyeCandidates[l],REyeCandidates[r],0);

						if (boDebugLog)
						{
							char cadena[512];
							sprintf(cadena,"Chequeada combinación ojos (%d,%d)\n",l,r);
							DebugLog->WriteToLogFile(cadena);
						} 

						//Checks eye pair appearance
						if (minaux[0]<mineyepairPCAerror)
						{
							mineyepairPCAerror=minaux[0];
							bestl=l;
							bestr=r;
						}

#ifdef DBGMode
						char cadena[128];
						IplImage *Candidate=cvCloneImage(ima);
						cvCircle(Candidate,cvPoint(LEyeCandidates[l].x,LEyeCandidates[l].y),2, CV_RGB(0,255,0),-1,8,0);
						sprintf(cadena,"LEye %d, %d",LEyeCandidates[l].x,LEyeCandidates[l].y);
						cvPutText(Candidate,cadena,cvPoint(40,50),&stcvFont,CV_RGB(255,255,255));
						cvCircle(Candidate,cvPoint(REyeCandidates[r].x,REyeCandidates[r].y),2, CV_RGB(0,255,0),-1,8,0);
						sprintf(cadena,"REye %d, %d",REyeCandidates[r].x,REyeCandidates[r].y);
						cvPutText(Candidate,cadena,cvPoint(40,70),&stcvFont,CV_RGB(255,255,255));
						
						sprintf(cadena,"PCAerror %f",minaux[0]);
						cvPutText(Candidate,cadena,cvPoint(40,20),&stcvFont,CV_RGB(255,255,255));
						sprintf(cadena,"%s\\Frame%04d_%d_%d_-1_-1_PCAError.jpg",cWorkDataDir,nProcesadas,l,r);
						cvSaveImage(cadena,Candidate);
						cvReleaseImage(&Candidate);
#endif

#else
						//Chequear combinaciones ?en función del tamaño? y almacenar probabilidad
						//Por empezar a probar prefiere ojos seguidos !!!!!
						if (LEyeTrackedAvailable && REyeTrackedAvailable)
						{
							bestl=nLEyecandidates-1;
							bestr=nREyecandidates-1;
						}
						else//No se han podido seguir ambos ojos
						{
							//Izquierdo
							if (LEyeTrackedAvailable)
							{
								bestl=nLEyecandidates-1;
							}
							else//NO se siguió, se escoge el de mayor área de los disponibles
							{
								long areamax=0,areaaux;
								
								for (int lll=0;lll<nLEyecandidates;lll++)//Para el izquierdo toma el de mayor área
								{
									areaaux=LEyeWidth[lll]*LEyeHeight[lll];
									if (areaaux>areamax)
									{
										bestl=lll;
										areamax=areaaux;
									}
									
								}
							}

							//Derecho
							if (REyeTrackedAvailable)
							{
								bestr=nREyecandidates-1;
							}
							else//NO se siguió, se escoge el de mayor área de los disponibles
							{
								long areamax=0,areaaux;
								for (int lll=0;lll<nREyecandidates;lll++)//Para el derecho toma el de mayor área
								{
									areaaux=REyeWidth[lll]*REyeHeight[lll];
									if (areaaux>areamax)
									{
										bestr=lll;
										areamax=areaaux;
									}
								}
							}
						}
#endif
					
					}//if maxlik<0
				}//MININTEROJO

			}
		}
	}
	else//Both eyes are not available
	{
		//Chequear combinaciones laterales y almacenar probabilidad
#ifdef VALECONUNJOJOYLABOCA
		//Left eye and mouth?
		if (nLEyecandidates && (/*nLMouthcandidates ||*/ nMouthcandidates))
		{
			bestl=bestr=bestn=bestm=-1;

			//¿Cuál escojo? De momento la mayor de cada uno para ojos y boca, y nariz una que quede entre ambos (heurística floja)
			long area=0,laux;
			for (l=0;l<nLEyecandidates;l++)
			{
				laux=LEyeWidth[l]*LEyeHeight[l];
				if (laux>area)
				{
					area=laux;
					bestl=l;
				}
			}

			area=0;
			for (m=0;m<nMouthcandidates;m++)
			{
				laux=MouthWidth[m]*MouthHeight[m];
				if (laux>area)
				{
					area=laux;
					bestm=m;
				}
			}

			

			area=0;
			for (n=0;n<nNosecandidates;n++)
			{
				laux=NoseWidth[n]*NoseHeight[n];
				if (laux>area && NoseCandidates[n].y>LEyeCandidates[bestl].y+LEyeHeight[bestl]/2
					&& NoseCandidates[n].y<MouthCandidates[bestm].y+MouthHeight[bestm]/2)
				{
					area=laux;
					bestn=n;
				}
			}
		
		}
		else
		{
			//Right eye and mouth?
			if (nREyecandidates && (/*nRMouthcandidates ||*/ nMouthcandidates))
			{
				bestl=bestr=bestn=bestm=-1;

				//¿Cuál escojo? De momento la mayor de cada uno para ojos y boca, y nariz una que quede entre ambos
				long area=0,laux;
				for (r=0;r<nREyecandidates;r++)
				{
					laux=REyeWidth[r]*REyeHeight[r];
					if (laux>area)
					{
						area=laux;
						bestr=r;
					}
				}

				area=0;
				for (m=0;m<nMouthcandidates;m++)
				{
					laux=MouthWidth[m]*MouthHeight[m];
					if (laux>area)
					{
						area=laux;
						bestm=m;
					}
				}

				area=0;
				for (n=0;n<nNosecandidates;n++)
				{
					laux=NoseWidth[n]*NoseHeight[n];
					if (laux>area && NoseCandidates[n].y>REyeCandidates[bestr].y+REyeHeight[bestr]/2
					&& NoseCandidates[n].y<MouthCandidates[bestm].y+MouthHeight[bestm]/2)
					{
						area=laux;
						bestn=n;
					}
				}

			}
		}
#endif
	}
	//for 

	//Depuración
	/*if (boDebugLog)
	{
		char cadena[512];
		sprintf(cadena,"Escogido %d,%d,%d,%d\n",bestl,bestr,bestn,bestm);
		DebugLog->WriteToLogFile(cadena);
	} */

#ifdef DBGMode
	IplImage *iplSelected=cvCloneImage(ima);
#endif

	//Copies results to output variables
	if (bestl!=-1)
	{
		*boLEye=true;
		(*LEye).x=LEyeCandidates[bestl].x/LocalScaleDown;
		(*LEye).y=LEyeCandidates[bestl].y/LocalScaleDown;

		
		//Actualiza el patrón a seguir si fuera necesario
		//Si estamos siguiendo y no sólo detectando
		if (boUsaTrackers)
		{
			//Si el seleccionado fue un candidato no obtenido por seguimiento, es decir, se obtuvo por detección
			//debe actualizar o crear el patrón para el tracker
			if ( !(bestl==nLEyecandidates-1 && LEyeTrackedAvailable) )
			{
				//Si el tracker está disponible
				if (DatosFaciales->Faces[i]->boLEyeTracker)
					DatosFaciales->Faces[i]->LEyeTracker->SetPattern(ima,(*LEye).x,(*LEye).y);
				else
				{
					//Aloja el tracker de la boca
					DatosFaciales->Faces[i]->LEyeTracker=new COpenTracking(ima,(*LEye).x,(*LEye).y,TrackWidth,TrackWidth,PATSIZEX,PATSIZEY);
					DatosFaciales->Faces[i]->boLEyeTracker=true;
				}
			}
		}

#ifdef DBGMode
	cvCircle(iplSelected,cvPoint((*LEye).x,(*LEye).y),2, CV_RGB(0,255,0),-1,8,0);
#endif
	}
	else
		*boLEye=false;

	if (bestr!=-1)
	{
		*boREye=true;
		(*REye).x=REyeCandidates[bestr].x/LocalScaleDown;
		(*REye).y=REyeCandidates[bestr].y/LocalScaleDown;

		//Actualiza el patrón a seguir si fuera necesario
		//Si estamos siguiendo y no sólo detectando
		if (boUsaTrackers)
		{
			//Si el seleccionado fue un candidato no obtenido por seguimiento, es decir, se obtuvo por detección
			//debe actualizar o crear el patrón para el tracker
			if ( !(bestr==nREyecandidates-1 && REyeTrackedAvailable) )
			{
				//Si el tracker está disponible
				if (DatosFaciales->Faces[i]->boREyeTracker)
					DatosFaciales->Faces[i]->REyeTracker->SetPattern(ima,(*REye).x,(*REye).y);
				else
				{
					//Aloja el tracker de la boca
					DatosFaciales->Faces[i]->REyeTracker=new COpenTracking(ima,(*REye).x,(*REye).y,TrackWidth,TrackWidth,PATSIZEX,PATSIZEY);
					DatosFaciales->Faces[i]->boREyeTracker=true;
				}
			}
		}


#ifdef DBGMode
	cvCircle(iplSelected,cvPoint((*REye).x,(*REye).y),2, CV_RGB(0,255,0),-1,8,0);
#endif
	}
	else
		*boREye=false;

	if (bestn!=-1)
	{
		*boNose=true;
		(*Nose).x=NoseCandidates[bestn].x/LocalScaleDown;
		(*Nose).y=NoseCandidates[bestn].y/LocalScaleDown;

		//Actualiza el patrón a seguir si fuera necesario
		//Si estamos siguiendo y no sólo detectando
		if (boUsaTrackers)
		{
			//Si el seleccionado fue un candidato no obtenido por seguimiento, es decir, se obtuvo por detección
			//debe actualizar o crear el patrón para el tracker
			if ( !(bestn==nNosecandidates-1 && NoseTrackedAvailable) )
			{
				//Si el tracker está disponible
				if (DatosFaciales->Faces[i]->boNoseTracker)
					DatosFaciales->Faces[i]->NoseTracker->SetPattern(ima,(*Nose).x,(*Nose).y);
				else
				{
					//Aloja el tracker de la boca
					DatosFaciales->Faces[i]->NoseTracker=new COpenTracking(ima,(*Nose).x,(*Nose).y,TrackWidth,TrackWidth,PATSIZEX,PATSIZEY);
					DatosFaciales->Faces[i]->boNoseTracker=true;
				}
			}
		}


#ifdef DBGMode
	cvCircle(iplSelected,cvPoint((*Nose).x,(*Nose).y),2, CV_RGB(0,255,0),-1,8,0);
#endif
	}
	else
		*boNose=false;

	if (bestm!=-1)
	{
		*boMouth=true;
		(*Mouth).x=MouthCandidates[bestm].x/LocalScaleDown;
		(*Mouth).y=MouthCandidates[bestm].y/LocalScaleDown;

		*boLMouth=true;
		*boRMouth=true;

#ifdef DBGMode
	cvCircle(iplSelected,cvPoint((*Mouth).x,(*Mouth).y),2, CV_RGB(0,255,0),-1,8,0);
#endif

		//Si es el último índice y se obtuvo información del tracker, quiere decir que se seleccionó
		if (bestm==nMouthcandidates-1 && MouthTrackedAvailable)
		{
			//Toma los valores del tracker (posición 0)
			(*LMouth).x=LMouthCandidates[0].x/LocalScaleDown;
			(*LMouth).y=LMouthCandidates[0].y/LocalScaleDown;
			(*RMouth).x=RMouthCandidates[0].x/LocalScaleDown;
			(*RMouth).y=RMouthCandidates[0].y/LocalScaleDown;

#ifdef DBGMode
	cvCircle(iplSelected,cvPoint((*RMouth).x,(*RMouth).y),2, CV_RGB(0,255,0),-1,8,0);
	cvCircle(iplSelected,cvPoint((*LMouth).x,(*LMouth).y),2, CV_RGB(0,255,0),-1,8,0);
#endif
		}
		else//Lo estima en base a la detección de Viola
		{
			int TrackWidth=(((x2-x1)/LocalScaleDown)/16+1)*8;
			int my1=MouthCandidates[bestm].y-MouthHeight[bestm]/2;
			int my2=MouthCandidates[bestm].y+MouthHeight[bestm]/2;

			int tlmx=(int)(MouthCandidates[bestm].x-TrackWidth*0.2)/LocalScaleDown,
				tlmy=(int)(my1*0.65+my2*0.35)/LocalScaleDown,
				trmx=(int)(MouthCandidates[bestm].x+TrackWidth*0.2)/LocalScaleDown,
				trmy=(int)(my1*0.65+my2*0.35)/LocalScaleDown;

		
			//Obtiene la aproximación de la inclinación de la boca en relación a la inclinación que tenemos para los ojos
			if (bestl!=-1 && bestr!=-1)
			{
				//Gets angle and inbetween point
				float angulo;
				float interx,intery;
				angulo=(float)MathUtils.GetAngle4Rotation((*LEye).x,(*LEye).y,(*REye).x,(*REye).y,&interx,&intery);

				float px,py;

				px=(float)tlmx;
				py=(float)tlmy;

				MathUtils.Rota2D((float)MouthCandidates[bestm].x,(float)MouthCandidates[bestm].y,(float)angulo,&px,&py);
				tlmx=(int)(px+0.5f);
				tlmy=(int)(py+0.5f);

				px=(float)trmx;
				py=(float)trmy;

				MathUtils.Rota2D((float)MouthCandidates[bestm].x,(float)MouthCandidates[bestm].y,(float)angulo,&px,&py);
				trmx=(int)(px+0.5f);
				trmy=(int)(py+0.5f);
			}			

			(*LMouth).x=tlmx;
			(*LMouth).y=tlmy;
			(*RMouth).x=trmx;
			(*RMouth).y=trmy;		

			//Actualiza el patrón a seguir si fuera necesario
			//Si estamos siguiendo y no sólo detectando
			if (boUsaTrackers)
			{				
				//Si el tracker correspondiente está disponible
				if (DatosFaciales->Faces[i]->boLMouthTracker)
					DatosFaciales->Faces[i]->LMouthTracker->SetPattern(ima,(*LMouth).x,(*LMouth).y);
				else
				{
					//Aloja el tracker de la boca
					DatosFaciales->Faces[i]->LMouthTracker=new COpenTracking(ima,(*LMouth).x,(*LMouth).y,TrackWidth,TrackWidth,PATSIZEX,PATSIZEY);
					DatosFaciales->Faces[i]->boLMouthTracker=true;
				}
				
				//Si el tracker correspondiente está disponible
				if (DatosFaciales->Faces[i]->boRMouthTracker)
					DatosFaciales->Faces[i]->RMouthTracker->SetPattern(ima,(*RMouth).x,(*RMouth).y);
				else
				{
					//Aloja el tracker de la boca
					DatosFaciales->Faces[i]->RMouthTracker=new COpenTracking(ima,(*RMouth).x,(*RMouth).y,TrackWidth,TrackWidth,PATSIZEX,PATSIZEY);
					DatosFaciales->Faces[i]->boRMouthTracker=true;
				}
			}
		}
	}
	else
		*boMouth=false;

		
	if (nLEyecandidates)
	{
		delete [] LEyeCandidates;
		delete [] LEyeWidth;
		delete [] LEyeHeight;
	}
	if (nREyecandidates)
	{
		delete [] REyeCandidates;
		delete [] REyeHeight;
		delete [] REyeWidth;
	}
	if (nNosecandidates)
	{
		delete [] NoseCandidates;
		delete [] NoseWidth;		
		delete [] NoseHeight;
	}
	if (nMouthcandidates)
	{
		delete [] MouthCandidates;
		delete [] MouthWidth;
		delete [] MouthHeight;
	}
	if (nLMouthcandidates) delete [] LMouthCandidates;
	if (nRMouthcandidates) delete [] RMouthCandidates;
	
	
#ifdef DBGMode
	char cadena[128];
	sprintf(cadena,"%d/%d %d/%d %d/%d %d/%d",bestl,nLEyecandidates-1,bestr,nREyecandidates-1,bestn,nNosecandidates-1,bestm,nMouthcandidates-1);
	cvPutText(iplSelected,cadena,cvPoint(40,20),&stcvFont,CV_RGB(255,255,255));
	sprintf(cadena,"%s\\Frame%04d.jpg",cWorkDataDir,nProcesadas);
	cvSaveImage(cadena,iplSelected);
	cvReleaseImage(&iplSelected);
#endif

	cvReleaseImage(&imgdbg);
	cvReleaseImage(&imaR);
	cvReleaseImage(&ima2R);
}

/*! Dadas dos cordenadas de ojos, obtiene el error de reconstrucción PCA para cierta característica

	h	Image to use
	leye Left eye coordinates
	reye Right eye coordinates
	feature If not -1 computes the PCA recosntruction error of a single feature after normalization
			0 eye pair
			1 left eye
			2 right eye
			3 central face (QUOREX*QUOREY)
	pair, le,re,fq Punteros a imágenes extraídas
*/
float *CENCARA2_2Detector::GetPCAErrorforNormalizedCoordinates(IplImage *h,
							CvPoint leye, CvPoint reye,  int feature,
							IplImage* pair,IplImage* le,IplImage* re,IplImage* fq)
{
	IplImage *aux;
	static float PCAError[4];
	
	//Initialization
	for (int i=0;i<4;i++)
		PCAError[i]=1000000;

	IplImage *IfFNormalizada;

	//Gets normalized image for the eyes configuration
	IfFNormalizada=ImageUtils.GetNormalizedImageGivenEyes(h,leye.x,leye.y,reye.x,reye.y,FACEX,FACEY,LEFTEYEX,LEFTEYEY,RIGHTEYEX,RIGHTEYEY,true);//true para enmascarar

#ifdef DBGMODE
	char strdbg[256];
	sprintf(strdbg,"%s\\Normalizada.jpg",cWorkDataDir);
	cvSaveImage(strdbg,IfFNormalizada);
#endif

	//Extracts and compute PCA error

	int ms,ms2;

	//par de ojos
	if (feature==0 || feature==-1)
	{
		//EXTRACCIÓN OJOS
								
		//Dimensiones par ocular, implica luego al entrenamiento
		ms=AUTOOJOSX/2;
		ms2=(int)(AUTOOJOSX/1.2);
								
		//Utilizo un ROI
		if (ROIinImage(IfFNormalizada,LEFTEYEX-ms2,LEFTEYEY-ms,RIGHTEYEX+2*ms2+1-LEFTEYEX,2*ms+1))
		{
			cvSetImageROI(IfFNormalizada,cvRect(LEFTEYEX-ms2,LEFTEYEY-ms,RIGHTEYEX+2*ms2+1-LEFTEYEX,2*ms+1));	

			aux=cvCloneImage(IfFNormalizada);//¿Hace falta copiarlo?

			if (pair!=NULL)
				cvCopyImage(aux,pair);

//Depurando
#ifdef DBGMODE
char strdbg[256];
sprintf(strdbg,"%s\\NormalizadaPar.jpg",cWorkDataDir);
cvSaveImage(strdbg,aux);
#endif
//fin depurando


			//Position 0
			if (PCAEyePair->PCAspace)
				PCAError[0]=(float)PCAEyePair->PCAspace->GetPCAReconstructionError(aux);
			
			//Resetea el ROI
			cvResetImageROI(IfFNormalizada);
			cvReleaseImage(&aux);
		}
	}
								
	//Leye
	if (feature==1 || feature==-1)
	{
		int sxojo=16,syojo=12;
		ms=sxojo/2;
		int msy=syojo/2;

		//Utilizo un ROI
		if (ROIinImage(IfFNormalizada,LEFTEYEX-ms,LEFTEYEY-msy,2*ms2,2*msy))
		{
			cvSetImageROI(IfFNormalizada,cvRect(LEFTEYEX-ms,LEFTEYEY-msy,2*ms2,2*msy));	

			aux=cvCloneImage(IfFNormalizada);//¿Hace falta copiarlo?

			if (pair!=NULL)
				cvCopyImage(aux,le);

//Depurando
#ifdef DBGMODE
char strdbg[256];
sprintf(strdbg,"%s\\NormalizadaLE.jpg",cWorkDataDir);
cvSaveImage(strdbg,aux);
#endif
//fin depurando

			if (PCALEye->PCAspace)
				PCAError[1]=(float)PCALEye->PCAspace->GetPCAReconstructionError(aux);
			
			//Resetea el ROI
			cvResetImageROI(IfFNormalizada);
			cvReleaseImage(&aux);
		}
	}
	
	//Reye
	if (feature==2 || feature==-1)
	{
		int sxojo=16,syojo=12;
		ms=sxojo/2;
		int msy=syojo/2;

		//Utilizo un ROI
		if (ROIinImage(IfFNormalizada,RIGHTEYEX-ms,RIGHTEYEY-msy,2*ms2,2*msy))
		{
			cvSetImageROI(IfFNormalizada,cvRect(RIGHTEYEX-ms,RIGHTEYEY-msy,2*ms2,2*msy));	

			aux=cvCloneImage(IfFNormalizada);//¿Hace falta copiarlo?

			if (pair!=NULL)
				cvCopyImage(aux,re);

//Depurando
#ifdef DBGMODE
char strdbg[256];
sprintf(strdbg,"%s\\NormalizadaRE.jpg",cWorkDataDir);
cvSaveImage(strdbg,aux);
#endif
//fin depurando

			if (PCAREye->PCAspace)
				PCAError[2]=(float)PCAREye->PCAspace->GetPCAReconstructionError(aux);
			
			//Resetea el ROI
			cvResetImageROI(IfFNormalizada);
			cvReleaseImage(&aux);
		}
	}


	//PARTE central
	if (feature==3 || feature==-1)
	{
	
		//Utilizo un ROI
		cvSetImageROI(IfFNormalizada,cvRect((FACEX-FACEQUOREX)/2,(FACEY-FACEQUOREY)/2,FACEQUOREX, FACEQUOREY));	

		aux=cvCloneImage(IfFNormalizada);//¿Hace falta copiarlo?

		if (pair!=NULL)
				cvCopyImage(aux,fq);

//Depurando
#ifdef DBGMODE
char strdbg[256];
sprintf(strdbg,"%s\\NormalizadaQuore.jpg",cWorkDataDir);
cvSaveImage(strdbg,aux);
#endif
//fin depurando


		if (PCARepFaceQuore->PCAspace)
			PCAError[3]=(float)PCARepFaceQuore->PCAspace->GetPCAReconstructionError(aux);
			
		//Resetea el ROI
		cvResetImageROI(IfFNormalizada);
		cvReleaseImage(&aux);
	}

	cvReleaseImage(&IfFNormalizada);

	return PCAError;
}

bool CENCARA2_2Detector::ROIinImage(IplImage *iplimg,int x,int y, int sx, int sy)
{
	if (x<0) return false;
	if (x+sx>=iplimg->width) return false;

	if (y<0) return false;
	if (y+sy>=iplimg->height) return false;

	return true;
}

//Dadas dos cordenadas de ojos, obtiene para boca y nariz que les corresponderían
float CENCARA2_2Detector::GetLikelihoodforNormalizedCoordinates(CvPoint leye, CvPoint reye,
													   bool boNose,CvPoint nose,
													   bool boMouth,CvPoint mouth)
{
	//Gets angle and inbetween point
	float angulo;
	float interx,intery;
	angulo=(float)MathUtils.GetAngle4Rotation(leye.x,leye.y,reye.x,reye.y,&interx,&intery);

	//Depuración
	/*if (boDebugLog)
	{
		char cadena[512];
		sprintf(cadena,"%d,%d) Angulo= %f\n",interx,intery,angulo);
		DebugLog->WriteToLogFile(cadena);
	} */
				
	//Los ojos rotados serían
	//Convierte coordenadas
	CMathUtils	MathUtils;

	float oixr,oiyr,odxr,odyr,nxr,nyr,mxr,myr;
	int nxrs,nyrs,mxrs,myrs;

	float px,py;
	px=(float)leye.x;
	py=(float)leye.y;

	MathUtils.Rota2D((float)interx,(float)intery,(float)-angulo,&px,&py);
	oixr=px;
	oiyr=py;

	px=(float)reye.x;
	py=(float)reye.y;

	MathUtils.Rota2D((float)interx,(float)intery,(float)-angulo,&px,&py);
	odxr=px;
	odyr=py;

	//Calcula el ratio de escalado entre las normalizadas y los ojos rotados	
	//Too close?
	if (odxr-oixr<8) return -100;

	float	feyeratio=(odxr-oixr)/(float)(RIGHTEYEX-LEFTEYEX);

	

	//Gets the probability
	float likelihood=0.f,
		aux=0,		//Probabilidad a añadir
		auxpos=0,//Probabilidad de su imagen
		auxneg=0;//Probabilidad de la otra, si fuera mayor cancela

	//Si se ha proporcionado la nariz
	if (boNose)
	{
		px=(float)nose.x;
		py=(float)nose.y;

		MathUtils.Rota2D((float)interx,(float)intery,(float)-angulo,&px,&py);
		nxr=px;
		nyr=py;

		nxrs=(int)((float)(LEFTEYEX)+(nxr-oixr)/feyeratio+0.5f);
		nyrs=(int)((float)(LEFTEYEY)+(nyr-oiyr)/feyeratio+0.5f);

	/*	if (boDebugLog)
	{
		char cadena[512];
		sprintf(cadena,"Nariz=(%d,%d)\n",nxrs,nyrs);
		DebugLog->WriteToLogFile(cadena);
	} */

		if (nyrs>=0 && nyrs<FACEY && nxrs>=0 && nxrs<FACEX)
		{
			auxpos=(float)cvGetReal2D(NoseLikelihood,(int)nyrs,(int)nxrs);//Suma la de la nariz
			auxneg=(float)cvGetReal2D(MouthLikelihood,(int)nyrs,(int)nxrs);//Resta la de la boca
			
			aux=auxpos-auxneg;
		}
		else aux=-100;

		likelihood+=aux;
	}

	//Si se ha proporcionado la boca
	if (boMouth)
	{
		px=(float)mouth.x;
		py=(float)mouth.y;

		MathUtils.Rota2D((float)interx,(float)intery,(float)-angulo,&px,&py);
		mxr=px;
		myr=py;

		mxrs=(int)((float)(LEFTEYEX)+(mxr-oixr)/feyeratio+0.5f);
		myrs=(int)((float)(LEFTEYEY)+(myr-oiyr)/feyeratio+0.5f);

		/*if (boDebugLog)
	{
		char cadena[512];
		sprintf(cadena,"Boca=(%d,%d)\n",mxrs,myrs);
		DebugLog->WriteToLogFile(cadena);

	} */

		if (myrs>=0 && myrs<FACEY && mxrs>=0 && mxrs<FACEX)
		{
			auxpos=(float)cvGetReal2D(MouthLikelihood,(int)myrs,(int)mxrs);//Suma la de la nariz
			auxneg=(float)cvGetReal2D(NoseLikelihood,(int)myrs,(int)mxrs);//Resta la de la boca
			
			aux=auxpos-auxneg;
		}
		else aux=-100;

		likelihood+=aux;
	}

	return likelihood;
}



/*!
	Searches for candidates for those elements with a single tracker and a single detector

	ima Full resolution image
	ima2 Half resolution image (only available if requested when calling ApplyENCARA2)
	scale Indicates up to which scale down is available (1 only full resolution, 2 also half reslution)
*/
bool CENCARA2_2Detector::GetCandidates(IplImage *ima,IplImage *ima2,IplImage *imaR,IplImage *ima2R,
									   int scale,
									   float angulo, float interx, float intery,
									   bool boTracker, COpenTracking *Tracker,
									   bool boPatternAvailable, CViolaDetector *Detector,
									   int despx,int despy,
									   int roiw,int roih,int facesx,
									   int *nCandidates,long **WidthCandidates,
									   long **HeightCandidates, CvPoint **Candidates)
{
#ifdef DBGMode
	char strdbg[256],strdbg2[256];
#endif

	clock_t tdbg=clock();			
			
	
	

	bool boLost=true;	//Set means that the pattern was not tracked
	bool boTrackedTrustful=false;
	bool boTrackedVeryTrustful=false;
	bool boTrackedStored=false;//Si se estima que el seguido es válido, lo guarda
	*nCandidates=0;
	*Candidates=NULL;


	//TRACKING candidate
	//Tracks if the tracker is available
	if (boTracker && boPatternAvailable)
	{
		//Tracks pattern using full resolution

#ifdef DBGMode
		//Debug
		IplImage *imacp=cvCloneImage(ima);
		int px,py,sx,sy;
		Tracker->GetSearchArea(&px,&py,&sx,&sy);
		cvRectangle(imacp,cvPoint(px,py),cvPoint(px+sx,py+sy),CV_RGB(255,255,255), 2 );
		cvRectangle(imacp,cvPoint(despx,despy),cvPoint(despx+roiw,despy+roih),CV_RGB(255,0,0), 2 );
		IplImage *pat=Tracker->GetPattern(),*patch=cvCreateImage( cvSize(Tracker->Patx,Tracker->Paty), imacp->depth, imacp->nChannels );;
		cvSetImageROI(imacp,cvRect(0,0,Tracker->Patx,Tracker->Paty));
		cvCvtColor(pat,patch, CV_GRAY2BGR);
		cvCopyImage(patch,imacp);
		cvResetImageROI(imacp);
		cvReleaseImage(&pat);
		cvReleaseImage(&patch);
#endif
		//
		Tracker->Track(ima);
		boLost=Tracker->boLost;
		if (!boLost)
		{
			//One candidate is given by the tracker
			*nCandidates=1;

			//Is the tracked candidate reliable enough
			if (Tracker->Reliability>0.5)
			{
				boTrackedTrustful=true;
				if (Tracker->Reliability>=0.8)
					boTrackedVeryTrustful=true;
			}

			if (boDebugLog)
			{
				char cadena[512];
				sprintf(cadena,"Tracker encontró %f \n",Tracker->Reliability);
				DebugLog->WriteToLogFile(cadena);
			} 

#ifdef DBGMode
			//debug
			cvCircle(imacp,cvPoint(Tracker->x,Tracker->y),2, CV_RGB(0,255,0),-1,8,0);
			//
#endif
		}

#ifdef DBGMode
		//debug
		sprintf(strdbg2,"MinTracker %d      Reliability %f",Tracker->GetLastMin(),Tracker->Reliability);
		cvPutText(imacp,strdbg2,cvPoint(40,20),&stcvFont,CV_RGB(255,255,255));
		sprintf(strdbg,"%s\\Frame%04d_Tracker_%p.jpg",cWorkDataDir,nProcesadas,Tracker);
		cvSaveImage(strdbg,imacp);
		//cvNamedWindow(strdbg, 1);
		//cvShowImage(strdbg,imacp);
		cvReleaseImage(&imacp);
#endif
		//
	}

	//DETECTION CANDIDATE(S)
	//Image to search using Viola
	IplImage *ima4Viola;

	int lscaledown,lroiw,lroih,ldespx,ldespy;

	//Decide la escala que se utiliza para la búsqueda, dependiendo de las disponibles, y el tamaño del roi
	//utiliza la imagen de mayor resolución
	CvSize Csz=Detector->cascade->orig_window_size;//Cascade size

	//Si sólo se dispone de la imagen a resolución completa o la zona de búsqueda es ya pequeña en relación al patrón buscado
	if (scale==1 || !(roiw/scale>(Csz.width*2) && roih/scale>(Csz.height*2)))
	{
		if (imaR!=NULL)//Si hay imagen rotada la utiliza
			ima4Viola=imaR;//Mantiene resolución máxima disponible para buscar los patrones
		else			//No hay imagen rotada disponible
			ima4Viola=ima;

		lscaledown=1;//La escala es pues 1

		lroiw=roiw;
		lroih=roih;
		ldespx=despx;
		ldespy=despy;
	}
	else//Asumimos 2, no aceptamos otra posibilidad de momento
	{
		if (ima2R!=NULL)//Si hay imagen rotada
			ima4Viola=ima2R;//Usa resolución menor (si disponible)
		else
			ima4Viola=ima2;

		lscaledown=2;

		lroiw=roiw/lscaledown;
		lroih=roih/lscaledown;

		ldespx=despx/lscaledown;
		ldespy=despy/lscaledown;
	}
	
	//Search for objects using Viola based detector 
	CvSeq* seq=NULL;
	int nobjects=0;
	int lViolaScaleUp=1;



	//For big enough ROIs
	if (lroiw>Csz.width && lroih>Csz.height && !boTrackedVeryTrustful)
	{
		//Sets ROI according to previous detection information using the image in reduced resolution
		cvSetImageROI(ima4Viola,cvRect(ldespx,ldespy,lroiw,lroih));

		//
		//Para usar la imagen de gris con histograma ecualizado la búsqueda para realzar los contrastes
		IplImage *ima4Violagray=cvCreateImage( cvSize((int)(ima4Viola->roi->width),(int)(ima4Viola->roi->height)), ima4Viola->depth, 1 );

		cvCvtColor(ima4Viola,ima4Violagray, CV_BGR2GRAY );	

		//??
		cvEqualizeHist(ima4Violagray,ima4Violagray);

		IplImage *ima2Use=ima4Violagray;

		//Scales up the image if the roi is small in relation to the pattern to serach
		if ( (lroiw<Csz.width*2 || lroih<Csz.height*2)) //Si el roi es muy pequeño, y no estamos en un caso scaleup lo aumenta de tamaño para tener más posibilidades
		{
			lViolaScaleUp=2;//Prueba a aumentar la zona de tamaño

			ima2Use=cvCreateImage( cvSize((int)(ima4Violagray->width*2),(int)(ima4Violagray->height*2)), ima4Violagray->depth, ima4Violagray->nChannels );

			cvPyrUp(ima4Violagray,ima2Use);
		}

#ifdef DBGMode
		//debug
		IplImage *iplDets=cvCloneImage(ima2Use);
#endif
		
		//

		//Releases any previous
		cvClearMemStorage( ViolaStorage );

		if (boDebugLog)
	{
		char cadena[256];
		
		sprintf(cadena,"Antes buscar (%ld)\n",clock() - tdbg );
	
		DebugLog->WriteToLogFile(cadena);
	}

		float minfactor=((float)facesx/6.f)/(float)Detector->cascade->orig_window_size.width;
	
		//Setting the last parameter is faster
		//seq = cvHaarDetectObjects(ima2Use, Detector->cascade, ViolaStorage, 1.2);
		//probando a restringir el tamaño mínimo en base al ancho de la cara
		//seq = cvHaarDetectObjects(ima2Use, Detector->cascade, ViolaStorage, 1.2, 3, CV_HAAR_DO_CANNY_PRUNING,cvSize(int(factor*VDet->cascade->orig_window_size.width),(int)(factor*VDet->cascade->orig_window_size.height)) );
		seq = cvHaarDetectObjects(ima2Use, Detector->cascade, ViolaStorage, 1.2, 3, 0,cvSize(int(minfactor*Detector->cascade->orig_window_size.width),(int)(minfactor*Detector->cascade->orig_window_size.height)) );

		if (boDebugLog)
	{
		char cadena[256];
		
		sprintf(cadena,"Tras buscar (%ld)\n",clock() - tdbg );
	
		DebugLog->WriteToLogFile(cadena);
	}

		if (lViolaScaleUp>1) cvReleaseImage(&ima2Use);//Si escaló, libera

		cvReleaseImage(&ima4Violagray);
	
		if (seq)
		{
			nobjects=(seq ? seq->total : 0);

			//Determina el número a alojar si ha habido detecciones con Viola
			if (nobjects>0 && nobjects<50)
			{
				if (boTrackedTrustful)// If the tracked candidate is trustfull, adds it to the list
					*nCandidates+=nobjects;
				else				// Avoids the tracked if Viola detected and it is not trustful
					*nCandidates=nobjects;

				//Allocs memory for candidates
				*HeightCandidates=(long *)new unsigned char[*nCandidates*sizeof(long)];
				*Candidates=(CvPoint *)new unsigned char[*nCandidates*sizeof(CvPoint)];
				*WidthCandidates=(long *)new unsigned char[*nCandidates*sizeof(long)];
				

				//Saves detected objects
				for (int i=0;i<nobjects;i++)
				{
					CvRect r = *(CvRect*)cvGetSeqElem( seq, i );

					(*Candidates)[i].x=(int)(despx+((r.x+(r.width)*0.5)/lViolaScaleUp)*lscaledown);
					(*Candidates)[i].y=(int)(despy+((r.y+(r.height)*0.5)/lViolaScaleUp)*lscaledown);	
					(*WidthCandidates)[i]=(r.width/lViolaScaleUp)*lscaledown;
					(*HeightCandidates)[i]=(r.height/lViolaScaleUp)*lscaledown;

#ifdef DBGMode
					//debug pintando la zona de búsqueda
					cvRectangle(iplDets,cvPoint(r.x,r.y),cvPoint(r.x+r.width,r.y+r.height),CV_RGB(0,255,0), 2 );
					cvCircle(iplDets,cvPoint(r.x+r.width/2,r.y+r.height/2),2, CV_RGB(0,255,0),-1,8,0);
#endif
					//

					//If the points was located in the rotated image, the input coodinates are obtained
					if (imaR!=NULL)
					{
						float px,py;

						px=(float)(*Candidates)[i].x;
						py=(float)(*Candidates)[i].y;

						MathUtils.Rota2D((float)interx,(float)intery,(float)angulo,&px,&py);

						(*Candidates)[i].x=(int)px;
						(*Candidates)[i].y=(int)py;
					}

				}
			}			

			cvClearSeq(seq);
			seq=NULL;			
		}
		

#ifdef DBGMode
		//debug
		sprintf(strdbg,"%s\\Frame%04d_Detector_%p.jpg",cWorkDataDir,nProcesadas,Detector);
		cvSaveImage(strdbg,iplDets);
		
		cvReleaseImage(&iplDets);
#endif
	
		cvResetImageROI(ima4Viola);
		
	}//if ROI big enough	


	//Stores the tracked candidate if it is trustful, or just the last possibility (no one was found using Viola)
	if ( !boLost && (*Candidates==NULL || boTrackedTrustful) )
	{
		//Nothing has been stored, that means no allocation has been performed
		if (*Candidates==NULL)
		{
			if (*nCandidates)//But the tracked one is present
			{
				*Candidates=(CvPoint *)new unsigned char[*nCandidates*sizeof(CvPoint)];
				*WidthCandidates=(long *)new unsigned char[*nCandidates*sizeof(long)];
				*HeightCandidates=(long *)new unsigned char[*nCandidates*sizeof(long)];
			}
		}

		//Stores the tracked candidate in the last position
		(*Candidates)[*nCandidates-1].x=Tracker->x;
		(*Candidates)[*nCandidates-1].y=Tracker->y;
		(*WidthCandidates)[*nCandidates-1]=10;//??¿Qué tamaño le asigno si se siguen?
		(*HeightCandidates)[*nCandidates-1]=10;//??
		boTrackedStored=true;//Tra tracked candidate has been stored

		if (boDebugLog)
		{
			char cadena[512];
			sprintf(cadena,"Guardó el seguido \n");
			DebugLog->WriteToLogFile(cadena);
		} 
	}

	if (boTrackedStored)//Returns true if ona candidate is the tracke one
		return true;
	else
		return false;
}


/*!

	ima full resolution
	ima2 reduced (if requested)
	scale (indicates the reduced scale)
*/
bool CENCARA2_2Detector::GetCandidates(IplImage *ima,IplImage *ima2,IplImage *imaR,
									   IplImage *ima2R,int scale,
									   float angulo, float interx, float intery,
									   bool boLTracker,COpenTracking *LTracker,
									   bool boRTracker,COpenTracking *RTracker,
									   bool boPatternAvailable, CViolaDetector *Detector,
									   int despx,int despy,
									   int roiw,int roih,int facesx,
									   int *nLCandidates,CvPoint **LCandidates,
									   int *nRCandidates,CvPoint **RCandidates,
									   int *nCandidates,long **WidthCandidates,
									   long **HeightCandidates, CvPoint **Candidates)
{
#ifdef DBGMode
	char strdbg[128],strdbg2[64];
#endif

	bool boLLost=true,boRLost=true;	//Set means that the pattern was not tracked
	bool boLTrackedTrustful=false,boRTrackedTrustful=false;
	bool boLTrackedVeryTrustful=false,boRTrackedVeryTrustful=false;
	bool boTrackedStored=false;//Si se estima que el seguido es válido, lo guarda
	*nCandidates=0;
	*nLCandidates=0;
	*nRCandidates=0;

	//TRACKING candidate
	//Tracks if the left tracker is available
	if (boLTracker && boPatternAvailable)
	{
#ifdef DBGMode
		//Debug
		IplImage *imacp=cvCloneImage(ima);
		int px,py,sx,sy;
		//Pinta la zona de búsqueda del tracker
		LTracker->GetSearchArea(&px,&py,&sx,&sy);
		cvRectangle(imacp,cvPoint(px,py),cvPoint(px+sx,py+sy),CV_RGB(255,255,255), 2 );
		IplImage *pat=LTracker->GetPattern(),*patch=cvCreateImage( cvSize(LTracker->Patx,LTracker->Paty), imacp->depth, imacp->nChannels );;
		cvSetImageROI(imacp,cvRect(0,0,LTracker->Patx,LTracker->Paty));
		cvCvtColor(pat,patch, CV_GRAY2BGR);
		cvCopyImage(patch,imacp);
		cvResetImageROI(imacp);
		cvReleaseImage(&pat);
		cvReleaseImage(&patch);
#endif
		//Tracks the pattern using full resolution
		LTracker->Track(ima);
		boLLost=LTracker->boLost;
		if (!boLLost)
		{
			*nLCandidates=1;
			*LCandidates=(CvPoint *)new unsigned char[*nLCandidates*sizeof(CvPoint)];

			//Si hay bastante semejanza con el patrón lo tiene en cuenta
			if (LTracker->Reliability>0.5)
			{
				boLTrackedTrustful=true;
				if (LTracker->Reliability>0.8)
					boLTrackedVeryTrustful=true;
			}

			//Almacena tracked si disponible en la última posición
			(*LCandidates)[0].x=LTracker->x;
			(*LCandidates)[0].y=LTracker->y;

#ifdef DBGMode
			//debug
			cvCircle(imacp,cvPoint(LTracker->x,LTracker->y),2, CV_RGB(0,255,0),-1,8,0);
			//
#endif
		}

#ifdef DBGMode
		//debug
		sprintf(strdbg2,"MinTracker %d      Reliability %f",LTracker->GetLastMin(),LTracker->Reliability);
		cvPutText(imacp,strdbg2,cvPoint(40,20),&stcvFont,CV_RGB(255,255,255));
		sprintf(strdbg,"%s\\Frame%04d_%p.jpg",cWorkDataDir,nProcesadas,LTracker);
		cvSaveImage(strdbg,imacp);
		//cvNamedWindow(strdbg, 1);
		//cvShowImage(strdbg,imacp);
		cvReleaseImage(&imacp);
#endif

	}
	//Tracks if the right tracker is available
	if (boRTracker && boPatternAvailable)
	{
#ifdef DBGMode
		//Debug
		IplImage *imacp=cvCloneImage(ima);
		int px,py,sx,sy;
		RTracker->GetSearchArea(&px,&py,&sx,&sy);
		cvRectangle(imacp,cvPoint(px,py),cvPoint(px+sx,py+sy),CV_RGB(255,255,255), 2 );
		IplImage *pat=RTracker->GetPattern(),*patch=cvCreateImage( cvSize(RTracker->Patx,RTracker->Paty), imacp->depth, imacp->nChannels );;
		cvSetImageROI(imacp,cvRect(0,0,RTracker->Patx,RTracker->Paty));
		cvCvtColor(pat,patch, CV_GRAY2BGR);
		cvCopyImage(patch,imacp);
		cvResetImageROI(imacp);
		cvReleaseImage(&pat);
		cvReleaseImage(&patch);
#endif
		//Tracks the pattern using full resolution
		RTracker->Track(ima);
		boRLost=RTracker->boLost;
		if (!boRLost)
		{
			*nRCandidates=1;
			*RCandidates=(CvPoint *)new unsigned char[*nRCandidates*sizeof(CvPoint)];

			//Si hay bastante semejanza con el patrón lo tiene en cuenta
			if (RTracker->Reliability>0.5)
			{
				boRTrackedTrustful=true;
				if (RTracker->Reliability>0.8)
					boRTrackedVeryTrustful=true;
			}

			//Almacena tracked si disponible en la última posición
			(*RCandidates)[0].x=RTracker->x;
			(*RCandidates)[0].y=RTracker->y;

#ifdef DBGMode
			//debug
			cvCircle(imacp,cvPoint(RTracker->x,RTracker->y),2, CV_RGB(0,255,0),-1,8,0);
			//
#endif
		}
#ifdef DBGMode
		//debug
		sprintf(strdbg2,"MinTracker %d      Reliability %f",RTracker->GetLastMin(),RTracker->Reliability);
		cvPutText(imacp,strdbg2,cvPoint(40,20),&stcvFont,CV_RGB(255,255,255));
		sprintf(strdbg,"%s\\Frame%04d_%p.jpg",cWorkDataDir,nProcesadas,RTracker);
		cvSaveImage(strdbg,imacp);
		//cvNamedWindow(strdbg, 1);
		//cvShowImage(strdbg,imacp);
		cvReleaseImage(&imacp);
#endif
	}

	//Image to search using Viola
	IplImage *ima4Viola;
	int lscaledown,lroiw,lroih,ldespx,ldespy;

	//Decide la escala que se utiliza para la búsqueda, dependiendo de las disponibles, y el tamaño del roi
	//utiliza la imagen de mayor resolución
	CvSize Csz=Detector->cascade->orig_window_size;//Cascade size

	//Si sólo se dispone de la imagen a resolución completa o la zona de búsqueda es ya pequeña en relación al patrón buscado
	if (scale==1 || !(roiw/scale>(Csz.width*2) && roih/scale>(Csz.height*2)))
	{
		if (imaR!=NULL)//Si hay imagen rotada la utiliza
			ima4Viola=imaR;//Mantiene resolución máxima disponible para buscar los patrones
		else			//No hay imagen rotada disponible
			ima4Viola=ima;

		lscaledown=1;//La escala es pues 1

		lroiw=roiw;
		lroih=roih;
		ldespx=despx;
		ldespy=despy;
	}
	else//Asumimos 2, no aceptamos otra posibilidad de momento
	{
		if (ima2R!=NULL)//Si hay imagen rotada
			ima4Viola=ima2R;//Usa resolución menor (si disponible)
		else
			ima4Viola=ima2;

		lscaledown=2;

		lroiw=roiw/lscaledown;
		lroih=roih/lscaledown;

		ldespx=despx/lscaledown;
		ldespy=despy/lscaledown;
	}
	

	//Search for objects using Viola based detector 
	CvSeq* seq=NULL;
	int nobjects=0;
	int lViolaScaleUp=1;

	//For big enough ROIs
	if (lroiw>Csz.width && lroih>Csz.height && !(boRTrackedVeryTrustful && boLTrackedVeryTrustful))
	{
		//Sets ROI according to previous detection information using scale info
		cvSetImageROI(ima4Viola,cvRect(ldespx,ldespy,lroiw,lroih));

		//Para los otros candidatos aplico aquí una ecualización del hostograma en la zona a buscar
		
		IplImage *ima2Use=ima4Viola;

		if ((lroiw<Csz.width*2 || lroih<Csz.height*2)) //Si el roi es muy pequeño, y no estamos en un caso scaleup lo aumenta de tamaño para tener mñas posibilidades
		{
			lViolaScaleUp=2;//Prueba a escalarla

			ima2Use=cvCreateImage( cvSize((int)(ima4Viola->roi->width*2),(int)(ima4Viola->roi->height*2)), ima4Viola->depth, ima4Viola->nChannels );

			cvPyrUp(ima4Viola,ima2Use);
		}

#ifdef DBGMode
		//debug
		IplImage *iplDets=cvCloneImage(ima2Use);
#endif

		//Releases any previous
		cvClearMemStorage( ViolaStorage );

		float minfactor=((float)facesx/6.f)/(float)Detector->cascade->orig_window_size.width;
	
		//Setting the last parameter is faster
		//seq = cvHaarDetectObjects(ima2Use, Detector->cascade, ViolaStorage, 1.2);
		//probando a restringir el tamaño mínimo en base al ancho de la cara
		//seq = cvHaarDetectObjects(ima2Use, Detector->cascade, ViolaStorage, 1.2, 3, CV_HAAR_DO_CANNY_PRUNING,cvSize(int(factor*VDet->cascade->orig_window_size.width),(int)(factor*VDet->cascade->orig_window_size.height)) );
		seq = cvHaarDetectObjects(ima2Use, Detector->cascade, ViolaStorage, 1.2, 3, 0,cvSize(int(minfactor*Detector->cascade->orig_window_size.width),(int)(minfactor*Detector->cascade->orig_window_size.height)) );

		if (lViolaScaleUp>1) cvReleaseImage(&ima2Use);//Si escaló, libera
	
		if (seq)
		{
			nobjects=(seq ? seq->total : 0);

			//Si ambos trackers detectaron y parece fiable, hay un candidato más
			if (nobjects>0 && nobjects<50)
			{
				if (boLTrackedTrustful && boRTrackedTrustful)// Si los seguidos parecen fiables los tiene en cuenta
					*nCandidates+=nobjects;
				else			//Desecha el seguido si parece poco fiablke
					*nCandidates=nobjects;
			
				*Candidates=(CvPoint *)new unsigned char[*nCandidates*sizeof(CvPoint)];
				*WidthCandidates=(long *)new unsigned char[*nCandidates*sizeof(long)];
				*HeightCandidates=(long *)new unsigned char[*nCandidates*sizeof(long)];
			
				//Almacena datos si hubo detección ajustando en función de la ventana suministrada
			
				//Saves detected objects
				for (int i=0;i<nobjects;i++)
				{
					CvRect r = *(CvRect*)cvGetSeqElem( seq, i );

					(*Candidates)[i].x=(int)(despx+((r.x+(r.width)*0.5)/lViolaScaleUp)*lscaledown);
					(*Candidates)[i].y=(int)(despy+((r.y+(r.height)*0.5)/lViolaScaleUp)*lscaledown);	

					//Hay que deshacer la rotación
					if (imaR!=NULL)
					{
						float px,py;

						px=(float)(*Candidates)[i].x;
						py=(float)(*Candidates)[i].y;

						MathUtils.Rota2D((float)interx,(float)intery,(float)angulo,&px,&py);

						(*Candidates)[i].x=(int)px;
						(*Candidates)[i].y=(int)py;
					}

					(*WidthCandidates)[i]=(r.width/lViolaScaleUp)*lscaledown;
					(*HeightCandidates)[i]=(r.height/lViolaScaleUp)*lscaledown;

#ifdef DBGMode
					//debug
					cvRectangle(iplDets,cvPoint(r.x,r.y),cvPoint(r.x+r.width,r.y+r.height),CV_RGB(0,255,0), 2 );
					cvCircle(iplDets,cvPoint(r.x+r.width/2,r.y+r.height/2),2, CV_RGB(0,255,0),-1,8,0);
#endif
				}
			}			
			cvClearSeq(seq);
			seq=NULL;			
		}	
		
#ifdef DBGMode
		//debug
		sprintf(strdbg,"%s\\Frame%04d_D%p.jpg",cWorkDataDir,nProcesadas,Detector);
		cvSaveImage(strdbg,iplDets);
		
		cvReleaseImage(&iplDets);
#endif

		cvResetImageROI(ima4Viola);	
		
	}//if ROI big enough
	
	//Stores the tracked candidate if they both are trustful, or they are the only possibility, i.e. Viola did not fine anything
	if ( (!boLLost && !boRLost) && ( (boLTrackedTrustful && boRTrackedTrustful) || (*Candidates==NULL) ) )
	{
		//Nothing has been stored so far as a candidate
		if (*Candidates==NULL)
		{
			if (*nRCandidates && *nLCandidates)//Both mouth corners were tracked
			{
				*nCandidates=1;

				*Candidates=(CvPoint *)new unsigned char[*nCandidates*sizeof(CvPoint)];
				*WidthCandidates=(long *)new unsigned char[*nCandidates*sizeof(long)];
				*HeightCandidates=(long *)new unsigned char[*nCandidates*sizeof(long)];
			}
		}

		//heuristic offset obtained from tracked points
		int offsety=((*RCandidates)[0].x-(*LCandidates)[0].x)/10;
		(*Candidates)[*nCandidates-1].x=((*LCandidates)[0].x+(*RCandidates)[0].x)/2;
		(*Candidates)[*nCandidates-1].y=((*LCandidates)[0].y+(*RCandidates)[0].y)/2+offsety;
		(*WidthCandidates)[*nCandidates-1]=10;//??
		(*HeightCandidates)[*nCandidates-1]=10;//??

#ifdef DBGMode
	//debug  OJO que estás pimtando en iplDets offset
//	cvCircle(iplDets,cvPoint((*Candidates)[*nCandidates-1].x-despx,(*Candidates)[*nCandidates-1].y-despy),2, CV_RGB(255,0,0),-1,8,0);
//	cvCircle(iplDets,cvPoint((*LCandidates)[0].x-despx,(*LCandidates)[0].y-despy),2, CV_RGB(255,0,0),-1,8,0);
//	cvCircle(iplDets,cvPoint((*RCandidates)[0].x-despx,(*RCandidates)[0].y-despy),2, CV_RGB(255,0,0),-1,8,0);
#endif

		//The tracked candidate was stored in the last position
		boTrackedStored=true;
	}

	if (boTrackedStored)
		return true;
	else
		return false;

}

/******************************************************************************************************************

	Part of FindFaces in charge of finding facial elements

*******************************************************************************************************************/
void CENCARA2_2Detector::SearchFacialElements(int NumFaces)
{
	//Si hubo detección, busca los elementos faciales
	if (NumFaces)
	{
		//Temporal images
		IplImage *hh,*h,*h2;

		//Scale up related information
		int imagenescalada;
		bool	boPiramide;

		int OjosDetectados=0;
		int eyex1=0,eyey1=0,eyex2=0,eyey2=0,
			mlx=0,mly=0,mrx=0,mry=0,
			mouthx1=0,mouthy1=0,mouthx2=0,mouthy2=0,
			nosex1=0,nosey1=0,nosex2=0,nosey2=0,
			nx=0,ny=0;
                        
		//Busca elementos faciales para cada cara encontrada (luego tendrá en cuenta sólo las no redundantes)
		for(int i = 0; i <NumFaces; i++ )
		{
			bool boEyeContainerAvailable=false,boEyesAvailable=false,
				boLEyeAvailable=false,boREyeAvailable=false,
				boMouthAvailable=false,boMouthContainerAvailable=false,
				boNoseContainerAvailable=false,boNoseAvailable=false;

			//No normalized image available for this face
			DatosFaciales.Faces[i]->boNormalizedAvailable=false;

			//No pyramide applied by default
			boPiramide=false;
			imagenescalada=1; //Indicates if the image was scaled

			//If it was not a lost face we update its values
			if (DatosFaciales.Faces[i]->framestodie==-1)//Todo esto sobra si ya hemos hecho coherencia temporal
			{

				float ancho=(float)(DatosFaciales.Faces[i]->x2-DatosFaciales.Faces[i]->x1),
					alto=(float)(DatosFaciales.Faces[i]->y2-DatosFaciales.Faces[i]->y1);

				//Updates color for face detections (not for HS and FB)
				if (!DatosFaciales.Faces[i]->hs && !DatosFaciales.Faces[i]->fb && !DatosFaciales.Faces[i]->lb)
				{							
					//Learns color if it is not a gray image nor was detected using color
					if (!boImagenGrises && !DatosFaciales.Faces[i]->boColorBasedFound)
					{
						//Computes color for face i considering just an are in the center of the detection
						CalculaImagenColorNormalizado_Ventana(image,imageINr_norm,imageINg_norm,
								(int)(DatosFaciales.Faces[i]->x1+0.25*ancho),
								(int)(DatosFaciales.Faces[i]->y1+0.125*alto),
								(int)(ancho*0.5),
								(int)(alto*0.75));

						//Computes average color in that area and combines with previous value (if available)
						AprendeColor(imageINr_norm,imageINg_norm,
								(int)(DatosFaciales.Faces[i]->x1+0.25*ancho),
								(int)(DatosFaciales.Faces[i]->y1+0.125*alto),
								(int)(ancho*0.5),
								(int)(alto*0.75),
								&DatosFaciales.Faces[i]->rn_min,&DatosFaciales.Faces[i]->rn_max,
								&DatosFaciales.Faces[i]->gn_min,&DatosFaciales.Faces[i]->gn_max,
								&DatosFaciales.Faces[i]->boColorAlreadyInitialized);
					}
				}//Color update



#ifdef NewFeatures2
				//Were eyes detected by tracking?
				if (DatosFaciales.Faces[i]->eyestracked)
				{			
					boEyesAvailable=true;

					//Sets variables used to search other facial elements
					odx_in=DatosFaciales.Faces[i]->e_rx;
					ody_in=DatosFaciales.Faces[i]->e_ry;
					oix_in=DatosFaciales.Faces[i]->e_lx;
					oiy_in=DatosFaciales.Faces[i]->e_ly;


					if (DatosFaciales.Faces[i]->nosetracked)
					{
						boNoseAvailable=true;
						nx=DatosFaciales.Faces[i]->np_x;
						ny=DatosFaciales.Faces[i]->np_y;
					}

					if (DatosFaciales.Faces[i]->mouthtracked)
					{
						boMouthAvailable=true;
						mlx=DatosFaciales.Faces[i]->ml_x;
						mly=DatosFaciales.Faces[i]->ml_y;
						mrx=DatosFaciales.Faces[i]->mr_x;
						mry=DatosFaciales.Faces[i]->mr_y;
					}

				}
				else//Eyes were not detected by tracking, just try to detect them them
				{

					//Scales up the face area is seems to be necessary to have better resolution
					//Selects a region of interest to search facial elements
					cvSetImageROI(image,cvRect(DatosFaciales.Faces[i]->x1,DatosFaciales.Faces[i]->y1,
						(int)ancho,(int)alto));

					//Sería bueno ecualizar el histograma

					//Scales the window if it is smaller than a specific dimension
					if(ancho<90)
					{
						//Scales up
						h=cvCreateImage( cvSize((int)(ancho*2),(int)(alto*2)), image->depth, image->nChannels );
						boPiramide=true;
						cvPyrUp(image,h);
						hh=h;
						imagenescalada=2;

						//Checks if another scale up is necessary
						if(ancho<40)
						{
							h2=cvCreateImage( cvSize(hh->width*2,hh->height*2), hh->depth, hh->nChannels );
							cvPyrUp(hh,h2);
							hh=h2;
							cvReleaseImage(&h);
							imagenescalada=4;
						}
					}
					else //No scale was necessary
					{
						hh=cvCreateImage( cvSize((int)ancho,(int)alto), image->depth, image->nChannels );
						cvCopyImage(image,hh);
						boPiramide=true;
					}

					cvResetImageROI(image);
					//End scale up image
					

				//EN PRUEBAS
					bool boLEye,boREye,boNose,boMouth,boLMouth,boRMouth;
					CvPoint LEye,REye,Nose,Mouth,LMouth,RMouth;

					//Without using trackers
					SearchEyesMouthandNose(hh,NULL,&DatosFaciales,i,
						&boLEye,&LEye,&boREye,&REye,&boNose,&Nose,&boLMouth,&LMouth,&boRMouth,&RMouth,&boMouth,&Mouth,
						1,imagenescalada,false,false);

					//tipo initialization
					int tipo=NOFACE;	
					if (boLEye && boREye)
					{				
						//An appearance based test could be used
						float DistanciaEntreOjos;

						odx_in=DatosFaciales.Faces[i]->x1+REye.x;//Ya vienen en resolución adecuada
						ody_in=DatosFaciales.Faces[i]->y1+REye.y;
						oix_in=DatosFaciales.Faces[i]->x1+LEye.x;
						oiy_in=DatosFaciales.Faces[i]->y1+LEye.y;

						//Chequea apariencia o quizás es excesivo
						tipo=TestAparienciaParaParOjos(imagegray,imagegrayIN,
									&odx_in,&ody_in,&oix_in,&oiy_in,
									&DistanciaEntreOjos,
									UMBRALSUAVE,false,false,false);

						//Si no se acepta desprecio la posible rotación hecha 
						if (tipo==POSIBLEFRONTAL)
							tipo=FRONTAL;
						
						tipo=FRONTAL;
					
					}
					if (tipo==FRONTAL)
					{
						
						boEyesAvailable=true;

						
					}
					else//Busca par ocular
					{
						OjosDetectados=0;
						if (boDetectaElementosFacialesconViola)
						{
							//Own detector
							BuscaParOjosconViola(ViolaEyePairDetector->cascade,
								hh,0,0,hh->width,(int)(hh->height*0.6),
								&OjosDetectados,&eyex1,&eyey1,&eyex2,&eyey2);

						}

						
						//If eye pair was detected, tries to find eyes using ENCARA and Viola based
						if (OjosDetectados)
						{
							//Ajusta la posición del par ocular a la escala y la ventana
							eyex1=DatosFaciales.Faces[i]->x1+eyex1/imagenescalada;
							eyey1=DatosFaciales.Faces[i]->y1+eyey1/imagenescalada;
							eyex2=DatosFaciales.Faces[i]->x1+eyex2/imagenescalada;
							eyey2=DatosFaciales.Faces[i]->y1+eyey2/imagenescalada;

							boEyeContainerAvailable=true;
						}
					}

					if (boNose)
					{
						boNoseAvailable=true;
						nx=DatosFaciales.Faces[i]->x1+Nose.x;
						ny=DatosFaciales.Faces[i]->y1+Nose.y;
					}

					//Si parece haberlo encontrado
					if (boMouth)
					{
						//
						boMouthAvailable=true;

						//Ajusta la posición de la boca a la escala y la ventana
						boMouthAvailable=true;//Hay que sumar ROI ??
						mlx=DatosFaciales.Faces[i]->x1+LMouth.x;
						mly=DatosFaciales.Faces[i]->y1+LMouth.y;
						mrx=DatosFaciales.Faces[i]->x1+RMouth.x;
						mry=DatosFaciales.Faces[i]->y1+RMouth.y;
					}
 
				}//eyestracker
#else
				if (!DatosFaciales.Faces[i]->lb)
				{
				//Were eyes detected by tracking? In that case the other elements have been serched too. Do not do that again
					if (DatosFaciales.Faces[i]->eyestracked
#ifdef VALECONUNJOJOYLABOCA
					|| ( (DatosFaciales.Faces[i]->leyetracked || DatosFaciales.Faces[i]->reyetracked) && DatosFaciales.Faces[i]->mouthtracked)
#endif
					)
					{		
						//Both available?
						if (DatosFaciales.Faces[i]->eyestracked) boEyesAvailable=true;

						//Sets variables used to search other facial elements
						if (DatosFaciales.Faces[i]->eyestracked || DatosFaciales.Faces[i]->reyetracked)
						{
							boREyeAvailable=true;
							odx_in=DatosFaciales.Faces[i]->e_rx;
							ody_in=DatosFaciales.Faces[i]->e_ry;
						}
						if (DatosFaciales.Faces[i]->eyestracked || DatosFaciales.Faces[i]->leyetracked)
						{
							boLEyeAvailable=true;
							oix_in=DatosFaciales.Faces[i]->e_lx;
							oiy_in=DatosFaciales.Faces[i]->e_ly;
						}

						//Nariz
						if (DatosFaciales.Faces[i]->nosetracked)
						{
							boNoseAvailable=true;
							nx=DatosFaciales.Faces[i]->np_x;
							ny=DatosFaciales.Faces[i]->np_y;
						}

						if (DatosFaciales.Faces[i]->mouthtracked)
						{
							boMouthAvailable=true;
							mlx=DatosFaciales.Faces[i]->ml_x;
							mly=DatosFaciales.Faces[i]->ml_y;
							mrx=DatosFaciales.Faces[i]->mr_x;
							mry=DatosFaciales.Faces[i]->mr_y;
						}
					}
					else//Eyes were not detected by tracking, just try to find them
					{
						//Scales up the face area is seems to be necessary to have better resolution
						//Selects a region of interest to search facial elements
						//image color, imagegray grises
						cvSetImageROI(imagegray,cvRect(DatosFaciales.Faces[i]->x1,DatosFaciales.Faces[i]->y1,
							(int)ancho,(int)alto));

						//Scales the window if it is smaller than a specific dimension
						if(ancho<90)
						{
							//Scales up
							h=cvCreateImage( cvSize((int)(ancho*2),(int)(alto*2)), imagegray->depth, imagegray->nChannels );
							boPiramide=true;
							cvPyrUp(imagegray,h);
							cvEqualizeHist(h,h);//¿Puedo usar la misma de entrada y salida?
							hh=h;
							imagenescalada=2;

							//Checks if another scale up is necessary
							if(ancho<40)
							{
								h2=cvCreateImage( cvSize(hh->width*2,hh->height*2), hh->depth, hh->nChannels );
								cvPyrUp(hh,h2);
								hh=h2;
								cvEqualizeHist(hh,hh);//¿Puedo usar la misma de entrada y salida?
								cvReleaseImage(&h);
								imagenescalada=4;
							}
						}
						else //No scale was necessary
						{
							hh=cvCreateImage( cvSize((int)ancho,(int)alto), imagegray->depth, imagegray->nChannels );
							//cvCopyImage(image,hh);
							cvEqualizeHist(imagegray,hh);
							boPiramide=true;
						}

						cvResetImageROI(imagegray);
						//End scale up image
						

						//tipo initialization
						int tipo=NOFACE;	

						//Searches the eye patterns stored if available
						int retprev=-1;
						float fdist=0.f;

						//Si localizó los ojos por seguimiento sus coordenadas están en corr_odx,corr_ody,corr_oix,corr_oiy
						if (retprev==POSIBLEFRONTAL)
						{
							/*DatosFaciales.Faces[i]->eyestracked=true;
							//Color detection flag
							DatosFaciales.Faces[i]->hs=false;
							DatosFaciales.Faces[i]->fb=false;
							DatosFaciales.Faces[i]->lb=false;

							DatosFaciales.Faces[i]->reye=true;
							DatosFaciales.Faces[i]->leye=true;

							//Color detection flag
							DatosFaciales.Faces[i]->boColorBasedFound=false;
							//Tracking flag
							DatosFaciales.Faces[i]->tracked=false;

							//Coordenadas de ojos  machaca las previas
							DatosFaciales.Faces[i]->e_dx=corr_odx;
							DatosFaciales.Faces[i]->e_dy=corr_ody;
							DatosFaciales.Faces[i]->e_ix=corr_oix;
							DatosFaciales.Faces[i]->e_iy=corr_oiy;

							//Not lost
							DatosFaciales.Faces[i]->framestodie=-1;

							//recuadro en base a ojos seguidos
							//Determina el ojo màs alto por si la cara està inclinada
							int eyeoffy,eyeoffxl=0,eyeoffxr=0,yval=corr_oiy;

							if (corr_ody<yval)
							{
								yval=corr_ody;
								eyeoffxl=(int)((corr_ody+corr_oiy)*0.5-yval);
							}
							else
								eyeoffxr=(int)((corr_ody+corr_oiy)*0.5-yval);

							eyeoffy=corr_oiy-yval;

							DatosFaciales.Faces[i]->x1=(int)(corr_oix-fdist*0.5-eyeoffxr);
							DatosFaciales.Faces[i]->y1=(int)(corr_oiy-eyeoffy-fdist*0.5);
							DatosFaciales.Faces[i]->x2=DatosFaciales.Faces[i]->x1+(int)(fdist*2.0+eyeoffxr+eyeoffxl);
							DatosFaciales.Faces[i]->y2=DatosFaciales.Faces[i]->y1+(int)(fdist*2.25+eyeoffy);

							IplUtils.WindowTestP(imagegrayIN,&DatosFaciales.Faces[i]->x1,&DatosFaciales.Faces[i]->y1,
									&DatosFaciales.Faces[i]->x2,&DatosFaciales.Faces[i]->y2);

							//Eyes are available
							boEyesAvailable=true;

							//Usadas para buscar la boca
							odx_in=DatosFaciales.Faces[i]->e_dx;
							ody_in=DatosFaciales.Faces[i]->e_dy;
							oix_in=DatosFaciales.Faces[i]->e_ix;
							oiy_in=DatosFaciales.Faces[i]->e_iy;*/

						}
						//Fin
						else
						{ //If eyes were not found once more correlating			

							//Employes ENCARA using skin color blob
							if (!boImagenGrises && boSearchEyesusingGray)//Busca los ojos por mínimos de grises
								tipo=BuscaOjosUsandoENCARA(i);
						
							//Eyes detected must be inside a window
							if (tipo==FRONTAL)
							{
								if ( !MathUtils.PuntoEnVentana(odx_in,ody_in,
									DatosFaciales.Faces[i]->x1,DatosFaciales.Faces[i]->y1,
									DatosFaciales.Faces[i]->x2,DatosFaciales.Faces[i]->y2)
									||  !MathUtils.PuntoEnVentana(oix_in,oiy_in,
									DatosFaciales.Faces[i]->x1,DatosFaciales.Faces[i]->y1,
									DatosFaciales.Faces[i]->x2,DatosFaciales.Faces[i]->y2) )
									tipo=NOFACE;//No están en la ventana
							}

							//Si encontró los ojos estarán en odx_in,ody_in,oix_in,oiy_in
							if (tipo==FRONTAL)
							{
								boEyesAvailable=true;		
							}
							else //Makes use of other techniques (e.g. Viola based) to find the eyes
							{								
								//initializes tipo
								tipo=APARIENCIARARA;

								//Uses Viola based search in the region of interest
								if (boDetectaElementosFacialesconViola)
								{
									//No a priori detection
									int ojosencontrados=0;
																								
									float slope=(float)0.4;
									bool boRepite=true;
									do {
										ojosencontrados=BuscaOjosconViola(ViolaRightEyeDetector->cascade,
											ViolaLeftEyeDetector->cascade,
											hh,	0,(int)(hh->height*0.1),
												hh->width-1,(int)(hh->height*(1-slope)),
												&odx_in,&ody_in,&oix_in,&oiy_in);

										//Ojos con clasificador uni munich
#ifndef ENCARA2INLINUX
										/*if (!ojosencontrados)
											ojosencontrados=BuscaOjosconViola(ViolaUMunichEyeDetector->cascade,
												ViolaUMunichEyeDetector->cascade,
												hh,0,(int)(hh->height*0.1),
												hh->width-1,(int)(hh->height*(1-slope)),
												&odx_in,&ody_in,&oix_in,&oiy_in);*/
#endif
										
										//Si los ojos detectados son muy dispares en y, 
										//repite la búsqueda algo más restringida en y
										//para evitar confusión con la boca
										if (ojosencontrados && slope<0.7)
										{
											if (abs(ody_in-oiy_in)>(int)((odx_in-oix_in)*0.3))
											{
												boRepite=true;
												slope+=(float)0.1;
											}
											else
												boRepite=false;
										}
										else
											boRepite=false;
									}while (boRepite);


									//Checks if eyes were found
									if (ojosencontrados)
									{	
										//An appearance based test could be used
										float DistanciaEntreOjos;

										//Chequea apariencia o quizás es excesivo
										tipo=TestAparienciaParaParOjos(imagegray,imagegrayIN,
													&odx_in,&ody_in,&oix_in,&oiy_in,
													&DistanciaEntreOjos,
													UMBRALSUAVE,false,false,false);

										//Si no se acepta desprecio la posible rotación hecha 
										if (tipo==POSIBLEFRONTAL)
											tipo=FRONTAL;
										
										tipo=FRONTAL;
									}
								}

								//If eyes were located, updates
								if (tipo==FRONTAL)
								{
									//Ajusta la posición de los ojos a la escala y la ventana
									odx_in=DatosFaciales.Faces[i]->x1+odx_in/imagenescalada;
									ody_in=DatosFaciales.Faces[i]->y1+ody_in/imagenescalada;
									oix_in=DatosFaciales.Faces[i]->x1+oix_in/imagenescalada;
									oiy_in=DatosFaciales.Faces[i]->y1+oiy_in/imagenescalada;

									boEyesAvailable=true;
								}
								else //Last trial using eye pair detection, only if the face was not found using tracking or color
								{
									OjosDetectados=0;
									if (boDetectaElementosFacialesconViola //&& !DatosFaciales.Faces[i]->boColorBasedFound && !DatosFaciales.Faces[i]->tracked
										)
									{
										//Own detector
										BuscaParOjosconViola(ViolaEyePairDetector->cascade,
											hh,0,0,hh->width,(int)(hh->height*0.6),
											&OjosDetectados,&eyex1,&eyey1,&eyex2,&eyey2);

									}

									
									//If eye pair was detected, tries to find eyes using ENCARA and Viola based
									if (OjosDetectados)
									{
										
										//Viola based
										int ojosencontrados=0;
										
										float anchoeyes=(float)(eyex2-eyex1);
										float altoeyes=(float)(eyey2-eyey1);

										
										ojosencontrados=BuscaOjosconViola(ViolaRightEyeDetector->cascade,
											ViolaLeftEyeDetector->cascade,hh,
											(int)(eyex1-anchoeyes*0.4),(int)(eyey1-altoeyes*0.6),
											(int)(eyex2+anchoeyes*0.4),(int)(eyey2+altoeyes*0.6),
											&odx_in,&ody_in,&oix_in,&oiy_in);

										
#ifndef ENCARA2INLINUX
										/*if (!ojosencontrados)
											ojosencontrados=BuscaOjosconViola(ViolaUMunichEyeDetector->cascade,
												ViolaUMunichEyeDetector->cascade,hh,
												(int)(eyex1-anchoeyes*0.4),(int)(eyey1-altoeyes*0.6),
												(int)(eyex2+anchoeyes*0.4),(int)(eyey2+altoeyes*0.6),
												&odx_in,&ody_in,&oix_in,&oiy_in);*/
#endif

										//If eyes were found
										if (ojosencontrados)
										{
											
											tipo=APARIENCIARARA;

											//Ajusta la posición de los ojos a la escala y la ventana
											odx_in=DatosFaciales.Faces[i]->x1+odx_in/imagenescalada;
											ody_in=DatosFaciales.Faces[i]->y1+ody_in/imagenescalada;
											oix_in=DatosFaciales.Faces[i]->x1+oix_in/imagenescalada;
											oiy_in=DatosFaciales.Faces[i]->y1+oiy_in/imagenescalada;

											//Appearance could be testes
											float DistanciaEntreOjos;

											//Chequea apariencia o quizás es excesivo
											tipo=TestAparienciaParaParOjos(imagegray,imagegrayIN,
												&odx_in,&ody_in,&oix_in,&oiy_in,
												&DistanciaEntreOjos,
												UMBRALSUAVE,false,false,false);
											tipo=POSIBLEFRONTAL;

				
											//Si no se acepta desprecio la posible rotación hecha 
											if (tipo==POSIBLEFRONTAL)
											{
												tipo=FRONTAL;										
											}

											//Tests if eyes are inside the window
											if ( !MathUtils.PuntoEnVentana(odx_in,ody_in,
												DatosFaciales.Faces[i]->x1,DatosFaciales.Faces[i]->y1,
												DatosFaciales.Faces[i]->x2,DatosFaciales.Faces[i]->y2)
												||  !MathUtils.PuntoEnVentana(oix_in,oiy_in,
												DatosFaciales.Faces[i]->x1,DatosFaciales.Faces[i]->y1,
												DatosFaciales.Faces[i]->x2,DatosFaciales.Faces[i]->y2) )
												tipo=NOFACE;
											else
												tipo=FRONTAL;
										}
					

										//If no success, uses color based detection (ENCARA)
										if (tipo!=FRONTAL && boSearchEyesusingGray)
										{
											
											//La zona de búsqueda será relativa al tamaño del contenedor de l acara
											int off=(int)((float)(DatosFaciales.Faces[i]->x2-DatosFaciales.Faces[i]->x1)*0.2);
											//Translates eyepair coordinates as they could be searched at higher resolution
											tipo=BuscaCaracteristicasFacialessinColor(imagegray,imagegrayIN,
												DatosFaciales.Faces[i]->x1+eyex1/imagenescalada-off,DatosFaciales.Faces[i]->y1+eyey1/imagenescalada,
												DatosFaciales.Faces[i]->x1+eyex2/imagenescalada+off,DatosFaciales.Faces[i]->y1+eyey2/imagenescalada);

											//Eyes must be inside a window
											if (tipo==FRONTAL)
											{
												if ( !MathUtils.PuntoEnVentana(odx_in,ody_in,
													DatosFaciales.Faces[i]->x1,DatosFaciales.Faces[i]->y1,
													DatosFaciales.Faces[i]->x2,DatosFaciales.Faces[i]->y2)
													||  !MathUtils.PuntoEnVentana(oix_in,oiy_in,
													DatosFaciales.Faces[i]->x1,DatosFaciales.Faces[i]->y1,
													DatosFaciales.Faces[i]->x2,DatosFaciales.Faces[i]->y2) )
													tipo=NOFACE;
											}
										}
								
										//Finalmente almaceno si procede
										if (tipo==FRONTAL) //Detectó los ojos y el par ocular
										{
											boEyesAvailable=true;
										}
										else //sólo detectó el par ocular
										{
											//Ajusta la posición del par ocular a la escala y la ventana
											eyex1=DatosFaciales.Faces[i]->x1+eyex1/imagenescalada;
											eyey1=DatosFaciales.Faces[i]->y1+eyey1/imagenescalada;
											eyex2=DatosFaciales.Faces[i]->x1+eyex2/imagenescalada;
											eyey2=DatosFaciales.Faces[i]->y1+eyey2/imagenescalada;

											boEyeContainerAvailable=true;
										}
									}
								}//cierra else par ocular
								
							}//cierra else de otras técnicas
						}//If correla ojos si ya están disponibles

					

						//Si no ha seguido tiene sentido buscar los demás elementos
					
						//Computes an estimation of eye distance to filter bad mouth and nose detections
						float DistanciaInterocular;
						int interx,intery;
						if (boEyesAvailable || boEyeContainerAvailable)
						{
							if (boEyesAvailable)
							{
								DistanciaInterocular=MathUtils.DistanciaEuclidea(odx_in,ody_in,oix_in,oiy_in)*imagenescalada;
								interx=((odx_in+oix_in)/2-DatosFaciales.Faces[i]->x1)*imagenescalada;
								intery=((ody_in+oiy_in)/2-DatosFaciales.Faces[i]->y1)*imagenescalada;
							}
							else
							{
								DistanciaInterocular=(float)((eyex2-eyex1)*imagenescalada);
								interx=((eyex1+eyex2)/2-DatosFaciales.Faces[i]->x1)*imagenescalada;
								intery=((eyey1+eyey2)/2-DatosFaciales.Faces[i]->y1)*imagenescalada;
							}
						}

						
						//Ojo, busca la boca y la nariz pero no usa la imagen rotada para encontrar con más posibilidades

						//BOCA
						if (DatosFaciales.Faces[i]->mouthtracked && boEyesAvailable)// la acepta si se han localizado los ojos
						{
							boMouthAvailable=true;
							mlx=DatosFaciales.Faces[i]->ml_x;
							mly=DatosFaciales.Faces[i]->ml_y;
							mrx=DatosFaciales.Faces[i]->mr_x;
							mry=DatosFaciales.Faces[i]->mr_y;
						}
						else
						{
	#ifdef NewFeatures
							if (!DatosFaciales.Faces[i]->eyestracked)
							{
	#endif
							//Search the mouth only if eyes or their container are available
							if ((boEyesAvailable || boEyeContainerAvailable) && boSearchMouth)
							{
								//No a priori detection
								int bocaencontrada=0;

								int posx,posy,sx,sy,auxpos,auxsize;
								posx=(int)(hh->width*0.1);
								posy=(int)(intery+DistanciaInterocular*0.65);
								sx=(int)(hh->width*0.8);
								sy=(int)(DistanciaInterocular*1.3);

								//Ajusta zona de búsqueda si localizó la nariz
								if (DatosFaciales.Faces[i]->nosetracked)
								{
									auxpos=(DatosFaciales.Faces[i]->np_y-DatosFaciales.Faces[i]->y1)*imagenescalada;
									auxsize=posy+sy-auxpos;
									posy=auxpos;
									sy=auxsize;
								}						
								//

								IplUtils.WindowTest(hh,&posx,&posy,&sx,&sy);

								bocaencontrada=BuscaBocaconViola(hh,
									posx,posy,sx,sy,
									&mouthx1,&mouthy1,&mouthx2,&mouthy2,
									DistanciaInterocular,interx,intery);

								//Checks if eyes were found
								if (bocaencontrada)
								{		
									boMouthContainerAvailable=true;

									//Ajusta la posición de laboca a la escala y la ventana
									mouthx1=DatosFaciales.Faces[i]->x1+mouthx1/imagenescalada;
									mouthy1=DatosFaciales.Faces[i]->y1+mouthy1/imagenescalada;
									mouthx2=DatosFaciales.Faces[i]->x1+mouthx2/imagenescalada;
									mouthy2=DatosFaciales.Faces[i]->y1+mouthy2/imagenescalada;
								}						
							}
		#ifdef NewFeatures
							}
		#endif
						}
						
						if (boDebugLog)
						{
							char cadena[512];
							sprintf(cadena,"A buscar narix %d %d %d %d %d\n",i,DatosFaciales.Faces[i]->x1,DatosFaciales.Faces[i]->y1,
							(int)ancho,(int)alto);
							DebugLog->WriteToLogFile(cadena);
						}
						
						//Nariz
						if (DatosFaciales.Faces[i]->nosetracked && boEyesAvailable)// la acepta si se han localizado los ojos
						{
							boNoseAvailable=true;
							nx=DatosFaciales.Faces[i]->np_x;
							ny=DatosFaciales.Faces[i]->np_y;
						}
						else
						{
		#ifdef NewFeatures
							if (!DatosFaciales.Faces[i]->eyestracked)
							{
		#endif
							//Search the nose only if eyes are available
							if ((boEyesAvailable || boEyeContainerAvailable) && boSearchNose)
							{
								//No a priori detection
								int bonarizencontrada=0;

								int posx,posy,sx,sy,auxpos,auxsize;

								//Likely nose location relative to eyes
								posx=(int)(hh->width*0.2);
								posy=(int)(intery);
								sx=(int)(hh->width*0.6);
								sy=(int)(DistanciaInterocular);//*0.8);

								//If the eyes were tracked, the lilely location is modified
								if (DatosFaciales.Faces[i]->eyestracked)
								{
									auxpos=((ody_in+oiy_in)/2-DatosFaciales.Faces[i]->y1)*imagenescalada;
									auxsize=posy+sy-auxpos;
									posy=auxpos;
									sy=auxsize;

								}
								//Also if the mouth was found
								if (DatosFaciales.Faces[i]->mouthtracked)
								{
									sy=(DatosFaciales.Faces[i]->mr_y+DatosFaciales.Faces[i]->ml_y)/2-posy;								
								}
								//
								IplUtils.WindowTest(hh,&posx,&posy,&sx,&sy);
								
								bonarizencontrada=BuscaNarizconViola(hh,
									posx,posy,sx,sy,
									&nosex1,&nosey1,&nosex2,&nosey2,
									DistanciaInterocular,interx,intery);

								//Checks if the nose was found
								if (bonarizencontrada)
								{					
									boNoseContainerAvailable=true;

									//Ajusta la posición de la nariz a la escala y la ventana
									nosex1=DatosFaciales.Faces[i]->x1+nosex1/imagenescalada;
									nosey1=DatosFaciales.Faces[i]->y1+nosey1/imagenescalada;
									nosex2=DatosFaciales.Faces[i]->x1+nosex2/imagenescalada;
									nosey2=DatosFaciales.Faces[i]->y1+nosey2/imagenescalada;
								}
							}
		#ifdef NewFeatures
							}
		#endif
						}

						//Estimate Face container from eyes location
						if (boEyesAvailable)		
							EstimateFacefromEyes(&DatosFaciales,i,oix_in,oiy_in,odx_in,ody_in);	

					
					}//else de if (DatosFaciales.Faces[i]->eyestracked)

				}//si no es una detección de piernas


#endif

				if (boDebugLog)
				{
					char cadena[256];
					sprintf(cadena,"Antes actualizar cara\n");

					DebugLog->WriteToLogFile(cadena);
				}

				//Updates facial features (eyes) data according to detection
				//If success boEyeContainerAvailable and boFeaturesAvailable
				//contains data
				ActualizaDatosCarasenImagen(image,imagegrayIN,&DatosFaciales,i,
						eyex1,eyey1,eyex2,eyey2,
						odx_in,ody_in,oix_in,oiy_in,										
						mouthx1,mouthy1,mouthx2,mouthy2,
						mlx,mly,mrx,mry,
						nosex1,nosey1,nosex2,nosey2,
						nx,ny,
						boEyeContainerAvailable,boEyesAvailable,
						boLEyeAvailable,boREyeAvailable,
						boMouthContainerAvailable,boMouthAvailable,
						boNoseContainerAvailable,boNoseAvailable);
				
				//Libero la imagen agrandada si se alojó
				if (boPiramide)
				{
					boPiramide=false;
					cvReleaseImage(&hh);
				}

			}//Not lost face

		}//for

	}
}
