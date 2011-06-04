#include <stdio.h>
#include "RenderWidget.h"
#include "cfd3.h"
#include <QHBoxLayout>

cfd3::cfd3( QWidget *parent ) :
	QMainWindow(parent) {

	renderWidget = new RenderWidget;

	setCentralWidget(renderWidget);
	resize(512, 512);
	//ui.setupUi(this);
}

cfd3::~cfd3() {

}
