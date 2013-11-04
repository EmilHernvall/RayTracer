#include <limits>
#include <cmath>
#include <iostream>

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

Surface::~Surface()
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

Planet::Planet(const vec3& location,
               int radius,
               const Material& material,
               gdImage* map,
               gdImage* ambient,
               gdImage* specular,
               double theta0)
    : m_location(location),
      m_radius(radius),
      m_material(material),
      m_ambient(ambient),
      m_specular(specular),
      m_img(map),
      m_theta0(theta0)
{
}

Planet::~Planet()
{
}

bool Planet::intersect(const vec3& origin,
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
            vec3 hit = origin + result.time() * ray;
            vec3 pos = hit - m_location;
            double theta = acos(pos.y() / m_radius);
            double phi = atan2(pos.z(), pos.x());

            phi = phi + m_theta0;
            if (phi > M_PI) {
                phi = phi - 2*M_PI;
            }
            else if (phi < -M_PI) {
                phi = phi + 2*M_PI;
            }

            int mapWidth = gdImageSX(m_img);
            int mapHeight = gdImageSY(m_img);

            int mapX = (int)(mapWidth/2.0 - phi*mapWidth/(2.0*M_PI));
            int mapY = (int)(theta*mapHeight/(M_PI));

            //cout << "theta=" << (theta*180.0/M_PI) << " "
            //     << "phi=" << (phi*180.0/M_PI) << " "
            //     << "x=" << mapX << " "
            //     << "y=" << mapY << endl;

            {
                int color = gdImageGetPixel(m_img, mapX, mapY);
                double r = ((color >> 16) & 0xFF) / (double)0xFF;
                double g = ((color >> 8) & 0xFF) / (double)0xFF;
                double b = (color & 0xFF) / (double)0xFF;

                m_material.diffuseColor(Color(r, g, b));
                m_material.highlightColor(Color(r, g, b));
            }

            if (m_ambient != NULL) {
                int color = gdImageGetPixel(m_ambient, mapX, mapY);
                double r = ((color >> 16) & 0xFF) / (double)0xFF;
                double g = ((color >> 8) & 0xFF) / (double)0xFF;
                double b = (color & 0xFF) / (double)0xFF;

                m_material.ambientColor(Color(r, g, b));
            } else {
                m_material.ambientColor(m_material.diffuseColor());
            }

            if (m_specular != NULL) {
                int color = gdImageGetPixel(m_specular, mapX, mapY);
                double c = (color & 0xFF) / (double)0xFF;
                m_material.specularWeight(c);
                m_material.shininess(10.0);
            } else {
                m_material.specularWeight(0.0);
                m_material.shininess(0.0);
            }

            result.hit(hit);
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

Triangle::Triangle(const vec3& location,
                 const vec3& a,
                 const vec3& b,
                 const Material& material)
    : m_location(location),
      m_a(a),
      m_b(b),
      m_material(material)
{
    m_normal = a.cross(b).normalize();
}

bool Triangle::intersect(const vec3& origin,
                        const vec3& ray,
                        double maxTime,
                        Intersection& result)
{
    if (ray.dot(m_normal) < EPSILON) {
        return false;
    }

    double t = (m_location - origin).dot(m_normal) / ray.dot(m_normal);
    if (t > maxTime) {
        return false;
    }

    vec3 p = origin + t * ray;
    vec3 c = p - m_location;

    // http://www.blackpawn.com/texts/pointinpoly/
    double dotaa = m_a.dot(m_a);
    double dotab = m_a.dot(m_b);
    double dotac = m_a.dot(c);
    double dotbb = m_b.dot(m_b);
    double dotbc = m_b.dot(c);

    double invDenom = 1.0 / (dotaa * dotbb - dotab * dotab);
    double u = (dotbb * dotac - dotab * dotbc) * invDenom;
    double v = (dotaa * dotbc - dotab * dotac) * invDenom;

    if (u >= 0.0 && v >= 0.0 && u + v < 1.0) {
        result.initialized(true);
        result.time(t);
        result.hit(p);
        result.normal(m_normal);
        result.material(m_material);

        return true;
    }

    return false;
}
