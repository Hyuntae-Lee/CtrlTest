// ThumbnailDlg.cpp: 구현 파일
//

#include "pch.h"
#include "CtrlTest.h"
#include "ThumbnailDlg.h"
#include "afxdialogex.h"
#include <gdiplus.h>

// CThumbnailDlg 대화 상자

IMPLEMENT_DYNAMIC(CThumbnailDlg, CDialogEx)

CThumbnailDlg::CThumbnailDlg(CWnd* pParent, int thumSizeX, int thumSizeY, bool bShowName, int rows, int cols, SIZE gap)
	: CDialogEx(IDD_DLG_THUMNAIL, pParent)
	, kImageSizeX(thumSizeX)
	, kImageSizeY(thumSizeY)
	, kShowName(bShowName)
	, kRows(rows)
	, kCols(cols)
	, kGap(gap)
{
}

CThumbnailDlg::~CThumbnailDlg()
{
}

bool CThumbnailDlg::loadImages(CString szDirPath)
{
	// get image list
	vector<CString> imageNameList;
	if (!getImageFileNames(imageNameList, szDirPath)) {
		return false;
	}

	// scrollbar
	{
		auto nRealRows = (int)(imageNameList.size() / kCols);

		m_scrollBarThumnail.SetScrollRange(0, (nRealRows - kRows) * kImageSizeY);
		m_scrollBarThumnail.SetScrollPos(0);
	}
	
	CThumbnailDlg::drawThumbnailList(szDirPath, imageNameList, kGap);

	return true;
}

void CThumbnailDlg::drawThumbnailList(CString szDirPath, vector<CString> imageNameList,
	SIZE gap)
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

		// position, name
		POINT pos;
		CString szName;
		pos.x = gap.cx + (kImageSizeX + gap.cx) * (i % kCols);

		if (kShowName) {
			pos.y = (kImageSizeY + gap.cy + kLabelSizeY) * (int)(i / kCols);

			szName = imageNameList[i];
		}
		else {
			pos.y = (kImageSizeY + gap.cy) * (int)(i / kCols);

			szName = _T("");
		}

		drawThumbnail(kImageSizeX, kImageSizeY, i, szPath, szName, pos);
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
		Gdiplus::Bitmap* pThumbnail = static_cast<Gdiplus::Bitmap*>(img.GetThumbnailImage(kImageSizeX, kImageSizeY, NULL, NULL));

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
	DDX_Control(pDX, IDC_SCROLLBAR_THUMNAIL, m_scrollBarThumnail);
}

BEGIN_MESSAGE_MAP(CThumbnailDlg, CDialogEx)
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


// CThumbnailDlg 메시지 처리기

BOOL CThumbnailDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_imageListThumb.Create(kImageSizeX, kImageSizeY, ILC_COLOR24, 0, 1);
	m_listCtrlThumbnail.SetImageList(&m_imageListThumb, LVSIL_NORMAL);

	m_listCtrlThumbnail.ShowScrollBar(0, FALSE);
	m_listCtrlThumbnail.ShowScrollBar(1, FALSE);

	// set window size
	{
		// width
		int wndW = kImageSizeX * kCols + kGap.cx * (kCols + 1);

		// height
		int wndH;
		if (kShowName) {
			wndH = static_cast<int>((kImageSizeY + kLabelSizeY) * kRows + kGap.cy * (kRows - 1));
		}
		else {
			wndH = static_cast<int>(kImageSizeY * kRows + kGap.cy * (kRows - 1));
		}

		// 
		CRect rtClient;
		m_listCtrlThumbnail.GetClientRect(&rtClient);
		rtClient.SetRect(rtClient.left, rtClient.top, rtClient.left + wndW, rtClient.top + wndH);

		m_listCtrlThumbnail.MoveWindow(&rtClient);

		CRect rtScrollbar;
		rtScrollbar.SetRect(rtClient.right, rtClient.top, rtClient.right + 20, rtClient.bottom);

		m_scrollBarThumnail.MoveWindow(rtScrollbar);
	}

	return TRUE;
}


void CThumbnailDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar == &m_scrollBarThumnail) {
		if (nPos == 0) {

			// TODO : https://jhb.kr/202 참고

			//m_listCtrlThumbnail.Scroll(CSize(0, kImageSizeY));



			SCROLLINFO  scrinfo;
			// 스크롤바 정보를 가져온다.
			if (pScrollBar->GetScrollInfo(&scrinfo))
			{
				switch (nSBCode)
				{
				case SB_PAGEUP:   // 스크롤 바의 위쪽 바를 클릭
					scrinfo.nPos -= scrinfo.nPage;
					break;
				case SB_PAGEDOWN:  // 스크롤 바의 아래쪽 바를 클릭
					scrinfo.nPos += scrinfo.nPage;
					break;
				case SB_LINEUP:   // 스크롤 바의 위쪽 화살표를 클릭
					scrinfo.nPos -= scrinfo.nPage / 10;
					break;
				case SB_LINEDOWN:  // 스크롤 바의 아래쪽 화살표를 클릭
					scrinfo.nPos += scrinfo.nPage / 10;
					break;
				case SB_THUMBPOSITION: // 스크롤바의 트랙이 움직이고 나서
				case SB_THUMBTRACK:  // 스크롤바의 트랙이 움직이는 동안
					scrinfo.nPos = scrinfo.nTrackPos;   // 16bit값 이상을 사용
					break;
				}

				{
					CString szLog;
					szLog.Format(_T("######## scroll position is %d"), scrinfo.nPos);
					OutputDebugString(szLog);
				}

				// 스크롤바의 위치를 변경한다.
				pScrollBar->SetScrollPos(scrinfo.nPos);
			}
		}
	}

	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}
