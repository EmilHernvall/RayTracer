#ifndef __LIGHTSOURCE_H_
#define __LIGHTSOURCE_H_

#include "vec3.h"
#include "color.h"

class LightSource {
public:
    LightSource();
    LightSource(const vec3&, double, const Color&);

    inline const vec3& location() const { return m_location; }
    inline double radius() const { return m_radius; }
    inline const Color& color() const { return m_color; }

private:
    vec3 m_location;
    double m_radius;
    Color m_color;
};

#endif // __LIGHTSOURCE_H_
