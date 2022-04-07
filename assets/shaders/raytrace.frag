#version 440 core
in vec2 uv;
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
	uint children[];
};

struct Ray{
	vec3 direction;
	vec3 direction_inverse;
	vec3 origin;
};


struct RayHit{
	float distance;
    int index;
	vec3 position;
	vec3 normal;
};


RayHit CreateRayHit()
{
	RayHit hit;
	hit.position=vec3(0.f,0.f,0.f);
	hit.distance=1000000000.;
	hit.normal=vec3(0.f,0.f,0.f);
    hit.index = -1;
	return hit;
}



Ray create_ray(vec3 origin,vec3 direction){
	Ray ray;
	ray.origin=origin;
	ray.direction=direction;
	ray.direction_inverse=direction;
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
	float maxT;
};

void main(){
	int s_max = 23;
	// base pixel colour for image
	vec4 pixel= vec4(0.,0.,0.,0.);
	StackEntry rayStack[32];
	const float epsilon = 0.00001;


	Ray ray=create_camera_ray(uv);
	
	vec3 dir = max(vec3(epsilon), abs(ray.direction));

	vec3 t_coef = vec3(-1.0f, -1.0f, -1.0f) / dir;
	vec3 t_bias = t_coef * ray.origin;

	// all flip positive axis
	// this way we don't have to calculate which planes are closer to the camera
	// but we have to also flip our voxel child axis so we don't grab the wrong one (the)
	int octant_mask = 7;
	if (dir.x > 0.0) octant_mask ^= 1, t_bias.x = 2.0 * t_coef.x - t_bias.x;
	if (dir.y > 0.0) octant_mask ^= 2, t_bias.y = 2.0 * t_coef.y - t_bias.y;
	if (dir.z > 0.0) octant_mask ^= 4, t_bias.z = 2.0 * t_coef.z - t_bias.z;
	
	float t_min = max(max(2.0 * t_coef.x - t_bias.x, 2.0f * t_coef.y - t_bias.y), 2.0 * t_coef.z - t_bias.z);
	float t_max = min(min(t_coef.x - t_bias.x, t_coef.y - t_bias.y), t_coef.z - t_bias.z);
	float h = t_max;
	t_min = max(t_min, 0.0f);
	t_max = min(t_max, 1.0f);

	float ray_scale = 0.001;

	uint parent_index = 0;
	uint child_ptr = 0;
	uint child_descriptor = 0; // invalid until fetched
	int idx = 0;
	vec3 pos = vec3(1.0f, 1.0f, 1.0f);
	int scale = s_max - 1;
	float scale_exp2 = 0.5f; // exp2f(scale - s_max)
	// intersect the outer voxel and set the index
	if (1.5 * t_coef.x - t_bias.x > t_min) idx ^= 1, pos.x = 1.5f;
	if (1.5 * t_coef.y - t_bias.y > t_min) idx ^= 2, pos.y = 1.5f;
	if (1.5 * t_coef.z - t_bias.z > t_min) idx ^= 4, pos.z = 1.5f;
	
	RayHit hit;

	

	while (scale < s_max) {

		// Fetch child descriptor unless it is already valid.
		
		if (child_descriptor == 0) {
			child_descriptor = children[parent_index];
			child_ptr = child_descriptor >> (32 - 15);
		}

		// Determine maximum t-value of the cube by evaluating
		// tx(), ty(), and tz() at its corner.
		vec3 t_corner = pos * t_coef - t_bias;
		float tc_max = min(min(t_corner.x, t_corner.y), t_corner.z);
		// Process voxel if the corresponding bit in valid mask is set
		// and the active t-span is non-empty.
		int child_shift = idx ^ octant_mask; // permute child slots based on the mirroring
		int child_masks = child_descriptor << child_shift;
		// is valid?
		if ((child_masks & 0x10000) != 0 && t_min <= t_max)
		{
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
				// Terminate if the corresponding bit in the non-leaf mask is not set.
				if ((child_masks & 0x0080) == 0)
					break; // at t_min (overridden with tv_min).
				// PUSH
				// Write current parent to the stack.
				if (tc_max < h)
					rayStack[scale] = StackEntry(parent_index, t_max);

				h = tc_max;
				// Find child descriptor corresponding to the current voxel.
			
				if ((child_ptr & 0x10000) != 0) // far
					ofs = parent[ofs * 2]; // far pointer
				ofs += popc8(child_masks & 0x7F);
				parent += ofs * 2;
				// Select child voxel that the ray enters first.
				idx = 0;
				scale--;
				scale_exp2 = half;
				if (tx_center > t_min) idx ^= 1, pos.x += scale_exp2;
				if (ty_center > t_min) idx ^= 2, pos.y += scale_exp2;
				if (tz_center > t_min) idx ^= 4, pos.z += scale_exp2;
				// Update active t-span and invalidate cached child descriptor.
				t_max = tv_max;
				child_descriptor.x = 0;
				continue;
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
				// Find the highest differing bit between the two positions.
				uint differing_bits = 0;
				if ((step_mask & 1) != 0) differing_bits |= floatBitsToUint(pos.x) ^ floatBitsToUint(pos.x + scale_exp2);
				if ((step_mask & 2) != 0) differing_bits |= floatBitsToUint(pos.y) ^ floatBitsToUint(pos.y + scale_exp2);
				if ((step_mask & 4) != 0) differing_bits |= floatBitsToUint(pos.z) ^ floatBitsToUint(pos.z + scale_exp2);
				scale = (floatBitsToUint((float)differing_bits) >> 23) - 127; // position of the highest bit
				scale_exp2 = uintBitsToFloat((scale - s_max + 127) << 23); // exp2f(scale - s_max)
				// Restore parent voxel from the stack.
				int2 stackEntry = stack[scale];
				parent = (int*)stackEntry.x;
				t_max = stackEntry.t_max;
				// Round cube position and extract child slot index.
				int shx = floatBitsToInt(pos.x) >> scale;
				int shy = floatBitsToInt(pos.y) >> scale;
				int shz = floatBitsToInt(pos.z) >> scale;
				pos.x = intBitsToFloat(shx << scale);
				pos.y = intBitsToFloat(shy << scale);
				pos.z = intBitsToFloat(shz << scale);
				idx = (shx & 1) | ((shy & 1) << 1) | ((shz & 1) << 2);
				// Prevent same parent from being stored again and invalidate cached child descriptor.
				h = 0.0f;
				child_descriptor.x = 0;
			}
		}
		
		// Indicate miss if we are outside the octree.
		if (scale >= s_max)
			t_min = 2.0f;
		// Undo mirroring of the coordinate system.
		if ((octant_mask & 1) == 0) pos.x = 3.0f - scale_exp2 - pos.x;
		if ((octant_mask & 2) == 0) pos.y = 3.0f - scale_exp2 - pos.y;
		if ((octant_mask & 4) == 0) pos.z = 3.0f - scale_exp2 - pos.z;
		// Output results.
		float hit_t = t_min;
		hit_pos.x = min(max(p.x + t_min * d.x, pos.x + epsilon), pos.x + scale_exp2 - epsilon);
		hit_pos.y = min(max(p.y + t_min * d.y, pos.y + epsilon), pos.y + scale_exp2 - epsilon);
		hit_pos.z = min(max(p.z + t_min * d.z, pos.z + epsilon), pos.z + scale_exp2 - epsilon);
		int hit_parent = parent_index;
		int hit_idx = idx ^ octant_mask ^ 7;
		int hit_scale = scale;
	}

	
	/*
    if (hit.index != -1) {
		

        Cube cube =cubes[hit.index];
		//vec3 light_position=vec3(2.,-5.,3.);
		//vec3 direction_to_light=normalize(bestHit.position-light_position);
		//float diffuse_intensity=max(0.,dot(bestHit.normal,direction_to_light));

		pixel = vec4(cube.color, 1); //*(.5+diffuse_intensity*.5);
		//float c = 1.0 / hit.distance;
		//pixel = vec4(c, c, c, 1);
    } else {
		//pixel.x = step(0.0, ray.direction.x);
		//pixel.y = step(0.0, ray.direction.y);
		//pixel.z = step(0.0, ray.direction.z);
		//pixel.xyz = vec3(0.5, 0.5, 0.5) - ray.direction*0.5;
		//pixel = vec4(hit.distance, hit.distance, hit.distance, 0);
			vec3 dir = ray.direction;
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
			

			//pixel.x = step(1.0, -t_coef.x/1000.0);
			//pixel.y = step(1.0, -t_coef.y/1000.0);
			//pixel.z = step(1.0, -t_coef.z/1000.0);
	}
	
	*/
	
    color = pixel;
	
	//float near = viewPlane.x;
	//float far = viewPlane.y;
	//float a = (far+near)/(far-near);
	//float b = 2.0*far*near/(far-near);
	//gl_FragDepth = a + b/hit.position.z;
	
	vec4 depth_vec = viewMatrix * projMatrix * vec4(hit.position.xyz, 1.0);
	float depth = ((depth_vec.z / depth_vec.w) + 1.0) * 0.5; 
	//gl_FragDepth = depth;
	
	//color = vec4((uv+1)*0.5, 1., 1.);
}