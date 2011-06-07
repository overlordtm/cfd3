#include "RenderWidget.h"

RenderWidget::RenderWidget( QWidget *parent ) :
	QGLWidget(parent) {

	renderPbo = 0;
	renderTex = 0;
	volumeSize = make_cudaExtent(96, 96, 96);
	renderSize = make_cudaExtent(96, 96, 0);

	points = {
		0.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		1.0, 0.0, 1.0,
		1.0, 1.0, 1.0,
		0.0, 1.0, 1.0,
	};
	indices = {1, 2, 3, 4, 2, 6, 7, 3, 1, 4, 8, 5, 4, 3, 7, 8, 1, 5, 6, 2, 5, 8, 7, 6,};

	projection = QMatrix4x4();
	modelView = QMatrix4x4();
	program = 0;

	blockSize = dim3(16, 16);
	gridSize = dim3((int) ceil(renderSize.width / blockSize.x), (int) ceil(renderSize.height / blockSize.y));

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

	QGLShader *vshader = new QGLShader(QGLShader::Vertex, this);
	vshader->compileSourceFile("vertex.glsl");

	QGLShader *raycastShader = new QGLShader(QGLShader::Fragment, this);
	raycastShader->compileSourceFile("frag.glsl");

	program = new QGLShaderProgram(this);
	//program->addShader(vshader);
	program->addShader(raycastShader);
	program->link();

	glGenBuffers(1, &cubeVbo);
	glBindBufferARB(GL_ARRAY_BUFFER, cubeVbo);
	glBufferDataARB(GL_ARRAY_BUFFER, sizeof(points) * sizeof(GLfloat), points, GL_STATIC_DRAW);

	glGenBuffers(1, &cubeIndicesVbo);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, cubeIndicesVbo);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * sizeof(GLubyte), indices, GL_STATIC_DRAW);
	//delete [] points;  // to da error


	// naloadamo odatke v teksturo
	//char* h_volume = (char*) loadRawFile("data/skull256x256x256.raw", 256 * 256 * 256 * sizeof(char));
	char* h_volume = makeCloud(96);

	glGenTextures(1, &volumeTex);
	glBindTexture(GL_TEXTURE_3D, volumeTex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, volumeSize.width, volumeSize.height, volumeSize.depth, 0, GL_RED, GL_UNSIGNED_BYTE, h_volume);
	glBindTexture(GL_TEXTURE_3D, 0);

	// transfer tex
	glGenTextures(1, &transferTex);
	glBindTexture(GL_TEXTURE_1D, transferTex);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	float4 transferFunc[] = { 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, };

	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, sizeof(transferFunc) / sizeof(float4), 0, GL_RGBA, GL_FLOAT, transferFunc);
	glBindTexture(GL_TEXTURE_1D, 0);
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
	/*
	 printf("%f %f %f %f \n", modelView.constData()[0], modelView.constData()[1], modelView.constData()[2], modelView.constData()[3]);
	 printf("%f %f %f %f \n", modelView.constData()[4], modelView.constData()[5], modelView.constData()[6], modelView.constData()[7]);
	 printf("%f %f %f %f \n", modelView.constData()[8], modelView.constData()[9], modelView.constData()[10], modelView.constData()[11]);
	 printf("%f %f %f %f \n\n", modelView.constData()[12], modelView.constData()[13], modelView.constData()[14], modelView.constData()[15]);
	 */
	//printf("%f %f %f %f \n", modelView.column(3).x(), modelView.column(3).y(), modelView.column(3).z(), modelView.column(3).w());


	program->bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, volumeTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, transferTex);
	program->setUniformValue("modelView", modelView);
	program->setUniformValue("modelViewInv", modelView.inverted());
	program->setUniformValue("rayOrigin", modelView.column(3).x(), modelView.column(3).y(), modelView.column(3).z());
	program->setUniformValue("volumeTex", 0);
	program->setUniformValue("transferTex", 1);
	program->setUniformValue("windowSize", (float)size().width(), (float)size().height());

	//glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_QUADS);
	{
		//glTexCoord3f(0, 0, viewTranslation.z);
		glVertex2f(0, 0);
		//glTexCoord3f(1, 0, viewTranslation.z);
		glVertex2f(1, 0);
		//glTexCoord3f(1, 1, viewTranslation.z);
		glVertex2f(1, 1);
		//glTexCoord3f(0, 1, viewTranslation.z);
		glVertex2f(0, 1);
	}
	glEnd();

	glBindTexture(GL_TEXTURE_1D, 0);
	glBindTexture(GL_TEXTURE_3D, 0);

	program->release();

}


void RenderWidget::mousePressEvent( QMouseEvent *event ) {

}
void RenderWidget::mouseMoveEvent( QMouseEvent *event ) {

	int dx, dy;

	dx = event->x() - oldx;
	dy = event->y() - oldy;

	qDebug() << "Rotating view! x:" << dx << " y:" << dy;

	if (abs(dx) > 10 || abs(dy) > 10) {
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

	printf("Read '%s', %zu bytes\n", filename, read);

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

				float off = fabsf(perlin.GetValue(x*frequency, y*frequency, z*frequency));

				float d = sqrtf(dx * dx + dy * dy + dz * dz) / size;

				cloud[x*(size*size) + y*size + z] = ((d - off) < 0.125f) ? 128 : 0;
			}
		}
	}

	return cloud;
}
