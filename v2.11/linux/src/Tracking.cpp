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
 // +++++++++++++++++++++++++++++++++++++++++++++++
// The Tracking function, C file
// Juan Mendez
// CeTSIA, ULPGC
// jmendez@dis.ulpgc.es
// Oct-2000
// +++++++++++++++++++++++++++++++++++++++++++++++



#include "string.h"
#include <stdlib.h> 
#include "Tracking.h"



void Tracking_Mod(	unsigned char*	Image,
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
	//Si l aimagen es 5x5 y la máscara 3x3, hay 3 posiciones válidas, no 2
//	EfecSizeRoiX = SizeRoiX-(SizeMask-1);
//	EfecSizeRoiY = SizeRoiY-(SizeMask-1);
	BaseRoi = PosRoiY*SizeImageX + PosRoiX;

	SizeBuffer = EfecSizeRoiX*EfecSizeRoiY;

	
//	memset((char*)Buffer,0,SizeBuffer*sizeof(int));
//Posible basura en ocasiones
	memset((char*)Buffer,0,SizeRoiX*SizeRoiY*sizeof(int));

//	TRACE("SizeBuffer %d\n",SizeBuffer);


	for(MaskArrow=0;MaskArrow<SizeMask;MaskArrow++){
		for(Arrow=0;Arrow<EfecSizeRoiY;Arrow++){
			switch (SizeMask)
			{
			case 24:
#ifdef USAPIIIMMX
			MX2_AbsSum24_Mod(	&Buffer[Arrow*EfecSizeRoiX],
							&Image[BaseRoi+(Arrow+MaskArrow)*SizeImageX],
							&Mask[MaskArrow*SizeMask],
							SizeMask,
							EfecSizeRoiX);
#else
			C_AbsSum_Mod(	&Buffer[Arrow*EfecSizeRoiX],
							&Image[BaseRoi+(Arrow+MaskArrow)*SizeImageX],
							&Mask[MaskArrow*SizeMask],
							SizeMask,
							EfecSizeRoiX);
#endif
			break;
			case 16:

#ifdef USAPIIIMMX
			MX2_AbsSum16_Mod(	&Buffer[Arrow*EfecSizeRoiX],
							&Image[BaseRoi+(Arrow+MaskArrow)*SizeImageX],
							&Mask[MaskArrow*SizeMask],
							SizeMask,
							EfecSizeRoiX);
#else
			C_AbsSum_Mod(	&Buffer[Arrow*EfecSizeRoiX],
							&Image[BaseRoi+(Arrow+MaskArrow)*SizeImageX],
							&Mask[MaskArrow*SizeMask],
							SizeMask,
							EfecSizeRoiX);
#endif
			break;
			default:
				break;

			}
		}
	}

#ifdef USAPIIIMMX
	_asm emms			// only for MMX
#endif

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
	else
	{
		IndexY = 0;
		IndexX = 0;

	}

	*TargetY = PosRoiY + SizeMask/2 + IndexY;
	*TargetX = PosRoiX + SizeMask/2 + IndexX;
	*Value   = Min/*/(SizeMask*SizeMask)*/; 

	//Evitando problemas si se da una ventana de tamaño nulo
	if (SizeBuffer==0)
		*Value=100000;

	return;
}

// 24 bytes Mask, missalineated access
void MX2_AbsSum24_Mod(	int*			Result, 
					unsigned char*	Data, 
					unsigned char*	Mask,
					int				SizeMask, // do not uses it
					int				SizeData){
#ifdef USAPIIIMMX

	
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

	

#endif

}



// 16 bytes Mask, missalineated access
void MX2_AbsSum16_Mod(	int*			Result, 
					unsigned char*	Data, 
					unsigned char*	Mask,
					int				SizeMask, // do not uses it
					int				SizeData){

#ifdef USAPIIIMMX

	_asm{
		mov		esi, Result
		mov		ecx, SizeData
		mov		edi, Mask
		mov		eax, Data

		movq	mm4, qword ptr [edi]	
		movq	mm5, qword ptr [edi+8]

		movq	mm0, qword ptr [eax]	// U

data_loop:

		movq	mm1, qword ptr [eax+8]	// U
		psadbw	mm0, mm4				// V				

		movd	mm7, dword ptr [esi]	// U 
		psadbw	mm1, mm5				// V
		
		paddusw	mm1, mm0				// U
		add		eax, 1					// V

		movq	mm0, qword ptr [eax]	// U
		paddd	mm7, mm1				// V
		

		movd	dword ptr [esi], mm7	// U (2 ciclos)
		add		esi, 4					// V?
		
		dec		ecx						// U?

		jnz		data_loop				// U
	}
#endif
}


//  SizeMask must be 8*n
#define MY_ABS(a) (((a)^((a)>>31))-((a)>>31))

void C_AbsSum_Mod(	int*			Result, 
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


bool Similarity_Mod(int number, 
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


