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
 /////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                         //
// SVM Utils																			   //
//     
// Utilities for computing SVM given a data set, projecting and so on					   //
//
// October 2004  Modesto Castrillón	Santana												   //
// Instituto Universitario de Sistemas Inteligentes y Aplicaciones Numéricas en Ingeniería //
//                                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SVMUTILS_H
#define SVMUTILS_H

#include "IplImageUtils.h"	//Utilities for IplImage

#include "GaborW.h"

#include "LBP.h"

#include "PCAUtils.h"	//Utilities for IplImage

#include "svm.h"		//LibSVM

#include "time.h"

#ifdef ENCARA2INLINUX
#include <glob.h>
#include <unistd.h>
#endif

#include "ENCARA2_2lib_defs.h"					//ENCARA2_2lib defines


#include "Dataset.h"
#include "PCA.h"


typedef struct TSVMClassifier {
	struct svm_parameter param;		// set by parse_command_line
	struct svm_problem prob;		// set by read_problem
	struct svm_model *model;
	struct svm_node *x_space;
	unsigned int	NumClasses;
	unsigned int	*FramesPerClass;	// Images per class
	char   **Label;					//Class label
	double *feature_max,*feature_min;
	int		*voting;
	long	tproc;					//Time employed for training
} SVMClassifier;

typedef struct TClassificationRate{
	int		iNClasses;
	long	*iNsamples;
	long	*Corrects;
	long	*tproc;
} ClassificationRate;

// Sample gabor images
typedef struct TGaborImages {
	IplImage		**G;				// Gabor Image
	unsigned int	NumCoefs;
	unsigned int	TotalImages;
} GaborImages;

// Sample gabor images
typedef struct TLBPImages {
	IplImage		**I;				// LBP Image
	unsigned int	TotalImages;
} LBPImages;


//! Utilities for SVM management
/*!
Provides some routines to make use of SVM
*/
class CSVMUtils
{

public:

	//! Class constructor for PCA representation
	/*!
	Constructor used to compute the classifier given the PCA projections of the classes to analyze
	\param BaseDir Location of the classifier
	\param PCADataIn PCA space used for representation (inout samples are projected to this space)
	\param iCoefs Number of coefficients used to represent each sample
	\param ndir Used for batch executions
	\param boTrain If set forces training
	\param boAuth if set computes also n binary classifiers (recognition+verification)
	\param TBC Training images (optional)
	\param boAllInTraining By default is a TBC is given all the images are considered for training
	\param trainingratio If the previous parameter is false, this value is used to select randomly the samples to be used for training

	*/
	CSVMUtils(char *BaseDir, CPCA *PCADataIn,int iCoefs,int ndir=-1,
		bool boTrain=false,bool boAuth=false,CClassifiedDataset* TBC=NULL,
		bool boAllInTraining=true, float trainingratio=0.5f)
	{
		//Pointers initialization
		Init();
		boSomesamplesnotintraining=false;
		SamplesInTraining=NULL;
		Dataset=NULL;
		

		strcpy(cBaseDir,BaseDir);

		//Directories configuration
#ifndef ENCARA2INLINUX
		if (ndir==-1)
		{
			sprintf(DatasetDir,"%s\\Objects\\",BaseDir);
			sprintf(SVMDir,"%s\\SVM\\",BaseDir);
		}
		else//ndir is used to use different directories names
		{
			sprintf(DatasetDir,"%s\\Objects%d\\",BaseDir,ndir);
			sprintf(SVMDir,"%s\\SVM%d\\",BaseDir,ndir);
		}
#else
		if (ndir==-1)
		{
			sprintf(DatasetDir,"%s/Objects/",BaseDir);
			sprintf(SVMDir,"%s/SVM/",BaseDir);
		}
		else//ndir is used to use different directories names
		{
			sprintf(DatasetDir,"%s/Objects%d/",BaseDir,ndir);
			sprintf(SVMDir,"%s/SVM%d/",BaseDir,ndir);
		}
#endif

		boSpecificPointsforGabor=boFromPCAGabor=boFromPCA=boSpecificPointsAvailable=boSearchBestGaborConfig=false;
		boFromPCA=true;	//This means the objects are projected using PCA before training
		PCAspace=PCADataIn;
		boFromLBP=boFromGabor=boFromPCAGabor=false;
		

		//Includes verification?
		boSVMAuth=boAuth;
		
		NumCoefs=iCoefs;//Sets the number of eigenobjects to use
		if (NumCoefs>(int)PCAspace->NumCoefs)
			NumCoefs=PCAspace->NumCoefs;

		//Is the dataset given as a parameter?
		if (TBC)
		{
			Dataset=TBC;
			boDatasetGiven=true;

			//All the samples in the training set?
			if (!boAllInTraining)
			{
				boSomesamplesnotintraining=true;
				SamplesInTraining=(bool*)new unsigned char[TBC->TotalImages*sizeof(bool)];
				nInTraining=0;

				//Sets the samples to be considered for training
				for (unsigned int ns=0;ns<TBC->TotalImages;ns++)
				{
					if (((double)rand()/(double)RAND_MAX)<trainingratio)
					{
						SamplesInTraining[ns]=true;
						nInTraining++;
					}
					else
						SamplesInTraining[ns]=false;
				}
			}
		}
		else
			boDatasetGiven=false;

		//LoadDataset and train if necessary
		LoadDatasetandTrain(boTrain,boAuth);
	}

	//! Class constructor for Gabor based representation using a grid
	/*!
	Constructor used to compute the classifier given the PCA projections of the classes to analyze
	\param BaseDir Location of the classifier
	\param step Step used for the different Gabor filters. Grid step used
	\param nfilters Number of filters to be used
	\param TBC Training images
	\param boTrain If set forces training
	\param boAuth if set computes also n binary classifiers (recognition+verification)
	*/
	CSVMUtils(char *BaseDir, int step,int nfilters=16,CClassifiedDataset* TBC=NULL,
		bool boTrain=false,bool boAuth=false)
	{
		//Pointers initialization
		Init();
		boSomesamplesnotintraining=false;
		SamplesInTraining=NULL;
		Dataset=NULL;
		

		strcpy(cBaseDir,BaseDir);

		//Directories configuration
	
#ifndef ENCARA2INLINUX
		sprintf(DatasetDir,"%s\\Objects\\",BaseDir);
		sprintf(SVMDir,"%s\\SVM\\",BaseDir);
#else
		sprintf(DatasetDir,"%s/Objects/",BaseDir);
		sprintf(SVMDir,"%s/SVM/",BaseDir);
#endif

		boFromGabor=true;
		boSpecificPointsforGabor=boFromPCAGabor=boFromPCA=boSpecificPointsAvailable=boSearchBestGaborConfig=false;
		Gaborstep=step;
		boFromLBP=false;

		nGaborFilters=nfilters;//luego se usan el doble

		boSVMAuth=boAuth;

		//Is the dataset given as a parameter?
		if (TBC)
		{
			Dataset=TBC;
			boDatasetGiven=true;
		}
		else
			boDatasetGiven=false;

		//LoadDataset and train if necessary
		LoadDatasetandTrain(boTrain,boAuth);
	}

	//! Class constructor for Gabor based representation given a set of points
	/*!
	Constructor used to compute the classifier given the PCA projections of the classes to analyze
	\param BaseDir Location of the classifier
	\param posx x coordinate of the filters location
	\param posy y coordinate of the filters location
	\param npos
	\param nfilters Number of filters to be used
	\param TBC Training images
	\param boTrain If set forces training
	\param boAuth if set computes also n binary classifiers (recognition+verification)
	*/
	CSVMUtils(char *BaseDir, int *posx,int *posy, int npos,int nfilters=16,
		CClassifiedDataset* TBC=NULL,bool boTrain=false,bool boAuth=false)
	{
		//Pointers initialization
		Init();
		boSomesamplesnotintraining=false;
		SamplesInTraining=NULL;
		Dataset=NULL;
		

		if (!npos) return;

		strcpy(cBaseDir,BaseDir);

		//Directories configuration
#ifndef ENCARA2INLINUX
		sprintf(DatasetDir,"%s\\Objects\\",BaseDir);
		sprintf(SVMDir,"%s\\SVM\\",BaseDir);
#else
		sprintf(DatasetDir,"%s/Objects/",BaseDir);
		sprintf(SVMDir,"%s/SVM/",BaseDir);
#endif

		boFromGabor=true;
		boSpecificPointsforGabor=boSpecificPointsAvailable=boSearchBestGaborConfig=true;
		boFromPCAGabor=boFromPCA=false;
		boFromLBP=false;

		
		GaborPoints=(CvPoint	*)new unsigned char[sizeof(CvPoint)*npos];
		nGaborPoints=npos;
		for (int i=0;i<npos;i++)
		{
			GaborPoints[i].x=posx[i];
			GaborPoints[i].y=posy[i];
		}		

		nGaborFilters=nfilters;

		boSVMAuth=boAuth;

		//Is the dataset given as a parameter?
		if (TBC)
		{
			Dataset=TBC;
			boDatasetGiven=true;
		}
		else
			boDatasetGiven=false;

		//LoadDataset and train if necessary
		LoadDatasetandTrain(boTrain,boAuth);
	}


	//! Class constructor for Gabor based representation given a set of points
	/*!
	Constructor used to compute the classifier given the PCA projections of the classes to analyze
	\param BaseDir Location of the classifier
	\param posx x coordinate of the filters location
	\param posy y coordinate of the filters location
	\param npos
	\param nfilters Number of filters to be used
	\param ndir Used for batch executions
	\param TBC Training images
	\param boTrain If set forces training
	\param boAuth if set computes also n binary classifiers (recognition+verification)
	*/
	CSVMUtils(char *BaseDir, int *posx,int *posy, int npos,int nfilters=16,int ndir=-1,
		CClassifiedDataset* TBC=NULL,bool boTrain=false,bool boAuth=false)
	{
		//Pointers initialization
		Init();
		boSomesamplesnotintraining=false;
		SamplesInTraining=NULL;
		Dataset=NULL;
		


		if (!npos) return;

		strcpy(cBaseDir,BaseDir);

		//Directories configuration
#ifndef ENCARA2INLINUX
		if (ndir==-1)
		{
			sprintf(DatasetDir,"%s\\Objects\\",BaseDir);
			sprintf(SVMDir,"%s\\SVM\\",BaseDir);
		}
		else//ndir is used to use different directories names
		{
			sprintf(DatasetDir,"%s\\Objects%d\\",BaseDir,ndir);
			sprintf(SVMDir,"%s\\SVM%d\\",BaseDir,ndir);
		}
#else
		if (ndir==-1)
		{
			sprintf(DatasetDir,"%s/Objects/",BaseDir);
			sprintf(SVMDir,"%s/SVM/",BaseDir);
		}
		else//ndir is used to use different directories names
		{
			sprintf(DatasetDir,"%s/Objects%d/",BaseDir,ndir);
			sprintf(SVMDir,"%s/SVM%d/",BaseDir,ndir);
		}
#endif


		boFromGabor=true;
		boSpecificPointsforGabor=boSpecificPointsAvailable=boSearchBestGaborConfig=true;
		boFromPCAGabor=boFromPCA=false;
		boFromLBP=false;

		
		GaborPoints=(CvPoint	*)new unsigned char[sizeof(CvPoint)*npos];
		nGaborPoints=npos;
		for (int i=0;i<npos;i++)
		{
			GaborPoints[i].x=posx[i];
			GaborPoints[i].y=posy[i];
		}		

		nGaborFilters=nfilters;

		boSVMAuth=boAuth;


		
		//Is the dataset given as a parameter?
		if (TBC)
		{
			Dataset=TBC;
			boDatasetGiven=true;
		}
		else
			boDatasetGiven=false;

		//LoadDataset and train if necessary
		LoadDatasetandTrain(boTrain,boAuth);
	}


	//! Class constructor for Gabor based representation given a set of points not a priori given
	/*!
	Constructor used to compute the classifier given the PCA projections of the classes to analyze
	\param BaseDir Location of the classifier
	\param nfilters Number of filters to be used
	\param ndir Used for batch executions
	\param TBC Training images
	\param boTrain If set forces training
	\param boAuth if set computes also n binary classifiers (recognition+verification)
	*/
	CSVMUtils(char *BaseDir,int ndir=-1,
		CClassifiedDataset* TBC=NULL, int nfilters=16,bool boTrain=false,bool boAuth=false)
	{
		//Pointers initialization
		Init();
		boSomesamplesnotintraining=false;
		SamplesInTraining=NULL;
		Dataset=NULL;
		

		strcpy(cBaseDir,BaseDir);

		//Directories configuration
#ifndef ENCARA2INLINUX
		if (ndir==-1)
		{
			sprintf(DatasetDir,"%s\\Objects\\",BaseDir);
			sprintf(SVMDir,"%s\\SVM\\",BaseDir);
		}
		else//ndir is used to use different directories names
		{
			sprintf(DatasetDir,"%s\\Objects%d\\",BaseDir,ndir);
			sprintf(SVMDir,"%s\\SVM%d\\",BaseDir,ndir);
		}
#else
		if (ndir==-1)
		{
			sprintf(DatasetDir,"%s/Objects/",BaseDir);
			sprintf(SVMDir,"%s/SVM/",BaseDir);
		}
		else//ndir is used to use different directories names
		{
			sprintf(DatasetDir,"%s/Objects%d/",BaseDir,ndir);
			sprintf(SVMDir,"%s/SVM%d/",BaseDir,ndir);
		}
#endif


		boFromGabor=true;
		boSpecificPointsforGabor=true;
		boSpecificPointsAvailable=false;
		boSearchBestGaborConfig=true;
		boFromPCAGabor=boFromPCA=false;
		boFromLBP=false;

	
		GaborPoints=NULL;

		nGaborFilters=nfilters;

		boSVMAuth=boAuth;

		
		//Is the dataset given as a parameter?
		if (TBC)
		{
			Dataset=TBC;
			boDatasetGiven=true;
		}
		else
			boDatasetGiven=false;

		//LoadDataset and train if necessary
		LoadDatasetandTrain(boTrain,boAuth);
	}

	//! Class constructor for LBP based representation 
	/*!
	Constructor used to compute the classifier given the PCA projections of the classes to analyze
	\param BaseDir Location of the classifier
	\param ndir Used for batch executions
	\param LBPmode INdicates the LBP type
	\param windowdim Window dimension used for LBP histograms
	\param boTrain If set forces training
	\param boAuth if set computes also n binary classifiers (recognition+verification)
	\param TBC Training images (optional)
	\param boAllInTraining By default is a TBC is given all the images are considered for training
	\param trainingratio If the previous parameter is false, this value is used to select randomly the samples to be used for training

	*/
	CSVMUtils(char *BaseDir,int ndir=-1,
		int LBPmode=LBP_ORIGINAL, int windowdim=10,bool boTrain=false,bool boAuth=false,
		CClassifiedDataset* TBC=NULL,
		bool boAllInTraining=true, float trainingratio=0.5f)
	{
		//Pointers initialization
		boDatasetGiven=false;
		boSomesamplesnotintraining=false;
		SamplesInTraining=NULL;
		Dataset=NULL;
		
		Init();
		

		strcpy(cBaseDir,BaseDir);

		//Directories configuration
#ifndef ENCARA2INLINUX
		if (ndir==-1)
		{
			sprintf(DatasetDir,"%s\\Objects\\",BaseDir);
			sprintf(SVMDir,"%s\\SVM\\",BaseDir);
		}
		else//ndir is used to use different directories names
		{
			sprintf(DatasetDir,"%s\\Objects%d\\",BaseDir,ndir);
			sprintf(SVMDir,"%s\\SVM%d\\",BaseDir,ndir);
		}
#else
		if (ndir==-1)
		{
			sprintf(DatasetDir,"%s/Objects/",BaseDir);
			sprintf(SVMDir,"%s/SVM/",BaseDir);
		}
		else//ndir is used to use different directories names
		{
			sprintf(DatasetDir,"%s/Objects%d/",BaseDir,ndir);
			sprintf(SVMDir,"%s/SVM%d/",BaseDir,ndir);
		}
#endif

		boFromLBP=true;
		LBPcode=LBPmode;
		LBPdim=windowdim;
		boFromGabor=false;
		boSpecificPointsforGabor=false;
		boSpecificPointsAvailable=false;
		boSearchBestGaborConfig=false;
		boFromPCAGabor=boFromPCA=false;

	
		GaborPoints=NULL;

		nGaborFilters=0;

		boSVMAuth=boAuth;

		
		//Is the dataset given as a parameter?
		if (TBC)
		{
			Dataset=TBC;
			boDatasetGiven=true;

			//All the samples in the training set?
			if (!boAllInTraining)
			{
				boSomesamplesnotintraining=true;
				SamplesInTraining=(bool*)new unsigned char[TBC->TotalImages*sizeof(bool)];
				nInTraining=0;

				//Sets the samples to be considered for training
				for (unsigned int ns=0;ns<TBC->TotalImages;ns++)
				{
					if (((double)rand()/(double)RAND_MAX)<trainingratio)
					{
						SamplesInTraining[ns]=true;
						nInTraining++;
					}
					else
						SamplesInTraining[ns]=false;
				}
			}
		}
		else
			boDatasetGiven=false;


		//LoadDataset and train if necessary
		LoadDatasetandTrain(boTrain,boAuth);
	}


	//! Class constructor for a combination of PCA and Gabor filters
	/*!
	Constructor used to compute the classifier given the PCA projections of the classes to analyze
	\param BaseDir Location of the classifier
	\param PCADataIn PCA space used for representation
	\param iCoefs Number of coefficients used to represent each sample
	\param posx x coordinate of the filters location
	\param posy y coordinate of the filters location
	\param npos
	\param nfilters Number of filters to be used
	\param TBC Training images
	\param boTrain If set forces training
	\param boAuth if set computes also n binary classifiers (recognition+verification)
	*/
	CSVMUtils(char *BaseDir, CPCA *PCADataIn,int iCoefs,int *posx,int *posy, int npos,int nfilters=16,
		CClassifiedDataset* TBC=NULL,bool boTrain=false,bool boAuth=false)
	{
		//Pointers initialization
		Init();
		boSomesamplesnotintraining=false;
		SamplesInTraining=NULL;
		Dataset=NULL;
		


		strcpy(cBaseDir,BaseDir);

		//Directories configuration
#ifndef ENCARA2INLINUX
		sprintf(DatasetDir,"%s\\Objects\\",BaseDir);
		sprintf(SVMDir,"%s\\SVM\\",BaseDir);
#else
		sprintf(DatasetDir,"%s/Objects/",BaseDir);
		sprintf(SVMDir,"%s/SVM/",BaseDir);
#endif

		boFromLBP=false;
		boFromPCAGabor=true;	//This means the objects are projected using PCA before training
		boSpecificPointsforGabor=boSpecificPointsAvailable=true;
		boSearchBestGaborConfig=false;
		boFromPCA=boFromGabor=false;

		GaborPoints=(CvPoint	*)new unsigned char[sizeof(CvPoint)*npos];
		nGaborPoints=npos;
		for (int i=0;i<npos;i++)
		{
			GaborPoints[i].x=posx[i];
			GaborPoints[i].y=posy[i];
		}		

		nGaborFilters=nfilters;

		PCAspace=PCADataIn;

		

		boSVMAuth=boAuth;

		NumCoefsPCA=iCoefs;//Sets the number of eigenobjects to use
		if (NumCoefs>(int)PCAspace->NumCoefs)
			NumCoefs=PCAspace->NumCoefs;


		//Is the dataset given as a parameter?
		if (TBC)
		{
			Dataset=TBC;
			boDatasetGiven=true;
		}
		else
			boDatasetGiven=false;

		//LoadDataset and train if necessary
		LoadDatasetandTrain(boTrain,boAuth);
	}



	//! Class destructor
	~CSVMUtils()
	{
		FreeMem();

		if (GaborPoints)
		{
			delete [] GaborPoints;
		}

		//Situations using a subset for training
		if (boSomesamplesnotintraining)
			delete [] SamplesInTraining;
		
	}


	//! Loads the dataset and trains
	/*!
	\param boTrain If set forces training
	\param boSVMAuth If set computes a verification classifier for each class
	*/
	void LoadDatasetandTrain(bool boTrain=false,bool boSVMAuth=false);

	//! Gets the minimal distance
	/*! 
	Gets the minimal distance in PCA space for a given image to the samples of a class (between 1..n)
	\param h Input image
	\param givenclass Class
	*/
	float GetMinimalDistanceToClass(IplImage *h,int givenclass);

	//! Gets the minimal distance and the class
	/*! 
	Gets the minimal distance in PCA space for a given image and the class (between 1..n)
	\param h Input image
	\param classselected Class computed
	*/
	float GetMinimalDistanceAndClass(IplImage *h,int *classselected);

	//! Given a directory checks classification rate
	/*!
	\param directory Folder to analyze
	*/
	ClassificationRate *TestClassifier(char *directory);
	//! Given a dataset checks classification rate
	/*!
	\param TBC Dataset
	*/
	ClassificationRate *TestClassifier(ClassifiedImageDataset *TBC);
	//! Given a dataset checks classification rate
	/*!
	\param TBC Dataset
	\param InTraining If not NULL the array provided contains true for those samples used for training
	*/
	ClassificationRate *TestClassifier(CClassifiedDataset *TBC, bool *InTraining=NULL);

	//! Given a directory checks classification rate
	/*!
	\param directory Folder to analyze
	\param SVMCondition Classifier which forces a condition
	\param DirData Data location
	\param iConditionalclass Class which imposes the condition
	\param iDefaultClass Default class
	*/
	ClassificationRate *TestClassifierConditioned(char *directory,CSVMUtils *SVMCondition, 
		char *DirData,int iConditionalclass, int iDefaultClass);


	//! Classified an image
	/*!
	 Classified an image
	 \param h Input image
	 \param givenclass If not -1, performs verification
	 */
	int IplImageSVMClassification(IplImage *h,int givenclass=-1);
	
	
	CClassifiedDataset	*Dataset;		//! Image dataset

	
	//SVM classifiers
	//! n-class classifier
	SVMClassifier *ModelSVMGeneral;	
	//! biclass classifier used for verification
	SVMClassifier **ModelSVMIndividuals;			

	//! Set if the the SVM classifier is available
	bool			boSVMAvailable;					

	//! Allows us to control the number of PCA coefs to use (instead of those given by the PCA)
	int				NumCoefs;						
	//! Number of PCA coefs (used only when a combination PCA Gabor is used)
	int				NumCoefsPCA,
	//! Number of Gabor filters (Used only when a combination PCA GAbor is used)
		NumCoefsG;			

	CPCA	*PCAspace;			//! PCA space	
	CDatasetProjections *Projections; //! Projections of given dataset


	bool	boSomesamplesnotintraining; //! Is the dataset is given using the CClassifiedDataset class, there is an option to use a random seuset as trainig
	bool	*SamplesInTraining; //!Valid only if the Dataset is given as a parameter using the CClassifiedDataset class
	int		nInTraining;		//Number of samples in training (only if boSomesamplesnotintraining is true)

private:

	//Clase inicia en 0, por ello restamos -1 para acceder al clasificador correspondiente
	int SVMIndividualClassification(float *Ip,int clase);
	//Clasifies using SVM, returns a class index (attention starting in 1 not 0)
	int SVMClassification(float *Ip);
	int SVMClassification(IplImage *Ip);
	int SVMClassification(float *Ic,IplImage *Ip);

	/******************************************************************************************************

			Free memory

	*******************************************************************************************************/


	//! Frees memory
	void FreeMem()
	{

		if (Projections) delete Projections;
		if (Dataset && (!boDatasetGiven)) delete Dataset;

		if  (ModelSVMGeneral)
		{
			FreeSVMData();		
		}

		if (GI)
		{
			FreeGaborImages(&GI);
		}

		if (LBP)
		{
			FreeLBPImages(&LBP);
		}

		
		Init();
	}

	

	void Init()
	{
		Projections=NULL;
		if (!boDatasetGiven) Dataset=NULL;

		ModelSVMGeneral=NULL;
		GI=NULL;
		LBP=NULL;
		GaborPoints=NULL;

		boSVMAvailable=false;
		boSVMComputed=false;
		boSVMIndividualsComputed=false;

		
	}

	void FreeGaborImages(GaborImages **GI);
	void FreeLBPImages(LBPImages **LBP);
	void FreeSVMData();


	//! LoadDataset: Loads dataset from disk to memory
	ClassifiedImageDataset* LoadDataset(char *directory);
	SVMClassifier *LoadSVMData(char *directory);			//Load SVM Data
	SVMClassifier **LoadIndividualSVMData(char *directory);	//Load Individual SVM Data

	void SaveSVMData(char *directory);
	void SaveIndividualsSVMData(char *directory);

	// ComputeSVM: Given SVM data computes SVM classifier
	SVMClassifier * ComputeSVMfromPCA();
	SVMClassifier * ComputeSVMfromGabor();
	SVMClassifier * ComputeSVMfromLBP();
	SVMClassifier * TrainSVMfromGabor();
	SVMClassifier * ComputeSVMfromPCAGabor();
	// GenerarDatosPCA: Realiza a partir de los datos PCA, un clasificador SVM
	SVMClassifier ** ComputeSVMIndividual();
	// GenerarDatosPCA: Realiza a partir de los datos PCA, un clasificador SVM
	SVMClassifier ** ComputeSVMIndividualfromGabor();

	//Depuración
	FILE	*fdepu;			//! For debugging
	//Fin depu

	//Dataset directories
	char			cBaseDir[160];
	char			DatasetDir[200];
	char			SVMDir[256];
	
	CIplImageUtils	IplUtils;		//IplImage utilities

	char	cPath[200];				//Auxiliar image

	
	bool			boFromPCA;		//Specifies if it is necessary a PCA projection

	bool			boDatasetGiven;

	//Gabor
	bool			boFromGabor;					//Specifies if Gabor is used to transform input images
	bool			boSpecificPointsforGabor;		//If set, the Gabor is based on specific point instead of a grid
	bool			boSpecificPointsAvailable;		//If set, the set of points is known
	bool			boSearchBestGaborConfig;		//Is set, and working with specific points, the best config will be searched for training
	int				Gaborstep;						//Step used for the grid
	CGaborW			GaborUtils;						//Uso Gabor Wavelet
	CvPoint			*GaborPoints;					//Set of points
	int				nGaborPoints;					//Number of points in the set
	int				nGaborFilters;					//Number of filters used
	GaborImages		*GI;
	
	//PCA + Gabor
	bool			boFromPCAGabor;

	//LBP
	bool			boFromLBP;						//Indicates if a LBP based representation is used
	int				LBPcode;						//LBP format used
	int				LBPdim;							//Dimension of the windows used
	LBPImages		*LBP;



	bool			boSVMAuth;		//Is verification used

	bool			boSVMComputed,boSVMIndividualsComputed;	//Have them been computed



};

#endif
