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
 #if ! defined ( ENCARA2_2DEFS_H )
#define ENCARA2_2DEFS_H

//USEFUL VALUES
#define PI 3.1415926535897932
#define PI2 PI/2.0
#define PI32 3.0*PI2

#define MAX_PATH	260

#define ID_STRING_SIZE 256				// tamaño de string para identidad

//OpenCV and ipp/ipl includes
#ifdef USA_IPP
	#include <ipps.h>
	#include <ippi.h>
	#include "decoder.h"
#else
	//#include "ipl.h"
#endif

#ifndef ENCARA2INLINUX
	#include "windows.h" //Using CreateDirectory
#endif

//OPENCV
#ifndef OPENCVINCLUDED
#include "opencv/cv.h"  //añadido opencv/ a partir 2.3.1
#include "opencv/cvaux.h"  //añadido opencv/ a partir 2.3.1

#ifndef     USES_OF
#include "opencv/highgui.h"  //añadido opencv/ a partir 2.3.1
#endif
#ifndef ENCARA2INLINUX
	#include "opencv/highgui.h" //Used in windows //añadido opencv/ a partir 2.3.1
#endif
#endif
#define OPENCVINCLUDED

#include "math.h"

//Face image normalization options
#define ELLIPSEMASK						true	//Indicates if an elliptical masl is applied to the normalized image

//===========================================================================================================
// TORSO HISTOGRAM COMPARISON THRESHOLD
#define UMBRAL_COMPARACION_TORSO_HISTOGRAMAS	1.1	// JeffreyDivergence using NUM_BINS=32

//Not yet tested
#define UMBRAL_COMPARACION_FACE_HISTOGRAMAS	0.07	// usin¡g distance L1
//===========================================================================================================

//Normalized face sizes
//standard
#define FACEX ((int)(59))
#define FACEY ((int)(65))
#define FACEXBIG ((int)(FACEX*1.7))//aproximadamente *1.7 para obtener una imagen mayor con lso ojos a casi 45 píxeles
#define FACEYBIG ((int)(FACEY*1.7))
//Enclosing local context
#define FACEXCONTEXT ((int)(FACEX+OFFSETXCONTEXT*2))
#define FACEYCONTEXT ((int)(FACEY+OFFSETYCONTEXT))

#define FACEXCONTEXTHS ((int)(FACEX+OFFSETXCONTEXTHS*2))
#define FACEYCONTEXTHS ((int)(FACEY+OFFSETYCONTEXTHS*2))

//Tamaño zona central de la cara
#define FACEQUOREX ((int)(40))
#define FACEQUOREY ((int)(40))

//Tamaño cara reducida (para reducir coste en cómputo operadores Harr)
#define FACEQUOREDUCX ((int)(20))
#define FACEQUOREDUCY ((int)(20))

//Normalized positions for facial elements
#define RIGHTEYEX		42
#define RIGHTEYEY		17
#define LEFTEYEX		16
#define LEFTEYEY		17
#define MOUTHX			30
#define MOUTHY			53 //Boca

//Biger size
#define OFFSETXCONTEXT	 8
#define OFFSETYCONTEXT	25
#define RIGHTEYEXBIG	((int)(RIGHTEYEX*1.7)) 
#define RIGHTEYEYBIG	((int)(RIGHTEYEY*1.7)) 
#define LEFTEYEXBIG		((int)(LEFTEYEX*1.7)) 
#define LEFTEYEYBIG		((int)(LEFTEYEY*1.7)) 

//Containing head and shoulders size
#define OFFSETXCONTEXTHS	50
#define OFFSETYCONTEXTHS	45
#define RIGHTEYEXBIGHS	RIGHTEYEX+OFFSETXCONTEXTHS 
#define RIGHTEYEYBIGHS	RIGHTEYEY+OFFSETYCONTEXTHS
#define LEFTEYEXBIGHS	LEFTEYEX+OFFSETXCONTEXTHS
#define LEFTEYEYBIGHS	LEFTEYEY+OFFSETYCONTEXTHS

//Positions to crop given the normalized face (FACEX*FACEY)
#define GLASSESX		2
#define GLASSESY		6
#define GLASSESSX		FACEX-GLASSESX*2
#define GLASSESSY		25					//It is symmetric
#define MOUSTACHEX		8
#define MOUSTACHEY		32
#define MOUSTACHESX		FACEX-MOUSTACHEX*2
#define MOUSTACHESY		24



//BLOBS
//Maximum number of blobs to manage and min area
#define MINAREA		800
#define NBLOBMAX	5

//FACIAL GEOMETRY
#define RATIOEJESELIPSEMIN	1.5
#define RATIOEJESELIPSEMAX	2.0

#define MINIMALINTEREYE	15

//PROJECTIONS
#define	MAXNUMPROYECCIONES	50

//COLOR DETECTION (used for ENCARA first version based only on color)

//Skin color sets
enum JuegosColores
{
	ConjuntoA,
	ConjuntoB
};

//Skin color definitions
//ConjuntoA
#define	RNINFmA  0.39
#define RNSUPmA  0.48
#define	GNINFmA  0.30
#define GNSUPmA  0.335

//ConjuntoB
#define	RNINFmB  0.39
#define RNSUPmB  0.48
#define	GNINFmB  0.30
#define GNSUPmB  0.335

//RETURNED VALUES
//Labels assigned by manual annotation
#define	POSIBLEFRONTAL	0
#define	FACERIGHT		1
#define	FACELEFT		2
#define NOFACE			8

#define FRONTAL			110		//Frontal pero obtenida con tonos de grises


//Detection error returned
#define ALGOLATERALDER	10
#define ALGOLATERALIZQ	20
#define MASLATERALDER	11
#define MASLATERALIZQ	21
#define NOHORIZONTALES	31
#define OJIJUNTO		40
#define NOCOHERENTE		47
#define OJOSNOPARECIDOS	48
#define	APARIENCIARARA	49
#define NOCOHERENTEPREVIA	50

#define	SMALL			32

#define NOBLOBLOCATED	80

//TRACKING
//Values returned
#define LEFT_LOST			-6
#define RIGHT_LOST			-5
#define BOTH_LOST			-4
#define LEFTNOCOHERENT		-3
#define RIGHTNOCOHERENT		-2
#define BOTHNOCOHERENT		-1
#define	NO_OK				-1
#define OK					1

//  SizeMask must be 8*n
#define MY_ABS(a) (((a)^((a)>>31))-((a)>>31))


//APPEARANCE
//APARIENCIA caras con PCA+SVM
#define APPCARANUMCOEFS		200
#define APPOJONUMCOEFS		25
#define APPCARANUMCLASES	2
#define APPNITERPCACARA		300
#define APPNITERPCAOJOS		105

#define AUTOOJOSX			11
#define AUTOOJOSY			11
#define UMBRALSUAVE			26//Umbral ojos
#define UMBRALEXTRASUAVE	31
#define FACETHRESHOLD		580


//Viola based detectors identifiers
#define OPENCV_FACE			10

#define OWN_LEYE			50
#define OWN_REYE			51
#define OWN_BOTHEYES		60
#define OWN_NOSE			70
#define OWN_MOUTH			80
#define OWN_HS				90


#endif
