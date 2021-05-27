#version 460

layout (location = 0) in vec3 VertexInitialVelocity;
layout (location = 1) in float VertexBirthTime;

out float Transparency; //of the particle
out vec2 TexCoord;

uniform float Time; //of animation
uniform vec3 Gravity = vec3(0.0, -0.05, 0.0);
uniform float LifeTime; //maximum
uniform float Size = 1.0; //of particle
uniform vec3 Position; //of emitter

uniform mat4 MV;
uniform mat4 Proj;

//a set of offsets for every vertex of a particle - representing their position in camera coordinates
const vec3 offsets[] = vec3[](vec3(-0.5, -0.5, 0),
							  vec3(0.5, -0.5, 0),
							  vec3(0.5, 0.5, 0),
							  vec3(-0.5, -0.5, 0),
							  vec3(0.5, 0.5, 0),
							  vec3(-0.5, 0.5, 0));

//texture coordinates for every vertex of a particle
const vec2 texCoords[] = vec2[](vec2(0,0),
								vec2(1,0),
								vec2(1,1),
								vec2(0,0),
								vec2(1,1),
								vec2(0,1));

void main(){
	vec3 camPos; //position in camera coordinates
	float t = Time - VertexBirthTime;
	if (t >= 0 && t < LifeTime){
		vec3 pos = Position + VertexInitialVelocity * t + Gravity * t * t;       //calculates a particle's journey every frame using emitter position P, vertex initial velocity V.
																			      //current position = P+(Vt)+(Gt^2)
		camPos = (MV * vec4(pos, 1)).xyz + (offsets[gl_VertexID] * Size);		  //offset each vertex based on their ID
		Transparency = mix(1, 0, t / LifeTime);                                   //particles get more transparent as they approach their maximum lifespan
	}
	else{
		camPos = vec3(0);
		Transparency = 0.0;														  //particle has exceeded maximum lifespan, therefore doesn't exist and must be transparent
	}

	TexCoord = texCoords[gl_VertexID];
	gl_Position = Proj * vec4(camPos, 1);										  //creates what we see on screen
}