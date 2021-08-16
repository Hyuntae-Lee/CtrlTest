// DlgTomographyItemListViewer.cpp: 구현 파일
//

#include "pch.h"
#include "CtrlTest.h"
#include "DlgTomographyItemListViewer.h"
#include "afxdialogex.h"


// CDlgTomographyItemListViewer 대화 상자

IMPLEMENT_DYNAMIC(CDlgTomographyItemListViewer, CDialogEx)

CDlgTomographyItemListViewer::CDlgTomographyItemListViewer(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TOMOGRAPHYITEMLISTVIEWER, pParent)
{

}

CDlgTomographyItemListViewer::~CDlgTomographyItemListViewer()
{
}

void CDlgTomographyItemListViewer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgTomographyItemListViewer, CDialogEx)
END_MESSAGE_MAP()


// CDlgTomographyItemListViewer 메시지 처리기
