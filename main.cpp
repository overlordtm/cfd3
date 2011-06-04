#include "cfd3.h"
#include "RenderWidget.h"
#include <cuda_runtime_api.h>
#include <cuda_gl_interop.h>

#include <QtGui>
#include <QApplication>

extern "C" void cudaTest();

int main( int argc, char *argv[] ) {

	QApplication app(argc, argv);
	cfd3 w;
	w.show();

	cudaTest();

	return app.exec();
}

