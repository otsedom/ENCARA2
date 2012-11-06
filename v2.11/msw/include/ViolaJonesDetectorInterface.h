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

	Interface required to process using the implementation of the Viola-Jones
	framework for object detection. The OpenCV implementation was developed by Lienhart.


	Modesto Castrillón Santana
	Septiembre 2003

****************************************************************************************/
#if ! defined ( VIOLAINTERFACE_H )
#define VIOLAINTERFACE_H

#include <time.h>
#include <stdio.h>

//#include "cvhaartraining.h"

//Estructura de datos faciales
//#include "FacialData.h" 

//MathUtils
#include "MathUtils.h"

//#include "DebugLog.h"//útil sólo para depurar

//!	Interface to Viola-Jones bases detectors
/*!
	Interface required to process using the implementation of the Viola-Jones
	framework for object detection. The OpenCV implementation was developed by Lienhart.
*/
class CViolaDetector  
{

public:
	//! Class constructor
	/*!
	\param	ClasifDir	Directorio que contiene el detector (NULL se corresponde con el detector facial por defecto
	\param sx	Width of patterns to be searched
	\param sy	Height of patterns to be searched
	\param boPruning If set reduces the search, recommended for face detection
	\param bofromDirectory	If set the classifier is given using a directory
	\param cDebug	If not null contains the identifier of the debug file
	*/
	CViolaDetector(char *ClasifDir,int sx,int sy, bool boPruning=false,bool bofromDirectory=false)
	{
		//Inicializaciones
		boDetectorUsed=false;
		boCascada=false;
		NumObjects=0;

		cascade=0;

		//Inicializa el clasificador
		InitViolaDetector(ClasifDir, &cascade,sx,sy,bofromDirectory);//No controlamos si da error en cuyo caso devuelve NULL

		boCannyPruning=boPruning;//if true makes use of contours, which is in curent OpenCV version optimized for faces

		boDepura=false;
	}

	//! Class constructor
	/*!
	\param	ClasifDir	Directorio que contiene el detector (NULL se corresponde con el detector facial por defecto
	\param boPruning If set reduces the search, recommended for face detection
	*/
	CViolaDetector(char *ClasifDir,bool boPruning=false)
	{
		//Inicializaciones
		boDetectorUsed=false;
		boCascada=false;
		NumObjects=0;

		cascade=0;

		//Inicializa el clasificador
		InitViolaDetector(ClasifDir, &cascade);//No controlamos si da error en cuyo caso devuelve NULL

		boCannyPruning=boPruning;//if true makes use of contours, which is in curent OpenCV version optimized for faces

		boDepura=false;//No debug info by default
	}

	//! Class destructoe
	/*!
	Deallocates memory used
	*/
	~CViolaDetector()
	{
		//Libera estrtucturas de datos de los objetos detectados
		Reset();

		//Libera estructura del detector en cascada
		if (boCascada)
		{
			cvReleaseHaarClassifierCascade( &cascade );

			cvReleaseMemStorage(&storage);
		}		
	}

	//! Reset
	/*!
	Deallocates memory used
	*/
	void Reset()
	{
		LiberaMemoria();
	}

	//! Process image
	/*!
	Searches objects using the Viola-Jones general detector framework implemented for OpenCV by Leinhart
	\param img Input image
	\param scale Scale used to reduce inout image dimensions
	\param boAppend If set processes image and appends detections to current list 
	\param min_size  Minimum window size. By default, it is set to the size of samples the classifier has been trained on. 
	*/
	void ProcessImage(IplImage *img,int scale, bool boAppend=false, CvSize min_size=cvSize(0,0))
	{		
		//Variable para contabilizar el tiempo
		clock_t	tini;

		//Storing previous detections if boAppend is set
		int prevdets, newdets, *px1,*px2,*py1,*py2,*cx1,*cy1,*cx2,*cy2;

		//Si tenemos clasificador e imagen
		if( boCascada && img )
		{
			CvSize img_size = cvGetSize( img );

			//Según la escala debemos hacer la pirámide un número de veces
			IplImage* temp,*temp2,*temp3,*temp4;

			//Reduce la resolución según la escala indicada
			switch (scale)
			{
			case 1:
				temp = cvCreateImage( cvSize(img_size.width,img_size.height), 8, img->nChannels );
				cvCopyImage( img, temp );
				break;
			case 4:
				temp = cvCreateImage( cvSize(img_size.width/4,img_size.height/4), 8, img->nChannels );
				temp2 = cvCreateImage( cvSize(img_size.width/2,img_size.height/2), 8, img->nChannels );
				//Realizamos  las pirámides necesarias
				cvPyrDown( img, temp2 );
				cvPyrDown( temp2, temp );
				break;
			case 8:
				temp = cvCreateImage( cvSize(img_size.width/8,img_size.height/8), 8, img->nChannels );
				temp2 = cvCreateImage( cvSize(img_size.width/2,img_size.height/2), 8, img->nChannels );
				temp3 = cvCreateImage( cvSize(img_size.width/4,img_size.height/4), 8, img->nChannels );
				//Realizamos  las pirámides necesarias
				cvPyrDown( img, temp2 );
				cvPyrDown( temp2, temp3 );
				cvPyrDown( temp3, temp );
				break;
			case 16:
				temp = cvCreateImage( cvSize(img_size.width/16,img_size.height/16), 8, img->nChannels );
				temp2 = cvCreateImage( cvSize(img_size.width/2,img_size.height/2), 8, img->nChannels );
				temp3 = cvCreateImage( cvSize(img_size.width/4,img_size.height/4), 8, img->nChannels );
				temp4 = cvCreateImage( cvSize(img_size.width/8,img_size.height/8), 8, img->nChannels );
				//Realizamos  las pirámides necesarias
				cvPyrDown( img, temp2 );
				cvPyrDown( temp2, temp3 );
				cvPyrDown( temp3, temp4 );
				cvPyrDown( temp4, temp );
				break;
			case 2:
			default:
				temp = cvCreateImage( cvSize(img_size.width/2,img_size.height/2), 8, img->nChannels );
				cvPyrDown( img, temp );
				break;
			}
		

			//Libero
	        cvClearMemStorage( storage );

			//Setting the las parameter should be faster
			CvSeq* objects=NULL;

			//Inicializo t para contabilizar el tiempo de poceso
			tini=clock();
			//Busco objetos
			//

			if (boCannyPruning)
				objects = cvHaarDetectObjects( temp, cascade, storage, 1.2, 2, CV_HAAR_DO_CANNY_PRUNING, min_size );
			else
				objects = cvHaarDetectObjects( temp, cascade, storage, 1.2, 3, 0, min_size);

			
			t=clock()-tini;


			prevdets=0;
			if (boAppend)
			{
				//Almacena detecciones previas
				prevdets=NumObjects;

				px1=(int *) new unsigned char[prevdets*sizeof(int)];
				px2=(int *) new unsigned char[prevdets*sizeof(int)];
				py1=(int *) new unsigned char[prevdets*sizeof(int)];
				py2=(int *) new unsigned char[prevdets*sizeof(int)];


				for (int i=0;i<prevdets;i++)
				{
					px1[i]=x1[i];
					px2[i]=x2[i];
					py1[i]=y1[i];
					py2[i]=y2[i];
				}
			}

			//Libero por si ya se utilizó
			LiberaMemoria();
		
			//Guarda valores de la detección
			if (objects)
			{
				newdets=objects->total;
				
				//Si hay Caras detectadas
				if (newdets)
				{		
					cx1=(int *) new unsigned char[newdets*sizeof(int)];
					cx2=(int *) new unsigned char[newdets*sizeof(int)];
					cy1=(int *) new unsigned char[newdets*sizeof(int)];
					cy2=(int *) new unsigned char[newdets*sizeof(int)];
					
					//Copiamos en la estructura los datos de detección
					for (int i=0;i<newdets;i++)
					{
						CvRect r = *(CvRect*)cvGetSeqElem( objects, i );

						cx1[i]=r.x*scale;
						cy1[i]=r.y*scale;
						cx2[i]=(r.x+r.width)*scale;
						cy2[i]=(r.y+r.height)*scale;
					}
				}

				cvClearSeq(objects);
				objects=NULL;//¿hace falta o no?
			}


			else
			{
				newdets=0;
			}


			NumObjects=newdets+prevdets;

			//Alojamos según número de detecciones
			AlojaMemoria(NumObjects);

			//Almacenamos detecciones previas
			if (boAppend)
			{
				for (int i=0;i<prevdets;i++)
				{
					x1[i]=px1[i];
					y1[i]=py1[i];
					x2[i]=px2[i];
					y2[i]=py2[i];
				}
				delete [] px1;
				delete [] py1;
				delete [] px2;
				delete [] py2;
			}

			//Copies new detections
			if (newdets)
			{
				for (int i=prevdets;i<NumObjects;i++)
				{
					x1[i]=cx1[i-prevdets];
					y1[i]=cy1[i-prevdets];
					x2[i]=cx2[i-prevdets];
					y2[i]=cy2[i-prevdets];
				}
				delete [] cx1;
				delete [] cy1;
				delete [] cx2;
				delete [] cy2;
			}

			
			
			//Libera la memoria según la resolución utilizada
			cvReleaseImage( &temp );
			if (scale>2) cvReleaseImage( &temp2 );
			if (scale>4) cvReleaseImage( &temp3 );
			if (scale>8) cvReleaseImage( &temp4 );

		
		}
		  
	}


		/****************************************************************************************

				Detección de caras empleando el esquema de Viola teniendo en cuenta la
				detección previa

				img					Imagen donde buscar
				scale				Escala a utilizar para procesar la imagen. Admite 1,2,4 y 8,
									que se corresponde a 1:1, 1:2, 1:4 y 1:8
				boTambienenTodalaImagen	Indica si se busca en toda la imagen en caso de no encontrar


	****************************************************************************************/
	void ProcesaImagenTCoherente(IplImage *img,int scale, bool boTambienenTodalaImagen=true)
	{
		//Alojamos memoria necesaria para el proceso
		CvSeq* faces;

		//Variable para contabilizar el tiempo
		clock_t	tini;


		//Inicializa tiempo de proceso
		t=0;

		bool boCoherencia=false;		//Indica si se detecta usando la información de la detección previa	
		int esqx,esqy;					//Posición relativa de la ROI
		int nfacestmp=0;				//Temporal para indicar número de caras detectadas

		//Si tenemos clasificador e imagen
		if( boCascada && img )
		{
			CvSize img_size = cvGetSize( img );

			//Según la escala debemos hacer la pirámide un número de veces
			IplImage* temp,*temp2,*temp3,*temp4;

			//Reduce la resolución según la escala indicada
			switch (scale)
			{
			case 1:
				temp = cvCreateImage( cvSize(img_size.width,img_size.height), 8, 3 );
				cvCopyImage( img, temp );
				break;
			case 4:
				temp = cvCreateImage( cvSize(img_size.width/4,img_size.height/4), 8, 3 );
				temp2 = cvCreateImage( cvSize(img_size.width/2,img_size.height/2), 8, 3 );
				//Realizamos  las pirámides necesarias
				cvPyrDown( img, temp2 );
				cvPyrDown( temp2, temp );
				break;
			case 8:
				temp = cvCreateImage( cvSize(img_size.width/8,img_size.height/8), 8, 3 );
				temp2 = cvCreateImage( cvSize(img_size.width/2,img_size.height/2), 8, 3 );
				temp3 = cvCreateImage( cvSize(img_size.width/4,img_size.height/4), 8, 3 );
				//Realizamos  las pirámides necesarias
				cvPyrDown( img, temp2 );
				cvPyrDown( temp2, temp3 );
				cvPyrDown( temp3, temp );
				break;
			case 16:
				temp = cvCreateImage( cvSize(img_size.width/16,img_size.height/16), 8, 3 );
				temp2 = cvCreateImage( cvSize(img_size.width/2,img_size.height/2), 8, 3 );
				temp3 = cvCreateImage( cvSize(img_size.width/4,img_size.height/4), 8, 3 );
				temp4 = cvCreateImage( cvSize(img_size.width/8,img_size.height/8), 8, 3 );
				//Realizamos  las pirámides necesarias
				cvPyrDown( img, temp2 );
				cvPyrDown( temp2, temp3 );
				cvPyrDown( temp3, temp4 );
				cvPyrDown( temp4, temp );
				break;
			case 2:
			default:
				temp = cvCreateImage( cvSize(img_size.width/2,img_size.height/2), 8, 3 );
				cvPyrDown( img, temp );
				break;

			}

			//Si detectó en la imagen anterior intenta localizar por coherencia espacial (tiene sentido si usa el ROI)
			if (NumObjects!=0)
			{
			//Direcciona la imagen actual

				//Tamaño de la ventana anterior detectada 
				//La nueva será el doble que la anterior en ancho y 1.5 veces en alto
				int winw=(int)(2.0*(x2[0]-x1[0]));
				int winh=(int)(1.5*(y2[0]-y1[0]));
				int roix,roiy,roiw,roih;
				
				//Comprueba que no supera los límites pensando en el tamaño de img
				esqx=(int)(x1[0]-winw*0.25);
				if (esqx<0) esqx=0;
				esqy=(int)(y1[0]-winh*0.125);
				if (esqy<0) esqy=0;

				if (esqx+winw>=img->width) winw=(img->width-esqx-1);
				if (esqy+winh>=img->height) winh=(img->height-esqy-1);

				//Define un ROI dependiendo de la detección anterior
				//Coloca el ROI al doble en área que la anterior detectada pero en la imagen de menor resolución
				roix=(int)(esqx*0.5);
				roiy=(int)(esqy*0.5);
				roiw=(int)(winw*0.5);
				roih=(int)(winh*0.5);

				//Comprueba límite máximo pensando en el tamaño de temp
				while (roix+roiw>=temp->width)
					roiw--;
				while (roiy+roih>=temp->height)
					roih--;

				while (roix<=0)
				{
					roiw--;
					roix++;
				}
				if (roiy<=0)
				{
					roih--;
					roiy++;
				}


				cvSetImageROI(temp,cvRect(roix,roiy,roiw,roih));

				IplImage *temproi=cvCreateImage( cvSize(roiw,roih), 8, 3 );

				//Copio a una intermedia
				cvCopyImage(temp,temproi);

				//Resetea el ROI
				cvResetImageROI(temp);

					//Libero
				cvClearMemStorage( storage );

				//Setting the las parameter should be faster
				CvSeq* faces=NULL;

				//Inicializo t para contabilizar el tiempo de poceso
				tini=clock();

				//Busco objetos
				if (boCannyPruning)
					faces = cvHaarDetectObjects( temproi, cascade, storage, 1.2, 2, 1 );//para caras
				else
					faces = cvHaarDetectObjects( temproi, cascade, storage, 1.2);

				t=clock()-tini;

				cvReleaseImage(&temproi);

				nfacestmp=(faces ? faces->total : 0);

				//Quizás al usar el ROI devuelve relativo a la esquina
				
				
			}

			//Si no ha detectado por coherencia espacial usa el esquema de búsqueda por toda la imagen
			if (nfacestmp==0)
			{
				if (boTambienenTodalaImagen)
				{
					cvClearMemStorage( storage );
					//Inicializo t para contabilizar el tiempo de poceso
					tini=clock();

					if (boCannyPruning)
						faces = cvHaarDetectObjects( temp, cascade, storage, 1.2, 2, 1 );//para caras
					else
						faces = cvHaarDetectObjects( temp, cascade, storage, 1.2);


					//Añade el tiempo a lo acumulado
					t+=(clock()-tini);
					boCoherencia=false;
				}
				else faces=NULL;
			}
			else 
				boCoherencia=true;

			
			//Libero si ya se utilizó
			LiberaMemoria();

			//Guarda valores
			NumObjects=0;

			if (faces)
			{
				
				NumObjects=faces->total;

				//Si hay Caras detectadas
				if (NumObjects)
				{
					//Alojamos según número de detecciones
					AlojaMemoria(NumObjects);
		
					
					//Copiamos en la estructura los datos de detección
					for (int i=0;i<NumObjects;i++)
					{
						CvRect r = *(CvRect*)cvGetSeqElem( faces, i );

						if (!boCoherencia) //Si no detectó usando ROI
						{
							x1[i]=r.x*scale;
							y1[i]=r.y*scale;
							x2[i]=(r.x+r.width)*scale;
							y2[i]=(r.y+r.height)*scale;

						}
						else
						{
							x1[i]=esqx+(r.x)*scale;
							y1[i]=esqy+(r.y)*scale;
							x2[i]=esqx+(r.x+r.width)*scale;
							y2[i]=esqy+(r.y+r.height)*scale;
						}
					}
				}
				cvClearSeq(faces);
				faces=NULL;
				
			}
			

			//Libera las imágenes según la resolución utilizada
			cvReleaseImage(&temp);
			if (scale>2) cvReleaseImage( &temp2 );
			if (scale>4) cvReleaseImage( &temp3 );
			if (scale>8) cvReleaseImage( &temp4 );
		}
		  
	}


	//! Paints detection results
    /*!
	Paints detection results in given image
	\param h Image to plot the results
	\param color Color to be used
	*/
	void PintaDetecciones(IplImage *h, CvScalar color)
	{
		//Si hay datos de caras detectadas pinta los rectángulos
		for (int i=0;i<NumObjects;i++)
			{
				cvRectangle( h,cvPoint(x1[i],y1[i]),
							cvPoint(x2[i],y2[i]),
							color, 3 );
			}
	}

	//! Paints detection results
    /*!
	Paints detection results in given image
	\param h Image to plot the results
	\param color Color to be used
	\param idet detcetion index
	*/
	void PintaDetecciones(IplImage *h, CvScalar color,int idet)
	{
		//paints detection
		cvRectangle( h,cvPoint(x1[idet],y1[idet]),
							cvPoint(x2[idet],y2[idet]),
							color, 3 );
	}

	//! Paints detection results for head and shoulders detection
    /*!
	Paints detection results in given image. These results are plotted using two rectangles
	\param h Image to plot the results
	\param color Color to be used
	\param color2 Color to be used
	\param ancho Rectangle border thickness
	*/
	void PintaDeteccionesdeHS(IplImage *h, CvScalar color, CvScalar color2, int ancho=3)
	{
		//Si hay datos de caras detectadas pinta los rectángulos
		for (int i=0;i<NumObjects;i++)
			{
				cvRectangle( h,cvPoint(x1[i],y1[i]),
							cvPoint(x2[i],y2[i]),
							color, ancho );

				//Pinta la posible zona de la cara
				int sx=x2[i]-x1[i],sy=y2[i]-y1[i];
				int wd=(int)((float)sx*0.3);
				int hd=(int)((float)sy*0.2);
				cvRectangle( h,cvPoint(x1[i]+wd,y1[i]+hd),
							cvPoint(x2[i]-wd,y2[i]-hd*2),
							color2, 1 );
			}
	}

	//! Gets largest detected object
	/*!
		Gets largest detected object
	*/
	int GetLargestDetected()
	{
		int i;
		int maxsize=0,csize=0,max=-1;

		if (NumObjects)
		{
			for (i=0;i<NumObjects;i++)
			{
				//Computes face size
				csize=(x2[i]-x1[i])*(y2[i]-y1[i]);

				//Stores the index of the face with largest size
				if (csize>maxsize)
				{
					maxsize=csize;
					max=i;
				}
			}				
		}

		return max;
	}

	//! Selects the largest detection
    /*!
	Stores in the first position, 0, the largest detection
	*/
	void EscogelaMayor()
	{
		int imayor=0;
		float area=0,areaaux;

		//It has sense only if there are more than one faces detected
		if (NumObjects>1)
		{
			//Si hay datos de caras detectadas Busca la mayor
			for (int i=0;i<NumObjects;i++)
			{
				areaaux=(float)((x2[i]-x1[i])*(y2[i]-y1[i]));

				if (areaaux>area)
				{
					area=areaaux;
					imayor=i;
				}
			}

			//Pone la mayor la primera y anula al resto liberando en consecuencia
			int x1aux,y1aux,x2aux,y2aux;
			
			x2aux=x2[imayor];
			x1aux=x1[imayor];
			y2aux=y2[imayor];
			y1aux=y1[imayor];
			
			//Libera y aloja para ahora una sola cara
			LiberaMemoria();
			NumObjects=1;
			AlojaMemoria(NumObjects);

			//Deja como cara detectada única la mayor localizada
			x2[0]=x2aux;
			x1[0]=x1aux;
			y2[0]=y2aux;
			y1[0]=y1aux;
						
		}


	}

	//! Selects the most similar detection in terms of size and overlapping
    /*!
	Returns the index of the new detection highest overlapping with given container
	\param x1prev Previous x cooodinate of the upper left corner
	\param y1prev Previous y cooodinate of the upper left corner
	\param x2prev Previous x cooodinate of the bottom right corner
	\param y2prev Previous y cooodinate of the bottom right corner
	\param escala Indicates the scale used for processing the image. Needed to fit previous and current detections.
	*/
	int EscogeParecida(int x1prev,int y1prev,int x2prev,int y2prev,int escala)
	{
		int w_prev=(x2prev-x1prev)/escala,
			h_prev=(y2prev-y1prev)/escala;//Ajusta a escala de la imagen donde hemos buscado
		long areaprev=(w_prev*h_prev),area;
		long areainter;
		float minratio=(float)0.55;//Probando antes era 0.45
		int escogida=-1;
		float *ratio;

		CMathUtils MathUtilsaux;

		if (NumObjects>=1)
		{
			ratio= (float*) new unsigned char[NumObjects*sizeof(float)];
			
			//Si hay datos de caras detectadas Busca la mayor
			for (int i=0;i<NumObjects;i++)
			{
				area=((x2[i]-x1[i])*(y2[i]-y1[i]));

				//Obtengo ratio siempre menor que uno
				if (area>areaprev)
					ratio[i]=((float)areaprev)/((float)area);
				else
					ratio[i]=((float)area)/((float)areaprev);

				//Guardo la de menor distancia en función de los tamaños
				MathUtilsaux.WindowsOverlap(x1prev,y1prev,x2prev,y2prev,x1[i],y1[i],x2[i],y2[i],&areainter);
				//Antes miraba que hubiera simple intersección, ahora exijo un poco más
				//Como antes se asume una cierta coindidencia de tamaños
				if (fabs(1.0-ratio[i])<minratio && (areainter/areaprev>0.2 || areainter/area>0.2))
				{
					minratio=(float)(1.0-ratio[i]);
					escogida=i;
				}

				//Sería bueno que la elección se debiera también al movimiento, los colores ...
			}

			

			delete [] ratio;
			
		}

		return escogida;
	}

	//! Removes redundant detections
	/*!
	Removes detections which overlap
	*/
	void EliminaRedundantes()
	{
		//
		int i,j,n;
		float area1,area2,areaaux;
		int ix1,ix2,iy1,iy2;
		bool boCambio;

		//Si se detectó alguna
		if (NumObjects>1)
		{

			for (i=0;i<NumObjects-1;i++)
			{
				area1=(float)((x2[i]-x1[i])*(y2[i]-y1[i]));
				for (j=i+1;j<NumObjects;j++)
				{
					area2=(float)((x2[j]-x1[j])*(y2[j]-y1[j]));
					boCambio=false;

                    //Obtiene intersección
					ix1=x1[i];
					if (x1[j]>ix1) ix1=x1[j];
					ix2=x2[i];
					if (x2[j]<ix2) ix2=x2[j];
					iy1=y1[i];
					if (y1[j]>iy1) iy1=y1[j];
					iy2=y2[i];
					if (y2[j]<iy2) iy2=y2[j];

					//Calcula área de la intersección si es posible
					if (ix2>ix1 && iy2>iy1)
					{
						areaaux=(float)((ix2-ix1)*(iy2-iy1));

						if (area1>area2)
						{
							//Si la menor está totalmente incluida y hay mucha diferencia de tamaño, 
							//elimina la pequeña que en este caso es la segunda
							if (areaaux/area2>0.95 && area2/area1>0.5)
							{
								boCambio=true;
							}
							else// en otro caso si la intersección es amplia, elimina la grande, en este caso la primera
							{
								if (areaaux/area1 > 0.8)
								{
									boCambio=true;

									//Eliminamos la mayor de las dos detecciones, en este caso la primera
									x1[i]=x1[j];
									x2[i]=x2[j];
									y1[i]=y1[j];
									y2[i]=y2[j];

								}
							}

						}
						else
						{
							if (areaaux/area1>0.95 && area1/area2>0.5)
							{
									boCambio=true;

									//Eliminamos la mayor de las dos detecciones, en este caso la primera
									x1[i]=x1[j];
									x2[i]=x2[j];
									y1[i]=y1[j];
									y2[i]=y2[j];

							}
							else
							{
								if (areaaux/area2 > 0.8)
								{
									boCambio=true;

								}
							}

						}

						//Si hubo cambio, movemos el resto
						if (boCambio)
						{
							//Machacamos la mayor de las dos detecciones
							for (n=j;n<NumObjects-1;n++)
							{
								x1[n]=x1[n+1];
								x2[n]=x2[n+1];
								y1[n]=y1[n+1];
								y2[n]=y2[n+1];
							}
							//Ahora tenemos una detección menos
							NumObjects--;

						}
					}
				}
			}
						
		}
	}

	//Variables
	//! Classifier identifier
	CvHaarClassifierCascade* cascade;			

	//! If set, the cascade is available
	bool	boCascada;							
	
	//! Number of objects detected
	int NumObjects;		
	//! Array of upper left x coordinates
	int *x1,
	//! Array of upper left y coordinates
		*y1,
	//! Array of lower right x coordinates
		*x2,
	//! Array of lower right y coordinates
		*y2;						
		


private:

	//Aloja memoria para la estructura de objetos localizados (pensado para caras)
	void AlojaMemoria(int NumFaces)
	{
		x1=(int *)new unsigned char[NumFaces*sizeof(int)];
		y1=(int *)new unsigned char[NumFaces*sizeof(int)];
		x2=(int *)new unsigned char[NumFaces*sizeof(int)];
		y2=(int *)new unsigned char[NumFaces*sizeof(int)];

		boDetectorUsed=true;//Hemos alojado memoria
	}

	//Libera la memoria de la estructura de objetos utilizada
	void LiberaMemoria()
	{
		if (boDetectorUsed)
		{
			delete [] x1;
			delete [] x2;
			delete [] y1;
			delete [] y2;

			boDetectorUsed=false;
		}
		NumObjects=0;
	}

	

	/*****************************************************************************************************************

		Viola Detector Initialization
			classifier_cascade_path			Localización del clasificador (NULL toma por defecto)
			hid								Identificador asignado al clasificador cargado
			winsize_w,winsize_h				Dimensiones de los patrones utilizados en el entrenamiento

	*****************************************************************************************************************/
	int InitViolaDetector( const char* classifier_cascade_path, CvHaarClassifierCascade** cascadeid, int winsize_w,int winsize_h, bool boDirectorio)
	{
		//PROVISIONAL transform from directories to xml file
		//Load old formar and saves new
		//char cascade_xml_filename[256];
		//Si no es el de defecto

		*cascadeid = 0;

		if (boDirectorio)
		{
			//Lee de formato directorio (old format)
			*cascadeid = cvLoadHaarClassifierCascade(
				classifier_cascade_path && strlen(classifier_cascade_path) > 0 ?
				classifier_cascade_path : "<default_face_cascade>",
				cvSize( winsize_w, winsize_h ));
		}
		else //Lee de formato XML (nuevo formato)
		{
			*cascadeid = (CvHaarClassifierCascade*)cvLoad( classifier_cascade_path, 0, 0, 0 );
		}		

		if (!(*cascadeid))
			return 0;

		storage = 0;
		storage = cvCreateMemStorage(0);

		boCascada=true;	//Indica clasificador cargado
		
		return 1;
	}

	/*****************************************************************************************************************

		Viola Detector Initialization
			classifier_cascade_path			Localización del clasificador (NULL toma por defecto)
			cascadeid						Identificador asignado al clasificador cargado
	
	*****************************************************************************************************************/
	int InitViolaDetector( const char* classifier_cascade_path, CvHaarClassifierCascade** cascadeid)
	{
		//Loads xml file
		*cascadeid = 0;

		*cascadeid = (CvHaarClassifierCascade*)cvLoad( classifier_cascade_path, 0, 0, 0 );
		
		if (!(*cascadeid))
			return 0;

		storage = 0;
		storage = cvCreateMemStorage(0);

		boCascada=true;	//Indica clasificador cargado
		
		return 1;
	}

	
	/****************************************************************************************

				Copia los datos de las caras detectadas

				caras				Estructira a escribir
								
	****************************************************************************************/
	/*void CopiaResultadoProceso(CFacialDataperImage *faces)
	{
		//Si ha detectado cara las copia junto a los ojos si están disponibles
		faces->Init(NumObjects);
		
		for (int i=0;i<NumObjects;i++)
		{
			//Copia datos de la detección
			//Contenedor
			faces->Faces[i]->x1=x1[i];
			faces->Faces[i]->x2=x2[i];
			faces->Faces[i]->y1=y1[i];
			faces->Faces[i]->y2=y2[i];
		}

		//Dato de tiempo de proceso
		faces->tproc=t;
	}*/

	//Variables

	CvMemStorage* storage;						//! Memory storage to be used with haardetection

	bool	boDetectorUsed;						//! If set, the detector has already been used

	long t;										//! Contains processing time

	int psx,psy;								//! Patterns dimensions

	bool boCannyPruning;							//! Checks for contours, optimized for faces

	
	bool	boDepura;		//! If set, debugging is active
};

#endif
