
// ENCARA2_2libDemoView.h: interfaz de la clase CENCARA2_2libDemoView
//

#pragma once

//opencv
#include "opencv/cv.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/highgui/highgui_c.h"
#include <iostream>
#include <vector>
#include <stdio.h>

//Using ENCARA2_2lib
#include "ENCARA2_2lib.h"

using namespace cv;
using namespace std;


class CENCARA2_2libDemoView : public CView
{
protected: // Crear sólo a partir de serialización
	CENCARA2_2libDemoView();
	DECLARE_DYNCREATE(CENCARA2_2libDemoView)

// Atributos
public:
	CENCARA2_2libDemoDoc* GetDocument() const;

// Operaciones
public:

// Reemplazos
public:
	virtual void OnDraw(CDC* pDC);  // Reemplazado para dibujar esta vista
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementación
public:
	virtual ~CENCARA2_2libDemoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void Process(Mat frame);
	void CameraInit();
	void CameraStop();
	void Reset();
	void LoadFaceDetector(Mat frame);
	void ReleaseFaceDetector();


	VideoCapture m_capture;
	bool m_bofileopen,m_boCapturando;
	bool m_boFirstFrame;

	//ENCARA2
	//Working folders
	char ENCARAdataDir[256];

	CENCARA2_2Detector *ENCARAFaceDetector;	//ENCARA2.2 detector

	CFacialDataperImage	*FacialData;	//Facial data

protected:

// Funciones de asignación de mensajes generadas
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnProcesoFichero();
	afx_msg void OnProcesoC32771();
	afx_msg void OnFileOpen();
};

#ifndef _DEBUG  // Versión de depuración en ENCARA2_2libDemoView.cpp
inline CENCARA2_2libDemoDoc* CENCARA2_2libDemoView::GetDocument() const
   { return reinterpret_cast<CENCARA2_2libDemoDoc*>(m_pDocument); }
#endif

