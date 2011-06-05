#ifndef CFD3_H
#define CFD3_H

#include <QtGui/QMainWindow>
#include "ui_cfd3.h"

class RenderWidget;

class cfd3: public QMainWindow {
Q_OBJECT

public:
	cfd3( QWidget *parent = 0 );
	~cfd3();

private slots:
    void fileOpen();

protected:
	void keyPressEvent( QKeyEvent *event );

private:
	Ui::cfd3Class ui;
	RenderWidget *renderWidget;
};

#endif // CFD3_H
