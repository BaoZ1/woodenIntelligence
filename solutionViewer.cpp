#include "solutionViewer.h"
#include <QPainter>

solutionViewer::solutionViewer(QWidget* parent)
	: QWidget(parent)
{
	setFixedSize(260, 400);
	setWindowTitle("Solutions");

	pagesNum = 1;
	currentPage = 1;

	btnL = new QPushButton(this);
	btnR = new QPushButton(this);
	btnLm = new QPushButton(this);
	btnRm = new QPushButton(this);
	pageL = new QLabel(this);

	btnL->setGeometry(45, 15, 20, 20);
	btnL->setText(tr("<"));
	QObject::connect(btnL, &QPushButton::clicked, [&] {
		if (currentPage > 1)
		{
			currentPage -= 1;
			repaint();
		}
					 }
	);

	btnR->setGeometry(195, 15, 20, 20);
	btnR->setText(tr(">"));
	QObject::connect(btnR, &QPushButton::clicked, [&] {
		if (currentPage < pagesNum)
		{
			currentPage += 1;
			repaint();
		}
					 }
	);

	btnLm->setGeometry(20, 15, 20, 20);
	btnLm->setText(tr("|<"));
	QObject::connect(btnLm, &QPushButton::clicked, [&] {
		currentPage = 1;
		repaint();
					 }
	);

	btnRm->setGeometry(220, 15, 20, 20);
	btnRm->setText(tr(">|"));
	QObject::connect(btnRm, &QPushButton::clicked, [&] {
		currentPage = pagesNum;
		repaint();
					 }
	);

	pageL->setGeometry(70, 15, 120, 20);
	pageL->setAlignment(Qt::AlignCenter);
	pageL->setText(tr("-/-"));

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
}

solutionViewer::~solutionViewer()
{
}

void solutionViewer::addSolution(vector<vector<uint8_t>>sln)
{
	vector<uint64_t>newSolutionNum = getNum(sln);
	for (vector<uint64_t>num : solutions)
	{
		bool flag = true;
		for (int i = 0; i < num.size(); i++)
		{
			if (num[i] != newSolutionNum[i])
			{
				flag = false;
				break;
			}
		}
		if (flag)
		{
			return;
		}
	}

	solutions.push_back(newSolutionNum);
	pagesNum = solutions.size() / 4 + ((solutions.size() % 4 == 0) ? 0 : 1);
	repaint();
}

void solutionViewer::addFromSave(vector<uint64_t> s)
{
	solutions.push_back(s);
	pagesNum = solutions.size() / 4 + ((solutions.size() % 4 == 0) ? 0 : 1);
	repaint();
}

vector<uint64_t> solutionViewer::getNum(vector<vector<uint8_t>>v)
{
	vector<vector<uint8_t>>vv = v;
	vector<uint64_t>r;
	for (int i = 0; i < vv[0].size(); i++)
	{
		uint64_t u = 0;
		for (int j = 0; j < vv.size(); j++)
		{
			u *= 10;
			u += vv[j][i];
		}
		r.push_back(u);
	}
	return r;
}

void solutionViewer::paintEvent(QPaintEvent*)
{
	pageL->setText(solutions.size() == 0 ? tr("-/-") :
				   tr(QString::number(currentPage).toLatin1() + "/" + QString::number(pagesNum).toLatin1()));

	QPainter pt(this);
	int startPoints[4][2] = { {20,60} ,{140,60},{20,230},{140,230} };
	for (int i = 0; i < 4; i++)
	{
		if ((currentPage - 1) * 4 + i < solutions.size())
		{
			for (int j = 0; j < 15; j++)
			{
				for (int k = 0; k < 10; k++)
				{
					uint64_t num = solutions[(currentPage - 1) * 4 + i][k];
					uint64_t n1 = 1, n2 = 10;
					for (int m = 14; m > j; m--)
					{
						n1 *= 10;
						n2 *= 10;
					}
					int n = (num % n2) / n1;
					pt.setBrush(colors[n]);
					pt.drawRect(startPoints[i][0] + 10 * k, startPoints[i][1] + 10 * j, 10, 10);
				}
			}
		}
		else
		{
			pt.drawLine(startPoints[i][0], startPoints[i][1], startPoints[i][0] + 100, startPoints[i][1]);
			pt.drawLine(startPoints[i][0] + 100, startPoints[i][1], startPoints[i][0] + 100, startPoints[i][1] + 150);
			pt.drawLine(startPoints[i][0] + 100, startPoints[i][1] + 150, startPoints[i][0], startPoints[i][1] + 150);
			pt.drawLine(startPoints[i][0], startPoints[i][1] + 150, startPoints[i][0], startPoints[i][1]);
			pt.drawLine(startPoints[i][0], startPoints[i][1], startPoints[i][0] + 100, startPoints[i][1] + 150);
			pt.drawLine(startPoints[i][0] + 100, startPoints[i][1], startPoints[i][0], startPoints[i][1] + 150);
		}
	}
}