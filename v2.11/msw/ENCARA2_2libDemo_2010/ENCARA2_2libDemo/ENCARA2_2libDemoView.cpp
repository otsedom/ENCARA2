
// ENCARA2_2libDemoView.cpp: implementación de la clase CENCARA2_2libDemoView
//

#include "stdafx.h"
// Se pueden definir SHARED_HANDLERS en un proyecto ATL implementando controladores de vista previa, miniatura
// y filtro de búsqueda, y permiten compartir código de documentos con ese proyecto.
#ifndef SHARED_HANDLERS
#include "ENCARA2_2libDemo.h"
#endif

#include "ENCARA2_2libDemoDoc.h"
#include "ENCARA2_2libDemoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CENCARA2_2libDemoView

IMPLEMENT_DYNCREATE(CENCARA2_2libDemoView, CView)

BEGIN_MESSAGE_MAP(CENCARA2_2libDemoView, CView)
	// Comandos de impresión estándar
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CENCARA2_2libDemoView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_PROCESO_FICHERO, &CENCARA2_2libDemoView::OnProcesoFichero)
	ON_COMMAND(ID_PROCESO_C32771, &CENCARA2_2libDemoView::OnProcesoC32771)
	ON_COMMAND(ID_FILE_OPEN, &CENCARA2_2libDemoView::OnFileOpen)
END_MESSAGE_MAP()

// Construcción o destrucción de CENCARA2_2libDemoView

CENCARA2_2libDemoView::CENCARA2_2libDemoView()
{
	// TODO: agregar aquí el código de construcción
	ENCARAFaceDetector=NULL;
	
	m_bofileopen=false;
	m_boCapturando=false;
	m_boFirstFrame=false;
}

CENCARA2_2libDemoView::~CENCARA2_2libDemoView()
{
	Reset();	

	//ENCARA
	ReleaseFaceDetector();

	destroyAllWindows();
}

BOOL CENCARA2_2libDemoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: modificar aquí la clase Window o los estilos cambiando
	//  CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// Dibujo de CENCARA2_2libDemoView

void CENCARA2_2libDemoView::OnDraw(CDC* /*pDC*/)
{
	CENCARA2_2libDemoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: agregar aquí el código de dibujo para datos nativos
	if (m_boCapturando)
	{
		Mat frame;	
		bool bohayframe=true;;

		m_capture >> frame;

		//Invalid frame
		if (frame.empty())
			bohayframe=false;

		if (bohayframe)
		{
			//Load face detector with first valid frame
			if (m_boFirstFrame)
			{
				LoadFaceDetector(frame);
				m_boFirstFrame=false;
			}
				
			//Process frame
			Process(frame);

			imshow("Cam", frame);

			frame.release();

			Invalidate();
		}

	}
}


// Impresión de CENCARA2_2libDemoView


void CENCARA2_2libDemoView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CENCARA2_2libDemoView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Preparación predeterminada
	return DoPreparePrinting(pInfo);
}

void CENCARA2_2libDemoView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: agregar inicialización adicional antes de imprimir
}

void CENCARA2_2libDemoView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: agregar limpieza después de imprimir
}

void CENCARA2_2libDemoView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CENCARA2_2libDemoView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// Diagnósticos de CENCARA2_2libDemoView

#ifdef _DEBUG
void CENCARA2_2libDemoView::AssertValid() const
{
	CView::AssertValid();
}

void CENCARA2_2libDemoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CENCARA2_2libDemoDoc* CENCARA2_2libDemoView::GetDocument() const // La versión de no depuración es en línea
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CENCARA2_2libDemoDoc)));
	return (CENCARA2_2libDemoDoc*)m_pDocument;
}
#endif //_DEBUG


// Controladores de mensaje de CENCARA2_2libDemoView


void CENCARA2_2libDemoView::OnProcesoFichero()
{
	// TODO: Agregue aquí su código de controlador de comandos
	m_boFirstFrame=true;

	if (m_bofileopen)
	{
		int nvalidframes = 0;
		int framelastcut=0,beginframe=0;

		
		string window_name = "video | q or esc to quit";
		cout << "press space to save a picture. q or esc to quit" << endl;
		namedWindow(window_name, CV_WINDOW_KEEPRATIO); //resizable window;
		Mat frame,frameprev;
		bool bohayframe=true;

		//string filename("C:\\Users\\Modesto\\Documents\\FACES\\Plenos\\tmps\\cut_"),filenamefull;

		while (bohayframe)
		{
			m_capture >> frame;

			//Invalid frame
			if (frame.empty())
				bohayframe=false;

			if (bohayframe)
			{
				nvalidframes++;

				//Load face detector with first valid frame
				if (m_boFirstFrame)
				{
					LoadFaceDetector(frame);
					m_boFirstFrame=false;
				}
				
				//Process frame
				Process(frame);

				
				//Debugging saving the image
				//imwrite(filenamefull,frame);

				imshow(window_name, frame);

				char key = (char)waitKey(5); //delay N millis, usually long enough to display and capture input


    //        switch (key) {
				//case 'q':
				//case 'Q':
				//case 27: //escape key
				//	return 0;
				//case ' ': //Save an image
    //    			sprintf(filename,"filename%.3d.jpg",n++);
    //    			imwrite(filename,frame);
    //    			cout << "Saved " << filename << endl;
    //    			break;
				//default:
				//	break;
				//	}
				//Stores current frame
				frameprev.release();
				frameprev=frame.clone();
				frame.release();
			}
        }

		
        return;

	}
}


void CENCARA2_2libDemoView::OnProcesoC32771()
{
	// TODO: Agregue aquí su código de controlador de comandos
	CameraInit();
}


void CENCARA2_2libDemoView::OnFileOpen()
{
	// TODO: Agregue aquí su código de controlador de comandos
	static char BASED_CODE szFilter[] = "";
	CFileDialog OpenFile(true, NULL, NULL, 0 ,szFilter, NULL);

    //Open dialog
	if (OpenFile.DoModal() != IDOK)
	{
		return;
	}
	//Gets name
	CString fullname = OpenFile.GetPathName();

	// Convert to a char*
	std::string fullnamestring(fullname);
	//fullnamestring += " (basic_string)";

	Reset();

	m_capture.open(fullnamestring); //try to open string, this will attempt to open it as a video file
	
	//if (!capture.isOpened()) //if this fails, try to open as a video camera, through the use of an integer param
	//	capture.open(atoi(arg.c_str()));

	if (!m_capture.isOpened())
	{
		cerr << "Failed to open a video device or video file!\n" << endl;
	}
	else //Se abre el vídeo
	{
		m_bofileopen=true;
	}
}


void CENCARA2_2libDemoView::LoadFaceDetector(Mat frame)
{
	ReleaseFaceDetector();

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
	ENCARAFaceDetector=new CENCARA2_2Detector(ENCARAdataDir,frame.size().width,frame.size().height);


	
}

void CENCARA2_2libDemoView::ReleaseFaceDetector()
{
	if (ENCARAFaceDetector)
	{
		delete ENCARAFaceDetector;
		ENCARAFaceDetector=NULL;
	}
}

void CENCARA2_2libDemoView::Reset()
{
	CameraStop();
	if (m_bofileopen)
	{
		m_capture.release();
		m_bofileopen=false;
	}
}

/*************************************************************************************************************

		Camera Management

*************************************************************************************************************/
/***************************************************************************************************
			Camera Initialization
***************************************************************************************************/
void CENCARA2_2libDemoView::CameraInit() 
{
	//Close still open channels
	CameraStop();
	
	m_capture.open(0); // open the default camera
    if(!m_capture.isOpened())  // check if we succeeded
	{
        return;
	}
	else
	{
		m_boFirstFrame=true;
		m_boCapturando=true;
		Invalidate();
	}	
}


/***************************************************************************************************
			Camera Stop
***************************************************************************************************/
void CENCARA2_2libDemoView::CameraStop() 
{
	//Close open channels
	if (m_boCapturando)
	{		
		if (m_bofileopen)//From file
		{			
			m_bofileopen=false;
		}
		m_capture.release();
	}
	m_boCapturando=false;
}

void CENCARA2_2libDemoView::Process(Mat frame)
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
