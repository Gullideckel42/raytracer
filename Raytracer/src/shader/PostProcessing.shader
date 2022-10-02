#shader vertex
#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

out vec2 v_uv;

void main() {
    v_uv = uv;
    gl_Position = vec4(position, 1.0f);
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 color;

in vec2 v_uv;
uniform sampler2D frame;

uniform float saturation = 1.0f;
uniform vec3 color_filter = vec3(1.0f);

uniform bool toneMapping;
uniform bool gammaCorrection;
uniform float u_gamma;
uniform float u_exposure;

void main()
{
    vec4 c = texture(frame, v_uv).rgba;
    vec3 clr = c.rgb;

    if (toneMapping)
        clr = vec3(1.0) - exp(-clr * u_exposure);
    if (gammaCorrection)
        clr = pow(clr, vec3(1.0 / u_gamma));

    clr = color_filter * clr;
    vec3 sw = vec3((clr.r + clr.g + clr.b) / 3.0f);

    color = vec4(mix(sw, clr, saturation), c.a);
}