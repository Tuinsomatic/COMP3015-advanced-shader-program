#version 460

in float Transparency;
in vec2 TexCoord;

layout (binding = 0) uniform sampler2D ParticleTex; //the particles will be textures in the fragment shader

layout (location = 0) out vec4 FragColor;

void main(){
	FragColor = texture(ParticleTex, TexCoord);	//render the partcle textures for each particle
	FragColor.a *= Transparency;				//set their appropriate transparency
}