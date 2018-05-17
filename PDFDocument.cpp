#include <Windows.h>
#include "PDFDocument.h"
#include <QMessageBox>
#include <tchar.h>
#include <QFileInfo>


PDFDocument::PDFDocument( QString fileName):Document(fileName),
	m_pFactory(NULL)
{
	m_currentPage = 1;
	if( NULL == m_pFactory )
	{
		m_pFactory = new CInterpreterFactory();
	}

	Init();


	LPCWSTR str = reinterpret_cast<const wchar_t *>(fileName.utf16());  
	m_pInterpreter = m_pFactory->CreateInterpreter( str );
	if( NULL == m_pInterpreter )
	{
		QMessageBox::information(NULL, "Load Failed", "Failed to open the file!" );
	}
	
	OpenDoc( str );
	m_sumPage = m_pInterpreter->InterpretPageCount();

	getPage(m_currentPage, m_currentPixmap );

}

QPixmap PDFDocument::getCurrentPixmap() const
{
	return m_currentPixmap;
}
QPixmap PDFDocument::getCurrentCheckedPixmap() const
{
	return m_currentCheckedPixmap;
}
QString PDFDocument::getCurrentFileName() const
{
	return m_currentFileName;
}


int PDFDocument::getCurrentPage() const
{
	return m_currentPage;
}

int PDFDocument::getSumPage() const
{
	return m_sumPage;
}

QString PDFDocument::createSaveFileName(int page)
{
	QFileInfo fileInfo = QFileInfo( m_currentFileName );
	QString baseName = fileInfo.baseName();
	QString outPutFileName = QString("cache/") + baseName + QString::number(m_currentPage,10) + QString(".png");
	return outPutFileName;
}

bool PDFDocument::getFirstPage()
{
	if( m_currentPage != 1 )
	{
		m_currentPage = 1;

		if( !getPage(m_currentPage, m_currentPixmap ) )
			return false;
	}
	return true;
}

bool PDFDocument::getPrePage()
{
	if( m_currentPage >1 )
	{
		m_currentPage = m_currentPage - 1;

		if( !getPage(m_currentPage, m_currentPixmap ) )
			return false;
	}
	return true;
}

bool PDFDocument::getNextPage()
{
	if( m_currentPage < m_sumPage )
	{
		m_currentPage = m_currentPage + 1;

		if( !getPage(m_currentPage, m_currentPixmap ) )
			return false;
	}
	return true;
}

bool PDFDocument::getLastPage()
{
	if( m_currentPage != m_sumPage )
	{
		m_currentPage = m_sumPage;

		if( !getPage(m_currentPage, m_currentPixmap ) )
			return false;
	}
	return true;
}

bool PDFDocument::getPage(int page, QPixmap& pixmap )
{
	map<int, QPixmap>::iterator iter;
	for(iter = m_allPagePixmap.begin(); iter != m_allPagePixmap.end(); iter++ )  
	{
		if( iter->first == page )
		{
			pixmap = iter->second;
			return true;
		}
	}
	
	QString outPutFileName = createSaveFileName(page);
	const wchar_t * encodedName = reinterpret_cast<const wchar_t *>(outPutFileName.utf16() );
 	bool isExport = ExportPageBitmap(page, encodedName );

	if( isExport )
	{
		QPixmap pixMapTemp;
		if( pixMapTemp.load( outPutFileName ) )
		{
			pixmap = pixMapTemp;  
			m_allPagePixmap.insert( pair<int, QPixmap>(page, pixmap) );
			return true;
		}
		else
		{
			QMessageBox::information( NULL, "Load Failed","Failed to load the File!");
			return false;
		}
	}
	else
	{
		return false;
	}
}

PDFDocument::~PDFDocument()
{
	CloseDoc();
	m_pFactory->DeleteInterpreter(m_pInterpreter);
}