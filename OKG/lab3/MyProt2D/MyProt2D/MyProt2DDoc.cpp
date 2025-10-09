
// MyProt2DDoc.cpp: реализация класса CMyProt2DDoc 


#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "MyProt2D.h"
#endif

#include "MyProt2DDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMyProt2DDoc

IMPLEMENT_DYNCREATE(CMyProt2DDoc, CDocument)

BEGIN_MESSAGE_MAP(CMyProt2DDoc, CDocument)
END_MESSAGE_MAP()


// Создание или уничтожение CMyProt2DDoc

CMyProt2DDoc::CMyProt2DDoc() noexcept
{


}

CMyProt2DDoc::~CMyProt2DDoc()
{
}

BOOL CMyProt2DDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;



	return TRUE;
}




// Сериализация CMyProt2DDoc

void CMyProt2DDoc::Serialize(CArchive& ar)
{

}



// Диагностика CMyProt2DDoc

#ifdef _DEBUG
void CMyProt2DDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMyProt2DDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

// Команды CMyProt2DDoc