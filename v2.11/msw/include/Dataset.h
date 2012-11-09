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
 #if ! defined ( DATASET_H )
#define DATASET_H

#ifdef ENCARA2INLINUX
#include <glob.h>	//using glob
#include <sys/stat.h>	//mkdir
#include <sys/types.h>
#define	MAXFILES	10000
#else
#include <direct.h>
#include <io.h>
#endif



//! Dataset of not classified images
/*!
Class defining a dataset not classified, used to generate Principal Component Analysis (PCA) spaces.
*/
class CDataset
{
public:

	//! Class constructor
	CDataset()
	{
		Init();
	}

	//! Class constructor
	/*!
	\param directory Dataset path
	*/
	CDataset(char *directory)
	{
		Init();

		LoadDataset(directory);
	}

	//! Class destructor
	~CDataset()
	{
		FreeDataset();
	}

	//! Images
	IplImage		**BC;
	//! Image size
	CvSize			ImageSize;
	//! Number of images
	unsigned int	TotalImages;


private:

	void Init()
	{
		BC=NULL;
		TotalImages=0;
	}


	//! LoadDataset: Loads dataset from disk to memory
	/*!
	\param directory Dataset path
	*/
	int LoadDataset(char *directory)
	{
		//aux variables

		int i=0;
		bool bojpg=false;
		bool bopng=false;

		TotalImages=0;

#ifndef ENCARA2INLINUX
		long l;
		struct _finddata_t fi;

		//moves to directory
		_chdir(directory);

		// Checks first dataset size

		//Counts total number of images in directory


		l=(long)_findfirst("*.Ipl", &fi);

		//If .Ipl are not available checks for jpg or png
		if (l==-1)
		{
			l=(long)_findfirst("*.jpg", &fi);
			if (l!=-1) bojpg=true;
		}


		if (l==-1)
		{
			l=(long)_findfirst("*.png", &fi);
			if (l!=-1)bopng=true;
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
		fi.gl_offs=0;

		glob("*.Ipl", 0, NULL, &fi);

		//If not checks for jpg
		if ((int)fi.gl_pathc==0)
		{
			glob("*.jpg", 0, NULL, &fi);
			bojpg=true;
		}

		//Guarda el número de imágenes encontradas
		TotalImages=(int)fi.gl_pathc;

		globfree(&fi);
#endif

		//If there are no images return
		if (TotalImages<=0)
		{
			return -1;
		}

		//Allocs memory for the dataset images
		BC=(IplImage**) new unsigned char [TotalImages*sizeof(IplImage*)];

		//Load images
		unsigned int Frame=0;

	#ifndef ENCARA2INLINUX
		if (bojpg)
			l=(long)_findfirst("*.jpg", &fi);
		else
		{
			if (bopng)
				l=(long)_findfirst("*.png", &fi);
			else
				l=(long)_findfirst("*.Ipl", &fi);
		}


		if (l!=-1)
		{
			do
			{
				if (!bojpg && !bopng)
					BC[Frame] = IplUtils.LoadIplImage(fi.name);
				else
					BC[Frame] = cvLoadImage(fi.name,0);	//Fuerza cargar imagen grises

				Frame++;
			} while (_findnext(l, &fi)==0);
			_findclose(l);
		}
	#else
		//Indica el número de posiciones que aloja
		fi.gl_offs=0;


		if (bojpg)
			glob("*.jpg", 0, NULL, &fi);
		else
		{
			if (bopng)
				glob("*.png", 0, NULL, &fi);
			else
				glob("*.Ipl", 0, NULL, &fi);
		}

		for (i=0;i<(int)fi.gl_pathc;i++)
		{
			//Si está disponible Ipl la carga
			if (!bojpg && !bopng)
					BC[Frame] = IplUtils.LoadIplImage(fi.gl_pathv[i]);
#ifndef         USES_OF
				else
					BC[Frame] = cvLoadImage(fi.gl_pathv[i],0);	//Fuerza cargar imagen grises
#endif

				Frame++;
		}

		globfree(&fi);
	#endif

		ImageSize=cvSize(BC[0]->width,BC[0]->height);

		return 1;
	}

	//! FreeDataset: Memory assigned to dataset
	void FreeDataset()
	{
		unsigned int i;

		if (BC!=NULL)
		{
			for (i=0;i<TotalImages;i++)
			{
				if (BC[i]!=NULL)
					cvReleaseImage(&(BC[i]));
			}
			delete [] BC;

			Init();
		}
	}



	CIplImageUtils	IplUtils;		//! IplImage utilities

};


//! Class defining a classified dataset of images
/*!
The dataset contains labelled images (of the same size) of the different classes. Used so far to generate SVM based classifiers.
*/
class CClassifiedDataset
{
public:

	//! Class constructor
	CClassifiedDataset()
	{
		boGaborImages=false;

		boLBPImages=false;

		Init();
	}

	//! Class constructor
	/*!
	\param directory Dataset path. It must contains a folder for each class.
	\param boGabor If set indicates that the dataset will consider Gabor images and store them under some circumstances the first time that they are computed.
	\param boLBP If set the dataset is composed of LBP images
	*/
	CClassifiedDataset(char *directory, bool boGabor=false,bool boLBP=false)
	{
		Init();

		boGaborImages=boGabor;

		boLBPImages=boLBP;

		LoadDataset(directory);
	}

	//! Class destructor
	~CClassifiedDataset()
	{
		FreeDataset();
	}

	//! Images
	IplImage		**BC;
	IplImage		**BCGabor;
	IplImage		**BCLBP;

	//! Array containing the number of images per class
	unsigned int	*FramesPerClass;
	//! Array of labels corresponding to the different classes
	char			**Label;
	//! Number of classes
	unsigned int	NumClasses;
	//! Image size
	CvSize			ImageSize;
	//! Total number of images
	unsigned int	TotalImages;

private:


	void Init()
	{
		BC=NULL;
		BCGabor=NULL;
		BCLBP=NULL;

		FramesPerClass=NULL;
		Label=NULL;
		NumClasses=0;
		TotalImages=0;
	}

	//! LoadDataset: Loads dataset from disk to memory
	int LoadDataset(char *directory)
	{

#ifndef ENCARA2INLINUX
		struct _finddata_t fi,ff;
#else

#endif
		int c,j;
		bool bojpg=false;
		bool bopng=false;

		chdir(directory);

		//Inicializa el número de imágenes
		TotalImages=0;

		// Primero vemos cuantas clases tenemos y el total de frames...
		NumClasses=0;
		bool	boOneClassHasNoImages=false;

#ifndef ENCARA2INLINUX
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
					NumClasses++;

					_chdir(fi.name);

					//Check first for Ipl
					m=(long)_findfirst("*.Ipl", &ff);

					//If not checks for jpg
					if (m==-1)
					{
						m=(long)_findfirst("*.jpg", &ff);

						if (m!=-1) bojpg=true;
					}

					//If not checks for png
					if (m==-1)
					{
						m=(long)_findfirst("*.png", &ff);

						if (m!=-1) bopng=true;
					}

					if (m!=-1)
					{
						//Cuenta las imágenes
						do
						{
							TotalImages++;
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

#else //Falta completar, copiado de Dataset
		glob_t fi,ff;
		int nclas;


		//Investiga el número de directorios
		//Indica el número de posiciones que aloja
		ff.gl_offs=0;//MAXFILES;

		glob("*", 0, NULL, &ff);

		if ((int)ff.gl_pathc!=0)
		{
			//Total de clases
			NumClasses+=(int)ff.gl_pathc;
			
			for (nclas=0;nclas<(int)ff.gl_pathc;nclas++)
			{
			      //Desciende a la carpeta
				chdir(ff.gl_pathv[nclas]);
				
				//Indica el número de posiciones que aloja
				fi.gl_offs=0;//MAXFILES;

				//glob("*.Ipl", GLOB_DOOFFS, NULL, &fi);
				glob("*.Ipl", 0, NULL, &fi);

				//If not checks for jpg
				if ((int)fi.gl_pathc==0)
				{
					glob("*.jpg", 0, NULL, &fi);
					bojpg=true;
				}

				if ((int)fi.gl_pathc>0)
					//Guarda el número de imágenes encontradas
					TotalImages+=(int)fi.gl_pathc;
				else
					boOneClassHasNoImages=true;//There are no samples for one class

				globfree(&fi);

				chdir("..");

			}
		}
		globfree(&ff);

#endif


		//Si no hay imàgenes o sólo una clase
		if (NumClasses<=1 || TotalImages<(unsigned int)NumClasses || boOneClassHasNoImages)
		{
			TotalImages=0;
			NumClasses=0;
			return -1;
		}

		FramesPerClass=(unsigned int*)new unsigned char[NumClasses*sizeof(unsigned int)];
		BC=(IplImage**) new unsigned char[TotalImages*sizeof(IplImage*)];
		if (boGaborImages)//Gabor images present?
			BCGabor=(IplImage**) new unsigned char[TotalImages*sizeof(IplImage*)];
		if (boLBPImages)//LBP images present?
			BCLBP=(IplImage**) new unsigned char[TotalImages*sizeof(IplImage*)];

		Label=(char**) new unsigned char[TotalImages*sizeof(char*)];

		//Ahora tras saber lo que hay y cerrar las estructuras, lee las imágenes
		c=0;  //class index
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

					if (bojpg)
						m=(long)_findfirst("*.jpg", &ff);
					else
					{
						if (bopng)
							m=(long)_findfirst("*.png", &ff);
						else
							m=(long)_findfirst("*.Ipl", &ff);
					}

					if (m!=-1)
					{
						do
						{
							j++;

							if (!bojpg && !bopng)
								BC[Frame] = IplUtils.LoadIplImage(ff.name);
							else
								BC[Frame] = cvLoadImage(ff.name,0);	//Fuerza cargar imagen grises


							if (boGaborImages || boLBPImages)//Gabor or LBP images present?
							{
								char ctmp[256];
								long l=(long)strlen(ff.name);

								strcpy(ctmp,ff.name);

								if (boGaborImages)
								{
									ctmp[l-3]='g';
									ctmp[l-2]='a';
									ctmp[l-1]='b';

									BCGabor[Frame] = IplUtils.LoadIplImage(ctmp);
								}
								else
								{
									if (boLBPImages)
									{
										ctmp[l-3]='p';
										ctmp[l-2]='n';
										ctmp[l-1]='g';


										BCLBP[Frame] = cvLoadImage(ctmp);
									}
								}


							}

							Label[Frame]=(char*)new unsigned char[sizeof(char)*ID_STRING_SIZE];
							strcpy(Label[Frame],fi.name);

							Frame++;

						} while (_findnext(m, &ff)==0);
					}
					_findclose(m);



					_chdir("..");
					FramesPerClass[c-1]=j;
				}

			} while (_findnext(l, &fi)==0);

			_findclose(l);
		}
#else//Falta completar, lo he copiado de Dataset
		//Indica el número de posiciones que aloja
		ff.gl_offs=0;

		glob("*", 0, NULL, &ff);

		if ((int)ff.gl_pathc!=0)
		{
			for (nclas=0;nclas<(int)ff.gl_pathc;nclas++)
			{
				int nsams;

				c++;
				
				//Desciende a la carpeta
				chdir(ff.gl_pathv[nclas]);

				j=0;

				//Indica el número de posiciones que aloja
				fi.gl_offs=0;

				if (!bojpg)
					glob("*.Ipl", 0, NULL, &fi);
				else
					glob("*.jpg", 0, NULL, &fi);

				for (nsams=0;nsams<(int)fi.gl_pathc;nsams++)
				{
					j++;

					//Si está disponible Ipl la carga
					if (!bojpg)
						BC[Frame] = IplUtils.LoadIplImage(fi.gl_pathv[nsams]);
#ifndef         USES_OF
					else
						BC[Frame] = cvLoadImage(fi.gl_pathv[nsams],0);	//Fuerza cargar imagen grises
#endif


					Label[Frame]=(char*)new unsigned char[sizeof(char)*ID_STRING_SIZE];
					strcpy(Label[Frame],ff.gl_pathv[nclas]);

					Frame++;


				}


				globfree(&fi);

				chdir("..");
				FramesPerClass[c-1]=j;

			}
		}
		globfree(&ff);

#endif

		ImageSize=cvSize(BC[0]->width,BC[0]->height);

		return 1;
	}



	// FreeDataset: Memory assigned to dataset
	void FreeDataset()
	{
		unsigned int i;

		if (BC!=NULL)
		{
			for (i=0;i<TotalImages;i++)
			{
				if (BC[i]!=NULL)
					cvReleaseImage(&(BC[i]));

				delete [] Label[i];
			}
			delete [] BC;

			delete [] Label;

			//Gabor images
			if (BCGabor!=NULL)
			{
				for (i=0;i<TotalImages;i++)
				{
					if (BCGabor[i]!=NULL)
						cvReleaseImage(&(BCGabor[i]));
				}
				delete [] BCGabor;
			}

			//LBP images
			if (BCLBP!=NULL)
			{
				for (i=0;i<TotalImages;i++)
				{
					if (BCLBP[i]!=NULL)
						cvReleaseImage(&(BCLBP[i]));
				}
				delete [] BCLBP;
			}



			delete [] FramesPerClass;

			Init();
		}
	}


	CIplImageUtils	IplUtils;		//! IplImage utilities

	bool	boGaborImages;
	bool	boLBPImages;

};


#endif
