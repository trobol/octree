#ifndef _MAT4_H
#define _MAT4_H

class mat4 {
public:
	float data[16];
	operator float* () {
		return data;
	}
};


#endif