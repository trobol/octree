#include <octree/load_obj.h>
#include <stdio.h>


vec3 load_pos(char* buffer);
vec2 load_uv(char* buffer);



float read_float(char** ptr);
int read_int(char** ptr);



// TODO: support materials
// TODO: support multiple materials
bool load_obj(const char* path, std::vector<Face>& faces) {

	FILE* fp = fopen(path, "r");
	if (fp == NULL) {
		printf("unable to open %s\n", path);
		return false;
	}
	char line_buffer[1024] = {};
	char* ptr = line_buffer + 1023;
	size_t bytes_read = 0;
	

	std::vector<vec3> positions;
	std::vector<vec2> uvs;
	std::vector<vec3> normals;
	
	positions.push_back(vec3{0, 0, 0}); // default
	uvs.push_back(vec2{0, 0}); // default
	normals.push_back(vec3{0, 0, 0}); // default
	do {
		if (ptr > line_buffer+1024) {
			puts("ERROR: PTR READ OVER END OF BUFFER");
			return false;
		}
		while(*ptr != 0 && *ptr != '\n') ptr++;
		if (*ptr == '\n') ptr++;
		size_t free_space = ptr - line_buffer;
		size_t left_over = 1023 - free_space;
		memmove(line_buffer, ptr, left_over);
		bytes_read = fread(line_buffer+left_over, 1, free_space, fp);
		line_buffer[left_over+bytes_read] = 0;

		ptr = line_buffer;
		char hdr0 = ptr[0];
		char hdr1 = ptr[1];
		if (hdr1 == ' ') ptr += 2;
		else ptr += 3;

		

		if (hdr0 == 0 || hdr1 == 0)
			break;
		// comment
		if (hdr0 == '#')
			continue;
		
		
		// uvs
		if (hdr0 == 'v' && hdr1 == 't') {
			vec2 uv;
			uv.x = read_float(&ptr);
			uv.y = read_float(&ptr);
			float z = read_float(&ptr);
			uvs.push_back(uv);
			continue;
		}

		// normals
		if (hdr0 == 'v' && hdr1 == 'n') {
			vec3 norm;
			norm.x = read_float(&ptr);
			norm.y = read_float(&ptr);
			norm.z = read_float(&ptr);
			float w = read_float(&ptr);
			normals.push_back(norm);
			continue;
		}
		
		// positions
		if (hdr0 == 'v' && hdr1 == ' ') {
			vec3 pos;
			pos.x = read_float(&ptr);
			pos.y = read_float(&ptr);
			pos.z = read_float(&ptr);
			float w = read_float(&ptr);
			positions.push_back(pos);

			continue;
		}

		
		// indices
		if (hdr0 == 'f' && hdr1 == ' ') {
			
			Vertex vertices[4] = {};
			unsigned int count = 0;
			for (;*ptr != '\n' && *ptr != 0; count++) {
				
				int pos = 0, uv = 0, norm = 0;
				pos = read_int(&ptr);
				if (*ptr == '/') ptr++, uv = read_int(&ptr);
				if (*ptr == '/') ptr++, norm = read_int(&ptr);

				if (pos < 0) pos = (int)positions.size() + pos + 1;
				if (uv < 0) uv = (int)uvs.size() + uv + 1;
				if (norm < 0) norm = (int)normals.size() + norm + 1;


				vertices[count].pos = positions[pos];
				vertices[count].uv = uvs[uv];
				vertices[count].norm = normals[norm];

					
				if (count > 4) {
					puts("ERROR: vertex count greater than 4 encountered");
					break;
				}
			}
			
			Face face0;
			face0.vertices[0] = vertices[0];
			face0.vertices[1] = vertices[1];
			face0.vertices[2] = vertices[2];
			faces.push_back(face0);

			// if quad, we are splitting it into two triangles
			if ( count > 3 ) {
				// add the other triangle
				// TODO: check that this is the right way around, that the triangle is faceing the right way
				Face face1;
				face1.vertices[0] = vertices[2];
				face1.vertices[1] = vertices[3];
				face1.vertices[2] = vertices[0];
				faces.push_back(face1);
			}
			
			continue;
		}
		
	} while(line_buffer[0] != 0);
	/*
	for (Face face : faces) {
		printf("%f, %f, %f\n", face.vertices[0].pos.x, face.vertices[0].pos.y, face.vertices[0].pos.z);
		printf("%f, %f, %f\n", face.vertices[1].pos.x, face.vertices[1].pos.y, face.vertices[1].pos.z);
		printf("%f, %f, %f\n", face.vertices[2].pos.x, face.vertices[2].pos.y, face.vertices[2].pos.z);
		puts("====");
	}
	*/
	
	return true;
}

#define isdigit(c) (c >= '0' && c <= '9')

float read_float(char** ptr) {
	char* s = *ptr;
	// skip whitespace
	while(*s == ' ') s++;
	// early return if no number
	if (!(isdigit(*s) || *s == '-')) {
		*ptr = s;
		return 0.0f;
	}

	unsigned int a = 0;
	int e = 0;
	int c;
	int sign = 1;
	
	if (*s == '-') s++, sign = -1;


	while ((c = *s++), isdigit(c))
		a = a*10 + (c - '0');

	if (c == '.') {
		while ((c = *s++), isdigit(c)) {
			a = a*10 + (c - '0');
			e--;
		}
	}
	
  	if (c == 'e' || c == 'E') {
		int esign = 1;
		int i = 0;
		c = *s++;
		if (c == '+') c = *s++;
		else if (c == '-') c = *s++, esign = -1;
		
		while ((c = *s++), isdigit(c)) {
			i = i*10 + (c - '0');
		}

		e += i*esign;
	}

	float mul = 1;

	while (e > 0) {
		mul *= 10.0f;
		e--;
	}
	while (e < 0) {
		mul *= 0.1f;
		e++;
	}

	*ptr = s-1;

	return (float)sign * (float)a * mul;
}

int read_int(char** ptr) {
	char* s = *ptr;

	while(*s == ' ') s++;
	if (!(isdigit(*s) || *s == '-')) {
		*ptr = s;
		return 0;
	}


	int a = 0;
	int sign = 1;
	char c;

	if (*s == '-') s++, sign = -1;
	while ((c = *s++), isdigit(c)) {
		a = a*10 + (c - '0');
	}

	*ptr = s-1;
	
	return sign * a;
}