#include "CentralWidget.h"
#include <QHeaderView>

#define FIXKEDWIDTH 200

CentralWidget::CentralWidget()
{
	m_tabWidget     = new QTabWidget();
	m_tabWidget->setMinimumWidth(FIXKEDWIDTH);
	m_tabWidget->setTabsClosable(true);

	createTableWidget();
	createRightLayout();
	createMainLayout();
	connect( m_tabWidget, SIGNAL( tabCloseRequested(int) ), this, SLOT( closeTabWidgetSlot(int)) );
	this->setMinimumSize(600,400);
}

void CentralWidget::createTableWidget()
{
	m_tableWidget = new QTableWidget( 6, 2);
	QHeaderView *m_header = m_tableWidget->verticalHeader();  
    m_header->setHidden(true);

	m_tableWidget->setFixedWidth( FIXKEDWIDTH );
	QStringList header;
	header<<QString("Atttribute")<<QString("Value");

	m_tableWidget->setHorizontalHeaderLabels(header);
	m_tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); 

	m_tableWidget->takeVerticalHeaderItem(1);
	m_tableWidget->setItem( 0, 0, new QTableWidgetItem("File Name") );
	m_tableWidget->setItem( 1, 0, new QTableWidgetItem("File Size") );
	m_tableWidget->setItem( 2, 0, new QTableWidgetItem("File Type") );
}


void CentralWidget::createRightLayout()
{
	m_leftTopLabel  = new QLabel();
	m_leftTopLabel->setFixedWidth( FIXKEDWIDTH );

	m_rightLayout  =new QVBoxLayout;
	m_rightLayout->addWidget( m_leftTopLabel );
	m_rightLayout->addWidget( m_tableWidget );
	m_rightLayout->setStretchFactor(m_leftTopLabel,1);
	m_rightLayout->setStretchFactor(m_tableWidget,1);
}
void CentralWidget::createMainLayout()
{
	QHBoxLayout *mainLayout = new QHBoxLayout;
	mainLayout->addWidget( m_tabWidget );
	mainLayout->addLayout( m_rightLayout );
	setLayout( mainLayout );
}



void CentralWidget::closeTabWidgetSlot(int index)
{
	m_tabWidget->removeTab( index );
}
QTabWidget * CentralWidget::getTabWidget()
{
	if( m_tabWidget != NULL )
		return m_tabWidget;
	else
		return NULL;
}