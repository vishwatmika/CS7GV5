//vertex shader
#version 330

in vec3 vertexPosition;
in vec3 vertexNormals;

out vec3 nEye;
out vec3 fragPos;
out vec3 vsNormals;
out vec3 vsPosition;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 ortho;
uniform mat4 model;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float ambientStr;
uniform float specularStr;


void main(){
	vsPosition = (vertexPosition);
	vsNormals = (vertexNormals);

	fragPos = vec3(model * vec4(vertexPosition, 1.0));
	nEye = (view * vec4 (vertexNormals, 0.0)).xyz;
	gl_Position =  proj * view * model * ortho* vec4 (vertexPosition, 1.0);
}