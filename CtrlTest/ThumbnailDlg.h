#pragma once

#include <vector>

#define DIR_PATH _T("G:/Projects/imageSamples")

using namespace std;

class CThumbnailDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CThumbnailDlg)

public:
	CThumbnailDlg(CWnd* pParent, long thumSizeX, long thumSizeY);
	virtual ~CThumbnailDlg();

	bool setImageDir(CString dzDirPath);

	enum { IDD = IDD_DLG_THUMNAIL };

protected:
	bool getImageFileNames(vector<CString>& out_list, CString szDirPath);
	void drawThumbnails(CString szDirPath, vector<CString> imgNameList);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	CListCtrl m_listThumbnail;
	CImageList m_imageListThumb;		// image list holding the thumbnails
	vector<CString>	m_imageNameList;		// vector holding the image names

	long m_thumSizeX;
	long m_thumSizeY;

	ULONG_PTR m_gdiplusToken;

protected:
	virtual BOOL OnInitDialog();
};
