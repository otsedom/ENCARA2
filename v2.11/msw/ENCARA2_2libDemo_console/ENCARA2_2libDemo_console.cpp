// ENCARA2_2libDemo_console.cpp: define el punto de entrada de la aplicación de consola.
//

#include "stdafx.h"

#include "opencv/cv.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/highgui/highgui_c.h"

//lectura de ficheros de carpetas
#include <direct.h>
#include <io.h>

//Using ENCARA2_2lib
#include "ENCARA2_2lib.h"

#include <iostream>
#include <sstream>
#include <iomanip>

using namespace cv;
using namespace std;


void Process(Mat frame,CENCARA2_2Detector *ENCARAFaceDetector)
{	
	//Face dectection
	if (ENCARAFaceDetector)
	{
		if (ENCARAFaceDetector->boDetectorAvailable)
			{
				//Work image
				IplImage h=frame;
				
				IplImage *imagen = cvCloneImage(&h);

				//An image got from an AVI is inverted
				if (imagen->origin==1)
				{
					cvFlip(imagen);
				}

				//Process the image
				//The last parameter indicates the resolution to use
				ENCARAFaceDetector->ApplyENCARA2(imagen,2);

				//Paints results
				IplImage iframe=frame;
				ENCARAFaceDetector->PaintFacialData(&iframe,CV_RGB(0,255,0));
				
				//Liberamos imagen temporal
				cvReleaseImage(&imagen);
			}
		}

}


void LoadFaceDetector(Mat frame,CENCARA2_2Detector **ENCARAFaceDetector)
{
	char ENCARAdataDir[256];
	char auxchar[256];
		
	FILE *fdata=fopen("ENCARAdataconfig.txt","r");

	//If the config file exists, the path it taken from that file
	if (fdata!=NULL)
	{
		bool boprimera=true;
		strcpy(ENCARAdataDir,"");

		//Carga la ruta de forma reiterativa si hubiera espacios intermedios
		while (fscanf(fdata,"%s",auxchar)!=EOF)
		{
			if (!boprimera) strcat(ENCARAdataDir," ");//Está partida por un espacio a partir de la segunda
			strcat(ENCARAdataDir,auxchar);
			boprimera=false;
		}
		fscanf(fdata,"%s",ENCARAdataDir);
		fclose(fdata);
	}
	else
	{
		//Gets the environment variable
		char *ENCARA2=getenv("ENCARA2");

		if (ENCARA2==NULL)
		{
			//ENCARA2.2 data directory (to be modified according to your configuration)
			sprintf(ENCARAdataDir,"D:\\MyFolder\\ENCARA2data");
		}
		else
		{
			//ENCARA2.2 data directory using the environment variable
			sprintf(ENCARAdataDir,"%s\\ENCARA2data",ENCARA2);
		}
	}
		
	//ENCARA2 creation
	*ENCARAFaceDetector=new CENCARA2_2Detector(ENCARAdataDir,frame.size().width,frame.size().height);
	
}


void help()
{
	// print help message
	cout << "TestDetector <file(.avi | cam)>" << endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	string ruta;
	
	//Input arguments check
	if (argc < 2)
	{
		help();
		ruta.assign("cam");
	}
	else
		ruta.assign(argv[1]);

	
	//ENCARA2
	CENCARA2_2Detector *ENCARAFaceDetector=NULL;
	//Facial data access
	CFacialDataperImage	*FacialData;	
		
	//Variables
	VideoCapture capture;
    Mat frame;

	//NUmber of processed frames
	int nframe=0;
	
	//Procesa según el tipo de fichero fuente proporcionada
	if (!ruta.compare("cam"))//Capturamos de la cámara
	{
		capture.open(0); //Abre la cámara

		if (!capture.isOpened()) 
		{
			cout << "Fallo en la cámara\n" << endl;			
		}
		else
		{
			bool boimagen=true;
			//Procesa mientras no pulse Esc
			while (boimagen)
			{
				capture >> frame;
				
				if( frame.empty() )
					boimagen=false;
				else
				{
					//Load face detector with first valid frame
					if (nframe==0)
					{
						LoadFaceDetector(frame, &ENCARAFaceDetector);
					}
					//Process
					Process( frame, ENCARAFaceDetector );

					//Accessing facial data info
					FacialData=ENCARAFaceDetector->GetFacialData();
					//Any face detected
					if (FacialData->NumFaces)
					{
						//First face data is written
						printf("Centro cara %d %d\n",(FacialData->Faces[0]->x1+FacialData->Faces[0]->x2),(FacialData->Faces[0]->y1+FacialData->Faces[0]->y2));
					}

					//Visualización resultado
					imshow("Detecciones",frame);

					if( waitKey( 2 ) >= 0 )
						boimagen=false;

					frame.release();	
					nframe++;
				}							
			}
			capture.release();
		}
	}
	else
	{		
		if (ruta.find(".avi")!=string::npos) //Ficheros avi
		{
			capture.open(ruta);

			if( !capture.isOpened() )
                cout << "Fallo en el fichero avi" << endl;
			else
			{				
				bool boimagen=true;
				//Pocesa mientras haya imagen
				while (boimagen)
				{
					capture >> frame;
					if( frame.empty() )
						boimagen=false;
					else
					{
						//Load face detector with first valid frame
						if (nframe==0)
						{
							LoadFaceDetector(frame, &ENCARAFaceDetector);
						}
						//Process
						Process( frame, ENCARAFaceDetector );

						imshow("Detecciones",frame);

						if( waitKey( 2 ) >= 0 )
							boimagen=false;

						frame.release();	
						nframe++;
					}										
				}
				capture.release();
			}
		}	
	}

	waitKey(10);	
	
	//Cierre de ventanas
	cvDestroyWindow("Detecciones");	

	//Dellocating detector
	if (ENCARAFaceDetector)
	{
		delete ENCARAFaceDetector;
		ENCARAFaceDetector=NULL;
	}

	return 0;
}

