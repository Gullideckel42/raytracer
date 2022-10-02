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

void main() {
	vec3 clr = textureLod(u_cubemap, v_texCoordVector, u_mip).rgb;
	
	color = vec4(clr, 1.0);
}
