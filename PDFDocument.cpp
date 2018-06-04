#include <Windows.h>
#include "PDFDocument.h"
#include <QMessageBox>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNodeList>
#include <QtXml/QDomNode>
#include <QPainter>
#include <QRect>
#include <QTime>
#include <QColor>

#include <tchar.h>
#include <QFileInfo>
#include "shlwapi.h"
#include "IVORPageNode.h"
#include "IVORPageNodeIterator.h"

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
		QMessageBox::information(NULL, "Initialized Failed", "Failed to create Interpreter." );
	}
	
	OpenDoc( str );
	m_sumPage = m_pInterpreter->InterpretPageCount();

	m_pCore = CreateVorCore();
	if( m_pCore == NULL )
	{
		QMessageBox::information(NULL,"Initialzed Failed","Failed to create VorCore." );
	}

	getPage(m_currentPage );

}

bool PDFDocument::Bind( IVORInterpreter* pInterpreter, IVORCore* pCore )
{
	pCore->Unbind();

	if (pCore->Bind(pInterpreter) != 0)
		return false;

	IVORStrategy* pStrategy = CreateStrategy();
	if (pStrategy == NULL)
		return false;

	int nSACLevel = m_currentMode;

	switch (nSACLevel)
	{
	case 0:
		pStrategy->ToggleTypePage();
		break;
	case 1:
		{
			pStrategy->ToggleTypePage();
			pStrategy->TogglePara();
		}
		break;
	case 2:
		{
			pStrategy->ToggleTypePage();
			pStrategy->TogglePara();
			pStrategy->ToggleFigure();
		}
		break;
	case 3:
		{
			pStrategy->ToggleTypePage();
			pStrategy->TogglePara();
			pStrategy->ToggleFigure();
			pStrategy->ToggleTable();
		}
		break;
	case 4:
		{
			pStrategy->ToggleTypePage();
			pStrategy->TogglePara();
			pStrategy->ToggleFigure();
			pStrategy->ToggleTable();
			pStrategy->ToggleFormula();
		}
		break;
	case 5:
		pStrategy->ToggleTOC();
		break;
	default:
		break;
	}

	if ( pCore->SetPageStrategy(pStrategy) != 0 )
		return false;
	DeleteStrategy(pStrategy);

	pCore->SetCacheSize(CacheSize);

	return true;
}

void PDFDocument::ExportCheckedAnnotFile( int page, QString outPutFileName )
{
	bool bRet = Bind(m_pInterpreter, m_pCore);

	if( !bRet )
	{
		QMessageBox::information(NULL, "Bind Failed", "Failed to bind Interpreter and Core.");
	}
	int nPageCount = m_pInterpreter->InterpretPageCount();

	//QTime startTime1 = QTime::currentTime();*/

	if(  VORR_OK != m_pCore->AnalysePage( m_currentPage ) )
	{
		QMessageBox::information(NULL, "Analyse Page Failed","Failed to analyse current page.");
	}

	//QTime startTime2 = QTime::currentTime();
	//QString sumTime2 = QString("%1 s").arg( startTime1.msecsTo( startTime2)/1000.0 );
	//QMessageBox::information(NULL, "Run time", sumTime2);

	VOR_WCHAR* str1 = (VOR_WCHAR*)reinterpret_cast<const wchar_t *>(outPutFileName.utf16());  

 	ExportStructureAnnotFile( m_pCore, m_currentPage, str1);
}

void PDFDocument::ExportStructureAnnotFile( IVORCore* pVorCore, int nPageNum, VOR_WCHAR* ipOutputPath )
{
	if (NULL == pVorCore)
	{
		return;
	}

	IVORPageNode* pNode = NULL;
	pVorCore->GetPageRootNode(nPageNum, pNode);

	if (NULL == pNode)
	{
		return;
	}

	// 采用的是导出到内存中
	if( GETRECTMODE == 1 )
	{
		m_RectBoxs.clear();
		ExportLogicInfo(pNode);
	}
	else
	{
		/**********  以下采用的是导出到文件中  *******************/ 
		ContentType aContentTypes[] = {CONTENT_TYPE_PARA, CONTENT_TYPE_FIGURE, CONTENT_TYPE_FORMULA, CONTENT_TYPE_TABLE};

		std::wstring strFileFullName = PathFindFileName(ipOutputPath);

		FILE* pLogFile = _wfopen(ipOutputPath, L"w");
		fprintf(pLogFile, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
		fwprintf(pLogFile, L"<document filename=\"%s\">\n", strFileFullName.c_str());

		ExportLogicInfo(pLogFile, pNode);

		fwprintf(pLogFile, L"</document>");
		fclose(pLogFile);
	}
}

void PDFDocument::ExportLogicInfo(FILE* pLogFile, IVORPageNode* pPageNode)
{
	IVORPageNodeIterator* pPageNodeIterator = VOR_NULL;
	pPageNode->GetChildNodes(pPageNodeIterator);

	while(VOR_NULL != pPageNodeIterator && pPageNodeIterator->HasNext())
	{
		double dScale = 4.0f;

		IVORPageNode* pNode = VOR_NULL;
		pPageNodeIterator->Next(pNode);

		VOR_DRECT rcBBox = pNode->GetBBox();
		if (fabsf(rcBBox.dLeft - DBL_MAX) < VOR_EPSILON || fabsf(rcBBox.dBottom - DBL_MAX) < VOR_EPSILON)
		{
			rcBBox.dLeft = 0.0f;
			rcBBox.dBottom = 0.0f;
		}
		rcBBox = VOR_DRECT(rcBBox.dLeft*dScale, rcBBox.dTop*dScale, rcBBox.dRight*dScale, rcBBox.dBottom*dScale);

		if (pNode->GetType() == CONTENT_TYPE_TEXTLINE){
			fprintf(pLogFile, "<textlineRegion>\n");
			fprintf(pLogFile, "   <Coords points=\"%d,%d %d,%d\"/>\n", (int)(rcBBox.dLeft), (int)(rcBBox.dBottom), (int)(rcBBox.dRight), (int)(rcBBox.dTop));
			fprintf(pLogFile, "</textlineRegion>\n");
		}
		else if (pNode->GetType() == CONTENT_TYPE_FIGURE || pNode->GetType() == CONTENT_TYPE_IMAGE) {
			fprintf(pLogFile, "<figureRegion>\n");
			fprintf(pLogFile, "   <Coords points=\"%d,%d %d,%d\"/>\n", (int)(rcBBox.dLeft), (int)(rcBBox.dBottom), (int)(rcBBox.dRight), (int)(rcBBox.dTop));
			fprintf(pLogFile, "</figureRegion>\n");
		}
		else if (pNode->GetType() == CONTENT_TYPE_TABLE) {
			fprintf(pLogFile, "<tableRegion>\n");
			fprintf(pLogFile, "   <Coords points=\"%d,%d %d,%d\"/>\n", (int)(rcBBox.dLeft), (int)(rcBBox.dBottom), (int)(rcBBox.dRight), (int)(rcBBox.dTop));
			fprintf(pLogFile, "</tableRegion>\n");
		}
		else if (pNode->GetType() == CONTENT_TYPE_FORMULA) {
			fprintf(pLogFile, "<formulaRegion>\n");
			fprintf(pLogFile, "   <Coords points=\"%d,%d %d,%d\"/>\n", (int)(rcBBox.dLeft), (int)(rcBBox.dBottom), (int)(rcBBox.dRight), (int)(rcBBox.dTop));
			fprintf(pLogFile, "</formulaRegion>\n");
		}
#if 0	// POD Annotation No Use
		else if (pNode->GetType() == CONTENT_TYPE_FORMULA_EM) {
			fprintf(pLogFile, "<formulaRegion>\n");
			fprintf(pLogFile, "   <Coords points=\"%d,%d %d,%d\"/>\n", (int)(rcBBox.dLeft), (int)(rcBBox.dBottom), (int)(rcBBox.dRight), (int)(rcBBox.dTop));
			fprintf(pLogFile, "</formulaRegion>\n");
		}
#endif
		else {
			ExportLogicInfo(pLogFile, pNode);
		}
	}
}

void PDFDocument::ExportLogicInfo(IVORPageNode* pPageNode)
{
	IVORPageNodeIterator* pPageNodeIterator = VOR_NULL;
	pPageNode->GetChildNodes(pPageNodeIterator);

	while(VOR_NULL != pPageNodeIterator && pPageNodeIterator->HasNext())
	{
		double dScale = 4.0f;

		IVORPageNode* pNode = VOR_NULL;
		pPageNodeIterator->Next(pNode);

		VOR_DRECT rcBBox = pNode->GetBBox();
		if (fabsf(rcBBox.dLeft - DBL_MAX) < VOR_EPSILON || fabsf(rcBBox.dBottom - DBL_MAX) < VOR_EPSILON)
		{
			rcBBox.dLeft = 0.0f;
			rcBBox.dBottom = 0.0f;
		}
		rcBBox = VOR_DRECT(rcBBox.dLeft*dScale, rcBBox.dTop*dScale, rcBBox.dRight*dScale, rcBBox.dBottom*dScale);

		if (pNode->GetType() == CONTENT_TYPE_TEXTLINE)
		{
			RectBox rectbox;
			rectbox.rect = rcBBox;
			rectbox.type = QString("textlineRegion");
			m_RectBoxs.push_back( rectbox);
		}
		else if (pNode->GetType() == CONTENT_TYPE_FIGURE || pNode->GetType() == CONTENT_TYPE_IMAGE) 
		{
			RectBox rectbox;
			rectbox.rect = rcBBox;
			rectbox.type = QString("figureRegion");
			m_RectBoxs.push_back( rectbox);

		}
		else if (pNode->GetType() == CONTENT_TYPE_TABLE) 
		{
			RectBox rectbox;
			rectbox.rect = rcBBox;
			rectbox.type = QString("tableRegion");
			m_RectBoxs.push_back( rectbox);

		}
		else if (pNode->GetType() == CONTENT_TYPE_FORMULA) 
		{
			RectBox rectbox;
			rectbox.rect = rcBBox;
			rectbox.type = QString("formulaRegion");
			m_RectBoxs.push_back( rectbox);
		}
#if 0	// POD Annotation No Use
		else if (pNode->GetType() == CONTENT_TYPE_FORMULA_EM) {
			fprintf(pLogFile, "<formulaRegion>\n");
			fprintf(pLogFile, "   <Coords points=\"%d,%d %d,%d\"/>\n", (int)(rcBBox.dLeft), (int)(rcBBox.dBottom), (int)(rcBBox.dRight), (int)(rcBBox.dTop));
			fprintf(pLogFile, "</formulaRegion>\n");
		}
#endif
		else {
			ExportLogicInfo(pNode);
		}
	}
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
	QFileInfo fileInfo = QFileInfo( m_oriFileName );
	QString baseName = fileInfo.baseName();
	QString outPutFileName = QString("cache/") + baseName + QString::number(m_currentPage,10) + QString(".png");
	return outPutFileName;
}

QString PDFDocument::createAnnotFileName( int mode )
{
	QFileInfo fileInfo = QFileInfo( m_currentFileName );
	QString baseName = fileInfo.baseName();
	QString suf;
	switch( mode )
	{
		case SACPARA:
			suf = QString("_para");
			break;
		case SACTABLE:
			suf = QString("_table");
			break;
		case SACFORMULA:
			suf = QString("_formula");
			break;
		default:
			suf =QString("");
	}
	QString outPutFileName = QString("cache/annot/") + baseName + suf + QString(".xml");
	return outPutFileName;	

}

bool PDFDocument::getFirstPage()
{
	if( m_currentPage != 1 )
	{
		m_currentPage = 1;

		if( !getPage(m_currentPage) )
			return false;
	}
	return true;
}

bool PDFDocument::getPrePage()
{
	if( m_currentPage >1 )
	{
		m_currentPage = m_currentPage - 1;

		if( !getPage(m_currentPage ) )
			return false;
	}
	return true;
}

bool PDFDocument::getNextPage()
{
	if( m_currentPage < m_sumPage )
	{
		m_currentPage = m_currentPage + 1;

		if( !getPage(m_currentPage) )
			return false;
	}
	return true;
}

bool PDFDocument::getLastPage()
{
	if( m_currentPage != m_sumPage )
	{
		m_currentPage = m_sumPage;

		if( !getPage(m_currentPage) )
			return false;
	}
	return true;
}

bool PDFDocument::getPage(int page )
{
	if( m_currentMode == BROWSE)
	{
		getBrowseModePage( page );
		return true;
	}
	else
	{
		getBrowseModePage(page);
		getCheckModePage(page);
		return true;
	}
}

bool PDFDocument::getBrowseModePage( int page )
{
	map<int, QPixmap>::iterator iter;
	for(iter = m_allPagePixmap.begin(); iter != m_allPagePixmap.end(); iter++ )  
	{
		if( iter->first == page )
		{
			m_currentPixmap = iter->second;
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
			m_currentPixmap = pixMapTemp;  
			m_currentFileName = outPutFileName;
			m_allPagePixmap.insert( pair<int, QPixmap>(page, m_currentPixmap) );
			m_allPageFileName.insert( pair<int, QString>(page, outPutFileName) );
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

bool PDFDocument::getCheckModePage( int page )
{
	int mode = getCurrentMode();
	QString outPutFileName = createAnnotFileName(mode);   
	map<int, QPixmap>::iterator iter;
	const wchar_t * encodedName = reinterpret_cast<const wchar_t *>(outPutFileName.utf16() );
	switch( mode )
	{
		case SACPARA:   
			{
				for(iter = m_allParaCheckedPixmap.begin(); iter != m_allParaCheckedPixmap.end(); iter++ )  
				{
					if( iter->first == page )
					{
						m_currentCheckedPixmap = iter->second;
						return true;
					}
				}
 				ExportCheckedAnnotFile( m_currentPage, outPutFileName );
				if( GETRECTMODE == 1)
				{
					getRectFromInterMemory(SACPARA);
				}
				else
				{
					getRectFromXML(outPutFileName,SACPARA );
				}
				m_allParaCheckedPixmap.insert(   pair<int, QPixmap>(page, m_currentCheckedPixmap) );	
				break;
			}
		case SACTABLE:
			{
				for(iter = m_allTableCheckedPixmap.begin(); iter != m_allTableCheckedPixmap.end(); iter++ )  
				{
					if( iter->first == page ) 
					{
						m_currentCheckedPixmap = iter->second;
						return true;
					}
				}
 				ExportCheckedAnnotFile( m_currentPage, outPutFileName );
				if( GETRECTMODE == 1)
				{
					getRectFromInterMemory(SACTABLE);
				}
				else
				{
					getRectFromXML(outPutFileName,SACTABLE );
				}
				m_allTableCheckedPixmap.insert(   pair<int, QPixmap>(page, m_currentCheckedPixmap) );	
				break;
			}
		case SACFORMULA:
			{ 
				for(iter = m_allFormulaCheckedPixmap.begin(); iter != m_allFormulaCheckedPixmap.end(); iter++ )  
				{
					if( iter->first == page ) 
					{
						m_currentCheckedPixmap = iter->second;
						return true;
					}
				}
 				ExportCheckedAnnotFile( m_currentPage, outPutFileName );
				if( GETRECTMODE == 1)
				{
					getRectFromInterMemory(SACFORMULA);
				}
				else
				{
					getRectFromXML(outPutFileName,SACFORMULA );
				}
				m_allFormulaCheckedPixmap.insert(   pair<int, QPixmap>(page, m_currentCheckedPixmap) );	
				break;
			}
		default:
			{

			}
	}
	return true;
}

bool PDFDocument::getRectFromXML(QString fileName,int model)
{
	QFile file(fileName);
	if( !file.open( QIODevice::ReadOnly) )
	{
		file.close();
		QMessageBox::information(NULL,"Open Failed","Open xml file failed!");
		return false;
	}

	QDomDocument doc;
	QDomElement  eleRoot;
	if( !doc.setContent( &file ) )
	{
		file.close();
		return false;
	}
	file.close();
	//查询记录
	eleRoot = doc.documentElement();
	QString tagName;
	QPen pen;
	if( model == SACPARA )
	{
		tagName = QString("textlineRegion");
		pen = QPen(QColor(220,220,220), 2, Qt::SolidLine);
	}
	else if( model == SACTABLE )
	{
		tagName = QString("tableRegion"); 
		pen = QPen(Qt::blue, 2, Qt::SolidLine);
	}
	else
	{
		tagName = QString("formulaRegion");
		pen = QPen(Qt::red, 2, Qt::SolidLine);
	}


	QDomNodeList  nodeList = eleRoot.childNodes();
	std::vector<Points> rectPoints;
	Points points;


	for( int i = 0; i < nodeList.count(); i++ )
	{
		QDomNode domNode = nodeList.item(i);
		
		QDomElement element = domNode.toElement();

		if( element.tagName() == tagName )
		{
			QDomNode    domNode2 = domNode.firstChild();
			QDomElement element2 = domNode2.toElement();
			QString str = element2.attribute("points");
			QStringList lst;  
			lst = str.split(" ");

			QStringList lstr1 = lst.at(0).split(",");
			QStringList lstr2 = lst.at(1).split(",");
			points.left = lstr1.at(0).toInt();
			points.top  = lstr1.at(1).toInt();
			points.right = lstr2.at(0).toInt();
			points.down  = lstr2.at(1).toInt();
			rectPoints.push_back(points);
		}
	}
	m_currentCheckedPixmap = m_currentPixmap;
	int height = m_currentPixmap.height();

	QPainter p(&m_currentCheckedPixmap); 
	p.setRenderHint(QPainter::SmoothPixmapTransform);  
    p.setPen(pen);

	for( int i = 0; i < rectPoints.size(); i ++ )
	{
		Points point = rectPoints[i];
		point.top  = height - point.top;
		point.down = height - point.down;
		p.drawRect(QRect( point.left, point.top, point.right - point.left, point.down - point.top) );
	}
	p.end();
	return true;
}

bool PDFDocument::getRectFromInterMemory(int model)
{
	QString tagName;
	QPen pen;
	if( model == SACPARA )
	{
		tagName = QString("textlineRegion");
		pen = QPen(QColor(220,220,220), 2, Qt::SolidLine);
	}
	else if( model == SACTABLE )
	{
		tagName = QString("tableRegion"); 
		pen = QPen(Qt::blue, 2, Qt::SolidLine);
	}
	else
	{
		tagName = QString("formulaRegion");
		pen = QPen(Qt::red, 2, Qt::SolidLine);
	}
	
	m_currentCheckedPixmap = m_currentPixmap;
	int height = m_currentPixmap.height();

	QPainter p(&m_currentCheckedPixmap); 
	p.setRenderHint(QPainter::SmoothPixmapTransform);  
	p.setPen(pen);

	for( int i = 0; i < m_RectBoxs.size() ; i++ )
	{
		if( m_RectBoxs[i].type == tagName )
		{
			VOR_DRECT rect = m_RectBoxs[i].rect;
			rect.dTop    = height - rect.dTop;
			rect.dBottom = height - rect.dBottom;
			
			p.drawRect(QRect( rect.dLeft, rect.dTop, rect.dRight - rect.dLeft, rect.dBottom - rect.dTop) );
		}
	}
	p.end();
	return true;
}

PDFDocument::~PDFDocument()
{
	CloseDoc();
	m_pFactory->DeleteInterpreter(m_pInterpreter);
}