#ifndef DOCUMENT_H
#define DOCUMENT_H

#include<iostream>
#include<QString>
#include<QPixmap>

enum CurrentMode{ BROWSE = -1, SACPARA = 1, SACTABLE = 3, SACFORMULA = 4 };
class Document
{
	public:
	Document(QString fileName);
	~Document();
	virtual bool	getPage(int page);
	virtual bool	getPrePage();
	virtual bool	getNextPage();
	virtual bool	getFirstPage();
	virtual bool	getLastPage();
	virtual int     getSumPage() const;
	virtual int     getCurrentPage() const;
	virtual QPixmap getCurrentPixmap() const;
	virtual QString getCurrentFileName() const;

	virtual QPixmap getCurrentCheckedPixmap() const;

	int             getCurrentRate() const;
	void            setCurrentRate(int rate);

	int             getCurrentMode()const;
	void            setCurrentMode(int value);

protected:
	int       m_sumPage;
	int       m_currentPage;
	QString   m_currentFileName;
	QString   m_oriFileName;
	QPixmap   m_currentPixmap;
	QPixmap   m_currentCheckedPixmap;


	int       m_currentRate;
	int       m_currentMode;

};

#endif