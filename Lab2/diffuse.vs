#version 330

in vec3 vertexPosition;
in vec3 vertexNormals;

out vec3 fragPos;
out vec3 vsNormals;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 ortho;
uniform mat4 model;


void main(){
	fragPos = vec3(model * vec4(vertexPosition, 1.0));
	vsNormals = normalize(mat3(model)*vertexNormals);
	gl_Position =  proj * view * model * ortho* vec4 (vertexPosition, 1.0);
}