#version 460

layout (location = 0) in vec3 VertexPosition;

uniform vec4 Color;
uniform mat4 MV;
uniform mat4 Proj;

void main(){
	gl_Position = Proj * MV * vec4(VertexPosition, 1.0);
}