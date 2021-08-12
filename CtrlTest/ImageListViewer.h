#pragma once


// CImageListViewer

class CImageListViewer : public CListCtrl
{
	DECLARE_DYNAMIC(CImageListViewer)

public:
	CImageListViewer();
	virtual ~CImageListViewer();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
};


