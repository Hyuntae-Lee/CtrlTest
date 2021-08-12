#pragma once

#include <vector>
#include <string>

using namespace std;

class CThumbnailDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CThumbnailDlg)

public:
	CThumbnailDlg(CWnd* pParent, int thumSizeX, int thumSizeY, int gapBetweenItems);
	virtual ~CThumbnailDlg();

	bool setImageDir(CString dzDirPath);

	enum { IDD = IDD_DLG_THUMNAIL };

protected:
	bool getImageFileNames(vector<CString>& out_list, CString szDirPath);
	void drawThumbnailList(CString szDirPath, vector<CString> imageNameList);
	void drawThumbnail(int thumSizeX, int thumSizeY, int gapBetweenItems, int index, wstring strImagePath, wstring strName);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	CListCtrl m_listCtrlThumbnail;
	CImageList m_imageListThumb;		// image list holding the thumbnails

	int m_thumSizeX;
	int m_thumSizeY;
	int m_gapBetweenItems;

	ULONG_PTR m_gdiplusToken = NULL;

protected:
	virtual BOOL OnInitDialog();
};
