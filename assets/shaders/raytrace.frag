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

vec3 max_cube=vec3(1,1,1);
vec3 min_cube=vec3(0,0,0);
RayHit intersect_cube(Ray ray,vec3 position,float size){
	RayHit hit = CreateRayHit();
	vec3 cube_min=min_cube*size+position;
	vec3 cube_max=max_cube*size+position;

	float txmin=(cube_min.x-ray.origin.x)/ray.direction.x;
	float txmax=(cube_max.x-ray.origin.x)/ray.direction.x;

	float txenter=min(txmin,txmax);
	float txexit=max(txmin,txmax);

	float tymin=(cube_min.y-ray.origin.y)/ray.direction.y;
	float tymax=(cube_max.y-ray.origin.y)/ray.direction.y;

	float tyenter=min(tymin,tymax);
	float tyexit=max(tymin,tymax);

	float tzmin=(cube_min.z-ray.origin.z)/ray.direction.z;
	float tzmax=(cube_max.z-ray.origin.z)/ray.direction.z;

	float tzenter=min(tzmin,tzmax);
	float tzexit=max(tzmin,tzmax);

	float tenter=max(txenter,max(tyenter,tzenter));
	float texit=min(txexit,min(tyexit,tzexit));

	if ( texit>=tenter) {
		
		hit.distance = tenter;
		hit.position=ray.origin+tenter*ray.direction;
		
		hit.normal=normalize(hit.position-position);// TODO: THIS IS WORNG
	}
	
	return hit;
	
}

RayHit intersect_all_cubes(Ray ray){
	RayHit bestHit=CreateRayHit();
	for(int i=0;i<cubeCount;i++){
		Cube cube=cubes[i];
		vec3 pos= cube.pos;
		RayHit hit=intersect_cube(ray,pos, 1);
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