#ifndef SUBWIDGET_H
#define SUBWIDGET_H
#include <QWidget>
#include <QHBoxLayout>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QString>
#include <QTabWidget>
#include"Document.h"
#include"ImageDocument.h"
#include"PDFDocument.h"


class SubWidget:public QWidget
{
Q_OBJECT
public:
	SubWidget( QString fileName );
	~SubWidget();
	void addWidget(QPixmap pixMap);
	QPixmap getCurrentPixmap() const;
	QPixmap getCurrentCheckedPixmap() const;

	QString getCurrentFileName();
	bool isChecked() const;

	QGraphicsScene * getOriScene();
	QGraphicsScene * getCheckedScene();
	void updateWidget( int value );
	void updateWidget( );
	void setParentTabWidget( QTabWidget *widget);
	QTabWidget *getParentTabWidget() const;

	int        getCurrentRate() const;
	
	int        getDocType() const;
	void       setDocType(int type);
	Document*  getDocument() const;
	bool       createDocument(QString fileName);

	
signals:
	void    subWidgetChangedSignal();

public slots:
	void    getPrePageSlot();
	void    getNextPageSlot();
	void    getFirstPageSlot();
	void    getLastPageSlot();
	void    setParaCheckedModeSlot();
	void    setTableCheckedModeSlot();
	void    setFormulaCheckedModeSlot();

	
private:
	QHBoxLayout       *m_hLayout;
	QString           m_currentFileName;
	QPixmap           m_currentPixmap;
	QPixmap           m_currentCheckedPixmap;
	QGraphicsScene    *m_oriScene;
	QGraphicsScene    *m_checkedScene;
	Document          *m_Doc;
	QGraphicsView     *m_checkedView;
	

	//int               m_currentRate;
	int getMinRate( int win_w, int win_h, int img_w, int img_h );
	QTabWidget        *m_parentTabWidget;

	bool              m_isChecked;
};
#endif


