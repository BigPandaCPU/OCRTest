#include "Document.h"
Document::Document( QString fileName ):
	m_currentFileName(fileName),
	m_oriFileName( fileName ),
	m_sumPage(0),
	m_currentPage(0),
	m_currentRate(100),
	m_currentMode( BROWSE )
{

}

Document::~Document()
{

}

 bool Document::getPage(int page)
{
	return false;
}

bool Document::getPrePage()
{
	return false;
}
bool Document::getNextPage()
{
	return false;
}
bool Document::getFirstPage()
{
	return false;
}
bool Document::getLastPage()
{
	return false;
}
int Document::getSumPage() const
{
	return m_sumPage;
}
int Document::getCurrentPage() const
{
	return m_currentPage;
}

QPixmap  Document::getCurrentPixmap() const
{
	return m_currentPixmap;
}

void Document::setCurrentRate(int rate)
{
	m_currentRate = rate;
}

int Document::getCurrentRate() const
{
	return m_currentRate;
}

QString Document::getCurrentFileName() const
{
	return m_currentFileName;
}

QPixmap Document::getCurrentCheckedPixmap() const
{
	return m_currentCheckedPixmap;
}

void Document::setCurrentMode( int value )
{
	m_currentMode = value;
}
int Document::getCurrentMode() const
{
	return m_currentMode;
}