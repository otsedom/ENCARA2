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
 #include "SVMUtils.h"

//Loads training set and/or trains
void CSVMUtils::LoadDatasetandTrain(bool boTrain,bool boSVMAuth)
{
	//Frees previous memory
	FreeMem();

	//Compute SVM
	if (!boTrain)
	{
		if (boFromGabor || boFromPCAGabor)//Necesitamos conocer la dimensión
		{
			if (Dataset==NULL)
			//Cargamos las caras si no están cargadas
				Dataset=new CClassifiedDataset(DatasetDir);
		}

		ModelSVMGeneral=LoadSVMData(SVMDir);
	}

	//Si no tenemos el SVM multiclase, lo calculamos
	if (ModelSVMGeneral==NULL)
	{
		if (Dataset==NULL)
		//Cargamos las caras si no están cargadas (y las de Gabor si fuera necesario)
			Dataset=new CClassifiedDataset(DatasetDir,boSearchBestGaborConfig,false);


		//If there are samples to compute the classifier
		if (Dataset)
		{
			if (Dataset->NumClasses>1)
			{
				if (boFromPCA)
					ModelSVMGeneral=ComputeSVMfromPCA();
				else
				{
					if (boFromGabor)
						ModelSVMGeneral=ComputeSVMfromGabor();
					else//boFromPCAGabor o boFromLBP
					{
						if (boFromLBP)
							ModelSVMGeneral=ComputeSVMfromLBP();
						else//boFromPCAGabor
							ModelSVMGeneral=ComputeSVMfromPCAGabor();
					}
				}


				SaveSVMData(SVMDir);

				boSVMAvailable=true;//SVM available
			}
			else
			{
				boSVMAvailable=false;//SVM available
				delete Dataset;
				Dataset=NULL;
			}
		}
		else//Dataset ==NULL
			boSVMAvailable=false;//SVM available
	}
	else 
		boSVMAvailable=true;//SVM available

	//If verification SVMs is used
	if (boSVMAuth && ModelSVMGeneral)
	{
		ModelSVMIndividuals=NULL;
		if (!boTrain)
			ModelSVMIndividuals=LoadIndividualSVMData(SVMDir);

		if (ModelSVMIndividuals==NULL && Dataset)
		{
			if (boFromPCA)
				ModelSVMIndividuals=ComputeSVMIndividual();
			else //boFromGabor
			{
				ModelSVMIndividuals=ComputeSVMIndividualfromGabor();					
			}
			
			if (ModelSVMIndividuals) SaveIndividualsSVMData(SVMDir);
		}

	}
}

//????Más lógico en PCAUtils???
//gets the minimal distance in PCA space for a given image to the samples of a class (between 1..n)
float CSVMUtils::GetMinimalDistanceToClass(IplImage *h,int givenclass)
{
	int i;
	float mindist=(float)1e20;

	if (boFromPCA && boSVMAvailable)
	{
		//Cargamos las caras si no están cargadas
		if (Dataset==NULL)				
			Dataset=new CClassifiedDataset(DatasetDir);

		//Proyectamos las caras si no están en memoria
		if (Projections==NULL)
		{
			Projections=new CDatasetProjections();
			Projections->ComputeProjections(Dataset,PCAspace);
		}

		float *coefs;
		coefs=PCAspace->Proyectar(h);//Equivalenta a 


		//coefs tiene la proyección de la imagen test

		//Buscamos mínima distancia
		
		int clase=1;
		int acum=Dataset->FramesPerClass[clase-1];
		i=0;
		while (i<(int)Projections->TotalProjections && clase<=givenclass)
	
		{				
			//Recorre las clases
			if (i>=acum)
			{
				clase++;

				acum+=Dataset->FramesPerClass[clase-1];
			}

			//Chequea si estamos en la clase que nos interesa
			if (clase==givenclass)
			{
				float dist=0;

				//Obtiene la distancia euclidea (probar con mahalanobis también)
				for (int n=0;n<(int)NumCoefs;n++)
				{
					dist+=(Projections->P[i][n]-coefs[n])*(Projections->P[i][n]-coefs[n]);
				}

				dist=(float)sqrt(dist);

				if (dist<mindist)
					mindist=dist;
			}

			i++;
		}

		delete [] coefs;

		return mindist;

	}
	else
		return (float)1e20;

}


//gets the minimal distance in PCA space for a given image and the class (between 1..n)
float CSVMUtils::GetMinimalDistanceAndClass(IplImage *h,int *classselected)
{
	int i;
	float mindist=(float)1e20;

	if (boFromPCA && boSVMAvailable)
	{
		if (Dataset==NULL)				
			Dataset=new CClassifiedDataset(DatasetDir);

		//Proyectamos las caras si no están en memoria
		if (Projections==NULL)
		{
			Projections=new CDatasetProjections();
			Projections->ComputeProjections(Dataset,PCAspace);
		}

		float *coefs;
		coefs=PCAspace->Proyectar(h);//Equivalenta a 

		//coefs tiene la proyección de la imagen test

		//Buscamos mínima distancia
		
		int clase=1;
		int acum=Dataset->FramesPerClass[clase-1];
		i=0;
		while (i<(int)Projections->TotalProjections)

		{				
			//Recorre las clases
			if (i>=acum)
			{
				clase++;
				acum+=Dataset->FramesPerClass[clase-1];
			}

			float dist=0;

			//Obtiene la distancia euclidea (probar con mahalanobis también)
			for (int n=0;n<(int)NumCoefs;n++)
			{
				//Euclidea
				dist+=(Projections->P[i][n]-coefs[n])*(Projections->P[i][n]-coefs[n]);
			}

			dist=(float)sqrt(dist);

			if (dist<mindist)
			{
				mindist=dist;
				*classselected=clase;
			}
			

			i++;
		}

		delete [] coefs;

		return mindist;

	}
	else
		return (float)1e20;

}


//Given a directory checks classification rate
ClassificationRate *CSVMUtils::TestClassifier(char *directory)
{
	bool bjpeg=false;
	int ii=0;

	ClassificationRate *results=(ClassificationRate *)new unsigned char[sizeof(ClassificationRate)];

	results->iNClasses=ModelSVMGeneral->NumClasses;
	results->iNsamples=(long *)new unsigned char [sizeof(long)*results->iNClasses];
	results->Corrects=(long *)new unsigned char [sizeof(long)*results->iNClasses];
	results->tproc=(long *)new unsigned char [sizeof(long)*results->iNClasses];

	
	//For each class
	for (int i=0;i<(int)ModelSVMGeneral->NumClasses;i++)
	{
		//Changes directory to be exactly where samples for this class are
		chdir(directory);
		chdir(ModelSVMGeneral->Label[i]);

		//resets counter for this class
		results->iNsamples[i]=0;
		results->Corrects[i]=0;
		results->tproc[i]=0;

		//Loads and process each sample
		IplImage *h;
		long l;

		unsigned int Frame=0;
		
#ifndef ENCARA2INLINUX
		struct _finddata_t fi;

		//Load images			

		l=(long)_findfirst("*.Ipl", &fi);

		if (l==-1)
		{	
			l=(long)_findfirst("*.jpg", &fi);

			if (l!=-1) bjpeg=true;
		}
		
		if (l!=-1)
		{
			do
			{
				if (!bjpeg)
					h = IplUtils.LoadIplImage(fi.name);
#ifndef		USES_OF
				else
					h= cvLoadImage(fi.name,0); //Loads the image in gray levels
					//h= cvLoadImage(fi.name,0); //Loads the image in RGB
#endif
			
				results->iNsamples[i]++;

				//Procesa la imagen
				clock_t	tini=clock();

				int clase=IplImageSVMClassification(h);

				results->tproc[i]+=(clock()-tini);

				//Checks if correct
				if (clase-1==i)
					results->Corrects[i]++;
				

				cvReleaseImage(&h);

			} while (_findnext(l, &fi)==0);
			
			_findclose(l);

		}
#else
		glob_t fi;

		fi.gl_offs = MAXFILES;

		glob("*.Ipl", GLOB_DOOFFS, NULL, &fi);

		if (l==-1)
		{	
			glob("*.jpg", GLOB_DOOFFS, NULL, &fi);

			if (l!=-1) bjpeg=true;
		}

		for (ii=0;ii<(int)fi.gl_pathc;ii++)
		{
			if (!bjpeg)
				h = IplUtils.LoadIplImage(fi.gl_pathv[ii]);
#ifndef		USES_OF
			else
				h= cvLoadImage(fi.gl_pathv[ii],0); //Loads the image in gray levels
				//h= cvLoadImage(fi.gl_pathv[ii],0); //Loads the image in RGB
#endif
		
			results->iNsamples[i]++;

			//Procesa la imagen
			clock_t	tini=clock();

			int clase=IplImageSVMClassification(h);

			results->tproc[i]+=(clock()-tini);

			//Checks if correct
			if (clase-1==i)
				results->Corrects[i]++;

			cvReleaseImage(&h);
		}

		globfree(&fi);
#endif
	}

	return results;
}


//Given a dataset checks classification rate
ClassificationRate *CSVMUtils::TestClassifier(ClassifiedImageDataset *TBC)
{
	bool bjpeg=false;

	ClassificationRate *results=(ClassificationRate *)new unsigned char[sizeof(ClassificationRate)];

	results->iNClasses=TBC->NumClasses;
	results->iNsamples=(long *)new unsigned char [sizeof(long)*results->iNClasses];
	results->Corrects=(long *)new unsigned char [sizeof(long)*results->iNClasses];
	results->tproc=(long *)new unsigned char [sizeof(long)*results->iNClasses];

	unsigned int Frame=0;

	//For each class
	for (int i=0;i<(int)TBC->NumClasses;i++)
	{
		
		//resets counter for this class
		results->iNsamples[i]=0;
		results->Corrects[i]=0;
		results->tproc[i]=0;

		for (int j=0;j<(int)TBC->FramesPerClass[i];j++)
		{	
				results->iNsamples[i]++;

				//Procesa la imagen
				clock_t	tini=clock();

				int clase;
				
				clase=IplImageSVMClassification(TBC->BC[Frame++]);

				results->tproc[i]+=(clock()-tini);

				//Checks if correct
				if (clase-1==i)
					results->Corrects[i]++;

		}

	}

	return results;
}

//Given a dataset checks classification rate
ClassificationRate *CSVMUtils::TestClassifier(CClassifiedDataset *TBC, bool *InTraining)
{
	bool bjpeg=false;

	ClassificationRate *results=(ClassificationRate *)new unsigned char[sizeof(ClassificationRate)];

	results->iNClasses=TBC->NumClasses;
	results->iNsamples=(long *)new unsigned char [sizeof(long)*results->iNClasses];
	results->Corrects=(long *)new unsigned char [sizeof(long)*results->iNClasses];
	results->tproc=(long *)new unsigned char [sizeof(long)*results->iNClasses];

	unsigned int Frame=0;

	//For each class
	for (int i=0;i<(int)TBC->NumClasses;i++)
	{
		
		//resets counter for this class
		results->iNsamples[i]=0;
		results->Corrects[i]=0;
		results->tproc[i]=0;

		for (int j=0;j<(int)TBC->FramesPerClass[i];j++)
		{	

			bool bocheckimage=true;

			//No lo analiza si está dado como incluido en el conjunto de entrenamiento
			if (InTraining)
			{
				if (InTraining[Frame])
					bocheckimage=false;
			}

			if (bocheckimage)
			{

				results->iNsamples[i]++;

				//Procesa la imagen
				clock_t	tini=clock();

				int clase;

				if (boSpecificPointsforGabor && boSpecificPointsAvailable)//Usamos la imagen de Gabor
					clase=IplImageSVMClassification(TBC->BCGabor[Frame]);
				else//Usamos la imagen de grises (en ocasiones se transformará a LBP si así lo exigiera el clasificador)
					clase=IplImageSVMClassification(TBC->BC[Frame]);

				results->tproc[i]+=(clock()-tini);

				//Checks if correct
				if (clase-1==i)
					results->Corrects[i]++;

			}

			//Next image
			Frame++;
		}

	}

	return results;
}


//Given a directory checks classification rate
ClassificationRate *CSVMUtils::TestClassifierConditioned(char *directory,CSVMUtils *SVMCondition, char *DirData,int iConditionalclass, int iDefaultClass)
{
	bool bjpeg=false;

	int Conditionedclass;
	int j;

	ClassificationRate *results=(ClassificationRate *)new unsigned char[sizeof(ClassificationRate)];

	results->iNClasses=ModelSVMGeneral->NumClasses;
	results->iNsamples=(long *)new unsigned char [sizeof(long)*results->iNClasses];
	results->Corrects=(long *)new unsigned char [sizeof(long)*results->iNClasses];
	results->tproc=(long *)new unsigned char [sizeof(long)*results->iNClasses];

	
	//For each class
	for (int i=0;i<(int)ModelSVMGeneral->NumClasses;i++)
	{
		//Changes directory to be exactly where samples for this class are
		chdir(directory);
		chdir(ModelSVMGeneral->Label[i]);

		//resets counter for this class
		results->iNsamples[i]=0;
		results->Corrects[i]=0;
		results->tproc[i]=0;

		//Loads and process each sample
		IplImage *h,*hcond;
		long l;
#ifndef ENCARA2INLINUX
		struct _finddata_t fi;

		l=(long)_findfirst("*.Ipl", &fi);

		if (l==-1)
		{
			l=(long)_findfirst("*.jpg", &fi);

			if (l!=-1) bjpeg=true;
		}
		unsigned int Frame=0;
		if (l!=-1)
		{
			do
			{
				//Opens image for conditional classifier
				_chdir(DirData);
				hcond=NULL;
				j=0;

				//Searches for the image in the directories of the other classifier (DUE TO WHOLE NORMALIZED IMAGE AND MOUTH AREA)
				do{
					_chdir(SVMCondition->ModelSVMGeneral->Label[j++]);

					if (!bjpeg)
						hcond = IplUtils.LoadIplImage(fi.name);
#ifndef		USES_OF
					else
						hcond= cvLoadImage(fi.name,0); //Loads the image in gray levels
#endif

					_chdir("..");
				} while (hcond==NULL && j<(int)SVMCondition->ModelSVMGeneral->NumClasses);

				int clase;
				clock_t	tini=clock();

				if (hcond)
				{
					//Checks class conditioned classifier
					Conditionedclass=SVMCondition->IplImageSVMClassification(hcond);

					if (Conditionedclass==iConditionalclass)
					{
						//If correct conditional class, opens image
						_chdir(directory);
						_chdir(ModelSVMGeneral->Label[i]);

						if (!bjpeg)
							h = IplUtils.LoadIplImage(fi.name);
#ifndef		USES_OF
						else
							h= cvLoadImage(fi.name,0); //Loads the image in gray levels
#endif
				
						//Procesa la imagen
						
						clase=IplImageSVMClassification(h);

						cvReleaseImage(&h);
					}
					else
						clase=iDefaultClass;
				}
				else
					clase=iDefaultClass;

				results->iNsamples[i]++;

				results->tproc[i]+=(clock()-tini);

				//Checks if correct
				if (clase-1==i)
					results->Corrects[i]++;

				cvReleaseImage(&hcond);

			} while (_findnext(l, &fi)==0);
			
			_findclose(l);

		}


#else//Falta con glob y globfree
	
#endif


	}

	return results;
}

int CSVMUtils::IplImageSVMClassification(IplImage *h,int givenclass)
{
	
	int iclass;

	if (boFromPCA || boFromPCAGabor)
	{
		float *coefs;
		
		if (boFromPCA)
			coefs=(float*)new unsigned char[sizeof(float)*NumCoefs];
		else
			coefs=(float*)new unsigned char[sizeof(float)*NumCoefsPCA];

		//Projections
		IplImage *I=cvCreateImage( PCAspace->ImageSize, IPL_DEPTH_32F, 1 );
		
		if (h->nChannels==I->nChannels)
			cvConvertScale(h,I,1,0); // convierte de 8U a 32F
		else
		{
			//Convierte primero a una imagen de grises
			IplImage *Itemp=cvCreateImage( PCAspace->ImageSize, IPL_DEPTH_8U, 1 );

			cvCvtColor( h, Itemp, CV_BGR2GRAY );
			cvConvertScale(Itemp,I,1,0); // convierte de 8U a 32F

			cvReleaseImage(&Itemp);
		}

		
		IplImage *itemp= cvCreateImage( PCAspace->ImageSize, IPL_DEPTH_32F, 1 );

		// restar a I la imagen media...
		cvSub(I,PCAspace->avg, itemp);

		cvReleaseImage(&I);

		if (boFromPCA)
		{
			// multiplicar itemp por las autocaras...
			for (int j=0;j<NumCoefs;j++)
				coefs[j]=(float)cvDotProduct(itemp,PCAspace->eigenObjects[j]);

			//Classifies given projection
			if (givenclass<0)//clasificador n clases
				iclass=SVMClassification(coefs);
			else	
				iclass=SVMIndividualClassification(coefs,givenclass);//clasifica usando verificación
		}
		else
		{
			// multiplicar itemp por las autocaras...
			for (int j=0;j<NumCoefsPCA;j++)
				coefs[j]=(float)cvDotProduct(itemp,PCAspace->eigenObjects[j]);

			IplImage *ipltmp;
		
			if (boSpecificPointsforGabor)
				ipltmp=GaborUtils.GaborWavelet(h,GaborPoints,nGaborPoints,nGaborFilters*2);
			else
				ipltmp=GaborUtils.GaborWavelet(h,Gaborstep,nGaborFilters*2);

			//Classifies the image
			iclass=SVMClassification(coefs,ipltmp);

			cvReleaseImage(&ipltmp);
		}

		cvReleaseImage(&itemp);

		delete [] coefs;
	}

	if (boFromLBP)
	{
		CLBP lbp;

		//Muy temporal
		IplImage *hlbp=lbp.GetLBPImage(h,0,LBPcode);//Debe desaparecer, gasta recursos

		//Obtenemos el histograma concatenado de la imagen
		float *Hist;
		int ndims;
		lbp.GetLBPHistogramFromLBPGridf(hlbp,LBPdim,&Hist,&ndims,LBPcode);

		cvReleaseImage(&hlbp);//Debe desaparecer, gasta

		//Classification
		iclass=SVMClassification(Hist);

		delete [] Hist;


	}

	if (boFromGabor)
	{
		if (!(boSpecificPointsforGabor && boSpecificPointsAvailable))
		{
			IplImage *ipltmp;

			if (boSpecificPointsforGabor)
				ipltmp=GaborUtils.GaborWavelet(h,GaborPoints,nGaborPoints,nGaborFilters*2);
			else
				ipltmp=GaborUtils.GaborWavelet(h,Gaborstep,nGaborFilters*2);

			//Classifies the image
			iclass=SVMClassification(ipltmp);

			cvReleaseImage(&ipltmp);
		}
		else
		{
			float *coefs=(float *)new unsigned char[NumCoefs*sizeof(float)];
			int nc=0;
			
			//Copies values
			for (int n=0;n<nGaborPoints;n++)
			{
				BYTE *p=(BYTE *)h->imageData+GaborPoints[n].y*h->widthStep+GaborPoints[n].x*nGaborFilters*2*sizeof(float);
				float *pf;

				pf=(float *)p;

				//Cada canal
				for (int pu=0;pu<nGaborFilters*2;pu++,pf++,nc++)
				{
					coefs[nc]=*pf;
				}
			}

			//Classification
			iclass=SVMClassification(coefs);

			delete [] coefs;
		}

		
	}
	

	return iclass;
}



// LoadDataset: Loads dataset from disk to memory
ClassifiedImageDataset* CSVMUtils::LoadDataset(char *directory)
{
	long l,m;
	
#ifndef ENCARA2INLINUX
	struct _finddata_t fi,ff;
#else
	glob_t fi;
#endif
	int c,j,NumClases;
	unsigned int TotalImagenes=0;
	bool bojpg=false;

	
	chdir(directory);

	// Primero vemos cuantas clases tenemos y el total de frames...
	NumClases=0;
	bool	boOneClassHasNoImages=false;
	
#ifndef ENCARA2INLINUX
	l=(long)_findfirst("*", &fi);

	if (l!=-1)
	{
		do
		{
			//Si tiene atributo de subdirectorio y no es . o .. o CVS
			//if (strcmp(fi.name,".") && strcmp(fi.name,"..") && strcmp(fi.name,"CVS") && (fi.attrib==_A_SUBDIR))
			if (strcmp(fi.name,".") && strcmp(fi.name,"..") && strcmp(fi.name,"CVS") && (fi.attrib==_A_SUBDIR))
			{
		//		TRACE("\n%s",fi.name);
				NumClases++;
				
				_chdir(fi.name);

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


#else //Falta con glob y globfree para linux
	
#endif
	

	//Si no hay imàgenes o sólo una clase
	if (NumClases<=1 || TotalImagenes<(unsigned int)NumClases || boOneClassHasNoImages)
	{
		TotalImagenes=0;
		return NULL;
	}

	ClassifiedImageDataset *BaseDeCaras=(ClassifiedImageDataset*)new char[sizeof(ClassifiedImageDataset)];

	unsigned int *FramesPorClase=(unsigned int*)new unsigned char[NumClases*sizeof(unsigned int)];
	IplImage** BC=(IplImage**) new unsigned char[TotalImagenes*sizeof(IplImage*)];
	char** Identidad=(char**) new unsigned char[TotalImagenes*sizeof(char*)];

	//Ahora tras saber lo que hay y cerrar las estructuras, lee las imágenes
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
			//	TRACE("\n%s",fi.name);
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
				//		TRACE("\n\t%s",ff.name); 

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
#else//Falta glob y globfree para linux
	
#endif
	

	BaseDeCaras->BC=BC;
	BaseDeCaras->FramesPerClass=FramesPorClase;
	BaseDeCaras->Label=Identidad;
	BaseDeCaras->NumClasses=NumClases;
	BaseDeCaras->TotalImages=TotalImagenes;
	BaseDeCaras->ImageSize=cvSize(BC[0]->width,BC[0]->height);

	return BaseDeCaras;
}


SVMClassifier *CSVMUtils::LoadSVMData(char *directory)
{
	int i;

	//File
	if (boFromPCA || boFromLBP)
		sprintf(cPath,"%sSVM_%d",directory,NumCoefs);
	else
	{
	
		if (boSpecificPointsforGabor)
		{
			//Standard Gabor
			if (!boFromPCAGabor)
			{
				//Sets the classifier name

				//Points given?
				if (boSpecificPointsAvailable)
				{
					sprintf(cPath,"%sSVM_GaborP_%d_%d",directory,nGaborPoints,nGaborFilters*2);

					//Includes the points in the name
					char ctmp[256];
					strcpy(ctmp,cPath);
					for (int i=0;i<nGaborPoints;i++)
					{
						sprintf(ctmp,"%s_%d_%d",cPath,GaborPoints[i].x,GaborPoints[i].y);
						strcpy(cPath,ctmp);
					}
				}
				else//Points saved in classifier folder
				{
					sprintf(cPath,"%sSVM_GaborPoints_%d.txt",directory,nGaborFilters*2);

					FILE *fp=fopen(cPath,"r");
					nGaborPoints=0;

					//Points available?
					if (fp)
					{
						//Number of points
						fscanf(fp,"%d\n",&nGaborPoints);

						GaborPoints=(CvPoint	*)new unsigned char[sizeof(CvPoint)*nGaborPoints];
						
						//Reads the points
						for (int i=0;i<nGaborPoints;i++)
						{
							fscanf(fp,"%d ",&GaborPoints[i].x);
							fscanf(fp,"%d\n",&GaborPoints[i].y);
						}	

						boSpecificPointsAvailable=true;

						fclose(fp);
					}

					//SVM Model filename
					sprintf(cPath,"%sSVM_GaborP_%d_%d",directory,nGaborPoints,nGaborFilters*2);

					//Includes the points in the name
					char ctmp[256];
					strcpy(ctmp,cPath);
					for (int i=0;i<nGaborPoints;i++)
					{
						sprintf(ctmp,"%s_%d_%d",cPath,GaborPoints[i].x,GaborPoints[i].y);
						strcpy(cPath,ctmp);
					}
				}

				NumCoefs=nGaborPoints*nGaborFilters*2;
			}
			else//boFromPCAGabor
			{
				sprintf(cPath,"%sSVM_PCAGaborP%d_%d",directory,nGaborPoints,nGaborFilters*2);

				NumCoefsG=nGaborPoints*nGaborFilters*2;
				NumCoefs=NumCoefsG+NumCoefsPCA;

			}
		}
		else
		{
			sprintf(cPath,"%sSVM_Gabor_%d",directory,Gaborstep);
			NumCoefs=((Dataset->BC[0]->width-18)/Gaborstep +1)*((Dataset->BC[0]->height-9)/Gaborstep +1)*nGaborFilters*2;		}		
		
	}

	
	struct svm_model *model=svm_load_model(cPath);

	if (model==NULL)
		return NULL;

	SVMClassifier *ModelSVM=(SVMClassifier*)new unsigned char[sizeof(SVMClassifier)];

	//
	ModelSVM->model=model;

	//Opens file
	if (boFromPCA  || boFromLBP)
	{		
#ifndef ENCARA2INLINUX
		sprintf(cPath,"%s\\SVMData_%d.txt",directory,NumCoefs);
#else
		sprintf(cPath,"%s/SVMData_%d.txt",directory,NumCoefs);
#endif
	}
	else
	{
		if (boSpecificPointsforGabor)
		{
			if (!boFromPCAGabor)
			{			
#ifndef ENCARA2INLINUX
				sprintf(cPath,"%s\\SVMData_GaborP%d_%d",directory,nGaborPoints,nGaborFilters*2);
#else
				sprintf(cPath,"%s/SVMData_GaborP%d_%d",directory,nGaborPoints,nGaborFilters*2);
#endif
				
				//Includes the points in the name
				char ctmp[256];
				strcpy(ctmp,cPath);
				for (int i=0;i<nGaborPoints;i++)
				{
					sprintf(ctmp,"%s_%d_%d",cPath,GaborPoints[i].x,GaborPoints[i].y);
					if (i==nGaborPoints-1)
						sprintf(cPath,"%s.txt",ctmp);
					else
						strcpy(cPath,ctmp);
				}
			}
			else
			{
				
#ifndef ENCARA2INLINUX
				sprintf(cPath,"%s\\SVMData_PCAGaborP%d_%d.txt",directory,nGaborPoints,nGaborFilters*2);
#else
				sprintf(cPath,"%s/SVMData_PCAGaborP%d_%d.txt",directory,nGaborPoints,nGaborFilters*2);
#endif
			}
		}
		else
		{				
#ifndef ENCARA2INLINUX
			sprintf(cPath,"%s\\SVMData_Gabor_%d.txt",directory,Gaborstep);
#else
			sprintf(cPath,"%s/SVMData_Gabor_%d.txt",directory,Gaborstep);
#endif
		}
	}
		
	FILE *fp=fopen(cPath,"r");

	//Gets num of classes
	fscanf(fp,"%d\n",&ModelSVM->NumClasses); 

	//time needed to compute
	fscanf(fp,"%ld\n",&ModelSVM->tproc);

	ModelSVM->feature_max=(double *)new unsigned char[sizeof(double)*NumCoefs];
	ModelSVM->feature_min=(double *)new unsigned char[sizeof(double)*NumCoefs];
	ModelSVM->Label =(char**) new unsigned char [ModelSVM->NumClasses*sizeof(char*)];
	ModelSVM->FramesPerClass =(unsigned int*) new unsigned char[ModelSVM->NumClasses*sizeof(unsigned int)];
	ModelSVM->voting=(int *)new char[sizeof(int)*(ModelSVM->NumClasses+1)];

	
	//Etiquetas identidades
	for (i=0;i<(int)ModelSVM->NumClasses;i++)
	{
		ModelSVM->Label[i]=(char*)new unsigned char[sizeof(char)*ID_STRING_SIZE];
		fscanf(fp,"%s %d\n",ModelSVM->Label[i],&(ModelSVM->FramesPerClass[i]));
	}
	//Màximos y mìnimos atributos
	for (i=0;i<(int)NumCoefs;i++)
		fscanf(fp,"%lf %lf\n",&(ModelSVM->feature_max[i]),&(ModelSVM->feature_min[i]));

	//Lee los puntos
	if (fscanf(fp,"%d\n",&nGaborPoints)!=EOF)
	{
		if (nGaborPoints)
		{
			boSpecificPointsforGabor=true;
			boFromGabor=true;
			for (int i=0;i<nGaborPoints;i++)
			{
				fscanf(fp,"%d %d\n",&GaborPoints[i].x,&GaborPoints[i].y);
			}	

		}
		else
			boSpecificPointsforGabor=false;
	}
	else
		boSpecificPointsforGabor=false;

	

	fclose(fp);
	
	return ModelSVM;
		
}

SVMClassifier **CSVMUtils::LoadIndividualSVMData(char *directory)
{
	int n,i;

	chdir(directory);

	SVMClassifier **ModelSVM=(SVMClassifier**)new char[ModelSVMGeneral->NumClasses*sizeof(SVMClassifier *)];

	for (n=0;n<(int)ModelSVMGeneral->NumClasses;n++)
		ModelSVM[n]=(SVMClassifier*)new char[sizeof(SVMClassifier)];

	//Loads n individual classifiers
	for (n=0;n<(int)ModelSVMGeneral->NumClasses;n++)
	{	

		if (boFromPCA)
		{
#ifndef ENCARA2INLINUX
			sprintf(cPath,"%s\\SVM_%s_%d",directory,ModelSVMGeneral->Label[n],NumCoefs);
#else
			sprintf(cPath,"%s/SVM_%s_%d",directory,ModelSVMGeneral->Label[n],NumCoefs);
#endif
		}
		else
		{
#ifndef ENCARA2INLINUX
			sprintf(cPath,"%sSVM%s_GaborP%d_%d",directory,ModelSVMGeneral->Label[n],nGaborPoints,nGaborFilters*2);
#else
			sprintf(cPath,"%s/SVM%s_GaborP%d_%d",directory,ModelSVMGeneral->Label[n],nGaborPoints,nGaborFilters*2);
#endif
			NumCoefs=nGaborPoints*nGaborFilters*2;
		}

		struct svm_model *model=svm_load_model(cPath);

		if (model==NULL)
		{
			for (int in=0;in<(int)ModelSVMGeneral->NumClasses;in++)
				delete [] ModelSVM[in];
			delete [] ModelSVM;
			return NULL;
		}

		ModelSVM[n]->model=model;

		//The number of classes is known, it is a binary classifier
		ModelSVM[n]->NumClasses=2;

		ModelSVM[n]->feature_max=(double *)new unsigned char[sizeof(double)*NumCoefs];
		ModelSVM[n]->feature_min=(double *)new unsigned char[sizeof(double)*NumCoefs];
		ModelSVM[n]->Label =(char**) new unsigned char[ModelSVM[n]->NumClasses*sizeof(char*)];
		ModelSVM[n]->FramesPerClass =(unsigned int*) new unsigned char[ModelSVM[n]->NumClasses*sizeof(unsigned int)];
		
		if (boFromPCA)
		{
#ifndef ENCARA2INLINUX
			sprintf(cPath,"%s\\SVMData_%s_%d.txt",directory,ModelSVMGeneral->Label[n],NumCoefs);
#else
			sprintf(cPath,"%s/SVMData_%s_%d.txt",directory,ModelSVMGeneral->Label[n],NumCoefs);
#endif
		}
		else
		{
#ifndef ENCARA2INLINUX
			sprintf(cPath,"%s\\SVMData_%s_GaborP%d_%d",directory,ModelSVMGeneral->Label[n],nGaborPoints,nGaborFilters*2);
#else
			sprintf(cPath,"%s/SVMData_%s_GaborP%d_%d",directory,ModelSVMGeneral->Label[n],nGaborPoints,nGaborFilters*2);
#endif
		}

		FILE *fp=fopen(cPath,"r");

		//time needed to compute
		fscanf(fp,"%ld\n",&ModelSVM[n]->tproc);

		//Etiquetas identidades
		for (i=0;i<(int)ModelSVM[n]->NumClasses;i++)
		{
			ModelSVM[n]->Label[i]=(char*)new unsigned char[sizeof(char)*ID_STRING_SIZE];
			fscanf(fp,"%s %d\n",ModelSVM[n]->Label[i],&(ModelSVM[n]->FramesPerClass[i]));
		}
	//Màximos y mìnimos atributos
		for (i=0;i<(int)NumCoefs;i++)
			fscanf(fp,"%lf %lf\n",&(ModelSVM[n]->feature_max[i]),&(ModelSVM[n]->feature_min[i]));


		if (fscanf(fp,"%d\n",&nGaborPoints)!=EOF)
		{
			if (nGaborPoints)
			{
				boSpecificPointsforGabor=true;
				boFromGabor=true;
				for (int i=0;i<nGaborPoints;i++)
				{
					fscanf(fp,"%d %d\n",&GaborPoints[i].x,&GaborPoints[i].y);
				}	

			}
			else
				boSpecificPointsforGabor=false;
		}
		else
			boSpecificPointsforGabor=false;

		fclose(fp);
}

	return ModelSVM;
		
}


void CSVMUtils::SaveSVMData(char *directory)
{
	int i;

	if (ModelSVMGeneral)
	{
		//model
		if (boFromPCA || boFromLBP)
		{
#ifndef ENCARA2INLINUX
			sprintf(cPath,"%s\\SVM_%d",directory,NumCoefs);
#else
			sprintf(cPath,"%s/SVM_%d",directory,NumCoefs);
#endif
		}			
		else
		{
			if (boSpecificPointsforGabor)
			{
				if (!boFromPCAGabor)
				{
					//Saves a file with the points
					FILE *fp;

					sprintf(cPath,"%sSVM_GaborPoints_%d.txt",directory,nGaborFilters*2);

					fp=fopen(cPath,"w");

					//Points available?
					if (fp)
					{
						//Number of points
						fprintf(fp,"%d\n",nGaborPoints);

						//Saves the points
						for (int i=0;i<nGaborPoints;i++)
						{
							fprintf(fp,"%d %d\n",GaborPoints[i].x,GaborPoints[i].y);
						}	

						fclose(fp);
					}


					//SVM Model filename
					sprintf(cPath,"%sSVM_GaborP_%d_%d",directory,nGaborPoints,nGaborFilters*2);

					//Includes the points in the name
					char ctmp[256];
					strcpy(ctmp,cPath);
					for (int i=0;i<nGaborPoints;i++)
					{
						sprintf(ctmp,"%s_%d_%d",cPath,GaborPoints[i].x,GaborPoints[i].y);
						strcpy(cPath,ctmp);
					}
				}
				else
				{
					sprintf(cPath,"%sSVM_PCAGaborP%d_%d",directory,nGaborPoints,nGaborFilters*2);
				}
			}
			else
			{
				sprintf(cPath,"%sSVM_Gabor_%d",directory,Gaborstep);
			}
			
		}
		
		svm_save_model(cPath,ModelSVMGeneral->model);

		//SVM data
		if (boFromPCA  || boFromLBP)
		{
#ifndef ENCARA2INLINUX
			sprintf(cPath,"%s\\SVMData_%d.txt",directory,NumCoefs);
#else
			sprintf(cPath,"%s/SVMData_%d.txt",directory,NumCoefs);
#endif
		}
		else
		{
			if (boSpecificPointsforGabor)
			{
				if (!boFromPCAGabor)
				{
#ifndef ENCARA2INLINUX
					sprintf(cPath,"%s\\SVMData_GaborP%d_%d",directory,nGaborPoints,nGaborFilters*2);
#else
					sprintf(cPath,"%s/SVMData_GaborP%d_%d",directory,nGaborPoints,nGaborFilters*2);
#endif
					
					//Includes the points in the name
					char ctmp[256];
					strcpy(ctmp,cPath);
					for (int i=0;i<nGaborPoints;i++)
					{
						sprintf(ctmp,"%s_%d_%d",cPath,GaborPoints[i].x,GaborPoints[i].y);
						if (i==nGaborPoints-1)
							sprintf(cPath,"%s.txt",ctmp);
						else
							strcpy(cPath,ctmp);
					}
				}
				else
				{		
#ifndef ENCARA2INLINUX
					sprintf(cPath,"%s\\SVMData_PCAGaborP%d_%d",directory,nGaborPoints,nGaborFilters*2);
#else
					sprintf(cPath,"%s/SVMData_PCAGaborP%d_%d",directory,nGaborPoints,nGaborFilters*2);
#endif
				}
			}
			else
			{
#ifndef ENCARA2INLINUX
				sprintf(cPath,"%s\\SVMData_Gabor_%d.txt",directory,Gaborstep);
#else
				sprintf(cPath,"%s/SVMData_Gabor_%d.txt",directory,Gaborstep);
#endif
			}
			
		}
			

		FILE *fp=fopen(cPath,"w");

		//Num of classes
		fprintf(fp,"%d\n",(int)ModelSVMGeneral->NumClasses);

		//time needed to compute
		fprintf(fp,"%ld\n",(int)ModelSVMGeneral->tproc);

		//labels and number per class
		for (i=0;i<(int)ModelSVMGeneral->NumClasses;i++)
		{
			fprintf(fp,"%s %d\n",ModelSVMGeneral->Label[i],ModelSVMGeneral->FramesPerClass[i]);
		}

		//Màximos y mìnimos atributos
		for (i=0;i<(int)NumCoefs;i++)
			fprintf(fp,"%lf %lf\n",ModelSVMGeneral->feature_max[i],ModelSVMGeneral->feature_min[i]);


		//Caso Gabor por puntos espacificados
		if (boSpecificPointsforGabor)
		{
			fprintf(fp,"%d\n",nGaborPoints);

			for (int i=0;i<nGaborPoints;i++)
			{
				fprintf(fp,"%d %d\n",GaborPoints[i].x,GaborPoints[i].y);
			}	

		}
		else
			fprintf(fp,"0\n");

		
		fclose(fp);
	}
	
}

void CSVMUtils::SaveIndividualsSVMData(char *directory)
{
	int i;

	if (ModelSVMGeneral && boSVMAuth)
	{
		//Salvo los n clasificadores individuales
		for (int n=0;n<(int)ModelSVMGeneral->NumClasses;n++)
		{
			//Salva el fichero
			
			//Fichero
			if (boFromPCA)
			{
#ifndef ENCARA2INLINUX
			sprintf(cPath,"%s\\SVM_%s_%d",directory,ModelSVMGeneral->Label[n],NumCoefs);
#else
			sprintf(cPath,"%s//SVM_%s_%d",directory,ModelSVMGeneral->Label[n],NumCoefs);
#endif
			}
			else
			{
				sprintf(cPath,"%sSVM%s_GaborP%d_%d",directory,ModelSVMGeneral->Label[n],nGaborPoints,nGaborFilters*2);
			}
		
			svm_save_model(cPath,ModelSVMIndividuals[n]->model);

			if (boFromPCA)
			{
#ifndef ENCARA2INLINUX
			sprintf(cPath,"%s\\SVMData_%s_%d.txt",directory,ModelSVMGeneral->Label[n],NumCoefs);
#else
			sprintf(cPath,"%s/SVMData_%s_%d.txt",directory,ModelSVMGeneral->Label[n],NumCoefs);
#endif
			}
			else
			{
#ifndef ENCARA2INLINUX
				sprintf(cPath,"%s\\SVMData_%s_GaborP%d_%d",directory,ModelSVMGeneral->Label[n],nGaborPoints,nGaborFilters*2);
#else
				sprintf(cPath,"%s/SVMData_%s_GaborP%d_%d",directory,ModelSVMGeneral->Label[n],nGaborPoints,nGaborFilters*2);
#endif
			}

			FILE *fp=fopen(cPath,"w");

			//The number of classes is not necessary, it is a binary classifier (2 classes)

			//time needed to compute
			fprintf(fp,"%ld\n",(int)ModelSVMIndividuals[n]->tproc);

			//Labels
			for (i=0;i<(int)ModelSVMIndividuals[n]->NumClasses;i++)
				fprintf(fp,"%s %d\n",ModelSVMIndividuals[n]->Label[i],ModelSVMIndividuals[n]->FramesPerClass[i]);
			//Màximos y mìnimos atributos
			for (i=0;i<(int)NumCoefs;i++)
				fprintf(fp,"%lf %lf\n",ModelSVMIndividuals[n]->feature_max[i],ModelSVMIndividuals[n]->feature_min[i]);

			//Caso Gabor por puntos espacificados
			if (boSpecificPointsforGabor)
			{
				fprintf(fp,"%d\n",nGaborPoints);

				for (int i=0;i<nGaborPoints;i++)
				{
					fprintf(fp,"%d %d\n",GaborPoints[i].x,GaborPoints[i].y);
				}	

			}
			else
				fprintf(fp,"0\n");

			fclose(fp);
		}
	}

}

//Compute

// ComputeSVM: Given SVM data computes SVM classifier
SVMClassifier * CSVMUtils::ComputeSVMfromPCA()
{
const char *error_msg;

	//If there is a dataset
	if (Dataset)
	{
		if (Dataset->NumClasses>1)
		{
			//Allocs memory
			SVMClassifier *ModelSVM=(SVMClassifier*)new unsigned char[sizeof(SVMClassifier)];

			int elements;

			//Configuration values
			// default values
			ModelSVM->param.svm_type =C_SVC;//NU_SVC;//NU_SVC;//ONE_CLASS???//era C_SVC 
			ModelSVM->param.kernel_type = RBF;
			ModelSVM->param.degree = 3;
			ModelSVM->param.gamma = 1.0/(float)NumCoefs;
			ModelSVM->param.coef0 = 0;
			ModelSVM->param.nu = 0.5;
			ModelSVM->param.cache_size = 200;
			ModelSVM->param.C = 50;//los experimentos recientes con 50000//500;//100;//1; //Aumento el coste //500 utilizado habitualmente
			ModelSVM->param.eps = 1e-3;
			ModelSVM->param.p = 0.1;
			ModelSVM->param.shrinking = 1;
			ModelSVM->param.nr_weight = 0;
			ModelSVM->param.probability = 0;
			ModelSVM->param.weight_label = NULL;
			ModelSVM->param.weight = NULL;


			//Problem definition (toma los datos de PCA)
			//Ver http://www.csie.ntu.edu.tw/~cjlin/libsvm/
			//Nùmero de muestras de entrenamiento, supongo que cada clase tiene el mismo nùmero???
			if (boSomesamplesnotintraining)
				ModelSVM->prob.l=nInTraining;
			else
				ModelSVM->prob.l=Dataset->TotalImages;

			elements=(NumCoefs+1)*ModelSVM->prob.l; //Para cada muestra tiene los coeficientes

			ModelSVM->prob.y = (double *)new unsigned char[sizeof(double)*ModelSVM->prob.l];
			ModelSVM->prob.x = (struct svm_node **)new unsigned char[sizeof(struct svm_node *)*ModelSVM->prob.l];
			ModelSVM->x_space = (struct svm_node *)new unsigned char[sizeof(struct svm_node)*elements];
			ModelSVM->Label =(char**) new unsigned char[Dataset->NumClasses*sizeof(char*)];
			ModelSVM->FramesPerClass =(unsigned int*) new unsigned char[Dataset->NumClasses*sizeof(unsigned int)];
			ModelSVM->feature_max=(double *)new unsigned char[sizeof(double)*NumCoefs];
			ModelSVM->feature_min=(double *)new unsigned char[sizeof(double)*NumCoefs];

			//Training samples with their label
			int j=0,i;
			int clase=1;
			int acum=Dataset->FramesPerClass[clase-1];
			ModelSVM->FramesPerClass[clase-1]=Dataset->FramesPerClass[clase-1];
			ModelSVM->Label[clase-1]=(char*)new unsigned char[sizeof(char)*ID_STRING_SIZE];
			strcpy(ModelSVM->Label[clase-1],Dataset->Label[0]);

			Projections=new CDatasetProjections();
			Projections->ComputeProjections(Dataset,PCAspace);

			
			//max and min initialization, used for later scaling
			for(i=0;i<(int)NumCoefs;i++)
			{
				ModelSVM->feature_max[i]=-1e20;
				ModelSVM->feature_min[i]=1e20;
			}
			
			//for (i=0;i<ModelSVM->prob.l;i++)
			i=0;
			for (unsigned int ns=0;ns<Dataset->TotalImages;ns++)
			{
				//Etiqueta del objetivo
				//Assigns a number to each class, depending of the samples for each one
				if (ns>= (unsigned int)acum)
				{
					clase++;
					acum+=Dataset->FramesPerClass[clase-1];
					ModelSVM->FramesPerClass[clase-1]=Dataset->FramesPerClass[clase-1];

					ModelSVM->Label[clase-1]=(char*)new unsigned char[sizeof(char)*ID_STRING_SIZE];
					strcpy(ModelSVM->Label[clase-1],Dataset->Label[ns]);
				}

				bool usesample=true;

				//Must the sample be used
				if (boSomesamplesnotintraining)
					if (!SamplesInTraining[ns])
						usesample=false;

				if (usesample)
				{
					ModelSVM->prob.y[i]=(double)clase;

					//Sample coefficients
					ModelSVM->prob.x[i] = &(ModelSVM->x_space[j]);
				

					//Copiamos las proyecciones
					for (int n=0;n<(int)NumCoefs;n++)
					{
						double val=Projections->P[ns][n];

						ModelSVM->x_space[j].index=n+1;
						//tomo cada componente del espacio PCA como un atributo
						ModelSVM->x_space[j].value=val;
						
						//Actualizo màximos y mìnimos
						if (val>ModelSVM->feature_max[n])
							ModelSVM->feature_max[n]=val;
						if (val<ModelSVM->feature_min[n])
							ModelSVM->feature_min[n]=val;


						j++;
					}

					//Añadimos el ùltimo
					ModelSVM->x_space[j++].index=-1;

					//New sample added to the training
					i++;
				}				
			}

			//Recorro ajustando el rango a 1,-1
			double upper=1,lower=-1;
			for (i=0;i<elements;i++)
			{
				if (ModelSVM->x_space[i].index!=-1)
				{
					int index=ModelSVM->x_space[i].index;
					ModelSVM->x_space[i].value=lower+(upper-lower)*(ModelSVM->x_space[i].value-ModelSVM->feature_min[index-1])/(ModelSVM->feature_max[index-1]-ModelSVM->feature_min[index-1]);
				}
			}

			ModelSVM->NumClasses=clase;
			//Vector de votos por clase
			ModelSVM->voting=(int *)new char[sizeof(int)*(ModelSVM->NumClasses+1)];


			error_msg = svm_check_parameter(&ModelSVM->prob,&ModelSVM->param);

			//Entrenamiento
			clock_t	tini=clock();


			ModelSVM->model = svm_train(&ModelSVM->prob,&ModelSVM->param);


			ModelSVM->tproc=clock()-tini;


			boSVMComputed=true;

			return ModelSVM;
		}
		else
			return NULL;
	}
	else
		return NULL;


}


SVMClassifier * CSVMUtils::ComputeSVMfromLBP()
{
	const char *error_msg;

	//If there is a dataset
	if (Dataset)
	{
		if (Dataset->NumClasses>1)
		{
			//Allocs memory
			SVMClassifier *ModelSVM=(SVMClassifier*)new unsigned char[sizeof(SVMClassifier)];		

			//NumCoefs depends on the LBP code used, the image size and window dimensions
			int ncomps;
			switch (LBPcode)
			{
				case LBP_UNIFORM:
					ncomps=10;
					break;
				case LBP_SIMPLIFIED_COMPACT:
					ncomps=9;
					break;
				case LBP_ORIGINAL:
				case LBP_SIMPLIFIED:
				default:
					ncomps=256;
					break;
			}
			int hhor=Dataset->BC[0]->width/LBPdim;
			int hver=Dataset->BC[0]->height/LBPdim;
			//Total number of coefficients for training
			NumCoefs=hhor*hver*ncomps;

			
			int elements;

			//Configuration values
			// default values
			ModelSVM->param.svm_type = C_SVC;//NU_SVC;//
			ModelSVM->param.kernel_type = RBF;
			ModelSVM->param.degree = 3;
			ModelSVM->param.gamma = 1.0/(float)NumCoefs;
			ModelSVM->param.coef0 = 0;
			ModelSVM->param.nu = 0.5;
			ModelSVM->param.cache_size = 200;
			ModelSVM->param.C = 50000;//500;//100;//1; //Aumento el coste //500 utilizado habitualmente
			ModelSVM->param.eps = 1e-3;
			ModelSVM->param.p = 0.1;
			ModelSVM->param.shrinking = 1;
			ModelSVM->param.nr_weight = 0;
			ModelSVM->param.probability = 0;
			ModelSVM->param.weight_label = NULL;
			ModelSVM->param.weight = NULL;


			//Problem definition (toma los datos de PCA)
			//Ver http://www.csie.ntu.edu.tw/~cjlin/libsvm/
			//Nùmero de muestras de entrenamiento, supongo que cada clase tiene el mismo nùmero???
			//Nùmero de muestras de entrenamiento, supongo que cada clase tiene el mismo nùmero???
			if (boSomesamplesnotintraining)
				ModelSVM->prob.l=nInTraining;
			else
				ModelSVM->prob.l=Dataset->TotalImages;

			elements=(NumCoefs+1)*ModelSVM->prob.l; //Para cada muestra tiene los coeficientes

			ModelSVM->prob.y = (double *)new unsigned char[sizeof(double)*ModelSVM->prob.l];
			ModelSVM->prob.x = (struct svm_node **)new unsigned char[sizeof(struct svm_node *)*ModelSVM->prob.l];
			ModelSVM->x_space = (struct svm_node *)new unsigned char[sizeof(struct svm_node)*elements];
			ModelSVM->Label =(char**) new unsigned char[Dataset->NumClasses*sizeof(char*)];
			ModelSVM->FramesPerClass =(unsigned int*) new unsigned char[Dataset->NumClasses*sizeof(unsigned int)];
			ModelSVM->feature_max=(double *)new unsigned char[sizeof(double)*NumCoefs];
			ModelSVM->feature_min=(double *)new unsigned char[sizeof(double)*NumCoefs];

			//Training samples with their label
			int j=0,i;
			int clase=1;
			int acum=Dataset->FramesPerClass[clase-1];
			ModelSVM->FramesPerClass[clase-1]=Dataset->FramesPerClass[clase-1];
			ModelSVM->Label[clase-1]=(char*)new unsigned char[sizeof(char)*ID_STRING_SIZE];
			strcpy(ModelSVM->Label[clase-1],Dataset->Label[0]);

			//Y si ya vienen dadas las imágenes LBP ¿¿
			//Gabor images computation is necessary if they are not contained in the dataset
			if (Dataset->BCLBP==NULL)
			{
				CLBP lbp;
			
				LBP=(LBPImages*)new unsigned char[sizeof(LBPImages)];
				LBP->TotalImages=Dataset->TotalImages;
				LBP->I=(IplImage**) new unsigned char[LBP->TotalImages*sizeof(IplImage*)];
			
				for (i=0;i<(int)LBP->TotalImages;i++)
				{			
					//Computes LBP Image
					LBP->I[i]=lbp.GetLBPImage(Dataset->BC[i],0,LBPcode);					
				}
			}

			//max and min initialization, used for later scaling
			for(i=0;i<(int)NumCoefs;i++)
			{
				ModelSVM->feature_max[i]=-1e20;
				ModelSVM->feature_min[i]=1e20;
			}
			
			//Para cada imagen dato introducimos sus datos en la estructura de datos de libsvm
			//for (i=0;i<ModelSVM->prob.l;i++)
			i=0;
			for (unsigned int ns=0;ns<Dataset->TotalImages;ns++)
			{
				//Etiqueta del objetivo
				//Assigns a number to each class, depending of the samples for each one
				if (ns>=(unsigned int) acum)
				{
					clase++;
					acum+=Dataset->FramesPerClass[clase-1];
					ModelSVM->FramesPerClass[clase-1]=Dataset->FramesPerClass[clase-1];

					ModelSVM->Label[clase-1]=(char*)new unsigned char[sizeof(char)*ID_STRING_SIZE];
					strcpy(ModelSVM->Label[clase-1],Dataset->Label[ns]);
				}

				bool usesample=true;

				//Must the sample be used
				if (boSomesamplesnotintraining)
					if (!SamplesInTraining[ns])
						usesample=false;

				if (usesample)
				{

					ModelSVM->prob.y[i]=(double)clase;

					//Sample coefficients
					ModelSVM->prob.x[i] = &(ModelSVM->x_space[j]);			

					//Calculamos el histograma concatenado, y lo almacenamos
					//en la estructura de la biblioteca SVM
					CLBP lbp;

					//Obtenemos el histograma concatenado de la imagen
					float *Hist;
					int ndims;

					//Hay que ir guardando los sucesivos histogramas o el compendio de histogramas para cada imagen
					lbp.GetLBPHistogramFromLBPGridf(LBP->I[ns],LBPdim,&Hist,&ndims,LBPcode);
					
					float *pf=(float *)Hist;
					for (int p=0;p<NumCoefs;p++,pf++)
					{
						double val=(double) *pf;

						ModelSVM->x_space[j].index=p+1;
						//tomo cada componente del espacio PCA como un atributo
						ModelSVM->x_space[j].value=val;
						
						//Actualizo màximos y mìnimos para cada componente
						if (val>ModelSVM->feature_max[p])
							ModelSVM->feature_max[p]=val;
						if (val<ModelSVM->feature_min[p])
							ModelSVM->feature_min[p]=val;

						j++;
					}				

					//Añadimos el ùltimo
					ModelSVM->x_space[j++].index=-1;	

				//New sample added to the training
					i++;
				}

			}

			//Recorro ajustando el rango a 1,-1
			double upper=1,lower=-1;
			for (i=0;i<elements;i++)
			{
				if (ModelSVM->x_space[i].index!=-1)
				{
					int index=ModelSVM->x_space[i].index;
					ModelSVM->x_space[i].value=lower+(upper-lower)*(ModelSVM->x_space[i].value-ModelSVM->feature_min[index-1])/(ModelSVM->feature_max[index-1]-ModelSVM->feature_min[index-1]);
                 }
			}

			ModelSVM->NumClasses=clase;
			//Vector de votos por clase
			ModelSVM->voting=(int *)new char[sizeof(int)*(ModelSVM->NumClasses+1)];

			error_msg = svm_check_parameter(&ModelSVM->prob,&ModelSVM->param);

			//Entrenamiento
			clock_t	tini=clock();

			//Entreno
			ModelSVM->model = svm_train(&ModelSVM->prob,&ModelSVM->param);

			ModelSVM->tproc=clock()-tini;


			boSVMComputed=true;

			return ModelSVM;
		}
		else
			return NULL;
	}
	else
		return NULL;

}



SVMClassifier * CSVMUtils::ComputeSVMfromGabor()
{

	//Separar caso de simple entreno y caso de selección de combinación
	SVMClassifier *Clasifmax=NULL;
	CvPoint		*Gabormax=NULL;

	if (boSearchBestGaborConfig)
	{	
		//Inicializa combinaciones probadas (útil si no haces un recorrido fijo sino aleatorio)
		IplImage *imgGaborProbadas=cvCreateImage(cvSize(Dataset->BCGabor[0]->width,Dataset->BCGabor[0]->height),8,1);

		

		float rate,maxrate=0.f;
		int nGaborPointsmax=0,nGaborPointsaProbar=0;
		int pgxmax,pgymax;
		int comb=0;
		bool bohamejorado;
		int combinacionesrestantes;
			
		//Busca un clasificador que permita clasificar correctamente todo el conjunto de entrenamiento
		do
		{//Busca para un máximo de puntos de Gabor si no consigue 100% con el entreno
			bohamejorado=false; //Sigue buscando más puntos a añadir si mejora
			nGaborPointsaProbar++;//Aumentamos el número de puntos de Gabor a considerar
			combinacionesrestantes=Dataset->BCGabor[0]->width*Dataset->BCGabor[0]->height;

			comb=0;//Empezamos en esta combinación (luego hacerlo aleatorio)

			//Todos los puntos son válidos
			cvSetZero(imgGaborProbadas);

			//Borra el máximo de las etapas previas
			for (int npg=0;npg<nGaborPointsaProbar-1;npg++)
			{
				cvSet2D(imgGaborProbadas,Gabormax[npg].y,Gabormax[npg].x,cvScalar(1));
				combinacionesrestantes--;//Una combinación menos a probar
			}
			

			do
			{
				//Escoger combinación
				bool boCombinacion=false;
				int pgx,pgy,tgx,tgy;

				//Busca una combinación libre
				do
				{
					//En lugar de incrementar comb, buscar un aleatorio con máximo combinaciones restantes, y luego
					//recorrer contando libres hasta llegar a la cuenta del aleatorio obtenido

					//¿Es una cobinación libre?
					tgx=comb%Dataset->BCGabor[0]->width;
					tgy=comb/Dataset->BCGabor[0]->width;

					CvScalar cvsc=cvGet2D(imgGaborProbadas,tgy,tgx);
					
					//Contiene un cero?
					if (cvsc.val[0]==0)
					{
						boCombinacion=true;
						pgx=tgx;
						pgy=tgy;
						cvSet2D(imgGaborProbadas,tgy,tgx,cvScalar(1));
						combinacionesrestantes--;//Una combinación menos a probar
					}
					comb++;//De momento no hago algo aleatorio

				}while(!boCombinacion && comb<Dataset->BCGabor[0]->width*Dataset->BCGabor[0]->height);

				if (boCombinacion)
				{
					//Configurar 
					nGaborPoints=nGaborPointsaProbar;
					
					GaborPoints=(CvPoint	*)new unsigned char[sizeof(CvPoint)*nGaborPoints];

					//Si hay puntos previos los mete
					for (int npg=0;npg<nGaborPointsaProbar-1;npg++)
					{
						GaborPoints[npg].x=Gabormax[npg].x;
						GaborPoints[npg].y=Gabormax[npg].y;
					}

					GaborPoints[nGaborPointsaProbar-1].x=pgx;
					GaborPoints[nGaborPointsaProbar-1].y=pgy;
					boSpecificPointsAvailable=true;

					//Entrenar
					ModelSVMGeneral=TrainSVMfromGabor();				

					//Comprobar rendimiento for the current training set
					ClassificationRate *Rates=TestClassifier(Dataset);

					rate=0;

					//Gets the total rate
					int total=0;
					for (unsigned int nclas=0;nclas<Dataset->NumClasses;nclas++)
					{
						rate+=Rates->Corrects[nclas];
						total+=Rates->iNsamples[nclas];
					}
					rate=((float)rate)/((float)total);

					//Depurando
					if (rate<1.f)
						rate=rate;

					delete Rates;

					if (rate>maxrate)
					{
						bohamejorado=true;

						//Si hay uno nuevo mejor 
						if (Clasifmax)
							delete Clasifmax;

						if (Gabormax)
							delete Gabormax;

						Gabormax=GaborPoints;

						nGaborPointsmax=nGaborPoints;

						Clasifmax=ModelSVMGeneral;

						maxrate=rate;
						pgxmax=pgx;
						pgymax=pgy;
					}
					else
					{
						delete ModelSVMGeneral;
						delete GaborPoints;
					}

				}
				
			//Si no clasifica a todos, seguir probando otra combinación
			}while(maxrate<1.f && comb<Dataset->BCGabor[0]->width*Dataset->BCGabor[0]->height && combinacionesrestantes);

		}while (maxrate<1.f && bohamejorado && nGaborPoints<3);//De momento me conformo con un punto de Gabor

       	cvReleaseImage(&imgGaborProbadas);

		//Tomo el mjeor encontrado
		GaborPoints=Gabormax;

		nGaborPoints=nGaborPointsmax;

		//retorna el mejor
		return Clasifmax;

	}
	else//Ya vienen los puntos dados
	{
		//Entrenar
		return TrainSVMfromGabor();
	}
}


SVMClassifier * CSVMUtils::TrainSVMfromGabor()
{

const char *error_msg;

	//If there is a dataset
	if (Dataset)
	{
		if (Dataset->NumClasses>1)
		{
			//Allocs memory
			SVMClassifier *ModelSVM=(SVMClassifier*)new unsigned char[sizeof(SVMClassifier)];		

			//NumCoefs depends on the number of points considering 32 (16) channels
			if (boSpecificPointsforGabor)				
				NumCoefs=nGaborPoints*nGaborFilters*2;
			else
				NumCoefs=((Dataset->BC[0]->width-18)/Gaborstep +1)*((Dataset->BC[0]->height-9)/Gaborstep +1)*nGaborFilters*2;

			int elements;

			//Configuration values
			// default values
			ModelSVM->param.svm_type = C_SVC;//NU_SVC;//
			ModelSVM->param.kernel_type = RBF;
			ModelSVM->param.degree = 3;
			ModelSVM->param.gamma = 1.0/(float)NumCoefs;
			ModelSVM->param.coef0 = 0;
			ModelSVM->param.nu = 0.5;
			ModelSVM->param.cache_size = 200;
			ModelSVM->param.C = 50000;//500;//100;//1; //Aumento el coste //500 utilizado habitualmente
			ModelSVM->param.eps = 1e-3;
			ModelSVM->param.p = 0.1;
			ModelSVM->param.shrinking = 1;
			ModelSVM->param.nr_weight = 0;
			ModelSVM->param.probability = 0;
			ModelSVM->param.weight_label = NULL;
			ModelSVM->param.weight = NULL;


			//Problem definition (toma los datos de PCA)
			//Ver http://www.csie.ntu.edu.tw/~cjlin/libsvm/
			//Nùmero de muestras de entrenamiento, supongo que cada clase tiene el mismo nùmero???
			ModelSVM->prob.l=Dataset->TotalImages;
			elements=(NumCoefs+1)*ModelSVM->prob.l; //Para cada muestra tiene los coeficientes

			ModelSVM->prob.y = (double *)new unsigned char[sizeof(double)*ModelSVM->prob.l];
			ModelSVM->prob.x = (struct svm_node **)new unsigned char[sizeof(struct svm_node *)*ModelSVM->prob.l];
			ModelSVM->x_space = (struct svm_node *)new unsigned char[sizeof(struct svm_node)*elements];
			ModelSVM->Label =(char**) new unsigned char[Dataset->NumClasses*sizeof(char*)];
			ModelSVM->FramesPerClass =(unsigned int*) new unsigned char[Dataset->NumClasses*sizeof(unsigned int)];
			ModelSVM->feature_max=(double *)new unsigned char[sizeof(double)*NumCoefs];
			ModelSVM->feature_min=(double *)new unsigned char[sizeof(double)*NumCoefs];

			//Training samples with their label
			int j=0,i,nc;
			int clase=1;
			int acum=Dataset->FramesPerClass[clase-1];
			ModelSVM->FramesPerClass[clase-1]=Dataset->FramesPerClass[clase-1];
			ModelSVM->Label[clase-1]=(char*)new unsigned char[sizeof(char)*ID_STRING_SIZE];
			strcpy(ModelSVM->Label[clase-1],Dataset->Label[0]);

			//Gabor images computation is necessary if they are not contained in the dataset
			if (Dataset->BCGabor==NULL)
			{
				GI=(GaborImages*)new unsigned char[sizeof(GaborImages)];
				GI->NumCoefs=NumCoefs;
				GI->TotalImages=Dataset->TotalImages;
				GI->G=(IplImage**) new unsigned char[GI->TotalImages*sizeof(IplImage*)];
			
				for (i=0;i<(int)GI->TotalImages;i++)
				{			
					//Computes Gabor Image
					if (boSpecificPointsforGabor)
						GI->G[i]=GaborUtils.GaborWavelet(Dataset->BC[i],GaborPoints,nGaborPoints,nGaborFilters*2);
					else
						GI->G[i]=GaborUtils.GaborWavelet(Dataset->BC[i],Gaborstep,nGaborFilters*2);
				}
			}

			//max and min initialization, used for later scaling
			for(i=0;i<(int)NumCoefs;i++)
			{
				ModelSVM->feature_max[i]=-1e20;
				ModelSVM->feature_min[i]=1e20;
			}
			
			//Para cada imagen dato introducimos sus datos en la estructura de datos de libsvm
			for (i=0;i<ModelSVM->prob.l;i++)
			{
				//Etiqueta del objetivo
				//Assigns a number to each class, depending of the samples for each one
				if (i>=acum)
				{
					clase++;
					acum+=Dataset->FramesPerClass[clase-1];
					ModelSVM->FramesPerClass[clase-1]=Dataset->FramesPerClass[clase-1];

					ModelSVM->Label[clase-1]=(char*)new unsigned char[sizeof(char)*ID_STRING_SIZE];
					strcpy(ModelSVM->Label[clase-1],Dataset->Label[i]);
				}
				ModelSVM->prob.y[i]=(double)clase;

				//Sample coefficients
				ModelSVM->prob.x[i] = &(ModelSVM->x_space[j]);			


				//Copiamos la imagen Gabor en la estructura de la biblioteca SVM
				if (Dataset->BCGabor==NULL)
				{
					BYTE *p=(BYTE *)GI->G[i]->imageData;
					float *pf;

					nc=0;

					//Para el ancho y alto de la imagen y canal (las componentes a clasificar)
					for (int n=0;n<GI->G[i]->height;n++,p+=GI->G[i]->widthStep)
					{
						pf=(float *)p;
						for (int m=0;m<GI->G[i]->width;m++)
						{
							//Cada canal
							for (int p=0;p<nGaborFilters*2;p++,pf++)
							{
								double val=(double) *pf;

								ModelSVM->x_space[j].index=nc+1;
								//tomo cada componente del espacio PCA como un atributo
								ModelSVM->x_space[j].value=val;
								
								//Actualizo màximos y mìnimos para cada componente
								if (val>ModelSVM->feature_max[nc])
									ModelSVM->feature_max[nc]=val;
								if (val<ModelSVM->feature_min[nc])
									ModelSVM->feature_min[nc]=val;

								j++;
								nc++;
							}

						}
					}
				}
				else//The Gabor images are included in Dataset
				{
					nc=0;
					for (int n=0;n<nGaborPoints;n++)
					{
						BYTE *p=(BYTE *)Dataset->BCGabor[i]->imageData+GaborPoints[n].y*Dataset->BCGabor[i]->widthStep+GaborPoints[n].x*nGaborFilters*2*sizeof(float);
						float *pf;

						pf=(float *)p;

						//Cada canal
						for (int pu=0;pu<nGaborFilters*2;pu++,pf++)
						{
							double val=(double) *pf;

							ModelSVM->x_space[j].index=nc+1;
							//tomo cada componente del espacio PCA como un atributo
							ModelSVM->x_space[j].value=val;
							
							//Actualizo màximos y mìnimos para cada componente
							if (val>ModelSVM->feature_max[nc])
								ModelSVM->feature_max[nc]=val;
							if (val<ModelSVM->feature_min[nc])
								ModelSVM->feature_min[nc]=val;

							j++;
							nc++;
						}
					}
				}

				//Añadimos el ùltimo
				ModelSVM->x_space[j++].index=-1;
				
			}

			//Recorro ajustando el rango a 1,-1
			double upper=1,lower=-1;
			for (i=0;i<elements;i++)
			{
				if (ModelSVM->x_space[i].index!=-1)
				{
					int index=ModelSVM->x_space[i].index;
					ModelSVM->x_space[i].value=lower+(upper-lower)*(ModelSVM->x_space[i].value-ModelSVM->feature_min[index-1])/(ModelSVM->feature_max[index-1]-ModelSVM->feature_min[index-1]);

				}
			}

			ModelSVM->NumClasses=clase;
			//Vector de votos por clase
			ModelSVM->voting=(int *)new char[sizeof(int)*(ModelSVM->NumClasses+1)];

			error_msg = svm_check_parameter(&ModelSVM->prob,&ModelSVM->param);

			//Entrenamiento
			clock_t	tini=clock();

			//Entreno
			ModelSVM->model = svm_train(&ModelSVM->prob,&ModelSVM->param);

			ModelSVM->tproc=clock()-tini;


			boSVMComputed=true;

			return ModelSVM;
		}
		else
			return NULL;
	}
	else
		return NULL;


}


SVMClassifier * CSVMUtils::ComputeSVMfromPCAGabor()
{
	int n;

const char *error_msg;

	//If there is a dataset
	if (Dataset)
	{
		if (Dataset->NumClasses>1)
		{
			//Allocs memory
			SVMClassifier *ModelSVM=(SVMClassifier*)new unsigned char[sizeof(SVMClassifier)];		

			if (boSpecificPointsforGabor)
			{
				if (!boFromPCAGabor)
					//NumCoefs depends on the number of points considering 32 (16) channels
					NumCoefs=nGaborPoints*nGaborFilters*2;
				else//boFromPCAGabor
				{
					NumCoefsG=nGaborPoints*nGaborFilters*2;
					NumCoefs=NumCoefsG+NumCoefsPCA;
				}
			}
			else
				NumCoefs=((Dataset->BC[0]->width-18)/Gaborstep +1)*((Dataset->BC[0]->height-9)/Gaborstep +1)*nGaborFilters*2;

			int elements;

			//Configuration values
			// default values
			ModelSVM->param.svm_type = C_SVC;//NU_SVC;//
			ModelSVM->param.kernel_type = RBF;
			ModelSVM->param.degree = 3;
			ModelSVM->param.gamma = 1.0/(float)NumCoefs;
			ModelSVM->param.coef0 = 0;
			ModelSVM->param.nu = 0.5;
			ModelSVM->param.cache_size = 200;
			ModelSVM->param.C = 50000;//500;//100;//1; //Aumento el coste //500 utilizado habitualmente
			ModelSVM->param.eps = 1e-3;
			ModelSVM->param.p = 0.1;
			ModelSVM->param.shrinking = 1;
			ModelSVM->param.nr_weight = 0;
			ModelSVM->param.probability = 0;
			ModelSVM->param.weight_label = NULL;
			ModelSVM->param.weight = NULL;


			//Problem definition (toma los datos de PCA)
			//Ver http://www.csie.ntu.edu.tw/~cjlin/libsvm/
			//Nùmero de muestras de entrenamiento, supongo que cada clase tiene el mismo nùmero???
			ModelSVM->prob.l=Dataset->TotalImages;
			elements=(NumCoefs+1)*ModelSVM->prob.l; //Para cada muestra tiene los coeficientes

			ModelSVM->prob.y = (double *)new unsigned char[sizeof(double)*ModelSVM->prob.l];
			ModelSVM->prob.x = (struct svm_node **)new unsigned char[sizeof(struct svm_node *)*ModelSVM->prob.l];
			ModelSVM->x_space = (struct svm_node *)new unsigned char[sizeof(struct svm_node)*elements];
			ModelSVM->Label =(char**) new unsigned char[Dataset->NumClasses*sizeof(char*)];
			ModelSVM->FramesPerClass =(unsigned int*) new unsigned char[Dataset->NumClasses*sizeof(unsigned int)];
			ModelSVM->feature_max=(double *)new unsigned char[sizeof(double)*NumCoefs];
			ModelSVM->feature_min=(double *)new unsigned char[sizeof(double)*NumCoefs];

			//Training samples with their label
			int j,i,nc;
			int clase=1;
			int acum=Dataset->FramesPerClass[clase-1];
			ModelSVM->FramesPerClass[clase-1]=Dataset->FramesPerClass[clase-1];
			ModelSVM->Label[clase-1]=(char*)new unsigned char[sizeof(char)*ID_STRING_SIZE];
			strcpy(ModelSVM->Label[clase-1],Dataset->Label[0]);

			//PCA Projections
			//Projections
			Projections=new CDatasetProjections();
			Projections->ComputeProjections(Dataset,PCAspace);

			//Gabor images
			GI=(GaborImages*)new unsigned char[sizeof(GaborImages)];
			GI->NumCoefs=NumCoefsG;
			GI->TotalImages=Dataset->TotalImages;
			GI->G=(IplImage**) new unsigned char[GI->TotalImages*sizeof(IplImage*)];
		
			for (i=0;i<(int)GI->TotalImages;i++)
			{			
				//Computes Gabor Image
				if (boSpecificPointsforGabor)
					GI->G[i]=GaborUtils.GaborWavelet(Dataset->BC[i],GaborPoints,nGaborPoints,nGaborFilters*2);
				else
					GI->G[i]=GaborUtils.GaborWavelet(Dataset->BC[i],Gaborstep,nGaborFilters*2);
			}


			//max and min initialization, used for later scaling
			for(i=0;i<(int)NumCoefs;i++)
			{
				ModelSVM->feature_max[i]=-1e20;
				ModelSVM->feature_min[i]=1e20;
			}
			
			j=0;

			//Para cada imagen dato introducimos sus datos en la estructura de datos de libsvm
			for (i=0;i<ModelSVM->prob.l;i++)
			{
				//Etiqueta del objetivo
				//Assigns a number to each class, depending of the samples for each one
				if (i>=acum)
				{
					clase++;
					acum+=Dataset->FramesPerClass[clase-1];
					ModelSVM->FramesPerClass[clase-1]=Dataset->FramesPerClass[clase-1];

					ModelSVM->Label[clase-1]=(char*)new unsigned char[sizeof(char)*ID_STRING_SIZE];
					strcpy(ModelSVM->Label[clase-1],Dataset->Label[i]);
				}
				ModelSVM->prob.y[i]=(double)clase;

				//Sample coefficients
				ModelSVM->prob.x[i] = &(ModelSVM->x_space[j]);	

				//Coef
				nc=0;

				//PCA projection
				//Copiamos las proyecciones
				for (n=0;n<(int)NumCoefsPCA;n++)
				{
					double val=Projections->P[i][n];

					ModelSVM->x_space[j].index=nc+1;
					//tomo cada componente del espacio PCA como un atributo
					ModelSVM->x_space[j].value=val;
					
					//Actualizo màximos y mìnimos
					if (val>ModelSVM->feature_max[nc])
						ModelSVM->feature_max[nc]=val;
					if (val<ModelSVM->feature_min[nc])
						ModelSVM->feature_min[nc]=val;

					j++;
					nc++;
				}

				//GABOR
				//Copiamos la imagen Gabor en la estructura de la biblioteca SVM
				BYTE *p=(BYTE *)GI->G[i]->imageData;
				float *pf;

				//Para el ancho y alto de la imagen y canal (las componentes a clasificar)
				for (n=0;n<GI->G[i]->height;n++,p+=GI->G[i]->widthStep)
				{
					pf=(float *)p;
					for (int m=0;m<GI->G[i]->width;m++)
					{
						//Cada canal
						for (int p=0;p<nGaborFilters*2;p++,pf++)
						{
							double val=(double) *pf;

							ModelSVM->x_space[j].index=nc+1;
							//tomo cada componente del espacio PCA como un atributo
							ModelSVM->x_space[j].value=val;
							
							//Actualizo màximos y mìnimos para cada componente
							if (val>ModelSVM->feature_max[nc])
								ModelSVM->feature_max[nc]=val;
							if (val<ModelSVM->feature_min[nc])
								ModelSVM->feature_min[nc]=val;

							j++;
							nc++;
						}

					}
				}
				

				//Anhadimos el ùltimo
				ModelSVM->x_space[j++].index=-1;
				
			}

			//Recorro ajustando el rango a 1,-1
			double upper=1,lower=-1;
			for (i=0;i<elements;i++)
			{
				if (ModelSVM->x_space[i].index!=-1)
				{
					int index=ModelSVM->x_space[i].index;
					ModelSVM->x_space[i].value=lower+(upper-lower)*(ModelSVM->x_space[i].value-ModelSVM->feature_min[index-1])/(ModelSVM->feature_max[index-1]-ModelSVM->feature_min[index-1]);

				}
			}

			ModelSVM->NumClasses=clase;
			//Vector de votos por clase
			ModelSVM->voting=(int *)new char[sizeof(int)*(ModelSVM->NumClasses+1)];

			error_msg = svm_check_parameter(&ModelSVM->prob,&ModelSVM->param);

			//Entrenamiento
			clock_t	tini=clock();

			//Entreno
			ModelSVM->model = svm_train(&ModelSVM->prob,&ModelSVM->param);

			ModelSVM->tproc=clock()-tini;


			boSVMComputed=true;

			return ModelSVM;
		}
		else
			return NULL;
	}
	else
		return NULL;


}

// GenerarDatosPCA: Realiza a partir de los datos PCA, un clasificador SVM
SVMClassifier ** CSVMUtils::ComputeSVMIndividual()
{

	const char *error_msg;
	int n;
	SVMClassifier **ModelSVM=NULL;

	
	//Creo los n clasificadores individuales, pero sólo si existe el n clases
	if (ModelSVMGeneral)
	{		
		if (ModelSVMGeneral->NumClasses>1)
		{
			ModelSVM=(SVMClassifier**)new char[ModelSVMGeneral->NumClasses*sizeof(SVMClassifier *)];

			for (n=0;n<(int)ModelSVMGeneral->NumClasses;n++)
				ModelSVM[n]=(SVMClassifier*)new char[sizeof(SVMClassifier)];

			//Entreno los n clasificadores individuales
			for (n=0;n<(int)ModelSVMGeneral->NumClasses;n++)
			{
				int elements;

				//Configura
				// default values
				ModelSVM[n]->param.svm_type = C_SVC;//NU_SVC;//ONE_CLASS???
				ModelSVM[n]->param.kernel_type = RBF;
				ModelSVM[n]->param.degree = 3;
				ModelSVM[n]->param.gamma = 1.0/(float)NumCoefs;
				ModelSVM[n]->param.coef0 = 0;
				ModelSVM[n]->param.nu = 0.5;
				ModelSVM[n]->param.cache_size = 200;
				ModelSVM[n]->param.C = 50;//era 50000, probé con 500 y va  mejor sin perder en reconocimiento desconocidos
				ModelSVM[n]->param.eps = 1e-3;
				ModelSVM[n]->param.p = 0.1;
				ModelSVM[n]->param.shrinking = 1;
				ModelSVM[n]->param.nr_weight = 0;
				ModelSVM[n]->param.probability = 0;
				ModelSVM[n]->param.weight_label = NULL;
				ModelSVM[n]->param.weight = NULL;

				//Definimos el problema (toma los datos de PCA)
				//Ver http://www.csie.ntu.edu.tw/~cjlin/libsvm/
				//Nùmero de muestras de entrenamiento, supongo que cada clase tiene el mismo nùmero
				ModelSVM[n]->prob.l=Dataset->TotalImages;
				elements=(NumCoefs+1)*ModelSVM[n]->prob.l; //Para cada muestra tiene los coeficientes

				ModelSVM[n]->prob.y = (double *)new unsigned char[sizeof(double)*ModelSVM[n]->prob.l];
				ModelSVM[n]->prob.x = (struct svm_node **)new unsigned char[sizeof(struct svm_node *)*ModelSVM[n]->prob.l];
				ModelSVM[n]->x_space = (struct svm_node *)new unsigned char[sizeof(struct svm_node)*elements];
				ModelSVM[n]->Label =(char**) new unsigned char[2*sizeof(char*)];
				ModelSVM[n]->FramesPerClass =(unsigned int*) new unsigned char[Dataset->NumClasses*sizeof(unsigned int)];
				ModelSVM[n]->feature_max=(double *)new unsigned char[sizeof(double)*NumCoefs];
				ModelSVM[n]->feature_min=(double *)new unsigned char[sizeof(double)*NumCoefs];
				
				ModelSVM[n]->NumClasses=2;
				//Una clase es la propia para una identidad, clase 1, y luego tenemos el resto clase 2
				ModelSVM[n]->Label[0]=(char*)new unsigned char[sizeof(char)*ID_STRING_SIZE];
				strcpy(ModelSVM[n]->Label[0],ModelSVMGeneral->Label[n]);
				ModelSVM[n]->Label[1]=(char*)new unsigned char[sizeof(char)*ID_STRING_SIZE];
				strcpy(ModelSVM[n]->Label[1],"unknown");

				int clase;
				int resto=0;
				ModelSVM[n]->FramesPerClass[0]=Dataset->FramesPerClass[n];					

				int i,j=0;

				//Inicializo max y min para escalar posteriormente
				for(i=0;i<(int)NumCoefs;i++)
				{
					ModelSVM[n]->feature_max[i]=-1e20;
					ModelSVM[n]->feature_min[i]=1e20;
				}

				for (i=0;i<ModelSVM[n]->prob.l;i++)
				{
					//Etiqueta del objetivo
					if (!strcmp(ModelSVM[n]->Label[0],Dataset->Label[i])) //clase en cuestiòn
					{
						clase=1;
					}
					else
					{
						resto++;//muestras en la clase resto
						clase=2;
					}
					ModelSVM[n]->prob.y[i]=(double)clase;


					//Puntero al los coeficientes de la muestra
					ModelSVM[n]->prob.x[i] = &(ModelSVM[n]->x_space[j]);
					//Copiamos
					for (int ni=0;ni<(int)NumCoefs;ni++)
					{
						double val=Projections->P[i][ni];

						ModelSVM[n]->x_space[j].index=ni+1;
						//tomo cada componente del espacio PCA como un atributo
						ModelSVM[n]->x_space[j].value=val;
						
						//Actualizo màximos y mìnimos
						if (val>ModelSVM[n]->feature_max[ni])
							ModelSVM[n]->feature_max[ni]=val;
						if (val<ModelSVM[n]->feature_min[ni])
							ModelSVM[n]->feature_min[ni]=val;


						j++;
					}

					//Anhadimos el ùltimo
					ModelSVM[n]->x_space[j++].index=-1;
					
				}
				//Muestras en la clase resto
				ModelSVM[n]->FramesPerClass[1]=resto;

				//Recorro ajustando el rango a 1,-1
				double upper=1,lower=-1;
				for (i=0;i<elements;i++)
				{
					if (ModelSVM[n]->x_space[i].index!=-1)
					{
						int index=ModelSVM[n]->x_space[i].index;
						ModelSVM[n]->x_space[i].value=lower+(upper-lower)*(ModelSVM[n]->x_space[i].value-ModelSVM[n]->feature_min[index-1])/(ModelSVM[n]->feature_max[index-1]-ModelSVM[n]->feature_min[index-1]);

					}
				}
				
				
				error_msg = svm_check_parameter(&ModelSVM[n]->prob,&ModelSVM[n]->param);

				//Entrenamiento
				clock_t	tini=clock();

	
				//Entrenamiento
				ModelSVM[n]->model = svm_train(&ModelSVM[n]->prob,&ModelSVM[n]->param);

				ModelSVM[n]->tproc=clock()-tini;
				
			}

			boSVMIndividualsComputed=true;

			return ModelSVM;
		}
		else//if ModelSVMGeneral->NumClasses>1
			return NULL;
	}//if ModelSVMGeneral
	else
		return NULL;

}

// GenerarDatosPCA: Realiza a partir de los datos PCA, un clasificador SVM
SVMClassifier ** CSVMUtils::ComputeSVMIndividualfromGabor()
{

	const char *error_msg;
	int n;
	SVMClassifier **ModelSVM=NULL;

	//Creo los n clasificadores individuales, pero sólo si existe el n clases
	if (ModelSVMGeneral)
	{		
		if (ModelSVMGeneral->NumClasses>1)
		{
			ModelSVM=(SVMClassifier**)new char[ModelSVMGeneral->NumClasses*sizeof(SVMClassifier *)];

			for (n=0;n<(int)ModelSVMGeneral->NumClasses;n++)
				ModelSVM[n]=(SVMClassifier*)new char[sizeof(SVMClassifier)];

			//NumCoefs ya viene definido por el modelo general

			//Entreno los n clasificadores individuales
			for (n=0;n<(int)ModelSVMGeneral->NumClasses;n++)
			{
				int elements;

				//Configura
				// default values
				ModelSVM[n]->param.svm_type = C_SVC;//NU_SVC;//
				ModelSVM[n]->param.kernel_type = RBF;
				ModelSVM[n]->param.degree = 3;
				ModelSVM[n]->param.gamma = 1.0/(float)NumCoefs;
				ModelSVM[n]->param.coef0 = 0;
				ModelSVM[n]->param.nu = 0.5;
				ModelSVM[n]->param.cache_size = 200;
				ModelSVM[n]->param.C = 50000;//500;//100;
				ModelSVM[n]->param.eps = 1e-3;
				ModelSVM[n]->param.p = 0.1;
				ModelSVM[n]->param.shrinking = 1;
				ModelSVM[n]->param.probability = 0;
				ModelSVM[n]->param.nr_weight = 0;
				ModelSVM[n]->param.probability = 0;
				ModelSVM[n]->param.weight_label = NULL;
				ModelSVM[n]->param.weight = NULL;

				//Definimos el problema (toma los datos de PCA)
				//Ver http://www.csie.ntu.edu.tw/~cjlin/libsvm/
				//Nùmero de muestras de entrenamiento, supongo que cada clase tiene el mismo nùmero
				ModelSVM[n]->prob.l=Dataset->TotalImages;
				elements=(NumCoefs+1)*ModelSVM[n]->prob.l; //Para cada muestra tiene los coeficientes

				ModelSVM[n]->prob.y = (double *)new unsigned char[sizeof(double)*ModelSVM[n]->prob.l];
				ModelSVM[n]->prob.x = (struct svm_node **)new unsigned char[sizeof(struct svm_node *)*ModelSVM[n]->prob.l];
				ModelSVM[n]->x_space = (struct svm_node *)new unsigned char[sizeof(struct svm_node)*elements];
				ModelSVM[n]->Label =(char**) new unsigned char[2*sizeof(char*)];
				ModelSVM[n]->FramesPerClass =(unsigned int*) new unsigned char[Dataset->NumClasses*sizeof(unsigned int)];
				ModelSVM[n]->feature_max=(double *)new unsigned char[sizeof(double)*NumCoefs];
				ModelSVM[n]->feature_min=(double *)new unsigned char[sizeof(double)*NumCoefs];
				
				ModelSVM[n]->NumClasses=2;
				//Una clase es la propia para una identidad, clase 1, y luego tenemos el resto clase 2
				ModelSVM[n]->Label[0]=(char*)new unsigned char[sizeof(char)*ID_STRING_SIZE];
				strcpy(ModelSVM[n]->Label[0],ModelSVMGeneral->Label[n]);
				ModelSVM[n]->Label[1]=(char*)new unsigned char[sizeof(char)*ID_STRING_SIZE];
				strcpy(ModelSVM[n]->Label[1],"unknown");

				int clase;
				int resto=0;
				ModelSVM[n]->FramesPerClass[0]=Dataset->FramesPerClass[n];

				int i,j=0,nc;

				//Gabor images have been computed for the general classifier
				if (GI==NULL)
				{
					GI=(GaborImages*)new unsigned char[sizeof(GaborImages)];
					GI->NumCoefs=NumCoefs;
					GI->TotalImages=Dataset->TotalImages;
					GI->G=(IplImage**) new unsigned char[GI->TotalImages*sizeof(IplImage*)];
				
					for (i=0;i<(int)GI->TotalImages;i++)
					{			
						//Computes Gabor Image
						if (boSpecificPointsforGabor)
							GI->G[i]=GaborUtils.GaborWavelet(Dataset->BC[i],GaborPoints,nGaborPoints,nGaborFilters*2);
						else
							GI->G[i]=GaborUtils.GaborWavelet(Dataset->BC[i],Gaborstep,nGaborFilters*2);
					}
				}

				//Inicializo max y min para escalar posteriormente
				for(i=0;i<(int)NumCoefs;i++)
				{
					ModelSVM[n]->feature_max[i]=-1e20;
					ModelSVM[n]->feature_min[i]=1e20;
				}

				for (i=0;i<ModelSVM[n]->prob.l;i++)
				{
					//Etiqueta del objetivo

					//Según si es muestra de la clase en cuestiòn o del resto
					if (!strcmp(ModelSVM[n]->Label[0],Dataset->Label[i])) //clase en cuestiòn
					{
						clase=1;
					}
					else
					{
						resto++;//muestras en la clase resto
						clase=2;
					}
					ModelSVM[n]->prob.y[i]=(double)clase;


					//Puntero al los coeficientes de la muestra
					ModelSVM[n]->prob.x[i] = &(ModelSVM[n]->x_space[j]);


					//Copiamos la imagen Gabor en la estructura de la biblioteca SVM
					BYTE *p=(BYTE *)GI->G[i]->imageData;
					float *pf;

					nc=0;

					//Para el ancho y alto de la imagen y canal (las componentes a clasificar)
					for (int nn=0;nn<GI->G[i]->height;nn++,p+=GI->G[i]->widthStep)
					{
						pf=(float *)p;
						for (int m=0;m<GI->G[i]->width;m++)
						{
							//Cada canal
							for (int p=0;p<nGaborFilters*2;p++,pf++)
							{
								double val=(double) *pf;

								ModelSVM[n]->x_space[j].index=nc+1;
								//tomo cada componente del espacio PCA como un atributo
								ModelSVM[n]->x_space[j].value=val;
								
								//Actualizo màximos y mìnimos para cada componente
								if (val>ModelSVM[n]->feature_max[nc])
									ModelSVM[n]->feature_max[nc]=val;
								if (val<ModelSVM[n]->feature_min[nc])
									ModelSVM[n]->feature_min[nc]=val;

								j++;
								nc++;
							}
						}
					}
					

					//Anhadimos el ùltimo
					ModelSVM[n]->x_space[j++].index=-1;
					
				}
				//Muestras en la clase resto
				ModelSVM[n]->FramesPerClass[1]=resto;

				//Recorro ajustando el rango a 1,-1
				double upper=1,lower=-1;
				for (i=0;i<elements;i++)
				{
					if (ModelSVM[n]->x_space[i].index!=-1)
					{
						int index=ModelSVM[n]->x_space[i].index;
						ModelSVM[n]->x_space[i].value=lower+(upper-lower)*(ModelSVM[n]->x_space[i].value-ModelSVM[n]->feature_min[index-1])/(ModelSVM[n]->feature_max[index-1]-ModelSVM[n]->feature_min[index-1]);

					}
				}
				
				
				error_msg = svm_check_parameter(&ModelSVM[n]->prob,&ModelSVM[n]->param);

				//Entrenamiento
				clock_t	tini=clock();

				//Entrenamiento
				ModelSVM[n]->model = svm_train(&ModelSVM[n]->prob,&ModelSVM[n]->param);

				ModelSVM[n]->tproc=clock()-tini;
				
		}
			boSVMIndividualsComputed=true;

			return ModelSVM;
		}
		else//if ModelSVMGeneral->NumClasses>1
			return NULL;
	}//if ModelSVMGeneral
	else
		return NULL;

}


//Classification

//Clase inicia en 0, por ello restamos -1 para acceder al clasificador correspondiente
int CSVMUtils::SVMIndividualClassification(float *Ip,int clase)
{
	unsigned int i;
	double lower=-1,upper=1;
	double v;

	struct svm_node *x = (struct svm_node *) new unsigned char[(NumCoefs+1)*sizeof(struct svm_node)];
	
	for (i=0;i<(unsigned int)NumCoefs;i++)
	{
		x[i].index=i+1;
		//Ajusto al rango (-1,1), los lìmites vienen dados por el 
		x[i].value=lower+(upper-lower)*(Ip[i]-ModelSVMIndividuals[clase-1]->feature_min[i])/(ModelSVMIndividuals[clase-1]->feature_max[i]-ModelSVMIndividuals[clase-1]->feature_min[i]);
	}
	x[i].index=-1;

	v= svm_predict(ModelSVMIndividuals[clase-1]->model,x);

	delete [] x;

	return (int)v;//La clase retornada empieza en 1, ...

}

//Clasifies using SVM, returns a class index (attention starting in 1 not 0)
int CSVMUtils::SVMClassification(float *Ip)
{
	int i=0;
	double lower=-1,upper=1;
	int v=1;

	//Translates info to libsvm format
	struct svm_node *x = (struct svm_node *) new unsigned char[(NumCoefs+1)*sizeof(struct svm_node)];
	
	for (i=0;i<NumCoefs;i++)
	{
		x[i].index=i+1;
		//Ajusto al rango (-1,1), los lìmites vienen dados por el 
		double num=(double)Ip[i]-ModelSVMGeneral->feature_min[i];
		//Controlo posible denominador nulo
		if (num==0.0)
			x[i].value=lower;
		else
			x[i].value=lower+(upper-lower)*(num)/(ModelSVMGeneral->feature_max[i]-ModelSVMGeneral->feature_min[i]);
	}
	x[i].index=-1;

	v = (int)svm_predictV(ModelSVMGeneral->model,x,ModelSVMGeneral->voting);

	delete [] x;

	return v;//La clase retornada empieza en 1, ...

}

int CSVMUtils::SVMClassification(IplImage *Ip)
{
	int i=0;
	double lower=-1,upper=1;
	int v=1;

	//Translates info to libsvm format
	struct svm_node *x = (struct svm_node *) new unsigned char[(NumCoefs+1)*sizeof(struct svm_node)];

	//Copiamos la imagen Gabor en la estructura de la biblioteca SVM
	BYTE *pb=(BYTE *)Ip->imageData;
	float *pf;

	for (int n=0;n<Ip->height;n++,pb+=Ip->widthStep)
	{
		pf=(float *)pb;
		for (int m=0;m<Ip->width;m++)
		{
			//Cada canal
			for (int p=0;p<nGaborFilters*2;p++,pf++)
			{
				double val=(double) *pf;

				x[i].index=i+1;
				//Ajusto al rango (-1,1), los lìmites vienen dados por el 
				x[i].value=lower+(upper-lower)*( (*pf) -ModelSVMGeneral->feature_min[i])/(ModelSVMGeneral->feature_max[i]-ModelSVMGeneral->feature_min[i]);

				i++;

			}

		}
	}
	x[i].index=-1;


	v = (int)svm_predictV(ModelSVMGeneral->model,x,ModelSVMGeneral->voting);

	delete [] x;

	return v;

}

int CSVMUtils::SVMClassification(float *Ic,IplImage *Ip)
{
	int i=0;
	double lower=-1,upper=1;
	int v=1;

	//Translates info to libsvm format
	struct svm_node *x = (struct svm_node *) new unsigned char[(NumCoefs+1)*sizeof(struct svm_node)];
	
	//PCA coefs
	for (i=0;i<NumCoefsPCA;i++)
	{
		x[i].index=i+1;
		//Ajusto al rango (-1,1), los lìmites vienen dados por el 
		x[i].value=lower+(upper-lower)*(Ic[i]-ModelSVMGeneral->feature_min[i])/(ModelSVMGeneral->feature_max[i]-ModelSVMGeneral->feature_min[i]);
	}
	
	//Gabor image
	//Copiamos la imagen Gabor en la estructura de la biblioteca SVM
	BYTE *pb=(BYTE *)Ip->imageData;
	float *pf;

	for (int n=0;n<Ip->height;n++,pb+=Ip->widthStep)
	{
		pf=(float *)pb;
		for (int m=0;m<Ip->width;m++)
		{
			//Cada canal
			for (int p=0;p<nGaborFilters*2;p++,pf++)
			{
				double val=(double) *pf;

				x[i].index=i+1;
				//Ajusto al rango (-1,1), los lìmites vienen dados por el 
				x[i].value=lower+(upper-lower)*( (*pf) -ModelSVMGeneral->feature_min[i])/(ModelSVMGeneral->feature_max[i]-ModelSVMGeneral->feature_min[i]);

				i++;

			}

		}
	}
	x[i].index=-1;

	v = (int)svm_predictV(ModelSVMGeneral->model,x,ModelSVMGeneral->voting);

	delete [] x;

	return v;

}



// FreeGabor: Memory assigned to Gabor images
void CSVMUtils::FreeGaborImages(GaborImages **GI)
{
	unsigned int i;

	if (*GI!=NULL)
	{

		for (i=0;i<(*GI)->TotalImages;i++)
		{
			cvReleaseImage(&((*GI)->G[i]));
		}
		delete [] (*GI)->G;
		delete [] (*GI);

		*GI=NULL;
	}
}

// FreeGabor: Memory assigned to Gabor images
void CSVMUtils::FreeLBPImages(LBPImages **LBP)
{
	unsigned int i;

	if (*LBP!=NULL)
	{

		for (i=0;i<(*LBP)->TotalImages;i++)
		{
			cvReleaseImage(&((*LBP)->I[i]));
		}
		delete [] (*LBP)->I;
		delete [] (*LBP);

		*LBP=NULL;
	}
}

void CSVMUtils::FreeSVMData()
{
	if (ModelSVMGeneral!=NULL)
	{
		//Si utiliza los modelos para cada individuo libera antes pq necesita saber el numero de clases
		if (boSVMAuth && ModelSVMIndividuals)
		{
			for (int n=0;n<(int)ModelSVMGeneral->NumClasses;n++)
			{
				svm_destroy_model(ModelSVMIndividuals[n]->model);

				if (boSVMIndividualsComputed)//Si los calculó libera estructuras intermedias
				{
					delete [] ModelSVMIndividuals[n]->prob.y;
					delete [] ModelSVMIndividuals[n]->prob.x;
					delete [] ModelSVMIndividuals[n]->x_space;
				}
				for (int i=0;i<(int)ModelSVMIndividuals[n]->NumClasses;i++) 
					delete [] ModelSVMIndividuals[n]->Label[i];

				delete [] ModelSVMIndividuals[n]->Label;
				delete [] ModelSVMIndividuals[n]->FramesPerClass;
				delete [] ModelSVMIndividuals[n]->feature_max;
				delete [] ModelSVMIndividuals[n]->feature_min;
				
				delete [] ModelSVMIndividuals[n];

			}
			delete [] ModelSVMIndividuals;
			ModelSVMIndividuals=NULL;
		}

		//Deallocates general SVM classifier
		svm_destroy_model(ModelSVMGeneral->model);

		//Was computed this time?
		if (boSVMComputed)
		{
			delete [] ModelSVMGeneral->prob.y;
			delete [] ModelSVMGeneral->prob.x;
			delete [] ModelSVMGeneral->x_space;
			boSVMComputed=false;
		}

		//Deallocates information related
		for (int i=0;i<(int)ModelSVMGeneral->NumClasses;i++) 
			delete [] ModelSVMGeneral->Label[i];
		delete [] ModelSVMGeneral->Label;
		delete [] ModelSVMGeneral->feature_max;
		delete [] ModelSVMGeneral->FramesPerClass;
		delete [] ModelSVMGeneral->feature_min;
		delete [] ModelSVMGeneral->voting;

		delete [] ModelSVMGeneral;

		ModelSVMGeneral=NULL;
	}
}


	
