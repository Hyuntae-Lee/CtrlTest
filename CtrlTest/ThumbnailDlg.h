#pragma once

#include <vector>
#include <string>
#include "ImageListViewer.h"

using namespace std;

class CThumbnailDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CThumbnailDlg)

public:
	CThumbnailDlg(CWnd* pParent, int thumSizeX, int thumSizeY);
	virtual ~CThumbnailDlg();

	bool setImageDir(CString dzDirPath);

	enum { IDD = IDD_DLG_THUMNAIL };

protected:
	void drawThumbnailList(CString szDirPath, vector<CString> imageNameList, int nCols, SIZE gap,
		bool bShowName);
	void drawThumbnail(int thumSizeX, int thumSizeY, int index, CString szImagePath, CString szName,
		POINT pos);

	bool getImageFileNames(vector<CString>& out_list, CString szDirPath);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	CImageListViewer m_listCtrlThumbnail;
	CImageList m_imageListThumb;		// image list holding the thumbnails

	int m_thumSizeX;
	int m_thumSizeY;

protected:
	virtual BOOL OnInitDialog();
};
