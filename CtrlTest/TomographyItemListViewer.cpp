// TomographyItemListViewer.cpp: 구현 파일
//

#include "pch.h"
#include "CtrlTest.h"
#include "TomographyItemListViewer.h"
#include "afxdialogex.h"
#include "cpprest/json.h"


// CTomographyItemListViewer 대화 상자

IMPLEMENT_DYNAMIC(CTomographyItemListViewer, CDialogEx)

CTomographyItemListViewer::CTomographyItemListViewer(CWnd* pParent, SIZE nBScanItems,
	SIZE iBScanItemSize)
	: CDialogEx(IDD_DLG_TOMOGRAPHYITEMLISTVIEWER, pParent)
	, kNBScanRows(nBScanItems.cx)
	, kNBScanCols(nBScanItems.cy)
	, kBScanItemW(iBScanItemSize.cx)
	, kBScanItemH(iBScanItemSize.cy)
{
	// compose bscan item rects
	for (int row = 0; row < kNBScanRows; row++) {
		for (int col = 0; col < kNBScanCols; col++) {
			
			int l = col * kBScanItemW;
			int t = row * kBScanItemH;
			int r = l + kBScanItemW;
			int b = t + kBScanItemH;

			CRect rect(l, t, r, b);

			m_bscanItemRectList.push_back(rect);
		}
	}
}

CTomographyItemListViewer::~CTomographyItemListViewer()
{
}

bool CTomographyItemListViewer::loadImages(wstring strDirPath, wstring strInfoFileName)
{
	m_strDirPath = strDirPath;

	// load json file
	web::json::value jsonValue;
	{
		auto strFilePath = strDirPath + L"/" + strInfoFileName;

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
	
	vector<CString> szImageNameList;
	CString szDirPath(strDirPath.c_str());
	if (!getImageFileNames(szImageNameList, szDirPath, m_imageNameListCScans)) {
		return false;
	}

	for (int i = 0; i < szImageNameList.size(); i++) {
		wstring strImageName = CT2CW(szImageNameList[i]);

		CString szName;
		szName.Format(_T("%d/%d"), i + 1, (int)szImageNameList.size());
		wstring strName = CT2CW(szName);

		addItem(strImageName, strName);
	}

	updateScrollInfo();

	return true;
}

void CTomographyItemListViewer::addItem(wstring strFileName, wstring strName)
{
	auto index = (int)m_itemList.size();

	CTomographyItem newItem(m_strDirPath, strFileName, index, strName);

	m_itemList.push_back(newItem);
}

bool CTomographyItemListViewer::readFile(wstring& out_value, wstring strPath)
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

bool CTomographyItemListViewer::getImageFileNames(vector<CString>& out_list, CString szDirPath,
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

void CTomographyItemListViewer::updateScrollInfo()
{
	int size = int(m_itemList.size());
	int rows = int(ceil(size / 3.0));

	int nMax = kNBScanRows - 1;
	int nMin = 0;
	int nPage = kNBScanCols;
	int nPos = 0;

	SCROLLINFO scrInfo;
	scrInfo.cbSize = sizeof(scrInfo);
	scrInfo.fMask = SIF_ALL;
	scrInfo.nMin = 0;
	scrInfo.nMax = 100;
	scrInfo.nPage = 50;
	scrInfo.nTrackPos = 0;
	scrInfo.nPos = 0;

	m_scrollbar.SetScrollInfo( &scrInfo ); // 스크롤바 정보 설정
}

void CTomographyItemListViewer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SCROLLBAR_VER, m_scrollbar);
}


BEGIN_MESSAGE_MAP(CTomographyItemListViewer, CDialogEx)
	ON_WM_PAINT()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


// CTomographyItemListViewer 메시지 처리기


void CTomographyItemListViewer::OnPaint()
{
	CPaintDC dc(this);


}


void CTomographyItemListViewer::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch (nSBCode) {
	case SB_LINEUP:
		pScrollBar->SetScrollPos(pScrollBar->GetScrollPos() - 1);
		break;
	case SB_LINEDOWN:
		pScrollBar->SetScrollPos(pScrollBar->GetScrollPos() + 1);
		break;
	}

	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}
