#pragma once

#include <vector>
#include "TomographyItem.h"

using namespace std;

class CTomographyItemListViewer : public CDialogEx
{
	const wstring kStrKeyCScan0 = L"cscan_0";
	const wstring kStrKeyCScan1 = L"cscan_1";
	const wstring kStrKeyBScanCnt = L"bscan_cnt";
	const wstring kStrKeyBScanFilePrefix = L"bscan_file_prefix";
	const wstring kStrKeyBScanFileExt = L"bscna_file_ext";

	const int kNBScanRows;
	const int kNBScanCols;
	const int kBScanItemW;
	const int kBScanItemH;

	DECLARE_DYNAMIC(CTomographyItemListViewer)

public:
	CTomographyItemListViewer(CWnd* pParent, SIZE nBScanItems, SIZE iBScanItemSize);
	virtual ~CTomographyItemListViewer();

	bool loadImages(wstring strDirPath, wstring strInfoFileName);

	enum { IDD = IDD_DLG_TOMOGRAPHYITEMLISTVIEWER };

protected:
	void addItem(wstring strFileName, wstring strName);
	bool readFile(wstring& out_value, wstring strPath);
	bool getImageFileNames(vector<CString>& out_list, CString szDirPath, vector<CString> excludes);
	void updateScrollInfo();
	void drawItem(CPaintDC& dc, CTomographyItem& item, CRect rect);

protected:
	vector<CTomographyItem> m_bscanItemList;
	CScrollBar m_scrollbar;
	vector<CRect> m_bscanItemRectList;
	vector<CString> m_imageNameListCScans;
	wstring m_strDirPath;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
};
