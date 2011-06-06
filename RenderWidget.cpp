#include "RenderWidget.h"

RenderWidget::RenderWidget( QWidget *parent ) :
	QGLWidget(parent) {

	renderPbo = 0;
	renderTex = 0;
	volumeSize = make_cudaExtent(256, 256, 256);
	renderSize = make_cudaExtent(256, 256, 0);

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
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0, 0.125, 0.25, 0); // background color

	// extensions
	glewInit();

	QGLShader *vshader = new QGLShader(QGLShader::Vertex, this);
	vshader->compileSourceFile("vertex.glsl");

	QGLShader *fshader = new QGLShader(QGLShader::Fragment, this);
	fshader->compileSourceFile("frag.glsl");

	program = new QGLShaderProgram(this);
	program->addShader(vshader);
	//program->addShader(fshader);
	program->link();

	glGenBuffers(1, &cubeVbo);
	glBindBufferARB(GL_ARRAY_BUFFER, cubeVbo);
	glBufferDataARB(GL_ARRAY_BUFFER, sizeof(points) * sizeof(GLfloat), points, GL_STATIC_DRAW);

	glGenBuffers(1, &cubeIndicesVbo);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, cubeIndicesVbo);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * sizeof(GLubyte), indices, GL_STATIC_DRAW);
	//delete [] points;  // to da error


	// naloadamo odatke v teksturo
	char* h_volume = (char*) loadRawFile("data/skull256x256x256.raw", 256 * 256 * 256 * sizeof(char));

	glGenTextures(1, &volumeTex);
	glBindTexture(GL_TEXTURE_3D, volumeTex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, volumeSize.width, volumeSize.height, volumeSize.depth, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, h_volume);
}

void RenderWidget::resizeGL( int w, int h ) {

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluOrtho2D(-0.2, 1.2, -0.2, 1.2);
	gluPerspective(30.0, 1.0, 0.0, 2.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//projection.perspective(45.0, 1.0, 0.0, 2.0);
}

void RenderWidget::paintGL() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	/*
	 modelView.setToIdentity();
	 modelView.translate(-0.5, -0.5, -5);
	 modelView.rotate(-viewRotation.x, 1.0, 0.0, 0.0);
	 modelView.rotate(-viewRotation.y, 0.0, 1.0, 0.0);
	 modelView.rotate(-viewRotation.z, 0.0, 0.0, 1.0);
	 */
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	{

		glLoadIdentity();
		glTranslatef(0, 0, -3);
		glRotatef(-viewRotation.x, 1.0, 0.0, 0.0);
		glRotatef(-viewRotation.y, 0.0, 1.0, 0.0);
		glRotatef(-viewRotation.z, 0.0, 0.0, 1.0);
		glTranslatef(-0.5, -0.5, 0);
		// center it
		//glTranslatef( viewTranslation.x, viewTranslation.y, viewTranslation.z );
	}

	/*
	 glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
	 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIndicesVbo);


	 //program->bind();
	 //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	 //glEnableVertexAttribArray(0);

	 //program->setUniformValue("proj", projection);
	 //program->setUniformValue("modelView", modelView);

	 glEnableClientState(GL_VERTEX_ARRAY);

	 glVertexPointer(3, GL_FLOAT, 0, 0);
	 glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, 0);

	 glDisableClientState(GL_VERTEX_ARRAY);
	 glBindBuffer(GL_ARRAY_BUFFER, 0);
	 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	 //program->release();
	 */

	glBindTexture(GL_TEXTURE_3D, volumeTex);
	glEnable(GL_TEXTURE_3D);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_QUADS);
	{
		glTexCoord3f(0, 0, 0.5);
		glVertex2f(0, 0);
		glTexCoord3f(1, 0, 0.5);
		glVertex2f(1, 0);
		glTexCoord3f(1, 1, 0.5);
		glVertex2f(1, 1);
		glTexCoord3f(0, 1, 0.5);
		glVertex2f(0, 1);
	}
	glEnd();
	glDisable(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, 0);

	//glDisable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//program->release();

	glPopMatrix();
}

void RenderWidget::render() {

	copyInvViewMatrix(invViewMatrix, sizeof(float4) * 3);
	uint *d_output;

	// map PBO to get CUDA device pointer
	cudaGraphicsMapResources(1, &cuda_pbo_resource, 0);

	size_t num_bytes;
	// save pointer to cuda_pbo_resource into d_output
	cudaGraphicsResourceGetMappedPointer((void **) &d_output, &num_bytes, cuda_pbo_resource);

	// clear image
	cudaMemset(d_output, 0, renderSize.width * renderSize.height * 4);

	// call CUDA kernel, writing results to PBO
	render_kernel(gridSize, blockSize, d_output, renderSize.width, renderSize.height, 0.15, 1.0, 1.0, 1.0);
	cudaGraphicsUnmapResources(1, &cuda_pbo_resource, 0);

}

void RenderWidget::initPixelBuffer() {

	if (renderPbo) {
		printf("Unregistring buffer");
		// unregister this buffer object from CUDA C
		cudaGraphicsUnregisterResource(cuda_pbo_resource);
		// delete old buffer
		glDeleteBuffers(1, &renderPbo);
		glDeleteTextures(1, &renderTex);
	}

	// create pixel buffer object for display
	glGenBuffers(1, &renderPbo);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, renderPbo);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, renderSize.width * renderSize.height * sizeof(GLubyte) * 4, 0, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	// register this buffer object with CUDA, so we can access it
	cudaGraphicsGLRegisterBuffer(&cuda_pbo_resource, renderPbo, cudaGraphicsMapFlagsWriteDiscard);

	// create texture for display
	glGenTextures(1, &renderTex);
	glBindTexture(GL_TEXTURE_2D, renderTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, renderSize.width, renderSize.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void RenderWidget::mousePressEvent( QMouseEvent *event ) {

}
void RenderWidget::mouseMoveEvent( QMouseEvent *event ) {
	qDebug() << "Rotating view! x:" << event->x() << " y:" << event->y();
	int dx, dy;

	dx = event->x() - oldx;
	dy = event->y() - oldy;

	viewRotation.x += dy / 3.0;
	viewRotation.y += dx / 3.0;
	oldx = event->x();
	oldy = event->y();

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
