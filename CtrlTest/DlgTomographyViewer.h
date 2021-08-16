#pragma once

#include <vector>
#include <string>

using namespace std;

class CDlgTomographyViewer : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgTomographyViewer)

	const wstring kStrKeyCScan0 = L"cscan_0";
	const wstring kStrKeyCScan1 = L"cscan_1";
	const wstring kStrKeyBScanCnt = L"bscan_cnt";
	const wstring kStrKeyBScanFilePrefix = L"bscan_file_prefix";
	const wstring kStrKeyBScanFileExt = L"bscna_file_ext";

	const int kScrollMargin = 64;

	const int kCScanImageSizeX;
	const int kCScanImageSizeY;
	const int kBScanImageSizeX;
	const int kBScanImageSizeY;
	const int kBScanCols;
	const int kBScanRows;

public:
	CDlgTomographyViewer(CWnd* pParent, SIZE cscanImageSize, SIZE bscanImageSize, int bscanRows,
		int bscanCols);
	virtual ~CDlgTomographyViewer();

	bool loadImages(CString szDirPath, CString szInfoFileName);

	enum { IDD = IDD_DLG_TOMOGRAPHYVIEWER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	void drawBScanImageList(POINT pos, CString szDirPath, vector<CString> imageNameList);
	void drawBScanImage(int thumSizeX, int thumSizeY, int index, CString szImagePath, CString szName,
		POINT pos);
	void drawCScanImage(CStatic& view, CString szImagePath);
	bool getImageFileNames(vector<CString>& out_list, CString szDirPath, vector<CString> excludes);
	bool readFile(wstring& out_value, wstring strPath);

	DECLARE_MESSAGE_MAP()

protected:
	CListCtrl m_listCtrlBScans;
	CImageList m_imageListBScans;
	CStatic m_imageCScan0;
	CStatic m_imageCScan1;

	vector<CString> m_imageNameListBScans;
	vector<CString> m_imageNameListCScans;

protected:
	virtual BOOL OnInitDialog();
};
