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
#version 330

layout(location = 0) out vec4 color;
in vec2 v_uv;

uniform float u_ambient = 0.03;
uniform float u_gamma = 2.2;
uniform float u_exposure = 3.0;
uniform sampler2D g_Albedo;
uniform sampler2D g_Position;
uniform sampler2D g_Normal;
uniform sampler2D g_Material;
uniform mat4 u_view;
uniform vec3 camPos;
uniform samplerCube u_cubemap;
uniform samplerCube irradianceMap;
uniform sampler2D u_brdf;
uniform bool u_irradianceMapSet;
uniform bool u_specularIBL;
uniform float mip1 = 0.0f;
uniform bool toneMapping = true;
uniform bool gammaCorrection = true;

#define MAX_LIGHTS 100

struct PointLight {
    vec3 position;
    vec3 color;
};

uniform int amountOfLights;

uniform PointLight lights[MAX_LIGHTS];


////

const float PI = 3.14159265359;

vec3 fresnelSchlick(float HdotV, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - HdotV, 5.0);
}


float distributionGGX(float NdotH, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;

    float denom = NdotH * NdotH * (a2 - 1.0) + 1.0;
    denom = PI * denom * denom;

    return a2 / max(denom, 0.0000001);
}

float geometrySmith(float NdotV, float NdotL, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float ggx2 = NdotV / (NdotV * (1.0 - k) + k);
    float ggx1 = NdotL / (NdotL * (1.0 - k) + k);;

    return ggx1 * ggx2;
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


vec3 ACESFilm(vec3 x)
{
    vec3 a = vec3(2.51f);
    vec3 b = vec3(0.03f);
    vec3 c = vec3(2.43f);
    vec3 d = vec3(0.59f);
    vec3 e = vec3(0.14f);
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

////
void main() {

    vec4 baseColor = texture(g_Albedo, v_uv).rgba;
    if (baseColor.a <= 0.1) {
        discard;
    }
    vec3 albedo = baseColor.rgb;

    vec3 N = normalize(texture(g_Normal, v_uv).rgb);
    vec3 POS = texture(g_Position, v_uv).xyz;
    vec3 WORLDPOS = vec3(u_view * vec4(POS, 1.0f));
    vec4 Mat = texture(g_Material, v_uv).rgba;
    float metallic = Mat.g;
    float roughness = Mat.r;
    float emmisive = Mat.b;

    vec3 V = normalize(camPos - POS);
    vec3 baseReflectivity = mix(vec3(0.04), albedo, metallic);
    vec3 Lo = albedo * emmisive;

    // Integral 
    for (int i = 0; i < amountOfLights; i++) {
        vec3 L = normalize(lights[i].position - POS);
        vec3 H = normalize(V + L);
        float distance = length(lights[i].position - POS);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lights[i].color * attenuation;

        // Cook-Torance BRDF
        float NdotV = max(dot(N, V), 0.0000001);
        float NdotL = max(dot(N, L), 0.0000001);
        float HdotV = max(dot(H, V), 0.0);
        float NdotH = max(dot(N, H), 0.0);

        float D = distributionGGX(NdotH, roughness);
        float G = geometrySmith(NdotV, NdotL, roughness);
        vec3 F = fresnelSchlick(HdotV, baseReflectivity);
        vec3 specular = D * G * F;
        specular /= 4.0 * NdotV * NdotL;
        vec3 kD = vec3(1.0) - F;
        kD *= 1.0 - metallic;
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }
    float ao = Mat.a;

    // Ambient
    vec3 kS = fresnelSchlickRoughness(max(dot(N, V), 0.0), baseReflectivity, roughness);
    vec3 kD = 1.0 - kS;
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = vec3(0.0);
    if (u_irradianceMapSet) {
        diffuse = kD * u_ambient * irradiance * albedo;
    }
    else {
        diffuse = kD * u_ambient * albedo;
    }

    vec3 specular = vec3(0.0);
    if (u_specularIBL) {
        const float MAX_REFLECTION_LOD = 6.0;
        vec3 prefilteredColor = textureLod(u_cubemap, reflect(-V, N), roughness * MAX_REFLECTION_LOD).rgb;
        vec2 brdf = texture(u_brdf, vec2(max(dot(N, V), 0.0), roughness)).rg;
        specular = prefilteredColor * (kS * brdf.r + brdf.g);
    }



    vec3 ambient = (diffuse + specular) * ao;

    //vec3 ambient = vec3(u_ambient) * albedo * ao;
    vec3 clr = ambient + Lo;

    //clr = clr / (clr + vec3(1.0));


    color = vec4(clr, 1.0f);
}