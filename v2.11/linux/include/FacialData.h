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
 #if ! defined ( FACIALDATA_H )
#define FACIALDATA_H

//Constants
#include "ENCARA2_2lib_defs.h"
#include "SVMUtils.h"

#include "MathUtils.h"
#include "IplImageUtils.h"
#include "LBP.h"

//Variables needed to enclose tracking information
#include "Tracking_defs.h"
#include "OpenTracking.h"


#define STRINGLEN	32

#define INLBP //Si activo la imagen normalizada para iluminación la obtiene por LBP en lugar del trabajo prl



//!	Exemplars information
/*!
	Contains information about the exemplars extracted during an interaction session for a detection thread. Therefore each detection thread will have
	its own set of exemplars.
*/
class CExemplars
{
public:
	//!	Class constructor
	/*!
		Sets everything to zero
	*/
	CExemplars()
	{
		//Initializes the number of exemplars stored
		iNImages=0;
		fAvg_PCARecError_Quore=0.f;
	}

	//!	Class destructor
	/*!
		Removes used memory
	*/
	~CExemplars()
	{
		//Frees memory
		FreeMem();	
	}

	

	
	//!	Fuses two exemplars structures
	/*!
		Fuses into this exemplar structure two exemplars structures
		\param A First exemplar structure
		\param Z Second exemplar structure
	*/
	void FuseExemplars(CExemplars *A,CExemplars *Z)
	{
		FreeMem();

		//Sets the number of exemplars
		iNImages=A->iNImages+Z->iNImages;	

		//New PCA reconstruction error average combining both detection threads
		fAvg_PCARecError_Quore=(float)(A->fAvg_PCARecError_Quore*A->iNImages+Z->fAvg_PCARecError_Quore*Z->iNImages)/(float)(iNImages);

		AllocExemplars();

		//Copies exemplars data
		int nadded=0;
		int i;

		for (i=0;i<A->iNImages;i++)
		{
			persistence[nadded]=A->persistence[i];
			timestamp[nadded]=A->timestamp[i];
			Id[nadded]=(char *)new unsigned char[sizeof(char)*STRINGLEN];
			strcpy(Id[nadded],A->Id[i]);
			Idauth[nadded]=(char *)new unsigned char[sizeof(char)*STRINGLEN];
			strcpy(Idauth[nadded],A->Idauth[i]);
			Gender[nadded]=(char *)new unsigned char[sizeof(char)*STRINGLEN];
			strcpy(Gender[nadded],A->Gender[i]);
			bogenderlabelled[nadded]=A->bogenderlabelled[i];
			boidlabelled[nadded]=A->boidlabelled[i];
			boauthlabelled[nadded]=A->boauthlabelled[i];
			genderclass[nadded]=A->genderclass[i];
			idclass[nadded]=A->idclass[i];
			authclass[nadded]=A->authclass[i];
			StableImage[nadded]=cvCreateImage(cvSize(A->StableImage[i]->width,A->StableImage[i]->height),A->StableImage[i]->depth,A->StableImage[i]->nChannels);
			cvCopyImage(A->StableImage[i],StableImage[nadded]);
			StableImageAvg[nadded]=cvCreateImage(cvSize(A->StableImageAvg[i]->width,A->StableImageAvg[i]->height),A->StableImageAvg[i]->depth,A->StableImageAvg[i]->nChannels);
			cvCopyImage(A->StableImageAvg[i],StableImageAvg[nadded]);/**/
			StableImageB[nadded]=cvCreateImage(cvSize(A->StableImageB[i]->width,A->StableImageB[i]->height),A->StableImageB[i]->depth,A->StableImageB[i]->nChannels);
			cvCopyImage(A->StableImageB[i],StableImageB[nadded]);
			StableImagewithContext[nadded]=cvCreateImage(cvSize(A->StableImagewithContext[i]->width,A->StableImagewithContext[i]->height),A->StableImagewithContext[i]->depth,A->StableImagewithContext[i]->nChannels);
			cvCopyImage(A->StableImagewithContext[i],StableImagewithContext[nadded]);
			StableImageIN[nadded]=cvCreateImage(cvSize(A->StableImageIN[i]->width,A->StableImageIN[i]->height),A->StableImageIN[i]->depth,A->StableImageIN[i]->nChannels);
			cvCopyImage(A->StableImageIN[i],StableImageIN[nadded]);
			StableImageEyes[nadded]=cvCreateImage(cvSize(A->StableImageEyes[i]->width,A->StableImageEyes[i]->height),A->StableImageEyes[i]->depth,A->StableImageEyes[i]->nChannels);
			cvCopyImage(A->StableImageEyes[i],StableImageEyes[nadded]);
			StableImageMouth[nadded]=cvCreateImage(cvSize(A->StableImageMouth[i]->width,A->StableImageMouth[i]->height),A->StableImageMouth[i]->depth,A->StableImageMouth[i]->nChannels);
			cvCopyImage(A->StableImageMouth[i],StableImageMouth[nadded]);
			if (A->THist[i])
				cvCopyHist(A->THist[i],&THist[nadded]);
			else
				THist[nadded]=NULL;
			//Los intercambia porque sé que el originl se libera luego
			float *faux=A->FaceHist[i];;
			A->FaceHist[i]=FaceHist[nadded];
			FaceHist[nadded]=faux;
			boHistogramAvailable[nadded]=A->boHistogramAvailable[i];
			fPCARecError_Quore[nadded]=A->fPCARecError_Quore[i];
			iLBPSum[nadded]=A->iLBPSum[i];

			nadded++;
		}

		for (i=0;i<Z->iNImages;i++)
		{
			persistence[nadded]=Z->persistence[i];
			timestamp[nadded]=Z->timestamp[i];
			Id[nadded]=(char *)new unsigned char[sizeof(char)*STRINGLEN];
			strcpy(Id[nadded],Z->Id[i]);
			Idauth[nadded]=(char *)new unsigned char[sizeof(char)*STRINGLEN];
			strcpy(Idauth[nadded],Z->Idauth[i]);
			Gender[nadded]=(char *)new unsigned char[sizeof(char)*STRINGLEN];
			strcpy(Gender[nadded],Z->Gender[i]);
			bogenderlabelled[nadded]=Z->bogenderlabelled[i];
			boidlabelled[nadded]=Z->boidlabelled[i];
			boauthlabelled[nadded]=Z->boauthlabelled[i];
			genderclass[nadded]=Z->genderclass[i];
			idclass[nadded]=Z->idclass[i];
			authclass[nadded]=Z->authclass[i];
			StableImage[nadded]=cvCreateImage(cvSize(Z->StableImage[i]->width,Z->StableImage[i]->height),Z->StableImage[i]->depth,Z->StableImage[i]->nChannels);
			cvCopyImage(Z->StableImage[i],StableImage[nadded]);
			StableImageAvg[nadded]=cvCreateImage(cvSize(Z->StableImageAvg[i]->width,Z->StableImageAvg[i]->height),Z->StableImageAvg[i]->depth,Z->StableImageAvg[i]->nChannels);
			cvCopyImage(Z->StableImageAvg[i],StableImageAvg[nadded]);/**/
			StableImageB[nadded]=cvCreateImage(cvSize(Z->StableImageB[i]->width,Z->StableImageB[i]->height),Z->StableImageB[i]->depth,Z->StableImageB[i]->nChannels);
			cvCopyImage(Z->StableImageB[i],StableImageB[nadded]);
			StableImagewithContext[nadded]=cvCreateImage(cvSize(Z->StableImagewithContext[i]->width,Z->StableImagewithContext[i]->height),Z->StableImagewithContext[i]->depth,Z->StableImagewithContext[i]->nChannels);
			cvCopyImage(Z->StableImagewithContext[i],StableImagewithContext[nadded]);
			StableImageIN[nadded]=cvCreateImage(cvSize(Z->StableImageIN[i]->width,Z->StableImageIN[i]->height),Z->StableImageIN[i]->depth,Z->StableImageIN[i]->nChannels);
			cvCopyImage(Z->StableImageIN[i],StableImageIN[nadded]);
			StableImageEyes[nadded]=cvCreateImage(cvSize(Z->StableImageEyes[i]->width,Z->StableImageEyes[i]->height),Z->StableImageEyes[i]->depth,Z->StableImageEyes[i]->nChannels);
			cvCopyImage(Z->StableImageEyes[i],StableImageEyes[nadded]);
			StableImageMouth[nadded]=cvCreateImage(cvSize(Z->StableImageMouth[i]->width,Z->StableImageMouth[i]->height),Z->StableImageMouth[i]->depth,Z->StableImageMouth[i]->nChannels);
			cvCopyImage(Z->StableImageMouth[i],StableImageMouth[nadded]);
			if (Z->THist[i])
				cvCopyHist(Z->THist[i],&THist[nadded]);
			else
				THist[nadded]=NULL;
			//Los intercambia porque sé que el originl se libera luego
			float *faux=Z->FaceHist[i];;
			Z->FaceHist[i]=FaceHist[nadded];
			FaceHist[nadded]=faux;
			boHistogramAvailable[nadded]=Z->boHistogramAvailable[i];
			fPCARecError_Quore[nadded]=Z->fPCARecError_Quore[i];
			iLBPSum[nadded]=Z->iLBPSum[i];

			nadded++;
		}
	}


	//!	Exchanges values among the given exemplars structure and the local
	/*!
		Exchanges exemplar info
		\param e Source exemplar structure

	*/
	void ExchangeExemplarsData(CExemplars *e)
	{
		int	val=iNImages;
		iNImages=e->iNImages;
		e->iNImages=val;

		int *iaux=persistence;
		persistence=e->persistence;
		e->persistence=iaux;

		long *laux=timestamp;
		timestamp=e->timestamp;
		e->timestamp=laux;

		char **caux=Id;
		Id=e->Id;
		e->Id=caux;

		caux=Idauth;
		Idauth=e->Idauth;
		e->Idauth=caux;

		caux=Gender;
		Gender=e->Gender;
		e->Gender=caux;

		//labelled
		bool *boaux=bogenderlabelled;
		bogenderlabelled=e->bogenderlabelled;
		e->bogenderlabelled=boaux;

		boaux=boidlabelled;
		boidlabelled=e->boidlabelled;
		e->boidlabelled=boaux;

		boaux=boauthlabelled;
		boauthlabelled=e->boauthlabelled;
		e->boauthlabelled=boaux;

		iaux=genderclass;
		genderclass=e->genderclass;
		e->genderclass=iaux;

		iaux=idclass;
		idclass=e->idclass;
		e->idclass=iaux;

		iaux=authclass;
		authclass=e->authclass;
		e->authclass=iaux;


		//Whole face
		IplImage **iplaux=StableImage;
		StableImage=e->StableImage;
		e->StableImage=iplaux;

		//Whole face avegared
		iplaux=StableImageAvg;
		StableImageAvg=e->StableImageAvg;
		e->StableImageAvg=iplaux;/**/


		//Whole face with context but masked
		iplaux=StableImageB;
		StableImageB=e->StableImageB;
		e->StableImageB=iplaux;

		//Whole face with context
		iplaux=StableImagewithContext;
		StableImagewithContext=e->StableImagewithContext;
		e->StableImagewithContext=iplaux;

		//Whole face illumination normalized
		iplaux=StableImageIN;
		StableImageIN=e->StableImageIN;
		e->StableImageIN=iplaux;

		//eyes
		iplaux=StableImageEyes;
		StableImageEyes=e->StableImageEyes;
		e->StableImageEyes=iplaux;

		//mouth area
		iplaux=StableImageMouth;
		StableImageMouth=e->StableImageMouth;
		e->StableImageMouth=iplaux;

		//Torso Histogram
		CvHistogram	**Histaux=THist;
		THist=e->THist;
		e->THist=Histaux;

		//Face Histogram
		float **FHistaux=FaceHist;
		FaceHist=e->FaceHist;
		e->FaceHist=FHistaux;

		boaux=boHistogramAvailable;	
		boHistogramAvailable=e->boHistogramAvailable;
		e->boHistogramAvailable=boaux;

		//PCA error
		float *pfaux=fPCARecError_Quore;
		fPCARecError_Quore=e->fPCARecError_Quore;
		e->fPCARecError_Quore=pfaux;

		float faux=fAvg_PCARecError_Quore;
		fAvg_PCARecError_Quore=e->fAvg_PCARecError_Quore;
		e->fAvg_PCARecError_Quore=faux;

		laux=iLBPSum;
		iLBPSum=e->iLBPSum;
		e->iLBPSum=laux;
	}

	
	//!	Allocs memory for a new exemplar in current structure
	/*!
		Allocs memory for a new exemplar in current CExemplars structure
	*/
	void AllocNewExemplar()
	{
		int iNImages_old=iNImages;
		int i;

		//If there are data already copies them (the pointers) on a temporary vars
		int			*persistence_aux,*genderclass_aux,*idclass_aux,*authclass_aux;		
		long		*timestamp_aux;		
		char		**Id_aux,**Idauth_aux,**Gender_aux;		
		bool		*bogenderlabelled_aux,*boidlabelled_aux,*boauthlabelled_aux,*boHistogramAvailable_aux;
		IplImage	**StableImage_aux,**StableImageAvg_aux,**StableImageB_aux,**StableImagewithContext_aux,**StableImageIN_aux,**StableImageEyes_aux,**StableImageMouth_aux;	
		CvHistogram	**Hist_aux;	
		float		**FHist_aux;	
		float		*fPCARecError_Quore_aux,fAvg_PCARecError_Quore_aux;	
		long		*iLBPSum_aux;

		//Pointers are copied
		if (iNImages_old)
		{
			persistence_aux=persistence;
			timestamp_aux=timestamp;
			Id_aux=Id;
			Idauth_aux=Idauth;
			Gender_aux=Gender;
			bogenderlabelled_aux=bogenderlabelled;
			boidlabelled_aux=boidlabelled;
			boauthlabelled_aux=boauthlabelled;
			genderclass_aux=genderclass;
			idclass_aux=idclass;
			authclass_aux=authclass;
			StableImage_aux=StableImage;
			StableImageAvg_aux=StableImageAvg;
			StableImageB_aux=StableImageB;
			StableImagewithContext_aux=StableImagewithContext;
			StableImageIN_aux=StableImageIN;
			StableImageEyes_aux=StableImageEyes;
			StableImageMouth_aux=StableImageMouth;
			Hist_aux=THist;
			FHist_aux=FaceHist;
			boHistogramAvailable_aux=boHistogramAvailable;
			fPCARecError_Quore_aux=fPCARecError_Quore;
			fAvg_PCARecError_Quore_aux=fAvg_PCARecError_Quore;
			iLBPSum_aux=iLBPSum;
		}		

		//Increases exemplars index
		iNImages++;			

		//Allocs memory for the new number of exemplars
		AllocExemplars();

		//Copies info stored in temporal structure
		if (iNImages_old)
		{
			for (i=0;i<iNImages_old;i++)
			{
				persistence[i]=persistence_aux[i];
				timestamp[i]=timestamp_aux[i];
				Id[i]=Id_aux[i];
				Idauth[i]=Idauth_aux[i];
				Gender[i]=Gender_aux[i];
				bogenderlabelled[i]=bogenderlabelled_aux[i];
				boidlabelled[i]=boidlabelled_aux[i];
				boauthlabelled[i]=boauthlabelled_aux[i];
				genderclass[i]=genderclass_aux[i];
				idclass[i]=idclass_aux[i];
				authclass[i]=authclass_aux[i];
				StableImage[i]=StableImage_aux[i];
				StableImageAvg[i]=StableImageAvg_aux[i];
				StableImageB[i]=StableImageB_aux[i];
				StableImagewithContext[i]=StableImagewithContext_aux[i];
				StableImageIN[i]=StableImageIN_aux[i];
				StableImageEyes[i]=StableImageEyes_aux[i];
				StableImageMouth[i]=StableImageMouth_aux[i];
				THist[i]=Hist_aux[i];
				FaceHist[i]=FHist_aux[i];
				boHistogramAvailable[i]=boHistogramAvailable_aux[i];
				fPCARecError_Quore[i]=fPCARecError_Quore_aux[i];
				iLBPSum[i]=iLBPSum_aux[i];
			}
			fAvg_PCARecError_Quore=fAvg_PCARecError_Quore_aux;

			//Finally deletes temporary memory
			delete [] persistence_aux;
			delete [] timestamp_aux;
			delete [] Id_aux;
			delete [] Idauth_aux;
			delete [] Gender_aux;
			delete [] bogenderlabelled_aux;
			delete [] boidlabelled_aux;
			delete [] boauthlabelled_aux;
			delete [] genderclass_aux;
			delete [] idclass_aux;
			delete [] authclass_aux;
			delete [] StableImage_aux;
			delete [] StableImageAvg_aux;
			delete [] StableImageB_aux;
			delete [] StableImagewithContext_aux;
			delete [] StableImageIN_aux;
			delete [] StableImageEyes_aux;
			delete [] StableImageMouth_aux;
			delete [] Hist_aux;
			delete [] FHist_aux;
			delete [] boHistogramAvailable_aux;
			delete [] fPCARecError_Quore_aux;
			delete [] iLBPSum_aux;
		}
	}

	//!	Sets exemplars values
	/*!
		Sets exemplars values
		\param hIN	Input color image necessary to extract the exemplar histogram
		\param NormalizedFace Normalized face image
		\param NormalizedFacewithContext Normalized face image with context
		\param NormalizedFacewithContextMasked Normalized face image with context but masked
		\param NormalizedFaceIN Normalized face image with illumination normalization
		\param BreastCont Rectangle defining the area to be considered for the histogram computation
		\param validhist If set the histogram is computed
		\param BHist Breast area histogram
		\param PCARepFaceQuore PCA based representation space
	*/
	
	void SetExemplarData(IplImage *hIN,IplImage *NormalizedFace,IplImage *NormalizedFacewithContext,
		IplImage *NormalizedFacewithContextMasked,IplImage *NormalizedFaceIN,
		CvRect BreastCont,bool validhist,CvHistogram *BHist, CPCAUtils *PCARepFaceQuore)
	{
		int nStable=iNImages-1;
		
		persistence[nStable]=0;
		timestamp[nStable]=clock();

		//Allocs memory if needed
		if (Id[nStable]==NULL)
			Id[nStable]=(char *)new unsigned char[sizeof(char)*STRINGLEN];
		if (Idauth[nStable]==NULL)
			Idauth[nStable]=(char *)new unsigned char[sizeof(char)*STRINGLEN];
		if (Gender[nStable]==NULL)
			Gender[nStable]=(char *)new unsigned char[sizeof(char)*STRINGLEN];

		bogenderlabelled[nStable]=false;//No labels initially assigned
		boidlabelled[nStable]=false;
		boauthlabelled[nStable]=false;
		strcpy(Id[nStable],"None");
		strcpy(Idauth[nStable],"None");
		strcpy(Gender[nStable],"None");

		genderclass[nStable]=-1;
		idclass[nStable]=-1;
		authclass[nStable]=-1;

		SetExemplarImages(nStable,NormalizedFace,NormalizedFacewithContext,
		NormalizedFacewithContextMasked,NormalizedFaceIN,PCARepFaceQuore);

		//Obtiene el histograma del torso para este ejemplar
		if (validhist)
		{
			//Hist[nStable].GeneraHistogramas(hIN,BreastCont);
			cvCopyHist(BHist,&THist[nStable]);
			boHistogramAvailable[nStable]=true;
		}
		else
			boHistogramAvailable[nStable]=false;

	}


	void SetExemplarImages(int iEx, IplImage *NormalizedFace, IplImage *NormalizedFacewithContext, 
		IplImage *NormalizedFacewithContextMasked,IplImage *NormalizedFaceIN, CPCAUtils *PCARepFaceQuore)
	{
		int x1,y1,sx,sy;

		//Allocs the images if it is the first time
		if (persistence[iEx]==0)
		{
			//Creates the new image for the whole normalized face masked
			if (StableImage[iEx]==NULL)
				StableImage[iEx]=
				cvCreateImage(cvSize(NormalizedFace->width,NormalizedFace->height),NormalizedFace->depth,NormalizedFace->nChannels);

				//Creates the new image to keep the average of the whole normalized face masked
			if (StableImageAvg[iEx]==NULL)
				StableImageAvg[iEx]=
				cvCreateImage(cvSize(NormalizedFace->width,NormalizedFace->height),NormalizedFace->depth,NormalizedFace->nChannels);
	
			//Creates the new image to copy the normalized face with context masked
			if (StableImageB[iEx]==NULL)
				StableImageB[iEx]=
				cvCreateImage(cvSize(NormalizedFacewithContextMasked->width,NormalizedFacewithContextMasked->height),NormalizedFacewithContextMasked->depth,NormalizedFacewithContextMasked->nChannels);

			//Creates the new image to copy the normalized face with context
			if (StableImagewithContext[iEx]==NULL)
				StableImagewithContext[iEx]=
				cvCreateImage(cvSize(NormalizedFacewithContext->width,NormalizedFacewithContext->height),NormalizedFacewithContext->depth,NormalizedFacewithContext->nChannels);

			//Ilumination normalized
			if (StableImageIN[iEx]==NULL)
				StableImageIN[iEx]=
				cvCreateImage(cvSize(NormalizedFace->width,NormalizedFace->height),NormalizedFace->depth,NormalizedFace->nChannels);

			//Eyes area
			sx=GLASSESSX;sy=GLASSESSY;
			if (StableImageEyes[iEx]==NULL)
				StableImageEyes[iEx]=cvCreateImage( cvSize(sx,sy),NormalizedFace->depth,NormalizedFace->nChannels);	

			//Boca
			x1=MOUSTACHEX;y1=MOUSTACHEY;sx=FACEX-x1*2;sy=MOUSTACHESY;
			if (StableImageMouth[iEx]==NULL)
				StableImageMouth[iEx]=cvCreateImage( cvSize(sx,sy),NormalizedFace->depth,NormalizedFace->nChannels);	

			iLBPSum[iEx]=(long)(10e8);
		}

		//Computes the focus measure based on the LBP image
		
		cvSetImageROI(StableImageIN[iEx],cvRect((FACEX-FACEQUOREX)/2,(FACEY-FACEQUOREY)/2,FACEQUOREX, FACEQUOREY));	
		CvScalar iFocus=cvSum(StableImageIN[iEx]);//Si es SIMPLIFIED los valores están multiplicados por 16, por lo tanto también la suma
		cvResetImageROI(StableImageIN[iEx]);
		

		
		//Updates the images if the normalized 
		if (iFocus.val[0]<iLBPSum[iEx] || persistence[iEx]==0)
		{	 

			//Focus measure for the current exemplar
			iLBPSum[iEx]=(long)iFocus.val[0];

			//PCA error
			//If the PCA set is available, computes the PCA reconstruction error for the exemplar
			double PCAError=(double)-1;//initializes the PCA error, just in case no PCA space is available

			if (PCARepFaceQuore->PCAspace)
			{
				//Computes PCA reconstruction error
				IplImage *quore=cvCreateImage(cvSize(FACEQUOREX, FACEQUOREY),IPL_DEPTH_8U, 1);

				//Copies inner face area (extracted using a ROI) to IplImage quore
				cvSetImageROI(NormalizedFace,cvRect((FACEX-FACEQUOREX)/2,(FACEY-FACEQUOREY)/2,FACEQUOREX, FACEQUOREY));	
			
				cvCopyImage(NormalizedFace,quore);

				//Resets the ROI
				cvResetImageROI(NormalizedFace);

				//Gets PCA error
				PCAError=PCARepFaceQuore->PCAspace->GetPCAReconstructionError(quore);

				cvReleaseImage(&quore);
				//Fin PCA reconstrucion error
			}

			//Inicializa o actualiza en error PCA
			fPCARecError_Quore[iEx]=(float)PCAError;

			//Copies the Normalized image
			cvCopyImage(NormalizedFace,StableImage[iEx]);

			//Copies the first instance of the exemplar
			cvCopyImage(NormalizedFace,StableImageAvg[iEx]);

			//Copies the image with context
			cvCopyImage(NormalizedFacewithContextMasked,StableImageB[iEx]);

			//Normalized with context
			cvCopyImage(NormalizedFacewithContext,StableImagewithContext[iEx]);

			//Illumination normalized
			cvCopyImage(NormalizedFaceIN,StableImageIN[iEx]);

			//Creates the new image for the eyes area
			x1=GLASSESX;y1=GLASSESY;sx=GLASSESSX;sy=GLASSESSY;
			cvSetImageROI(NormalizedFace,cvRect(x1,y1,sx,sy));		
			cvCopyImage(NormalizedFace,StableImageEyes[iEx]);
			cvResetImageROI(NormalizedFace);

			//Creates the new image for the mouth area
			x1=MOUSTACHEX;y1=MOUSTACHEY;sx=FACEX-x1*2;sy=MOUSTACHESY;
			cvSetImageROI(NormalizedFace,cvRect(x1,y1,sx,sy));
			cvCopyImage(NormalizedFace,StableImageMouth[iEx]);
			cvResetImageROI(NormalizedFace);

			//Establecer el histograma de la cara (usa sólo la parte superior)
			CLBP m_lbp;
			m_lbp.GetLBPHistogramFromLBPf(StableImageIN[iEx],0,0,StableImageIN[iEx]->width,StableImageIN[iEx]->height/2,&(FaceHist[iEx]));// al dividir entre dos se queda sólo con la parte superior ...

			//Actualiza el promedio de error PCA
			if (iEx)//no es el primer ejemplar
			{
				//Calculamos la media de forma incremental
				fAvg_PCARecError_Quore=0;
				for (int ii=0;ii<iEx;ii++)
					fAvg_PCARecError_Quore+=fPCARecError_Quore[ii];

				fAvg_PCARecError_Quore/=iEx;
			}
			else//Primer ejemplar
				fAvg_PCARecError_Quore=fPCARecError_Quore[0];

		}

		
	
		

	}


	//!	Increases exemplar persistence
	/*!
		Increases persistence value for current exemplar (the last one inserted)
	*/
	void IncreasePersistence()
	{
		persistence[iNImages-1]++;
	}

	void UpdateAvg(IplImage *newNormalized)
	{
		//Updates the stable image average
		//If already an image has been stored for this exemplar
		if (persistence[iNImages-1])
		{
			IplImage *iplaux=cvCloneImage(StableImageAvg[iNImages-1]);

			//Media incremental
			cvAddWeighted(iplaux,
				((float)persistence[iNImages-1])/((float)persistence[iNImages-1]+1),
				newNormalized,(1.f)/((float)persistence[iNImages-1]+1),
				0.f,StableImageAvg[iNImages-1]);

			cvReleaseImage(&iplaux);		
		}
	}

	//!	Resets labels
	/*!
		Resets exemplar labels related to appearance
	*/
	void ResetLabels()
	{
		int i;

		for (i=0;i<iNImages;i++)
		{
			bogenderlabelled[i]=false;
			boidlabelled[i]=false;
			boauthlabelled[i]=false;

			genderclass[i]=-1;
			idclass[i]=-1;
			authclass[i]=-1;

		}
	}


	//!	Removes used memory
	/*!
		Frees the information associated to the exemplars
	*/
	void FreeMem()
	{
		//Frees only if there are exemplars stored
		if (iNImages)
		{
			for (int j=0;j<iNImages;j++)
			{
				if (Id[j]) { delete [] Id[j]; Id[j]=NULL;}
				if (Idauth[j]) {delete [] Idauth[j]; Idauth[j]=NULL;}
				if (Gender[j]) {delete [] Gender[j]; Gender[j]=NULL;}
				if (StableImage[j]) {cvReleaseImage(&StableImage[j]); StableImage[j]=NULL;}
				if (StableImageAvg[j]) {cvReleaseImage(&StableImageAvg[j]); StableImageAvg[j]=NULL;}
				if (StableImageB[j]) {cvReleaseImage(&StableImageB[j]); StableImageB[j]=NULL;}
				if (StableImagewithContext[j]) {cvReleaseImage(&StableImagewithContext[j]); StableImagewithContext[j]=NULL;}
				if (StableImageIN[j]) {cvReleaseImage(&StableImageIN[j]); StableImageIN[j]=NULL;}
				if (StableImageEyes[j]) {cvReleaseImage(&StableImageEyes[j]); StableImageEyes[j]=NULL;}
				if (StableImageMouth[j]) {cvReleaseImage(&StableImageMouth[j]); StableImageMouth[j]=NULL;}
				if (THist[j]) {	cvReleaseHist(&THist[j]); THist[j]=NULL;}
				if (FaceHist[j]) {delete [] FaceHist[j]; FaceHist[j]=NULL;}
			}

			delete [] persistence;
			delete [] timestamp;
			delete [] Id;
			delete [] Idauth;
			delete [] Gender;
			delete [] bogenderlabelled;
			delete [] boidlabelled;
			delete [] boauthlabelled;
			delete [] genderclass;
			delete [] idclass;
			delete [] authclass;
			delete [] StableImage;
			delete [] StableImageAvg;
			delete [] StableImageB;
			delete [] StableImagewithContext;
			delete [] StableImageIN;
			delete [] StableImageEyes;
			delete [] StableImageMouth;
			delete [] THist;
			delete [] FaceHist;
			delete [] boHistogramAvailable;
			delete [] fPCARecError_Quore;
			delete [] iLBPSum;

			iNImages=0;
			fAvg_PCARecError_Quore=0.f;
		}
	}

	//Exemplars information
	//! Number of exemplars
	int			iNImages;		
	//! Array of exemplars persistence	
	int			*persistence;		
	//! Indicates the moment when each exemplar was added
	long		*timestamp;					

	//Exemplars normalized images
	//! Array of cropped face image exemplars
	IplImage	**StableImage;	
	//! Array of averaged cropped face image exemplars
	IplImage	**StableImageAvg;	
	//! Array of cropped face image exemplars including some additional area
	IplImage	**StableImageB;	
	//! Array of face images with context exemplars
	IplImage	**StableImagewithContext;	
	//! Array of cropped face image exemplars but illumination normalized 
	IplImage	**StableImageIN;		
	//! Array of eyes area image exemplars
	IplImage	**StableImageEyes;			
	//! Array of mouth area image exemplars
	IplImage	**StableImageMouth;			
	//! Array of torso histogram corresponding to each exemplar
	CvHistogram **THist;
	//! Array of face histogram, based on LBP, corresponding to each exemplar
	float **FaceHist;
	//!
	//! Set if the breast histogram associated to the exemplar has been computed
	bool		*boHistogramAvailable;		

	//Exemplars PCA recosntruction error info
	//! Contains the PCA reconstruction error using Faces_QUOREXQUOREY
	float		*fPCARecError_Quore;		
	//! Contains the average PCA reconstruction error using Faces_QUOREXQUOREY
	float		fAvg_PCARecError_Quore;		

	//! Focus measure for the exemplar based on the LBP image using Faces_QUOREXQUOREY
	long		*iLBPSum;

	//Recognition (not in use in detection demo)
	//! Identity assigned (if classified) for each exemplar
	char		**Id;				
	//! Label assigned (if classified and verified) for each exemplar
	char		**Idauth;					
	//! Gender assigned (if classified and verified) for each exemplar
	char		**Gender;					
	//! Set if the exemplar gender has been labelled
	bool		*bogenderlabelled;			
	//! Set if the exemplar id has been labelled
	bool		*boidlabelled;				
	//! Set if the exemplar auth has been labelled
	bool		*boauthlabelled;			
	//! Class assigned to the exemplar for gender
	int			*genderclass;				
	//! Class assigned to the exemplar for id
	int			*idclass;					
	//! Class assigned to the exemplar for auth
	int			*authclass;					


private:

	//!	Allocates memory for the exemplars
	/*!
		Allocs memory for iNImages exemplars
	*/
	void AllocExemplars()
	{
		int i;

		//Allocs memory for the exemplars
		persistence=(int *)new unsigned char[sizeof(int)*iNImages];
		timestamp=(long *)new unsigned char[sizeof(long)*iNImages];
		Id=(char **)new unsigned char[sizeof(char *)*iNImages];
		Idauth=(char **)new unsigned char[sizeof(char *)*iNImages];
		Gender=(char **)new unsigned char[sizeof(char *)*iNImages];
		bogenderlabelled=(bool *)new unsigned char[sizeof(bool)*iNImages];
		boidlabelled=(bool *)new unsigned char[sizeof(bool)*iNImages];
		boauthlabelled=(bool *)new unsigned char[sizeof(bool)*iNImages];
		genderclass=(int *)new unsigned char[sizeof(int)*iNImages];
		idclass=(int *)new unsigned char[sizeof(int)*iNImages];
		authclass=(int *)new unsigned char[sizeof(int)*iNImages];
		StableImage=(IplImage **)new unsigned char[sizeof(IplImage *)*iNImages];
		StableImageAvg=(IplImage **)new unsigned char[sizeof(IplImage *)*iNImages];
		StableImageB=(IplImage **)new unsigned char[sizeof(IplImage *)*iNImages];
		StableImagewithContext=(IplImage **)new unsigned char[sizeof(IplImage *)*iNImages];
		StableImageIN=(IplImage **)new unsigned char[sizeof(IplImage *)*iNImages];
		StableImageEyes=(IplImage **)new unsigned char[sizeof(IplImage *)*iNImages];
		StableImageMouth=(IplImage **)new unsigned char[sizeof(IplImage *)*iNImages];
		THist=(CvHistogram **)new unsigned char[sizeof(CvHistogram *)*iNImages];
		FaceHist=(float **)new unsigned char[sizeof(float *)*iNImages];
		boHistogramAvailable=(bool *)new unsigned char[sizeof(bool)*iNImages];
		fPCARecError_Quore=(float *)new unsigned char[sizeof(float)*iNImages];
		iLBPSum=(long *)new unsigned char[sizeof(long)*iNImages];

		for (i=0;i<iNImages;i++)
		{
			Id[i]=NULL;
			Idauth[i]=NULL;
			Gender[i]=NULL;
			StableImage[i]=NULL;
			StableImageAvg[i]=NULL;
			StableImageB[i]=NULL;
			StableImagewithContext[i]=NULL;
			StableImageIN[i]=NULL;
			StableImageEyes[i]=NULL;
			StableImageMouth[i]=NULL;
			THist[i]=NULL;
			FaceHist[i]=NULL;
			boHistogramAvailable[i]=false;
			iLBPSum[i]=(long)(10e8);
		}
	}
};



//!	Contains information related to a detection thread
/*!
	Information related to a single detection thread
*/
class CFacialData
{
public:
	//!	Class constructor
	/*!
	Initializes detectionthread information
	*/
	CFacialData()
	{
		//By default the facial elements are not available
		fb=hs=false;
		leye=false;
		reye=false;
		eyepair=false;
		mouth=false;
		mouthcontainer=false;
		nose=nosecontainer=false;

		//Torso
		boTorsoContainer=false;
		boTorsoHistAlreadyInitialized=false;
		TorsoHist=NULL;
		TorsoLBPHist=NULL;
		TorsoHistpersistence=0;
		SkinHist=NULL;
		SkinHistpersistence=0;

		//Color flags
		boColorBasedFound=false;
		boColorAlreadyInitialized=false;

		//Initial persistence is null
		persistence=0;

		//Life of a thread
		framestodie=0;

		//Initialization of the label assigned for this subject 
		strcpy(Label,"");
		strcpy(Id,"");
		strcpy(Gender,"");

		boSmiling=false;//default value

		//Eyes have not been detected for the current thread
		boEyesDetectedAtLeastOnceforThisFace=false;
		boStableTrackedFace=false;
		boStableTrackedFaceSaved=false;
		
		//Indicate that the normalized memory has not been allocated
		boiplFaceImagesNormalized=false;
		boNormalizedAvailable=false;

		//Tracking flags
		eyestracked=false;
		leyetracked=false;
		reyetracked=false;
		nosetracked=false;
		mouthtracked=false;		
		headtracked=false;
		iconttracked=0;

		boLEyeTracker=boREyeTracker=boLMouthTracker=boRMouthTracker=boNoseTracker=boHeadTracker=false;//No trackers available at the beginning


		//timestamps
		timestamp=-1;
		timestamplast=-1;

		//LK Tracker initialization (not in use)
		cornerCount=30;
		boLKTracker=false;
	}

	//!	Class destructor
	/*!
	Releases memory estructures
	*/
	~CFacialData()
	{
		//Frees memory if a normalization process was performed
		if (boiplFaceImagesNormalized)
		{
			cvReleaseImage(&iplFaceContextNormalized);
			cvReleaseImage(&iplFaceContextNormalizedMasked);
			cvReleaseImage(&iplFaceNormalized);
			cvReleaseImage(&iplFaceNormalizedIN);

			boiplFaceImagesNormalized=false;
		}

		//Frees Trackers
		if (boHeadTracker)
		{
			delete HeadTracker;
			boHeadTracker=false;
		}
		if (boLEyeTracker)
		{
			delete LEyeTracker;
			boLEyeTracker=false;
		}
		if (boREyeTracker)
		{
			delete REyeTracker;
			boREyeTracker=false;
		}
		if (boLMouthTracker)
		{
			delete LMouthTracker;
			boLMouthTracker=false;
		}
		if (boRMouthTracker)
		{
			delete RMouthTracker;
			boRMouthTracker=false;
		}
		if (boNoseTracker)
		{
			delete NoseTracker;
			boNoseTracker=false;
		}
		//En releases trackers


		//Histogramas
		//Torso
		if (TorsoHist)
		{
			cvReleaseHist(&TorsoHist);
			TorsoHist=NULL;
		}

		if (TorsoLBPHist)
		{
			delete [] TorsoLBPHist;
			TorsoLBPHist=NULL;
		}

		//Skin
		if (SkinHist)
		{
			cvReleaseHist(&SkinHist);
			SkinHist=NULL;
		}


		//Removes exemplars info created
		Exemplars.FreeMem();

	}

	
	//!	Classifies detection thread given a classifier (TBD)
	/*!
		\param Clasif	Classifier to be used
		\param suggestedlabel Label assigned
		\param Area2Use Indicates the area used for classification (face =0, with context=1, eyes=2 )
		\param Res2Use Indicates the resolution used for classification (fullres =0, half res=1, 1/4 res=2 )
		\param boId	The computed label affects the identity
		\param suggestedlabel1stage Label assigned by the first stage classifier
		\param boIlluminationNormalized Sets indicates the the classification is performed using the illumination normalized image
		\param boCompute Forces recomputing the label
	*/
	int Classify(CSVMUtils *Clasif,char * suggestedlabel,int Area2Use=0, int Res2Use=0, bool boId=true,
		char * suggestedlabel1stage=NULL, bool boIlluminationNormalized=false,bool boCompute=true)
	{
		int j,ni,clase,clasev;
		
		float *claseslk;
		IplImage *aux2=NULL,*aux4=NULL,*aux8=NULL;

		//Obtiene la persistencia, si es cero no tiene sentido continuar
		long totalpersistence=0;
		for (j=0;j<Exemplars.iNImages;j++)
		{
			totalpersistence+=Exemplars.persistence[j];
		}

printf("%d\n", totalpersistence);
		if (Clasif->boSVMAvailable && totalpersistence>0)
		{
printf("Disponible\n");
			int nclases=Clasif->ModelSVMGeneral->NumClasses;
			//Aloja un vector para acumular las probabilidades de asociación a una clase
			claseslk=(float *) new unsigned char[sizeof(float)*nclases];

			for (j=0;j<nclases;j++)
				claseslk[j]=0.f;

			//Computes the label for each stable pattern of this detection thread
			for (j=0;j<Exemplars.iNImages;j++)
			{
				bool boalreadycomputed;

				if (boId)// Computing id
				{
					boalreadycomputed=Exemplars.boidlabelled[j];
				}
				else	//Computing gender
				{
					boalreadycomputed=Exemplars.bogenderlabelled[j];
				}
				
				if (!boalreadycomputed || boCompute)
				{					
					if (boIlluminationNormalized)//De momento sólo para la cara normalizada
						clase=Clasif->IplImageSVMClassification(Exemplars.StableImageIN[j]);
					else
					{
						switch (Area2Use)
						{
						case 0://StableImage (falta tratar StableImageAvg)
							switch (Res2Use)
							{
							case 2:
								aux2=cvCreateImage(cvSize(Exemplars.StableImage[j]->width/2, Exemplars.StableImage[j]->height/2),Exemplars.StableImage[j]->depth, Exemplars.StableImage[j]->nChannels);

								cvPyrDown(Exemplars.StableImage[j],aux2);

								clase=Clasif->IplImageSVMClassification(aux2);

								cvReleaseImage(&aux2);
								break;
							case 4:
								aux2=cvCreateImage(cvSize(Exemplars.StableImage[j]->width/2, Exemplars.StableImage[j]->height/2),Exemplars.StableImage[j]->depth, Exemplars.StableImage[j]->nChannels);
								aux4=cvCreateImage(cvSize(Exemplars.StableImage[j]->width/4, Exemplars.StableImage[j]->height/4),Exemplars.StableImage[j]->depth, Exemplars.StableImage[j]->nChannels);

								cvPyrDown(Exemplars.StableImage[j],aux2);
								cvPyrDown(aux2,aux4);

								clase=Clasif->IplImageSVMClassification(aux4);

								cvReleaseImage(&aux2);
								cvReleaseImage(&aux4);
								break;
							case 8:
								aux2=cvCreateImage(cvSize(Exemplars.StableImage[j]->width/2, Exemplars.StableImage[j]->height/2),Exemplars.StableImage[j]->depth, Exemplars.StableImage[j]->nChannels);
								aux4=cvCreateImage(cvSize(Exemplars.StableImage[j]->width/4, Exemplars.StableImage[j]->height/4),Exemplars.StableImage[j]->depth, Exemplars.StableImage[j]->nChannels);
								aux8=cvCreateImage(cvSize(Exemplars.StableImage[j]->width/8, Exemplars.StableImage[j]->height/8),Exemplars.StableImage[j]->depth, Exemplars.StableImage[j]->nChannels);

								cvPyrDown(Exemplars.StableImage[j],aux2);
								cvPyrDown(aux2,aux4);
								cvPyrDown(aux4,aux8);

								clase=Clasif->IplImageSVMClassification(aux8);

								cvReleaseImage(&aux2);
								cvReleaseImage(&aux4);
								cvReleaseImage(&aux8);
								break;
							default:
							case 0:
								clase=Clasif->IplImageSVMClassification(Exemplars.StableImage[j]);
								break;
							
							}
							
							break;
						case 1://StableImagewithContext
							switch (Res2Use)
							{
							case 2:
								aux2=cvCreateImage(cvSize(Exemplars.StableImagewithContext[j]->width/2, Exemplars.StableImagewithContext[j]->height/2),Exemplars.StableImagewithContext[j]->depth, Exemplars.StableImagewithContext[j]->nChannels);

								cvPyrDown(Exemplars.StableImagewithContext[j],aux2);

								clase=Clasif->IplImageSVMClassification(aux2);

								cvReleaseImage(&aux2);
								break;
							case 4:
								aux2=cvCreateImage(cvSize(Exemplars.StableImagewithContext[j]->width/2, Exemplars.StableImagewithContext[j]->height/2),Exemplars.StableImagewithContext[j]->depth, Exemplars.StableImagewithContext[j]->nChannels);
								aux4=cvCreateImage(cvSize(Exemplars.StableImagewithContext[j]->width/4, Exemplars.StableImagewithContext[j]->height/4),Exemplars.StableImagewithContext[j]->depth, Exemplars.StableImagewithContext[j]->nChannels);

								cvPyrDown(Exemplars.StableImagewithContext[j],aux2);
								cvPyrDown(aux2,aux4);

								clase=Clasif->IplImageSVMClassification(aux4);

								cvReleaseImage(&aux2);
								cvReleaseImage(&aux4);
								break;
							case 8:
								aux2=cvCreateImage(cvSize(Exemplars.StableImagewithContext[j]->width/2, Exemplars.StableImagewithContext[j]->height/2),Exemplars.StableImagewithContext[j]->depth, Exemplars.StableImagewithContext[j]->nChannels);
								aux4=cvCreateImage(cvSize(Exemplars.StableImagewithContext[j]->width/4, Exemplars.StableImagewithContext[j]->height/4),Exemplars.StableImagewithContext[j]->depth, Exemplars.StableImagewithContext[j]->nChannels);
								aux8=cvCreateImage(cvSize(Exemplars.StableImagewithContext[j]->width/8, Exemplars.StableImagewithContext[j]->height/8),Exemplars.StableImagewithContext[j]->depth, Exemplars.StableImagewithContext[j]->nChannels);

								cvPyrDown(Exemplars.StableImagewithContext[j],aux2);
								cvPyrDown(aux2,aux4);
								cvPyrDown(aux4,aux8);

								clase=Clasif->IplImageSVMClassification(aux8);

								cvReleaseImage(&aux2);
								cvReleaseImage(&aux4);
								cvReleaseImage(&aux8);
								break;
							default:
							case 0:
								clase=Clasif->IplImageSVMClassification(Exemplars.StableImagewithContext[j]);
								break;							
							}
							
							break;
						case 2://StableImageB
							switch (Res2Use)
							{
							case 2:
								aux2=cvCreateImage(cvSize(Exemplars.StableImageB[j]->width/2, Exemplars.StableImageB[j]->height/2),Exemplars.StableImageB[j]->depth, Exemplars.StableImageB[j]->nChannels);

								cvPyrDown(Exemplars.StableImageB[j],aux2);

								clase=Clasif->IplImageSVMClassification(aux2);

								cvReleaseImage(&aux2);
								break;
							case 4:
								aux2=cvCreateImage(cvSize(Exemplars.StableImageB[j]->width/2, Exemplars.StableImageB[j]->height/2),Exemplars.StableImageB[j]->depth, Exemplars.StableImageB[j]->nChannels);
								aux4=cvCreateImage(cvSize(Exemplars.StableImageB[j]->width/4, Exemplars.StableImageB[j]->height/4),Exemplars.StableImageB[j]->depth, Exemplars.StableImageB[j]->nChannels);

								cvPyrDown(Exemplars.StableImageB[j],aux2);
								cvPyrDown(aux2,aux4);

								clase=Clasif->IplImageSVMClassification(aux4);

								cvReleaseImage(&aux2);
								cvReleaseImage(&aux4);
								break;
							case 8:
								aux2=cvCreateImage(cvSize(Exemplars.StableImageB[j]->width/2, Exemplars.StableImageB[j]->height/2),Exemplars.StableImageB[j]->depth, Exemplars.StableImageB[j]->nChannels);
								aux4=cvCreateImage(cvSize(Exemplars.StableImageB[j]->width/4, Exemplars.StableImageB[j]->height/4),Exemplars.StableImageB[j]->depth, Exemplars.StableImageB[j]->nChannels);
								aux8=cvCreateImage(cvSize(Exemplars.StableImageB[j]->width/8, Exemplars.StableImageB[j]->height/8),Exemplars.StableImageB[j]->depth, Exemplars.StableImageB[j]->nChannels);

								cvPyrDown(Exemplars.StableImageB[j],aux2);
								cvPyrDown(aux2,aux4);
								cvPyrDown(aux4,aux8);

								clase=Clasif->IplImageSVMClassification(aux8);

								cvReleaseImage(&aux2);
								cvReleaseImage(&aux4);
								cvReleaseImage(&aux8);
								break;
							default:
							case 0:
								clase=Clasif->IplImageSVMClassification(Exemplars.StableImageB[j]);
								break;
							
							}
							
							break;
						case 3:
							clase=Clasif->IplImageSVMClassification(Exemplars.StableImageEyes[j]);
							break;
						default:
							break;
						}
						
					}

					if (boId)
					{
						Exemplars.boidlabelled[j]=true;
						Exemplars.idclass[j]=clase;

						//Gives a label to each exemplar (according to the class assigned
						sprintf(Exemplars.Id[j],"%4d",clase);
					}
					else
					{
						Exemplars.bogenderlabelled[j]=true;
						Exemplars.genderclass[j]=clase;

						//Gives a label to each exemplar 
						strcpy(Exemplars.Gender[j],Clasif->ModelSVMGeneral->Label[clase-1]);
					}
				}
				else
				{
					if (boId)
					{
						clase=Exemplars.idclass[j];
					}
					else
					{
						clase=Exemplars.genderclass[j];
					}
				}
				//adds persistence to class returned
				claseslk[clase-1]+=Exemplars.persistence[j];
			}

			//Obtiene para el hilo de detección la clase más probable (en rango 1 ... N
			//Podemos atender también a aquellos más estables y cuyo PCA error no se pase ...
			float max=0;
			clase=0;
			for (ni=0;ni<nclases;ni++)
			{
				if (claseslk[ni]>max)
				{
					max=claseslk[ni];
					clase=ni+1;
				}
			}

			if (boId)//Verificación
			{//clase nos indica el clasificador de verificación a utilizar a continuación (so ya hay más de dos clases)

				if (suggestedlabel1stage)//keeps record of the first stage classification
					strcpy(suggestedlabel1stage,Clasif->ModelSVMGeneral->Label[clase-1]);
			
				//Only two classes
				int verified=0,unknown=0;

				//Computes the label for each stable pattern of this detection thread
				for (j=0;j<Exemplars.iNImages;j++)
				{
					//Gets the class
					if (boIlluminationNormalized)
						clasev=Clasif->IplImageSVMClassification(Exemplars.StableImageIN[j],clase);
					else
					{
						switch (Area2Use)
						{
						case 1:
							clasev=Clasif->IplImageSVMClassification(Exemplars.StableImagewithContext[j],clase);
							break;
						case 2:
							clasev=Clasif->IplImageSVMClassification(Exemplars.StableImageB[j],clase);
							break;
						case 3:
							clasev=Clasif->IplImageSVMClassification(Exemplars.StableImageEyes[j],clase);
							break;
						default:
						case 0:
							clasev=Clasif->IplImageSVMClassification(Exemplars.StableImage[j],clase);
							break;
						}
					}
						
		
					//Acumula votos para cada clase (1 clase correspondiente a la identidad elegida en la 
					//primera fase del clasificador entre n clases, y 2 el resto
					switch (clasev)
					{
					case 1://Confirma la clase
						verified+=Exemplars.persistence[j];	
						//Gives a label to each exemplar (according to the class assigned
						strcat(Exemplars.Idauth[j]," verified");

						break;
					default://No de la clase
						unknown+=Exemplars.persistence[j];	
						//Gives a label to each exemplar (according to the class assigned
						sprintf(Exemplars.Idauth[j],"unknown");

						break;
					}	
				}

				//Hacemos el recuento de los votos por ejemplar
				if (verified>unknown)//Lo confirma
				{
					//La clase sugerida, es la etiqueta de la clase 0 (la 1 es el resto al verificar)
					sprintf(suggestedlabel,"%05d",atoi(Clasif->ModelSVMGeneral->Label[clase-1]));

				}
				else//Lo rechaza como desconocido
				{					
					sprintf(suggestedlabel,"unknown");				
				}


			}//id
			else//gender
			{
				//Label associated to the whole detection thread
				strcpy(suggestedlabel,Clasif->ModelSVMGeneral->Label[clase-1]);
			}


			delete [] claseslk;

			return 1;
		}//Clasif available

		return -1;
	}


	//!	Extract the normalized images
	/*!
	Receives a caraxctx*carayctx BYTE image and stores the corresponding normalized images.
	\param img Unmasked input image with caraxctx*carayctx dimensions
	\param imgmasked Masked input image with caraxctx*carayctx dimensions
	\param imgIN Illumination normalized input image with caraxctx*carayctx dimensions
	\param caraxctx Width of the normalized image containing some context
	\param carayctx Height of the normalized image containing some context
	\param carax Width of the normalized image
	\param caray Height of the normalized image
	\param offxctx Relates the width of both images
	\param offyctx Relates the height of both images
	*/
	void CopyNormalizedImages(BYTE *img,BYTE *imgmasked,BYTE *imgIN,int caraxctx, int carayctx,int carax,int caray,
		int offxctx,int offyctx)
	{
		int i;

		//Allocs memory if necessary
		AllocFaceImages(caraxctx,carayctx,carax,caray);

		//Copies context image
        BYTE *dst=(BYTE *)iplFaceContextNormalized->imageData;
		BYTE *src=img;
		for (i=0;i<carayctx;i++,src+=caraxctx,dst+=iplFaceContextNormalized->widthStep)
			memcpy(dst,src,caraxctx);

		//Copies context image masked
        dst=(BYTE *)iplFaceContextNormalizedMasked->imageData;
		src=imgmasked;
		for (i=0;i<carayctx;i++,src+=caraxctx,dst+=iplFaceContextNormalizedMasked->widthStep)
			memcpy(dst,src,caraxctx);

		//Copies CARAX*CARAY image
        dst=(BYTE *)iplFaceNormalized->imageData;
		src=imgmasked+offyctx*caraxctx+offxctx;
		for (i=0;i<caray;i++,src+=caraxctx,dst+=iplFaceNormalized->widthStep)
			memcpy(dst,src,carax);

#ifdef INLBP //Mejor SIMPLIFIED_COMPACT
		CLBP lbp;

		//Converts the normalized image using LBP simplified LBP_SIMPLIFIED or other
		lbp.GetLBPImage(iplFaceNormalized,0,LBP_ORIGINAL,iplFaceNormalizedIN);//0 means no interpolated
		//lbp.GetLBPImage(iplFaceNormalized,0,LBP_SIMPLIFIED_COMPACT,iplFaceNormalizedIN);//0 means no interpolated

#else
		//Copies CARAX*CARAY image with illumination normalization
        dst=(BYTE *)iplFaceNormalizedIN->imageData;
		src=imgIN;
		for (i=0;i<caray;i++,src+=carax,dst+=iplFaceNormalizedIN->widthStep)
			memcpy(dst,src,carax);
#endif
	}


	//!	Copies Facial information
	/*!
	Copies all facial information contained in the given structure

	\param FacialData Facial information to be copied
	*/
	void CopyFacialData(CFacialData *FacialData)
	{
		//Containers
		x1=FacialData->x1;//Faces
		y1=FacialData->y1;
		x2=FacialData->x2;
		y2=FacialData->y2;

		hs=FacialData->hs;//HS
		hsx1=FacialData->hsx1;
		hsy1=FacialData->hsy1;
		hsx2=FacialData->hsx2;
		hsy2=FacialData->hsy2;

		fb=FacialData->fb;//FB
		fbx1=FacialData->fbx1;
		fby1=FacialData->fby1;
		fbx2=FacialData->fbx2;
		fby2=FacialData->fby2;

		lb=FacialData->lb;//LB
		lbx1=FacialData->lbx1;
		lby1=FacialData->lby1;
		lbx2=FacialData->lbx2;
		lby2=FacialData->lby2;

		eyepair=FacialData->eyepair;//Eyepair
		eyepairx1=FacialData->eyepairx1;
		eyepairy1=FacialData->eyepairy1;
		eyepairx2=FacialData->eyepairx2;
		eyepairy2=FacialData->eyepairy2;

		//Facial features
		leye=FacialData->leye;
		reye=FacialData->reye;

		e_rx=FacialData->e_rx;
		e_ry=FacialData->e_ry;
		e_lx=FacialData->e_lx;
		e_ly=FacialData->e_ly;

		nose=FacialData->nose;//nose
		np_x=FacialData->np_x;
		np_y=FacialData->np_y;
		nosecontainer=FacialData->nosecontainer;
		nosex1=FacialData->nosex1;
		nosey1=FacialData->nosey1;
		nosex2=FacialData->nosex2;
		nosey2=FacialData->nosey2;

		mouth=FacialData->mouth;//Mouth
		ml_x=FacialData->ml_x;
		ml_y=FacialData->ml_y;
		mr_x=FacialData->mr_x;
		mr_y=FacialData->mr_y;
		mouthcontainer=FacialData->mouthcontainer;
		mouthx1=FacialData->mouthx1;
		mouthy1=FacialData->mouthy1;
		mouthx2=FacialData->mouthx2;
		mouthy2=FacialData->mouthy2;

		//Color info
		rn_min=FacialData->rn_min;
		rn_max=FacialData->rn_max;
		gn_min=FacialData->gn_min;
		gn_max=FacialData->gn_max;
		boColorBasedFound=FacialData->boColorBasedFound;
		boColorAlreadyInitialized=FacialData->boColorAlreadyInitialized;

		boTorsoContainer=FacialData->boTorsoContainer;//Breast container
		boTorsoHistAlreadyInitialized=FacialData->boTorsoHistAlreadyInitialized;
		torsox1=FacialData->torsox1;
		torsoy1=FacialData->torsoy1;
		torsox2=FacialData->torsox2;
		torsoy2=FacialData->torsoy2;
		if (FacialData->TorsoHist)
			cvCopyHist(FacialData->TorsoHist,&TorsoHist);//In development

		//LBP histogram
		if (FacialData->TorsoLBPHist)
		{
			//Allocs the dst if still not created
			if (!TorsoLBPHist)
				TorsoLBPHist=(float *) new unsigned char [256*sizeof(float)];

			//Copies values
			for (int i=0;i<256;i++)
				TorsoLBPHist[i]=FacialData->TorsoLBPHist[i];
		}

        TorsoHistpersistence=FacialData->TorsoHistpersistence;
		if (FacialData->SkinHist)
			cvCopyHist(FacialData->SkinHist,&SkinHist);//In development
		SkinHistpersistence=FacialData->SkinHistpersistence;
	
		//Persistence
		persistence=FacialData->persistence;

		//Frames to die
		framestodie=FacialData->framestodie;

		//Have been eyes detected for this thread
		boEyesDetectedAtLeastOnceforThisFace=FacialData->boEyesDetectedAtLeastOnceforThisFace;

		//Tracking
		headtracked=FacialData->headtracked;
		iconttracked=FacialData->iconttracked;
		eyestracked=FacialData->eyestracked;
		leyetracked=FacialData->leyetracked;
		reyetracked=FacialData->reyetracked;
		mouthtracked=FacialData->mouthtracked;
		nosetracked=FacialData->nosetracked;

		//Whole face tracker scale
		escalaCaraf=FacialData->escalaCaraf;
		escalaCara=FacialData->escalaCara;

		//Trackers 
		bool boaux=boHeadTracker;			//Head/face
		boHeadTracker=FacialData->boHeadTracker;
		FacialData->boHeadTracker=boaux;

		COpenTracking *Taux=HeadTracker;//Intercambia
		HeadTracker=FacialData->HeadTracker;
		FacialData->HeadTracker=Taux;

		boaux=boLEyeTracker;			//LEye
		boLEyeTracker=FacialData->boLEyeTracker;
		FacialData->boLEyeTracker=boaux;

		Taux=LEyeTracker;
		LEyeTracker=FacialData->LEyeTracker;
		FacialData->LEyeTracker=Taux;

		boaux=boREyeTracker;			//REye
		boREyeTracker=FacialData->boREyeTracker;
		FacialData->boREyeTracker=boaux;

		Taux=REyeTracker;
		REyeTracker=FacialData->REyeTracker;
		FacialData->REyeTracker=Taux;

		boaux=boLMouthTracker;			//Mouth
		boLMouthTracker=FacialData->boLMouthTracker;
		FacialData->boLMouthTracker=boaux;

		Taux=LMouthTracker;
		LMouthTracker=FacialData->LMouthTracker;
		FacialData->LMouthTracker=Taux;

		boaux=boRMouthTracker;			
		boRMouthTracker=FacialData->boRMouthTracker;
		FacialData->boRMouthTracker=boaux;

		Taux=RMouthTracker;
		RMouthTracker=FacialData->RMouthTracker;
		FacialData->RMouthTracker=Taux;

		boaux=boNoseTracker;			//Nose
		boNoseTracker=FacialData->boNoseTracker;
		FacialData->boNoseTracker=boaux;

		Taux=NoseTracker;
		NoseTracker=FacialData->NoseTracker;
		FacialData->NoseTracker=Taux;

		//Label for subject
		strcpy(Label,FacialData->Label);

		//Appearance
		strcpy(Id,FacialData->Id);
		strcpy(Gender,FacialData->Gender);

		boSmiling=FacialData->boSmiling;

		//Normalized images
		boaux=boiplFaceImagesNormalized;
		boiplFaceImagesNormalized=FacialData->boiplFaceImagesNormalized;
		FacialData->boiplFaceImagesNormalized=boaux;

		boaux=boNormalizedAvailable;
		boNormalizedAvailable=FacialData->boNormalizedAvailable;
		FacialData->boNormalizedAvailable=boaux;

		IplImage *iplaux=iplFaceContextNormalized;
		iplFaceContextNormalized=FacialData->iplFaceContextNormalized;
		FacialData->iplFaceContextNormalized=iplaux;

		iplaux=iplFaceContextNormalizedMasked;
		iplFaceContextNormalizedMasked=FacialData->iplFaceContextNormalizedMasked;
		FacialData->iplFaceContextNormalizedMasked=iplaux;

		iplaux=iplFaceNormalized;
		iplFaceNormalized=FacialData->iplFaceNormalized;
		FacialData->iplFaceNormalized=iplaux;

		iplaux=iplFaceNormalizedIN;
		iplFaceNormalizedIN=FacialData->iplFaceNormalizedIN;
		FacialData->iplFaceNormalizedIN=iplaux;

		//Time stamps
		long laux=timestamp;
		timestamp=FacialData->timestamp;
		FacialData->timestamp=laux;

		laux=timestamplast;
		timestamplast=FacialData->timestamplast;
		FacialData->timestamplast=laux;

		//Stable patterns or exemplars
		boStableTrackedFace=FacialData->boStableTrackedFace;
		boStableTrackedFaceSaved=FacialData->boStableTrackedFaceSaved;
		//Stable images (copies pointers and resets the one copied)
		Exemplars.ExchangeExemplarsData(&FacialData->Exemplars);

		//LK tracker (not in use)
		boLKTracker=FacialData->boLKTracker;
		cornerCount=FacialData->cornerCount;
		for (int i=0;i<cornerCount;i++)
		{
			corners[i].x=FacialData->corners[i].x;
			corners[i].y=FacialData->corners[i].y;
		}
		cornerx=FacialData->cornerx;
		cornery=FacialData->cornery;
	}

private:
	//!	Allocs memory for the normalized images
	/*!
	Allocs memory for the normalized images extracted from current frame
	\param caraxctx Width of the normalized image containing some context
	\param carayctx Height of the normalized image containing some context
	\param carax Width of the normalized image
	\param carax Height of the normalized image
	*/
	void AllocFaceImages(int caraxctx, int carayctx,int carax,int caray)
	{
		//If the memory has not been already allocated
		if (!boiplFaceImagesNormalized)
		{
			//Allocs memory
			iplFaceContextNormalized = cvCreateImage( cvSize(caraxctx,carayctx), IPL_DEPTH_8U, 1 );
			iplFaceContextNormalizedMasked = cvCreateImage( cvSize(caraxctx,carayctx), IPL_DEPTH_8U, 1 );
			iplFaceNormalized = cvCreateImage( cvSize(carax,caray), IPL_DEPTH_8U, 1 );
			iplFaceNormalizedIN = cvCreateImage( cvSize(carax,caray), IPL_DEPTH_8U, 1 );
		}

		boiplFaceImagesNormalized=true;
	}

	
public:

	//CURRENT FRAME INFORMATION FOR EACH DETECTION THREAD
	//Face Location and Geometry
	//! Face container upper left x coordinate
	int		x1,
	//! Face container lower right x coordinate
			x2,
	//! Face container upper left y coordinate
			y1,
	//! Face container lower right y coordinate
			y2;	
	
	//! Head and Shoulders container availability
	bool	hs;	
	//! Head and Shoulders container upper left x coordinate
	int		hsx1,
	//! Head and Shoulders container lower right x coordinate	
			hsy1,
	//! Head and Shoulders container upper left y coordinate
			hsx2,
	//! Head and Shoulders container lower right y coordinate	
			hsy2;	

	//! Lower Body container availability
	bool	lb;	
	//! Lower Body container upper left x coordinate
	int		lbx1,
	//! Lower Body container lower right x coordinate	
			lby1,
	//! Lower Body container upper left y coordinate
			lbx2,
	//! Lower Body container lower right y coordinate	
			lby2;	
	
	//! Full body container availability
	bool	fb;	
	//! Full body container upper left x coordinate
	int		fbx1,
	//! Full body container upper left y coordinate		
			fby1,
	//! Full body container lower right x coordinate	
			fbx2,
	//! Full body container lower right y coordinate		
			fby2;		
	
	//! Eye pair container availability
	bool	eyepair;	
	//! Eye pair container upper left x coordinate
	int		eyepairx1,
	//! Eye pair container upper left y coordinate			
			eyepairy1,
	//! Eye pair container lower right x coordinate		
			eyepairx2,
	//! Eye pair container lower right y coordinate	
			eyepairy2;	
	//! Left eye validity
	bool	leye,
	//! Right eye validity
			reye;					
	//! Left eye x coordinate
	int		e_lx,
	//! Left eye y coordinate
			e_ly,
	//! Right eye x coordinate
			e_rx,
	//! Left eye y coordinate
			e_ry;	
	
	//! Nose validity
	bool	nose;	
	//! Nose x coordinate
	int		np_x,
	//! Nose y coordinate
			np_y;			
	//! Nose container validity
	bool	nosecontainer;		

	//! Nose container upper left x coordinate
	int		nosex1,
	//! Nose container upper left y coordinate
			nosey1,
	//! Nose container lower right x coordinate
			nosex2,
	//! Nose container lower right y coordinate
			nosey2;	
	
	//! Mouth validity
	bool	mouth;	
	//! Mouth left estimated corner x coordinate
	int		ml_x,
	//! Mouth left estimated corner y coordinate
			ml_y;	
	//! Mouth right estimated corner x coordinate
	int		mr_x,
	//! Mouth right estimated corner x coordinate
			mr_y;			
	//! Mouth container validity
	bool	mouthcontainer;		
	//! Mouth container upper left x coordinate
	int		mouthx1,
	//! Mouth container upper left y coordinate
			mouthy1,
	//! Mouth container lower right x coordinate
			mouthx2,
	//! Mouth container lower right y coordinate		
			mouthy2;	
	
	//! Torso container validity
	bool	boTorsoContainer;	
	//! Estimated torso container upper left x coordinate
	int		torsox1,
	//! Estimated torso container upper left y coordinate	
			torsox2,
	//! Estimated torso container lower right x coordinate		
			torsoy1,
	//! Estimated torso container lower right y coordinate		
			torsoy2;			
	//! The torso histogram has been computed at least once for this detection thread
	bool	boTorsoHistAlreadyInitialized;				

	//! Torso color histogram (only available if the torso is available, i.e. two facial features were detected)					
	CvHistogram* TorsoHist;		
	//! Torso histogram persistence
	int			TorsoHistpersistence;	
	//! Torso LBP histogram (under development)
	float		*TorsoLBPHist;
	//! Skin color histogram (only available if two facial features were detected)					
	CvHistogram* SkinHist;		
	//! Skin histogram persistence
	int			SkinHistpersistence;					
	
	//Color features
	//! Normalized red min value
	float	rn_min,
	//! Normalized red max value
			rn_max,
	//! Normalized green min value
			gn_min,
	//! Normalized green max value		
			gn_max;	
	//! Set if detection was achieved by means of skin color
	bool	boColorBasedFound;		
	//! Set if there is a color model for this detection thread
	bool	boColorAlreadyInitialized;		
	
	//! Consecutive frames of detection thread
	long	persistence;							
	//! Number of frames remaining to cancel a not found previous detection				
	int		framestodie;		
	//! Eyes have been detected at least once for this face
	bool	boEyesDetectedAtLeastOnceforThisFace;	

	//Tracking variables
	//! Set if the head was tracked not detected
	bool	headtracked;				
	//! Consecutive frames tracked
	int		iconttracked;				
	//! Set if eyes were tracked not detected
	bool	eyestracked;				
	//! Set if left eye was tracked
	bool	leyetracked;				
	//! Set if right eye was tracked
	bool	reyetracked;				
	//! Set if the mouth was tracked
	bool	mouthtracked;				
	//! Set if the nose center was tracked
	bool	nosetracked;				

	//! Relates original face image with face pattern size
	int		escalaCara;	
	//! Similar but float value
	float	escalaCaraf;				

	//Trackers
	//! HeadTracker availability
	bool	boHeadTracker;		
	//! LEyeTracker availability
	bool	boLEyeTracker;		
	//! REyeTracker availability
	bool	boREyeTracker;		
	//!	LMouthTracker availability
	bool	boLMouthTracker;	
	//! RMouthTracker availability
	bool	boRMouthTracker;
	//! NoseTracker availability
	bool	boNoseTracker;	
	//! Tracker instance for head/face
	COpenTracking	*HeadTracker;	
	//! Tracker instance for left eye (in the image)
	COpenTracking	*LEyeTracker;	
	//! Tracker instance for right eye (in the image)
	COpenTracking	*REyeTracker;	
	//! Tracker instance for nose
	COpenTracking	*NoseTracker;		
	//! Tracker instances for Lmouth
	COpenTracking	*LMouthTracker;		
	//! Tracker instances for Rmouth
	COpenTracking	*RMouthTracker;		
	
	//! Label used if no recognition is applied
	char	Label[32];

	//Normalized images
	//! Is memory for normalized images already allocated?
	bool	boiplFaceImagesNormalized;		

	//! Have the eyes been detected for this detection thread
	bool	boStableTrackedFace;	
	//! Has been saved for this detection thread already an exemplar?
	bool	boStableTrackedFaceSaved;		

	//! Indicates if there is a normalized image available for current detection
	bool	boNormalizedAvailable;			

	//! Normalized (in size) image containing a little bit more than the inner face but unmasked
	IplImage *iplFaceContextNormalized,	
	//! Normalized (in size) image containing a little bit more than the inner face but masked
		     *iplFaceContextNormalizedMasked,	
	//! the FACEX*FACEY part
			*iplFaceNormalized,			
	//! FACEX*FACEY including illumination normalization (using LBP or PRL paper according to define INLBPSIMPLIFIED)
			*iplFaceNormalizedIN;			


	//Exemplars. Information related to the selected normalized images extraced for this detection thread
	//! Contains selected exemplars along the whole detection thread history
	CExemplars			Exemplars;			


	//Time stamps
	//! Timestamp for the first detection of the detection thread
	long	timestamp,
	//! Timestamp for the last detection of the detection thread
			timestamplast;

	//Face appearance
	//Labels of the thread (not in use)
	//! Identity labels
	char	Id[32];			
	//! Gender labels
	char	Gender[32];				


	//!Externally activated to indicate that the face is smiling to plot a different mouth
	bool	boSmiling;

	//LK Tracker (not used in present version)
	int				cornerCount;						
	CvPoint2D32f	corners[30],corners_new[30];		//! Good festures to track (not used)
	int				cornerx,cornery;					//! Corner offset (if ROI is used) (not used)
	bool			boLKTracker;						//! Good features to track are available (not used)

};

//!	Contains all detection thread information for current interaction session
/*!

	Contains information about detection thread. This structure is used to store active and lost 
	detection threads
*/
class CFacialDataperImage
{
public:
	//!	Class constructor
	/*!

	Class constructor. Sets to zero the number of faces in the detection thread
	*/
	CFacialDataperImage()
	{
		//No detections by default
		NumFaces=0;		

		//Detection threads counter
		nSubjects=0;			

		Faces=NULL;
	}
	//!	Class destructor
	/*!

	Deallocates memory
	*/
	~CFacialDataperImage()
	{
		Libera();
	}

	//!	Allocates memory for a given number of faces
	/*!
		\param nFaces	Number of faces to be stored
		\param ids		Index number of id to start labelling
	*/
	void Init(int nFaces,int ids=0)
	{
		//Frees memory first
		Libera(ids);

		//Updates
		NumFaces=nFaces;
		//Allocs if there was a  detection
		if (NumFaces>0)
		{
			Faces=(CFacialData	**)new unsigned char [sizeof(CFacialData *)*NumFaces];

			for (int i=0;i<NumFaces;i++)
				Faces[i]=(CFacialData	*)new CFacialData();

		}
	}

	//! Frees memory
	/*!
		Removes detection thread info structures
	*/
	void Libera(int ids=0)
	{
		if (NumFaces)
		{
			for (int i=0;i<NumFaces;i++)
				delete Faces[i];
			delete [] Faces;
			NumFaces=0;
			
		}
		nSubjects=ids;			//Identities found so far
	}

	
	//! Gets detection thread index with highest persistence
	/*!
		Gets detection thread index with highest persistence (useful in video processing)
	*/
	int GetMostPersistentDetectionThread()
	{
		int i,j;
		int maxpersistence=0,dtpersistence,dtmax=-1;

		if (NumFaces)
		{
			for (i=0;i<NumFaces;i++)
			{
				dtpersistence=0;

				//Gets detection thread persistence
				for (j=0;j<Faces[i]->Exemplars.iNImages;j++)
				{
					dtpersistence+=Faces[i]->Exemplars.persistence[j];
				}

				//Stores the index of the detection trhead with largest persistence
				if (dtpersistence>maxpersistence)
				{
					maxpersistence=dtpersistence;
					dtmax=i;
				}
			}				
		}

		return dtmax;
	}

	//! Gets largest detected face
	/*!
		Gets largest detected face
	*/
	int GetLargestDetectedFace()
	{
		int i;
		int maxsize=0,csize=0,max=-1;

		if (NumFaces)
		{
			for (i=0;i<NumFaces;i++)
			{
				//Computes face size
				csize=(Faces[i]->x2-Faces[i]->x1)*(Faces[i]->y2-Faces[i]->y1);

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

	//! Gets largest detected face
	/*!
		Gets largest detected face with located eyes
	*/
	int GetLargestDetectedFacewithEyes()
	{
		int i;
		int maxsize=0,csize=0,max=-1;

		if (NumFaces)
		{
			for (i=0;i<NumFaces;i++)
			{
				//Considers only faces with eyes detected
				if (Faces[i]->leye && Faces[i]->reye)
				{
					//Computes face size
					csize=(Faces[i]->x2-Faces[i]->x1)*(Faces[i]->y2-Faces[i]->y1);

					//Stores the index of the face with largest size
					if (csize>maxsize)
					{
						maxsize=csize;
						max=i;
					}
				}
			}				
		}

		return max;
	}

	//! Returns the normalized position of both eyes
	/*!
		Given a face index, returns if possible the location of the mouth after normalization

		\param i	Face index
		\param leyex Left eye x coordinate after normalization
		\param leyey Left eye y coordinate after normalization
		\param reyex Right eye x coordinate after normalization
		\param reyey Right eye y coordinate after normalization
	*/
	bool GetNormalizedPosofEyes(int i, int *leyex,int *leyey, int *reyex,int *reyey)
	{
		//These positions should correspond to the established normalized ones
		*leyex=LEFTEYEX;				
		*leyey=LEFTEYEY;
		*reyex=RIGHTEYEX;				
		*reyey=RIGHTEYEY;

		return true;
	}

	//! Returns the normalized position of the mouth.
	/*!
		Given a face index, returns if possible the location of the mouth after normalization

		\param i	Face index
		\param mouthx Mouth x coordinate after normalization
		\param mouthy Mouth y coordinate after normalization
	*/
	bool GetNormalizedPosofMouth(int i, int *mouthx,int *mouthy)
	{

		//Valid index?
		if (i<NumFaces && i>=0)
		{
			//Eyes and mouth available?
			if (Faces[i]->leye && Faces[i]->reye && Faces[i]->mouth)
			{

				double xs=0,ys=0,angulo;
				int odx,ody,oix,oiy;
				odx=Faces[i]->e_rx;
				ody=Faces[i]->e_ry;
				oix=Faces[i]->e_lx;
				oiy=Faces[i]->e_ly;

				float dist=MathUtils.DistanciaEuclidea(odx,ody,oix,oiy);

				//Relación de la distancia entre los ojos real y la esperada, afectada por la escala
				float ratio=dist/(float)(RIGHTEYEX-LEFTEYEX);

				//Obtiene punto intermedio
				int interx=(odx + oix)/2,intery=(oiy + ody)/2;

				//Hace una copia de la imagen en tratamiento
				angulo=atan2((float)(intery - ody),(float)(odx - interx));//retorna radianes
				
				angulo=(float)(-angulo*180.0/3.141592); //paso a grados

				//Convierte coordenadas
				CMathUtils	MathUtils;

				float px=(Faces[i]->ml_x+Faces[i]->mr_x)*0.5f,py=(Faces[i]->ml_y+Faces[i]->mr_y)*0.5f;
				MathUtils.Rota2D((float)interx,(float)intery,(float)-angulo,&px,&py);
				
				//Posición de la boca tras rotar
				*mouthx=LEFTEYEX+ (int)((px-oix)/ratio);				
				*mouthy=LEFTEYEY+ (int)((py-oiy)/ratio);

				return true;

			}
		}
		return false;

	}


////PROVISIONAL
	//! Provides external detection data for a face
	/*!
	For situations in which facial feature detection has been done by an external tool

		\param iface Face to correct
		\param boeyes If set forces both eye positions
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
		\param hIN  Input image used to detect in color
		\param hgrayIN  Input image used to detect in gray
	*/
	void ForceDetection(int iface,bool boeyes, int oix, int oiy, int odx, int ody, bool bonose,int nx, int ny, bool bomouth,int mlx, int mly, int mrx, int mry,IplImage *hIN, IplImage *hgrayIN=NULL)
	{
		bool bograyallocated=false;

		if (!hgrayIN)
		{
			hgrayIN = cvCreateImage( cvSize(hIN->width,hIN->height), 8, 1);
			cvCvtColor(hIN,hgrayIN, CV_BGR2GRAY );
			bograyallocated=true;
		}

		//Given a valid index?
		if (iface>=0 && iface<NumFaces)
		{
			//Estima el contenedor dados los ojos
			if (boeyes)
				EstimateFacefromEyes(iface,oix,oiy,odx,ody,hIN->width,hIN->height);

			//Toma como ancho el múltiplo de 8 más cercano a la mitad del ancho de la detección
			int TrackWidth=((Faces[iface]->x2-Faces[iface]->x1)/16+1)*8;
			while (TrackWidth<=PATSIZEX || TrackWidth<=PATSIZEY) TrackWidth+=8;

			//Actualiza los ojos
			if (boeyes)
				SetEyes(iface,true,true,oix,oiy,true,odx,ody,TrackWidth,hIN,hgrayIN);

			//Actualiza patrones y posición de la nariz
			if (bonose)
				SetNose(iface,nx,ny,false,TrackWidth,hIN);

			//Actualiza patrones y posición de la boca
			if (bomouth)
				SetMouth(iface,mlx,mly,mrx,mry,false,TrackWidth,hIN);


			if (bograyallocated) cvReleaseImage(&hgrayIN);
		
		}


	}



	//! Set eyes of a detection thread
	/*!
		\param iface	Face or detection thread to update
		\param boOK Both eyes valid
		\param boLE Valid left eye?
		\param oix	x coordinate of left eye
		\param oiy	y coordinate of left eye
		\param boRE Valid right eye?
		\param odx	x coordinate of right eye
		\param ody	y coordinate of right eye
		\param TrackWidth	Heuristic value for trackers
		\param hIN	Input color image
		\param hgrayIN Input gray image

	*/
				
	void SetEyes(int iface, bool boOK, bool boLE, int oix, int oiy, bool boRE, int odx, int ody,int TrackWidth,IplImage *hIN,IplImage *hgrayIN)
	{
		//First eyes detection for this subject?
		if (!Faces[iface]->boEyesDetectedAtLeastOnceforThisFace)
		{
			Faces[iface]->boEyesDetectedAtLeastOnceforThisFace=true;

			//Eyes have been detected, establishes a label for this detection
			sprintf(Faces[iface]->Label,"Person_%d",nSubjects++);

			//Sets the initial timestamp fot the detectio thread
			Faces[iface]->timestamp=clock();
		}

		//Timestamp
		//Last time this detection thread was seen
		Faces[iface]->timestamplast=clock();
		
		//Eyes were detected, so likely tracked eyes are not needed   ?????????
		/*if (!boLE) Faces[iface]->leyetracked=false;
		else Faces[iface]->leyetracked=true;
		if (!boRE) Faces[iface]->reyetracked=false;
		else Faces[iface]->reyetracked=true;*/
		Faces[iface]->reyetracked=false;
		Faces[iface]->leyetracked=false;
		
		//Al disponer de los ojos carga el patrón a seguir si está activado el seguimiento
		//Obtiene el patrón del ojo izquierdo a seguir
		if (boLE || boOK)
		{
			Faces[iface]->leye=true;
			Faces[iface]->e_lx=oix;
			Faces[iface]->e_ly=oiy;


			if (!Faces[iface]->boLEyeTracker)//No ha sido alojado				
			{				
				//Aloja el tracker de la boca
				Faces[iface]->LEyeTracker=new COpenTracking(hIN,oix,oiy,TrackWidth,TrackWidth,PATSIZEX,PATSIZEY);
				Faces[iface]->boLEyeTracker=true;
			}
			else
			{
				Faces[iface]->LEyeTracker->SetPattern(hIN,oix,oiy);
			}
		}

		
		//Obtiene el patrón del ojo derecho a seguir
		if (boRE || boOK)
		{
			Faces[iface]->reye=true;
			Faces[iface]->e_rx=odx;
			Faces[iface]->e_ry=ody;

			if (!Faces[iface]->boREyeTracker)
			{	
				//Aloja el tracker de la boca
				Faces[iface]->REyeTracker=new COpenTracking(hIN,odx,ody,TrackWidth,TrackWidth,PATSIZEX,PATSIZEY);
				Faces[iface]->boREyeTracker=true;
			}
			else
			{
				Faces[iface]->REyeTracker->SetPattern(hIN,odx,ody);
			}
		}
	
		//SALVA IMAGEN NORMALIZADA if both eyes are available
		if ((boLE && boRE) || boOK)
		{
			Faces[iface]->boNormalizedAvailable=true;
						
			//Salva la imagen normalizada en la estructira de la imagen
			//Rotamos según las posiciones de los ojos, almacenándolo en cwiImage
			IplImage *clone=cvCreateImage(cvSize(hgrayIN->width,hgrayIN->height),hgrayIN->depth,hgrayIN->nChannels);
			cvCopyImage(hgrayIN,clone);

			double xs=0,ys=0,angulo;
			float interx,intery;

			//Gets angle
			angulo=MathUtils.GetAngle4Rotation(oix,oiy,odx,ody,&interx,&intery);

						
			//Usamos el ROI para reducir operaciones
			int ancho=Faces[iface]->x2-Faces[iface]->x1,ancho2=ancho/2,
				alto=Faces[iface]->y2-Faces[iface]->y1,alto2=alto/2;

			int esqx=Faces[iface]->x1-ancho2,
				esqy=Faces[iface]->y1-alto2,
				sx=2*ancho,sy=2*alto;

			IplUtils.WindowTest(hgrayIN,&esqx,&esqy,&sx,&sy);

			
			CIplImageUtils iplutils;
			iplutils.RotateImage(hgrayIN,clone,(float)angulo,(float)interx,(float)intery,true, cvRect(esqx,esqy,sx,sy));
				
			
			//Normalization
			//Conversion to BYTE *
			BYTE *btmp=new BYTE[hgrayIN->width*hgrayIN->height];

			IplUtils.CopyIpl2BYTE(clone,btmp,hgrayIN->width,hgrayIN->height);
			
			//Calcula imagen normalizada correspondiente a este par de puntos
			CWarpingImage *cwiImage;
			int dummy;

			//Recorta la imagen pero sin aplicar la máscara elíptica
			//y normalizando a una imagen algo mayor
			cwiImage= new CWarpingImage(FACEXCONTEXT,FACEYCONTEXT,LEFTEYEX+OFFSETXCONTEXT,LEFTEYEY+OFFSETYCONTEXT,
				RIGHTEYEX+OFFSETXCONTEXT,RIGHTEYEY+OFFSETYCONTEXT);

			IplImage *ipl=cvCreateImage(cvSize(hgrayIN->width,hgrayIN->height),8,1);

			IplUtils.CopyBYTE2Ipl(btmp,&ipl,hgrayIN->width,hgrayIN->height);

			cvReleaseImage(&ipl);

			
			//
			//Al normalizar debo utilizar las coordenadas de los ojos rotadas
			//Roto los ojos
			int leyexr,leyeyr,reyexr,reyeyr;
			float pxr=(float)oix,pyr=(float)oiy;
			
			MathUtils.Rota2D((float)interx,(float)intery,(float)-angulo,&pxr,&pyr);
			leyexr=(int)pxr;
			leyeyr=(int)pyr;

			pxr=(float)odx;
			pyr=(float)ody;
			
			MathUtils.Rota2D((float)interx,(float)intery,(float)-angulo,&pxr,&pyr);
			reyexr=(int)pxr;
			reyeyr=(int)pyr;
			//Fin rotación de ojos

			//SIZE NORMALIZATION

			//Normaliza aplicando la máscara elíptica but using illumination normalization
			cwiImage->NormalizaOjos(btmp,clone->width,
					clone->height,leyexr,leyeyr,
					reyexr,reyeyr,&dummy, true,true);//penúltimo true coloca máscara, último aplica normalización por iluminación

			//Normalización de la iluminación (en pruebas)
			BYTE *btmpIN=(BYTE *)new unsigned char[FACEX*FACEY];
			BYTE *dst=btmpIN;
			BYTE *src=cwiImage->pbImageMasked+OFFSETYCONTEXT*FACEXCONTEXT+OFFSETXCONTEXT;
			for (int i4=0;i4<FACEY;i4++,src+=FACEXCONTEXT,dst+=FACEX)
				memcpy(dst,src,FACEX); 

			//Normalization without illumination normalization

			//Normaliza sin aplicar la máscara elíptica
			cwiImage->NormalizaOjos(btmp,clone->width,
					clone->height,leyexr,leyeyr,
					reyexr,reyeyr,&dummy, true,false);//penúltimo true calcula también la imagen con máscara, último aplica normalización por iluminación

			/*BYTE *btmpctx=(BYTE *)new unsigned char[FACEXCONTEXT*FACEYCONTEXT];
			dst=btmpctx;
			src=cwiImage->pbImage;
			for (int i4=0;i4<FACEYCONTEXT;i4++,src+=FACEXCONTEXT,dst+=FACEXCONTEXT)
				memcpy(dst,src,FACEXCONTEXT); 

			//Normaliza aplicando la máscara elíptica
			cwiImage->NormalizaOjos(DebugLog,btmp,clone->width,
					clone->height,leyexr,leyeyr,
					reyexr,reyeyr,&dummy, true,false);//penúltimo true coloca máscara, último aplica normalización por iluminación


			BYTE *btmpctxmasked=(BYTE *)new unsigned char[FACEXCONTEXT*FACEYCONTEXT];
			dst=btmpctxmasked;
			src=cwiImage->pbImage;
			for (int i4=0;i4<FACEYCONTEXT;i4++,src+=FACEXCONTEXT,dst+=FACEXCONTEXT)
			{
				memcpy(dst,src,FACEXCONTEXT); 
			}*/
			
			//Copia in face structure		//cwiImage->pbImage imagen normalizada
			Faces[iface]->CopyNormalizedImages(cwiImage->pbImage,cwiImage->pbImageMasked,btmpIN,FACEXCONTEXT,FACEYCONTEXT,
				FACEX,FACEY,OFFSETXCONTEXT,OFFSETYCONTEXT);

			//Parte de prueba Illumination Normalization
			delete [] btmpIN;
			/*delete [] btmpctx;
			delete [] btmpctxmasked;*/

			
			delete cwiImage;
			delete [] btmp;
			cvReleaseImage(&clone);
			//FIN COPIA NORMALIZADA
		}
	}

	//! Set nose of a detection thread
	/*!
		\param i	Face or detection thread to update
		\param naux_x	x coordinate of left eye
		\param naux_y	y coordinate of left eye
		\param boTracked If true does not update the pattenrs to track
		\param TrackWidth	Heuristic value for trackers
		\param hIN	Input color image
		
	*/
	void SetNose(int i,int naux_x,int naux_y,bool boTracked,int TrackWidth,IplImage *hIN)
	{	
	
		//Sets detection values
		Faces[i]->nose=true;
		Faces[i]->np_x=naux_x;
		Faces[i]->np_y=naux_y;
	
		//Al disponer de la nariz carga el patrón a seguir si está activado el seguimiento y no se ha seguido
		if (!boTracked)
		{	
			//Obtiene el patrón de la nariz a seguir
			if (!Faces[i]->boNoseTracker)
			{
			
				//Aloja el tracker de la nariz
				Faces[i]->NoseTracker=new COpenTracking(hIN,Faces[i]->np_x,Faces[i]->np_y,TrackWidth,TrackWidth,PATSIZEX,PATSIZEY);
				Faces[i]->boNoseTracker=true;
			}
			else
				Faces[i]->NoseTracker->SetPattern(hIN,Faces[i]->np_x,Faces[i]->np_y);
		}

	}

	//! Set mouth of a detection thread
	/*!
		\param i	Face or detection thread to update
		\param mlaux_x	x coordinate of left mouth corner
		\param mlaux_y	y coordinate of left mouth corner
		\param mraux_x	x coordinate of right mouth corner
		\param mraux_y	y coordinate of right mouth corner
		\param boTracked If true does not update the pattenrs to track
		\param TrackWidth	Heuristic value for trackers
		\param hIN	Input color image
		
	*/
	void SetMouth(int i,int mlaux_x,int mlaux_y,int mraux_x,int mraux_y,bool boTracked,int TrackWidth,IplImage *hIN)
	{

		Faces[i]->mouth=true;
		Faces[i]->ml_x=mlaux_x;
		Faces[i]->ml_y=mlaux_y;
		Faces[i]->mr_x=mraux_x;
		Faces[i]->mr_y=mraux_y;


		//Al disponer de la boca carga el patrón a seguir si está activado el seguimiento
		if (!boTracked)
		{	
			//Obtiene el patrón de la boca a seguir
			if (!Faces[i]->boLMouthTracker)
			{
				//Aloja el tracker de la boca
				Faces[i]->LMouthTracker=new COpenTracking(hIN,Faces[i]->ml_x,Faces[i]->ml_y,TrackWidth,TrackWidth,PATSIZEX,PATSIZEY);
				Faces[i]->boLMouthTracker=true;
			}
			else
			{
				Faces[i]->LMouthTracker->SetPattern(hIN,Faces[i]->ml_x,Faces[i]->ml_y);
			}

			if (!Faces[i]->boRMouthTracker)
			{
				//Aloja el tracker de la boca
				Faces[i]->RMouthTracker=new COpenTracking(hIN,Faces[i]->mr_x,Faces[i]->mr_y,TrackWidth,TrackWidth,PATSIZEX,PATSIZEY);
				Faces[i]->boRMouthTracker=true;
			}
			else
			{
				Faces[i]->RMouthTracker->SetPattern(hIN,Faces[i]->mr_x,Faces[i]->mr_y);
			}
		}
	}

	//! Paints the whole set of detections
	/*!
    \param h	Image to plot the results
	\param color To be used 
	\param stcvFont If given, indicates the font used to print labels	
	\param Masked Paints using different "creative" ways
	*/
	void PaintDetections(IplImage *h,CvScalar color,CvFont	*stcvFont=NULL, bool Masked=false)
	{
	
		//Painting in the normal way
		if (!Masked)
		{
			//Paints facial data available
			for (int i=0;i<NumFaces;i++)
			{
				PaintDetection(h,i,color,stcvFont);
			}
		}
		else //Paints the image but masked (showing only an area around the eyes)
		{
			for (int i=0;i<h->height;i++)
			{
				for (int j=0;j<h->width;j++)
				{
					bool boclosetoeye=false;
					for (int ic=0;ic<NumFaces;ic++)
					{
						
						
						if (Faces[ic]->leye && Faces[ic]->reye)//Both eyes available?
						{
							float distL,distR;

							float dist=MathUtils.DistanciaEuclidea(Faces[ic]->e_lx,Faces[ic]->e_ly,
								Faces[ic]->e_rx,Faces[ic]->e_ry);

							distL=MathUtils.DistanciaEuclidea(j,i,Faces[ic]->e_lx,Faces[ic]->e_ly);

							if (distL<0.4*dist)
								boclosetoeye=true;
							else
							{
								distR=MathUtils.DistanciaEuclidea(Faces[ic]->e_rx,Faces[ic]->e_ry,j,i);
								if (distR<0.4*dist)
									boclosetoeye=true;
							}
						}						
					}					

					//Pinta en negro los que están a más distancia que un valor relacionado con la distancia
					if (!boclosetoeye)
						cvLine(h,cvPoint(j,i),cvPoint(j,i),color);
				}
			}

			
			//Paints facial data available
			

			
			//Obtiene la distancia entre los ojos
			
		}
	}

	//! Paints a detection thread
	/*!
    \param h	Image to plot the results
	\param i	Detection thread to paint
	\param color To be used 
	\param stcvFont If given, indicates the font used to print labels
	*/
	void PaintDetection(IplImage *h,int i, CvScalar color,CvFont	*stcvFont=NULL)
	{
		char cadena[200];
		CvScalar	colorlocal;

		
		//Temporaly lost face
		if (Faces[i]->framestodie!=-1)
		{
			//Face container
			cvRectangle( h,cvPoint(Faces[i]->x1,Faces[i]->y1),
						cvPoint(Faces[i]->x2,Faces[i]->y2),
						CV_RGB(0,0,0), 3 );
			cvRectangle( h,cvPoint(Faces[i]->x1,Faces[i]->y1),
						cvPoint(Faces[i]->x2,Faces[i]->y2),
						CV_RGB(255,255,255), 1 );

			//Plots string
			//if (stcvFont)
			//{
			//sprintf(cadena,"%s  missed",Faces[i]->Label,Faces[i]->persistence);
			//cvPutText( h,cadena,cvPoint(Faces[i]->x1,Faces[i]->y1-4),
			//		stcvFont,CV_RGB(255,0,0));
			//}
		}
		else//Normal detection
		{
			//Face container
			//Uses BLUE if the face was tracked
			if (Faces[i]->headtracked)
			{
				cvRectangle( h,cvPoint(Faces[i]->x1,Faces[i]->y1),
							cvPoint(Faces[i]->x2,Faces[i]->y2),
							CV_RGB(0,0,255), 3 );
			}
			else
			{
				//If eyes have not been detected for this detection thread uses GRAY
				if (!Faces[i]->boEyesDetectedAtLeastOnceforThisFace && !Faces[i]->lb)
				{
					cvRectangle( h,cvPoint(Faces[i]->x1,Faces[i]->y1),
								cvPoint(Faces[i]->x2,Faces[i]->y2),
								CV_RGB(128,128,128), 3 );
				}
				else //The eyes have already been detected for this detection thread
				{		
					if (Faces[i]->boColorBasedFound) //If color based found
						cvRectangle( h,cvPoint(Faces[i]->x1,Faces[i]->y1),
							cvPoint(Faces[i]->x2,Faces[i]->y2),
							CV_RGB(255,255,255), 3 );
					else	//Uses requested color
						cvRectangle( h,cvPoint(Faces[i]->x1,Faces[i]->y1),
							cvPoint(Faces[i]->x2,Faces[i]->y2),
							color, 3 );
				}
			}

			//Breast area used for tshirt color estimation is painted if available
			//if (Faces[i]->boTorsoContainer)
			//	cvRectangle( h,cvPoint(Faces[i]->torsox1,Faces[i]->torsoy1),
			//				cvPoint(Faces[i]->torsox2,Faces[i]->torsoy2),
			//				color, 3 );

			//Plots Good features to track if available
			if (Faces[i]->boLKTracker && 0)
			{
				for (int ii=0;ii<Faces[i]->cornerCount;ii++)
				{
					cvCircle( h, cvPoint((int)(Faces[i]->cornerx+Faces[i]->corners[ii].x),
									(int)(Faces[i]->cornery+Faces[i]->corners[ii].y)), 
							2, CV_RGB(255,0,0),-1,8,0);

				}
			}

			//Label assigned and persistence
			//Faces whose eyes have not been detected
			if (stcvFont)
			{
				if (!Faces[i]->boEyesDetectedAtLeastOnceforThisFace)
				{
					sprintf(cadena,"%d",(int)Faces[i]->persistence);
					cvPutText( h,cadena,cvPoint(Faces[i]->x1,Faces[i]->y1-4),
						stcvFont,CV_RGB(255,255,0));
				}
				else//Faces whose eyes have already been detected
				{
					sprintf(cadena,"%s (%d)",Faces[i]->Label,(int)Faces[i]->persistence);
					cvPutText( h,cadena,cvPoint(Faces[i]->x1-5,Faces[i]->y1-4),
						stcvFont,CV_RGB(0,255,0));
				}
			}

			//If hs detection, paints hs container
			if (Faces[i]->hs)
			{
				if (Faces[i]->boEyesDetectedAtLeastOnceforThisFace)
					cvRectangle( h,cvPoint(Faces[i]->hsx1,Faces[i]->hsy1),
							cvPoint(Faces[i]->hsx2,Faces[i]->hsy2),
							CV_RGB(255,0,0), 2 );
				else
					cvRectangle( h,cvPoint(Faces[i]->hsx1,Faces[i]->hsy1),
							cvPoint(Faces[i]->hsx2,Faces[i]->hsy2),
							CV_RGB(150,150,150), 2 );
			}

			//If fullbody detection, paits fullbody container
			if (Faces[i]->fb)
				cvRectangle( h,cvPoint(Faces[i]->fbx1,Faces[i]->fby1),
							cvPoint(Faces[i]->fbx2,Faces[i]->fby2),
							CV_RGB(255,0,255), 2 );

			//If lowerbody detection, paits fullbody container
			if (Faces[i]->lb)
				cvRectangle( h,cvPoint(Faces[i]->lbx1,Faces[i]->lby1),
							cvPoint(Faces[i]->lbx2,Faces[i]->lby2),
							CV_RGB(0,255,255), 2 );

			//Eye container if available
			if (Faces[i]->eyepair)
			{
				//Contenedor del par ocular
				cvRectangle( h,cvPoint(Faces[i]->eyepairx1,Faces[i]->eyepairy1),
								cvPoint(Faces[i]->eyepairx2,Faces[i]->eyepairy2),
								color, 1 );
			}

			
			//Facial elements if available

			//Eye color dependes on if they were tracked, just one
			if (Faces[i]->eyestracked)
			{
				//Both tracked
				colorlocal=CV_RGB(0,0,255);
			}
			else 
			{
				//A single tracked
				if (Faces[i]->leyetracked || Faces[i]->reyetracked)
					colorlocal=CV_RGB(255,255,255);
				else//Or just detected
                    colorlocal=CV_RGB(0,255,0);
			}

			//Plots eyes if they are available (only for non fb)
			if (Faces[i]->leye && !Faces[i]->fb && !Faces[i]->lb
				&& !(Faces[i]->leyetracked || Faces[i]->reyetracked) )
			{
				cvCircle( h, cvPoint(Faces[i]->e_lx,Faces[i]->e_ly), 
						3, colorlocal,-1,8,0);
				cvCircle( h, cvPoint(Faces[i]->e_lx,Faces[i]->e_ly), 
						1, CV_RGB(255,255,255),-1,8,0);
			}
									
			if (Faces[i]->reye && !Faces[i]->fb && !Faces[i]->lb
				&& !(Faces[i]->leyetracked || Faces[i]->reyetracked) )
			{
				cvCircle( h, cvPoint(Faces[i]->e_rx,Faces[i]->e_ry), 
						3, colorlocal,-1,8,0);
				cvCircle( h, cvPoint(Faces[i]->e_rx,Faces[i]->e_ry), 
						1, CV_RGB(255,255,255),-1,8,0);
			}

		
			//Mouth 
			if (Faces[i]->mouth)
			{
				cvLine( h, cvPoint(Faces[i]->ml_x,Faces[i]->ml_y), 
					cvPoint(Faces[i]->mr_x,Faces[i]->mr_y), 
					CV_RGB(0,255,0),3);

				if (Faces[i]->mouthtracked)
				{
					cvLine( h, cvPoint(Faces[i]->ml_x,Faces[i]->ml_y), 
						cvPoint(Faces[i]->mr_x,Faces[i]->mr_y), 
						CV_RGB(0,0,255),2);
				}
				else//detected
				{
					cvRectangle( h,cvPoint(Faces[i]->mouthx1,Faces[i]->mouthy1),
								cvPoint(Faces[i]->mouthx2,Faces[i]->mouthy2),
								color, 1 );
				}

				//Smile
				if (Faces[i]->boSmiling)
				{
					float laxis=MathUtils.DistanciaEuclidea(Faces[i]->mr_x,Faces[i]->mr_y,
						Faces[i]->ml_x,Faces[i]->ml_y);

					float angle=(float)(atan2((float)Faces[i]->mr_y-Faces[i]->ml_y,(float)Faces[i]->mr_x-Faces[i]->ml_x)*180.f/M_PI);

					cvEllipse(h,cvPoint((Faces[i]->mr_x+Faces[i]->ml_x)/2,(Faces[i]->mr_y+Faces[i]->ml_y)/2),
						cvSize((int)(laxis*0.5),(int)(laxis*0.33)),-angle,180,360,CV_RGB(0,255,0),3);

				}

			}

			//Nose
			if (Faces[i]->nose && Faces[i]->nosetracked)
				cvCircle( h, cvPoint(Faces[i]->np_x,Faces[i]->np_y), 
						3, CV_RGB(0,0,255),2);
			else//detected
			{
				if (Faces[i]->nosecontainer)
				{
					cvRectangle( h,cvPoint(Faces[i]->nosex1,Faces[i]->nosey1),
							cvPoint(Faces[i]->nosex2,Faces[i]->nosey2),
							color, 1 );
					cvCircle( h, cvPoint(Faces[i]->np_x,Faces[i]->np_y), 
						1, color,2);
				}
			}

		}

	}



	/***********************************************************************************************************

				Containers estimation routines

	***********************************************************************************************************/

	//! Estimate face container from eyes
	/*!
	Estimate face container from eyes

		\param i	Face to manage
		\param oix	x coordinate of left eye
		\param oiy	y coordinate of left eye
		\param odx	x coordinate of right eye
		\param ody	y coordinate of right eye
		\param sx	Image width, used to check coordinates boundaries
		\param sy	Image height, used to check coordinates boundaries

	*/
	void EstimateFacefromEyes(int i,int oix,int oiy, int odx, int ody, int sx, int sy)
	{
		float fdist=MathUtils.DistanciaEuclidea(odx,ody,oix,oiy);

		//recuadro en base a ojos seguidos
		//Determina el ojo màs alto por si la cara està inclinada
		int eyeoffy,eyeoffxl=0,eyeoffxr=0,yval=oiy;

		//Para caras rotadas hay variación
		if (ody<yval)
		{
			yval=ody;
			eyeoffxl=(int)((ody+oiy)*0.5-yval);
		}
		else
			eyeoffxr=(int)((ody+oiy)*0.5-yval);

		eyeoffy=oiy-yval;

		float midx=(odx+oix)*0.5f;

		//Asigna valores del contenedor
		//Asigna valores del contenedor
		//Faces[i]->x1=(int)(oix-fdist*0.6-eyeoffxr);
		Faces[i]->x1=(int)(midx-fdist*1.1-eyeoffxr);
		Faces[i]->y1=(int)(oiy-eyeoffy-fdist*0.6);
		Faces[i]->x2=Faces[i]->x1+(int)(fdist*2.2+eyeoffxr+eyeoffxl);//fdist + 0.6 * fdist dos veces
		Faces[i]->y2=Faces[i]->y1+(int)(fdist*2.5+eyeoffy);

		//Comprueba que no se salga la estimación de la imagen
		IplUtils.WindowTestP(sx,sy,&Faces[i]->x1,&Faces[i]->y1,
				&Faces[i]->x2,&Faces[i]->y2);
	}

	/*!
	Estimate face container from eyes or mouth

		\param i	Face to manage
		\param oix	x coordinate of left eye
		\param oiy	y coordinate of left eye
		\param odx	x coordinate of right eye
		\param ody	y coordinate of right eye
		\param mx	x coordinate of mouth center
		\param my	y coordinate of mouth center
		\param sx	Image width, used to check coordinates boundaries
		\param sy	Image height, used to check coordinates boundaries

	*/
	void EstimateFacefromEyesandMouth(int i,int oix,int oiy, int odx, int ody, int mx, int my, int sx, int sy)
	{
		float fdist=MathUtils.DistanciaEuclidea(odx,ody,oix,oiy);
		float fdist2=(MathUtils.DistanciaEuclidea(mx,my,oix,oiy)+MathUtils.DistanciaEuclidea(odx,ody,mx,my))/2.f;

		//Takes into consideration the distance to the mouth if it is bigger
		if (fdist2>fdist*1.2f) fdist=fdist2/1.2f;

		//recuadro en base a ojos seguidos
		//Determina el ojo màs alto por si la cara està inclinada
		int eyeoffy,eyeoffxl=0,eyeoffxr=0,yval=oiy;

		//Para caras rotadas hay variación
		if (ody<yval)
		{
			yval=ody;
			eyeoffxl=(int)((ody+oiy)*0.5-yval);
		}
		else
			eyeoffxr=(int)((ody+oiy)*0.5-yval);

		eyeoffy=oiy-yval;

		float midx=(odx+oix)*0.5f;

		//Asigna valores del contenedor
		//Faces[i]->x1=(int)(oix-fdist*0.6-eyeoffxr);
		Faces[i]->x1=(int)(midx-fdist*1.1-eyeoffxr);
		Faces[i]->y1=(int)(oiy-eyeoffy-fdist*0.6);
		Faces[i]->x2=Faces[i]->x1+(int)(fdist*2.2+eyeoffxr+eyeoffxl);//fdist + 0.6 * fdist dos veces
		Faces[i]->y2=Faces[i]->y1+(int)(fdist*2.5+eyeoffy);

		//Comprueba que no se salga la estimación de la imagen
		IplUtils.WindowTestP(sx,sy,&Faces[i]->x1,&Faces[i]->y1,
				&Faces[i]->x2,&Faces[i]->y2);
	}

	/*!
	Estimate face container from eyes or mouth

		\param i	Face to manage
		\param boLE	True means the given eye is the left one, false means right eye
		\param ox	x coordinate of eye
		\param oy	y coordinate of eye
		\param mx	x coordinate of mouth center
		\param my	y coordinate of mouth center
		\param sx	Image width, used to check coordinates boundaries
		\param sy	Image height, used to check coordinates boundaries

	*/
	void EstimateFacefromEyeandMouth(int i,bool boLE, int ox,int oy, int mx, int my, int sx, int sy)
	{
		//Distance calculation
		float fdist=MathUtils.DistanciaEuclidea(ox,oy,mx,my);

		//Asigna valores del contenedor
		Faces[i]->x1=(int)(mx-fdist*1.1);
		Faces[i]->y1=(int)(oy-fdist*0.6);
		Faces[i]->x2=Faces[i]->x1+(int)(fdist*2.2);
		Faces[i]->y2=Faces[i]->y1+(int)(fdist*2.5);

		//Comprueba que no se salga la estimación de la imagen
		IplUtils.WindowTestP(sx,sy,&Faces[i]->x1,&Faces[i]->y1,
				&Faces[i]->x2,&Faces[i]->y2);
	}



	//! Estimate face container from head and shoulders
	/*!
	Estimate face container from head and shoulders

		\param i	Face to manage
		\param wsx	Image width, used to check coordinates boundaries
		\param hsy	Image height, used to check coordinates boundaries

	*/
	void EstimateFacefromHS(int i, int wsx, int hsy)
	{
		//Estimo el contenedor de la cara
		int sx=Faces[i]->hsx2-Faces[i]->hsx1,
			sy=Faces[i]->hsy2-Faces[i]->hsy1;
		int wd=(int)((float)sx*0.3);
		int hd=(int)((float)sy*0.2);

		Faces[i]->x1=Faces[i]->hsx1+wd;
		Faces[i]->x2=Faces[i]->hsx2-wd;
		Faces[i]->y1=Faces[i]->hsy1+hd;
		Faces[i]->y2=Faces[i]->hsy2-hd*2;

		IplUtils.WindowTestP(wsx,hsy,&Faces[i]->x1,&Faces[i]->y1,
			&Faces[i]->x2,&Faces[i]->y2);
	}


	//! Estimate face container from full body container
	/*!
	Estimate face container from full body container

		\param i	Face to manage
		\param wsx	Image width, used to check coordinates boundaries
		\param hsy	Image height, used to check coordinates boundaries

	*/
	void EstimateFacefromFB(int i,int wsx, int hsy)
	{
		//Estimo el contenedor de la cara
		//Estimo el contenedor de la cara
		int sx=Faces[i]->fbx2-Faces[i]->fbx1,
			sy=Faces[i]->fby2-Faces[i]->fby1;
		int wd=(int)((float)sx*0.3);
		int hd=(int)((float)sy*0.1);

		Faces[i]->x1=Faces[i]->fbx1+wd;
		Faces[i]->x2=Faces[i]->fbx2-wd;
		Faces[i]->y1=Faces[i]->fby1+hd;
		Faces[i]->y2=(int)(Faces[i]->fby2-(hd*7.2));

		IplUtils.WindowTestP(wsx,hsy,&Faces[i]->x1,&Faces[i]->y1,
			&Faces[i]->x2,&Faces[i]->y2);
	}

	//! Estimate head and shoulders container from face
	/*!
	Estimate head and shoulders container from face

		\param i	Face to manage
		\param wsx	Image width, used to check coordinates boundaries
		\param hsy	Image height, used to check coordinates boundaries

	*/

	void EstimateHSfromFace(int i,int wsx, int hsy)
	{
		//Estimates HS container
		int sx=Faces[i]->x2-Faces[i]->x1,
				sy=Faces[i]->y2-Faces[i]->y1;
		int wd=(int)((float)sx*0.75);
		int hd=(int)((float)sy*0.5);

		Faces[i]->hsx1=Faces[i]->x1-wd;
		Faces[i]->hsx2=Faces[i]->x2+wd;
		Faces[i]->hsy1=Faces[i]->y1-hd;
		Faces[i]->hsy2=Faces[i]->y2+2*hd;

		IplUtils.WindowTestP(wsx, hsy,&Faces[i]->hsx1,&Faces[i]->hsy1,
			&Faces[i]->hsx2,&Faces[i]->hsy2);
	}

	//! Estimate breast container from face
	/*!
	Estimate breast container from face

		\param i	Face to manage
		\param sx	Image width, iy must be considered to avoid an estimation out of limits
		\param sy	Image height

	*/
	bool EstimateBreastfromFace(int i,int sx,int sy)
	{
		//Versión precedente
		/*//Estimates Breast container
		int blobwidth=Faces[i]->x2-Faces[i]->x1;
		int blobheight=Faces[i]->y2-Faces[i]->y1;

		//Histogram dimensions for torso modeling
		float factorlateralwidth=0.1;
		float factorROI=1.75;
		
		int lx = Faces[i]->x1;
		int rx = Faces[i]->x2;

		Faces[i]->torsox1=(int)(lx-blobwidth*factorlateralwidth);
		Faces[i]->torsoy1=(int)(Faces[i]->y2 + (float)blobheight*0.5);
		Faces[i]->torsox2=(int)(rx+blobwidth*factorlateralwidth);
		Faces[i]->torsoy2=(int)(Faces[i]->torsoy1+factorROI*blobheight);

	*/

		//Versión actual

		int fsx=Faces[i]->x2-Faces[i]->x1;
		int fsy=Faces[i]->y2-Faces[i]->y1;
						
		int p1x=Faces[i]->x1-(int)(fsx*0.75);
		int torsosx=(int)(fsx*2.5);

		//Controlo bordes
		//en x
		if (p1x<0)
		{
			torsosx=torsosx+p1x;
			p1x=0;
		}
		if (p1x+torsosx>sx)
			torsosx=sx-p1x-1;

		int p1y=Faces[i]->y1+(int)(fsy*0.75);
		int torsosy=(int)(fsy*1.85);

		//Controlo bordes
		//en x
		if (p1y<0)
		{
			torsosy=torsosy+p1y;
			p1y=0;
		}
		if (p1y+torsosy>sy)
			torsosy=sy-p1y-1;

		//Fix container and checl dimensions
		Faces[i]->torsox1=(int)p1x;
		Faces[i]->torsoy1=(int)p1y;
		Faces[i]->torsox2=(int)p1x+torsosx;
		Faces[i]->torsoy2=(int)p1y+torsosy;

		if (Faces[i]->torsox1 <0) return false;
		if (Faces[i]->torsoy1 <0) return false;
		if (Faces[i]->torsox2 >= sx) return false;
		if (Faces[i]->torsoy2 >= sy)
		{
			if (sy-1>Faces[i]->torsoy1)
				Faces[i]->torsoy2=sy-1;
			else
				return false;
		}

		return true;
	}



	//! Number of faces
	int				NumFaces;	
	//! Face data
	CFacialData		**Faces;		

	//! Number of different subjects detected during the session
	int		nSubjects;								

	//Tiempo de proceso
	//!Frame processing time
	long	tproc;						

	private:
	CMathUtils MathUtils;	//Utilities
	CIplImageUtils IplUtils;
		
	
};

#endif

