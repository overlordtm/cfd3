//in vec3 position;
uniform mat4 projection;
uniform mat4 modelView;
void main(void) {
	gl_Position = gl_ModelViewMatrix * gl_Vertex; //vec4(position, 1);
	//gl_Position = vec4(1.0, 1.0, 1.0, 1.0);
	gl_FrontColor = gl_Vertex; //vec4(position, 1);
}