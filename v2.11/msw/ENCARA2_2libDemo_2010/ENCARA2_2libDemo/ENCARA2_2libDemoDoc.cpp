
// ENCARA2_2libDemoDoc.cpp: implementación de la clase CENCARA2_2libDemoDoc
//

#include "stdafx.h"
// Se pueden definir SHARED_HANDLERS en un proyecto ATL implementando controladores de vista previa, miniatura
// y filtro de búsqueda, y permiten compartir código de documentos con ese proyecto.
#ifndef SHARED_HANDLERS
#include "ENCARA2_2libDemo.h"
#endif

#include "ENCARA2_2libDemoDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CENCARA2_2libDemoDoc

IMPLEMENT_DYNCREATE(CENCARA2_2libDemoDoc, CDocument)

BEGIN_MESSAGE_MAP(CENCARA2_2libDemoDoc, CDocument)
END_MESSAGE_MAP()


// Construcción o destrucción de CENCARA2_2libDemoDoc

CENCARA2_2libDemoDoc::CENCARA2_2libDemoDoc()
{
	// TODO: agregar aquí el código de construcción único

}

CENCARA2_2libDemoDoc::~CENCARA2_2libDemoDoc()
{
}

BOOL CENCARA2_2libDemoDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: agregar aquí código de reinicio
	// (los documentos SDI volverán a utilizar este documento)

	return TRUE;
}




// Serialización de CENCARA2_2libDemoDoc

void CENCARA2_2libDemoDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: agregar aquí el código de almacenamiento
	}
	else
	{
		// TODO: agregar aquí el código de carga
	}
}

#ifdef SHARED_HANDLERS

// Compatibilidad con miniaturas
void CENCARA2_2libDemoDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modifique este código para dibujar los datos del documento
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Compatibilidad con controladores de búsqueda
void CENCARA2_2libDemoDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Establezca contenido de búsqueda a partir de los datos del documento. 
	// Las partes de contenido deben ir separadas por ";"

	// Por ejemplo:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CENCARA2_2libDemoDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// Diagnósticos de CENCARA2_2libDemoDoc

#ifdef _DEBUG
void CENCARA2_2libDemoDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CENCARA2_2libDemoDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// Comandos de CENCARA2_2libDemoDoc
