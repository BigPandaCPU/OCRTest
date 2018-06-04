#include "ImageDocument.h"
#include <QMessageBox>
ImageDocument::ImageDocument(QString fileName):Document(fileName)
{
	m_currentPage = 1;
	m_sumPage     = 1;
	
	if( !m_currentPixmap.load(fileName) )
	{
		QMessageBox::warning( NULL, "Load faild", "ImageDocument Load Image Failed!");
	}
}

bool ImageDocument::getPage(int page, QPixmap &pixMap)
{
	return true;
}
bool ImageDocument::getPrePage()
{
	return true;
}
bool ImageDocument::getNextPage()
{
	return true;
}
bool ImageDocument::getFirstPage()
{
	return true;
}

bool ImageDocument::getLastPage()
{
	return true;
}
int ImageDocument::getSumPage() const
{
	return true;
}
int ImageDocument::getCurrentPage() const
{
	return m_currentPage;
}
QPixmap ImageDocument::getCurrentPixmap() const
{
	return m_currentPixmap;
}

QPixmap ImageDocument::getCurrentCheckedPixmap() const
{
	return m_currentCheckedPixmap;
}

QString ImageDocument::getCurrentFileName() const
{
	return m_currentFileName;
}