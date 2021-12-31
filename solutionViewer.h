#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <vector>

using std::vector;

class solutionViewer : public QWidget
{
	Q_OBJECT

public:
	solutionViewer(QWidget* parent = Q_NULLPTR);
	~solutionViewer();

	vector<vector<uint64_t>>solutions;
	vector<QColor>colors;
	uint64_t pagesNum, currentPage;
	QPushButton* btnR, * btnL, * btnRm, * btnLm;
	QLabel* pageL;

	void addSolution(vector<vector<uint8_t>>);
	void addFromSave(vector<uint64_t>);
	vector<uint64_t>getNum(vector<vector<uint8_t>>);

protected:
	void paintEvent(QPaintEvent*);
};
