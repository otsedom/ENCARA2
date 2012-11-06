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
 #if ! defined ( DATASETPROJECTIONS_H )
#define DATASETPROJECTIONS_H

#include "PCA.h"


//! Class Projections of a Dataset
/*!
Class containing the projections of a dataset in an PCA space
*/
class CDatasetProjections
{
public:

	//! Class constructor
	CDatasetProjections()
	{
		Init();		
	}

	//! Class constructor	
	CDatasetProjections(char *directory)
	{
		Init();		
		LoadProjections(directory);
	}

	//! Class destructor
	~CDatasetProjections()
	{
		FreeProjections();
	}


	//! Saves projections data
	/*!
	\param directory Projections location
	*/
	void SaveProjections(char *directory)
	{
		FILE *f;
		unsigned int i,j;

		//Fichero	
#ifndef ENCARA2INLINUX
		sprintf(cPath,"%s\\Projections.txt",directory);
#else
		sprintf(cPath,"%s/Projections.txt",directory);
#endif
		
		f=fopen(cPath,"wt");
		

		if(f==NULL)
		{
			//Likely the folder does not exist
#ifndef ENCARA2INLINUX
			CreateDirectory((LPCSTR)directory,0);
#else
			mkdir(directory,0);
#endif
			f=fopen(cPath,"wt");
		}

		fprintf(f,"%d\n",TotalProjections);
		fprintf(f,"%d\n",NumCoefs);

		for (i=0;i<TotalProjections;i++)
		{
			for (j=0;j<NumCoefs;j++) fprintf(f,"%f\n",P[i][j]);
		}
		fclose(f);

	}



	//! Load projections from disk
	/*!
	\param directory Projections location
	*/
	int LoadProjections(char *directory)
	{
		FILE *f;
		unsigned int i,j;
		

		//Fichero	
#ifndef ENCARA2INLINUX
		sprintf(cPath,"%s\\Projections.txt",directory);
#else
		sprintf(cPath,"%s/Projections.txt",directory);
#endif

		f=fopen(cPath,"rt");
		if (f==NULL) return -1;

		fscanf(f,"%d\n",&TotalProjections);
		fscanf(f,"%d\n",&NumCoefs);

		P=(float**)new unsigned char[sizeof(float*)*TotalProjections];

		for (i=0;i<TotalProjections;i++)
		{
			P[i]=(float*)new unsigned char[sizeof(float)*NumCoefs];
			for (j=0;j<NumCoefs;j++) fscanf(f,"%f\n",&P[i][j]);
		}
		fclose(f);

		return 1;
	}

	// GenerarProyeccionesEntrenamiento: Genera las proys. PCA de las imagenes de entrenamiento
	void ComputeProjections(CDataset *BC, CPCA *PCA)
	{
		unsigned int i;

		NumCoefs=PCA->NumCoefs;
		TotalProjections=BC->TotalImages;
		
		P=(float**)new unsigned char[sizeof(float*)*TotalProjections];

		for (i=0;i<TotalProjections;i++)
		{
			P[i]=PCA->Proyectar(BC->BC[i],NumCoefs);//Equivalenta a 
		}
	}


	// GenerarProyeccionesEntrenamiento: Genera las proys. PCA de las imagenes de entrenamiento
	void ComputeProjections(CClassifiedDataset *BC, CPCA *PCA)
	{
		unsigned int i;

		NumCoefs=PCA->NumCoefs;
		TotalProjections=BC->TotalImages;
		
		P=(float**)new unsigned char[sizeof(float*)*TotalProjections];

		for (i=0;i<TotalProjections;i++)
		{
			P[i]=PCA->Proyectar(BC->BC[i],NumCoefs);//Equivalenta a 
		}	
	}


	// FreeProjections: Memory assigned to dataset projections
	void FreeProjections()
	{
		unsigned int i;

		for (i=0;i<TotalProjections;i++)
		{
			delete [] P[i];
		}	
		delete [] P;

		Init();
	}


	//! Projection matrix
	float			**P;		
	//! Number of projections
	unsigned int	TotalProjections;	
	//! Number of coefficients used for projections
	unsigned int	NumCoefs;			


private:


	void Init()
	{
		P=NULL;
		TotalProjections=0;
		NumCoefs=0;
	}

	char cPath[256];//aux
};



#endif
