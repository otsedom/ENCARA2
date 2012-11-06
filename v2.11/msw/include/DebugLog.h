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
 #if ! defined ( DEBUGLOG_H )
#define DEBUGLOG_H


//! Log file class
/*!
	Class used to write logs during execution
*/
class CDebugLog
{
public:

	//! Class Constructor
	/*!
	\param cWorkDataDir			Indicates the location of the working dir
	\param	NameLog				Indicates the name of the log file, NULL means the default name is taken
	*/
	CDebugLog(char *cWorkDataDir, char *NameLog=NULL)
	{
		//Initializes and resets the log file
#ifndef ENCARA2INLINUX
		if (NameLog==NULL)
			sprintf(cPathLog,"%s\\Debuglog.txt",cWorkDataDir);
		else
			sprintf(cPathLog,"%s\\%s",cWorkDataDir,NameLog);
#else
		if (NameLog==NULL)
			sprintf(cPathLog,"%s/Debuglog.txt",cWorkDataDir);
		else
			sprintf(cPathLog,"%s/%s",cWorkDataDir,NameLog);
#endif

		FILE *fDepura=fopen(cPathLog,"w");
		if (fDepura) fclose(fDepura);
	}

	//! Class destructor
	~CDebugLog(){};

	//! Convenient function used to write a string to file
	/*!
	\param Logstr			String to write in log file
	*/
	void WriteToLogFile(char *Logstr)
	{
		FILE *fDepura=NULL;
		
		fDepura=fopen(cPathLog,"a+");

		if (fDepura)
		{
			fprintf(fDepura,"%s",Logstr);

			fclose(fDepura);
		}

	}

private:

	char	cPathLog[256];			//! Log file name

};

#endif
