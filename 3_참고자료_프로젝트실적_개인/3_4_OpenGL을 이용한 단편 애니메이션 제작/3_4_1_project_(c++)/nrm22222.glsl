#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec4 vTangent;

uniform int option;
uniform vec3 VertexInitPos;
uniform vec3 VertexInitVel;
uniform float StartTime;

out float Transp;

uniform float Time;
uniform vec3 Gravity = vec3(0.0, -0.05, 0.0); // world coord.
uniform float ParticleLifetime = 0.5; 

struct LightInfo
{
	vec4 Position;
	vec3 Intensity;
};
uniform LightInfo Light;

out vec3 LightDir;
out vec3 ViewDir;
out vec2 TexCoord;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

void main()
{
	if(option==1){
		vec3 pos = vec3(0.0);
		Transp = 0.0;
			
		if (Time > StartTime) {
			float t = Time - StartTime;

		if (t < ParticleLifetime) {
			pos = VertexInitPos + VertexInitVel * t + Gravity * t * t;
			Transp = 1.0 - t / ParticleLifetime;
			}
		}
	gl_Position = proj_matrix*mv_matrix*vec4(pos, 1.0);
	}
	else{
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
	}
}
