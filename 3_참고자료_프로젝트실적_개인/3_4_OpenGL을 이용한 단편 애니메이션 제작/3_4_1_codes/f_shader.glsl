#version 450

in vec2 TexCoord;
in vec3 LightDir;
in vec3 ViewDir;
in float Transp;
in vec3 Normal;
in vec3 Position;

uniform sampler2D ColorTex;
uniform sampler2D NormalMapTex;
uniform int coloring;
uniform vec4 orgColor;
uniform vec3 leafcolor;

struct SpotLightInfo{
	vec4 Position;  
	vec3 Intensity;
	vec3 direction; // normalized direction of the spotlight
	float exponent; // angular attenuation exponent
	float cutoff;   // cutoff angle (btw 0 and 90)
	int onoff;
	int phong;
};
uniform SpotLightInfo Spot;

struct LightInfo{
	vec4 Position;
	vec3 Intensity;
};
uniform LightInfo Light;

struct MaterialInfo{

	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float Shininess;
};
uniform MaterialInfo Material;

out vec4 color;

vec3 phongSpot()
{
	vec3 S = normalize(vec3(Spot.Position) - Position);
	vec3 spotDir = normalize(Spot.direction);
	float angle = acos(dot (-S, spotDir));
	float cutoff = radians (clamp(Spot.cutoff, 0.0, 90.0));
	vec3 ambient = Spot.Intensity * Material.Ka;

	if (angle < cutoff) {
		float spotFactor = pow(dot(-S, spotDir), Spot.exponent);
		vec3 V = normalize(vec3(-Position));
		vec3 H = normalize(V+S);

		return ambient + spotFactor * Spot.Intensity * (
			Material.Kd * max(dot(S, Normal), 0.0) +
			 Material.Ks * pow(max(dot(H, Normal), 0.0), Material.Shininess));	
	}
	else {return ambient;}
}

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

void main(void){

    vec3 N = normalize(2.0 * texture(NormalMapTex, TexCoord).rgb - vec3(1.0));
	vec3 texColor = texture(ColorTex, TexCoord).rgb;
	
	vec4 bcolor = texture(NormalMapTex, TexCoord);
	vec4 mcolor = texture(ColorTex, TexCoord);
	vec3 orgColor3 = vec3(orgColor.x, orgColor.y, orgColor.z);
	if(coloring==1)
		color=orgColor;
	else if(coloring==2)
	{
	color = texture(ColorTex, gl_PointCoord);
	color *= vec4(leafcolor,1.0);
	color.a *= Transp;
	return;
	}
	else 
		color = mix(bcolor, mcolor, mcolor.a) * 2.0 * vec4(phongModel(N, texColor), 1.0);


	if(Spot.phong==1)
		color = 2*color*vec4(phongModel(Normal, orgColor3), 1.0);
	if(Spot.onoff==1)
		color = 2*color*vec4(phongSpot(), 1.0);
}
