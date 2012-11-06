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
 #if ! defined ( PCA_H )
#define PCA_H

//Files management
#ifdef ENCARA2INLINUX
#include <glob.h>	//uso de glob
#include <sys/stat.h>	//mkdir
#include <sys/types.h>
#define	MAXFILES	10000
#else
#include <direct.h>
#include <io.h>
#endif

#define MAXEIGENOBJECTS	200

#include "ENCARA2_2lib_defs.h"

#include "Dataset.h"
#include "IplImageUtils.h"	//Utilities for IplImage

class CDataset;//in advance

//! Class defining a dataset
class CPCA
{
public:

	//! Class constructor
	/*!
	*/
	CPCA()
	{
		Init();
	}

	//! Class constructor reading PCA space
	/*!
	\param directory Data location
	*/
	CPCA(char *directory)
	{
		Init();
		LoadPCAData(directory);
	}

	//! Class destructor
	~CPCA()
	{
		FreePCAData();
	}



	//! Loads PCA data from disk (iplimages), returning 0 is not found
	/*!
	\param directory Data location
	*/
	int LoadPCAData(char *directory)
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
			return -1;

		fscanf(f,"%d\n",&(NumEigenobjects));
		fscanf(f,"%d\n",&(NumCoefs));
		fscanf(f,"%d\n",&ImageSize.width);
		fscanf(f,"%d\n",&ImageSize.height);
		fscanf(f,"%d\n",&NumSamples);//not available for old sets

		if (!NumEigenobjects)
		{
			fclose(f);
			return -1;
		}

		eigVals = (float*) new unsigned char[(NumEigenobjects+1)*sizeof(float)];
		for (i=0;i<NumEigenobjects;i++)
			fscanf(f,"%f\n",&eigVals[i]);


		fclose(f);

#ifndef ENCARA2INLINUX
		sprintf(cPath,"%s\\Avg.Ipl",directory);
#else
		sprintf(cPath,"%s/Avg.Ipl",directory);
#endif

		avg=IplUtils.LoadIplImage(cPath);

		eigenObjects=(IplImage**) new unsigned char[(NumEigenobjects+1)*sizeof(IplImage*)];

		for (i=0;i<NumEigenobjects;i++)
		{

#ifndef ENCARA2INLINUX
			sprintf(cPath,"%s\\Eigenobject %d.Ipl",directory,i);
#else
			sprintf(cPath,"%s/Eigenobject %d.Ipl",directory,i);
#endif
			eigenObjects[i]=IplUtils.LoadIplImage(cPath);
		}

		// To achieve the same result than  ComputePCA... (error de la primera versión)
		//PCA->eigenObjects[PCA->NumAutoobjetos]=LoadIplImage(s,1);	(Parece dar problemas si reentreno en caliente a partir de datos leidos de fichero no generados en la sesiòn)
		eigenObjects[NumEigenobjects]=NULL;

		return 1;
	}

	//! Saves PCA data
	/*!
	\param directory Data location
	*/
	void SavesPCAData(char *directory)
	{
		FILE *f;
		unsigned int i;

		if (NumEigenobjects)
		{

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

			fprintf(f,"%d\n",NumEigenobjects);
			fprintf(f,"%d\n",NumCoefs);
			fprintf(f,"%d\n",ImageSize.width);
			fprintf(f,"%d\n",ImageSize.height);
			fprintf(f,"%d\n",NumSamples);//not available for all sets

			for (i=0;i<NumEigenobjects;i++)
				fprintf(f,"%f\n",eigVals[i]);
			fclose(f);

#ifndef ENCARA2INLINUX
			sprintf(cPath,"%s\\Avg.Ipl",directory);
#else
			sprintf(cPath,"%s/Avg.Ipl",directory);
#endif
			IplUtils.SaveIplImage(cPath,avg);

			
#ifndef         USES_OF
				//Imagen png
			sprintf(cPath,"%s/Avg.png",directory);
#else
			//Imagen Ipl
			sprintf(cPath,"%s/Avg.Ipl",directory);
#endif
			//Si la imagen de entrada no es de bytes, la transforma para visualizarla
			if (avg->depth>8)
			{
				IplImage *imgfromdirGray=cvCreateImage(cvSize(avg->width, avg->height),IPL_DEPTH_8U, avg->nChannels);

				double max,min,scale;

				cvMinMaxLoc(avg,&min,&max);

				scale=(double)(255./(max-min));

				cvConvertScale(avg,imgfromdirGray,scale,-min*scale);

#ifndef         USES_OF
				cvSaveImage(cPath,imgfromdirGray);
#else
				IplUtils.SaveIplImage(cPath,imgfromdirGray);
#endif

				cvReleaseImage(&imgfromdirGray);
			}

			else
				//Saves image
#ifndef         USES_OF
				cvSaveImage(cPath,avg);
#else
				IplUtils.SaveIplImage(cPath,avg);
#endif

			//Fin png

			//Save eigenimages
			for (i=0;i<NumEigenobjects;i++)
			{
#ifndef ENCARA2INLINUX
				sprintf(cPath,"%s\\Eigenobject %d.Ipl",directory,i);
#else
				sprintf(cPath,"%s/Eigenobject %d.Ipl",directory,i);
#endif
				IplUtils.SaveIplImage(cPath,eigenObjects[i]);

				//Imagen png
#ifndef         USES_OF
					sprintf(cPath,"%s/Eigenobject %d.png",directory,i);
#else
					sprintf(cPath,"%s/Eigenobject %d.Ipl",directory,i);
#endif				

				//Si la imagen de entrada no es de bytes, la transforma para visualizarla
				if (eigenObjects[i]->depth>8)
				{
					IplImage *imgfromdirGray=cvCreateImage(cvSize(eigenObjects[i]->width, eigenObjects[i]->height),IPL_DEPTH_8U, eigenObjects[i]->nChannels);

					double max,min,scale;

					cvMinMaxLoc(eigenObjects[i],&min,&max);

					scale=(double)(255./(max-min));

					cvConvertScale(eigenObjects[i],imgfromdirGray,scale,-min*scale);

#ifndef         USES_OF
					cvSaveImage(cPath,imgfromdirGray);
#else
					IplUtils.SaveIplImage(cPath,imgfromdirGray);
#endif

					cvReleaseImage(&imgfromdirGray);
				}

				else
					//Saves image
#ifndef         USES_OF
					cvSaveImage(cPath,eigenObjects[i]);
#else
					IplUtils.SaveIplImage(cPath,eigenObjects[i]);
#endif

				//Fin png
			}

		}
	}


	//! Computes PCA
	/*!
	Computes the Principal Component Analysis for a given dataset

	\param BC Image dataset
	*/
	void ComputePCA(CDataset *BC)
	{
		unsigned int i;

		//
		NumEigenobjects=BC->TotalImages-1;

		//Checks if training image dimension is lower than the number of images used for training (suggested by David)
		if ((unsigned int)((BC->BC[0]->width*BC->BC[0]->height))<=NumEigenobjects)
			NumEigenobjects=((BC->BC[0]->width*BC->BC[0]->height)-1);

		NumSamples=BC->TotalImages;

		//Bounds the number of objects to calculate
		if (NumEigenobjects>MAXEIGENOBJECTS)
			NumEigenobjects=MAXEIGENOBJECTS;

		ImageSize=BC->ImageSize;
		NumCoefs=NumEigenobjects;

		//Allocating memory
		eigenObjects=(IplImage**) new unsigned char[(NumEigenobjects+1)*sizeof(IplImage*)];

		if( !( eigVals = (float*) new unsigned char[(NumEigenobjects+1)*sizeof(float)] ) )	exit(1);

		if( !( avg = cvCreateImage( ImageSize, IPL_DEPTH_32F, 1 ) ) )	exit(1);
		for( i=0; i< (NumEigenobjects+1); i++ )
		{
			eigenObjects[i] = cvCreateImage( ImageSize, IPL_DEPTH_32F, 1 );
			if (!(eigenObjects[i]))	exit(1);
		}


		//Viejo esquema (versión linux bielefeld, aunque debería funcionar con última versión de openCV la otra variante
#ifdef ENCARA2INLINUX
		CvTermCriteria ct=cvTermCriteria(CV_TERMCRIT_ITER,NumEigenobjects,0);

		cvCalcEigenObjects(NumSamples,(void*)BC->BC, (void*)eigenObjects,
			CV_EIGOBJ_NO_CALLBACK,
			0,
			0,
			&ct,
			avg,
			eigVals );
#else
		//Apaño provisional para probar con cvCalcPCA
		CvMat* Input;
		CvMat src_row;

		unsigned int iimag;

		Input=cvCreateMat(BC->TotalImages, BC->BC[0]->width*BC->BC[0]->height, CV_8U );

		//Copiamos la imagen en su estructura cvMat (útil para calculos PCA)

		for (iimag=0;iimag<BC->TotalImages;iimag++)
		{
			cvGetRow( Input, &src_row, iimag );
			cvReshape( &src_row, &src_row, 0, BC->BC[iimag]->height );
			cvCopy( BC->BC[iimag], &src_row );
		}

		//Nueva forma
		CvMat* navg = cvCreateMat( 1, Input->cols, CV_32F );
		CvMat* neigenObjects=cvCreateMat((NumEigenobjects+1), Input->cols, CV_32F );
		CvMat* neigVals = cvCreateMat((NumEigenobjects+1), 1, CV_32F );

		cvCalcPCA(Input, navg, neigVals, neigenObjects,CV_PCA_DATA_AS_ROW);

		//Copiar a viejas estructuras
		//avg
		cvGetRow(navg, &src_row, 0 );
		cvReshape( &src_row, &src_row, 0, BC->BC[0]->height );
		cvCopy(&src_row,avg);

		for (iimag=0;iimag<NumEigenobjects;iimag++)
		{
			//eigenobjects
			cvGetRow( neigenObjects, &src_row, iimag );
			cvReshape( &src_row, &src_row, 0, BC->BC[iimag]->height );
			IplImage gray_img_hdr, *gray_img;
			gray_img = cvGetImage( &src_row, &gray_img_hdr );
			cvCopy(gray_img,eigenObjects[iimag]);

			//eigenvalues
			eigVals[iimag]=(float)cvGetReal1D(neigVals,iimag);
		}

		cvReleaseMat(&navg);
		cvReleaseMat(&neigenObjects);
		cvReleaseMat(&neigVals);
		cvReleaseMat(&Input);
#endif
	}

	//! Projects an image into given PCA space
	/*!
	Projects input image to PCA space. The input image format must be IPL_DEPTH_32F, if it is not provided
	in that format it will be temporary converted. Returns coefficients corresponding to the projection
	in this space
	\param I Input image to be projected
	\param ncomps Components used for projected representation
	*/
	float *Proyectar(IplImage *I, int ncomps=-1)
	{
		if ( !(ncomps<0 || ncomps>(int)NumCoefs) )
			NumCoefs=ncomps;

		float *Coef=(float*) new unsigned char[NumCoefs*sizeof(float)];

		//The average image is substracted
		IplImage *itemp= cvCreateImage( ImageSize, IPL_DEPTH_32F, 1 );

		//Checks if input image is IPL_DEPTH_32F
		if (I->depth!=IPL_DEPTH_32F)
		{
			IplImage *iaux= cvCreateImage( ImageSize, IPL_DEPTH_32F, 1 );
			cvConvert(I,iaux);

			cvSub(iaux,avg, itemp);

			cvReleaseImage(&iaux);
		}
		else
			cvSub(I,avg, itemp);

		// Then the eigenobjects are multiplied by the input image to get the coefficients
		int i;
		for (i=0;i<(int)NumCoefs;i++)
			Coef[i]=(float)cvDotProduct(itemp,eigenObjects[i]);

		cvReleaseImage(&itemp);

		return(Coef);
	}

	//! Reconstructs a projection
	/*!
	Reconstructs an image given its PCA representation, i.e. its coefficients. 	The output image format
	will be IPL_DEPTH_32F.
	\param Ip Input image to be projected
	\param ncomps Components used for projected representation
	*/
	IplImage * Reconstruir(float *Ip, int ncomps=-1)
	{
		int i;
		IplImage *ia;

		if (ncomps<0 || ncomps>(int)NumCoefs)
			NumCoefs=(int)NumCoefs;
		else
			NumCoefs=ncomps;

		//Allocates space for the reconstruction and an aux image
		ia= cvCreateImage( ImageSize, IPL_DEPTH_32F, 1 );
		IplImage *itemp= cvCreateImage( ImageSize, IPL_DEPTH_32F, 1 );

		//Multiply each eigenobject by its correspondin coefficient
		cvSetZero(ia);
		for (i=0;i<(int)NumCoefs;i++)
		{
			cvConvertScale(eigenObjects[i],itemp,Ip[i]);
			cvAdd(ia,itemp,ia);//??Para qué??
		}

		//Adds the average
		cvAdd(ia,avg,ia);

		cvReleaseImage(&itemp);

		return(ia);
	}


	//! Computes PCA reconstruction error
	/*!
	Computes the PCA reconstruction error of image imgIN
	\param imgIN Input image to be projected and reconstructed
	*/
	double GetPCAReconstructionError(IplImage *imgIN)
	{
		double error;
		IplImage *imgINfloat=cvCreateImage(ImageSize,IPL_DEPTH_32F, 1);

		IplImage *Reconsfloat;

		//Converts input image IPL_DEPTH_8U to IPL_DEPTH_32F
		cvConvert( imgIN, imgINfloat );

		//Projects input image
		float *coefs=Proyectar(imgINfloat);

		//Reconstructs the input image
		Reconsfloat=Reconstruir(coefs);

		delete [] coefs;

		//Computes the difference (non absolute)
		IplImage *DifS=cvCreateImage(ImageSize,IPL_DEPTH_32F, 1);
		IplImage *DifNorm=cvCreateImage(ImageSize,IPL_DEPTH_32F, 1);

		cvSub(imgINfloat,Reconsfloat,DifS);

		//Normalises the difference
		cvNormalize(DifS,DifNorm);

		//Gets the sum Consultar bibliografía
		error=cvNorm(DifS);//Hjelmas usa el cuadrado y de sólo la primera diferenciada con la media ??

		//Realeases memory

		cvReleaseImage(&imgINfloat);
		cvReleaseImage(&Reconsfloat);
		cvReleaseImage(&DifS);
		cvReleaseImage(&DifNorm);

		return error;
	}

	//! Modifies PCA space incrementally with a new observation
	/*!
	Computes the PCA reconstruction error of image imgIN. The reference used was "Incremental PCA
	for On-line Visual Learning and Recognition"
	\param NewImage	New observation to be considered
	\param ncomps		Number of components to be used for reconstruction, if none is indicated
						the number available will be used
	*/
	//Modifies PCA space incrementally with a new observation
	void IncrementalPCA(IplImage *NewImage, int ncomps=-1)
	{
		float	auxf,auxf2;
		int		i,j;

		if (NumCoefs && ImageSize.height==NewImage->height && ImageSize.width==NewImage->width)
		{
			//States the number of coefficiewnts
			/*if (ncomps<0 || ncomps>(int)PCA->NumCoefs)
				NumCoefs=(int)PCA->NumCoefs;
			else
				NumCoefs=ncomps;*/

			//Iplimages
			IplImage *imgINfloat=cvCreateImage(ImageSize,IPL_DEPTH_32F, 1);
			IplImage *Reconsfloat;

			IplImage *NewAvg=cvCreateImage(ImageSize,IPL_DEPTH_32F, 1);
			IplImage *xshift= cvCreateImage(ImageSize, IPL_DEPTH_32F, 1 );

			//Converts input image IPL_DEPTH_8U to IPL_DEPTH_32F
			cvConvert(NewImage, imgINfloat);

			//COMPUTES NEW AVERAGE avg_n+1= (n * avg - NewImage)/(n+1)
			//New values for the total number of samples
			int NewNumSamples=NumSamples+1;
			auxf=1.f/(float)NewNumSamples;
			auxf2=auxf*NumSamples;
			cvAddWeighted(avg,auxf2,imgINfloat,auxf,0,NewAvg);

			//COMPUTES THE RESIDUE
			//Projects input image
			float *a=Proyectar(imgINfloat);

			//Reconstructs the input image
			Reconsfloat=Reconstruir(a);

			//Computes the difference (non absolute)
			IplImage *h=cvCreateImage(ImageSize,IPL_DEPTH_32F, 1);
			IplImage *hNorm=cvCreateImage(ImageSize,IPL_DEPTH_32F, 1);

			cvSub(imgINfloat,Reconsfloat,h);

			//Normalises the difference getting the residue
			cvNormalize(h,hNorm);

			//
			cvSub(imgINfloat,avg, xshift);//Entrada menos media
			float gamma=(float)cvDotProduct(hNorm,xshift);

			//En otro caso no hay error
			if (gamma)
			{
				//D is built
				/********************************************************************************

					D= (n/(n+1)) Da + (n/((n+1)*(n+1))) Db


				*******************************************************************************/
				CvMat *D=cvCreateMat(NumCoefs+1,NumCoefs+1,CV_32FC1);//32 bits y un canal
				CvMat *Da=cvCreateMat(NumCoefs+1,NumCoefs+1,CV_32FC1);
				CvMat *Db=cvCreateMat(NumCoefs+1,NumCoefs+1,CV_32FC1);


				//Da creation
				cvSetZero(Da);

				//Sets the diagonal with the current PCA->NumSamples eigenvalues
				for (i=0;i<(int)NumCoefs;i++)
				{
					cvSetReal2D(Da,i,i,eigVals[i]);
				}


				//Db creation

				//Rellenamos todo excepto últimas filas y columnas
				for (i=0;i<(int)NumCoefs;i++)
				{
					for (j=0;j<(int)NumCoefs;j++)
						cvSetReal2D(Db,i,j,a[i]*a[j]);
				}
				//última fila y columna excepto esquina inferior izquierda
				for (j=0;j<(int)NumCoefs;j++)
				{
					cvSetReal2D(Db,NumCoefs,j,gamma*a[j]);
					cvSetReal2D(Db,j,NumCoefs,gamma*a[j]);
				}
				//última posición
				cvSetReal2D(Db,NumCoefs,NumCoefs,gamma*gamma);

				//D
				auxf=(float)NumSamples/(float)NewNumSamples;
				auxf2=auxf/(float)NewNumSamples;
				cvAddWeighted(Da,auxf,Db,auxf2,0,D);

				//Calculamos PCA planteando un problema de menor dimensión, sólo PCA->NumCoefs+1*PCA->NumCoefs+1
				CvMat* R=cvCreateMat(NumCoefs+1,NumCoefs+1,CV_32FC1),
					*evals=cvCreateMat(NumCoefs+1,1,CV_32FC1);;

				//Resuelve el problema reducido para obtener nuevos autovalores y R
				cvSVD(D,evals,R);//SVD es más rápida para casos definida positiva que EigenVV

				//Copia los autovectores previos
				CvMat* Up=cvCreateMat(ImageSize.width*ImageSize.height,NumCoefs+1,CV_32FC1);
				CvMat* Upfinal=cvCreateMat(ImageSize.width*ImageSize.height,NumCoefs+1,CV_32FC1);

				//Copia los que teníamos que son PCA->NumCoefs
				for (i=0;i<(int)NumCoefs;i++)
				{
					for (j=0;j<ImageSize.width*ImageSize.height;j++)
					{
						cvSetReal2D(Up,j,i,cvGetReal2D(eigenObjects[i],j/ImageSize.width,j%ImageSize.width));
					}

				}
				//la última columna será el residuo
				for (j=0;j<ImageSize.width*ImageSize.height;j++)
				{
					cvSetReal2D(Up,j,NumCoefs,cvGetReal2D(hNorm,j/ImageSize.width,j%ImageSize.width));
				}

				//Rota los autovectores previos utilizando la R tomada del problema PCA reducido
				cvMatMul(Up,R,Upfinal);

				//Actualiza media, nsamples, autovalores y autovectores. De momento nos quedamos con esa dimensión, PCA->NumCoefs
				//Algunos autores hablan de técnicas para decidir si es necesario aumentar la dimensión
				cvCopy(NewAvg,avg);
				NumSamples++;

				//valores principales
				for (i=0;i<(int)NumCoefs;i++)
					eigVals[i]=(float)cvGetReal1D(evals,i);

				//autovectores
				for (i=0;i<(int)NumCoefs;i++)
				{
					for (j=0;j<ImageSize.width*ImageSize.height;j++)
					{
						cvSetReal2D(eigenObjects[i],j/ImageSize.width,j%ImageSize.width,cvGetReal2D(Upfinal,j,i));
					}
				}

				cvReleaseMat(&D);
				cvReleaseMat(&Da);
				cvReleaseMat(&Db);
				cvReleaseMat(&evals);
				cvReleaseMat(&R);
				cvReleaseMat(&Up);
				cvReleaseMat(&Upfinal);

			}//if (gamma)

			delete [] a;

			//Releases memory

			cvReleaseImage(&h);
			cvReleaseImage(&hNorm);

			cvReleaseImage(&imgINfloat);
			cvReleaseImage(&NewAvg);
			cvReleaseImage(&Reconsfloat);
			cvReleaseImage(&xshift);
		}
	}


	//! Average image
	IplImage		*avg;
	//! Eigenobjects (e.g. eigenfaces)
	IplImage		**eigenObjects;
	//! Eigen vals
	float			*eigVals;

	//! Image size
	CvSize			ImageSize;
	//! Number of eigenobjects
	unsigned int	NumEigenobjects;
	//! Number of coefficients to use (<=NumAutoobjetos)
	unsigned int	NumCoefs;
	//! Number of samples used to compute this space (if available)
	unsigned int	NumSamples;

private:


	void FreePCAData()
	{
		if (NumEigenobjects)
		{
			delete [] eigVals;
			for( unsigned int i=0; i< NumEigenobjects+1; i++ )
				cvReleaseImage(&(eigenObjects[i]));
			delete [] eigenObjects;
			cvReleaseImage(&(avg));


			Init();
		}
	}

	void Init()
	{
		avg=NULL;
		eigenObjects=NULL;
		eigVals=NULL;
		NumSamples=0;
		NumCoefs=0;
		NumEigenobjects=0;
	}


	char cPath[512];
	CIplImageUtils	IplUtils;
};





#endif
