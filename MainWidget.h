#pragma once

#include <QtWidgets/QMainWindow>
#include <QPushButton>
#include <Qlabel>
#include <QTimer>
#include <QFile>
#include <complex>
#include <vector>
#include "solutionViewer.h"

using std::complex;
using std::vector;

struct Shape
{
	uint8_t num;
	uint8_t type;
	vector<complex<int8_t>>blocks;
};

struct Unit
{
	complex<uint8_t> startPoint;
	Shape shape;
};

class MainWidget : public QMainWindow
{
	Q_OBJECT

public:
	MainWidget(QWidget* parent = Q_NULLPTR);

protected:
	void paintEvent(QPaintEvent*);
	void closeEvent(QCloseEvent*);
	void init();
	void addShape(uint8_t, uint8_t, vector<complex<int8_t>>);
	void attempt();
	void save();

private:
	QLabel* attemptCountL, * saveL;
	QPushButton* switchBtn, * solutionBtn;
	QTimer* timer;
	QFile* file;

	vector<vector<uint8_t>>ground;
	vector<Shape>shapes;
	vector<QColor>colors;
	vector<Unit>shapeSeries;
	vector<uint8_t>rest;
	uint64_t attemptCount;
	bool running;

	solutionViewer* viewer;
};
