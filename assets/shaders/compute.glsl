#version 430
precision mediump float;
layout(local_size_x=1,local_size_y=1)in;
layout(rgba32f,binding=0)uniform image2D img_output;

struct CubeElement
{
	float x,y,z;
	float r,g,b;
	float size;
};

layout(std430,binding=4)readonly buffer Cube_data
{
	CubeElement cubes[];
};

uniform int cube_count;
uniform mat4 camMatrix;
uniform mat4 projMatrix;

struct Ray{
	vec3 direction;
	vec3 direction_inverse;
	vec3 origin;
};

struct RayHit{
	float distance;
	vec3 position;
	vec3 normal;
};

Ray create_ray(vec3 origin,vec3 direction){
	Ray ray;
	ray.origin=origin;
	ray.direction=direction;
	ray.direction_inverse=direction;
	return ray;
}
Ray create_camera_ray(vec2 uv)
{
	mat4 matrix=inverse(camMatrix);
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
vec3 max_cube=vec3(1,1,1);
vec3 min_cube=vec3(0,0,0);
bool intersect_cube(Ray ray,vec3 position,float size){
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
	
	return texit>=tenter;
	
}
RayHit CreateRayHit()
{
	RayHit hit;
	hit.position=vec3(0.f,0.f,0.f);
	hit.distance=1000000000.;
	hit.normal=vec3(0.f,0.f,0.f);
	return hit;
}

bool intersect_all_cubes(Ray ray){
	for(int i=0;i<cube_count;i++){
		
		CubeElement cube=cubes[i];
		vec3 pos=vec3(cube.x,cube.y,cube.z);
		if(intersect_cube(ray,pos,1.)){
			return true;
		}
	}
	return false;
}

vec3 intersect_all_spheres(Ray ray){
	int hit_index=-1;
	RayHit bestHit=CreateRayHit();
	for(int i=0;i<cube_count;i++){
		CubeElement cube=cubes[i];
		vec3 pos=vec3(cube.x,cube.y,cube.z);
		RayHit hit=intersect_sphere(ray,pos,1.);
		if(hit.distance>0&&hit.distance<bestHit.distance){
			bestHit=hit;
			hit_index=i;
		}
		
	}
	
	if(hit_index>-1){
		CubeElement cube=cubes[hit_index];
		vec3 light_position=vec3(2.,-5.,3.);
		vec3 direction_to_light=normalize(bestHit.position-light_position);
		float diffuse_intensity=max(0.,dot(bestHit.normal,direction_to_light));
		
		return vec3(cube.r,cube.g,cube.b)*(.5+diffuse_intensity*.5);
	}else{
		return vec3(0,0,0);
	}
}
void main(){
	// base pixel colour for image
	vec4 pixel=vec4(0.,0.,1.,1.);
	// get index in global work group i.e x,y position
	ivec2 pixel_coords=ivec2(gl_GlobalInvocationID.xy);
	//transform to -1 to 1
	vec2 uv=(vec2(pixel_coords)/(512./2.))-1.;
	Ray ray=create_camera_ray(uv);
	//ray.direction=vec3(0.,0.,1.);
	//ray.origin=vec3(pixel_coords,0.);
	
	//pixel.rgb=intersect_all_spheres(ray);
	
	if(intersect_all_cubes(ray)){
		pixel.xyz=vec3(1.,0.,0.);
	}
	
	//pixel=vec4(bestHit.distance/10.,0,0,1.);
	//pixel=vec4(bestHit.normal,1.);
	
	//pixel=vec4(ray.direction,1.);
	
	//pixel=vec4(p,1.);
	
	//
	// interesting stuff happens here later
	//
	
	// output to a specific pixel in the image
	imageStore(img_output,pixel_coords,pixel);
}