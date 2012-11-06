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
 #include "PCAUtils.h"



// SavesPCAData: Saves PCA data to disk
/*void CPCAUtils::SavesPCAData(PCAData *PCA, char *directory)
{
	FILE *f;
	unsigned int i;

	//File
#ifndef ENCARA2INLINUX
	sprintf(cPath,"%s\\PCAData.txt",directory);
#else
	sprintf(cPath,"%s/PCAData.txt",directory);
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

	fprintf(f,"%d\n",PCA->NumEigenobjects);
	fprintf(f,"%d\n",PCA->NumCoefs);
	fprintf(f,"%d\n",PCA->ImageSize.width);
	fprintf(f,"%d\n",PCA->ImageSize.height);
	fprintf(f,"%d\n",PCA->NumSamples);//not available for all sets

	for (i=0;i<PCA->NumEigenobjects;i++)
		fprintf(f,"%f\n",PCA->eigVals[i]);
	fclose(f);

#ifndef ENCARA2INLINUX
	sprintf(cPath,"%s\\Avg.Ipl",directory);
#else
	sprintf(cPath,"%s/Avg.Ipl",directory);
#endif
	IplUtils.SaveIplImage(cPath,PCA->avg);

	//Save eigenimages
	for (i=0;i<PCA->NumEigenobjects;i++)
	{
#ifndef ENCARA2INLINUX
		sprintf(cPath,"%s\\Eigenobject %d.Ipl",directory,i);
#else
		sprintf(cPath,"%s/Eigenobject %d.Ipl",directory,i);
#endif
		IplUtils.SaveIplImage(cPath,PCA->eigenObjects[i]);

		//Imagen png
		sprintf(cPath,"%s/Eigenobject %d.png",directory,i);

		cvSaveImage(cPath,PCA->eigenObjects[i]);


	}

}*/


// LoadPCAData: Load PCA data from disk, returning 0 is not found
PCAData* CPCAUtils::LoadPCAData(char *directory)
{
	FILE *f;
	unsigned int i;

	//Data File	
#ifndef ENCARA2INLINUX
	sprintf(cPath,"%s\\PCAData.txt",directory);
#else
	sprintf(cPath,"%s/PCAData.txt",directory);
#endif
	f=fopen(cPath,"rt");

	if (f==NULL)
		return NULL;

	PCAData *PCA=(PCAData*)new unsigned char[sizeof(PCAData)];
	fscanf(f,"%d\n",&(PCA->NumEigenobjects));
	fscanf(f,"%d\n",&(PCA->NumCoefs));
	fscanf(f,"%d\n",&PCA->ImageSize.width);
	fscanf(f,"%d\n",&PCA->ImageSize.height);
	fscanf(f,"%d\n",&PCA->NumSamples);//not available for old sets

	PCA->eigVals = (float*) new unsigned char[(PCA->NumEigenobjects+1)*sizeof(float)];
	for (i=0;i<PCA->NumEigenobjects;i++)
		fscanf(f,"%f\n",&PCA->eigVals[i]);
	fclose(f);
	
#ifndef ENCARA2INLINUX
	sprintf(cPath,"%s\\Avg.Ipl",directory);
#else
	sprintf(cPath,"%s/Avg.Ipl",directory);
#endif

	PCA->avg=IplUtils.LoadIplImage(cPath);

	PCA->eigenObjects=(IplImage**) new unsigned char[(PCA->NumEigenobjects+1)*sizeof(IplImage*)];

	for (i=0;i<PCA->NumEigenobjects;i++)
	{
		
#ifndef ENCARA2INLINUX
		sprintf(cPath,"%s\\Eigenobject %d.Ipl",directory,i);
#else
		sprintf(cPath,"%s/Eigenobject %d.Ipl",directory,i);
#endif
		PCA->eigenObjects[i]=IplUtils.LoadIplImage(cPath);
	}

	// To achieve the same result than  ComputePCA... (error de la primera versión)
	//PCA->eigenObjects[PCA->NumAutoobjetos]=LoadIplImage(s,1);	(Parece dar problemas si reentreno en caliente a partir de datos leidos de fichero no generados en la sesiòn)	
	PCA->eigenObjects[PCA->NumEigenobjects]=NULL;

	return(PCA);
}


//Removes PCA space directories
void CPCAUtils::RemoveDirectories() 

{
	//aux vars
	long l;

	//Projections
	chdir(ProjectionsDir);

#ifndef ENCARA2INLINUX
	struct _finddata_t fi;

	l=(long)_findfirst("*", &fi);
	if (l!=-1)
	{
		do
		{
			if (strcmp(fi.name,".") && strcmp(fi.name,"..") && strcmp(fi.name,"CVS"))
			{
				remove(fi.name);				
			}
		} while (_findnext(l, &fi)==0);
		_findclose(l);					
	}
#else

	glob_t fi;
	int i;

	//Indica el número de posiciones que aloja
	fi.gl_offs=MAXFILES;

	glob("*", GLOB_DOOFFS, NULL, &fi);

	fi.gl_offs = 2;

	for (i=0;i<(int)fi.gl_pathc;i++)
	{
		//Borra todos
		if (strcmp(fi.gl_pathv[i],".") && strcmp(fi.gl_pathv[i],"..") && strcmp(fi.gl_pathv[i],"CVS"))
		{
			remove(fi.gl_pathv[i]);				
		}
	}

	globfree(&fi);
#endif		
	

	//Eigenobjects
	chdir(EigenobjectsDir);

#ifndef ENCARA2INLINUX
	l=(long)_findfirst("*", &fi);
	if (l!=-1)
	{
		do
		{
			if (strcmp(fi.name,".") && strcmp(fi.name,"..") && strcmp(fi.name,"CVS"))
			{
				remove(fi.name);				
			}
		} while (_findnext(l, &fi)==0);
		_findclose(l);					
	}
#else
	//Indica el número de posiciones que aloja
	fi.gl_offs=MAXFILES;

	glob("*", GLOB_DOOFFS, NULL, &fi);

	//Borra
	for (i=0;i<(int)fi.gl_pathc;i++)
	{
		if (strcmp(fi.gl_pathv[i],".") && strcmp(fi.gl_pathv[i],"..") && strcmp(fi.gl_pathv[i],"CVS"))
		{
			remove(fi.gl_pathv[i]);				
		}
	}

	globfree(&fi);
#endif	

}


//For the given PCA space, loads a dataset and saves the corresponding UCI format
void CPCAUtils::SaveDatasetinUCIFormat(char *directory)
{
	ClassifiedImageDataset* tBC;
	int ii;

	char DatasetDir[256];
	sprintf(DatasetDir,"%s\\Objects\\",directory);

	tBC=LoadClassifiedDataset(DatasetDir);

	//Projections
	DatasetProjections *tPDS=(DatasetProjections*)new unsigned char[sizeof(DatasetProjections)];
	tPDS->TotalProjections=tBC->TotalImages;

	tPDS->NumCoefs=PCAspace->NumCoefs;

	tPDS->P=(float**)new unsigned char[sizeof(float*)*tPDS->TotalProjections];

	int clase=1;
	int acum=tBC->FramesPerClass[clase-1];

	char cName[256];

#ifndef ENCARA2INLINUX
	sprintf(cName,"%s\\UCI.txt",directory);
#else
	sprintf(cName,"%s/UCI.txt",directory);
#endif
	FILE *fUCI=fopen(cName,"w");

	fprintf(fUCI,"%d\n%d\n",tPDS->TotalProjections,tPDS->NumCoefs);


	for (ii=0;ii<(int)tPDS->TotalProjections;ii++)
	{
		tPDS->P[ii]=(float*)new unsigned char[sizeof(float)*tPDS->NumCoefs];

		//Etiqueta del objetivo
		//Assigns a number to each class, depending of the samples for each one
		if (ii>=acum)
		{
			clase++;
			acum+=tBC->FramesPerClass[clase-1];
		}

		//Projections
		IplImage *I=cvCreateImage( PCAspace->ImageSize, IPL_DEPTH_32F, 1 );

		if (tBC->BC[ii]->nChannels==I->nChannels)
			cvConvertScale(tBC->BC[ii],I,1,0); // convierte de 8U a 32F
		else
		{
			//Convierte primero a una imagen de grises
			IplImage *Itemp=cvCreateImage( PCAspace->ImageSize, IPL_DEPTH_8U, 1 );

			cvCvtColor( tBC->BC[ii], Itemp, CV_BGR2GRAY );
			cvConvertScale(Itemp,I,1,0); // convierte de 8U a 32F

			cvReleaseImage(&Itemp);
		}
		IplImage *itemp= cvCreateImage( PCAspace->ImageSize, IPL_DEPTH_32F, 1 );

		// restar a I la imagen media...
		//iplSubtract(I,PCA->avg, itemp);
		
		cvSub(I,PCAspace->avg, itemp);

		// multiplicar itemp por las autocaras...
		for (unsigned int j=0;j<tPDS->NumCoefs;j++)
		{
			tPDS->P[ii][j]=(float)cvDotProduct(itemp,PCAspace->eigenObjects[j]);
			fprintf(fUCI,"%f ",tPDS->P[ii][j]);//Escribe en fichero el atributo
		}
		fprintf(fUCI,"%d\n",clase);//Escribe en fichero la clase

		cvReleaseImage(&itemp);
		
		cvReleaseImage(&I);
	}

	fclose(fUCI);

	//Escribir ahora a fichero


	//Free projections
	if (tPDS!=NULL)
	{
		for (unsigned int ii=0;ii<(int)tPDS->TotalProjections;ii++)
		{
			delete [] tPDS->P[ii];
		}
		delete [] tPDS->P;
		delete [] tPDS;

		tPDS=NULL;
	}

	delete [] tBC;
}


/******************************************************************************************************

			Memory management

*******************************************************************************************************/

// FreeDataset: Memory assigned to dataset
void CPCAUtils::FreeDataset(ImageDataset **BC)
{
	unsigned int i;

	if (*BC!=NULL)
	{
		for (i=0;i<(*BC)->TotalImages;i++)	
			cvReleaseImage(&((*BC)->BC[i]));
		delete [] (*BC)->BC;
		delete [] *BC;
		*BC=NULL;
	}
}

// FreePCAData: Memory assigned to PCA data
void CPCAUtils::FreePCAData(PCAData **PCA)
{
	if (*PCA!=NULL)
	{
		delete [] (*PCA)->eigVals;
		for( unsigned int i=0; i< (*PCA)->NumEigenobjects+1; i++ ) cvReleaseImage(&((*PCA)->eigenObjects[i]));
		delete [] (*PCA)->eigenObjects;
		cvReleaseImage(&((*PCA)->avg));
		delete [] *PCA;
		*PCA=NULL;
	}
}

// FreeProjections: Memory assigned to dataset projections
void CPCAUtils::FreeProjections(DatasetProjections **PE)
{
	unsigned int i;

	if (*PE!=NULL)
	{

		for (i=0;i<(*PE)->TotalProjections;i++)
		{
			delete [] (*PE)->P[i];
		}
		delete [] (*PE)->P;
		delete [] (*PE);

		*PE=NULL;
	}
}



/********************************************************************************

	Load PCA Data and the dataset if the training is forced, or no PCA data are available

	boEntrenamientoForzosoPCA	If true forces training


****************************************************************************/
void CPCAUtils::LoadDatasetandTrain(bool boForcesTraining)
{
	//Load PCA already computed unless the training is forced
	if (!boForcesTraining)
	{
		PCAspace= new CPCA(EigenobjectsDir);

		//No PCA info found?
		if (PCAspace->eigenObjects==NULL)
		{
			delete PCAspace;
			PCAspace=NULL;
		}
	}

	//Computes PCA if the eigenobjects are not available
		//old version
		//Dataset load
	if (PCAspace==NULL)
	{
		Dataset=new CDataset(DatasetDir);

		//Images available
		if (Dataset->BC!=NULL)
		{	
			//PCA calculation
			//Creates the PCA instance
			PCAspace=new CPCA;
			//Computes the space for the given dataset
			PCAspace->ComputePCA(Dataset);
			
			//Saves PCA space
			PCAspace->SavesPCAData(EigenobjectsDir);

			//Computes projections
			Projections=new CDatasetProjections;
			Projections->ComputeProjections(Dataset,PCAspace);
			//Saves projections
			Projections->SaveProjections(ProjectionsDir);
			//end nedw
		}
		else
		{
			delete Dataset;
			Dataset=NULL;
		}
	}
}


// LoadDataset: Loads dataset from disk to memory
ImageDataset* CPCAUtils::LoadDataset(char *directory)
{
	//aux variables
	long l;
	int i=0;
	unsigned int TotalImages=0;
	bool bojpg=false;

	ImageDataset *Dataset=(ImageDataset*)new char[sizeof(ImageDataset)];

#ifndef ENCARA2INLINUX
	struct _finddata_t fi;

	//moves to directory
	_chdir(directory);

	// Checks first dataset size

	//Counts total number of images in directory


	l=(long)_findfirst("*.Ipl", &fi);
	//If not checks for jpg
	if (l==-1)
	{
		l=(long)_findfirst("*.jpg", &fi);
		bojpg=true;
	}

	if (l!=-1)
	{
		do
		{
			TotalImages++;

		} while (_findnext(l, &fi)==0);
		_findclose(l);

	}
#else
	glob_t fi;

	//Indica el número de posiciones que aloja
	fi.gl_offs=MAXFILES;

	glob("*.Ipl", GLOB_DOOFFS, NULL, &fi);

	//If not checks for jpg
	if (l==-1)
	{
		glob("*.jpg", GLOB_DOOFFS, NULL, &fi);
		bojpg=true;
	}

	//Guarda el número de imágenes encontradas
	TotalImages=(int)fi.gl_pathc;	

	globfree(&fi);
#endif


	//If there are no images return
	if (TotalImages<=0)
	{
		delete [] Dataset;
		return NULL;
	}

	//Allocs memory for the dataset images
	IplImage** BC=(IplImage**) new unsigned char[TotalImages*sizeof(IplImage*)];

	//Load images
	unsigned int Frame=0;

#ifndef ENCARA2INLINUX
	if (!bojpg)
		l=(long)_findfirst("*.Ipl", &fi);
	else
		l=(long)_findfirst("*.jpg", &fi);

	if (l!=-1)
	{
		do
		{
			if (!bojpg)
				BC[Frame] = IplUtils.LoadIplImage(fi.name);
#ifndef		USES_OF
			else
				BC[Frame] = cvLoadImage(fi.name,0);	//Fuerza cargar imagen grises				
#endif
		
			Frame++;
		} while (_findnext(l, &fi)==0);
		_findclose(l);		
	}
#else
	//Indica el número de posiciones que aloja
	fi.gl_offs=MAXFILES;

	if (!bojpg)
		glob("*.Ipl", GLOB_DOOFFS, NULL, &fi);
	else
		glob("*.jpg", GLOB_DOOFFS, NULL, &fi);
	
	for (i=0;i<(int)fi.gl_pathc;i++)
	{
		if (!bojpg)
				BC[Frame] = IplUtils.LoadIplImage(fi.gl_pathv[i]);
#ifndef		USES_OF
			else
				BC[Frame] = cvLoadImage(fi.gl_pathv[i],0);	//Fuerza cargar imagen grises				
#endif
		
			Frame++;
	}	

	globfree(&fi);
#endif

	Dataset->BC=BC;
	Dataset->TotalImages=TotalImages;
	Dataset->ImageSize=cvSize(BC[0]->width,BC[0]->height);

	return Dataset;
}




// ComputePCA: Computes PCA
PCAData* CPCAUtils::ComputePCA(ImageDataset *BC)
{
	unsigned int i;

	PCAData *PCA=(PCAData*)new unsigned char[sizeof(PCAData)];

	//Number of eigenvalues bouded by number of images in the dataset
	PCA->NumEigenobjects=BC->TotalImages-1;

	PCA->NumSamples=BC->TotalImages;

	//Bounds the number of objects to calculate
	if (PCA->NumEigenobjects>MAXEIGENOBJECTS)
		PCA->NumEigenobjects=MAXEIGENOBJECTS;

	if (PCA->NumEigenobjects>(unsigned int)(BC->ImageSize.height*BC->ImageSize.width))
		PCA->NumEigenobjects=(BC->ImageSize.height*BC->ImageSize.width)-2;

	PCA->ImageSize=BC->ImageSize;
	PCA->NumCoefs=PCA->NumEigenobjects;

	//Allocating memory
	PCA->eigenObjects=(IplImage**) new unsigned char[(PCA->NumEigenobjects+1)*sizeof(IplImage*)];

	if( !( PCA->eigVals = (float*) new unsigned char[(PCA->NumEigenobjects+1)*sizeof(float) ] ) )	exit(1);
	CvSize size=BC->ImageSize;
	if( !( PCA->avg = cvCreateImage( size, IPL_DEPTH_32F, 1 ) ) )	exit(1);
	for( i=0; i< (PCA->NumEigenobjects+1); i++ )
	{
		PCA->eigenObjects[i] = cvCreateImage( size, IPL_DEPTH_32F, 1 );
		if (!(PCA->eigenObjects[i]))	exit(1);
	}

	
	CvTermCriteria ct=cvTermCriteria(CV_TERMCRIT_ITER,PCA->NumEigenobjects,0);

	cvCalcEigenObjects(PCA->NumSamples,(void*)BC->BC, (void*)PCA->eigenObjects,
		CV_EIGOBJ_NO_CALLBACK,
		0, 
		0,
		&ct,
		PCA->avg,
		PCA->eigVals );	

	return(PCA);
}



// SaveProjections: Save projections to disk
void CPCAUtils::SaveProjections(DatasetProjections *PE, char *directory)
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

	fprintf(f,"%d\n",PE->TotalProjections);
	fprintf(f,"%d\n",PE->NumCoefs);

	for (i=0;i<PE->TotalProjections;i++)
	{
		for (j=0;j<PE->NumCoefs;j++) fprintf(f,"%f\n",PE->P[i][j]);
	}
	fclose(f);

}




// LoadProjections: Load projections from disk
DatasetProjections * CPCAUtils::LoadProjections(char *directory)
{
	FILE *f;
	unsigned int i,j;

	//Fichero
	
#ifndef ENCARA2INLINUX
	sprintf(cPath,"%s\\Projections.txt",directory);
#else
	sprintf(cPath,"%s/Projections.txt",directory);
#endif


	DatasetProjections *PE=(DatasetProjections*)new unsigned char[sizeof(DatasetProjections)];

	f=fopen(cPath,"rt");
	if (f==NULL) return NULL;

	fscanf(f,"%d\n",&PE->TotalProjections);
	fscanf(f,"%d\n",&PE->NumCoefs);

	PE->P=(float**)new unsigned char[sizeof(float*)*PE->TotalProjections];

	for (i=0;i<PE->TotalProjections;i++)
	{
		PE->P[i]=(float*)new unsigned char[sizeof(float)*PE->NumCoefs];
		for (j=0;j<PE->NumCoefs;j++) fscanf(f,"%f\n",&PE->P[i][j]);
	}
	fclose(f);

	return(PE);
}



// GenerarProyeccionesEntrenamiento: Genera las proys. PCA de las imagenes de entrenamiento
DatasetProjections * CPCAUtils::ComputeProjections(ImageDataset *BC, PCAData *PCA)
{
	unsigned int i;

	DatasetProjections *PE=(DatasetProjections *)new unsigned char[sizeof(DatasetProjections)];

	float **P=(float**)new unsigned char[sizeof(float*)*BC->TotalImages];

	for (i=0;i<BC->TotalImages;i++)
	{
		P[i]=PCAspace->Proyectar(BC->BC[i]);//Equivalenta a 
	}

	PE->P=P;
	PE->NumCoefs=PCAspace->NumCoefs;
	PE->TotalProjections=BC->TotalImages;
	return(PE);
}




// LoadDataset: Loads dataset from disk to memory
ClassifiedImageDataset* CPCAUtils::LoadClassifiedDataset(char *directory)
{
	int c,j,NumClases;
	unsigned int TotalImagenes=0;
	bool bojpg=false;

	ClassifiedImageDataset *BaseDeCaras=(ClassifiedImageDataset*)new char[sizeof(ClassifiedImageDataset)];

	chdir(directory);

	// Primero vemos cuantas clases tenemos y el total de frames...
	NumClases=0;
	bool	boOneClassHasNoImages=false;

#ifndef ENCARA2INLINUX
	struct _finddata_t fi,ff;
	long l,m;

	l=(long)_findfirst("*", &fi);	

	if (l!=-1)
	{
		do
		{
			//Si tiene atributo de subdirectorio y no es . o .. o CVS
			//if (strcmp(fi.name,".") && strcmp(fi.name,"..") && strcmp(fi.name,"CVS") && (fi.attrib==_A_SUBDIR))
			if (strcmp(fi.name,".") && strcmp(fi.name,"..") && strcmp(fi.name,"CVS") && (fi.attrib==_A_SUBDIR))
			{
				NumClases++;
				
				chdir(fi.name);

				//Check first for Ipl
				m=(long)_findfirst("*.Ipl", &ff);
				
				//If not checks for jpg
				if (m==-1)
				{

					m=(long)_findfirst("*.jpg", &ff);

					
					bojpg=true;
				}

				if (m!=-1)
				{
					do
					{
						TotalImagenes++;
					
					} while (_findnext(m, &ff)==0);


				}
				else
					boOneClassHasNoImages=true;//There are no samples for one class


				_findclose(m);

				_chdir("..");
			}

		} while (_findnext(l, &fi)==0);
		
		_findclose(l);			
	}

#else//Falta terminarlo
	int i;
	glob_t fi,ff;

	//Indica el número de posiciones que aloja
	fi.gl_offs=MAXFILES;
	ff.gl_offs=MAXFILES;

	glob("*", GLOB_DOOFFS, NULL, &fi);

	for (i=0;i<(int)fi.gl_pathc;i++)
	{
		//Asume que todos son directorios (ojo)
		if (strcmp(fi.gl_pathv[i],".") && strcmp(fi.gl_pathv[i],"..") && strcmp(fi.gl_pathv[i],"CVS"))
		{
			NumClases++;
				
			chdir(fi.gl_pathv[i]);	

			glob("*.Ipl", GLOB_DOOFFS, NULL, &ff);

			if (ff.gl_pathc==0)
			{
				glob("*.jpg", GLOB_DOOFFS, NULL, &ff);
				bojpg=true;
			}

			if (ff.gl_pathc==0)
				TotalImagenes=ff.gl_pathc;
			else
				boOneClassHasNoImages=true;//There are no samples for one class

			chdir("..");

			globfree(&ff);
		}
	}

	globfree(&fi);
	
#endif

	//Si no hay imàgenes o clases
	if (NumClases<=0 || TotalImagenes<(unsigned int)NumClases || boOneClassHasNoImages)
	{
		delete [] BaseDeCaras;
		return NULL;
	}

	unsigned int *FramesPorClase=(unsigned int*)new unsigned char[NumClases*sizeof(unsigned int)];
	IplImage** BC=(IplImage**) new unsigned char[TotalImagenes*sizeof(IplImage*)];
	char** Identidad=(char**) new unsigned char[TotalImagenes*sizeof(char*)];

	//Ahora tras saber lo que hay y cerar las estructuras, lee las imágenes
	

	c=0;
	unsigned int Frame=0;

#ifndef ENCARA2INLINUX
	l=(long)_findfirst("*", &fi);
	
	if (l!=-1)
	{
		do
		{
			//if (strcmp(fi.name,".") && strcmp(fi.name,"..") && strcmp(fi.name,"CVS") && (fi.attrib==_A_SUBDIR))
			if (strcmp(fi.name,".") && strcmp(fi.name,"..") && strcmp(fi.name,"CVS") && (fi.attrib==_A_SUBDIR))
			{			
				c++;
				_chdir(fi.name);

				j=0;

				//Check first for Ipl

				if (!bojpg)
					m=(long)_findfirst("*.Ipl", &ff);
				else
					m=(long)_findfirst("*.jpg", &ff);			

				if (m!=-1)
				{
					do
					{
						j++;
			
						if (!bojpg)
							BC[Frame] = IplUtils.LoadIplImage(ff.name);
#ifndef		USES_OF
						else
							BC[Frame] = cvLoadImage(ff.name,0);	//Fuerza cargar imagen grises		
#endif

						
						Identidad[Frame]=(char*)new unsigned char[sizeof(char)*ID_STRING_SIZE];
						strcpy(Identidad[Frame],fi.name);

						Frame++;

					} while (_findnext(m, &ff)==0);
				}
				_findclose(m);

				

				_chdir("..");
				FramesPorClase[c-1]=j;
			}

		} while (_findnext(l, &fi)==0);
		
		_findclose(l);
		}
#else		
	glob("*", GLOB_DOOFFS, NULL, &fi);

	for (i=0;i<(int)fi.gl_pathc;i++)
	{
		//Asume que todos son directorios (ojo)
		if (strcmp(fi.gl_pathv[i],".") && strcmp(fi.gl_pathv[i],"..") && strcmp(fi.gl_pathv[i],"CVS"))
		{
			c++;
			chdir(fi.gl_pathv[i]);	

			j=0;
				
			if (!bojpg)
				glob("*.Ipl", GLOB_DOOFFS, NULL, &ff);
			else
				glob("*.jpg", GLOB_DOOFFS, NULL, &ff);

			for (j=0;j<(int)ff.gl_pathc;j++)
			{
				if (!bojpg)
					BC[Frame] = IplUtils.LoadIplImage(ff.gl_pathv[j]);

#ifndef		USES_OF
				else
					BC[Frame] = cvLoadImage(ff.gl_pathv[j],0);	//Fuerza cargar imagen grises
#endif

				Identidad[Frame]=(char*)new unsigned char[sizeof(char)*ID_STRING_SIZE];
				strcpy(Identidad[Frame],fi.gl_pathv[j]);

				Frame++;

			}

			FramesPorClase[c-1]=ff.gl_pathc;
			
			chdir("..");

			globfree(&ff);
		}
	}

	globfree(&fi);
		
#endif
	
	BaseDeCaras->BC=BC;
	BaseDeCaras->FramesPerClass=FramesPorClase;
	BaseDeCaras->Label=Identidad;
	BaseDeCaras->NumClasses=NumClases;
	BaseDeCaras->TotalImages=TotalImagenes;
	BaseDeCaras->ImageSize=cvSize(BC[0]->width,BC[0]->height);

	return BaseDeCaras;
}
