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

using namespace std;

class PDFDocument:public Document
{
public:
	PDFDocument(QString fileName);
	~PDFDocument();
	bool	getPage(int page, QPixmap &pixMap);
	bool	getPrePage();
	bool	getNextPage();
	bool	getFirstPage();
	bool	getLastPage();
	int     getSumPage() const;
	int     getCurrentPage() const;
	QPixmap getCurrentPixmap() const;
	QString getCurrentFileName() const;
	QPixmap getCurrentCheckedPixmap() const;

private:
	map<int, QString>   m_allPageFileName;
	map<int, QPixmap>   m_allPagePixmap;
	map<int, QPixmap>   m_allTableCheckedPixmap;
	map<int, QPimxap>   m_allParaCheckedPixmap;
	map<int, QPixmap>   m_allCheckedPixmap;

	CInterpreterFactory*   m_pFactory;
	IVORInterpreter*       m_pInterpreter;

	QString createSaveFileName(int page);
};
#endif




