#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;
uniform mat4 u_viewProj;
uniform mat4 u_model;
out vec3 vg_N;
out vec3 vg_Pos;
out vec2 vg_TexCoord;
uniform bool smoothShaded = false;
void main() {

    if (smoothShaded)
        vg_N = normalize(mat3(u_model) * position);
    else
        vg_N = mat3(u_model) * normal;
    vec4 pos = u_viewProj * u_model * vec4(position, 1.0f);
    vg_Pos = vec3(u_model * vec4(position, 1.0f));
    vg_TexCoord = uv;
    gl_Position = pos;

}

#shader geometry

#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;


in vec3 vg_N[];
in vec3 vg_Pos[];
in vec2 vg_TexCoord[];


out vec3 gf_T;
out vec3 gf_B;
out vec3 v_normal;
out vec3 v_position;
out vec2 v_uv;


vec3 GetTangent(vec3 A, vec3 B, vec3 C, vec2 Auv, vec2 Buv, vec2 Cuv)
{
    float Bv_Cv = Buv.y - Cuv.y;
    if (Bv_Cv == 0.0)
        return (B - C) / (Buv.x - Cuv.x);

    float Quotient = (Auv.y - Cuv.y) / (Bv_Cv);
    vec3 D = C + (B - C) * Quotient;
    vec2 Duv = Cuv + (Buv - Cuv) * Quotient;
    return (D - A) / (Duv.x - Auv.x);
}
vec3 GetBitangent(vec3 A, vec3 B, vec3 C, vec2 Auv, vec2 Buv, vec2 Cuv)
{
    return GetTangent(A, C, B, Auv.yx, Cuv.yx, Buv.yx);
}

void main(void)
{
    vec3 T = GetTangent(vg_Pos[0], vg_Pos[1], vg_Pos[2],
        vg_TexCoord[0], vg_TexCoord[1], vg_TexCoord[2]);
    vec3 B = GetBitangent(vg_Pos[0], vg_Pos[1], vg_Pos[2],
        vg_TexCoord[0], vg_TexCoord[1], vg_TexCoord[2]);

    for (int i = 0; i < 3; ++i) {
        gf_T = T;
        gf_B = B;
        v_uv = vg_TexCoord[i];
        v_normal = vg_N[i];
        v_position = vg_Pos[i];
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }

}

#shader fragment
#version 330 core

layout(location = 0) out vec4 g_albedo;
layout(location = 1) out vec4 g_position;
layout(location = 2) out vec4 g_normal;
layout(location = 3) out vec4 g_material;

in vec3 v_normal;
in vec3 v_position;
in vec2 v_uv;

in vec3 gf_T;
in vec3 gf_B;

uniform vec4 albedo;
uniform float u_metallic = 0.0f;
uniform float u_roughness = 0.0f;
uniform float u_emmisive = 0.0f;

uniform bool useDeffuseMap = false;
uniform sampler2D deffuseMap;
uniform bool useMetallicMap = false;
uniform sampler2D metallicMap;
uniform bool useRoughnessMap = false;
uniform sampler2D roughnessMap;

uniform bool useNormalMap = false;
uniform sampler2D normalMap;

uniform bool useAmbientOcclusionMap = false;
uniform sampler2D ambientOcclusionMap;

uniform vec3 camPos;

uniform float u_extend = 0.05;

uniform bool lightBulb = false;

void main() {

    g_position = vec4(v_position, 1.0f);
    mat3 tbn = mat3(gf_T, gf_B, v_normal);

    vec3 viewDirection = normalize(camPos - v_position);
    vec2 m_uv = v_uv;

    if (lightBulb)
    {
        float tex = texture(deffuseMap, m_uv).a;
        if (tex <= 0.1)
            discard;
        else
        {
            g_albedo = albedo;
            g_normal = vec4(normalize(v_normal), 1.0f);
            g_material = vec4(0.0, 0.0, 1.0,1.0);
        }
        return;
    }


    // Geometry output
    if (useDeffuseMap) {
        g_albedo = texture2D(deffuseMap, m_uv).rgba;
    }
    else g_albedo = albedo;

    if (useNormalMap) {
        vec3 nrml = texture2D(normalMap, m_uv).rgb;
        nrml = nrml * 2.0 - 1.0;
        g_normal = vec4(normalize(tbn * nrml), 1.0f);
    }
    else
        g_normal = vec4(normalize(v_normal), 1.0f);
    float mMap = u_metallic;
    if (useMetallicMap) {
        mMap = texture2D(metallicMap, m_uv).r;
    }
    float rMap = u_roughness;
    if (useRoughnessMap) {
        rMap = texture2D(roughnessMap, m_uv).r;
    }
    float ambientO = 1.0f;
    if (useAmbientOcclusionMap) {
        ambientO = texture2D(ambientOcclusionMap, m_uv).r;
    }

    g_material = vec4(rMap, mMap, u_emmisive, ambientO);
}