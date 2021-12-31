#include "MainWidget.h"
#include <QPainter>

/*
	save.bin结构：
		ground			uint8_t * 15 * 10
		rest			uint8_t * 9
		shapeSeriesLen	uint8_t
		shapeSeries		uint8_t * (2 + 1) * shapeSeriesLen   -----------两个表示startPoint，一个表示num
		attempCount		uint64_t
		solutionsNum	uint64_t
		solutions		uint64_t * 10 * solutionsNum

*/

MainWidget::MainWidget(QWidget* parent)
	: QMainWindow(parent)
{
	setFixedSize(330, 540);
	setWindowTitle(tr("WoodIntelligence"));

	attemptCountL = new QLabel(this);
	saveL = new QLabel(this);
	switchBtn = new QPushButton(this);
	solutionBtn = new QPushButton(this);
	timer = new QTimer(this);
	file = new QFile("save.bin");
	viewer = new solutionViewer;

	attemptCountL->setGeometry(0, 0, 330, 30);
	attemptCountL->setAlignment(Qt::AlignCenter);

	saveL->setGeometry(210, 495, 120, 30);
	saveL->setAlignment(Qt::AlignCenter);

	switchBtn->setGeometry(120, 495, 90, 30);
	switchBtn->setEnabled(false);
	QObject::connect(switchBtn, &QPushButton::clicked, [&] {
		if (!running)
		{
			running = true;
			saveL->setText(tr(""));
			switchBtn->setText("stop");
			timer->start();
		}
		else
		{
			running = false;
			save();
			switchBtn->setText("start");
		}
					 }
	);

	solutionBtn->setGeometry(15, 495, 30, 30);
	solutionBtn->setText("S");
	QObject::connect(solutionBtn, &QPushButton::clicked, [&] {viewer->show(); });

	timer->setSingleShot(true);
	timer->setInterval(0);
	QObject::connect(timer, &QTimer::timeout, [&] {
		if (running)
		{
			attempt();
		}
					 }
	);

	vector<uint8_t>types = {
	1,1,//白
	2,2,2,2,2,2,2,2,//蓝
	3,3,3,3,//橙
	4,4,4,4,//黄
	5,5,5,5,//红
	6,6,6,6,//紫
	7,7,//黑
	8 //绿
	};
	vector<vector<complex<int8_t>>>coords = {
		{{0,0},{0,1}},//白 0
		{{0,0},{1,0}},//白 90
		{{0,0},{0,1},{0,2},{1,0}},//蓝 0
		{{0,0},{0,1},{1,1},{2,1}},//蓝 90
		{{0,0},{1,-2},{1,-1},{1,0}},//蓝 180
		{{0,0},{1,0},{2,0},{2,1}},//蓝 270
		{{0,0},{1,0},{1,1},{1,2}},//蓝 翻 0
		{{0,0},{0,1},{1,0},{2,0}},//蓝 翻 90
		{{0,0},{0,1},{0,2},{1,2}},//蓝 翻 180
		{{0,0},{1,0},{2,-1},{2,0}},//蓝 翻 270
		{{0,0},{0,2},{1,0},{1,1},{1,2}},//橙 0
		{{0,0},{0,1},{1,0},{2,0},{2,1}},//橙 90
		{{0,0},{0,1},{0,2},{1,0},{1,2}},//橙 180
		{{0,0},{0,1},{1,1},{2,0},{2,1}},//橙 270
		{{0,0},{1,-1},{1,0},{1,1}},//黄 0
		{{0,0},{1,0},{1,1},{2,0}},//黄 90
		{{0,0},{0,1},{0,2},{1,1}},//黄 180
		{{0,0},{1,-1},{1,0},{2,0}},//黄 270
		{{0,0},{0,1},{1,1},{1,2}},//红 0
		{{0,0},{1,-1},{1,0},{2,-1}},//红 90
		{{0,0},{0,1},{1,-1},{1,0}},//红 翻 0
		{{0,0},{1,0},{1,1},{2,1}},//红 翻 90
		{{0,0},{0,1},{1,1}},//紫 0
		{{0,0},{1,-1},{1,0}},//紫 90
		{{0,0},{1,0},{1,1}},//紫 180
		{{0,0},{0,1},{1,0}},//紫 270
		{{0,0},{0,1},{0,2},{0,3}},//黑 0
		{{0,0},{1,0},{2,0},{3,0}},//黑 90
		{{0,0},{0,1},{1,0},{1,1}}//绿
	};
	for (int i = 0; i < types.size(); i++)
	{
		addShape(i, types[i], coords[i]);
	}

	colors = {
		QColor(0, 0, 0, 0),//透明
		QColor(160, 160, 160),//白（看不清，换成灰的了）
		QColor(81, 166, 255),//蓝
		QColor(255, 163, 85),//橙
		QColor(255, 233, 85),//黄
		QColor(233, 33, 36),//红
		QColor(141, 0, 141),//紫
		QColor(0, 0, 0),//黑
		QColor(36, 185, 3),//绿
	};
	viewer->show();
	init();
}

void MainWidget::paintEvent(QPaintEvent*)
{
	attemptCountL->setText(tr("attempt: ") + QString::number(attemptCount));

	QPainter pt(this);
	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			pt.setBrush(colors[ground[i][j]]);
			pt.drawRect(15 + 30 * j, 30 + 30 * i, 30, 30);
		}
	}
}

void MainWidget::closeEvent(QCloseEvent*)
{
	viewer->close();
}

void MainWidget::attempt()
{
	complex<int8_t>point;
	uint8_t shapeNum;
	shapeNum = 0;
	for (int8_t i = 0; i < 15; i++)
	{
		for (int8_t j = 0; j < 10; j++)
		{
			if (ground[i][j] == 0)
			{
				point = { i,j };
				goto chooseShape;
			}
		}
	}

	viewer->addSolution(ground);

	goto stepBack;

chooseShape:
	bool foundShape = false;
	for (int i = shapeNum; i < shapes.size(); i++)
	{//找没用完的形状
		if (rest[shapes[i].type] > 0)
		{
			shapeNum = i;
			foundShape = true;
			break;
		}
	}
	if (!foundShape)
	{
		goto stepBack;
	}
	for (int i = 0; i < shapes[shapeNum].blocks.size(); i++)
	{//看看这个形状能不能放得开
		complex<uint8_t>blockLocation = point + shapes[shapeNum].blocks[i];
		if (blockLocation.real() < 0 || blockLocation.real() > 14
			|| blockLocation.imag() < 0 || blockLocation.imag() > 9)
		{//直接超出去了
			if (++shapeNum < shapes.size())
			{//还有没试的形状，从下一个开始回去继续试
				goto chooseShape;
			}
			else
			{//各种形状都试完了，没一个放得下的
				goto stepBack;
			}
		}
		else if (ground[blockLocation.real()][blockLocation.imag()] != 0)
		{//跟已有的重合了
			if (++shapeNum < shapes.size())
			{//还有没试的形状，从下一个开始回去继续试
				goto chooseShape;
			}
			else
			{//各种形状都试完了，没一个放得下的
				goto stepBack;
			}
		}
	}
	{
		Shape fitShape = shapes[shapeNum];//最终确定这个合适
		rest[fitShape.type] = rest[fitShape.type] - 1;
		for (complex<int8_t>blockLocation : fitShape.blocks)
		{//把这个新形状摆上去
			complex<int8_t>p = point + blockLocation;
			ground[p.real()][p.imag()] = fitShape.type;
		}

		Unit newUnit;//序列上加上它
		newUnit.shape = fitShape;
		newUnit.startPoint = point;
		shapeSeries.push_back(newUnit);
	}
	attemptCount++;
	repaint();
	timer->start();
	return;

stepBack:
	Unit lastShape = shapeSeries[shapeSeries.size() - 1];
	shapeSeries.pop_back();
	point = lastShape.startPoint;
	shapeNum = lastShape.shape.num + 1;
	for (complex<int8_t>blockLocation : lastShape.shape.blocks)
	{
		complex<int8_t>p = point + blockLocation;
		ground[p.real()][p.imag()] = 0;
	}
	rest[lastShape.shape.type] = rest[lastShape.shape.type] + 1;
	goto chooseShape;
}

void MainWidget::save()
{
	switchBtn->setEnabled(false);
	saveL->setText(tr("Saving..."));
	file->resize(0);
	file->open(QIODevice::WriteOnly);

	QDataStream ds(file);
	for (vector<uint8_t>row : ground)
	{
		for (uint8_t data : row)
		{
			ds << data;
		}
	}

	for (uint8_t r : rest)
	{
		ds << r;
	}

	ds << (uint8_t)shapeSeries.size();
	for (Unit u : shapeSeries)
	{
		ds << u.startPoint.real() << u.startPoint.imag() << u.shape.num;
	}

	ds << attemptCount << viewer->solutions.size();
	for (vector<uint64_t> s : viewer->solutions)
	{
		for (uint64_t col : s)
		{
			ds << col;
		}
	}
	file->close();
	switchBtn->setEnabled(true);
	saveL->setText(tr("Saved!"));
}

void MainWidget::init()
{
	saveL->setText(tr("Loading..."));
	if (file->exists())
	{
		rest.resize(9);
		ground = vector<vector<uint8_t>>(15, vector<uint8_t>(10));
		file->open(QIODevice::ReadOnly);
		QDataStream ds(file);
		for (vector<uint8_t>& row : ground)
		{
			for (uint8_t& data : row)
			{
				ds >> data;
			}
		}
		for (uint8_t& r : rest)
		{
			ds >> r;
		}
		uint8_t seriesLen;
		ds >> seriesLen;
		for (uint8_t i = 0; i < seriesLen; i++)
		{
			uint8_t num, re, im;
			ds >> re >> im >> num;
			Shape s = shapes[num];
			complex<uint8_t>p = { re, im };
			Unit u;
			u.shape = s;
			u.startPoint = p;
			shapeSeries.push_back(u);
		}

		ds >> attemptCount;

		uint64_t solutionCount;
		ds >> solutionCount;
		viewer->solutions.reserve(solutionCount);
		for (uint64_t i = 0; i < solutionCount; i++)
		{
			///*vector<vector<uint8_t>>s(15, vector<uint8_t>(10, 0));
			//for (vector<uint8_t>& row : s)
			//{
			//	for (uint8_t& u : row)
			//	{
			//		ds >> u;
			//	}
			//}*/
			//viewer->addSolution(s);
			vector<uint64_t> s(10);
			for (uint64_t& col : s)
			{
				ds >> col;
			}
			viewer->addFromSave(s);
		}
		file->close();
	}
	else
	{
		attemptCount = 0;
		attemptCountL->setText(tr("attempt: 0"));
		rest = { 0,5,5,5,5,5,5,5,5 };//0那个是占空用的，硬要说的话代表“空白”的剩余数量。。。永远也用不到这个数
		ground = vector<vector<uint8_t>>(15, vector<uint8_t>(10, 0));

		file->open(QIODevice::WriteOnly);
		QDataStream ds(file);
		for (vector<uint8_t>row : ground)
		{
			for (uint8_t data : row)
			{
				ds << data;
			}
		}

		for (uint8_t r : rest)
		{
			ds << r;
		}
		ds << (uint8_t)0 << (uint64_t)0 << (uint64_t)0;//分别是shapeSeriesLen,attemptCount和solutionsNum(solution的数量)
		file->close();
	}

	switchBtn->setText(tr("start"));

	running = false;

	saveL->setText(tr("Loaded!"));
	switchBtn->setEnabled(true);
}

void MainWidget::addShape(uint8_t n, uint8_t t, vector<complex<int8_t>> b)
{
	Shape s;
	s.num = n;
	s.type = t;
	s.blocks = b;
	shapes.push_back(s);
}