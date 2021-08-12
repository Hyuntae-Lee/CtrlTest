// ImageListViewer.cpp: 구현 파일
//

#include "pch.h"
#include "CtrlTest.h"
#include "ImageListViewer.h"


// CImageListViewer

IMPLEMENT_DYNAMIC(CImageListViewer, CListCtrl)

CImageListViewer::CImageListViewer()
{

}

CImageListViewer::~CImageListViewer()
{
}


BEGIN_MESSAGE_MAP(CImageListViewer, CListCtrl)
	ON_WM_NCCALCSIZE()
END_MESSAGE_MAP()



// CImageListViewer 메시지 처리기




void CImageListViewer::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	ModifyStyle(WS_HSCROLL, 0);

	CListCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);
}
