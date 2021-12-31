#pragma once

#include <QPushButton>
#include <vector>

using std::vector;

class viewBtn : public QPushButton
{
	Q_OBJECT

public:
	viewBtn(vector<vector<int>>, QWidget* parent);
	~viewBtn();
	void updateData(vector<vector<int>>);

	vector<vector<int>>solution;
	vector<QColor>colors;

protected:
	void paintEvent(QPaintEvent*);
};
