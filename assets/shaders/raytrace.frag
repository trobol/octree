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
	return create_ray(origin,direction);
}

struct StackEntry {
	uint offset;
	float t_max;
} stack[STACK_SIZE];

layout (binding = 0) uniform usampler1D texture1;

RayHit traverse_octree(Ray ray) {
	const int s_max = 23;
	// base pixel colour for image
	vec4 pixel= vec4(0.,0.,0.,0.);
	const float epsilon = 0.00001;


	vec3 dir = max(vec3(epsilon), abs(ray.dir));

	vec3 t_coef = vec3(-1.0, -1.0, -1.0) / dir;
	vec3 t_bias = t_coef * ray.origin;

	// all flip positive axis
	// this way we don't have to calculate which planes are closer to the camera
	// but we have to also flip our voxel child axis so we don't grab the wrong one (the)
	uint octant_mask = 0u;
	if (ray.dir.x > 0.0) octant_mask ^= 1, t_bias.x = 3.0 * t_coef.x - t_bias.x;
	if (ray.dir.y > 0.0) octant_mask ^= 2, t_bias.y = 3.0 * t_coef.y - t_bias.y;
	if (ray.dir.z > 0.0) octant_mask ^= 4, t_bias.z = 3.0 * t_coef.z - t_bias.z;
	
	float t_min = max(max(2.0 * t_coef.x - t_bias.x, 2.0 * t_coef.y - t_bias.y), 2.0 * t_coef.z - t_bias.z);
	float t_max = min(min(t_coef.x - t_bias.x, t_coef.y - t_bias.y), t_coef.z - t_bias.z);
	float h = t_max;
	t_min = max(t_min, 0.0);
	//t_max = min(t_max, 1.0);

	float ray_scale = 0.001;

	uint parent_index = 0;
	// just the ptr
	uint child_ptr = 0;
	// the full child ptr, flags and pointer
	uint child_descriptor = 0; // invalid until fetched
	uint idx = 0;
	vec3 pos = vec3(1.0, 1.0, 1.0);
	int scale = s_max - 1;
	float scale_exp2 = 0.5; // exp2f(scale - s_max)
	// intersect the outer voxel and set the index
	if (1.5 * t_coef.x - t_bias.x > t_min) idx ^= 1u, pos.x += scale_exp2;
	if (1.5 * t_coef.y - t_bias.y > t_min) idx ^= 2u, pos.y += scale_exp2;
	if (1.5 * t_coef.z - t_bias.z > t_min) idx ^= 4u, pos.z += scale_exp2;
	
	RayHit hit = CreateRayHit();

	vec3 colors[8] = {
		vec3(0.6, 0.2, 0.5),
		vec3(0.5, 0.5, 0.3),
		vec3(0.8, 1.0, 0.1),
		vec3(0.2, 0.2, 0.6),
		vec3(0.0, 1.0, 0.0),
		vec3(0.5, 0.1, 0.0),
		vec3(0.0, 0.0, 1.0),
		vec3(0.9, 0.8, 0.0)
	};
	/*
	if (t_min > t_max) {
		hit.norm = vec3(0, 0, 1);
		hit.idx = 1;
		return hit;
	}
	*/
	uint cur =  parent + ( idx ^ oct_mask );
	int i = 0;
	while (scale < s_max) {
		i++;

		if (i > 20) {
			hit.idx = 1;
			hit.norm = colors[23-scale];
			return hit;
		}
		// Fetch child descriptor unless it is already valid.
		
		if (child_descriptor == 0) {
			cur =  parent + ( idx ^ oct_mask );
		}

		// Determine maximum t-value of the cube by evaluating
		// tx(), ty(), and tz() at its corner.
		vec3 t_corner = pos * t_coef - t_bias;
		float tc_max = min(min(t_corner.x, t_corner.y), t_corner.z);
		// Process voxel if the corresponding bit in valid mask is set
		// and the active t-span is non-empty.
		int child_shift = int(idx ^ octant_mask); // permute child slots based on the mirroring
		uint child_masks = child_descriptor >> child_shift;
		
		// is valid?
		if ((child_masks & 0x80000000u) != 0 && t_min <= t_max)
		{
			if (scale == 21) {
				hit.idx = 1;
				hit.norm = colors[child_ptr];
				return hit;
			}
			// Terminate if the voxel is small enough.
			if (tc_max * ray_scale >= scale_exp2)
				break; // at t_min
			// INTERSECT
			// Intersect active t-span with the cube and evaluate
			// tx(), ty(), and tz() at the center of the voxel.
			float tv_max = min(t_max, tc_max);
			float _half = scale_exp2 * 0.5;
			float tx_center = _half * t_coef.x + t_corner.x;
			float ty_center = _half * t_coef.y + t_corner.y;
			float tz_center = _half * t_coef.z + t_corner.z;
			/*
			// Intersect with contour if the corresponding bit in contour mask is set.
			int contour_mask = child_descriptor.y << child_shift;
			if ((contour_mask & 0x80) != 0)
			{
				int ofs = (unsigned int)child_descriptor.y >> 8; // contour pointer
				int value = parent[ofs + popc8(contour_mask & 0x7F)]; // contour value
				float cthick = (float)(unsigned int)value * scale_exp2 * 0.75f; // thickness
				float cpos = (float)(value << 7) * scale_exp2 * 1.5f; // position
				float cdirx = (float)(value << 14) * d.x; // nx
				float cdiry = (float)(value << 20) * d.y; // ny
				float cdirz = (float)(value << 26) * d.z; // nz
				float tcoef = 1.0f / (cdirx + cdiry + cdirz);
				float tavg = tx_center * cdirx + ty_center * cdiry + tz_center * cdirz + cpos;
				float tdiff = cthick * tcoef;
				t_min = fmaxf(t_min, tcoef * tavg - fabsf(tdiff)); // Override t_min with tv_min.
				tv_max = fminf(tv_max, tcoef * tavg + fabsf(tdiff));
			}
			*/
			// Descend to the first child if the resulting t-span is non-empty.
			if (t_min <= tv_max)
			{
			
				// Terminate if the corresponding bit in the leaf mask is set.
				if ((child_masks & 0x80) != 0) {
					hit.idx = 1;
					float darkness = 1.0;
					if (tx_center > t_min) darkness -= 0.1;
					if (ty_center > t_min) darkness -= 0.1;
					if (tz_center > t_min) darkness -= 0.1;
					hit.norm = vec3(0.0, 0.3, 0.8) * darkness;
					hit.norm = colors[23-scale];
					return hit;
					break; // at t_min (overridden with tv_min).
				}
				// ==== PUSH =====
				// Write current parent to the stack.
				if (tc_max < h) 
					rayStack[scale] = StackEntry(parent_index, t_max);

				h = tc_max;
				// Find child descriptor corresponding to the current voxel.
				
				uint ofs = child_ptr; // child pointer
				// TODO: far pointers
				//if ((child_ptr & 0x10000) != 0) // far
				//	ofs = parent[ofs * 2]; // far pointer
				
				
				// TODO: sibling count
				//uint sibling_count = bitcount (child_descriptor & 0xF);
				//ofs += sibling_count;
				//parent += ofs * 2; // why is this multiplied by 2?
				
				parent_index += ofs;
				
				// Select child voxel that the ray enters first.
				idx = 0;
				scale--;
				scale_exp2 = _half;
				if (tx_center > t_min) idx ^= 1, pos.x += scale_exp2;
				if (ty_center > t_min) idx ^= 2, pos.y += scale_exp2;
				if (tz_center > t_min) idx ^= 4, pos.z += scale_exp2;
				// Update active t-span and invalidate cached child descriptor.
				t_max = tv_max;
				child_descriptor = 0;
				continue;
			}
		}
		
		// ADVANCE
		// Step along the ray.
		int step_mask = 0;
		if (t_corner.x <= tc_max) step_mask ^= 1, pos.x -= scale_exp2;
		if (t_corner.y <= tc_max) step_mask ^= 2, pos.y -= scale_exp2;
		if (t_corner.z <= tc_max) step_mask ^= 4, pos.z -= scale_exp2;
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
			scale = int((floatBitsToUint(float(differing_bits)) << 23) - 127);
			scale_exp2 = uintBitsToFloat((scale - s_max + 127) >> 23); // exp2f(scale - s_max)

			if (scale >= 23) {
				hit.idx = 1;
				hit.norm = vec3(1, 0, 0);
				return hit;
			}
			// Restore parent voxel from the stack.
			StackEntry stackEntry = rayStack[scale];
			parent_index = stackEntry.offset;
			t_max = stackEntry.t_max;
			// Round cube position and extract child slot index.
			int shx = floatBitsToInt(pos.x) >> scale;
			int shy = floatBitsToInt(pos.y) >> scale;
			int shz = floatBitsToInt(pos.z) >> scale;
			pos.x = intBitsToFloat(shx << scale);
			pos.y = intBitsToFloat(shy << scale);
			pos.z = intBitsToFloat(shz << scale);
			idx = (shx & 1) | ((shy & 1) >> 1) | ((shz & 1) >> 2);
			// Prevent same parent from being stored again and invalidate cached child descriptor.
			h = 0.0f;
			child_descriptor = 0;
		}
	}

	// Indicate miss if we are outside the octree.
	if (scale >= s_max) {
		hit.idx = 1;
		hit.norm = vec3(1, 0.5, 0);
	}

	return hit;
	// Undo mirroring of the coordinate system.
	if ((octant_mask & 1) == 0) pos.x = 3.0f - scale_exp2 - pos.x;
	if ((octant_mask & 2) == 0) pos.y = 3.0f - scale_exp2 - pos.y;
	if ((octant_mask & 4) == 0) pos.z = 3.0f - scale_exp2 - pos.z;

	vec3 p = ray.origin;
		vec3 d = ray.dir;
		// Output results.
		float hit_t = t_min;

	hit.pos.x = min(max(p.x + t_min * d.x, pos.x + epsilon), pos.x + scale_exp2 - epsilon);
	hit.pos.y = min(max(p.y + t_min * d.y, pos.y + epsilon), pos.y + scale_exp2 - epsilon);
	hit.pos.z = min(max(p.z + t_min * d.z, pos.z + epsilon), pos.z + scale_exp2 - epsilon);
	hit.parent = int(parent_index);
	hit.idx = int(idx ^ octant_mask ^ 7);
	hit.dist = abs(t_min);
	hit.idx = 1;

	if (scale >= s_max)
		hit.idx = -1;
	
	return hit;
}


void main(){
	
	Ray ray=create_camera_ray(screen_uv);
	RayHit hit = traverse_octree(ray);
	vec4 pixel = vec4(0., 0., 0., 1.);

	int index = int(uv.y * 8.0);
	int section = int(uv.x * 3.0);
	
	int mask = int(uv.x * 32.0);

	float s = 0;


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