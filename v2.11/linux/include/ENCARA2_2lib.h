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
 /** \mainpage ENCARA2 interface

 \section des_sec DESCRIPTION

 ENCARA2 performs real-time multiple face detection. It makes use of OpenCV,
 combining Viola-Jones based detectors, skin color and tracking. Consult
 the doc folder to get further info.

 \section lic_sec CONTRIBUTORS LICENSE AGREEMENT


 IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
   By downloading, copying, installing or using the software you agree
   to this license.
   If you do not agree to this license, do not download, install,
   copy or use the software.

 Copyright (c) 2003-9, Modesto Castrillon-Santana (IUSIANI, University of
 Las Palmas de Gran Canaria, Spain).
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are
 met:

    \li Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
    \li Redistributions in binary form must reproduce the above
        copyright notice, this list of conditions and the following
        disclaimer in the documentation and/or other materials provided
        with the distribution.
    \li The name of Contributor may not used to endorse or promote products
        derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 \section res_sec RESEARCH USE
If you are using any of the involved ideas please cite (avalilable <a href="http://dx.doi.org/10.1016/j.jvcir.2006.11.004">online</a>):

\@ARTICLE{Castrillon07-jvci,
  author =       "Castrill\'on Santana, M. and D\'eniz Su\'arez, O. and Hern\'andez Tejera, M. and Guerra Artal, C.",
  title =        "ENCARA2: Real-time Detection of Multiple Faces at Different Resolutions in Video Streams",
  journal =      "Journal of Visual Communication and Image Representation",
  year =         "2007",
  vol =          "18",
  issue =        "2",
  month =        "April",
  pages =        "130-140"
}


More information can be found at <a href="http://mozart.dis.ulpgc.es/Gias/modesto_eng.html"><b>my site</b></a>

\section comm_sec COMMERCIAL USE
If you have any commercial interest in this work please contact
mcastrillon@iusiani.ulpgc.es


 */


#if ! defined ( ENCARA2_2_H )
#define ENCARA2_2_H

//#define DEMOVERSION	//if active restricts the number of detections to an specific number of frames

//#define VALECONUNJOJOYLABOCA

//Basic includes
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

//Own includes
#include "ENCARA2_2lib_defs.h"					//ENCARA2_2lib defines

//Blob information
#include "SkinBlob.h"

//Utilities to  warp BYTE images
#include "CWarpingImage.h"

//Rowley detector (used for comparison)
//#include "RowleyFaceDetectorInterface.h"

//Viola Jones based detectors interface
#include "ViolaJonesDetectorInterface.h"

//IplImage utils
#include "IplImageUtils.h"

//Image utils
#include "ImageUtils.h"

//Math utils
#include "MathUtils.h"

//Gabor
#include "GaborW.h"

//PCAUtils
#include "PCAUtils.h"

//SVMUtils
#include "SVMUtils.h"

//Logs
#include "DebugLog.h"


//Facialdata info
#include "FacialData.h"

//! <b>Main interface class</b>
/*! Standard interface with ENCARA2 detector

 */
class CENCARA2_2Detector
{
public:

	//! Class constructor
	/*!
	The constructor must specify the dimensions of the images where will be later analyzed. However,
	the dimensions can be modified after initialization by means of SetWorkingSize()

    \param DataDir	Contains the location of the data folder required by the classifiers enclosed in the detector
	\param sx	Width of the image to process
	\param sy	Height of the image to process
	\param cfgFile If not null, describes the location, relative to DataDir, of the config file
	*/
	CENCARA2_2Detector(char *DataDir,int sx=320, int sy=240, char *cfgFile=NULL)
	{
		//Stores data folder
		strcpy(cWorkDataDir,DataDir);

#ifndef ENCARA2INLINUX
		//Creates data structures if the directory exists
		if (_access( cWorkDataDir,00 )!=-1 )
		{
#endif
			//There are no working images allocated by default
			boHayImagenesdeTrabajo=false;

			//Loads configuration file

			//Sets name
#ifndef ENCARA2INLINUX
			if (cfgFile)
				sprintf(cPath,"%s\\%s",cWorkDataDir,cfgFile);
			else
				sprintf(cPath,"%s\\ENCARA2_2lib_cfg.txt",cWorkDataDir);
#else
			if (cfgFile)
				sprintf(cPath,"%s/%s",cWorkDataDir,cfgFile);
			else
				sprintf(cPath,"%s/ENCARA2_2lib_cfg.txt",cWorkDataDir);
#endif

			LoadConfigFile(cPath);
			//End

			//No work images a priori
			boWorkImagesAlloc=false;

			//Is debug log active? Then it resets the file
			if (boDebugLog)
			{
				if (DebugLog==NULL)
					DebugLog=(CDebugLog *)new CDebugLog(cWorkDataDir);
			}
			else
				DebugLog=NULL;

			//Alloc memory and so on
			LoadEnvironment();

#ifdef USESEGMENTATION
			//Initially the segmentation (segmenter by Luis) variable must be NULL
			SEG = NULL;
#endif

			//Allocs memory for default image size
			SetWorkingSize(sx,sy);

			// Seed the random-number generator with current time so that
			//	the numbers will be different every time we run.
			srand( (unsigned)time( NULL ) );

			boDetectorAvailable=true;

#ifndef ENCARA2INLINUX
		}
		else
			boDetectorAvailable=false;
#endif

	}

	//! Class destructor
	/*!
	Releases the structures allocated.
	*/
	~CENCARA2_2Detector()
	{
		if (boDetectorAvailable)
		{
			//Frees working images
			LiberaImagenesdeTrabajo();

			//Viola based detectors
			delete ViolaFaceDetector;
	//		delete ViolaBasedOwnFaceDetector;
			delete ViolaEyePairDetector;
	//		delete ViolaParOjosDetector;
			delete ViolaLeftEyeDetector;
#ifndef ENCARA2INLINUX
//			delete ViolaUMunichEyeDetector;
#endif
			delete ViolaRightEyeDetector;
			delete ViolaHSDetector;
//			delete ViolaUpperBodyDetector;
			delete ViolaLowerBodyDetector;
			delete ViolaFullBodyDetector;
			delete ViolaProfileDetector;
			delete ViolaMouthDetector;
			delete ViolaNoseDetector;

			//PCA representation
			delete PCALEye;
			delete PCAREye;
			delete PCAEyePair;

			//SVM based
			if (SVMFace) delete SVMFace;


			//Face appearance
			delete PCARepFaceQuore;

			//Facial elements likelihood
			if (MouthLikelihood)
				cvReleaseImage(&MouthLikelihood);
			if (NoseLikelihood)
				cvReleaseImage(&NoseLikelihood);


			cvReleaseMemStorage(&ViolaStorage);

			//Logs file
			if (DebugLog)
			{
				delete DebugLog;
				DebugLog=NULL;
			}


		}


	}


	//! Resets debug file
	/*!
	Debug file size is set to zero.
	*/
	void ResetsDebugFile()
	{
		//Is debug log active? Then it resets the file
		if (boDebugLog)
		{
			if (DebugLog) delete DebugLog;//Fuerza la nueva creación

			DebugLog=(CDebugLog *)new CDebugLog(cWorkDataDir);
		}
		else
			DebugLog=NULL;
	}


	//! Creates working images according to the provided size. It allows the modification of the processing size without invoking again the constructor
	/*!
    \param sx	Width of the image to process
	\param sy	Height of the image to process
	*/
	void SetWorkingSize(int sx, int sy)
	{
		//No gray image
		boImagenGrises=false;

		//Detector reset
		DetectorReset();

		//Frees previous memory
		LiberaImagenesdeTrabajo();

		//Working dimensions
		width=sx;
		height=sy;

		//Maximum intereye distance allowed
		iMaximoInterOjo=(int)(sx*0.4);

		//Allocs memory
		CvSize imgsz=cvSize(width, height);

		image = cvCreateImage( imgsz, IPL_DEPTH_8U, 3 );
		imagerot = cvCreateImage( imgsz, IPL_DEPTH_8U, 3 );
		imagegray = cvCreateImage( imgsz, IPL_DEPTH_8U, 1 );
		dummy = cvCreateImage( imgsz, IPL_DEPTH_8U, 3 );
		dummygray = cvCreateImage( imgsz, IPL_DEPTH_8U, 1 );
		imagegrayIN = cvCreateImage( imgsz, IPL_DEPTH_8U, 1 );
		imagelbp = cvCreateImage( imgsz, IPL_DEPTH_8U, 1 );
		imagegrayINprev = cvCreateImage( imgsz, IPL_DEPTH_8U, 1 );
		pyramid = cvCreateImage( imgsz, IPL_DEPTH_8U, 1 );
        prev_pyramid = cvCreateImage( imgsz, IPL_DEPTH_8U, 1 );
		imageINr_norm = cvCreateImage( imgsz, IPL_DEPTH_32F, 1);
		imageINg_norm = cvCreateImage( imgsz, IPL_DEPTH_32F, 1);

		//Creates blob structure for high and low resolution
		Blob=new CSkinBlobInfo(width,height);
		Blob4=new CSkinBlobInfo(width/4,height/4);

		//Sets the flag of available working images
		boHayImagenesdeTrabajo=true;

#ifdef USESEGMENTATION
		//Segmentation Luis
		//imagesegmented = cvCreateImage( cvSize(image->width/2,image->height/2), 8, image->nChannels );
		imagesegmented = cvCreateImage( cvSize(image->width,image->height), 8, image->nChannels );
		SEG = new Segmenta(imagesegmented);
#endif
	}

	//! Returns current working size
	/*!
    \param sx	Width of current working images
	\param sy	Height of current working images
	*/
	void GetWorkingSize(int *sx, int *sy)
	{
		//Working dimensions
		*sx=width;
		*sy=height;
	}

	//! Resets the detector history.
	/*!
	Resets the detector history. The next frame processed will be considered the first one, i.e.
	the detection memory (contgaining information about current and previous detection threads) will be empty until a new detection is achieved.
	*/
	void DetectorReset()
	{
		//Frees facial data structures
		DatosFaciales.Libera();
		DatosFacialesLost.Libera();
		DatosFacialesPrevios.Libera();
		DatosFacialesAux.Libera();

		//Counters
		nFrontal=0;				//Frontal faces detected
		nProcesadas=0;			//Images processed

		ids_sofar=0;			//Ids so far found

		nConsecutivasDetectandoporCoherenciaTemporal=0;	//Consecutive frames with detecttion by means of temporal coherence
	}

	//! Frees information related to lost detection threads
	/*!
	Frees information related only to those detection threads that are currently not active, i.e. to lost detection threads.
	*/
	void DetectorLostReset()
	{
		DatosFacialesLost.Libera();
	}



	//! Provides external detection data for a selected face among actives
	/*!
	It allows the detector the possibility of integrating, information about the facial element detection, provided by an external source.
	That can be interesting for situations in which eye, mouth and nose detection has been done by an external tool.

		\param iface Face to correct
		\param boeyes If set forces both eye position
		\param oix	x coordinate of the left eye
		\param oiy	y coordinate of the left eye
		\param odx	x coordinate of the right eye
		\param ody	y coordinate of the right eye
		\param bonose If set forces nose position
		\param nx	x coordinate of the nose
		\param ny	y coordinate of the nose
		\param bomouth If set forces mouth position
		\param mlx	x coordinate of the mouth left corner
		\param mly	y coordinate of the mouth left corner
		\param mrx	x coordinate of the mouth right corner
		\param mry	y coordinate of the mouth right corner
	*/
	void ForceDetection(int iface,bool boeyes,int oix, int oiy, int odx, int ody,bool bonose,int nx, int ny, bool bomouth,int mlx, int mly, int mrx, int mry)
	{
		//Given a valid index?
		if (iface>=0 && iface<DatosFaciales.NumFaces)
		{
			DatosFaciales.ForceDetection(iface,boeyes,oix, oiy, odx,ody,bonose,nx,ny,bomouth,mlx,mly,mrx,mry,image,imagegrayIN);
		}


	}

#ifdef USESEGMENTATION
	/********************************************************************************************************
		Segmentation by Luis (TBD)
	********************************************************************************************************/
	//! Serves the segmenter pointer
	/*!
	Serves the access to the segmentation information (under development)
	*/
	Segmenta *GetSeg()
	{
		return SEG;
	}

	//! Serves a segmented image
	/*!
	Provides the segmented image

		\param minsize Minimum size accepted for a region. Parameter taken into account by the segmenter.
	*/
	IplImage *GetSegmentation(int minsize=10)
	{

		if (boHayImagenesdeTrabajo)
		{
			//Reduce la dimensión de la imagen a tratar
			//cvPyrDown( image, imagesegmented );//Menor resolución
			cvCopyImage(image,imagesegmented);

	//MOD		SEG->flush();

	//MOD		SEG->segmenta(imagesegmented,minsize);

			return imagesegmented;

		}
		else return NULL;

	}
#endif

	/********************************************************************************************************
		Facial data interface
	********************************************************************************************************/
	//! Returns pointer to the list of active detection threads
	/*!
	Allows the access to face detection information
	*/
	CFacialDataperImage	*GetFacialData()
	{
		return &DatosFaciales;
	}

	//! Returns pointer to the list of lost detection threads
	/*!
	Allows the access to face detection information of already lost detection threads
	*/
	CFacialDataperImage	*GetFacialDataLost()
	{
		return &DatosFacialesLost;
	}


	//! <b>Standard interface to detect faces using ENCARA2</b>
	/*!
    \param h	Image to process
	\param scale	1 uses the input image resolution, 2 halves it.
	*/
	void ApplyENCARA2(IplImage *h, int scale=2);


	//! Standard interface to detect faces in an image using a Viola-Jones classifier
	/*!
    \param h	Image to process
	\param iDetector	Identifies the detector to apply (OPENCV_FACE, OWN_LEYE, OWN_REYE, OWN_BOTHEYES, OWN_NOSE, OWN_MOUTH, OWN_HS)
	\param px1 Array of left upper corner x coodinates returned
	\param py1 Array of left upper corner y coodinates returned
	\param px2 Array of bottom right corner x coodinates returned
	\param py2 Array of bottom right corner y coodinates returned
	\param scale	1 uses the input image scale, 2 halves it
	*/
	void ApplyViolaBasedDetector(IplImage *h, int iDetector, int **px1, int **py1, int **px2,int **py2,int scale=2);

	//! Paints detection results
	/*!
    \param h	Image to plot the results
	\param color Color to be used
	\param Masked Paints using different "creative" ways
	*/
	void PaintFacialData(IplImage *h,CvScalar color,bool Masked=false);
	
	//! Sets mouth and nose likelihood images
	/*!
    	\param Mouth	Mouth likelihood image
	\param Nose	Nose likelihood image
	*/
	void SetLikelihoodImages(IplImage *Mouth,IplImage *Nose)
	{
	  cvCopyImage(Mouth,MouthLikelihood);
	  cvCopyImage(Nose,NoseLikelihood);
	}

	//! Detector created
	bool	boDetectorAvailable;					//Is the detector available, set to false if the data folder was not located


	
	
private:
	//! Loads configuration file
	/*!
		Loads and sets ENCARA2_2Detector configuration
		\param	Config file path, if null the default configuration will be used.
	*/
	void LoadConfigFile(char *cfgFile)
	{
		//Sets default configuration
		SetDefaultConfig();

		//Opens config file
		FILE *fp=fopen(cfgFile,"r");

		//Reads configuration, and modifies default configuration according to the file
		if (fp)
		{
			char code[20],value[20];

			while(fscanf(fp,"%s",code)!=EOF)
			{
				fscanf(fp,"%s",value);

				//DebugLogs
				if (strcmp("DEBUGLOG",code)==0)
				{
					if (strcmp("false",value)==0)
					{
						boDebugLog=false;
						DebugLog=NULL;
					}

					if (strcmp("true",value)==0)
					{
						boDebugLog=true;
						DebugLog=NULL;
						//Resetea el fichero de depuración
						ResetsDebugFile();
					}
				}

				//Search eyes using gray levels heuristics
				if (strcmp("SEARCHEYESUSINGGRAY",code)==0)
				{
					if (strcmp("false",value)==0)
						boSearchEyesusingGray=false;

					if (strcmp("true",value)==0)
						boSearchEyesusingGray=true;
				}


				//Search mouth
				if (strcmp("SEARCHMOUTH",code)==0)
				{
					if (strcmp("false",value)==0)
						boSearchMouth=false;

					if (strcmp("true",value)==0)
						boSearchMouth=true;
				}

				//Search nose
				if (strcmp("SEARCHNOSE",code)==0)
				{
					if (strcmp("false",value)==0)
						boSearchNose=false;

					if (strcmp("true",value)==0)
						boSearchNose=true;
				}

				//Legs detection
				if (strcmp("SEARCHLEGS",code)==0)
				{
					if (strcmp("false",value)==0)
						boSearchLegs=false;

					if (strcmp("true",value)==0)
						boSearchLegs=true;
				}

				//Full body detection
				if (strcmp("VIOLAFULLBODY",code)==0)
				{
					if (strcmp("false",value)==0)
						boBuscaconViolaFullBody=false;

					if (strcmp("true",value)==0)
						boBuscaconViolaFullBody=true;
				}


				//Speed
				if (strcmp("SPEED",code)==0)
				{
					if (strcmp("false",value)==0)
						boSpeed=false;

					if (strcmp("true",value)==0)
						boSpeed=true;
				}

				//PreviousDetection
				if (strcmp("USEPREVIOUSINFO",code)==0)
				{
					if (strcmp("false",value)==0)
						boUsePreviousDetection=false;

					if (strcmp("true",value)==0)
						boUsePreviousDetection=true;
				}


			}
			fclose(fp);
		}
	}

	//! Sets default ENCARA2_2Detector configuration
	/*!
		Sets default ENCARA2_2Detector configuration. Providing a config file path with CENCARA2_2Detector()
		will overwrite the specified parameters
	*/
	void SetDefaultConfig()
	{
		//Tracking
		boUsaCorrelacion=true;
		boCorrelaOjos=true;
		boCorrelaBoca=true;
		boCorrelaNariz=true;

		boAjustaIrisCorrelando=true;//true,
		boAjustaIrisDetectando=false;

		boUsaProyecciones=true;//false;

		//Geometric tests
		boTestOjijunto=true;//false;
		boTestPendientes=true;//false;

		//Viola based detectors
		boBuscaconViolaCaras=true;
		FViolaCaras=1;
		boBuscaconViolaProfile=false;//true;
		FViolaProfile=1;
		boBuscaconViolaHS=true;
		FViolaHS=1;
		boBuscaconViolaFullBody=false;//true;
		FViolaFullBody=1;
		boSpeed=true;//Establece que se acelera la búsqueda

		boDetectaElementosFacialesconViola=true;		//Active means that Viola Detectors are used
		boUseAlternativeDetectors=false;				//Uses more than one detector for faces and HS

		boUsePreviousDetection=true;					//Makes use of previour detection to speed up the process

		//Learning
		boLearningLive=true;							//Active learns a classifier on line

		//Appearance tests (some are obsolete and therefore set to false)
		boTestPCAOjos=true;//false;
		boTestHarrOjos=false;
		boTestHarrAdaBoostOjos=false;
		boTestHarrFloatBoostOjos=false;
		boTestPCASVMOjos=false;
		boSintestOjos=false;//true;

		boTestPCACara=false;
		boTestPCACaraold=false;
		boTestPCASVMCara=false;
		boTestHarrPCASVMCara=true;//false;
		boTestHarrCara=false;
		boTestHarrAdaBoostCara=false;
		boTestHarrFloatBoostCara=false;
		boSintestCara=false;

		//Logs
		boDebugLog=false;
		DebugLog=NULL;

		//Facial elements search
		boSearchEyesusingGray=false;
		boSearchMouth=true;
		boSearchNose=true;
		boSearchLegs=false;

		}

	//! Configures the detector, creating required structures
	/*!
		Sets up detector configuration creating and allocating all the structures needed
	*/
	void LoadEnvironment()
	{
		//REQUIRED STRUCTURES

		//IMAGES
		//Elliptical mask (created just once to speed up)
		EllipseMask  = cvCreateImage( cvSize(FACEX,FACEY), IPL_DEPTH_8U, 1 );
		
		ImageUtils.GetEllipticalMask(EllipseMask,RIGHTEYEX,RIGHTEYEY,LEFTEYEX,LEFTEYEY);

		//VIOLA-JONES based detectors included
		ViolaStorage = cvCreateMemStorage();//0 is the default size, 64000 bytes

		//Frontal face detector, (default 24x24, alt 20x20) Estoy experimentando problemas con el default
#ifndef ENCARA2INLINUX
		//sprintf(cPath,"%s\\HaarClassifiers\\haarcascade_frontalface_default_tree.xml",cWorkDataDir);

		sprintf(cPath,"%s\\HaarClassifiers\\haarcascade_frontalface_alt2.xml",cWorkDataDir);//Parece ir mejor según VISSAP
		ViolaFaceDetector=new CViolaDetector(cPath,true);
#else
		//sprintf(cPath,"%s/HaarClassifiers_linux/haarcascade_frontalface_alt_tree.xml",cWorkDataDir);
		//El anterior estaba hasta octubre 2008 en la versión linux
		sprintf(cPath,"%s/HaarClassifiers_linux/haarcascade_frontalface_alt2.xml",cWorkDataDir);

		ViolaFaceDetector=new CViolaDetector(cPath,true);//24x24
#endif

		if (boDebugLog)
		{
			char cadena[512];

			if (ViolaFaceDetector==NULL)
			{
				sprintf(cadena,"Error while reading ViolaFaceDetector\n");
				DebugLog->WriteToLogFile(cadena);
			}
			else
			{
				if (ViolaFaceDetector->cascade==NULL)
				{
					sprintf(cadena,"Error while reading ViolaFaceDetector->cascade\n");
					DebugLog->WriteToLogFile(cadena);
				}
			}
		}

		//Own set frontal face detector
/*#ifndef ENCARA2INLINUX
		sprintf(cPath,"%s\\HaarClassifiers\\CarasFrontalesPropio.xml",cWorkDataDir);
#else
		sprintf(cPath,"%s/HaarClassifiers_linux/CarasFrontalesPropio.xml",cWorkDataDir);
#endif
		ViolaBasedOwnFaceDetector=new CViolaDetector(cPath,true);//20x24

		if (boDebugLog)
		{
			char cadena[512];

			if (ViolaBasedOwnFaceDetector==NULL)
			{
				sprintf(cadena,"Error while reading ViolaBasedOwnFaceDetector\n");
				DebugLog->WriteToLogFile(cadena);
			}
			else
			{
				if (ViolaBasedOwnFaceDetector->cascade==NULL)
				{
					sprintf(cadena,"Error while reading ViolaBasedOwnFaceDetector->cascade\n");
					DebugLog->WriteToLogFile(cadena);
				}
			}
		}
*/
		//Eye pair detector  (from OpenCV yusuf) We can use our own alternatives, but this one seems to be the best
#ifndef ENCARA2INLINUX
		sprintf(cPath,"%s\\HaarClassifiers\\Eyes\\eye_cascade.xml",cWorkDataDir);
#else
		sprintf(cPath,"%s/HaarClassifiers_linux/Eyes/eye_cascade.xml",cWorkDataDir);
#endif
		ViolaEyePairDetector=new CViolaDetector(cPath);//35x16

		if (boDebugLog)
		{
			char cadena[512];

			if (ViolaEyePairDetector==NULL)
			{
				sprintf(cadena,"Error while reading ViolaEyePairDetector\n");
				DebugLog->WriteToLogFile(cadena);
			}
			else
			{
				if (ViolaEyePairDetector->cascade==NULL)
				{
					sprintf(cadena,"Error while reading ViolaEyePairDetector->cascade\n");
					DebugLog->WriteToLogFile(cadena);
				}
			}
		}

		/*//Own set eye pair detector
#ifndef ENCARA2INLINUX
		sprintf(cPath,"%s\\HaarClassifiers\\Eyes\\parojos.xml",cWorkDataDir);
#else
		sprintf(cPath,"%s/HaarClassifiers_linux/Eyes/parojos.xml",cWorkDataDir);
#endif
		ViolaParOjosDetector=new CViolaDetector(cPath,true);//22x5

		if (boDebugLog)
		{
			char cadena[512];

			if (ViolaParOjosDetector==NULL)
			{
				sprintf(cadena,"Error while reading ViolaParOjosDetector\n");
				DebugLog->WriteToLogFile(cadena);
			}
			else
			{
				if (ViolaParOjosDetector->cascade==NULL)
				{
					sprintf(cadena,"Error while reading ViolaParOjosDetector->cascade\n");
					DebugLog->WriteToLogFile(cadena);
				}
			}
		}*/


		//Right eye detector
#ifndef ENCARA2INLINUX
		sprintf(cPath,"%s\\HaarClassifiers\\Eyes\\REye.xml",cWorkDataDir);
#else
		sprintf(cPath,"%s/HaarClassifiers_linux/Eyes/REye.xml",cWorkDataDir);
#endif
		ViolaRightEyeDetector=new CViolaDetector(cPath,true);//18x12

		if (ViolaRightEyeDetector)
			if (ViolaRightEyeDetector->cascade)
				ViolaRightEyeDetector->cascade->count=16;//Originally 20


		if (boDebugLog)
		{
			char cadena[512];

			if (ViolaRightEyeDetector==NULL)
			{
				sprintf(cadena,"Error while reading ViolaRightEyeDetector\n");
				DebugLog->WriteToLogFile(cadena);
			}
			else
			{
				if (ViolaRightEyeDetector->cascade==NULL)
				{
					sprintf(cadena,"Error while reading ViolaRightEyeDetector->cascade\n");
					DebugLog->WriteToLogFile(cadena);
				}
			}
		}

		//Left eye detector
#ifndef ENCARA2INLINUX
		sprintf(cPath,"%s\\HaarClassifiers\\Eyes\\LEye.xml",cWorkDataDir);
#else
		sprintf(cPath,"%s/HaarClassifiers_linux/Eyes/LEye.xml",cWorkDataDir);
#endif
		ViolaLeftEyeDetector=new CViolaDetector(cPath,true);//18x12

		if (ViolaLeftEyeDetector)
			if (ViolaLeftEyeDetector->cascade)
				ViolaLeftEyeDetector->cascade->count=18;//Originally 20 (con 16 da saltito falsas detecciones)

		if (boDebugLog)
		{
			char cadena[512];

			if (ViolaLeftEyeDetector==NULL)
			{
				sprintf(cadena,"Error while reading ViolaLeftEyeDetector\n");
				DebugLog->WriteToLogFile(cadena);
			}
			else
			{
				if (ViolaLeftEyeDetector->cascade==NULL)
				{
					sprintf(cadena,"Error while reading ViolaLeftEyeDetector->cascade\n");
					DebugLog->WriteToLogFile(cadena);
				}
			}
		}

		//Own head and shoulder detector
#ifndef ENCARA2INLINUX
		sprintf(cPath,"%s\\HaarClassifiers\\HeadShoulders\\HS.xml",cWorkDataDir);
#else
		sprintf(cPath,"%s/HaarClassifiers_linux/HeadShoulders/HS.xml",cWorkDataDir);
#endif
		ViolaHSDetector=new CViolaDetector(cPath,true);//22x20 Nuevo en pruebas el true y false reduce las escalas a probar

		if (boDebugLog)
		{
			char cadena[512];

			if (ViolaHSDetector==NULL)
			{
				sprintf(cadena,"Error while reading ViolaHSDetector\n");
				DebugLog->WriteToLogFile(cadena);
			}
			else
			{
				if (ViolaHSDetector->cascade==NULL)
				{
					sprintf(cadena,"Error while reading ViolaHSDetector->cascade\n");
					DebugLog->WriteToLogFile(cadena);
				}
			}
		}
/*
#ifndef ENCARA2INLINUX
		sprintf(cPath,"%s\\HaarClassifiers\\haarcascade_upperbody.xml",cWorkDataDir);
#else
		sprintf(cPath,"%s/HaarClassifiers_linux/haarcascade_upperbody.xml",cWorkDataDir);
#endif
		ViolaUpperBodyDetector=new CViolaDetector(cPath);//22x18

		if (boDebugLog)
		{
			char cadena[512];

			if (ViolaUpperBodyDetector==NULL)
			{
				sprintf(cadena,"Error while reading ViolaUpperBodyDetector\n");
				DebugLog->WriteToLogFile(cadena);
			}
			else
			{
				if (ViolaUpperBodyDetector->cascade==NULL)
				{
					sprintf(cadena,"Error while reading ViolaUpperBodyDetector->cascade\n");
					DebugLog->WriteToLogFile(cadena);
				}
			}
		}
*/
#ifndef ENCARA2INLINUX
		//sprintf(cPath,"%s\\HaarClassifiers\\feet22_26.xml",cWorkDataDir);
		sprintf(cPath,"%s\\HaarClassifiers\\haarcascade_lowerbody.xml",cWorkDataDir);
#else
		sprintf(cPath,"%s/HaarClassifiers_linux/haarcascade_lowerbody.xml",cWorkDataDir);
#endif
		ViolaLowerBodyDetector=new CViolaDetector(cPath);//

		if (boDebugLog)
		{
			char cadena[512];

			if (ViolaLowerBodyDetector==NULL)
			{
				sprintf(cadena,"Error while reading ViolaLowerBodyDetector\n");
				DebugLog->WriteToLogFile(cadena);
			}
			else
			{
				if (ViolaLowerBodyDetector->cascade==NULL)
				{
					sprintf(cadena,"Error while reading ViolaLowerBodyDetector->cascade\n");
					DebugLog->WriteToLogFile(cadena);
				}
			}
		}

#ifndef ENCARA2INLINUX
		sprintf(cPath,"%s\\HaarClassifiers\\haarcascade_fullbody.xml",cWorkDataDir);
#else
		sprintf(cPath,"%s/HaarClassifiers_linux/haarcascade_fullbody.xml",cWorkDataDir);
#endif
		ViolaFullBodyDetector=new CViolaDetector(cPath);//14x28


		if (boDebugLog)
		{
			char cadena[512];

			if (ViolaFullBodyDetector==NULL)
			{
				sprintf(cadena,"Error while reading ViolaFullBodyDetector\n");
				DebugLog->WriteToLogFile(cadena);
			}
			else
			{
				if (ViolaFullBodyDetector->cascade==NULL)
				{
					sprintf(cadena,"Error while reading ViolaFullBodyDetector->cascade\n");
					DebugLog->WriteToLogFile(cadena);
				}
			}
		}

		//New OpenCV ver 4 detectors
#ifndef ENCARA2INLINUX
		sprintf(cPath,"%s\\HaarClassifiers\\haarcascade_profileface.xml",cWorkDataDir);
#else
		sprintf(cPath,"%s/HaarClassifiers_linux/haarcascade_profileface.xml",cWorkDataDir);
#endif
		ViolaProfileDetector=new CViolaDetector(cPath);//22x18

		if (boDebugLog)
		{
			char cadena[512];

			if (ViolaProfileDetector==NULL)
			{
				sprintf(cadena,"Error while reading ViolaProfileDetector\n");
				DebugLog->WriteToLogFile(cadena);
			}
			else
			{
				if (ViolaProfileDetector->cascade==NULL)
				{
					sprintf(cadena,"Error while reading ViolaProfileDetector->cascade\n");
					DebugLog->WriteToLogFile(cadena);
				}
			}
		}


		//Mouth detector
#ifndef ENCARA2INLINUX
		sprintf(cPath,"%s\\HaarClassifiers\\Mouth\\Mouth.xml",cWorkDataDir);//NUevo Junio 08
		//sprintf(cPath,"%s\\HaarClassifiers\\Mouth\\Boca.xml",cWorkDataDir);//Dic 07
#else
		sprintf(cPath,"%s/HaarClassifiers_linux/Mouth/Mouth.xml",cWorkDataDir);
#endif
		ViolaMouthDetector=new CViolaDetector(cPath,true);// 20 stages

		if (boDebugLog)
		{
			char cadena[512];

			if (ViolaMouthDetector==NULL)
			{
				sprintf(cadena,"Error while reading ViolaMouthDetector\n");
				DebugLog->WriteToLogFile(cadena);
			}
			else
			{
				if (ViolaMouthDetector->cascade==NULL)
				{
					sprintf(cadena,"Error while reading ViolaMouthDetector->cascade\n");
					DebugLog->WriteToLogFile(cadena);
				}
			}
		}

		//Nariz
#ifndef ENCARA2INLINUX
		sprintf(cPath,"%s\\HaarClassifiers\\Nose\\Nose.xml",cWorkDataDir);//Nuevo Junio 08
#else
		sprintf(cPath,"%s/HaarClassifiers_linux/Nose/Nose.xml",cWorkDataDir);
#endif
		ViolaNoseDetector=new CViolaDetector(cPath,true);//20 stages

		//Hecho con el anterior a 18
		if (ViolaNoseDetector)
			if (ViolaNoseDetector->cascade)
				ViolaNoseDetector->cascade->count=18;//Tiene 20

		if (boDebugLog)
		{
			char cadena[512];

			if (ViolaNoseDetector==NULL)
			{
				sprintf(cadena,"Error while reading ViolaNoseDetector\n");
				DebugLog->WriteToLogFile(cadena);
			}
			else
			{
				if (ViolaNoseDetector->cascade==NULL)
				{
					sprintf(cadena,"Error while reading ViolaNoseDetector->cascade\n");
					DebugLog->WriteToLogFile(cadena);
				}
			}
		}



/*		//Detector ojo uniMunchen (no consigo transformarlo a xml)
#ifndef ENCARA2INLINUX
		sprintf(cPath,"%s\\HaarClassifiers\\Ojos\\OjoMunich",cWorkDataDir);
		ViolaUMunichEyeDetector=new CViolaDetector(cPath,25,15,false,true);//Ojos\\OjoMunich
#endif
	*/

		//Representation for eyes, eye pair and face

#ifndef ENCARA2INLINUX
		sprintf(cPath,"%s\\PCA\\LEye",cWorkDataDir);
#else
		sprintf(cPath,"%s/PCA/LEye",cWorkDataDir);
#endif
		PCALEye=new CPCAUtils(cPath);

		if (boDebugLog)
		{
			char cadena[512];

			if (PCALEye->PCAspace==NULL)
			{
				sprintf(cadena,"Error while reading %s\n", cPath);
				DebugLog->WriteToLogFile(cadena);
			}
		}

#ifndef ENCARA2INLINUX
		sprintf(cPath,"%s\\PCA\\REye",cWorkDataDir);
#else
		sprintf(cPath,"%s/PCA/REye",cWorkDataDir);
#endif
		PCAREye=new CPCAUtils(cPath);

		if (boDebugLog)
		{
			char cadena[512];

			if (PCAREye->PCAspace==NULL)
			{
				sprintf(cadena,"Error while reading %s\n", cPath);
				DebugLog->WriteToLogFile(cadena);
			}
		}

#ifndef ENCARA2INLINUX
		sprintf(cPath,"%s\\PCA\\EyePair",cWorkDataDir);
#else
		sprintf(cPath,"%s/PCA/EyePair",cWorkDataDir);
#endif
		PCAEyePair=new CPCAUtils(cPath);

		if (boDebugLog)
		{
			char cadena[512];

			if (PCAEyePair->PCAspace==NULL)
			{
				sprintf(cadena,"Error while reading %s\n", cPath);
				DebugLog->WriteToLogFile(cadena);
			}
		}

		//SVM based facial appearance, still to be integrated
		SVMFace=NULL;


#ifndef ENCARA2INLINUX
		sprintf(cPath,"%s\\PCA\\Faces_QUOREXQUOREY",cWorkDataDir);
#else
		sprintf(cPath,"%s/PCA/Faces_QUOREXQUOREY",cWorkDataDir);
#endif
		PCARepFaceQuore=new CPCAUtils(cPath);

		if (boDebugLog)
		{
			char cadena[512];

			if (PCARepFaceQuore->PCAspace==NULL)
			{
				sprintf(cadena,"Error while reading %s\n", cPath);
				DebugLog->WriteToLogFile(cadena);
			}
		}

		//Facial elements likelihood images
		char name[128];
		sprintf(name,"%s/MouthLikelihood.jpg",cWorkDataDir);

#ifndef         USES_OF
		MouthLikelihood=cvLoadImage(name,0);
#else		//Problemas usando cvLoadImage con OF  llamar luego a SetLikelihoodImages
		MouthLikelihood=cvCreateImage(cvSize(FACEX,FACEY),IPL_DEPTH_8U, 1); //ALojo sin más
#endif

		sprintf(name,"%s/NoseLikelihood.jpg",cWorkDataDir);



#ifndef         USES_OF
		NoseLikelihood=cvLoadImage(name,0);
#else
		NoseLikelihood=cvCreateImage(cvSize(FACEX,FACEY),IPL_DEPTH_8U, 1); //ALojo sin más	
#endif

		//OpenCV fonts
		//font used to print labels related to detections
		cvInitFont(&stcvFont,CV_FONT_VECTOR0,0.4,0.4,0.0,1);
	}


	

	//! Frees working images
	/*!
	Frees working images

	*/
	void LiberaImagenesdeTrabajo()
	{
		if (boHayImagenesdeTrabajo)
		{
			//Libera imágenes
			cvReleaseImage(&image);
			cvReleaseImage(&imagerot);
			cvReleaseImage(&dummy);
			cvReleaseImage(&dummygray);
			cvReleaseImage(&imagegray);
			cvReleaseImage(&imagegrayIN);
			cvReleaseImage(&imagelbp);
			cvReleaseImage(&pyramid);
			cvReleaseImage(&prev_pyramid);
			cvReleaseImage(&imagegrayINprev);
			cvReleaseImage(&imageINr_norm);
			cvReleaseImage(&imageINg_norm);


#ifdef USESEGMENTATION
			//Segmentación Luis
			cvReleaseImage(&imagesegmented);
			//Segmentación Luis
			if (SEG)
			{
				delete [] SEG;
				SEG=NULL;
			}
#endif

			//Libera estructura de blob
			delete Blob;
			delete Blob4;

			boHayImagenesdeTrabajo=false;

		}
	}



	//! Locates faces in image
	/*!
	Searches faces in current image, provided by ProcesaImagenENCARA2MultiCaras()
		\param escala	Scale applied to input image
		*/
	void FindFaces(int escala);		//Main ENCARA2.2 routine

	//Facial elements
	int BuscaOjos_Grises(IplImage *hgray,int *odx,int *ody,unsigned char *vd,int *oix,int *oiy,unsigned char *vi,CvPoint Cmasa,float laxis, float saxis);
	int BuscaOjos_Grises(IplImage *hgray,int *odx,int *ody,unsigned char *vd,int *oix,int *oiy,unsigned char *vi,int x1,int y1, int x2, int y2);
	void BuscaParOjosconViola(CvHaarClassifierCascade* cascade,
										   IplImage *h,int x1,int y1,int x2,int y2,int *NumOjos,
										   int *ojosx1,int *ojosy1,int *ojosx2,int *ojosy2);
	int	BuscaOjosconViola(CvHaarClassifierCascade* cascadeD,CvHaarClassifierCascade* cascadeI,
										   IplImage *h,int x1,int y1,int x2,int y2,
										int *ojoDx,int *ojoDy,int *ojoIx,int *ojoIy);
	int	BuscaBocaconViola(IplImage *h,int x1,int y1,int x2,int y2,
							int *MOUTHX1,int *MOUTHY1,int *MOUTHX2,int *MOUTHY2,
							float eyesdist,int interx,int intery);
	int	BuscaNarizconViola(IplImage *h,int x1,int y1,int x2,int y2,
							int *MOUTHX1,int *MOUTHY1,int *MOUTHX2,int *MOUTHY2,
							float eyesdist,int interx,int intery);
	void BuscaBoca_Grises(IplImage *hgray, int * bdx, int *bdy, int *bix, int * biy,int interx, int intery, int distN);


	int BuscaOjosUsandoENCARA(int i);
	int BuscaCaracteristicasFacialesconColor(IplImage *hgray,IplImage *hgrayIN,CvRect rect,CvBox2D32f elipse);
	int BuscaCaracteristicasFacialessinColor(IplImage *hgray,IplImage *hgrayIN,int x1,int y1,int x2,int y2);


	//Color utilities
	void CalculaImagenColorNormalizado_Ventana(IplImage *h,IplImage *hrn,IplImage *hgn,int x1,int y1,int sx,int sy);
	void AprendeColor(IplImage *hrn,IplImage *hgn,int x1, int y1, int sx, int sy,
		float *rnmin,float *rnmax,float *gnmin,float *gnmax,bool *boColorAlreadyInitialized=false);

	//Min max
	void BuscaMinimob(BYTE *pbImagen,int isx,int isy,unsigned char *min,int *mx,int *my,int cx,int cy,int scx,int scy);
	void BuscaMaximob(BYTE *pbImagen,int isx,int isy,unsigned char *max,int *mx,int *my,int cx,int cy,int scx,int scy);



	//Facial data update (within Findfaces)
	void ActualizaDatosCarasenImagen(IplImage *hIN,IplImage *hgrayIN,
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
												  bool NoseContainerAvailable,bool boNoseAvailable);


	//Containers estimators
	void EstimateHSfromFace(CFacialDataperImage	*DatosFaciales,int i);
	void EstimateFacefromHS(CFacialDataperImage	*DatosFaciales,int i);
	void EstimateFacefromFB(CFacialDataperImage	*DatosFaciales,int i);
	bool EstimateBreastfromFace(IplImage *h,CFacialDataperImage	*DatosFaciales,int i);
	void EstimateFacefromEyes(CFacialDataperImage	*DatosFaciales,int i,int oix,int oiy, int odx, int ody);
	void EstimateFacefromEyeandMouth(CFacialDataperImage	*DatosFaciales,int i,bool boLE,int ox,int oy, int mx, int my);

	//Windows utilities
	void ControlVentanaBusqueda(CvRect *rSearch);

	void VentanaCoherenciaTemporal(IplImage *imageIN,int *esqx,int *esqy,int *winw,int *winh,int x1,int y1,int x2,int y2);
	void VentanaCoherenciaTemporalTorso(IplImage *imageIN,int *esqx,int *esqy,int *winw,int *winh,int x1,int y1,int x2,int y2);
	bool VentanaCoherenciaTemporalHS(IplImage *imageIN,int *esqx,int *esqy,int *winw,int *winh,int x1,int y1,int x2,int y2);
	bool VentanaCoherenciaTemporalFB(IplImage *imageIN,int *esqx,int *esqy,int *winw,int *winh,int x1,int y1,int x2,int y2);

	//Coordinate transformation
	void GetPositiononWorkImage(int inputx,int inputy,int *outx,int *outy);
	void GetPositiononInputImage(int inputx,int inputy,int *outx,int *outy);

	//Searches better eye pattern
	bool AjustaZonaOjo(IplImage *patron,int *posx,int *posy,float DistanciaInterocular);

	//TESTS
	int TestPosOjosconPrevia(int oix_in,int oiy_in,int odx_in,int ody_in,CFacialDataperImage	*DatosFaciales,int i);
	int TestAparienciaTracking(IplImage *hgrayIN,float fdist, bool boTestsApariencia);
	int TestAparienciaParaParOjos(IplImage *hgray,IplImage *hgrayIN,
										   int *oix,int *oiy, int *odx, int *ody,
										   float *DistanciaEntreOjos,
										   int umbralAutoOjos,bool boICorrOK,bool boDCorrOK,bool boBuscaRestoElementos=true);
	int TestBuenaPosPardeOjos(IplImage *hgray,int dx, int dy, int vd,int ix, int iy, int vi, CvPoint Cmasa,float sa,float la);



	//LK Tracker
	void SaveGoodFeaturestoTrack(IplImage *hgrayIN,CFacialDataperImage	*DatosFaciales,int i);


	//Proyections
	void Proyecciones_Grises(IplImage *hgray, CvPoint Cmasa,float laxis, float saxis,bool boSingleFace=true);
	void ProyeccionesBuscaMinimo(int *pos,int *val,long *proy,int dim,int min,int th,int th2,int *n,int nmax);

	//For better reading
	void SearchUsingPreviousFrameDetections(int *NumFaces,int escala,IplImage *image2);
	void SearchIntheWholeImage(int *NumFaces,int escala,IplImage *image2);
	void RemoveRedundantFaces(int *NumFaces);
	void FuseLostandNew();
	void SearchFacialElements(int NumFaces);
	void SearchEyesMouthandNose(IplImage *ima, IplImage *ima2,
												CFacialDataperImage	*DatosFaciales,int i,
												bool *boLEye, CvPoint *LEye,
												bool *boREye, CvPoint *REye,
												bool *boNose, CvPoint *Nose,
												bool *boLMouth, CvPoint *LMouth,
												bool *boRMouth, CvPoint *RMouth,
												bool *boMOuth, CvPoint *Mouth,
												int scaledown, int scaleup=-1,
												bool boUseTrackers=true,
												bool boRotateImage=true);//Nueva versión, menos estática que chequea múltiples posibilidades
	bool GetCandidates(IplImage *ima,IplImage *ima2,IplImage *imaR, IplImage *ima2R,int scale,
									   float angulo, float interx, float intery,
									   bool boTracker,COpenTracking *Tracker,
									   bool boPatternAvailable, CViolaDetector *Detector,
									   int despx,int despy,
									   int roiw,int roih,  int facesx,
									   int *nCandidates,long **WidthCandidates,
									   long **HeightCandidates,CvPoint **Candidates);
	bool GetCandidates(IplImage *ima,IplImage *ima2,IplImage *imaR,IplImage *ima2R,int scale,
									   float angulo, float interx, float intery,
									   bool boLTracker,COpenTracking *LTracker,
									   bool boRTracker,COpenTracking *RTracker,
									   bool boPatternAvailable, CViolaDetector *Detector,
									   int despx,int despy,
									   int roiw,int roih,int facesx,
									   int *nLCandidates,CvPoint **LCandidates,
									   int *nRCandidates,CvPoint **RCandidates,
									   int *nCandidates,long **WidthCandidates,
									   long **HeightCandidates, CvPoint **Candidates);
	float GetLikelihoodforNormalizedCoordinates(CvPoint leye, CvPoint reye,
													   bool boNose,CvPoint nose,
													   bool boMouth,CvPoint mouth);
	float* GetPCAErrorforNormalizedCoordinates(IplImage *h,CvPoint leye, CvPoint reye,  int feature=-1,
						IplImage* pair=NULL,IplImage* le=NULL,IplImage* re=NULL,IplImage* fq=NULL);
	bool ROIinImage(IplImage *iplimg,int x,int y, int sx, int sy);

	//Variables
	char	cWorkDataDir[200];						//Data directory
	char	cPath[256];								//Auxiliar string
	int		width,height;							//Working dimensions

	int		iMaximoInterOjo;						//Macimal intereye distance

	int		nFrontal;								//Number of faces detected during the session
	int		nProcesadas;							//Number of images processed during the session
	int		nConsecutivasDetectandoporCoherenciaTemporal;		//Consecutives images with detection by means of temporal coherence


	//Flags for memory management
	bool	boWorkImagesAlloc;						//Indicate if the work images were already allocated

	//Flags
	bool	boImagenGrises;					//If active avoids any color treatment for the image due to it is a gray image
	bool	boHayImagenesdeTrabajo;			//If active indicates that the working images have been created
	bool	boOjosBuscadosenGrises;			//Set if eyes were searched


	bool	boAlreadySearchedintheWholeImage;	//If set indicates that for this image a search in the whole image was performed



	//ENCARA2.2 configuration flags
	bool	boDebugLog;								//Is Debuglog active?
	CDebugLog	*DebugLog;

	//Correlation
	bool	boUsaCorrelacion,			//Uses correlation
			boCorrelaOjos,				//Correlates eyes
			boCorrelaBoca,				//Correlates the mouth
			boCorrelaNariz,				//Correlates the nose
			boAjustaIrisCorrelando,		//Adjust iris duting correlation
			boAjustaIrisDetectando;		//Adjust iris during detection

	//Projections
	bool boUsaProyecciones;				//Active uses gray projections for eye detection

	//Geometry
	bool boTestOjijunto; //Two too close eyes are rejected

	bool boTestPendientes; //Eyes mouth aligment test

	//Appearance
	//Para los ojos
	bool boTestPCAOjos,				//Aplica el test basado en PCA para los ojos normalizados
		boTestHarrOjos,				//Aplica el test basado en el operador de Harr para los ojos normalizados
		boTestHarrAdaBoostOjos,		//Aplica el test basado en una combinación Adaboost
		boTestHarrFloatBoostOjos,	//Aplica el test basado en una combinación Floatboost
		boTestPCASVMOjos,			//Aplica test de apariencia de la cara basado el PCA+SVM
		boSintestOjos;				//No aplica  a los ojos ningún test de apariencia
	//Para la cara
	bool boTestPCACaraold,			//Aplica el test basado en PCA para las caras normalizadas 59x65. Reduce falsos positivos
		boTestPCACara,				//Aplica el test basado en PCA para las caras normalizadas 20x20. Reduce falsos positivos
		boTestPCASVMCara,			//Aplica test de apariencia de la cara basado el PCA+SVM
		boTestHarrPCASVMCara,		//Aplica test de apariencia de la cara primero basado en Harr con bajo FN y luego basado el PCA+SVM
		boTestHarrCara,				//Aplica el test basado en el operador de Harr para los ojos normalizados
		boTestHarrAdaBoostCara,		//Aplica el test basado en una combinación Adaboost
		boTestHarrFloatBoostCara,	//Aplica el test basado en una combinación Floatboost
		boSintestCara;				//No aplica  a la cara ningún test de apariencia


	//Viola Detectors
	bool	boBuscaconViolaCaras,boBuscaconViolaHS,boBuscaconViolaProfile,boBuscaconViolaFullBody;
	//Execution frequencies
	int		FViolaCaras,FViolaHS,FViolaProfile,FViolaFullBody;

	bool	boSpeed;//Executes the detectors using frecuencies, i.e. speeding up the process. Tries to reduce searching continously in the whole image

	//Indicates if other Viola based detectors are used (Hannes, own ...)
	bool	boUseAlternativeDetectors;
	bool	boDetectaElementosFacialesconViola;	//Active uses Viola detectors


	bool	boUsePreviousDetection;				//Use previous detection info to speed up the process in the new frame

	//Learning
	bool	boLearningLive;	//Active indicates that detection threads exemplars are stored during detection


	//Elements search
	bool	boSearchMouth;	//If active, the detector searches the mouth
	bool	boSearchNose;	//If active, the detector searches the nose
	bool	boSearchEyesusingGray;	//If active, the detector searahces the eyes using gray levels too

	//Others
	bool	boSearchLegs;   //If active searches lower body

	//End configuration flags

	//IplImages required
	IplImage	*image;					//Work copy of input image
	IplImage	*imagerot;				//Rotated input image
	IplImage	*dummy;					//Aux
	IplImage	*dummygray;				//Aux
	IplImage	*imagegray;				//Input image converted to gray image
	IplImage	*imagegrayIN;			//Copy of gray input image
	IplImage	*imagelbp;				//lbp image of the input image
	IplImage	*imagegrayINprev;		//Previous gray image
	IplImage	*imageINr_norm;
	IplImage	*imageINg_norm;			//normalized red and green images of input image
	IplImage	*prev_pyramid,*pyramid;	//Pyramids used for LK tracker

#ifdef USESEGMENTATION
	IplImage	*imagesegmented;		//For Luis' segmentation
#endif



	//Aux variables
	IplImage	*EllipseMask;						//Used for applying a mask to normalized face images

	CSkinBlobInfo	*Blob,*Blob4;					//Color blobs info

	CvFont	stcvFont;						//Font used to visualize results


	//Viola based detectors
	CViolaDetector		*ViolaFullBodyDetector;
	CViolaDetector		*ViolaHSDetector;
//	CViolaDetector		*ViolaUpperBodyDetector;
	CViolaDetector		*ViolaLowerBodyDetector;
	CViolaDetector		*ViolaProfileDetector;
	CViolaDetector		*ViolaFaceDetector;
//	CViolaDetector		*ViolaBasedOwnFaceDetector;
	CViolaDetector		*ViolaEyePairDetector;
//	CViolaDetector		*ViolaParOjosDetector;
	CViolaDetector		*ViolaLeftEyeDetector;
	CViolaDetector		*ViolaRightEyeDetector;
#ifndef ENCARA2INLINUX
//	CViolaDetector		*ViolaUMunichEyeDetector;
#endif
	CViolaDetector		*ViolaMouthDetector;
	CViolaDetector		*ViolaNoseDetector;

	CvMemStorage		*ViolaStorage;

		//Facial data corresponding to previous image and auxiliares
	CFacialDataperImage	DatosFaciales,DatosFacialesLost,
		DatosFacialesPrevios,DatosFacialesAux;



	long t;											//Time needed to process the image

	//Auxiliar variables

	int		ids_sofar;

	//Tracking
	//Indicating if any of these elemens was lost by the tracking module
	int BocaCorrLost;
	int OjosCorrLost;
	int NarizCorrLost;
	//bool boCoherenciaTresEltos;	//Indicates if three elements were tracked

	int corr_odx,corr_ody,corr_oix,corr_oiy;		//Eye positions returned by different routines
	int oix_in,oiy_in,odx_in,ody_in;				//Eyes iris

	//Integral projections
	int facevm_y[MAXNUMPROYECCIONES],facevm_val[MAXNUMPROYECCIONES],nvm;		//mínimos verticales
	int facevmI_y[MAXNUMPROYECCIONES],facevmI_val[MAXNUMPROYECCIONES],nvmI;		//mínimos verticales parte izquierda
	int facevmD_y[MAXNUMPROYECCIONES],facevmD_val[MAXNUMPROYECCIONES],nvmD;		//mínimos verticales parte derecha
	int facehm_x[MAXNUMPROYECCIONES],facehm_val[MAXNUMPROYECCIONES],nhm;		//mínimos horizontales
	int facevx_y;

	//! Array used to indicate if one detection is overlapped by any other
	bool *boRedundante;				//Indicates is a detection is contained in another

	//! Aux variable used to perform some actions with IplImages
	CIplImageUtils	IplUtils;		//IplImage utilities

	CImageUtils	ImageUtils;		//IplImage utilities

	CMathUtils	MathUtils;			//! Aux variable for Math utilities

	CGaborW		GaborUtils;			//! Gabor filter utilities


	CPCAUtils	*PCARepFaceQuore;	//PCA representation for inner face QUOREX x QUOREY

	CPCAUtils *PCALEye,*PCAREye,*PCAEyePair;//PCA Representation

	CSVMUtils *SVMFace;

	//Facial elements Likelihood
	IplImage	*NoseLikelihood,	//! Nose likelihood for a normalized image
				*MouthLikelihood;	//! Mouth likelihood for a normalized image

	//End auxiliar

#ifdef USESEGMENTATION
	//Segmentation Luis
	Segmenta	*SEG;
#endif
};

#endif
