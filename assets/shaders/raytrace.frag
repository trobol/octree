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
    Cube cubes[];
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


RayHit intersect_cube(vec3 dT, vec3 bT, vec3 small, vec3 big, Ray ray, vec3 position, float size){
	RayHit hit = CreateRayHit();
	vec3 cube_min=small*size+position;
	vec3 cube_max=big*size+position;

	vec3 t_min = dT * cube_min + bT;
	vec3 t_max = dT * cube_max + bT;

	float t_enter = max(t_min.x, max(t_min.y, t_min.z));
	float t_exit = min(t_max.x, min(t_max.y, t_max.z));

	if ( t_exit>=t_enter ) {
		
		hit.distance = t_enter;
		hit.position=ray.origin+t_enter*ray.direction;
		
		hit.normal=normalize(hit.position-position);// TODO: THIS IS WORNG
	}
	
	
	return hit;
	
}

RayHit intersect_all_cubes(Ray ray){
	vec3 dir = ray.direction;
	//dir.x = max(abs(dir.x), 0.0001);
	//dir.y = max(abs(dir.y), 0.0001);
	//dir.z = max(abs(dir.z), 0.0001);
	vec3 dT = vec3(1., 1., 1.) / dir;
	vec3 bT = -ray.origin / dir;
	vec3 big = vec3(step(0.0, dir.x), step(0.0, dir.y), step(0.0, dir.z));
	vec3 small = 1.0 - big;

	RayHit bestHit=CreateRayHit();
	for(int i=0;i<cubeCount;i++){
		Cube cube=cubes[i];
		vec3 pos= cube.pos;
		RayHit hit=intersect_cube(dT, bT, small, big, ray, pos, 1);
		if(hit.distance>0&&hit.distance<bestHit.distance){
            hit.index=i;
			bestHit=hit;
		}

	}

    return bestHit;
}


Ray create_ray(vec3 origin,vec3 direction){
	Ray ray;
	ray.origin=origin;
	ray.direction=direction;
	ray.direction_inverse=direction;
	return ray;
}

RayHit intersect_sphere(Ray ray,vec3 position,float size)
{
	// Calculate distance along the ray where the sphere is intersected
	vec3 d=ray.origin-position;
	float p1=-dot(ray.direction,d);
	float p2sqr=p1*p1-dot(d,d)+size*size;
	RayHit hit;
	if(p2sqr<0){
		hit.distance=-1.;
		return hit;
	}

	float p2=sqrt(p2sqr);
	float t=p1-p2>0?p1-p2:p1+p2;

	hit.distance=t;
	hit.position=ray.origin+t*ray.direction;
	hit.normal=normalize(hit.position-position);
	return hit;
}



RayHit intersect_all_spheres(Ray ray){
	RayHit bestHit=CreateRayHit();
	for(int i=0;i<cubeCount;i++){
		Cube cube=cubes[i];
		vec3 pos= cube.pos+vec3(0.5,0.5,0.5);
		RayHit hit=intersect_sphere(ray,pos, .5);
		if(hit.distance>0&&hit.distance<bestHit.distance){
            hit.index=i;
			bestHit=hit;
		}

	}

    return bestHit;
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


void main(){
	// base pixel colour for image
	vec4 pixel= vec4(0.,0.,0.,0.);


	Ray ray=create_camera_ray(uv);
	
  
    RayHit hit = intersect_all_cubes(ray);
    if (hit.index != -1) {
		

        Cube cube =cubes[hit.index];
		//vec3 light_position=vec3(2.,-5.,3.);
		//vec3 direction_to_light=normalize(bestHit.position-light_position);
		//float diffuse_intensity=max(0.,dot(bestHit.normal,direction_to_light));

		pixel = vec4(cube.color, 1); //*(.5+diffuse_intensity*.5);
		float c = 1.0 / hit.distance;
		pixel = vec4(c, c, c, 1);
    } else {
		//pixel.x = step(0.0, ray.direction.x);
		//pixel.y = step(0.0, ray.direction.y);
		//pixel.z = step(0.0, ray.direction.z);
		//pixel.xyz = vec3(0.5, 0.5, 0.5) - ray.direction*0.5;
		//pixel = vec4(hit.distance, hit.distance, hit.distance, 0);
	}
	
	
	
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