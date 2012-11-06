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
#include "stdio.h"
#include "string.h"

#include "math.h"

#include "CWarpingImage.h"

//Default constructor
CWarpingImage::CWarpingImage(int iX,int iY)
{

	iSx=iX;
	iSy=iY;
	pbImage=new unsigned char[iSx*iSy];
	pbImageMasked=new unsigned char[iSx*iSy];
	pbEllipticalMask=NULL;
}

//Constructor providing normalized eye positions
CWarpingImage::CWarpingImage(int iX,int iY,int OIx,int OIy,int ODx,int ODy)
{

	iSx=iX;
	iSy=iY;
	pbImage=new unsigned char[iSx*iSy];
	pbImageMasked=new unsigned char[iSx*iSy];
	OJOIX=OIx;
	OJOIY=OIy;
	OJODX=ODx;
	OJODY=ODy;

	pbEllipticalMask=new unsigned char[iSx*iSy];

	EllipticalMask();

}

CWarpingImage::~CWarpingImage()
{
	delete [] pbImage;
	delete [] pbImageMasked;

	if (pbEllipticalMask)
		delete [] pbEllipticalMask;
}


/************************************************************************************************** 
Normalizes face according to the position of the eyes (other alternatives are also included
***************************************************************************************************
 
DESCRIPCION : Considera que se realiza un escalado y una traslación,
resuelve a partir de dos pares de puntos datos (Px,Py)
 
   
PARAMETROS : 
				OIx,OIy	coordenadas ojo izquierdo
				ODx,ODy	coordenadas ojo derecho
				*out	valores de la transformación retornados
				OInx,OIny	coordenadas normalizadas ojo izquierdo
				ODnx,ODny	coordenadas normalizadas ojo derecho
   
 
***********************************************************************/ 
//Devuelve los valores de escalado y traslación para situar los ojos en una posición
void CWarpingImage::ResuelveEscaladoTraslacion(int OIx, int OIy,int ODx, int ODy, float*out,int OInx, int OIny,int ODnx, int ODny)
{
	
		//Completamente alterable OJO
		//Para el tamaño 60*65 consideramos los ojos en 20,17 (OI) y 40,17 (OD)
		//sx
		out[0]=(float)(ODnx-OInx)/(float)(ODx-OIx);
		//tx
		out[1]=ODnx-out[0]*ODx;
		//sy lo hacemos homogéneo (o parecerán alargadas)
		out[2]=out[0];
		//ty
		out[3]=ODny-out[2]*ODy;

		
}

/**********************************************************************

  Rotation:
  Realizando tres desplazamientos calcula la rotación (Wolberg) sobre el punto
  cx,cy. Para evitar posibles agujeros debido al redondeo el esquema 
  obliga a que un pixel influya sobre dos en cada desplazamiento 

  BYTE *BYTEcara			Imagen de entrada
  BYTE **BYTEcaraR			Imagen de salida (rotada)
  int foveax,int foveay		Dimensiones imagen de entrada
  int cx, int cy			Coordenadas del punto sobre el que rotamos
  int ix,int iy				Esquina superior izquierda de la zona a rotar
  int dx, int dy			Esquina inferior derecha
  float th					Ángulo a rotar
//Espero que se pueda reducir coste, sobre todo en el último desplazamiento (lo malo es que se accede a un romboide.
************************************************************************/
void CWarpingImage::Rotar(BYTE *BYTEcara,BYTE **BYTEcaraR,int foveax,int foveay,int cx, int cy,int ix,int iy,int dx, int dy,float th)
{
int i,j,aux;
double tth,sth;
double xant,yant,iaux,peso;
BYTE	*BYTEtemp,*BYTEtemp2;

	tth=(float)tan(th/(float)2.0);
	sth=(float)sin(th);

	BYTEtemp=new BYTE[foveax*foveay*sizeof(BYTE)];
	BYTEtemp2=new BYTE[foveax*foveay*sizeof(BYTE)];

	memset((void *)BYTEtemp,0,foveax*foveay*sizeof(BYTE));
	memset((void *)BYTEtemp2,0,foveax*foveay*sizeof(BYTE));
	memset((void *)(*BYTEcaraR),0,foveax*foveay*sizeof(BYTE));

	//Horizontal skew
	for (i=iy;i<dy;i++){
		aux=i*foveax;
		iaux=tth*(double)(i-cy);
		for (j=ix;j<dx;j++){
			//en tres pasos
			//Horizontal skew
			xant=((double)((j-cx)-iaux)+(double)cx);
			yant=(double)(i);
			peso=xant-(int)xant;
			//afecta a dos pixels
			if ((int)(xant)>0 && (int)(xant+0.5)<foveax)
			{
				BYTEtemp[aux+(int)(xant)]+=(BYTE)(BYTEcara[aux+j]*(1.0-peso));
				BYTEtemp[aux+(int)(xant+0.5)]+=(BYTE)(BYTEcara[aux+j]*peso);
			}

			
		}
	}
//vertical skew
	for (i=iy;i<dy;i++){
		aux=i*foveax;
		iaux=tth*(double)(i-cy);
		for (j=ix-(int)iaux;j<dx-(int)iaux;j++){
			yant=((double)((j-cx)*sth+(double)(i-cy))+(double)cy);
			peso=yant-(int)yant;
			if (((int)(yant+0.5))>0 && ((int)(yant+0.5))<foveay)
			{
				BYTEtemp2[((int)(yant))*foveax+j]+=(BYTE)(BYTEtemp[aux+j]*(double)(1.0-peso));
				BYTEtemp2[((int)(yant+0.5))*foveax+j]+=(BYTE)(BYTEtemp[aux+j]*peso);
			}
		}
	}

 
  //second Horizontal skew no está minimizado el recorrido por la matriz (OJO)
	for (i=0;i<foveay/*dy+tth*(double(dy-cy))*/;i++){
		aux=i*foveax;
		iaux=tth*(double)(i-cy);
		for (j=0;j<foveax/*dx+(double)(dx-cx)*sth*/;j++){
			//en tres pasos
			//Horizontal skew
			xant=((double)((j-cx)-iaux)+(double)cx);
			yant=(double)(i);
			peso=xant-(int)xant;
			//afecta a dos pixels
			if ((int)(xant)>0 && (int)(xant+0.5)<foveax && j<foveax &&j >0)
			{
				(*BYTEcaraR)[aux+(int)(xant)]+=(BYTE)(BYTEtemp2[aux+j]*(1.0-peso));
				(*BYTEcaraR)[aux+(int)(xant+0.5)]+=(BYTE)(BYTEtemp2[aux+j]*peso);
			}

			
		}
	}



delete [] BYTEtemp;
delete [] BYTEtemp2;
}


/**********************************************************************

  Rotation:
  Realizando tres desplazamientos calcula la rotación (Wolberg) sobre el punto
  cx,cy. Cada pixel rotado afecta a un solo pixel, usa aritmética entera y es más
  rápido, pero se nota cierto efecto escalera.

  BYTE *BYTEcara			Imagen de entrada
  BYTE **BYTEcaraR			Imagen de salida (rotada)
  int foveax,int foveay		Dimensiones imagen de entrada
  int cx, int cy			Coordenadas del punto sobre el que rotamos
  int ix,int iy				Esquina superior izquierda de la zona a rotar
  int dx, int dy			Esquina inferior derecha
  float th					Ángulo a rotar
//Espero que se pueda reducir coste, sobre todo en el último desplazamiento (lo malo es que se accede a un romboide.
************************************************************************/
//Hace la partición pero no promedia los pixels afectados, la escalera se nota, pero es más rápida
void CWarpingImage::RotarInt(BYTE *BYTEcara,BYTE **BYTEcaraR,int foveax,int foveay,int cx, int cy,int ix,int iy,int dx, int dy,float th)
{
int i,j,aux;

	double tth,sth;
	int xant,yant;
	double iaux;

	tth=(float)tan(th/(float)2.0);
	sth=(float)sin(th);

	//Realiza la rotación alrededor del punto cx,cy en base a tres desplazamientos (Wolberg)
	for (i=iy;i<dy;i++){
		aux=i*foveax;
		iaux=tth*(double)(i-cy);
		for (j=ix;j<dx;j++){
			//en tres pasos
			//Horizontal skew
			xant=(int)(((double)((j-cx)-iaux)+(double)cx)+0.5);
			yant=i;
			//vertical skew
			//xant=(xant);
			yant=(int)(((double)((xant-cx)*sth+(double)(yant-cy))+(double)cy)+0.5);
			//Second Horizontal skew
			xant=(int)(((double)((xant-cx)-tth*(double)(yant-cy))+(double)cx)+0.5);
			//yant=(yant);
			if ((int)(xant)>0 && (int)(xant+0.5)<foveax && (int)(yant+0.5)<foveay && yant>0)
				(*BYTEcaraR)[((int)(yant+0.5))*foveax+(int)(xant+0.5)]=(BYTE)BYTEcara[aux+j];
			
		}
	}
}

// Rotates a point
void CWarpingImage::Rotarxy(int cx, int cy,int *px,int *py,float th)
{
double tth,sth;
double xant,yant;

	tth=(float)tan(th/(float)2.0);
	sth=(float)sin(th);
	
	//Horizontal skew
	xant=((double)((*px-cx)-tth*(double)(*py-cy))+(double)cx);
	
	yant=((double)((xant-cx)*sth+(double)(*py-cy))+(double)cy);

	xant=((double)((xant-cx)-tth*(double)(yant-cy))+(double)cx);

	*px=(int)(xant+0.5);
	*py=(int)(yant+0.5);

}

/****************************************************************

  Adjust according to eyes, and adjust gray range if necessary

  isx,isy	Input image size
  pPixels	Input image pointer
  iOjoDX,iOjoDY,iOjoIX,iOjoIY	Detected eye positions
  mean							Normalized image mean value

  ***************************************************************/
void CWarpingImage::NormalizaOjos(BYTE **ppPixels,int isx, int isy,int iOjoIX,int iOjoIY,int iOjoDX,int iOjoDY,int *mean)
{
	//Normalized eyes and given relation
	float	feyerate=((float)iOjoDX-(float)iOjoIX)/((float)OJODX-(float)OJOIX);

    BYTE * pbaux,*p;
    int sx, sy,iox,ioy,i,j;
	float	sa,la,fval;
	int		incx,incy,px,py;

	//Adjusting the size of the intermediate image
	sx=(int)(iSx*feyerate);
	sy=(int)(iSy*feyerate);
	//Adjusting the origin to copy the area to warp
	iox=iOjoIX-(int)(OJOIX*feyerate);
	ioy=iOjoIY-(int)(OJOIY*feyerate);

	pbaux=(unsigned char *) new unsigned char[sx*sy];

	p=pbaux;
	for (i=ioy;i<ioy+sy;i++)
		for(j=iox;j<iox+sx;j++,p++){
			//Checking not to go outbounds
			if (i<isy && j<isx && i>=0 && j>=0)
				*(p)=ppPixels[i][j];
			else
				*p=0;
		}

	
	//Resampling
	Remap(pbaux,sx,sy);

	delete [] pbaux;

	//Elliptical mask
	//The elliptical nask is related with the eye position
	sa=(OJODX-OJOIX)*(float)1.15;
	la=sa*(float)1.37;
	sa*=sa;
	la*=la;
	//Center of the elliptical mask
	incy=(OJODY+(int)((double)(OJODX-OJOIX)*0.3));
	incx=((OJODX+OJOIX)>>1);
		
	p=pbImage;
	*mean=0;
	int cont=0;
	for (i=0;i<iSy;i++)
		for(j=0;j<iSx;j++,p++)
		{
			//Is the elliptical mask is selected
			if (ELLIPSEMASK)
			{
				py=i-incy;
				px=j-incx;
				//Applies the mask points
				fval=((float)px*((float)px))/(sa)+((float)py*((float)py))/(la);
				if (fval>=1.0) 
					*p=125;
			}
		}


}

/****************************************************************

  Adjust according to eyes

  isx,isy	Input image size
  pPixels	Input image pointer
  iOjoDX,iOjoDY,iOjoIX,iOjoIY	Detected eye positions
  iBarbillaX,iBarbillaY			Detected chin position
  OJODX,OJODY,OJOIX,OJOIY		Normalized eye positions
  BARBX,BARBY					Normalized chin position
  mean							Normalized image mean value

  ***************************************************************/
void CWarpingImage::NormalizaOjos(BYTE **ppPixels,int isx, int isy,
								  int iOjoIX,int iOjoIY,int iOjoDX,int iOjoDY,
								  int iBarbillaX, int iBarbillaY,
								  int OJOIX,int OJOIY,int OJODX,int OJODY, 
								  int BARBX, int BARBY, int *mean)
{
	//Relación entre ojos normalizados y detectados
	float	feyeratex=((float)iOjoDX-(float)iOjoIX)/((float)OJODX-(float)OJOIX);
	float	feyeratey=(iBarbillaY-((float)iOjoDY+(float)iOjoIY)/(float)2.0)/(BARBY-((float)OJODY+(float)OJOIY)/(float)2.0);

    BYTE *pbaux,*p,*q;
    int sx, sy,iox,ioy,i,j;

	//Ajusto los tamaños de la imagen intermedia según la relación
	sx=(int)(iSx*feyeratex);
	sy=(int)(iSy*feyeratey);
	//Ajusto el origen para copiar la zona de la imagen a redimensionar
	iox=iOjoIX-(int)(OJOIX*feyeratex);
	ioy=iOjoIY-(int)(OJOIY*feyeratey);

	pbaux=(unsigned char *) new unsigned char[sx*sy];

	p=pbaux;
	for (i=ioy;i<ioy+sy;i++)
		for(j=iox;j<iox+sx;j++,p++)
		{
			//controlando no salirse
			if (i<isy && j<isx && i>=0 && j>=0)
				*(p)=ppPixels[i][j];
			else
				*p=0;
		}
	
	//Resampling
	Remap(pbaux,sx,sy);
	
	delete [] pbaux;


	p=pbImage;
	q=pbEllipticalMask;
	*mean=0;
	int cont=0;
	if (ELLIPSEMASK){
		for (i=0;i<iSy;i++)
		{
			for(j=0;j<iSx;j++,p++,q++)
			{
				//Si es màscara
				if (*q)
					*p=*q;
				else
				{
					*mean+=*p;
					cont++;
				}
			}
		}
	}
	*mean/=cont;


}

/****************************************************************

  Adjust according to eyes

  isx,isy	Input image size
  pPixels	Input image pointer
  iOjoDX,iOjoDY,iOjoIX,iOjoIY	Detected eye positions
  mean							Normalized image mean value
  boMascaraEliptica				true by default to apply a mask to face images


  ***************************************************************/
void CWarpingImage::NormalizaOjos(BYTE *pPixels,int isx, int isy,
								  int iOjoIX,int iOjoIY,
								  int iOjoDX,int iOjoDY,int * mean, 
								  bool boMascaraEliptica,bool boIlluminationNormalization)
{
	
	//Normalized eyes and given relation
	double	feyeratio=(double)(iOjoDX-iOjoIX)/(double)(OJODX-OJOIX);

    BYTE    *pbaux,*p,*pm,*q;
	BYTE	*pp,*qq;
	float	*pfaux=NULL,*fq;
    int		wsx,wsy,iox,ioy,i,j;

	//Adjusting the size of the intermediate image
	wsx=(int)((double)(iSx)*feyeratio);
	wsy=(int)((double)(iSy)*feyeratio);

	//Adjusting the origin to copy the area to warp
	int oiox=(int)(((double)(OJOIX))*feyeratio);
	int oioy=(int)(((double)(OJOIY))*feyeratio);
	
	iox=iOjoIX-oiox;
	ioy=iOjoIY-oioy;

	pbaux=(unsigned char *) new unsigned char[wsx*wsy];

	//Copying the image
	p=pbaux;
	for (i=ioy;i<ioy+wsy;i++)
		for(j=iox;j<iox+wsx;j++,p++){
			//Checking not to go outbounds
			if (i<isy && j<isx && i>=0 && j>=0)
			{
				*p=pPixels[i*isx+j];
			}
			else 
				*p=0;
		}

	//Resampling
	Remap(pbaux,wsx,wsy);

	delete [] pbaux;

	float max=0,min=256;

	//Illumination normalizatio
	//Artículo PRL Usa máscara 7x7
	if (boIlluminationNormalization)
	{
		pfaux=(float *) new unsigned char[iSx*iSy*sizeof(float)];

		long	lmeans;
		int		nm,ii;

		p=pbImage;
		fq=pfaux;
		//Local means

		//Variance

		//f(x,y)=(f(x,y)-E(f(x,y))/Var(f(x,y)))

		for (i=0;i<iSy;i++)
		{
			//Para el primer punto de la fila obtengo la lmeans del que sería su vecino izquierdo,
			//es decir, las tres primeras columnas
			lmeans=0;
			nm=0;
			pp=p-3*iSx;//Puntero al inicio de la fila
			for (int pi=i-3;pi<=i+3;pi++,pp+=iSx)
			{
				//Es una fila dentro de la imagen
				if (pi>=0 && pi<iSy)
				{
					qq=pp;
	
					//Recorre los tres píxeles de la fila
					for (int pj=0;pj<3;pj++,qq++)
					{		
						lmeans+=*qq;
						nm++;
					}
				}
			}
			//He obtenido lo que sería la media del vecino izquierdo al primero de la fila


			for(j=0;j<iSx;j++,p++,fq++)
			{
				//Calculo media local de forma incremental a partir del vecino izquierdo
				//Resto primera columna (7 valores al ser 7x7) del anterior, y sumo la nueva por la derecha
				for (ii=i-3;ii<=i+3;ii++)
				{
					//Si es una fila válida
					if (ii>=0 && ii<iSy)
					{
						//Quito elemento de cuatro filas a la izquierda
						if (j-4>=0)
						{
							lmeans-=*(p-4+(ii-i)*iSx);
							nm--;
						}
						//Añado elemento de la tercera fila a la derecha
						if (j+3<iSx)
						{
							lmeans+=*(p+3+(ii-i)*iSx);
							nm++;
						}
					}
				}

				int lmeansdiv=lmeans/nm;
				//Variance
				int nvar=0;
				float var=0;
				int aux;

				//Coloca puntero
				pp=p-3*iSx-3;

				for (ii=i-3;ii<=i+3;ii++,pp+=iSx)
				{
					if (ii>=0 && ii<iSy)
					{
						qq=pp;
						for (int jj=j-3;jj<=j+3;jj++,qq++)
						{
							if (jj>=0 && jj<iSx)
							{
								nvar++;

								aux=*qq-lmeansdiv;
								var+=aux*aux;
							}
						}
					}
					//Si es una fila válida				
				}

				var/=nvar;

				var=(float)(sqrt(var)+0.0001);

				//Nueva imagen
				float val=(float)((((float)(*p) - (float)(lmeansdiv))/(var))+0.5);
				*fq= val;
				if (*fq<min) min=*fq;
				if (*fq>max) max=*fq;
			}
		}

	}

	//Aplies mask in requested
	p=pbImage;
	pm=pbImageMasked;
	q=pbEllipticalMask;
	fq=pfaux;


	//If the mask is requested
	if (boMascaraEliptica)
	{
		for (i=0;i<iSy;i++)
		{
			for(j=0;j<iSx;j++,p++,pm++,q++,fq++)
			{
				if(*q) //If the pixel is in the masked zone it is overwritten with the mask value
				{
					*pm=*q;
				}
				else
				{
					if (boIlluminationNormalization)
						*pm=(unsigned char) (255.0*(*fq-min)/(max-min));
					else
						*pm=*p;
				}
			}
		}
	}
	


	if (boIlluminationNormalization) delete [] pfaux;


}


/****************************************************************

  Adjust the image size

  isx,isy	Input image size
  pPixels	Input image pointer
 
  ***************************************************************/
void CWarpingImage::TransformaEscala(BYTE *pPixels,int isx, int isy)
{
	
	//Relación entre ojos normalizados y detectados
	double	fratio=(double)(isx)/(double)(iSx);

    BYTE    *pbaux,*p;
    int		wsx,wsy,i,j;

	//Ajusto los tamaños de la imagen intermedia según la relación
	wsx=(int)((double)(iSx)*fratio);
	wsy=(int)((double)(iSy)*fratio);

	
	pbaux=(unsigned char *) new unsigned char[wsx*wsy];

	p=pbaux;
	for (i=0;i<wsy;i++)
		for(j=0;j<wsx;j++,p++){
			//controlando no salirse de la imagen de entrada
			if (i<isy && j<isx && i>=0 && j>=0)
			{
				*p=pPixels[i*isx+j];

			}
			else 
				*p=0;
		}

	//Resampling
	Remap(pbaux,wsx,wsy);

	delete [] pbaux;

}


/******************************************************************************

				Create an image with the elliptical mask applied 

  *****************************************************************************/
void CWarpingImage::EllipticalMask()
{

	int i,j;
	int		incx,incy,px,py;
	float	sa,la,fval;

	//Voy a usar los cuadrados, heurísticas
	sa=(float)((OJODX-OJOIX)*1.15);
	la=(float)(sa*1.37);
	sa*=sa;
	la*=la;
	//Centro de la elipse de recorte en base a posiciones normalizadas
	incy=(OJODY+(int)((double)(OJODX-OJOIX)*0.3));
	incx=((OJODX+OJOIX)>>1);

	BYTE *p=pbEllipticalMask;
	//Recorremos toda la imagen rectangular
	for (i=0;i<iSy;i++)
		for(j=0;j<iSx;j++,p++){
				//Trasladamos con respecto al centro de la elipse
				py=i-incy;
				px=j-incx;
				//Sustuimos en la fórmula de la elipse verificando que no es interior
				fval=((float)px*((float)px))/(sa)+((float)py*((float)py))/(la);
				if (fval>=1.0) 
					*p=125;
				else
					*p=0;
		}
}



/*******************************************************************************


				Utilities for Warping images


********************************************************************************/

void CWarpingImage::resample_rows( BYTE * INi, BYTE * OUTi, int INlen, int OUTlen)
{
    int u, x;
    double acc, intensity=0, INSFAC, SIZFAC, INSEG, OUTSEG;

    SIZFAC= (double) OUTlen / (double) INlen;
    INSFAC= 1.0 / SIZFAC;
    OUTSEG= INSFAC;
    INSEG= 1.0;
    acc= 0.0;

    for(x=u=0; x<OUTlen;) {
		if (u+1<INlen) //añadido por modes
			intensity= (INSEG*(INi[u])) + ((1.0-INSEG)*(INi[u+1]));

        if(INSEG < OUTSEG) {
            acc += (intensity*INSEG);
            OUTSEG -= INSEG;
            INSEG= 1.0;         
            u++;
        }

        else {
            acc += (intensity*OUTSEG);
            OUTi[x] = (BYTE) (acc * SIZFAC);
            acc = 0.0;

            INSEG -= OUTSEG;
            OUTSEG= INSFAC;
            x++;
        }

    }

}

void CWarpingImage::resample_columns( BYTE * INi, BYTE * OUTi, int INlen, int OUTlen,
                        int INjmp, int OUTjmp)
{
    int u, x, ujmp, xjmp;
    double acc, intensity=0, INSFAC, SIZFAC, INSEG, OUTSEG;

    SIZFAC= (double) OUTlen / (double) INlen;    
    INSFAC= 1.0 / SIZFAC;
    OUTSEG= INSFAC;
    INSEG= 1.0;
    acc= 0.0;

    for(x=u=ujmp=xjmp=0; x<OUTlen,ujmp<INlen*INjmp;) {
		intensity= (INSEG*INi[ujmp]);
		//Modificación para evitar que se salga del límite realizada por Modes
		if (ujmp+INjmp<INlen*INjmp)
				intensity+= ((1.0-INSEG)*INi[ujmp+INjmp]);

        if(INSEG < OUTSEG) {
            acc += (intensity*INSEG);
            OUTSEG -= INSEG;
            INSEG= 1.0;
            u++;
            ujmp += INjmp;

        }

        else {
            acc += (intensity*OUTSEG);
            OUTi[xjmp] =(BYTE)(acc * SIZFAC);
            acc = 0.0;

            INSEG -= OUTSEG;         
             OUTSEG= INSFAC;
            x++;
            xjmp += OUTjmp;
        }
    }
	//Añadido Parece que no siempre pintaba última fila
	if (x<OUTlen && ujmp==INlen*INjmp)
	{
		//modificación ya que a veces no pinta última fila, añado if para no salirnos
		if (ujmp<INlen*INjmp)
			intensity = (INSEG*INi[ujmp]);
		//modificación para evitar que se salga del límite
		if (ujmp+INjmp<INlen*INjmp)
			intensity+= ((1.0-INSEG)*INi[ujmp+INjmp]);

        if(INSEG < OUTSEG) {
            acc += (intensity*INSEG);
            OUTSEG -= INSEG;
            INSEG= 1.0;
            u++;
            ujmp += INjmp;

        }

        else {
            acc += (intensity*OUTSEG);
            OUTi[xjmp] =(BYTE)(acc * SIZFAC);
            acc = 0.0;

            INSEG -= OUTSEG;         
             OUTSEG= INSFAC;
            x++;
            xjmp += OUTjmp;
        }
    }


}


/**************************************************************

	Transforma la imagen de entrada al tamaño instancia de la clase, resultado sobre pbImage (de la clase)
	Adaptado por Jorge Cabrera a partir de Digital Image Warping, G. Wolberg

  *************************************************************/
void CWarpingImage::Remap(BYTE *pbInImage,int sx,int sy)
{
    BYTE * aux;
    int row, col;

	aux=(unsigned char *) new unsigned char[sy*iSx];


    for(row=0; row < sy; row++) {
        resample_rows((BYTE *)(pbInImage+row*sx),
                      (BYTE*)(aux+row*iSx),
                      sx, iSx);
    }


	for(col=0; col < iSx; col++) {
        resample_columns((BYTE *)(aux+col),
                 (BYTE*)(pbImage+col),
                 sy, iSy,
                 iSx, iSx);
    }

    delete [] aux;
}


