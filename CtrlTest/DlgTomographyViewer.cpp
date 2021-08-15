// DlgTomographyViewer.cpp: 구현 파일
//

#include "pch.h"
#include "CtrlTest.h"
#include "DlgTomographyViewer.h"
#include "afxdialogex.h"
#include <fstream>

// CDlgTomographyViewer 대화 상자

IMPLEMENT_DYNAMIC(CDlgTomographyViewer, CDialogEx)

CDlgTomographyViewer::CDlgTomographyViewer(CWnd* pParent, SIZE thumbImageSize, int rows, int cols)
	: CDialogEx(IDD_DLG_TOMOGRAPHYVIEWER, pParent)
	, kImageSizeX(thumbImageSize.cx)
	, kImageSizeY(thumbImageSize.cy)
	, kRows(rows)
	, kCols(cols)
{

}

CDlgTomographyViewer::~CDlgTomographyViewer()
{
}

bool CDlgTomographyViewer::loadImages(CString szDirPath, CString szInfoFileName)
{
	wstring strDirPath = CT2CW(szDirPath);

	// load json file
	web::json::value jsonValue;

	wstring strFileName = CT2CW(szInfoFileName);
	auto strFilePath = strDirPath + L"/" + strFileName;

	if (!loadJsonFile(jsonValue, strFilePath)) {
		return false;
	}
	

	// TODO

	// get image list
	imageNameListBScans.clear();
	if (!getImageFileNames(imageNameListBScans, szDirPath)) {
		return false;
	}

	drawThumbnailList(szDirPath, imageNameListBScans);

	return true;
}

void CDlgTomographyViewer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_BSCANS, m_listCtrlBScans);
}

void CDlgTomographyViewer::drawThumbnailList(CString szDirPath, vector<CString> imageNameList)
{
	// hold the window update to avoid flicking
	m_listCtrlBScans.SetRedraw(FALSE);

	// reset our image list
	for (int i = 0; i < m_imageListBScans.GetImageCount(); i++) {
		m_imageListBScans.Remove(i);
	}

	// remove all items from list view
	if (m_listCtrlBScans.GetItemCount() != 0) {
		m_listCtrlBScans.DeleteAllItems();
	}

	// set the size of the image list
	m_imageListBScans.SetImageCount(static_cast<UINT>(imageNameList.size()));

	// draw the thumbnails
	for (int i = 0; i < imageNameList.size(); i++) {
		auto imageName = imageNameList[i];

		// image path
		CString szPath;
		szPath.Format(_T("%s\\%s"), szDirPath.GetBuffer(), imageName.GetBuffer());

		// position, name
		POINT pos;
		CString szName;
		pos.x = kImageSizeX * (i % kCols);

		pos.y = kImageSizeY * (int)(i / kCols);

		szName = _T("");

		drawThumbnail(kImageSizeX, kImageSizeY, i, szPath, szName, pos);
	}

	// let's show the new thumbnails
	m_listCtrlBScans.SetRedraw();
}

void CDlgTomographyViewer::drawThumbnail(int thumSizeX, int thumSizeY, int index,
	CString szImagePath, CString szName, POINT pos)
{
	ULONG_PTR gdiplusToken = NULL;

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

	{
		// load the bitmap
		USES_CONVERSION;
		Gdiplus::Bitmap img(szImagePath);
		Gdiplus::Bitmap* pThumbnail = static_cast<Gdiplus::Bitmap*>(img.GetThumbnailImage(kImageSizeX, kImageSizeY, NULL, NULL));

		// attach the thumbnail bitmap handle to an CBitmap object
		HBITMAP hBmp;
		pThumbnail->GetHBITMAP(NULL, &hBmp);
		auto pImage = new CBitmap();
		pImage->Attach(hBmp);

		// add bitmap to our image list
		m_imageListBScans.Replace(index, pImage, NULL);

		// put item to display
		// set the image file name as item text
		m_listCtrlBScans.InsertItem(index, szName, index);

		// set position
		m_listCtrlBScans.SetItemPosition(index, pos);

		delete pImage;
		delete pThumbnail;
	}

	Gdiplus::GdiplusShutdown(gdiplusToken);
}

bool CDlgTomographyViewer::getImageFileNames(vector<CString>& out_list, CString szDirPath)
{
	CString	strExt;
	CString	strName;
	CString	strPattern;
	BOOL	bRC = TRUE;

	HANDLE			hFind = NULL;
	WIN32_FIND_DATA	FindFileData;

	if (szDirPath[szDirPath.GetLength() - 1] == TCHAR('\\'))
		strPattern.Format(_T("%s*.*"), szDirPath.GetBuffer());
	else
		strPattern.Format(_T("%s\\*.*"), szDirPath.GetBuffer());

	hFind = ::FindFirstFile(strPattern, &FindFileData);	// strat search	
	if (hFind == INVALID_HANDLE_VALUE) {
		return false;
	}

	// filter off the system files and directories
	if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
		!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) &&
		!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) &&
		!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY))
	{
		// test file extension
		strName = FindFileData.cFileName;
		strExt = strName.Right(3);

		if ((strExt.CompareNoCase(TEXT("bmp")) == 0) ||
			(strExt.CompareNoCase(TEXT("jpg")) == 0) ||
			(strExt.CompareNoCase(TEXT("gif")) == 0) ||
			(strExt.CompareNoCase(TEXT("tif")) == 0) ||
			(strExt.CompareNoCase(TEXT("png")) == 0))
		{
			// save the image file name
			out_list.push_back(strName);
		}
	}

	// loop through to add all of them to our vector	
	while (bRC)
	{
		bRC = ::FindNextFile(hFind, &FindFileData);
		if (bRC)
		{
			// filter off the system files and directories
			if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
				!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) &&
				!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) &&
				!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY))
			{
				// test file extension
				strName = FindFileData.cFileName;
				strExt = strName.Right(3);

				if ((strExt.CompareNoCase(TEXT("bmp")) == 0) ||
					(strExt.CompareNoCase(TEXT("jpg")) == 0) ||
					(strExt.CompareNoCase(TEXT("gif")) == 0) ||
					(strExt.CompareNoCase(TEXT("tif")) == 0) ||
					(strExt.CompareNoCase(TEXT("png")) == 0))
				{
					// save the image file name
					out_list.push_back(strName);
				}
			}
		}
		else
		{
			DWORD err = ::GetLastError();
			if (err != ERROR_NO_MORE_FILES) {
				return false;
			}
		}
	} // end of while loop

	// close the search handle
	::FindClose(hFind);

	//
	return true;
}

BEGIN_MESSAGE_MAP(CDlgTomographyViewer, CDialogEx)
END_MESSAGE_MAP()

bool CDlgTomographyViewer::loadJsonFile(web::json::value& out_value, wstring strPath)
{
	// read file
	wifstream file(strPath);
	if (!file.is_open()) {
		return false;
	}

	wstringbuf strContentsBuf;
	wostream os(&strContentsBuf);

	while (file) {
		wstring strLine;
		getline(file, strLine);

		os << strLine;
	}
	file.close();

	// load json conents
	out_value = web::json::value::parse(strContentsBuf.str());

	return true;
}

// CDlgTomographyViewer 메시지 처리기
BOOL CDlgTomographyViewer::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_imageListBScans.Create(kImageSizeX, kImageSizeY, ILC_COLOR24, 0, 1);
	m_listCtrlBScans.SetImageList(&m_imageListBScans, LVSIL_NORMAL);

	m_listCtrlBScans.ShowScrollBar(0, FALSE);
	m_listCtrlBScans.ShowScrollBar(1, FALSE);

	// set window size
	{
		// width
		int wndW = kImageSizeX * kCols + kScrollMarginForNoName;

		// height
		int wndH = static_cast<int>(kImageSizeY * kRows);

		// 
		CRect rtClient;
		m_listCtrlBScans.GetClientRect(&rtClient);
		rtClient.SetRect(rtClient.left, rtClient.top, rtClient.left + wndW, rtClient.top + wndH);

		m_listCtrlBScans.MoveWindow(&rtClient);
	}

	return TRUE;
}
