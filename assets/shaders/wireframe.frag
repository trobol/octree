#version 330 core
#extension GL_EXT_gpu_shader4 : enable
out vec4 color;
in vec3 mColor;

noperspective in vec3 dist;
void main(){
	color=vec4(mColor, 1);
	//vec3 dist = vec3( 1, 1, 1);
		// determine frag distance to closest edge
	float nearD = min(min(dist[0],dist[1]),dist[2]);
	float edgeIntensity = exp2(-1.0*nearD*nearD);
	//vec3 L = lightDir;
	//vec3 V = normalize(cameraPos - worldPos);
	//vec3 N = normalize(worldNormal);
	//vec3 H = normalize(L+V);
	//vec4 color = isSingleColor*singleColor + (1.0-isSingleColor)*gl_Color;
	//float amb = 0.6;
	//vec4 ambient = color * amb;
	//vec4 diffuse = color * (1.0 - amb) * max(dot(L, N), 0.0);
	//vec4 specular = vec4(0.0);
	//edgeIntensity = 0.0;

	vec4 base_color = vec4(1.0, 1.0, 1.0, 1.0);
	// blend between edge color and normal lighting color
	color = (edgeIntensity * vec4(1.0,0.1,0.1,1.0)) + ((1.0-edgeIntensity) * base_color);
}