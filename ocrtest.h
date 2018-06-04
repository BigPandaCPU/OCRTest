#ifndef OCRTEST_H
#define OCRTEST_H

#include <QtWidgets/QMainWindow>
//#include "ui_ocrtest.h"
#include <QAction>
#include <QToolBar.h>
#include <QSpinBox>
#include <QSlider>
#include <QPushButton>
#include <QLineEdit>
#include <QMenu>



#include "CentralWidget.h"


class OCRTest : public QMainWindow
{
	Q_OBJECT

public:
	OCRTest(QWidget *parent = 0);
	~OCRTest();
	
public slots:
	void openFileSlot();
	void autoCheckSlot();
	void imageScaledSlot(int);
	void resizeEvent(QResizeEvent * event);
	void updateStatusBar();

private:
	CentralWidget     *m_centralWidget;

	QString           getFileNameFromPath( QString filePath );

	QMenu             *m_fileMenu;
	QMenu             *m_sacMenu;

	QToolBar          *m_fileTool;
	QToolBar          *m_sacTool;
	 
	QAction           *m_openFileAction;
	QAction           *m_sacTableCheckAction;
	QAction           *m_sacFormulaCheckAction;
	QAction           *m_sacParaCheckAction;



	QSpinBox          *m_imageSpinBox;
	QSlider           *m_imageSlider; 

	QPushButton       *m_prePageBtn;
	QPushButton       *m_nextPageBtn;
	QPushButton       *m_firstPageBtn;
	QPushButton       *m_lastPageBtn;
	QLineEdit         *m_pageLineEdit;

	void              createActions();
	void              createMenus();
	void              createToolBars();
	void              createStatusBars();

public slots:
	void              updateStatusSlot();
	void              sacParaCheckSlot();
	void              sacTableCheckSlot();
	void              sacFormulaCheckSlot();
};

#endif // OCRTEST_H
