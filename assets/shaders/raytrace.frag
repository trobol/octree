#version 450 core

#define STACK_SIZE 23 //must be 23
#define EPS 3.552713678800501e-15

in vec2 uv;
in vec2 screen_uv;
out vec4 color;


struct Cube
{
 vec3 pos;
 vec3 color;
 float size;
};

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform int cubeCount;
uniform vec2 viewPlane; // near, far
uniform float size;



layout(std430, binding = 3) buffer CubesBuffer
{
	uint children[500];
};

struct Ray{
	vec3 dir;
	vec3 dir_inverse;
	vec3 origin;
};


struct RayHit{
	float dist;
	vec3 pos;
	vec3 norm;
	int idx;
	int parent;
	float scale;
};


RayHit CreateRayHit()
{
	RayHit hit;
	hit.pos=vec3(0.f,0.f,0.f);
	hit.dist=1000000000.;
	hit.norm=vec3(0.f,0.f,0.f);
    hit.idx = -1;
	hit.parent = -1;
	hit.scale = 0;
	return hit;
}



Ray create_ray(vec3 origin,vec3 direction){
	Ray ray;
	ray.origin=origin;
	ray.dir=direction;
	ray.dir_inverse=direction;
	return ray;
}

Ray create_camera_ray(vec2 uv)
{
	mat4 matrix=inverse(viewMatrix);
	mat4 proj=inverse(projMatrix);

	// Transform the camera origin to world space
	vec3 origin=(matrix*vec4(0.f,0.f,0.f,1.f)).xyz;

	// Invert the perspective projection of the view-space position
	vec3 direction=(proj*vec4(uv,0.f,1.f)).xyz;
	//vec3 direction=vec3(0.,0.,1.);
	// Transform the direction from camera to world space and normalize
	direction=(matrix*vec4(direction,0.f)).xyz;
	direction=normalize(direction);

	//origin = vec3(uv, 0)+vec3(1.5f, 1.5f, 0);
	//direction = normalize(vec3(screen_uv*0.1, 1));
	return create_ray(origin,direction);
}

struct StackEntry {
	uint offset;
	float t_max;
} stack[STACK_SIZE];

layout (binding = 0) uniform usampler1D texture1;



RayHit traverse_octree(Ray ray) {
	

	vec3 d = normalize(ray.dir);
	vec3 o = ray.origin;
	d.x = abs(d.x) > EPS ? d.x : (d.x >= 0 ? EPS : -EPS);
	d.y = abs(d.y) > EPS ? d.y : (d.y >= 0 ? EPS : -EPS);
	d.z = abs(d.z) > EPS ? d.z : (d.z >= 0 ? EPS : -EPS);

	vec3 t_coef = 1.0f / -abs(d);
	vec3 t_bias = t_coef * o;

	// all flip positive axis
	// this way we don't have to calculate which planes are closer to the camera
	// but we have to also flip our voxel child axis so we don't grab the wrong one (the)
	uint oct_mask = 7u;
	if(d.x > 0.0f) oct_mask ^= 1u, t_bias.x = 3.0f * t_coef.x - t_bias.x;
	if(d.y > 0.0f) oct_mask ^= 2u, t_bias.y = 3.0f * t_coef.y - t_bias.y;
	if(d.z > 0.0f) oct_mask ^= 4u, t_bias.z = 3.0f * t_coef.z - t_bias.z;
	
	float t_min = max(max(2.0 * t_coef.x - t_bias.x, 2.0 * t_coef.y - t_bias.y), 2.0 * t_coef.z - t_bias.z);
	float t_max = min(min(      t_coef.x - t_bias.x,       t_coef.y - t_bias.y),       t_coef.z - t_bias.z);
	float h = t_max;
	
	t_min = max(t_min, 0.0);


	uint parent = 0u;
	uint cur = 0u;
	vec3 pos = vec3(1.0f);
	uint idx = 0u;
	// intersect the outer voxel and set the index
	if (1.5f * t_coef.x - t_bias.x > t_min) idx ^= 1u, pos.x = 1.5f;
	if (1.5f * t_coef.y - t_bias.y > t_min) idx ^= 2u, pos.y = 1.5f;
	if (1.5f * t_coef.z - t_bias.z > t_min) idx ^= 4u, pos.z = 1.5f;
	
	uint scale = STACK_SIZE - 1;
	float scale_exp2 = 0.5f;

	RayHit hit = CreateRayHit();
	hit.idx = 1;

	vec3 colors[8] = {
		vec3(0.6, 0.2, 0.5), // 0
		vec3(0.5, 0.5, 0.3), // 1
		vec3(0.8, 1.0, 0.1), // 2
		vec3(0.2, 0.2, 0.6),
		vec3(0.0, 1.0, 0.0),
		vec3(0.5, 0.1, 0.0),
		vec3(0.0, 0.0, 1.0),
		vec3(0.9, 0.8, 0.0)
	};
	uint i = 0;
	while (scale < STACK_SIZE) {
		i++;
		// Fetch child descriptor unless it is already valid.
		if (cur == 0u) cur = children[parent];


		// Determine maximum t-value of the cube by evaluating
		// tx(), ty(), and tz() at its corner.
		vec3 t_corner = pos * t_coef - t_bias;
		float tc_max = min(min(t_corner.x, t_corner.y), t_corner.z);


		uint child_shift = idx ^ oct_mask;
		uint child_masks = cur << child_shift;

		// is valid?
		if ((child_masks & 0x8000u) != 0 && t_min <= t_max)
		{
			
			float tv_max = min(t_max, tc_max);
			float half_scale_exp2 = scale_exp2 * 0.5f;
			vec3 t_center = (half_scale_exp2 * t_coef) + t_corner;


			if( t_min <= tv_max ) {	
				if( (child_masks & 0x80u) != 0 ) { // leaf node 
					//t_min = max(t_min, );
					//uint color_index = children[]
					//norm.color = 
		
					break;
				}	
					// PUSH
				if( tc_max < h ) {
					stack[ scale ].offset = parent;
					stack[ scale ].t_max = t_max;
				}
				h = tc_max;

				// mask out 15 bits
				parent += (cur >> 17) + 7 - child_shift;

				idx = 0u;
				scale--;
				scale_exp2 = half_scale_exp2;
				if(t_center.x > t_min) idx ^= 1u, pos.x += scale_exp2;
				if(t_center.y > t_min) idx ^= 2u, pos.y += scale_exp2;
				if(t_center.z > t_min) idx ^= 4u, pos.z += scale_exp2;

				cur = 0;
				t_max = tv_max;

				continue;
			}
			
		} 
		
		// ADVANCE
		// Step along the ray.
		uint step_mask = 0u;
		if (t_corner.x <= tc_max) step_mask ^= 1u, pos.x -= scale_exp2;
		if (t_corner.y <= tc_max) step_mask ^= 2u, pos.y -= scale_exp2;
		if (t_corner.z <= tc_max) step_mask ^= 4u, pos.z -= scale_exp2;

		// Update active t-span and flip bits of the child slot index.
		t_min = tc_max;
		idx ^= step_mask;
		// Proceed with pop if the bit flips disagree with the ray direction.
		if ((idx & step_mask) != 0)
		{
			
			// POP
			// proceed to the next sibling of the highest ancestor that the ray exits.
			// find the "scale" of the difference of old position with the new
			// scale_exp2 is always a power of 2 is >= 0.5 so the Mantissa is always 0
			// first we get the highest set bit in the exponent of all the pos and 

			// Find the highest differing bit between the two positions.
			uint differing_bits = 0;
			if ((step_mask & 1) != 0) differing_bits |= floatBitsToUint(pos.x) ^ floatBitsToUint(pos.x + scale_exp2); 
			if ((step_mask & 2) != 0) differing_bits |= floatBitsToUint(pos.y) ^ floatBitsToUint(pos.y + scale_exp2);
			if ((step_mask & 4) != 0) differing_bits |= floatBitsToUint(pos.z) ^ floatBitsToUint(pos.z + scale_exp2);
			scale = findMSB(differing_bits);
			scale_exp2 = uintBitsToFloat((scale - STACK_SIZE + 127u) << 23u); // exp2f(scale - s_max)

			
			// Restore parent voxel from the stack.
			// Restore parent voxel from the stack.
			parent = stack[scale].offset;
			t_max  = stack[scale].t_max;

			
			// Round cube position and extract child slot index.
			uint shx = floatBitsToUint(pos.x) >> scale;
			uint shy = floatBitsToUint(pos.y) >> scale;
			uint shz = floatBitsToUint(pos.z) >> scale;
			pos.x = uintBitsToFloat(shx << scale);
			pos.y = uintBitsToFloat(shy << scale);
			pos.z = uintBitsToFloat(shz << scale);
			idx  = (shx & 1u) | ((shy & 1u) << 1u) | ((shz & 1u) << 2u);

			// Prevent same parent from being stored again and invalidate cached child descriptor.
			h = 0.0f;
			cur = 0;
		}
	}
		vec3 norm, t_corner = t_coef * (pos + scale_exp2) - t_bias;
	
	if ((oct_mask & 1) != 0) pos.x = 3.0f - scale_exp2 - pos.x;
	if ((oct_mask & 2) != 0) pos.y = 3.0f - scale_exp2 - pos.y;
	if ((oct_mask & 4) != 0) pos.z = 3.0f - scale_exp2 - pos.z;


	
	if(t_corner.x > t_corner.y && t_corner.x > t_corner.z)
		norm = vec3(1, 0, 0);
	else if(t_corner.y > t_corner.z)
		norm = vec3(0, 1, 0);
	else
		norm = vec3(0, 0, 1);

	if ((oct_mask & 1u) != 0u) norm.x = -norm.x;
	if ((oct_mask & 2u) != 0u) norm.y = -norm.y;
	if ((oct_mask & 4u) != 0u) norm.z = -norm.z;
	
	// Indicate miss if we are outside the octree.
	if (scale >= STACK_SIZE) {
		hit.norm = vec3(0);
	} else {
		vec3 lightDir = normalize(vec3(0, -1.0, -0.5));
		float diff = max(dot(norm, lightDir), 0.0);
		hit.norm = vec3(0.5 + diff * 0.5);
		//hit.norm = norm + 1.0f / 2.0;//colors[step_mask];
	}

	
	

	return hit;
}


void main(){
	
	Ray ray=create_camera_ray(screen_uv);
	/*
	vec3 d = ray.dir;//normalize(vec3(screen_uv*0.1, 1.0));
	vec3 o = ray.origin;//vec3(screen_uv, 0);
	d.x = abs(d.x) > EPS ? d.x : (d.x >= 0 ? EPS : -EPS);
	d.y = abs(d.y) > EPS ? d.y : (d.y >= 0 ? EPS : -EPS);
	d.z = abs(d.z) > EPS ? d.z : (d.z >= 0 ? EPS : -EPS);

	vec3 t_coef = 1.0f / -abs(d);
	vec3 t_bias = t_coef * o;

	//float top = 2.0f * t_coef.x - t_bias.x;
	//float  = 2.0f * t_coef.y - t_bias.y;
	//float btm = 

	float t_min = max(max(2.0 * t_coef.x - t_bias.x, 2.0f * t_coef.y - t_bias.y), 2.0f * t_coef.z - t_bias.z);
	float t_max = min(min(1.0f * t_coef.x - t_bias.x, 1.0f * t_coef.y - t_bias.y), 1.0f * t_coef.z - t_bias.z);
	//color=vec4(back, back, back, 1.0);
	//return;
	if (t_min <= t_max) {
		float t = 1/-t_min;
		color=vec4(t, t, t, 1.0);
		
	} else {
		color=vec4(d, 1.0);
	}
	return;
	*/


	RayHit hit = traverse_octree(ray);
	vec4 pixel = vec4(0., 0., 0., 1.);

	int index = int(uv.y * 8.0);
	int section = int(uv.x * 3.0);
	
	int mask = int(uv.x * 32.0);

	float s = 0;

	color=vec4(hit.norm, 1.0f);
	return;

	//s = float(bitfieldExtract(texture(texture1, index).r, mask, 1));

	//uint a = texture(texture1, index).r; 
   	//s = float((a >> (mask - 1)) & 1);

	//color = vec4(s, mask % 2, s, 1);
	//return;
    if (hit.idx != -1 ) {
		
		
        //Cube cube =cubes[hit.idx];
		//vec3 light_position=vec3(2.,-5.,3.);
		//vec3 direction_to_light=normalize(bestHit.pos-light_position);
		//float diffuse_intensity=max(0.,dot(bestHit.nor,direction_to_light));

		//pixel = vec4(cube.color, 1); //*(.5+diffuse_intensity*.5);
		//float c = 1.0 / hit.dist;
		//pixel = vec4(c, c, c, 1);
		pixel.xyz = hit.norm;
    } else {
		//pixel.x = step(0.0, ray.dir.x);
		//pixel.y = step(0.0, ray.dir.y);
		//pixel.z = step(0.0, ray.dir.z);
		//pixel.xyz = vec3(0.5, 0.5, 0.5) - ray.dir*0.5;
		//pixel = vec4(hit.dist, hit.dist, hit.dist, 0);
			vec3 dir = ray.dir;
			vec3 d = dir;
			float epsilon = 0.00001;
			
			d.x = max(abs(d.x), epsilon);
			d.y = max(abs(d.y), epsilon);
			d.z = max(abs(d.z), epsilon);

			vec3 t_coef = 1.0 / -d;
			vec3 t_bias = t_coef * ray.origin;

			if (dir.x > 0.0) t_bias.x = size * t_coef.x - t_bias.x;
			if (dir.y > 0.0) t_bias.y = size * t_coef.y - t_bias.y;
			if (dir.z > 0.0) t_bias.z = size * t_coef.z - t_bias.z;
			

			pixel.x = -t_coef.x/10.0;
			pixel.y = -t_coef.y/10.0;
			pixel.z = -t_coef.z/10.0;
	}
	
	
	
    color = pixel;
	
	//float near = viewPlane.x;
	//float far = viewPlane.y;
	//float a = (far+near)/(far-near);
	//float b = 2.0*far*near/(far-near);
	//gl_FragDepth = a + b/hit.position.z;
	
	vec4 depth_vec = viewMatrix * projMatrix * vec4(hit.pos.xyz, 1.0);
	float depth = ((depth_vec.z / depth_vec.w) + 1.0) * 0.5; 
	//gl_FragDepth = depth;
	
	//color = vec4((uv+1)*0.5, 1., 1.);
}