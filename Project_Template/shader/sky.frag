#version 460

layout(binding=0) uniform samplerCube SkyBoxTex; 
in vec3 Vec;
layout( location = 0 ) out vec4 FragColor;

void main() {
    vec3 texColor = texture(SkyBoxTex, normalize(Vec)).rgb; 
    //Gamma correction
    texColor = pow( texColor, vec3(1.0/2.2));
    FragColor = vec4(texColor,1); 
}

//the skybox we loaded earlier retrieved here as a uniform
//apply the skybox textures to the world space
//pass the skybox shading out of the fragment shader for every pixel