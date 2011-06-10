#ifndef _GLWIDGET_H
#define _GLWIDGET_H

#include <stdio.h>
#include <math.h>
#include <iostream>
#include <libnoise/noise.h>

#include <GL/glew.h>

#include <QtOpenGL/QGLWidget>
#include <QtGui/QMouseEvent>
#include <QtDebug>
#include <QGLShader>

#include <cuda_runtime.h>
#include <cutil_inline.h>
#include <cuda_gl_interop.h>

//using namespace noise;

typedef unsigned char VolumeData;

void* loadRawFile( const char *filename, size_t size );
char* makeCloud(int size);
void checkGlErr(const char* msg);
extern "C" void initCfd( void* h_volume, void* h_velocity, cudaExtent volumeSize);
extern "C" void checkCudaErr(const char* msg);
extern "C" void simulate(dim3 gridSize, dim3 blockSize, char* pbo, cudaExtent volumeSize, int magic);

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
	void keyPressEvent( QKeyEvent* event );

private:
	cudaExtent volumeSize;
	cudaExtent renderSize;

	char* h_volume;
	float4* h_velocity;

	QMatrix4x4 projection;
	QMatrix4x4 modelView;

	GLuint renderPbo;
	GLuint renderTex;
	GLuint volumeTex;
	GLuint volumePbo;
	GLuint transferTex;
	GLuint cubeVbo;
	GLuint cubeIndicesVbo;

	GLfloat points[24];
	GLubyte indices[24];

	struct cudaGraphicsResource *cuda_pbo_resource; // CUDA Graphics Resource (to transfer PBO)

	float3 viewRotation;
	float3 viewTranslation;
	float transferScale;
	float transferOffset;
	float densityScale;
	int oldx, oldy; // mouse tracking

	dim3 blockSize;
	dim3 gridSize;

	QGLShaderProgram *program;

	void render();
	void initPixelBuffer();

	bool doSim;
};

#endif  /* _GLWIDGET_H */
