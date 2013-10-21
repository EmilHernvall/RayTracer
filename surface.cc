#include <limits>

#include "raytracer.h"
#include "surface.h"

using namespace std;

Intersection::Intersection()
    : m_initialized(false),
      m_time(numeric_limits<double>::infinity()),
      m_hit(vec3()),
      m_normal(vec3())
{
}

Sphere::Sphere(const vec3& location, int radius, const Material& material)
    : m_location(location),
      m_radius(radius),
      m_material(material)
{
}

bool Sphere::intersect(const vec3& origin,
                       const vec3& ray,
                       double maxTime,
                       Intersection& result)
{
    vec3 l = origin - m_location;
    double B = 2.0 * ray.dot(l);
    double C = pow(l.abs(),2) - m_radius * m_radius;
    double square = B * B  - 4 * C;
    if (square >= 0) {
        double root = sqrt(square);
        double t1 = 0.5 * (-B - root);
        double t2 = 0.5 * (-B + root);

        result.initialized(false);
        if (t1 >= EPSILON && t1 <= maxTime) {
            result.initialized(true);
            result.time(t1);
        }
        else if (t2 >= EPSILON && t2 < maxTime) {
            result.initialized(true);
            result.time(t2);
        }

        if (result.initialized()) {
            result.hit(origin + result.time() * ray);
            result.normal((result.hit() - m_location).normalize());
            result.material(m_material);
            return true;
        }
    }

    return false;
}

Plane::Plane(const vec3& normal, const vec3& point, const Material& material)
    : m_normal(normal),
      m_point(point),
      m_material(material)
{
}

bool Plane::intersect(const vec3& origin,
                       const vec3& ray,
                       double maxTime,
                       Intersection& result)
{
    if (ray.dot(m_normal) < EPSILON) {
        return false;
    }

    double t = (m_point - origin).dot(m_normal) / ray.dot(m_normal);
    if (t > maxTime) {
        return false;
    }

    result.initialized(true);
    result.time(t);
    result.hit(origin + t * ray);
    result.normal(m_normal);
    result.material(m_material);

    return true;
}

