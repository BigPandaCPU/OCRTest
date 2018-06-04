#include "SubWidget.h"
#include <QMessageBox>
#include <QLayoutItem>
#include<QFileInfo>

#include <QSize>


SubWidget::SubWidget( QString fileName):
	m_isChecked(false),
	m_parentTabWidget(NULL),
	m_checkedView(NULL),
	m_checkedScene(NULL)
{
	QRect rect = geometry();
    
	QFileInfo  fileInfo = QFileInfo( fileName );

	createDocument(fileName);

	m_currentFileName = m_Doc->getCurrentFileName();
	m_currentPixmap   = m_Doc->getCurrentPixmap();

	QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem;
	pixmapItem->setPixmap( m_currentPixmap );
	m_oriScene = new QGraphicsScene;
	m_oriScene->addItem( pixmapItem );
	m_oriScene->setBackgroundBrush( Qt::gray );
	
	QGraphicsView *view = new QGraphicsView();
	view->setScene( m_oriScene );
	view->centerOn(0,0);

	m_hLayout = new QHBoxLayout;
	m_hLayout->addWidget(view);

	this->setLayout( m_hLayout );
	update();
}

void SubWidget::addWidget( QPixmap pixMap )
{
	m_currentCheckedPixmap = pixMap;
	QGraphicsPixmapItem *newPixmapItem = new QGraphicsPixmapItem;
	newPixmapItem->setPixmap( m_currentCheckedPixmap );

	if( m_checkedView == NULL )
	{
		m_checkedScene = new QGraphicsScene;
		m_checkedScene->addItem( newPixmapItem );
		m_checkedScene->setBackgroundBrush( Qt::gray );

		m_checkedView = new QGraphicsView();
		m_checkedView->setScene( m_checkedScene );
		m_checkedView->centerOn(0,0);

		m_hLayout->addWidget( m_checkedView );
	}
	else
	{
		if(  m_checkedScene != NULL )
		{
			QList<QGraphicsItem*> all = m_checkedScene->items();
			for (int i = 0; i < all.size(); i++)
			{
				QGraphicsItem *gi = all[i];
				if(gi->parentItem()==NULL) 
				{
					delete gi;
				}
			}
			m_checkedScene->update();
			m_checkedScene->removeItem( m_checkedScene->focusItem() );
			m_checkedScene->addItem( newPixmapItem );
		}
		
	}
	updateWidget();
	m_isChecked = true;
}

QString SubWidget::getCurrentFileName()
{
	m_currentFileName = m_Doc->getCurrentFileName();
	return m_currentFileName;
}

QPixmap SubWidget::getCurrentPixmap() const
{
	return m_currentPixmap;
}

QPixmap SubWidget::getCurrentCheckedPixmap() const
{
	return m_currentCheckedPixmap;
}
QGraphicsScene * SubWidget::getOriScene()
{
	return m_oriScene;
}

bool SubWidget::isChecked() const
{
	return m_isChecked;
}
void SubWidget::updateWidget( int value )
{
	m_Doc->setCurrentRate(value);
	QLayout *layout = this->layout();
	int count = layout->count();

	QList<QGraphicsItem*> all = m_oriScene->items();
	for (int i = 0; i < all.size(); i++)
	{
		QGraphicsItem *gi = all[i];
		if(gi->parentItem()==NULL) 
		{
			delete gi;
		}
	}
	m_oriScene->update();
	m_oriScene->removeItem( m_oriScene->focusItem() );

	QGraphicsPixmapItem *newPixmapItem = new QGraphicsPixmapItem;
	int w = m_currentPixmap.width() * value/100.0;
	int h = m_currentPixmap.height() * value/100.0;

	newPixmapItem->setPixmap(  m_currentPixmap.scaled(w,h) );
	m_oriScene->addItem( newPixmapItem );

	if( count == 2 )
	{
		QList<QGraphicsItem*> all = m_checkedScene->items();
		for (int i = 0; i < all.size(); i++)
		{
			QGraphicsItem *gi = all[i];
			if(gi->parentItem()==NULL) 
			{
				delete gi;
			}
		}
		m_checkedScene->update();
		m_checkedScene->removeItem( m_checkedScene->focusItem() );

		QGraphicsPixmapItem *newPixmapItem = new QGraphicsPixmapItem;
		int w = m_currentCheckedPixmap.width() * value/100.0;
		int h = m_currentCheckedPixmap.height() * value/100.0;

		newPixmapItem->setPixmap(  m_currentCheckedPixmap.scaled(w,h) );
		m_checkedScene->addItem( newPixmapItem );
	}
}


void SubWidget::updateWidget()
{
	QSize size = this->size();
	int win_w = size.width();
	int win_h = size.height();
	int img_w = m_currentPixmap.width();
	int img_h = m_currentPixmap.height();

	QLayout *layout = this->layout();
	int count = layout->count();
	if( count == 2 )
	{
		win_w = win_w/2;
	}
	int currentRate = getMinRate( win_w, win_h, img_w, img_h);
	m_Doc->setCurrentRate( currentRate );
	updateWidget( currentRate );
}

int SubWidget::getMinRate( int win_w, int win_h, int img_w, int img_h  )
{
	float rate_w = float( win_w)/img_w;
	float rate_h = float( win_h)/img_h;

	if( rate_w <= rate_h )
		return int( rate_w*100 );
	else
		return int(rate_h * 100);
}

int SubWidget::getCurrentRate() const
{
	return m_Doc->getCurrentRate();
}

void SubWidget::setParentTabWidget( QTabWidget *tabWidget)
{
	m_parentTabWidget = tabWidget;
}
QTabWidget* SubWidget::getParentTabWidget() const
{
	return m_parentTabWidget;
}

SubWidget::~SubWidget()
{
}


void SubWidget::getPrePageSlot()
{
	if( m_Doc->getPrePage() )
	{
		m_currentPixmap = m_Doc->getCurrentPixmap();
		updateWidget( m_Doc->getCurrentRate() );
		emit subWidgetChangedSignal();
	}
}

void SubWidget::getNextPageSlot()
{
	if( m_Doc->getNextPage() )
	{
		m_currentPixmap = m_Doc->getCurrentPixmap();
		updateWidget( m_Doc->getCurrentRate() );
		emit subWidgetChangedSignal();
	}
}

void SubWidget::getFirstPageSlot()
{
	if( m_Doc->getFirstPage() )
	{
		m_currentPixmap = m_Doc->getCurrentPixmap();
		updateWidget( m_Doc->getCurrentRate() );
		emit subWidgetChangedSignal();
	}
}

void SubWidget::getLastPageSlot()
{
	if( m_Doc->getLastPage() )
	{
		m_currentPixmap = m_Doc->getCurrentPixmap();
		updateWidget( m_Doc->getCurrentRate() );
		emit subWidgetChangedSignal();
	}
}

void SubWidget::setParaCheckedModeSlot()
{
   	int page = m_Doc->getCurrentPage();
	m_Doc->setCurrentMode( SACPARA );
	m_Doc->getPage(page);
	updateWidget( m_Doc->getCurrentRate() );
	emit subWidgetChangedSignal();
}

void SubWidget::setTableCheckedModeSlot()
{
   	int page = m_Doc->getCurrentPage();
	m_Doc->setCurrentMode( SACTABLE );
	m_Doc->getPage(page);
	updateWidget( m_Doc->getCurrentRate() );
	emit subWidgetChangedSignal();
}

void SubWidget::setFormulaCheckedModeSlot()
{
   	int page = m_Doc->getCurrentPage();
	m_Doc->setCurrentMode( SACFORMULA );
	m_Doc->getPage(page);
	updateWidget( m_Doc->getCurrentRate() );
	emit subWidgetChangedSignal();
}


Document* SubWidget::getDocument() const
{
	return m_Doc;
}

bool SubWidget::createDocument( QString fileName )
{
	QFileInfo fileInfo = QFileInfo(fileName);
	QString suffix = fileInfo.suffix();
	if( suffix == QString("pdf") )
	{
		m_Doc = new PDFDocument( fileName );
	}
	else
	{
		m_Doc = new ImageDocument(fileName);
	}
	return true;
}