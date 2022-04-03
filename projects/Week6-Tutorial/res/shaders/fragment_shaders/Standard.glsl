#version 440 core

#include "../fragments/fs_common_inputs.glsl"
#include "../fragments/multiple_point_lights.glsl"
#include "../fragments/color_correction.glsl"

//out vec4 frag_color;

layout(location = 0) out vec4 frag_color;
layout(location = 1) out vec4 normals;
layout(location = 2) out vec4 emissive;
layout(location = 3) out vec3 view_pos;

struct AmbientLight
{
	vec3 color;
	float ambientStrength;
	bool isOn;
};

struct DiffuseLight
{
	vec3 color;
	vec3 direction;
	float factor;
	bool isOn;
};

struct SpecularLight
{
	float specularStrength;
	bool isOn;
};

struct CustomShader
{
	vec3 rimColor;
	float rim;
	bool isOn;
};

//Uniforms
uniform sampler2D texColor;
uniform sampler2D emissiveMap;
uniform vec3 emissiveColor;
uniform float emissiveIntensity;
uniform bool toggleColorCorrect;

uniform AmbientLight u_ALight;
uniform DiffuseLight u_DLight;
uniform SpecularLight u_SLight;
uniform CustomShader u_Custom;

//Get Each Lighting Type
vec3 getAmbientLight(AmbientLight ambientLight);
vec3 getDiffuseLightColor(DiffuseLight diffuseLight,vec3 normal);

vec3 getAmbientLight(AmbientLight ambientLight)
{
	return ambientLight.isOn ? ambientLight.color * ambientLight.ambientStrength : vec3(0.0, 0.0, 0.0);
}

vec3 getDiffuseLightColor(DiffuseLight diffuseLight,vec3 normal)
{
	if(diffuseLight.isOn == false)
	{
		return vec3(0.0, 0.0, 0.0);
	}

	float finalIntensity = max(0.0, dot(normal, -diffuseLight.direction));
	finalIntensity = clamp(finalIntensity*diffuseLight.factor,0.0,1.0);

	return vec3(diffuseLight.color*finalIntensity);
}

void main()
{
		vec4 textureColor = texture(texColor, inUV);
		vec3 vertNormal = normalize(inNormal);

		//Specular
		vec3 viewDir = normalize(inViewPos);
		vec3 reflectDir = reflect(-u_DLight.direction,vertNormal);
		float spec = pow(max(dot(viewDir,reflectDir),0.0),32);
		vec3 specular =  u_SLight.specularStrength * spec * u_DLight.color;
		if(u_SLight.isOn==false)
		{
			specular = vec3(0.0,0.0,0.0);
		}
		//Custom Shader
		float difference = 1.0 - dot(vertNormal,viewDir);

		difference=step(u_Custom.rim,difference)* difference;

		float value = step(u_Custom.rim,difference)*(difference-u_Custom.rim)/u_Custom.rim;
		//Emissive
		emissive = texture(emissiveMap, inUV);
		emissive = vec4(emissive.rgb * emissiveColor * emissiveIntensity,1.0f);

		vec3 result = vec3(textureColor.rgb *(getAmbientLight(u_ALight) + getDiffuseLightColor(u_DLight,vertNormal) + specular));

		if(u_Custom.isOn==true)
		{
			result = vec3(textureColor.rgb *(getAmbientLight(u_ALight) + getDiffuseLightColor(u_DLight,vertNormal) + specular + (value*u_Custom.rimColor)));
		}
		
		if(toggleColorCorrect==true)
		{
			frag_color = vec4(ColorCorrect(result),1.0);
		}
		else
		{
			frag_color = vec4(result,1.0);
		}

		normals = vec4(vertNormal,1.0);
		view_pos=inViewPos;
}
//Rim Lighting code from built from
//https://shadowmint.gitbooks.io/unity-material-shaders/content/shaders/surface/rim_lighting.html