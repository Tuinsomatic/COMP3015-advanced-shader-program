#version 460

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord; //in from the vertex shader

layout (location = 0) out vec4 FragColor;

layout (binding = 0) uniform sampler2D Tex1; //the textures we loaded earlier retrieved here as uniforms
layout (binding = 1) uniform sampler2D mossTex;

uniform struct LightInfo{
    vec4 Position;
    vec3 La;
    vec3 L;
} Light;

uniform struct SpotLightInfo{
    vec3 Position;
    vec3 L;
    vec3 La;
    vec3 Direction;
    float Exponent;
    float Cutoff;
} Spot;

uniform struct MaterialInfo {
    vec3 Ka; 
    vec3 Kd; 
    vec3 Ks; 
    float Shininess; 
} Material;

vec3 phong (vec3 pos, vec3 n){
    vec3 ambient = Light.La;
    vec3 s = normalize(vec3(Light.Position));
    float dotprod = max( dot(s,n), 0.0 );
	vec3 dif = Material.Kd * dotprod;
	vec3 spec = vec3(0.0);
	if( dotprod > 0.0 ) 
	{
		vec3 view = normalize(-pos.xyz);
		vec3 ref = reflect( -s, n );
		spec = Material.Ks * pow( max( dot(ref,view), 0.0 ), Material.Shininess );
	}	

 return ambient + Light.L * (dif + spec);
}

vec3 blinnPhong( vec3 pos, vec3 n )
{
	vec3 ambient = Light.La;
	vec3 s = normalize(vec3(Light.Position));
	float dotprod = max( dot(s,n), 0.0 );
	vec3 dif = Material.Kd * dotprod;
	vec3 spec = vec3(0.0);
	if( dotprod > 0.0 ) 
	{
		vec3 view = normalize(-pos.xyz);
		vec3 halfv = normalize(view + s);
		spec = Material.Ks * pow( max( dot(halfv,n), 0.0 ), 
		Material.Shininess );
	}	

 return ambient + Light.L * (dif + spec);
}

vec3 spotlight( vec3 pos, vec3 n )
{
	vec4 metalColour = texture(Tex1, TexCoord); //calculate the texture colour using the current texture coordinate
    vec4 mossColour = texture(mossTex, TexCoord);
    vec3 color = mix(metalColour.rgb, mossColour.rgb, mossColour.a); //combine the textures, using the alpha for interpolation

	vec3 ambient = Light.La * color;
	vec3 s = normalize(vec3(Spot.Position) - pos);

	float cosin = dot(-s, normalize(Spot.Direction));
	
	float angle = acos( cosin );
	float spotScale = 0.0;
		
	vec3 dif;
	vec3 spec = vec3(0.0);

	if(angle < Spot.Cutoff )
	{
		spotScale = pow( cosin, Spot.Exponent );
		float dotprod = max( dot(s,n), 0.0 ); 
		dif = Material.Kd * dotprod * Spot.L * color;

		if( dotprod > 0.0 ) 
		{
			vec3 view = normalize(-pos.xyz);
			vec3 halfv = normalize(view + s);
			spec = Material.Ks * pow( max( dot(halfv,n), 0.0 ), Material.Shininess );
		}
	}

 return ambient + spotScale * Spot.L * (dif + spec);
}

void main()
{
	//FragColor = vec4(phongModel(Position, normalize(Normal)), 1);
	//FragColor = vec4(blinnPhong(Position, normalize(Normal)), 1);
	FragColor = vec4(spotlight(Position, normalize(Normal)), 1);
}

