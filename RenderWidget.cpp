#include <stdio.h>
#include <QtGui/QMouseEvent>
#include <QtDebug>
#include "RenderWidget.h"

RenderWidget::RenderWidget( QWidget *parent ) :
	QGLWidget(parent) {
	setMouseTracking(true);
}

void RenderWidget::initializeGL() {
	glEnable( GL_TEXTURE_2D);
	glDisable( GL_DEPTH_TEST);
	glDisable( GL_COLOR_MATERIAL);
	glEnable( GL_BLEND);
	glEnable( GL_POLYGON_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0, 0, 0, 0);
}

void RenderWidget::resizeGL( int w, int h ) {
	glViewport(0, 0, w, h);
	glMatrixMode( GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1, 0, 1);
	glMatrixMode( GL_MODELVIEW);
	glLoadIdentity();
}

void RenderWidget::paintGL() {
	glClear( GL_COLOR_BUFFER_BIT);
	glColor3f(1, 0, 0);
	glBegin( GL_QUADS );
	glVertex2f(0, 0);
	glVertex2f(1, 0);
	glVertex2f(1, 1);
	glVertex2f(0, 1);
	glEnd();
}

void RenderWidget::mousePressEvent( QMouseEvent *event ) {

}
void RenderWidget::mouseMoveEvent( QMouseEvent *event ) {
	//printf("%d, %d\n", event->x(), event->y());
}

void RenderWidget::loadModel( QString filename ) {
	qDebug() << "ivjo zoki!";
}

void RenderWidget::keyPressEvent( QKeyEvent* event ) {
	switch (event->key()) {
		case Qt::Key_Escape:
			close();
			break;
		default:
			event->ignore();
			break;
	}
}
