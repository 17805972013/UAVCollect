#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QCustomPlot/qcustomplot.h>
#include <time.h>
#include "node.h"
#include <stack>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	void initplot();
	void SetUavColor(int sel)
	{
		switch(sel)
		{
		case 0:
			solidPen.setColor(Qt::darkCyan);
			break;
		case 1:
			solidPen.setColor(Qt::darkRed);
			break;
		case 2:
			solidPen.setColor(Qt::darkGreen);
			break;
		default:
			solidPen.setColor(Qt::cyan);
		}
	}
	void UAVScheme();

	~MainWindow();
private slots:
	void UpdatePosition();
	void pauseEvent();
	void continueEvent();
private:
	Ui::MainWindow *ui;
	QTimer eventTimer;
	QPen solidPen;
	std::vector<Node> SumNode;
	std::vector<Node> Hotpoint;
	/* UAV  */
	int running;
	Node UAV[3];
	QCPItemText *Text_UAV[3];
	QCPItemEllipse *Circle_UAV[3];
	/* Car Node */
	QCPItemText *Car;
	QCPItemEllipse *Circle_Car;
	std::vector<int> Record;

};
#endif // MAINWINDOW_H
