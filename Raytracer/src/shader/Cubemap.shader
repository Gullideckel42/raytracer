#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;
uniform mat4 u_proj;
uniform mat4 u_view;

out vec3 v_texCoordVector;

void main() {
	v_texCoordVector = position;
	gl_Position = u_proj * vec4(mat3(u_view) * position, 1.0f);
	
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 color;
//layout(location = 3) out vec4 material;
uniform samplerCube u_cubemap;

in vec3 v_texCoordVector;
uniform float u_mip = 0;
uniform float u_exposure;
uniform float u_gamma;
uniform bool u_toneMapping = true;
uniform bool u_gammacorrection = true;

vec3 ACESFilm(vec3 x)
{
	vec3 a = vec3(2.51f);
	vec3 b = vec3(0.03f);
	vec3 c = vec3(2.43f);
	vec3 d = vec3(0.59f);
	vec3 e = vec3(0.14f);
	return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() {
	vec3 clr = textureLod(u_cubemap, v_texCoordVector, u_mip).rgb;
	
	
	// Tone mapping
	if (u_toneMapping)
	{
		clr = vec3(1.0) - exp(-clr * u_exposure);
	}
	

	// Gamma correct
	//clr = clr / (clr + vec3(1.0));
	if (u_gammacorrection)
	{
		clr = pow(clr, vec3(1.0 / u_gamma));
	}
	

	color = vec4(clr, 1.0);
}
