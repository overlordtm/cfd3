#include <stdio.h>
#include <QtGui/QMouseEvent>
#include <QtDebug>
#include "RenderWidget.h"

RenderWidget::RenderWidget( QWidget *parent ) :
	QGLWidget(parent) {

	//cfdCfd = {0};

	cfdCfg.gridSize = make_cudaExtent(96, 96, 96);
	cfdCfg.renderSize = make_cudaExtent(256, 256, 0);
	//setFocusPolicy(Qt::StrongFocus);
	//setMouseTracking(true);
}

void RenderWidget::initializeGL() {
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	//glEnable( GL_POLYGON_SMOOTH); // if enabled you get diagonal strech over quad http://www.opengl.org/discussion_boards/ubbthreads.php?ubb=showflat&Number=233585
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0, 0.125, 0.25, 0); // background color

	// extensions
	glewInit();

	//cuda
	initRaycast(0, cfdCfg.renderSize);
}

void RenderWidget::resizeGL( int w, int h ) {

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void RenderWidget::paintGL() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	{
		glLoadIdentity();
		glRotatef(-viewRotation.x, 1.0, 0.0, 0.0);
		glRotatef(-viewRotation.y, 0.0, 1.0, 0.0);
		glRotatef(-viewRotation.z, 0.0, 0.0, 1.0);
		glTranslatef(0, 0, 3); // center it
		//glTranslatef( viewTranslation.x, viewTranslation.y, viewTranslation.z );
	}

	GLfloat modelView[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
	glPopMatrix();

	// inverse is transpose
	invViewMatrix[0] = modelView[0];
	invViewMatrix[1] = modelView[4];
	invViewMatrix[2] = modelView[8];
	invViewMatrix[3] = modelView[12];
	invViewMatrix[4] = modelView[1];
	invViewMatrix[5] = modelView[5];
	invViewMatrix[6] = modelView[9];
	invViewMatrix[7] = modelView[13];
	invViewMatrix[8] = modelView[2];
	invViewMatrix[9] = modelView[6];
	invViewMatrix[10] = modelView[10];
	invViewMatrix[11] = modelView[14];

	glColor3f(1, 0, 0);
	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(1, 0);
	glVertex2f(1, 1);
	glVertex2f(0, 1);
	glEnd();
}

void RenderWidget::initPixelBuffer() {

	if (cfdCfg.renderPbo) {
		printf("Unregistring buffer");
		// unregister this buffer object from CUDA C
		cudaGraphicsUnregisterResource( cuda_pbo_resource);
		// delete old buffer
		glDeleteBuffers(1, &cfdCfg.renderPbo);
		glDeleteTextures(1, &cfdCfg.renderTex);
	}

	// create pixel buffer object for display
	glGenBuffers(1, &cfdCfg.renderPbo);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, cfdCfg.renderPbo);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, cfdCfg.renderSize.width * cfdCfg.renderSize.height * sizeof(GLubyte) * 4, 0, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	// register this buffer object with CUDA, so we can access it
	cudaGraphicsGLRegisterBuffer(&cuda_pbo_resource, cfdCfg.renderPbo, cudaGraphicsMapFlagsWriteDiscard);

	// create texture for display
	glGenTextures(1, &cfdCfg.renderTex);
	glBindTexture(GL_TEXTURE_2D, cfdCfg.renderTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, cfdCfg.renderSize.width, cfdCfg.renderSize.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void RenderWidget::mousePressEvent( QMouseEvent *event ) {

}
void RenderWidget::mouseMoveEvent( QMouseEvent *event ) {
	qDebug() << "Rotating view!";
	int dx, dy;
	dx = event->x() - oldx;
	dy = event->y() - oldy;

	viewRotation.x += dy / 3.0;
	viewRotation.y += dx / 3.0;
	oldx = event->x();
	oldy = event->y();
}

void RenderWidget::loadModel( QString filename ) {
	qDebug() << "Loading model! " << filename;
}
