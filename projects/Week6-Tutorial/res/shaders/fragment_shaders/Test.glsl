#version 440 core

#include "../fragments/fs_common_inputs.glsl"
#include "../fragments/frame_uniforms.glsl"
#include "../fragments/multiple_point_lights.glsl"

out vec4 frag_color;

uniform sampler2D texColor;
uniform vec3 rimColor;
uniform float rim;
void main()
{
		vec4 textureColor = texture(texColor, inUV);
		vec3 vertNormal = normalize(inNormal);
		vec3 viewDir = normalize(inViewPos);

		float difference = 1.0 - dot(vertNormal,viewDir);
		difference=step(rim,difference)*difference;

		float value = step(rim,difference)*(difference-rim)/rim;

		frag_color = vec4(textureColor.rgb + (value*rimColor) , 1.0);
}

//Rim Lighting code from built from
//https://shadowmint.gitbooks.io/unity-material-shaders/content/shaders/surface/rim_lighting.html