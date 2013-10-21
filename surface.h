#ifndef __SURFACE_H_
#define __SURFACE_H_

#include "vec3.h"
#include "material.h"

class Intersection {
public:
    Intersection();

    inline bool initialized() const { return m_initialized; }
    inline void initialized(bool initialized) { m_initialized = initialized; }

    inline double time() const { return m_time; }
    inline void time(double time) { m_time = time; }

    inline const vec3& hit() const { return m_hit; }
    inline void hit(const vec3& hit) { m_hit = hit; }

    inline const vec3& normal() const { return m_normal; }
    inline void normal(const vec3& normal) { m_normal = normal; }

    inline const Material& material() const { return m_material; }
    inline void material(const Material& material) { m_material = material; }

private:
    bool m_initialized;
    double m_time;
    vec3 m_hit;
    vec3 m_normal;
    Material m_material;
};

class Surface {
public:
    virtual bool intersect(const vec3& origin,
                           const vec3& ray,
                           double maxTime,
                           Intersection& result) = 0;
};

class Sphere : public Surface {
public:
    Sphere(const vec3&, int, const Material&);

    virtual bool intersect(const vec3& origin,
                           const vec3& ray,
                           double maxTime,
                           Intersection& result);
private:
    vec3 m_location;
    int m_radius;
    Material m_material;
};

class Plane : public Surface {
public:
    Plane(const vec3&, const vec3&, const Material&);

    virtual bool intersect(const vec3& origin,
                           const vec3& ray,
                           double maxTime,
                           Intersection& result);
private:
    vec3 m_normal;
    vec3 m_point;
    Material m_material;
};

#endif // __SURFACE_H_