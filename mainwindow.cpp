#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	setGeometry(400, 100, 600, 610); // 1120
	/* 产生随机粒子 */
	srand(time(NULL));
	/* 初始化界面 */
	initplot();
	/* SLOT槽中的函数必须被定义在private slots:下才能调用 */
	running = 0;
	QMenu *editMenu = ui->menuBar->addMenu(tr("edit"));
	QAction *pauseAction = editMenu->addAction(tr("Pause"), this, SLOT(pauseEvent()));
	QAction *continueAction = editMenu->addAction(tr("Continue"), this, SLOT(continueEvent()));
	ui->mainToolBar->addAction(pauseAction);
	ui->mainToolBar->addAction(continueAction);
	connect(&eventTimer, SIGNAL(timeout()), this, SLOT(UpdatePosition()));
	eventTimer.start(100);

}
void MainWindow::initplot()
{
	/* 对点参数的设置，实线蓝色 */
	QPen pen(Qt::SolidLine);
	pen.setColor(Qt::blue);
	/* 将点添加到画板上去 */
	QCPGraph *graph0 = ui->customPlot->addGraph();
	graph0->setPen(pen);
	graph0->setLineStyle(QCPGraph::lsNone);
	graph0->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 4));
	/* car and uav初始位置信息 */
	QVector<double> xCoord,yCoord;
		double x = 0;
		double y = 2500;
		Node T;
		T.SetCarID(0);
		T.SetX(x);
		T.SetY(y);
		T.SetspeedX(32);
		T.SetspeedY(0);
		SumNode.push_back(T);
		xCoord.push_back(x);
		yCoord.push_back(y);
	/* set UAV Parameter */
	int MAXCarID = 1;
	//First Address
	for(int i = 0;i < 3;++i){
		UAV[i].SetUavID(i+1);
		UAV[i].SetCarID(MAXCarID++);
		UAV[i].SetRadius(500);
		UAV[i].SetX(0);
		UAV[i].SetY(2500);
		UAV[i].SetspeedX(45);
		UAV[i].SetspeedY(0);
		SumNode.push_back(UAV[i]);
		xCoord.push_back(UAV[i].GetX());
		yCoord.push_back(UAV[i].GetY());
	}
	/* set Hotpoint */
	Node HT[30];
	for(int i = 0;i < 5;++i){
		HT[i].SetX(1000*i+200);
		HT[i].SetY(2700);
		HT[i+1].SetX(1000*i+500);
		HT[i+1].SetY(2900);
		HT[i+2].SetX(1000*i+800);
		HT[i+2].SetY(2700);
		HT[i+3].SetX(1000*i+200);
		HT[i+3].SetY(2300);
		HT[i+4].SetX(1000*i+500);
		HT[i+4].SetY(2100);
		HT[i+5].SetX(1000*i+800);
		HT[i+5].SetY(2300);
		Hotpoint.push_back(HT[i]);
		Hotpoint.push_back(HT[i+1]);
		Hotpoint.push_back(HT[i+2]);
		Hotpoint.push_back(HT[i+3]);
		Hotpoint.push_back(HT[i+4]);
		Hotpoint.push_back(HT[i+5]);
		xCoord.push_back(HT[i].GetX());
		yCoord.push_back(HT[i].GetY());
		xCoord.push_back(HT[i+1].GetX());
		yCoord.push_back(HT[i+1].GetY());
		xCoord.push_back(HT[i+2].GetX());
		yCoord.push_back(HT[i+2].GetY());
		xCoord.push_back(HT[i+3].GetX());
		yCoord.push_back(HT[i+3].GetY());
		xCoord.push_back(HT[i+4].GetX());
		yCoord.push_back(HT[i+4].GetY());
		xCoord.push_back(HT[i+5].GetX());
		yCoord.push_back(HT[i+5].GetY());
	}
	/* set Car */
	SumNode[0].SetRadius(500);
	Car = new QCPItemText(ui->customPlot);
	Car->setPositionAlignment(Qt::AlignmentFlag::AlignCenter);
	Car->setText("Car");
	Car->position->setCoords(SumNode[0].GetX(), SumNode[0].GetY());
	Car->setColor(Qt::red);
	Circle_Car = new QCPItemEllipse(ui->customPlot);
	Circle_Car->topLeft->setCoords(SumNode[0].GetX() - SumNode[0].GetRadius(),SumNode[0].GetY() + SumNode[0].GetRadius());
	Circle_Car->bottomRight->setCoords(SumNode[0].GetX() +SumNode[0].GetRadius(),SumNode[0].GetY() - SumNode[0].GetRadius());
	Circle_Car->setPen(solidPen);
	solidPen.setColor(Qt::darkCyan);
	/* set UAV */
	char Uavname[2];
	for(int i = 0;i < 3;++i){
	Uavname[0] = 'U';
	Uavname[1] = '0'+i;
	UAV[i].SetRadius(500);
	Text_UAV[i] = new QCPItemText(ui->customPlot);
	Text_UAV[i]->setPositionAlignment(Qt::AlignmentFlag::AlignCenter);
	Text_UAV[i]->setText(Uavname);
	Text_UAV[i]->position->setCoords(UAV[i].GetX(), UAV[i].GetY());
	Text_UAV[i]->setColor(Qt::darkGreen);
	Circle_UAV[i] = new QCPItemEllipse(ui->customPlot);
	Circle_UAV[i]->topLeft->setCoords(UAV[i].GetX() - UAV[i].GetRadius(), UAV[i].GetY() + UAV[i].GetRadius());
	Circle_UAV[i]->bottomRight->setCoords(UAV[i].GetX() + UAV[i].GetRadius(), UAV[i].GetY() - UAV[i].GetRadius());
	Circle_UAV[i]->setPen(solidPen);
	SetUavColor(i);
	}
	/* 用于设置x轴与y轴的刻度标签 */
	ui->customPlot->xAxis2->setVisible(true);
	ui->customPlot->xAxis2->setTickLabels(false);
	ui->customPlot->yAxis2->setVisible(true);
	ui->customPlot->yAxis2->setTickLabels(false);

	/* 不加这两句话则当鼠标滚轮时,x轴上侧与y轴右侧范围不会变化 */
	connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
	connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

	/* 设置x,y轴的范围 */
	ui->customPlot->xAxis->setRange(0, 5000);
	ui->customPlot->yAxis->setRange(0, 5000);

	/* 利用鼠标滚轮自动调整大小 */
	ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	graph0->setData(xCoord,yCoord);
	ui->customPlot->replot();
}
void MainWindow::UpdatePosition()
{
	if(running == 1)
		return;
	ui->customPlot->clearGraphs();
	ui->customPlot->clearItems();
	/* 对点参数的设置，实线蓝色 */
	QPen pen(Qt::SolidLine);
	pen.setColor(Qt::blue);
	/* 将点添加到画板上去 */
	QCPGraph *graph0 = ui->customPlot->addGraph();
	graph0->setPen(pen);
	graph0->setLineStyle(QCPGraph::lsNone);
	graph0->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 4));

	QVector<double> xCoord,yCoord;
	for(std::vector<Node>::iterator iter = SumNode.begin();iter != SumNode.end();++iter)
	{
		double x = iter->GetX();
		double y = iter->GetY();
		double speedx = iter->GetSpeedx();
		double speedy = iter->GetSpeedy();
	if(!iter->GetUavID()){
		/*
			Dest:X,Y
			X = x + rand()%21 - 10;
			Y = y + rand()%21 - 10;
		*/
		//beyond area
		if(x>5000)
			x = 0;
		iter->SetX(x+speedx);
		iter->SetY(y+speedy);
		iter->SetspeedX(32);
		iter->SetspeedY(0);
		}
	/************///Set Neighber Node
	Table &_t = iter->GetNeighber();
	_t.NeighberID.clear();
	_t.NeighberSpeedx.clear();
	_t.NeighberSpeedy.clear();
	_t.NeighberX.clear();
	_t.NeighberY.clear();
	/*for(std::vector<Node>::iterator _iter = SumNode.begin();_iter!=SumNode.end();++_iter )
	{
		if(Dis(iter->GetX(),iter->GetY(),_iter->GetX(),_iter->GetY()) <= iter->GetRadius() && iter!=_iter)
		{
			_t.NeighberID.push_back(_iter->GetCarID());
			_t.NeighberSpeedx.push_back(_iter->GetSpeedx());
			_t.NeighberSpeedy.push_back(_iter->GetSpeedy());
			_t.NeighberX.push_back(_iter->GetX());
			_t.NeighberY.push_back(_iter->GetY());

		}
	}*/
	/********************************/
	xCoord.push_back(x);
	yCoord.push_back(y);
	}
	/*
		UAV schedule

	*/
	UAVScheme();
	/* set Car */
	SumNode[0].SetRadius(500);
	Car = new QCPItemText(ui->customPlot);
	Car->setPositionAlignment(Qt::AlignmentFlag::AlignCenter);
	Car->setText("Car");
	Car->position->setCoords(SumNode[0].GetX(), SumNode[0].GetY());
	Car->setColor(Qt::red);
	Circle_Car = new QCPItemEllipse(ui->customPlot);
	Circle_Car->topLeft->setCoords(SumNode[0].GetX() - SumNode[0].GetRadius(),SumNode[0].GetY() + SumNode[0].GetRadius());
	Circle_Car->bottomRight->setCoords(SumNode[0].GetX() +SumNode[0].GetRadius(),SumNode[0].GetY() - SumNode[0].GetRadius());
	Circle_Car->setPen(solidPen);
	solidPen.setColor(Qt::darkCyan);
	/* set UAV */
	/* set Hotpoint */
	for(std::vector<Node>::iterator iter = Hotpoint.begin();iter != Hotpoint.end();++iter)
	{
		xCoord.push_back(iter->GetX());
		yCoord.push_back(iter->GetY());
	}
	/* UAV apart */
	char Uavname[2];
	for(int i = 0;i < 3;++i){
	Uavname[0] = 'U';
	Uavname[1] = '0'+i;
	SumNode[i].SetRadius(500);
	Text_UAV[i] = new QCPItemText(ui->customPlot);
	Text_UAV[i]->setPositionAlignment(Qt::AlignmentFlag::AlignCenter);
	Text_UAV[i]->setText(Uavname);
	Text_UAV[i]->position->setCoords(UAV[i].GetX(), UAV[i].GetY());
	Text_UAV[i]->setColor(Qt::darkGreen);
	Circle_UAV[i] = new QCPItemEllipse(ui->customPlot);
	Circle_UAV[i]->topLeft->setCoords(UAV[i].GetX() - UAV[i].GetRadius(), UAV[i].GetY() + UAV[i].GetRadius());
	Circle_UAV[i]->bottomRight->setCoords(UAV[i].GetX() + UAV[i].GetRadius(), UAV[i].GetY() - UAV[i].GetRadius());
	Circle_UAV[i]->setPen(solidPen);
	SetUavColor(i);
	}
	graph0->setData(xCoord,yCoord);
	ui->customPlot->replot();
}
void MainWindow::UAVScheme()
{

	std::vector<Node> &s = Hotpoint;
	std::vector<int> Record;
	for(int i = 0;i < 3;++i){
		double x = UAV[i].GetX();
		double y = UAV[i].GetY();
		double distance[s.size()];
			int mindis = 0;
			for(int j = 0;j < s.size();++j){
				distance[j] = Dis(x,y,s[j].GetX(),s[j].GetY());
			}
			int cnt = 0;
			double temp = 2*SumNode[0].GetRadius();
			for(int j = 0;j < s.size();++j){
				double _dis = Dis(SumNode[0].GetX(),SumNode[0].GetY(),s[j].GetX(),s[j].GetY());
				bool fr = false;
				for(std::vector<int>::iterator iter = Record.begin();iter != Record.end();++iter)
					if(*iter == j)
					{
						fr = true;
					}
				if(!fr && !s[j].flags && _dis <= SumNode[0].GetRadius()){
					if(temp > distance[j]){
						temp = distance[j];
						mindis = j;
					}
				}
				else
					++cnt;
			}
			/* Not find object point */
			if(cnt == s.size())
			{
				double dis = Dis(x,y,SumNode[0].GetX(),SumNode[0].GetY());
				double speedx = 45*(SumNode[0].GetX()-x)/dis;
				double speedy = 45*(SumNode[0].GetY()-y)/dis;
				if(distance[mindis] < 30){
				  UAV[i].SetX(SumNode[0].GetX());
				  UAV[i].SetY(SumNode[0].GetY());
				}
				else{
					UAV[i].SetX(x+speedx);
					UAV[i].SetY(y+speedy);
				}

			}
			else
			{
				Record.push_back(mindis);
				double dis = Dis(x,y,s[mindis].GetX(),s[mindis].GetY());
				double speedx = 45*(s[mindis].GetX()-x)/dis;
				double speedy = 45*(s[mindis].GetY()-y)/dis;
				if(distance[mindis] < 30){
					UAV[i].SetX(s[mindis].GetX());
					UAV[i].SetY(s[mindis].GetY());
					s[mindis].flags = true;
				}
				else{
					UAV[i].SetX(x+speedx);
					UAV[i].SetY(y+speedy);

				}

			}
	}
	for(int i = 0;i < 3;++i)
		if(UAV[i].GetX() > 5000)
	{
		for(int j = 0;j < 3;++j){
		UAV[j].SetX(0);
		UAV[j].SetY(2500);
		UAV[j].SetspeedX(45);
		UAV[j].SetspeedY(0);
		}
		for(std::vector<Node>::iterator iter = Hotpoint.begin();iter != Hotpoint.end();++iter)
			iter->flags = false;
	}
}
void MainWindow::pauseEvent()
{
	running = 1;
}
void MainWindow::continueEvent()
{
	running = 0;
}
MainWindow::~MainWindow()
{
	delete ui;
}
