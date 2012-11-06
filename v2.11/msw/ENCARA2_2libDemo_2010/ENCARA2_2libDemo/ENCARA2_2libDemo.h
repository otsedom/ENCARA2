
// ENCARA2_2libDemo.h: archivo de encabezado principal para la aplicación ENCARA2_2libDemo
//
#pragma once

#ifndef __AFXWIN_H__
	#error "incluir 'stdafx.h' antes de incluir este archivo para PCH"
#endif

#include "resource.h"       // Símbolos principales


// CENCARA2_2libDemoApp:
// Consulte la sección ENCARA2_2libDemo.cpp para obtener información sobre la implementación de esta clase
//

class CENCARA2_2libDemoApp : public CWinAppEx
{
public:
	CENCARA2_2libDemoApp();


// Reemplazos
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementación
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CENCARA2_2libDemoApp theApp;
