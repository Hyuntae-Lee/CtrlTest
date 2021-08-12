// ThumbnailDlg.cpp: 구현 파일
//

#include "pch.h"
#include "CtrlTest.h"
#include "ThumbnailDlg.h"
#include "afxdialogex.h"
#include <gdiplus.h>

// CThumbnailDlg 대화 상자

IMPLEMENT_DYNAMIC(CThumbnailDlg, CDialogEx)

CThumbnailDlg::CThumbnailDlg(CWnd* pParent, int thumSizeX, int thumSizeY)
	: CDialogEx(IDD_DLG_THUMNAIL, pParent)
{
	m_thumSizeX = thumSizeX;
	m_thumSizeY = thumSizeY;
}

CThumbnailDlg::~CThumbnailDlg()
{
}

bool CThumbnailDlg::setImageDir(CString szDirPath)
{
	vector<CString> imageNameList;

	if (!getImageFileNames(imageNameList, szDirPath)) {
		return false;
	}

	CThumbnailDlg::drawThumbnailList(szDirPath, imageNameList, 2, { 10, 10 }, false);

	return true;
}

void CThumbnailDlg::drawThumbnailList(CString szDirPath, vector<CString> imageNameList,
	int nCols, SIZE gap, bool bShowName)
{
	// hold the window update to avoid flicking
	m_listCtrlThumbnail.SetRedraw(FALSE);

	// reset our image list
	for (int i = 0; i < m_imageListThumb.GetImageCount(); i++) {
		m_imageListThumb.Remove(i);
	}

	// remove all items from list view
	if (m_listCtrlThumbnail.GetItemCount() != 0) {
		m_listCtrlThumbnail.DeleteAllItems();
	}

	// set the size of the image list
	m_imageListThumb.SetImageCount(static_cast<UINT>(imageNameList.size()));

	// draw the thumbnails
	for (int i = 0; i < imageNameList.size(); i++) {
		auto imageName = imageNameList[i];

		// image path
		CString szPath;
		szPath.Format(_T("%s\\%s"), szDirPath.GetBuffer(), imageName.GetBuffer());

		// position
		POINT pos;
		pos.x = (m_thumSizeX + gap.cx) * (i % nCols);
		pos.y = (m_thumSizeY + gap.cy) * (int)(i / nCols);

		// name
		CString szName = bShowName ? imageNameList[i] : _T("");

		drawThumbnail(m_thumSizeX, m_thumSizeY, i, szPath, szName, pos);
	}

	// let's show the new thumbnails
	m_listCtrlThumbnail.SetRedraw();
}

void CThumbnailDlg::drawThumbnail(int thumSizeX, int thumSizeY, int index,
	CString szImagePath, CString szName, POINT pos)
{
	ULONG_PTR gdiplusToken = NULL;

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

	{
		// load the bitmap
		USES_CONVERSION;
		Gdiplus::Bitmap img(szImagePath);
		Gdiplus::Bitmap* pThumbnail = static_cast<Gdiplus::Bitmap*>(img.GetThumbnailImage(m_thumSizeX, m_thumSizeY, NULL, NULL));

		// attach the thumbnail bitmap handle to an CBitmap object
		HBITMAP hBmp;
		pThumbnail->GetHBITMAP(NULL, &hBmp);
		auto pImage = new CBitmap();
		pImage->Attach(hBmp);

		// add bitmap to our image list
		m_imageListThumb.Replace(index, pImage, NULL);

		// put item to display
		// set the image file name as item text
		m_listCtrlThumbnail.InsertItem(index, szName, index);

		// set position
		m_listCtrlThumbnail.SetItemPosition(index, pos);

		delete pImage;
		delete pThumbnail;
	}

	Gdiplus::GdiplusShutdown(gdiplusToken);
}

bool CThumbnailDlg::getImageFileNames(vector<CString>& out_list, CString szDirPath)
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

void CThumbnailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_THUMNAILS, m_listCtrlThumbnail);
}

BEGIN_MESSAGE_MAP(CThumbnailDlg, CDialogEx)
END_MESSAGE_MAP()


// CThumbnailDlg 메시지 처리기

BOOL CThumbnailDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_imageListThumb.Create(m_thumSizeX, m_thumSizeY, ILC_COLOR24, 0, 1);
	m_listCtrlThumbnail.SetImageList(&m_imageListThumb, LVSIL_NORMAL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
