// DlgTomographyViewer.cpp: 구현 파일
//

#include "pch.h"
#include "CtrlTest.h"
#include "DlgTomographyViewer.h"
#include "afxdialogex.h"
#include <fstream>
#include "cpprest/json.h"
	// 이 헤더를 include 하는 것만으로도 Visual Studio 실행결과에는 Memory Leak 이 발생하는 것으로 표시됨.
	// 실제로 메모리가 새는 것은 아니고, dll load 순서 때문에 그렇게 표시되는 것이라 함.
	// 출처 : http://codename26.rssing.com/chan-8623770/all_p204.html

// CDlgTomographyViewer 대화 상자

IMPLEMENT_DYNAMIC(CDlgTomographyViewer, CDialogEx)

CDlgTomographyViewer::CDlgTomographyViewer(CWnd* pParent, SIZE cscanImageSize, SIZE bscanImageSize,
	int bscanRows, int bscanCols)
	: CDialogEx(IDD_DLG_TOMOGRAPHYVIEWER, pParent)
	, kCScanImageSizeX(cscanImageSize.cx)
	, kCScanImageSizeY(cscanImageSize.cy)
	, kBScanImageSizeX(bscanImageSize.cx)
	, kBScanImageSizeY(bscanImageSize.cy)
	, kBScanRows(bscanRows)
	, kBScanCols(bscanCols)
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
	{
		wstring strFileName = CT2CW(szInfoFileName);
		auto strFilePath = strDirPath + L"/" + strFileName;

		wstring strContents;

		if (!readFile(strContents, strFilePath)) {
			return false;
		}

		jsonValue = web::json::value::parse(strContents);
	}

	// get data info.
	auto strFileNameCScan0 = jsonValue[kStrKeyCScan0].as_string();
	auto strFileNameCScan1 = jsonValue[kStrKeyCScan1].as_string();
	auto strNBScans = jsonValue[kStrKeyBScanCnt].as_integer();
	auto strFileNamePrefixBScan = jsonValue[kStrKeyBScanFilePrefix].as_string();
	auto strFileExtBScan = jsonValue[kStrKeyBScanFileExt].as_string();

	// get image list
	m_imageNameListCScans.clear();
	m_imageNameListCScans = {
		CString(strFileNameCScan0.c_str()),
		CString(strFileNameCScan1.c_str()),
	};
	m_imageNameListBScans.clear();
	if (!getImageFileNames(m_imageNameListBScans, szDirPath, m_imageNameListCScans)) {
		return false;
	}

	drawBScanImageList({ kCScanImageSizeX, kCScanImageSizeY }, szDirPath, m_imageNameListBScans);

	drawCScanImage(m_imageCScan0, szDirPath + _T("\\") + m_imageNameListCScans[0]);
	drawCScanImage(m_imageCScan1, szDirPath + _T("\\") + m_imageNameListCScans[1]);

	return true;
}

void CDlgTomographyViewer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_BSCANS, m_listCtrlBScans);
	DDX_Control(pDX, IDC_STATIC_CSCAN_0, m_imageCScan0);
	DDX_Control(pDX, IDC_STATIC_CSCAN_1, m_imageCScan1);
}

void CDlgTomographyViewer::drawBScanImageList(POINT pos, CString szDirPath, vector<CString> imageNameList)
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
		pos.x = kBScanImageSizeX * (i % kBScanCols);
		pos.y = kBScanImageSizeY * (int)(i / kBScanCols);

		szName = _T("");

		drawBScanImage(kBScanImageSizeX, kBScanImageSizeY, i, szPath, szName, pos);
	}

	// let's show the new thumbnails
	m_listCtrlBScans.SetRedraw();
}

void CDlgTomographyViewer::drawBScanImage(int thumSizeX, int thumSizeY, int index,
	CString szImagePath, CString szName, POINT pos)
{
	ULONG_PTR gdiplusToken = NULL;

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

	{
		// load the bitmap
		USES_CONVERSION;
		Gdiplus::Bitmap img(szImagePath);
		Gdiplus::Bitmap* pThumbnail = static_cast<Gdiplus::Bitmap*>(img.GetThumbnailImage(kBScanImageSizeX, kBScanImageSizeY, NULL, NULL));

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

bool CDlgTomographyViewer::readFile(wstring& out_value, wstring strPath)
{
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

	out_value = strContentsBuf.str();

	return true;
}

void CDlgTomographyViewer::drawCScanImage(CStatic& view, CString szImagePath)
{
	//CImage img;
	//img.Load(szImagePath);
	//CBitmap bitmap;
	//bitmap.Attach(img.Detach());
	//view.SetBitmap(bitmap);


	ULONG_PTR gdiplusToken = NULL;

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

	{
		// load the bitmap
		USES_CONVERSION;
		Gdiplus::Bitmap img(szImagePath);
		Gdiplus::Bitmap* pImageScaled = static_cast<Gdiplus::Bitmap*>(img.GetThumbnailImage(kCScanImageSizeX, kCScanImageSizeY, NULL, NULL));

		// attach the thumbnail bitmap handle to an CBitmap object
		HBITMAP hBmp;
		pImageScaled->GetHBITMAP(NULL, &hBmp);
		
		auto pImage = new CBitmap();
		pImage->Attach(hBmp);

		view.SetBitmap((HBITMAP)*pImage);

		delete pImage;
		delete pImageScaled;
	}

	Gdiplus::GdiplusShutdown(gdiplusToken);
}

bool CDlgTomographyViewer::getImageFileNames(vector<CString>& out_list, CString szDirPath,
	vector<CString> excludes)
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

		//
		auto itr = find_if(excludes.begin(), excludes.end(),
			[strName](auto x) { return x == strName; });
		if (itr == excludes.end()) {
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
				//
				auto itr = find_if(excludes.begin(), excludes.end(),
					[strName](auto x) { return x == strName; });
				if (itr == excludes.end()) {
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

// CDlgTomographyViewer 메시지 처리기
BOOL CDlgTomographyViewer::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_imageListBScans.Create(kBScanImageSizeX, kBScanImageSizeY, ILC_COLOR24, 0, 1);
	m_listCtrlBScans.SetImageList(&m_imageListBScans, LVSIL_NORMAL);

	m_listCtrlBScans.ShowScrollBar(0, FALSE);
	m_listCtrlBScans.ShowScrollBar(1, FALSE);

	// CScan
	{
		int offsetX = 0;
		int offsetY = 0;

		m_imageCScan0.MoveWindow(offsetX, offsetY, kCScanImageSizeX, kCScanImageSizeY);
		m_imageCScan1.MoveWindow(offsetX, offsetY + kCScanImageSizeY, kCScanImageSizeX, kCScanImageSizeY);
	}

	// BScan
	{
		// width
		int wndW = kBScanImageSizeX * kBScanCols + kScrollMargin;

		// height
		int wndH = static_cast<int>(kBScanImageSizeY * kBScanRows);

		CRect rtCScan0;
		m_imageCScan0.GetClientRect(&rtCScan0);

		auto left = rtCScan0.right;
		auto top = rtCScan0.top;
		auto right = left + wndW;
		auto bottom = top + wndH;

		m_listCtrlBScans.MoveWindow(CRect(left, top, right, bottom));
	}

	return TRUE;
}
