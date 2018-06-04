#ifndef PDFDOCUMENT_H
#define PDFDOCUMENT_H
#include<iostream>
#include<QString>
#include<map>
#include<QPixmap>

#include "VORDef.h"
#include "VORRetCode.h"
#include "VORMacro.h"
#include "VORPublicFunction.h"
#include "VORAdaptType.h"

#include "Interpreter.h"

#include "XELFDef.h"
#include "VORXMLDef.h"

// SAC Core
#include "VORAPI.h"
#include "IVorCore.h"

#include "KernelWrapper.h"
#include "Document.h"
#define	 CacheSize	200
#define  GETRECTMODE 1   //获取rect的方式，0表示从xml文件中获取,1表示从内存中获取

using namespace std;
typedef struct Points
{
	int left;
	int top;
	int right;
	int down;
}Points;

typedef struct RectBox
{
	VOR_DRECT rect;
	QString   type;
}RectBox;


class PDFDocument:public Document
{
public:
	PDFDocument(QString fileName);
	~PDFDocument();
	bool	getPage(int page);
	bool	getPrePage();
	bool	getNextPage();
	bool	getFirstPage();
	bool	getLastPage();
	int     getSumPage() const;
	int     getCurrentPage() const;
	QPixmap getCurrentPixmap() const;
	QString getCurrentFileName() const;
	QPixmap getCurrentCheckedPixmap() const;
	bool    getRectFromXML(QString fileName,int model);
	bool    getRectFromInterMemory(int mode);


private:
	map<int, QString>   m_allPageFileName;
	map<int, QPixmap>   m_allPagePixmap;
	map<int, QPixmap>   m_allParaCheckedPixmap;
	map<int, QPixmap>   m_allTableCheckedPixmap;
	map<int, QPixmap>   m_allFormulaCheckedPixmap;

	CInterpreterFactory*   m_pFactory;
	IVORInterpreter*       m_pInterpreter;
	IVORCore*              m_pCore;

	QString createSaveFileName(int page);
	QString createAnnotFileName( int mode );
	

	bool Bind(IVORInterpreter* pInterpreter, IVORCore* pCore);
	void ExportCheckedAnnotFile( int page, QString outPutFileName );
	void ExportStructureAnnotFile( IVORCore* pVorCore,int nPageNum, VOR_WCHAR* ipOutputPath  );
	void ExportLogicInfo(FILE* pLogFile, IVORPageNode* pPageNode);
	void ExportLogicInfo(IVORPageNode* pPageNode);

	bool getBrowseModePage( int page );
	bool getCheckModePage( int page );
	std::vector< RectBox >   m_RectBoxs;
};
#endif




