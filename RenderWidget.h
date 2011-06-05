#ifndef _GLWIDGET_H
#define _GLWIDGET_H

#include <GL/glew.h>
#include <QtOpenGL/QGLWidget>

//#include <GL/gl.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

typedef unsigned char VolumeData;

extern "C" void initRaycast( void* volume, cudaExtent volumeSize );

class RenderWidget: public QGLWidget {

Q_OBJECT // must include this if you use Qt signals/slots

public:
	RenderWidget( QWidget *parent = NULL );
	void loadModel( QString filename );

protected:
	void initializeGL();
	void resizeGL( int w, int h );
	void paintGL();
	void mousePressEvent( QMouseEvent *event );
	void mouseMoveEvent( QMouseEvent *event );

private:
	struct cfdConfig {
		/*
		static const int gridWidth = 96;
		static const int gridHeight = 96;
		static const int gridDepth = 96;
		*/
		cudaExtent gridSize;
		cudaExtent renderSize;
		/*
		static const int renderWidth = 256;
		static const int renderHeight = 256;
	*/
		GLuint renderPbo;
		GLuint renderTex;
	};

	struct cudaGraphicsResource *cuda_pbo_resource; // CUDA Graphics Resource (to transfer PBO)
	struct cfdConfig cfdCfg;


	float3 viewRotation;
	float3 viewTranslation;
	float invViewMatrix[12];
	int oldx, oldy; // mouse tracking

	void initPixelBuffer();
};

#endif  /* _GLWIDGET_H */
