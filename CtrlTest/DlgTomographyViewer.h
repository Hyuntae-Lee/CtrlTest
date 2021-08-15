#pragma once

#include <vector>
#include <string>
#include "cpprest/json.h"

using namespace std;

class CDlgTomographyViewer : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgTomographyViewer)

	const int kScrollMargin = 59;
	const int kScrollMarginForNoName = 64;

	const int kImageSizeX;
	const int kImageSizeY;
	const int kCols;
	const int kRows;

public:
	CDlgTomographyViewer(CWnd* pParent, SIZE thumbImageSize, int rows, int cols);
	virtual ~CDlgTomographyViewer();

	bool loadImages(CString szDirPath, CString szInfoFileName);

	enum { IDD = IDD_DLG_TOMOGRAPHYVIEWER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	void drawThumbnailList(CString szDirPath, vector<CString> imageNameList);
	void drawThumbnail(int thumSizeX, int thumSizeY, int index, CString szImagePath, CString szName,
		POINT pos);
	bool getImageFileNames(vector<CString>& out_list, CString szDirPath);
	bool loadJsonFile(web::json::value& out_value, wstring strPath);

	DECLARE_MESSAGE_MAP()

protected:
	CListCtrl m_listCtrlBScans;
	CImageList m_imageListBScans;
	CStatic m_imageCScan0;
	CStatic m_imageCScan1;

	vector<CString> imageNameListBScans;

protected:
	virtual BOOL OnInitDialog();
};
