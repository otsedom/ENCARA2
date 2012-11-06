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
// PCA Utils																			   //
//     
// Utilities for computing PCA given a data set, projecting and so on					   //
//
// October 2004-7  Modesto Castrillón	Santana													   //
// Instituto Universitario de Sistemas Inteligentes y Aplicaciones Numéricas en Ingeniería //
//                                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef PCAUTILS_H
#define PCAUTILS_H


#ifdef ENCARA2INLINUX
#include <glob.h>	//uso de glob
#include <sys/stat.h>	//mkdir
#include <sys/types.h>
#include <unistd.h>
#define	MAXFILES	10000
#else
#include <direct.h>
#include <io.h>
#endif

#include "ENCARA2_2lib_defs.h"					//ENCARA2_2lib defines

#include "IplImageUtils.h"	//Utilities for IplImage

#include "Dataset.h"
#include "DatasetProjections.h"
#include "PCA.h"


#define MAXEIGENOBJECTS	200				// Maximum number of eigenobjects to use


// Image dataset structure
typedef struct TImageDataset {
	IplImage		**BC;				// Samples
	CvSize			ImageSize;			// Image size
	unsigned int	TotalImages;		// Number of images
} ImageDataset;


// PCA data structure
typedef struct TPCAData {
	IplImage		*avg;				// Average image
	IplImage		**eigenObjects;		// eigenobjects (e.g. eigenfaces)
	float			*eigVals;			// eigen vals
	CvSize			ImageSize;			// image size
	unsigned int	NumEigenobjects;	// number of eigenobjects
	unsigned int	NumCoefs;			// number of coefficients to use (<=NumAutoobjetos)
	unsigned int	NumSamples;			// number of samples used to compute this space (if available)
} PCAData;

// Sample data set projections
typedef struct TDatasetProjections {
	float			**P;				// projection matrix
	unsigned int	TotalProjections;
	unsigned int	NumCoefs;
} DatasetProjections;

#ifndef TCID
#define TCID
typedef struct TClassifiedImageDataset {
	IplImage		**BC;				// Images
	unsigned int	*FramesPerClass;	// Images per class
	char			**Label;			// Class label
	unsigned int	NumClasses;
	CvSize			ImageSize;			// Image size
	unsigned int	TotalImages;		
} ClassifiedImageDataset;
#endif



//! Utilities for using PCA
/*!
This class provides basic tools for managing a PCA space
*/
class CPCAUtils
{

public:


	//! Class constructor
	/*
	Creates the structure for a PCA space, it will load or train the PCA space
	\param BaseDir Folder where the PCA or source information is located
	\param boTrainPCA If set, the training is forced even if the space were available
	*/
	CPCAUtils(char *BaseDir, bool boTrainPCA=false)
	{
		strcpy(cBaseDir,BaseDir);

		//Directories configuration
#ifndef ENCARA2INLINUX
		sprintf(DatasetDir,"%s\\Dataset\\",BaseDir);
		sprintf(EigenobjectsDir,"%s\\Eigenobjects\\",BaseDir);
		sprintf(ProjectionsDir,"%s\\Projections\\",BaseDir);
#else
		sprintf(DatasetDir,"%s/Dataset/",BaseDir);
		sprintf(EigenobjectsDir,"%s/Eigenobjects/",BaseDir);
		sprintf(ProjectionsDir,"%s/Projections/",BaseDir);
#endif

		//Init
		Init();
		
		//LoadDataset and train if necessary
		LoadDatasetandTrain(boTrainPCA);

		//VerCaras(BC);
	};

	//! Class destructor
	/*
	Removes used memory
	*/
	~CPCAUtils()
	{

		if (Projections) delete Projections;
		if (Dataset) delete Dataset;
		if (PCAspace) delete PCAspace;	
	};

	

	//! SavesPCAData: Saves PCA data to disk
	/*
	\param PCA Data to be saved
	\param directory Folder to save the data
	*/
	//void SavesPCAData(PCAData *PCA, char *directory);

	//! Load PCA data from disk.
	/*
	Load PCA data from disk, returning 0 if it is not found
	\param directory Folder to save the data
	*/
	PCAData* LoadPCAData(char *directory);


	//! For the given PCA space, loads a dataset and saves the corresponding UCI format
	void SaveDatasetinUCIFormat(char *directory);

	//! Removes PCA space directories.
	/*!
	Removes PCA space directories forcing a training during next load
	*/
	void RemoveDirectories();	


	//! Image dataset
	CDataset	*Dataset;	
	//! PCA space	
	CPCA	*PCAspace;			
	//! Projections of given dataset
	CDatasetProjections *Projections; 

private:

	void Init()
	{
		//Initialization
		Dataset=NULL;
		PCAspace=NULL;
		Projections=NULL;
		
	}
	
	// FreeDataset: Memory assigned to dataset
	void FreeDataset(ImageDataset **BC);
	// FreePCAData: Memory assigned to PCA data
	void FreePCAData(PCAData **PCA);
	// FreeProjections: Memory assigned to dataset projections
	void FreeProjections(DatasetProjections **PE);

	//Loads PCA data unless training is forced
	void LoadDatasetandTrain(bool boForcesTraining);
	// LoadDataset: Loads dataset from disk to memory
	ImageDataset* LoadDataset(char *directory);
	// ComputePCA: Computes PCA
	PCAData* ComputePCA(ImageDataset *BC);
	// SaveProjections: Save projections to disk
	void SaveProjections(DatasetProjections *PE, char *directory);
	// LoadProjections: Load projections from disk
	DatasetProjections * LoadProjections(char *directory);
	// GenerarProyeccionesEntrenamiento: Genera las proys. PCA de las imagenes de entrenamiento
	DatasetProjections * ComputeProjections(ImageDataset *BC, PCAData *PCA);
	// LoadDataset: Loads dataset from disk to memory
	ClassifiedImageDataset* LoadClassifiedDataset(char *directory);

	//Dataset directories
	char			cBaseDir[160];
	char			DatasetDir[200];
	char			EigenobjectsDir[200];
	char			ProjectionsDir[200];

	
	CIplImageUtils	IplUtils;		//! IplImage utilities

	char	cPath[200];				//! Auxiliar image

};

#endif
