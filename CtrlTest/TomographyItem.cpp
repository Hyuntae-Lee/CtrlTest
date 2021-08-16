#include "pch.h"
#include "TomographyItem.h"

CTomographyItem::CTomographyItem(wstring strDirPath, wstring strFileName, int index, wstring strName)
{
	m_strDirPath = strDirPath;
	m_strFileName = strFileName;
	m_index = index;
	m_strName = strName;
}

CTomographyItem::~CTomographyItem()
{

}

auto CTomographyItem::getName()->wstring
{
	return m_strName;
}

auto CTomographyItem::getIndex()->int
{
	return m_index;
}

auto CTomographyItem::getImagePath()->wstring
{
	return m_strDirPath + L"\\" + m_strFileName;
}
