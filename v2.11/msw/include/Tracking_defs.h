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
 #if ! defined ( TRACKINGDEFS_H )
#define TRACKINGDEFS_H



//Parámetros Tracking
#define TRACKINGCONPERDIDA
#define POSIBLECAMBIOPATRON
	//Si están muy cerca el mínimo y el segundo es sospechoso: por ejemplo ha parpadeado, si actualiza
	//el patrón pierde un buen patrón de iris


#define NUMPATTERNS						10
#define PATSIZEX						24
#define PATSIZEY						24
#define ROIX							80
#define ROIY							80
#define EYEPATTERN						16	//Tamaños de patrones
#define INBETWEENEYESPATTERN			16//24
#define EYEBRUSHPATTERN					16
#define MOUTHCORNERPATTERN				16
#define MOUTHPATTERN					16//24
#define NOSEHOLEPATTERN					16
#define NOSEPATTERN						16//24
#define TRACKINGEYECOHERENT				2200
#define TRACKINGMOUTHCORNERLOST			1800
#define TRACKINGMOUTHLOST				1800//3000
#define TRACKINGNOSEHOLELOST			1800
#define TRACKINGNOSELOST				1800//3000
#define TRACKINGINBETWEENEYELOST		1800//3000
#define TRACKINGINBETWEENEYECOHERENT	1800//3000

#ifdef DOBLEUMBRAL
#define TRACKINGEYELOST		5000
#else
#define TRACKINGEYELOST		2200
#endif

#define TRACKWINSIZEMIN					24 //Tamaño mínimo de la ventana de búsqueda para el tracking




#endif

