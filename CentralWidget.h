#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QDockWidget>
#include <QTableWidget>
#include <QLayout>

class CentralWidget:public QWidget
{
	Q_OBJECT
public:
	CentralWidget();
	QTabWidget *getTabWidget();


private:
	QVBoxLayout   *m_rightLayout;
	QHBoxLayout   *m_mainLayout;
	QTabWidget    *m_tabWidget;
	QLabel        *m_leftTopLabel;
	QLabel        *m_leftDownLabel;

	QTableWidget  *m_tableWidget;

	

	void          createTableWidget();
	void          createMainLayout();
	void          createRightLayout();


	
private slots:
	void closeTabWidgetSlot(int);
};

#endif