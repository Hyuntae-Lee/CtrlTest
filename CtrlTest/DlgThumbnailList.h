#pragma once

#include <vector>
#include <string>

using namespace std;

class CDlgThumbnailList : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgThumbnailList)

	const int kLabelSizeY = 24;
	const int kScrollMargin = 59;
	const int kScrollMarginForNoName = 64;

	const int kImageSizeX;
	const int kImageSizeY;
	const int kCols;
	const int kRows;
	const bool kShowName;
	const SIZE kMarginForItems;

public:
	CDlgThumbnailList(CWnd* pParent, int thumSizeX, int thumSizeY, bool bShowName, int rows,
		int cols, SIZE marginForItems);
	virtual ~CDlgThumbnailList();

	bool loadImages(CString dzDirPath);

	enum { IDD = IDD_DLG_THUMNAIL };

protected:
	void drawThumbnailList(CString szDirPath, vector<CString> imageNameList);
	void drawThumbnail(int thumSizeX, int thumSizeY, int index, CString szImagePath, CString szName,
		POINT pos);
	bool getImageFileNames(vector<CString>& out_list, CString szDirPath);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	CListCtrl m_listCtrlThumbnail;
	CImageList m_imageListThumb;		// image list holding the thumbnails

protected:
	virtual BOOL OnInitDialog();
};
