#ifndef __RAYTRACER_H
#define __RAYTRACER_H

#include "vec3.h"

#define EPSILON 0.000001

class RandomDoubles {
public:
    RandomDoubles(int seed, int count);
    ~RandomDoubles();

    double next();
private:
    int m_count;
    int m_index;
    double* m_values;
};

class Color : public vec3 {
public:
    Color();
    Color(int r, int g, int b);

    inline int r() const { return x(); }
    inline int g() const { return y(); }
    inline int b() const { return z(); }

    inline int rgb() const {
        double inv_gamma = 1.0/2.2;
        int r = 0xFF * pow(x(), inv_gamma);
        if (r > 0xFF) { r = 0xFF; }

        int g = 0xFF * pow(y(), inv_gamma);
        if (g > 0xFF) { g = 0xFF; }

        int b = 0xFF * pow(z(), inv_gamma);
        if (b > 0xFF) { b = 0xFF; }

        return ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
    }
};

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

class Material {
public:
    Material();

    Material(double,
             double,
             double,
             double,
             double,
             const Color&,
             const Color&,
             const Color&);

    inline double ambientWeight() const { return m_ambientWeight; }
    inline double diffuseWeight() const { return m_diffuseWeight; }
    inline double specularWeight() const { return m_specularWeight; }
    inline double reflectionWeight() const { return m_reflectionWeight; }
    inline double shininess() const { return m_shininess; }

    inline const Color& diffuseColor() const { return m_diffuseColor; }
    inline const Color& highlightColor() const { return m_highlightColor; }
    inline const Color& reflectionColor() const { return m_reflectionColor; }

private:
    double m_ambientWeight;
    double m_diffuseWeight;
    double m_specularWeight;
    double m_reflectionWeight;
    double m_shininess;
    Color m_diffuseColor;
    Color m_highlightColor;
    Color m_reflectionColor;
};

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

#endif // __RAYTRACER_H
