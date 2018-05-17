#include "ocrtest.h"
#include <QTextCodec>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QTabWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileInfo>
#include <QProcess>
#include <QMessageBox>
#include <QTime>
#include <QSize>
#include <QLabel>
#include <QStatusBar>
#include <QMenuBar>




#include "CentralWidget.h"
#include "SubWidget.h"



OCRTest::OCRTest(QWidget *parent)
	: QMainWindow(parent)
{
	//ui.setupUi(this);
	m_centralWidget = new CentralWidget();
	this->setCentralWidget( m_centralWidget );
	createActions();
	createMenus();
	createToolBars();
	createStatusBars();
	setIconSize( QSize(48,48));

	connect( m_centralWidget->getTabWidget(), SIGNAL( currentChanged(int) ), this,SLOT( updateStatusSlot()));
	//ui.mainToolBar->setHidden(true);
} 
void OCRTest::createActions()
{
	m_openFileAction = new QAction( tr("&Open"), this );
	m_openFileAction->setIcon( QIcon("Icon/open.ico") );
	connect( m_openFileAction, SIGNAL( triggered() ), this, SLOT( openFileSlot() ) );

	m_sacParaCheckAction = new QAction( tr("Para Check"), this );
	m_sacParaCheckAction->setIcon( QIcon("Icon/text.ico"));

	m_sacTableCheckAction = new QAction( tr("Table Check"), this );
	m_sacTableCheckAction->setIcon( QIcon( "Icon/table.ico") );

	m_sacFormulaCheckAction = new QAction( tr("Formula Check"), this );
	m_sacFormulaCheckAction->setIcon( QIcon( "Icon/formula.ico") );
}
void OCRTest::createMenus()
{
	m_fileMenu = menuBar()->addMenu(tr("&File"));
	m_fileMenu->addAction( m_openFileAction );

	m_sacMenu = menuBar()->addMenu(tr("&SAC Check") ) ;
	m_sacMenu->addAction( m_sacParaCheckAction );
	m_sacMenu->addAction( m_sacTableCheckAction );
	m_sacMenu->addAction( m_sacFormulaCheckAction );

}

void OCRTest::createToolBars()
{
	m_fileTool = addToolBar(tr("&File") );
	m_fileTool->addAction( m_openFileAction );
	
	m_sacTool  = addToolBar( tr("&SAC Check") );
	m_sacTool->addAction( m_sacParaCheckAction );
	m_sacTool->addAction( m_sacTableCheckAction );
	m_sacTool->addAction( m_sacFormulaCheckAction );
}
void OCRTest::createStatusBars()
{
	m_prePageBtn    = new QPushButton;
	m_prePageBtn->setText("Pre");
	m_prePageBtn->setFixedWidth(40);

	m_nextPageBtn   = new QPushButton;
	m_nextPageBtn->setText("Next");
	m_nextPageBtn->setFixedWidth(40);

	m_firstPageBtn  = new QPushButton;
	m_firstPageBtn->setText("First");
	m_firstPageBtn->setFixedWidth(40);

	m_lastPageBtn   = new QPushButton;
	m_lastPageBtn->setText("Last");
	m_lastPageBtn->setFixedWidth(40);



	m_pageLineEdit  = new QLineEdit;
	m_pageLineEdit->setAlignment(Qt::AlignHCenter);
	m_pageLineEdit->setFixedWidth( 80 );
	QLabel *label1 = new QLabel;
	QLabel *label2 = new QLabel;
	int nMin = 1;
	int nMax = 100;
	int nSingleStep = 1;
	int width = 80;

	m_imageSlider  = new QSlider;
	m_imageSlider->setOrientation( Qt::Horizontal );
	m_imageSlider->setMinimum( nMin );
	m_imageSlider->setMaximum( nMax );
	m_imageSlider->setSingleStep( nSingleStep );
	m_imageSlider->setFixedWidth( width );

	m_imageSpinBox = new QSpinBox;
	m_imageSpinBox->setMinimum( nMin );
	m_imageSpinBox->setMaximum( nMax );
	m_imageSpinBox->setSingleStep( nSingleStep );

	connect( m_imageSpinBox , SIGNAL( valueChanged(int)), m_imageSlider, SLOT( setValue(int)) );
	connect( m_imageSlider, SIGNAL( valueChanged(int) ), m_imageSpinBox, SLOT( setValue(int)) );
	connect( m_imageSlider, SIGNAL( valueChanged(int) ), this, SLOT( imageScaledSlot(int)) );

	m_imageSlider->setValue( nMin );
	
	statusBar()->addPermanentWidget(label1,1);
	statusBar()->addPermanentWidget(m_firstPageBtn );
	statusBar()->addPermanentWidget(m_prePageBtn );
	statusBar()->addPermanentWidget(m_pageLineEdit);
	statusBar()->addPermanentWidget(m_nextPageBtn );
	statusBar()->addPermanentWidget(m_lastPageBtn );
	statusBar()->addPermanentWidget( label2, 1);

	statusBar()->addPermanentWidget( m_imageSpinBox );
	statusBar()->addPermanentWidget( m_imageSlider );

}

void OCRTest::createPageControl()
{

}


void OCRTest::imageScaledSlot(int value)
{
	if( m_centralWidget->getTabWidget()->currentWidget() != NULL )
	{
		SubWidget *subWidget = (SubWidget*)m_centralWidget->getTabWidget()->currentWidget();
		subWidget->updateWidget( value );
	}
	update();
}

void OCRTest::resizeEvent(QResizeEvent * event)
{
	if( m_centralWidget->getTabWidget()->currentWidget() != NULL )
	{
		SubWidget *subWidget = (SubWidget*)m_centralWidget->getTabWidget()->currentWidget();
		subWidget->updateWidget();
		m_imageSlider->setValue( subWidget->getCurrentRate() );
	}

}

void OCRTest::openFileSlot()
{
	QString filePath = QFileDialog::getOpenFileName(this);
	if( !filePath.isEmpty() )
	{
		SubWidget *subWidget = new SubWidget( filePath );
		QString fileName = getFileNameFromPath( filePath );
		m_centralWidget->getTabWidget()->addTab( subWidget,fileName);
		subWidget->setParentTabWidget( m_centralWidget->getTabWidget() );
		m_centralWidget->getTabWidget()->setCurrentWidget(subWidget);
		subWidget->updateWidget();
		
		m_imageSlider->setValue( subWidget->getCurrentRate() );
		updateStatusSlot();
		connect( m_prePageBtn,  SIGNAL(clicked()), subWidget, SLOT( getPrePageSlot()) );
		connect( m_nextPageBtn, SIGNAL(clicked()), subWidget, SLOT( getNextPageSlot()) );
		connect( m_firstPageBtn,SIGNAL(clicked()), subWidget, SLOT( getFirstPageSlot()) );
		connect( m_lastPageBtn, SIGNAL(clicked()), subWidget, SLOT( getLastPageSlot()) );
		connect( subWidget, SIGNAL(subWidgetChangedSignal()), this, SLOT( updateStatusSlot()));
	}

}


void OCRTest::autoCheckSlot()
{
	SubWidget *subWidget = (SubWidget*)m_centralWidget->getTabWidget()->currentWidget();
	if( subWidget != NULL && !subWidget->isChecked() )
	{
		QTime startTime0,startTime1,startTime2;
		

		QString sourceFileName = subWidget->getCurrentFileName();
		
		QString outPutFileName = "data_out.jpg";
		QString para = sourceFileName + outPutFileName;
		QString g_exe = "F:\\PODcode\\dist\\getProposalBlocks\\getProposalBlocks.exe";
		QProcess *process = new  QProcess;
		QStringList str;
		str<<sourceFileName<<outPutFileName;
		process->start(g_exe, str);

		startTime0 = QTime::currentTime();
		bool isFinshed = process->waitForFinished();
		startTime1 = QTime::currentTime();

		if( isFinshed )
		{
			QPixmap pixmap;
			if( pixmap.load( outPutFileName ) )
			{
				subWidget->addWidget( pixmap );
				subWidget->updateWidget();
				m_imageSlider->setValue( subWidget->getCurrentRate() );
				
			}
			else
			{
				QMessageBox::warning(this,"Failed load","Load Image Filed!");
			}
			
		}
		startTime2 = QTime::currentTime();

		QString sumTime1 = QString("%1 s").arg( startTime0.msecsTo( startTime1)/1000.0 );
		QString sumTime2 = QString::number( startTime1.msecsTo( startTime2), 10 );
		//QString sumTime = sumTime1 + QString(" ") +sumTime2;
		QMessageBox::information(NULL, "Run time", sumTime1);
	}
}


QString OCRTest::getFileNameFromPath( QString filePath )
{
	QFileInfo fileInfo = QFileInfo( filePath );
	return fileInfo.fileName();
}

void OCRTest::updateStatusSlot()
{
	SubWidget *subWidget = (SubWidget*)m_centralWidget->getTabWidget()->currentWidget();
	int currentPage = subWidget->getDocument()->getCurrentPage();
	int sumPage     = subWidget->getDocument()->getSumPage();

	m_pageLineEdit->setText( QString("%1/%2").arg(currentPage).arg(sumPage) );
	m_imageSlider->setValue( subWidget->getCurrentRate() );
}

void OCRTest::updateStatusBar()
{

}

OCRTest::~OCRTest()
{

}
