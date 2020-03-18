#version 450

in vec2 TexCoord;
in vec3 LightDir;
in vec3 ViewDir;

uniform sampler2D ColorTex;
uniform sampler2D NormalMapTex;

struct LightInfo{
	vec4 Position;
	vec3 Intensity;
};
uniform LightInfo Light;

struct MaterialInfo{
	vec3 Ka;
	vec3 Ks;
	float Shininess;
};
uniform MaterialInfo Material;

out vec4 color;

vec3 phongModel(vec3 N, vec3 diffR) 
{
  vec3 V = normalize(ViewDir);
  vec3 S = normalize(LightDir);
  vec3 R = reflect(-S, N);

  vec3 ambient = Light.Intensity * Material.Ka;
  float sDotN = max(dot(S, N), 0.0);
  vec3 diffuse = Light.Intensity * diffR * sDotN;

  vec3 spec = Light.Intensity * Material.Ks * pow(max(dot(R, V), 0.0), Material.Shininess);
  return ambient+diffuse+spec;
}

void main(void)
{
    vec3 N = normalize(2.0 * texture(NormalMapTex, TexCoord).rgb - vec3(1.0));
	vec3 texColor = texture(ColorTex, TexCoord).rgb;

	color = vec4(phongModel(N, texColor), 1.0);
}

