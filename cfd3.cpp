#include <stdio.h>
#include "RenderWidget.h"
#include "cfd3.h"
#include <QHBoxLayout>
#include <QFileDialog>
#include <QString>
#include <QtDebug>
#include <QFileInfo>
#include <QMessageBox>
#include <QtGui/QKeyEvent>

cfd3::cfd3( QWidget *parent ) :
	QMainWindow(parent) {

	setWindowTitle("CFD3");
	renderWidget = new RenderWidget();
	setCentralWidget(renderWidget);

	QMenu *fileMenu;
	QAction* openAction = new QAction(tr("&Open"), this);
	openAction->setShortcut(tr("Ctrl+O"));
	openAction->setStatusTip(tr("Open a document"));
	connect(openAction, SIGNAL(triggered()), this, SLOT(fileOpen()));

	fileMenu = menuBar()->addMenu("File");
	fileMenu->addAction(openAction);

	QString message = *new QString("To start/stop simulation, press S. To reset, pres R");
	statusBar()->showMessage(message);

	setMinimumSize(512, 512);
	resize(512, 512);
	//ui.setupUi(this);
}

void cfd3::fileOpen() {
	QString filename = QFileDialog::getOpenFileName(this);
	if (filename.isEmpty())
		return;
	else {
		QFileInfo fileinfo = *new QFileInfo(filename);
		if (fileinfo.suffix().toUpper().compare(new QString("PRO")) == 0) {
			renderWidget->loadModel(filename);
		} else {
			// alert user that filetype is not ok :)
			QMessageBox msgBox;
			msgBox.setText("File type must be raw!");
			msgBox.setIcon(QMessageBox::Warning);
			msgBox.exec();
		}

	}
}

void cfd3::keyPressEvent( QKeyEvent* event ) {
	switch (event->key()) {
		case Qt::Key_Escape:
			close();
			break;
		case Qt::Key_S:
			qDebug() << "Starting simulation";
			break;
		case Qt::Key_R:
			qDebug() << "Reseting simulation";
			break;
		default:
			event->ignore();
			break;
	}
}

cfd3::~cfd3() {

}
