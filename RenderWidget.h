#ifndef _GLWIDGET_H
#define _GLWIDGET_H

#include <stdio.h>
#include <math.h>

#include <GL/glew.h>

#include <QtOpenGL/QGLWidget>
#include <QtGui/QMouseEvent>
#include <QtDebug>
#include <QGLShader>

#include <cuda_runtime.h>
#include <cutil_inline.h>
#include <cuda_gl_interop.h>


typedef unsigned char VolumeData;

void* loadRawFile( const char *filename, size_t size );
extern "C" void initRaycast( void* volume, cudaExtent volumeSize );
extern "C" void copyInvViewMatrix( float *invViewMatrix, size_t sizeofMatrix );
extern "C" void render_kernel( dim3 gridSize, dim3 blockSize, uint *d_output, uint imageW, uint imageH, float density, float brightness, float transferOffset, float transferScale );

static  GLfloat coords[] = {
		0.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
};

class RenderWidget: public QGLWidget {

Q_OBJECT // must include this if you use Qt signals/slots

public:
	RenderWidget( QWidget *parent = NULL );
	~RenderWidget();
	void loadModel( QString filename );

protected:
	void initializeGL();
	void resizeGL( int w, int h );
	void paintGL();
	void mousePressEvent( QMouseEvent *event );
	void mouseMoveEvent( QMouseEvent *event );
	void wheelEvent ( QWheelEvent * event );

private:
	cudaExtent volumeSize;
	cudaExtent renderSize;

	QMatrix4x4 projection;
	QMatrix4x4 modelView;

	GLuint renderPbo;
	GLuint renderTex;
	GLuint volumeTex;
	GLuint transferTex;
	GLuint cubeVbo;
	GLuint cubeIndicesVbo;

	GLfloat points[24];
	GLubyte indices[24];

	struct cudaGraphicsResource *cuda_pbo_resource; // CUDA Graphics Resource (to transfer PBO)

	float3 viewRotation;
	float3 viewTranslation;
	float invViewMatrix[12];
	int oldx, oldy; // mouse tracking

	dim3 blockSize;
	dim3 gridSize;

	QGLShaderProgram *program;

	void render();
	void initPixelBuffer();
};

#endif  /* _GLWIDGET_H */
