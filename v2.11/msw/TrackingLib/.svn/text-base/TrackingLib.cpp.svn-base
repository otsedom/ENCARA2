/*************************************/
/*

	     Tracking Library, 2004
		
			Cayetano Guerra

			Modificada por coherencia y sentido común por Modesto Castrillón Octubre 2004

*/
/*************************************/

#include <stdio.h>
#include "OpenTracking.h"
#include "cv.h"



void C_AbsSum24M(	int*			Result, 
				unsigned char*	 Data, 
				unsigned char*	 Mask, 
				int				SizeMask, 
				int				Size);



/*************************************************
	CTracking: Constructor de la clase

	*ping : Puntero a la imagen (espera una imagen BGR)
	ix, iy : Posición central del objeto a seguir
	iROIx, iROIy : Ancho y alto de la región de interés
	iPatx, iPaty : Ancho y alto del patrón (posibles problemas si no es cuadrado, dado que TrackingMMX lo asume)
	
**************************************************/

COpenTracking::COpenTracking(IplImage *pimg, int ix, int iy, int iROI_x, int iROI_y, int iPatternSize_x, int iPatternSize_y)
{
	//No hay memoria alojada a priori
	boimg_aux=false;			//No hay estructuras auxiliares almacenadas
	boBuffer=false;				//No hay ventana almacenada

	image_width=image_height=-1;//No aux images allocated

	//En principio no se ha perdido
	boLost = false;
	Reliability=1.f;

	boInit = true;

	//Tamaño del patrón
	Patx = iPatternSize_x;
	Paty = iPatternSize_y;

	//Aloja estructuras de memoria
	//Patrón a seguir
	pbPattern = (unsigned char *)new unsigned char[sizeof(unsigned char)*Patx*Paty];


	SetPatternandROI(pimg,ix,iy,iROI_x,iROI_x);

	//Tamaño de la ventana de búsqueda
	/*ROIx = iROI_x;
	ROIy = iROI_y;

	//La zona de interés debe ser mayor que el patrón
	while (ROIx<=Patx)
		ROIx+=8;
	while (ROIy<=Paty)
		ROIy+=8;
	
	//Resultado del tamaño relacionado con la zona de busqueda y el del patrón
	buffer_width=(ROIx-Patx);
	buffer_height=(ROIy-Paty);
	if (boBuffer)
		delete piBuffer
	piBuffer = (int *)new unsigned char [sizeof(unsigned int)*buffer_width*buffer_height];
	boBuffer=true;

	//Llama a SetPattern para copiar el patrón y arrancar el tracker
	SetPattern(pimg,ix,iy);*/
	
}

void COpenTracking::FreeAux()
{
	if (boimg_aux)
	{
		delete [] pbImageAux;
		cvReleaseImage(&pimg_aux);
		boimg_aux=false;
	}
}

void COpenTracking::AllocAux()
{
	FreeAux();

	//Auxiliar
	pbImageAux = (unsigned char *) new unsigned char [sizeof(unsigned char)*image_width*image_height];
	pimg_aux = cvCreateImage(cvSize(image_width,image_height), IPL_DEPTH_8U, 3);
	boimg_aux=true;

}

COpenTracking::~COpenTracking()
{
	FreeAux();

	delete [] pbPattern;
	if (boBuffer)
	{
		delete [] piBuffer;
		boBuffer=false;
	}
	
}

/*! 
	Returns the seacrh are for the current configuration
	
	\param esqx x coordinate of the left upper corner
	\param esqy y coordinate of the left upper corner
	\param sx Search area width
	\param sy Search area height
	*/
void COpenTracking::GetSearchArea(int *esqx,int *esqy, int *sx, int *sy)
{
	//En base a la última detección
	*esqx=x-(ROIx>>1);
	*esqy=y-(ROIy>>1);
	*sx=ROIx;
	*sy=ROIy;

	//Controla límites
	FitROI(esqx,esqy,sx,sy);
}


void COpenTracking::Track(IplImage *pimg)
{
	bool cen=false;

	int i;

	//Realloc images if necessary due to size change
	if (pimg->width!=image_width || pimg->height!=image_height)
	{
		image_width=pimg->width;
		image_height=pimg->height;
		AllocAux();
	}

	//Convierte la imagen a un espacio similar al del patrón
	cvCvtColor( pimg, pimg_aux, CV_BGR2YCrCb );
	
	//Copia el primer byte de cada píxel () de la imagen donde buscar
	BYTE *p=pbImageAux;
	BYTE *src=(unsigned char *)pimg_aux->imageData,*psrc;
	for (i=0;i<image_height;i++,src+=pimg_aux->widthStep)
	{
		psrc=src;
		for (int j=0;j<image_width;j++,p++,psrc+=3)
			*p=*psrc;
	}

	int esqx,esqy,sx,sy;
	GetSearchArea(&esqx,&esqy,&sx,&sy);


	//Si cabe lo que buscamos y donde buscamos en la imagen
	if (sx<=image_width && sy<=image_height && Patx<sx && Paty<sy)
	{
	
		TrackingMMX(pbImageAux,
				image_width,
				image_height,
				esqx,
				esqy,
				sx,
				sy,
				pbPattern,
				Patx,
				piBuffer,
				&x,
				&y,
				&iValue);

		Min4Pattern=iValue;//Min of the first search

		Reliability=1.f -( ( (float)iValue)/( (float)threshold));


		//Obtiene el segundo mínimo en el contexto para estimar si se actualiza el umbral
		SecondMin(piBuffer,sx,sy);

		if (!threshold)//Acaba de establecer el patrón, ajusta el primer umbral en base al segundo mínimo
		{
			threshold = iSecondMin>>1;

			Reliability=1.f -( ( (float)iValue)/( (float)threshold));
		}
		else//No estamos empezando con el patrón
		{
			//Pérdida, el patrón es muy superior
			if (iValue>(threshold<<1)) 
				boLost = true; // Una posible condición de pérdida pero seguir actualizando y buscando

			//Condiciones para actualizar el patrón
			if ((iValue>=threshold)||(iSecondMin<threshold))
			{	
				//Actualiza el patrón
				CopyPattern(pimg);
			
				//Antes debemos chequear si la ventana de búsqueda se sale de la imagen para poder seguir en el borde
				//Obliga a que el ancho de l aventana sea mútiplo de 8
				
				int esqx,esqy,sx,sy;

				GetSearchArea(&esqx,&esqy,&sx,&sy);

				//Obtiene el segundo mínimo con el nuevo patrón
				TrackingMMX(pbImageAux,
							image_width,
							image_height,
							esqx,
							esqy,
							sx,
							sy,
							pbPattern,
							Patx,
							piBuffer,
							&x,
							&y,
							&iValue);


				SecondMin(piBuffer,sx,sy);

				threshold = iSecondMin>>1;

				Reliability=1.f -( ( (float)iValue)/( (float)threshold));
			}	
		}	
	}
	else
		boLost = true;
}


/*************************************************
	SetPattern: Establece el patrón

	*pimg : Puntero a la imagen
	ix, iy : Posición cntral del objeto a seguir
	
**************************************************/

void COpenTracking::SetPatternandROI(IplImage *pimg, int ix, int iy, int iROI_x,int iROI_y)
{
	//Tamaño de la ventana de búsqueda
	ROIx = iROI_x;
	ROIy = iROI_y;

	//La zona de interés debe ser mayor que el patrón
	while (ROIx<=Patx)
		ROIx+=8;
	while (ROIy<=Paty)
		ROIy+=8;
	
	//Resultado del tamaño relacionado con la zona de busqueda y el del patrón
	buffer_width=(ROIx-Patx);
	buffer_height=(ROIy-Paty);

	if (boBuffer)
		delete [] piBuffer;

	piBuffer = (int *)new unsigned char [sizeof(unsigned int)*buffer_width*buffer_height];
	boBuffer=true;

	//Llama a SetPattern para copiar el patrón y arrancar el tracker
	SetPattern(pimg,ix,iy);
}


/*************************************************
	SetPattern: Establece el patrón

	*pimg : Puntero a la imagen
	ix, iy : Posición central del objeto a seguir
	
**************************************************/

void COpenTracking::SetPattern(IplImage *pimg, int ix, int iy)
{
	//Posición del objetivo
	x = ix;
	y = iy;

	//Realloc images if necessary due to size change
	if (pimg->width!=image_width || pimg->height!=image_height || !boimg_aux)
	{
		//Tamaño de la imagen donde buscar, se utiliza para crear estructuras auxiliares
		image_width=pimg->width;
		image_height=pimg->height;
		AllocAux();
	}

	CopyPattern(pimg);

	//Initializes the threshold for this new pattern according to the context
	threshold = 0;

	Track(pimg);	
	
}



// Devuelve el segundo mínimo de la búsqueda

void COpenTracking::SecondMin(int *buffer, int sx,int sy)
{
	int i,j;
	int pitchX = (sx-Patx);
	int pitchY = (sy-Paty);
	int current;
	int min1, min2;
	int *piResult;

	piResult = (int *)new unsigned char[sizeof(int)*(sx-Patx)*(sy-Paty)];
	if (piResult==NULL)
	{
		printf("No hay memoria suficiente");
	}
	else
	{
		for (j=1;j<(pitchY-1);j++)
			for (i=1;i<(pitchX-1);i++)
			{
				current = buffer[j*pitchX+i];

				if ((buffer[(j-1)*pitchX+(i-1)] > current) &&
					(buffer[(j-1)*pitchX+i] > current) &&
					(buffer[(j-1)*pitchX+(i+1)] > current) &&
					(buffer[j*pitchX+(i-1)] > current) &&
					(buffer[j*pitchX+(i+1)] > current) &&
					(buffer[(j+1)*pitchX+(i-1)] > current) &&
					(buffer[(j+1)*pitchX+i] > current) &&
					(buffer[(j+1)*pitchX+(i+1)] > current)) 
				{
					piResult[j*pitchX+i] = current; //0xFF; 
				}
				else
				{
					piResult[j*pitchX+i] = -1;
				}
			}

		min1=147000; // first local minimum 
		min2=148000; // second local minimum

		for (j=1;j<(pitchY-1);j++)
			for (i=1;i<(pitchX-1);i++)
			{
				if ((piResult[j*pitchX+i] < min1) && (piResult[j*pitchX+i]!=-1))
				{
					min2=min1;
					min1=piResult[j*pitchX+i];
				} else if ((piResult[j*pitchX+i] < min2) && (piResult[j*pitchX+i]!=-1))
					min2=piResult[j*pitchX+i];
			}

		iSecondMin = min2;
	}
	delete [] piResult;

}

//Obliga a que el ancho y alto de la ventana de búsqueda estén dentro de la imagen y sean mútiplos de 8
void COpenTracking::FitROI(int *esqx,int *esqy, int *sx, int *sy)
{
	int aux;

	if (*esqx<0)
	{
		aux= (*sx + *esqx)/8;// al ser negativo se reduce el tamaño
		*sx=aux*8;
		*esqx=0;
	}
	if ( *esqx+ *sx>=image_width)
	{
		aux=(image_width - *esqx-1)/8;
		*sx=aux*8;
	}
	if ( *esqy<0)
	{
		int aux= ( *sy + *esqy)/8;
		*sy=aux*8;
		*esqy=0;
	}
	if (*esqy + *sy>=image_height)
	{
		aux=( image_height- *esqy-1)/8;
		*sy=aux*8;
	}

	//La ventana de búsqueda debe ser mayor que el patrón
	while (*sx<=Patx)
		*sx+=8;

	//Pero hace falta vigilar que no se salga tras esta corrección
	if ( *esqx+ *sx>=image_width)
	{
		*esqx=(image_width - *sx);
	}


	while (*sy<=Paty)
		*sy+=8;

	//Pero hace falta vigilar que no se salga tras esta corrección
	if ( *esqy+ *sy>=image_height)
	{
		*esqy=(image_height - *sy);
	}

	//Crea de nuevo la ventana si no coinciden los tamaños
	if (*sx!=buffer_width-Patx || *sy!=buffer_height-Paty)
	{
		if (boBuffer)
			delete [] piBuffer;

		//Reajusta dimensiones
		buffer_width= *sx-Patx;
		buffer_height= *sy-Paty;

		if (buffer_width>0 && buffer_height>0)
			piBuffer = (int *)new unsigned char [sizeof(unsigned int)*buffer_width*buffer_height];
		else
		{
			boLost=true;
			boBuffer=false;
			return;
		}
	}
}

//Copia el patrón
void COpenTracking::CopyPattern(IplImage *pimg)
{
	int i,j;
	int cx,cy;

	//Nuevo de Modesto (modifica la x e y si el patrón se saliera de la imagen)
	//Modificado por Modesto para garantizar que el patrón se tomará dentro de la imagen
	if ((x+(Patx>>1))>=image_width) x=image_width-(Patx>>1)-1;
	if ((x-(Patx>>1))<0) x=(Patx>>1);

	if ((y+(Paty>>1))>=image_height) y=image_height-(Paty>>1)-1;
	if ((y-(Paty>>1))<0) y=(Paty>>1);

	cvCvtColor( pimg, pimg_aux, CV_BGR2YCrCb );

	// Centrado

	// Dada la coordenada central del patrón, desplaza hacia la esquina superior derecha
	cx=x-(Patx>>1);
	cy=y-(Paty>>1);

	//Copia el patrón
	for (j=0;j<Paty;j++)
		for (i=0;i<Patx;i++)
		{
			pbPattern[j*Patx+i] = pimg_aux->imageData[(cx+i)*pimg_aux->nChannels+(cy+j)*pimg_aux->widthStep];
		}
}

IplImage* COpenTracking::GetPattern()
{
	int i,j;
	IplImage *iplPat=cvCreateImage( cvSize(Patx,Paty),8,1);

	//Copia el patrón
	for (j=0;j<Paty;j++)
		for (i=0;i<Patx;i++)
		{
			 iplPat->imageData[i*iplPat->nChannels+j*iplPat->widthStep] = pbPattern[j*Patx+i];
		}

	return iplPat;


}

int COpenTracking::GetLastMin()
{
	return Min4Pattern;
}



// +++++++++++++++++++++++++++++++++++++++++++++++
// The Tracking function, C file
// Juan Mendez
// CeTSIA, ULPGC
// jmendez@dis.ulpgc.es
// Oct-2000
// +++++++++++++++++++++++++++++++++++++++++++++++


#include "string.h"
#include <stdlib.h> 

//Asume patrones cuadrados a buscar
void TrackingMMX(	unsigned char*	Image,
				int				SizeImageX,
				int				SizeImageY,
				int				PosRoiX,
				int				PosRoiY,
				int				SizeRoiX,
				int				SizeRoiY,
				unsigned char*	Mask,
				int				SizeMask,
				int*			Buffer,
				int*			TargetX,
				int*			TargetY,
				int*			Value	){


	int	EfecSizeRoiX;
	int EfecSizeRoiY;
	int	BaseRoi;

	int	Arrow;
	int MaskArrow;
	int	Min;
	int IndexMin;
	int	Index;
	int SizeBuffer;
	int	IndexX;
	int	IndexY;

	EfecSizeRoiX = SizeRoiX-SizeMask;
	EfecSizeRoiY = SizeRoiY-SizeMask;
	BaseRoi = PosRoiY*SizeImageX + PosRoiX;

	SizeBuffer = EfecSizeRoiX*EfecSizeRoiY;
	memset((char*)Buffer,0,SizeBuffer*sizeof(int));


	for(MaskArrow=0;MaskArrow<SizeMask;MaskArrow++){
		for(Arrow=0;Arrow<EfecSizeRoiY;Arrow++){

#ifdef ENCARA2INLINUX
			C_AbsSum24M(	&Buffer[Arrow*EfecSizeRoiX],
							&Image[BaseRoi+(Arrow+MaskArrow)*SizeImageX],
							&Mask[MaskArrow*SizeMask],
							SizeMask,
							EfecSizeRoiX);
#else
			MX2_AbsSum24(	&Buffer[Arrow*EfecSizeRoiX],
							&Image[BaseRoi+(Arrow+MaskArrow)*SizeImageX],
							&Mask[MaskArrow*SizeMask],
							SizeMask,
							EfecSizeRoiX);
#endif
			
		}
	}

#ifndef ENCARA2INLINUX
	_asm emms			// only for MMX
#endif //ENCARA2INLINUX

	Min = Buffer[0];
	IndexMin = 0;
	for(Index=1;Index<SizeBuffer;Index++){

		if(Buffer[Index] < Min){
			Min = Buffer[Index];
			IndexMin = Index;
		}
	}
	if (EfecSizeRoiX)
	{
		IndexY = IndexMin / EfecSizeRoiX;
		IndexX = IndexMin % EfecSizeRoiX;
	}
	else//Añadido por modest
	{
		IndexY=IndexX=0;
	}
	

	*TargetY = PosRoiY + (SizeMask>>1) + IndexY;
	*TargetX = PosRoiX + (SizeMask>>1) + IndexX;
	*Value   = Min/*/(SizeMask*SizeMask)*/; 
	return;
}

// 24 bytes Mask, missalineated access
void MX2_AbsSum24(	int*			Result, 
					unsigned char*	Data, 
					unsigned char*	Mask,
					int				SizeMask, // do not uses it
					int				SizeData){

#ifndef ENCARA2INLINUX
	_asm{
		mov		esi, Result
		mov		ecx, SizeData
		mov		edi, Mask
		mov		eax, Data

		movq	mm4, qword ptr [edi]	
		movq	mm5, qword ptr [edi+8]
		movq	mm6, qword ptr [edi+16]

		movq	mm0, qword ptr [eax]	// U

data_loop:

		movq	mm1, qword ptr [eax+8]	// U
		psadbw	mm0, mm4				// V				

		movq	mm2, qword ptr [eax+16]	// U
		psadbw	mm1, mm5				// V

		movd	mm7, dword ptr [esi]	// U 
		add		eax, 1					// V

		psadbw	mm2, mm6				// U
		paddusw	mm1, mm0				// V

		movq	mm0, qword ptr [eax]	// U
		paddusw	mm2, mm1				// V

		paddd	mm7, mm2				// U
		add		esi, 4					// V

		movd	dword ptr [esi-4], mm7	// U (2 ciclos)
		dec		ecx						// V

		jnz		data_loop				// U
	}
#endif //ENCARA2INLINUX
}

					//  SizeMask must be 8*n
#define MY_ABS(a) (((a)^((a)>>31))-((a)>>31))

void C_AbsSum24M(	int*			Result, 
				unsigned char*	 Data, 
				unsigned char*	 Mask, 
				int				SizeMask, 
				int				Size){

	int x, z;
	int Add;
	int  v[8];

	int cont=0;
	
	for(z=0;z<Size;z++){
		Add=0;
		for(x=0;x<SizeMask/8;x+=8){
			v[0] = (int)Data[z+x]-(int)Mask[x];
			v[1] = (int)Data[z+x+1]-(int)Mask[x+1];
			v[2] = (int)Data[z+x+2]-(int)Mask[x+2];
			v[3] = (int)Data[z+x+3]-(int)Mask[x+3];
			v[4] = (int)Data[z+x+4]-(int)Mask[x+4];
			v[5] = (int)Data[z+x+5]-(int)Mask[x+5];
			v[6] = (int)Data[z+x+6]-(int)Mask[x+6];
			v[7] = (int)Data[z+x+7]-(int)Mask[x+7];

			Add += MY_ABS(v[0]) + MY_ABS(v[1]) + MY_ABS(v[2]) + MY_ABS(v[3])+
				MY_ABS(v[4]) + MY_ABS(v[5]) + MY_ABS(v[6]) + MY_ABS(v[7]);

#ifdef VERIFYMYABS
			if (MY_ABS(v[0])!=abs(v[0]) || MY_ABS(v[1])!=abs(v[1]) || 
				MY_ABS(v[2])!=abs(v[2]) ||  MY_ABS(v[3])!=abs(v[3]) || 
				MY_ABS(v[4])!=abs(v[4]) ||  MY_ABS(v[5])!=abs(v[5]) || 
				MY_ABS(v[6]) !=abs(v[6]) ||  MY_ABS(v[7])!=abs(v[7]))

				TRACE("MY_ABS(v[0]) %d,abs(v[0]  %d,MY_ABS(v[1]) %d,abs(v[1] %d,MY_ABS(v[2]) %d,abs(v[2] %d, MY_ABS(v[3]) %d,abs(v[3] %d, MY_ABS(v[4]) %d,abs(v[4] %d,  MY_ABS(v[5]) %d,abs(v[5] %d,MY_ABS(v[6]) %d,abs(v[6] %d,  MY_ABS(v[7]) %d,abs(v[7] %d\n",
				MY_ABS(v[0]),abs(v[0]),MY_ABS(v[1]),abs(v[1]), 
				MY_ABS(v[2]),abs(v[2]), MY_ABS(v[3]),abs(v[3]), 
				MY_ABS(v[4]),abs(v[4]),  MY_ABS(v[5]),abs(v[5]), 
				MY_ABS(v[6]),abs(v[6]),  MY_ABS(v[7]),abs(v[7]) );

#endif
		}
		Result[z] += Add;
		cont++;
	}

//	TRACE("cont %d\n",cont);
}



bool Similarity(int number, 
				int cx, // Template size x
				int cy, // Template size y
				unsigned char *pimage, 
				int offset, 
				int pitch, 
				int threshold,
				bool *m_boTemplateFree,
				unsigned char **m_pTemplateBank
				) 
{
	int result;
	
	for (int n=0;n<number;n++)
	{
		if (!m_boTemplateFree[n])
		{
			result = 0;
			for (int y=0;y<cy;y++)
			{
				for (int x=0;x<cx;x++)
				{	
					result += abs(m_pTemplateBank[n][y*cx+x] - pimage[offset+x+y*pitch]);
				}
			}
			if (result < threshold) return true;
		}
	}
	return false;
}
