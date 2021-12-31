#include "MainWidget.h"
#include <QPainter>

/*
	save.bin�ṹ��
		ground			uint8_t * 15 * 10
		rest			uint8_t * 9
		shapeSeriesLen	uint8_t
		shapeSeries		uint8_t * (2 + 1) * shapeSeriesLen   -----------������ʾstartPoint��һ����ʾnum
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
	1,1,//��
	2,2,2,2,2,2,2,2,//��
	3,3,3,3,//��
	4,4,4,4,//��
	5,5,5,5,//��
	6,6,6,6,//��
	7,7,//��
	8 //��
	};
	vector<vector<complex<int8_t>>>coords = {
		{{0,0},{0,1}},//�� 0
		{{0,0},{1,0}},//�� 90
		{{0,0},{0,1},{0,2},{1,0}},//�� 0
		{{0,0},{0,1},{1,1},{2,1}},//�� 90
		{{0,0},{1,-2},{1,-1},{1,0}},//�� 180
		{{0,0},{1,0},{2,0},{2,1}},//�� 270
		{{0,0},{1,0},{1,1},{1,2}},//�� �� 0
		{{0,0},{0,1},{1,0},{2,0}},//�� �� 90
		{{0,0},{0,1},{0,2},{1,2}},//�� �� 180
		{{0,0},{1,0},{2,-1},{2,0}},//�� �� 270
		{{0,0},{0,2},{1,0},{1,1},{1,2}},//�� 0
		{{0,0},{0,1},{1,0},{2,0},{2,1}},//�� 90
		{{0,0},{0,1},{0,2},{1,0},{1,2}},//�� 180
		{{0,0},{0,1},{1,1},{2,0},{2,1}},//�� 270
		{{0,0},{1,-1},{1,0},{1,1}},//�� 0
		{{0,0},{1,0},{1,1},{2,0}},//�� 90
		{{0,0},{0,1},{0,2},{1,1}},//�� 180
		{{0,0},{1,-1},{1,0},{2,0}},//�� 270
		{{0,0},{0,1},{1,1},{1,2}},//�� 0
		{{0,0},{1,-1},{1,0},{2,-1}},//�� 90
		{{0,0},{0,1},{1,-1},{1,0}},//�� �� 0
		{{0,0},{1,0},{1,1},{2,1}},//�� �� 90
		{{0,0},{0,1},{1,1}},//�� 0
		{{0,0},{1,-1},{1,0}},//�� 90
		{{0,0},{1,0},{1,1}},//�� 180
		{{0,0},{0,1},{1,0}},//�� 270
		{{0,0},{0,1},{0,2},{0,3}},//�� 0
		{{0,0},{1,0},{2,0},{3,0}},//�� 90
		{{0,0},{0,1},{1,0},{1,1}}//��
	};
	for (int i = 0; i < types.size(); i++)
	{
		addShape(i, types[i], coords[i]);
	}

	colors = {
		QColor(0, 0, 0, 0),//͸��
		QColor(160, 160, 160),//�ף������壬���ɻҵ��ˣ�
		QColor(81, 166, 255),//��
		QColor(255, 163, 85),//��
		QColor(255, 233, 85),//��
		QColor(233, 33, 36),//��
		QColor(141, 0, 141),//��
		QColor(0, 0, 0),//��
		QColor(36, 185, 3),//��
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
	{//��û�������״
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
	{//���������״�ܲ��ܷŵÿ�
		complex<uint8_t>blockLocation = point + shapes[shapeNum].blocks[i];
		if (blockLocation.real() < 0 || blockLocation.real() > 14
			|| blockLocation.imag() < 0 || blockLocation.imag() > 9)
		{//ֱ�ӳ���ȥ��
			if (++shapeNum < shapes.size())
			{//����û�Ե���״������һ����ʼ��ȥ������
				goto chooseShape;
			}
			else
			{//������״�������ˣ�ûһ���ŵ��µ�
				goto stepBack;
			}
		}
		else if (ground[blockLocation.real()][blockLocation.imag()] != 0)
		{//�����е��غ���
			if (++shapeNum < shapes.size())
			{//����û�Ե���״������һ����ʼ��ȥ������
				goto chooseShape;
			}
			else
			{//������״�������ˣ�ûһ���ŵ��µ�
				goto stepBack;
			}
		}
	}
	{
		Shape fitShape = shapes[shapeNum];//����ȷ���������
		rest[fitShape.type] = rest[fitShape.type] - 1;
		for (complex<int8_t>blockLocation : fitShape.blocks)
		{//���������״����ȥ
			complex<int8_t>p = point + blockLocation;
			ground[p.real()][p.imag()] = fitShape.type;
		}

		Unit newUnit;//�����ϼ�����
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
		rest = { 0,5,5,5,5,5,5,5,5 };//0�Ǹ���ռ���õģ�ӲҪ˵�Ļ������հס���ʣ��������������ԶҲ�ò��������
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
		ds << (uint8_t)0 << (uint64_t)0 << (uint64_t)0;//�ֱ���shapeSeriesLen,attemptCount��solutionsNum(solution������)
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