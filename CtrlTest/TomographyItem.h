#pragma once

#include <string>

using namespace std;

class CTomographyItem
{
public:
	CTomographyItem(wstring strDirPath, wstring strFileName, int index, wstring strName);
	~CTomographyItem();

	auto getName()->wstring;
	auto getIndex()->int;

protected:
	wstring m_strDirPath;
	wstring m_strFileName;
	int m_index;
	wstring m_strName;
};

