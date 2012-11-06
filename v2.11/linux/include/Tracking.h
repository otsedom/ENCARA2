/**
 ** The ENCARA2 project
 ** Copyright (C) 2003-2011
 **    Universidad de Las Palmas de Gran Canaria (ULPGC) (<http://www.ulpgc.es>)
 **    
 **    This software is a project which has been designed, developed and
 **    written, and is maintained by members, students and collaborators of the Divisi�n
 **    de Inteligencia Artificial y Sistemas (DIAS) at the Instituto Universitario de 
 **    Sistemas Inteligentes y Aplicaciones Num�ricas en Ingenier�a (SIANI) 
 **    (<http://www.siani.es/>), and also of the Departamento de Inform�tica y Sistemas 
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
// The Tracking function, H file
// Juan Mendez
// CeTSIA, ULPGC
// jmendez@dis.ulpgc.es
// Oct-2000
// +++++++++++++++++++++++++++++++++++++++++++++++


void Tracking_Mod(unsigned char*,int,int,int,int,int,int,unsigned char*,
			  int,int*,int*,int*,int*);
bool Similarity_Mod(int , int, int, unsigned char*, int, int, int, bool* , unsigned char**);


void MX2_AbsSum24_Mod(int*,unsigned char*,unsigned char*,int,int//		printf("val=%d, x=%d, y=%d\n", val,pos_x,pos_y);
);
void MX2_AbsSum16_Mod(	int*			Result, 
					unsigned char*	Data, 
					unsigned char*	Mask,
					int				SizeMask, // do not uses it
					int				SizeData);


void C_AbsSum_Mod(	int*			Result, 
				unsigned char*	 Data, 
				unsigned char*	 Mask, 
				int				SizeMask, 
				int				Size);
