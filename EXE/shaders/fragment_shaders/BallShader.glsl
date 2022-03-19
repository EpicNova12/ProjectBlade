#version 430

#include "../fragments/fs_common_inputs.glsl"
#include "../fragments/frame_uniforms.glsl"

struct Material
{
	sampler2D texColor;
	vec3 ambientLightColor;
	vec3 direction;
	float diffuseFactor;
	vec3 normal;
	//Toggle Lighting
	bool ambientIsOn;
	bool diffuesIsOn;
	bool specularIsOn;
};

uniform Material u_Material;

out vec4 frag_color;

vec3 getDiffuseLightColor(Material diffuseLight,vec3 normal,vec4 textureDiffuse);
vec3 getAmbientLightColor(Material ambientLight);

vec3 getDiffuseLightColor(Material diffuseLight, vec3 normal,vec4 textureDiffuse)
{
	if(diffuseLight.diffuesIsOn == false)
	{
		return vec3(0.0, 0.0, 0.0);
	}

	float finalIntensity = max(0.0, dot(normal, -diffuseLight.direction));
	finalIntensity = clamp(finalIntensity*diffuseLight.diffuseFactor,0.0,1.0);

	return vec3(textureDiffuse*finalIntensity);
}

vec3 getAmbientLightColor(Material ambientLight)
{
	return ambientLight.ambientIsOn ? ambientLight.ambientLightColor :vec3 (0.0, 0.0, 0.0);
}
void main() 
{ 
	//Get Texture
	vec4 textureColor = texture(u_Material.textureColor,inUV);

	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * vec3(0.0,0.0,0.0);

	vec3 result = getAmbientLightColor(u_Material) * textureColor * getDiffuseLightColor(u_Material,u_Material.normal,textureColor);

	frag_color = vec4(result, 1.0);
}

//Shader code from Lighting and Shaders lecture slides