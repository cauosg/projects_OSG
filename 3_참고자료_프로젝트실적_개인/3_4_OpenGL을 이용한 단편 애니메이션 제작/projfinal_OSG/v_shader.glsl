#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec4 vTangent;


uniform int option;

uniform vec3 Gravity = vec3(0.0, -0.05, 0.0); // world coord.
uniform float ParticleLifetime = 250; 

uniform float initvel;
uniform float Time;
out float Transp;
uniform float poslimit;
struct LightInfo
{
	vec4 Position;
	vec3 Intensity;
};
uniform LightInfo Light;

out vec3 LightDir;
out vec3 ViewDir;
out vec2 TexCoord;
out vec3 Normal;
out vec3 Position;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

void getEyeSpace(out vec3 norm, out vec3 position)
{
	position = vec3(mv_matrix*vec4(vPosition, 1.0));
	norm = normalize(mat3(mv_matrix)*vNormal);
}

void main()
{
	if(option==1){
		vec3 pos = vec3(0.0);
		Transp = 0.0;
			pos = vPosition;
			pos.y-=initvel*Time;
			if(pos.y<poslimit)
				pos.y = poslimit;
			

			Transp = 1.0 - Time / ParticleLifetime;
	gl_Position = proj_matrix*mv_matrix*vec4(pos, 1.0);
	}
	else{
		getEyeSpace(Normal, Position);
		mat3 normal_matrix = mat3(mv_matrix);
		vec3 pos = vec3(mv_matrix*vec4(vPosition, 1.0));

		// transform normal and tangent to eye space
		vec3 norm = normalize(normal_matrix*vNormal);
		vec3 tang = normalize(normal_matrix * vTangent.xyz);

		// compute the binormal
		vec3 binormal = cross(norm, tang);
	
		vec3 L = Light.Position.xyz - pos;
		LightDir = vec3(dot(L, tang), dot(L, binormal), dot(L, norm));
		ViewDir = vec3(dot(-pos, tang), dot(-pos, binormal), dot(-pos, norm));

		TexCoord = vTexCoord;
		gl_Position = proj_matrix* mv_matrix *vec4(vPosition, 1.0);
		Transp = 1.0;
	}
}
