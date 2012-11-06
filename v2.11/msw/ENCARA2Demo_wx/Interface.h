#ifndef _INTERFACE_H
#define _INTERFACE_H

//Using OpenCV
#include "cv.h"
#include "cvaux.h"
#include "highgui.h"


//Using ENCARA2_2lib
#include "ENCARA2_2lib.h"
#include "SVMUtils.h"

// ----------------------------------------------------------------------------
// Recursos
// ----------------------------------------------------------------------------

// Define un nuevo tipo de aplicación. Todos deben derivar de wxApp
class MainApp : public wxApp
{
public:
    // Se sobreescriben los métodos virtuales
    // ----------------------------

    // OnInit se invoca al lanzar la aplicación, por lo que es adecuado para las inicializaciones.
	// Si retornara false, la aplicación finaliza.
    virtual bool OnInit();


};

// Nuevo tipo de marco (ventana), que será nuestro marco principal
class MainFrame : public wxFrame
{
public:
    // constructor
    MainFrame(const wxString& title);

    // Manejadores de eventos
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
	void MiLlamada(wxCommandEvent& event);
	void FileDialog(wxCommandEvent& event);
	void StartCam(wxCommandEvent& event);
	void StopCam(wxCommandEvent& event);


	
	

private:
    // Cualquir clase que procese eventos wxWidgets debe usar esta macro
    DECLARE_EVENT_TABLE()


	//Variables propias
	bool	m_boCapturando;
	bool	m_boImagenCargada;
	CvCapture *m_capture;


	//ENCARA2
	int m_width,m_height;					//image dimensions
	int frame;								//Sequence frame number

	char cPath[256];						//Aux var

	clock_t ttotal;							//Used to compute processing time

	//Working folders
	char ENCARAdataDir[256];

	CENCARA2_2Detector *ENCARAFaceDetector;	//ENCARA2.2 detector

	CFacialDataperImage	*FacialData;	//Facial data

};

// ----------------------------------------------------------------------------
// Constantes
// ----------------------------------------------------------------------------

// IDs de los controles de menú
enum
{
    // elementos del menú
    Submenu1_Quit = wxID_EXIT,
	
    // Para el comando "About" es adecuado usar este identificador para que funcione de forma correcta
    About = wxID_ABOUT,

	//Otros elementos
	Submenu1_Ficheros = wxID_HIGHEST,

	Submenu2_StartCam = wxID_HIGHEST+1,
	Submenu2_StopCam = wxID_HIGHEST+2

};

// ----------------------------------------------------------------------------
// Tabla de eventos y otras macros para wxWidgets
// ----------------------------------------------------------------------------

// Las tablas de eventos conectan los eventos wxWidgets con las funciones que los procesan. 
// Puede también establecerse en tiempo de ejecución.
BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(Submenu1_Quit,  MainFrame::OnQuit)
    EVT_MENU(About, MainFrame::OnAbout)
	EVT_MENU(Submenu1_Ficheros, MainFrame::FileDialog)
	EVT_MENU(Submenu2_StartCam, MainFrame::StartCam)
	EVT_MENU(Submenu2_StopCam, MainFrame::StopCam)
END_EVENT_TABLE()

#endif