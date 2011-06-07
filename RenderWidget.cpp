#include "RenderWidget.h"

RenderWidget::RenderWidget( QWidget *parent ) :
	QGLWidget(parent) {

	doSim = false;

	renderPbo = 0;
	renderTex = 0;
	//volumeSize = make_cudaExtent(256, 256, 256);
	volumeSize = make_cudaExtent(96, 96, 96);
	//renderSize = make_cudaExtent(256, 256, 0);

	transferScale = 1.0f;
	transferOffset = 0.0f;
	densityScale = 1.0f;

	projection = QMatrix4x4();
	modelView = QMatrix4x4();
	program = 0;

	blockSize = dim3(8, 8, 8);
	gridSize = dim3((int) ceil(volumeSize.width / blockSize.x) * (int) ceil(volumeSize.depth / blockSize.z), (int) ceil(volumeSize.height / blockSize.y));

	fprintf(stderr, "\nblockSize(%d, %d, %d)\n", blockSize.x, blockSize.y, blockSize.z);
	fprintf(stderr, "gridSize(%d, %d, %d)\n", gridSize.x, gridSize.y, gridSize.z);
}

void RenderWidget::initializeGL() {

	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_1D);
	glEnable(GL_TEXTURE_3D);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glClearColor(0, 0.125, 0.25, 0); // background color
	glClearColor(0, 0, 0, 0);

	// extensions
	glewInit();
	cudaGLSetGLDevice(0);

	QGLShader *vshader = new QGLShader(QGLShader::Vertex, this);
	vshader->compileSourceFile("vertex.glsl");

	QGLShader *raycastShader = new QGLShader(QGLShader::Fragment, this);
	raycastShader->compileSourceFile("frag.glsl");

	program = new QGLShaderProgram(this);
	//program->addShader(vshader);
	program->addShader(raycastShader);
	program->link();

	// naloadamo podatke v teksturo
	//char* h_volume = (char*) loadRawFile("data/skull256x256x256.raw", 256 * 256 * 256 * sizeof(char));
	char* h_volume = makeCloud(96);

	// pbo za volume
	glGenBuffers(1, &volumePbo);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, volumePbo);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, volumeSize.width * volumeSize.height * volumeSize.depth, h_volume, GL_DYNAMIC_DRAW);
	cudaGraphicsGLRegisterBuffer(&cuda_pbo_resource, volumePbo, cudaGraphicsMapFlagsNone);

	checkGlErr("PBO napaka!");
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	// naloadamo podatke v tex
	glGenTextures(1, &volumeTex);
	glBindTexture(GL_TEXTURE_3D, volumeTex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, volumeSize.width, volumeSize.height, volumeSize.depth, 0, GL_RED, GL_BYTE, 0);
	//glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, volumeSize.width, volumeSize.height, volumeSize.depth, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_3D, 0);

	checkGlErr("TEX napaka!");

	// transfer tex
	glGenTextures(1, &transferTex);
	glBindTexture(GL_TEXTURE_1D, transferTex);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	float4 transferFunc[] = { 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.3, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.3, };

	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, sizeof(transferFunc) / sizeof(float4), 0, GL_RGBA, GL_FLOAT, transferFunc);
	glBindTexture(GL_TEXTURE_1D, 0);

	float4* h_velocity = (float4*) malloc(volumeSize.width * volumeSize.height * volumeSize.depth * sizeof(float4));

	initCfd(h_volume, h_velocity, volumeSize);

}

void RenderWidget::resizeGL( int w, int h ) {

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1.0, 0.0, 1.0);
	//gluPerspective(30.0, 1.0, 0.0, 2.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	projection.ortho(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);
	//projection.perspective(45.0, 1.0, 0.0, 2.0);
}

void RenderWidget::paintGL() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	modelView.setToIdentity();

	modelView.rotate(-viewRotation.x, 1.0, 0.0, 0.0);
	modelView.rotate(-viewRotation.y, 0.0, 1.0, 0.0);
	modelView.rotate(-viewRotation.z, 0.0, 0.0, 1.0);
	modelView.translate(0.0, 0.0, 2.0);
	modelView.translate(viewTranslation.x, viewTranslation.y, viewTranslation.z);

	if (simulate) {
		char* d_output;
		cudaGraphicsMapResources(1, &cuda_pbo_resource, 0);
		size_t num_bytes;
		// save pointer to cuda_pbo_resource into d_output
		cudaGraphicsResourceGetMappedPointer((void **) &d_output, &num_bytes, cuda_pbo_resource);

		//fprintf(stderr, "CUDA resource pointer mapped, has access to %u bytes", num_bytes);
		//cudaMemset(d_output, 64, volumeSize.width * volumeSize.height * volumeSize.depth);

		int magic = volumeSize.depth / blockSize.z;
		simulate(gridSize, blockSize, d_output, volumeSize, magic);
		checkCudaErr("Simulation failed!");

		cudaGraphicsUnmapResources(1, &cuda_pbo_resource, 0);
	}

	program->bind();
	// read volume from PBO to 3D tex
	glActiveTexture(GL_TEXTURE0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, volumePbo);
	glBindTexture(GL_TEXTURE_3D, volumeTex);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, volumeSize.width, volumeSize.height, volumeSize.depth, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, transferTex);
	program->setUniformValue("modelView", modelView);
	program->setUniformValue("modelViewInv", modelView.inverted());
	program->setUniformValue("rayOrigin", modelView.column(3).x(), modelView.column(3).y(), modelView.column(3).z());
	program->setUniformValue("volumeTex", 0);
	program->setUniformValue("transferTex", 1);
	program->setUniformValue("windowSize", (float) size().width(), (float) size().height());
	program->setUniformValue("densityScale", densityScale);
	program->setUniformValue("transferScale", transferScale);
	program->setUniformValue("transferOffset", transferOffset);

	glBegin(GL_QUADS);
	{
		glVertex2f(0, 0);
		glVertex2f(1, 0);
		glVertex2f(1, 1);
		glVertex2f(0, 1);
	}
	glEnd();

	glBindTexture(GL_TEXTURE_1D, 0);
	glBindTexture(GL_TEXTURE_3D, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	program->release();

	checkGlErr("render napaka!");

}

void RenderWidget::mousePressEvent( QMouseEvent *event ) {

}
void RenderWidget::mouseMoveEvent( QMouseEvent *event ) {

	int dx, dy;

	dx = event->x() - oldx;
	dy = event->y() - oldy;

	//qDebug() << "Rotating view! x:" << dx << " y:" << dy;

	if (abs(dx) > 15 || abs(dy) > 15) {
		oldx = event->x();
		oldy = event->y();
	} else {

		viewRotation.x += dy;
		viewRotation.y += dx;
		oldx = event->x();
		oldy = event->y();

		updateGL();
	}
}

void RenderWidget::wheelEvent( QWheelEvent * event ) {
	qDebug() << "\nKolsecek !" << event->delta();

	viewTranslation.z += (float) (event->delta() / 360.0);

	updateGL();
}

void RenderWidget::keyPressEvent( QKeyEvent* event ) {
	switch (event->key()) {
		case Qt::Key_Plus:
			densityScale += 0.05;
			break;
		case Qt::Key_Minus:
			densityScale -= 0.05;
			break;

		case Qt::Key_Home:
			transferScale += 0.05;
			break;
		case Qt::Key_End:
			transferScale -= 0.05;
			break;

		case Qt::Key_PageUp:
			transferOffset += 0.01;
			break;
		case Qt::Key_PageDown:
			transferOffset -= 0.01;
			break;

		case Qt::Key_Up:
			viewTranslation.y -= 0.05;
			break;
		case Qt::Key_Down:
			viewTranslation.y += 0.05;
			break;

		case Qt::Key_Left:
			viewTranslation.x += 0.05;
			break;
		case Qt::Key_Right:
			viewTranslation.x -= 0.05;
			break;

		case Qt::Key_S:
			if(doSim) {
				qDebug() << "Stopping simulation";
			} else {
				qDebug() << "Starting simulation";
			}
			doSim = !doSim;
			break;
		case Qt::Key_R:
			qDebug() << "Reseting simulation";
			break;
	}

	updateGL();
}

void RenderWidget::loadModel( QString filename ) {
	qDebug() << "Loading model! " << filename;
}

RenderWidget::~RenderWidget() {
	glDeleteBuffersARB(1, &cubeVbo);
}

void* loadRawFile( const char *filename, size_t size ) {

	FILE *fp = fopen(filename, "rb");

	if (!fp) {
		fprintf(stderr, "Error opening file '%s'\n", filename);
		return 0;
	}

	void *data = malloc(size);
	size_t read = fread(data, 1, size, fp);
	fclose(fp);

	printf("Read '%s', %u bytes\n", filename, read);

	return data;
}

char* makeCloud( int size ) {

	noise::module::Perlin perlin;

	char* cloud = (char*) malloc(size * size * size * sizeof(char));

	float frequency = 10.0f / size;
	float center = size / 2.0f + 0.5f;

	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			for (int z = 0; z < size; z++) {
				float dx = center - x;
				float dy = center - y;
				float dz = center - z;

				float off = fabsf(perlin.GetValue(x * frequency, y * frequency, z * frequency));

				float d = sqrtf(dx * dx + dy * dy + dz * dz) / size;

				cloud[x * (size * size) + y * size + z] = ((d - off) < 0.125f) ? 255 : 0;
			}
		}
	}

	return cloud;
}

void checkGlErr( const char* msg ) {
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		fprintf(stderr, "\nGL error: %s (%s)\n", msg, gluErrorString(err));
	}
}
