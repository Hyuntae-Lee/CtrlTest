#pragma once

#include <vector>
#include <string>

using namespace std;

class CThumbnailDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CThumbnailDlg)

	const int kLabelSizeY = 24;

	const int kImageSizeX;
	const int kImageSizeY;
	const int kCols;
	const int kRows;
	const bool kShowName;
	const SIZE kGap;

public:
	CThumbnailDlg(CWnd* pParent, int thumSizeX, int thumSizeY, bool bShowName, int rows, int cols, SIZE gap);
	virtual ~CThumbnailDlg();

	bool loadImages(CString dzDirPath);

	enum { IDD = IDD_DLG_THUMNAIL };

protected:
	void drawThumbnailList(CString szDirPath, vector<CString> imageNameList, SIZE gap);
	void drawThumbnail(int thumSizeX, int thumSizeY, int index, CString szImagePath, CString szName,
		POINT pos);
	bool getImageFileNames(vector<CString>& out_list, CString szDirPath);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	CListCtrl m_listCtrlThumbnail;
	CScrollBar m_scrollBarThumnail;
	CImageList m_imageListThumb;		// image list holding the thumbnails

protected:
	virtual BOOL OnInitDialog();

public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
