#ifndef _DRAWABLE_DRAWABLE_H
#define _DRAWABLE_DRAWABLE_H

#include <string>
class Drawable {

public:
	Drawable(std::string name) : m_name{name} {}
	std::string m_name;
	bool m_enable = true;

	virtual void Draw() = 0;
	virtual void Initialize() = 0;

};


class PointCloudDrawable {

};

#endif