// ThumbnailDlg.cpp: 구현 파일
//

#include "pch.h"
#include "CtrlTest.h"
#include "ThumbnailDlg.h"
#include "afxdialogex.h"
#include <gdiplus.h>

// CThumbnailDlg 대화 상자

IMPLEMENT_DYNAMIC(CThumbnailDlg, CDialogEx)

CThumbnailDlg::CThumbnailDlg(CWnd* pParent, long thumSizeX, long thumSizeY)
	: CDialogEx(IDD_DLG_THUMNAIL, pParent)
{
	m_thumSizeX = thumSizeX;
	m_thumSizeY = thumSizeY;
}

CThumbnailDlg::~CThumbnailDlg()
{
}

bool CThumbnailDlg::setImageDir(CString dzDirPath)
{
	m_imageNameList.clear();
	if (!getImageFileNames(m_imageNameList, dzDirPath)) {
		return false;
	}

	CThumbnailDlg::drawThumbnails(dzDirPath, m_imageNameList);
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
		strPattern.Format(_T("%s*.*"), szDirPath);
	else
		strPattern.Format(_T("%s\\*.*"), szDirPath);

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

void CThumbnailDlg::drawThumbnails(CString szDirPath, vector<CString> imgNameList)
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, nullptr);

	CBitmap* pImage = NULL;
	HBITMAP		hBmp = NULL;
	POINT		pt;
	CString		strPath;
	int			i;

	// set the length of the space between thumbnails
	// you can also calculate and set it based on the length of your list control
	int nGap = 6;

	// hold the window update to avoid flicking
	m_listThumbnail.SetRedraw(FALSE);

	// reset our image list
	for (i = 0; i < m_imageListThumb.GetImageCount(); i++)
		m_imageListThumb.Remove(i);

	// remove all items from list view
	if (m_listThumbnail.GetItemCount() != 0)
		m_listThumbnail.DeleteAllItems();

	// set the size of the image list
	m_imageListThumb.SetImageCount(imgNameList.size());
	i = 0;

	// draw the thumbnails
	std::vector<CString>::iterator	iter;
	for (iter = imgNameList.begin(); iter != imgNameList.end(); iter++)
	{
		// load the bitmap
		strPath.Format(_T("%s\\%s"), szDirPath, *iter);

		USES_CONVERSION;
		Gdiplus::Bitmap img(strPath);
		Gdiplus::Bitmap* pThumbnail = static_cast<Gdiplus::Bitmap*>(img.GetThumbnailImage(m_thumSizeX, m_thumSizeY, NULL, NULL));

		// attach the thumbnail bitmap handle to an CBitmap object
		pThumbnail->GetHBITMAP(NULL, &hBmp);
		pImage = new CBitmap();
		pImage->Attach(hBmp);

		// add bitmap to our image list
		m_imageListThumb.Replace(i, pImage, NULL);

		// put item to display
		// set the image file name as item text
		m_listThumbnail.InsertItem(i, imgNameList[i], i);

		// get current item position	 
		m_listThumbnail.GetItemPosition(i, &pt);

		// shift the thumbnail to desired position
		pt.x = nGap + i * (m_thumSizeX + nGap);
		m_listThumbnail.SetItemPosition(i, pt);
		i++;

		delete pImage;
		delete pThumbnail;
	}

	// let's show the new thumbnails
	m_listThumbnail.SetRedraw();

	Gdiplus::GdiplusShutdown(m_gdiplusToken);
}

void CThumbnailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_THUMNAILS, m_listThumbnail);
}

BEGIN_MESSAGE_MAP(CThumbnailDlg, CDialogEx)
END_MESSAGE_MAP()


// CThumbnailDlg 메시지 처리기

BOOL CThumbnailDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_imageListThumb.Create(m_thumSizeX, m_thumSizeY, ILC_COLOR24, 0, 1);
	m_listThumbnail.SetImageList(&m_imageListThumb, LVSIL_NORMAL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
