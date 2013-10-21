#ifndef __VEC3_H
#define __VEC3_H

#include <string>

class vec3 {

// constructors
public:
    vec3();
    vec3(const vec3&);
    vec3(double, double, double);

// methods
public:

    inline double x() const { return m_x; }
    inline double y() const { return m_y; }
    inline double z() const { return m_z; }

    // represent this vector as a string
    std::string repr() const;

    // calculate the absolute value of this vector
    double abs() const;
    double abs2() const;

    // return a unit vector
    vec3 normalize() const;

    // elementwise multiplication
    vec3 mul(const vec3&) const;

    // the dot product of two vectors
    double dot(const vec3&) const;

    // the cross product of two vectors
    vec3 cross(const vec3&) const;

// overloaded operators
public:
    vec3& operator=(const vec3&);
    vec3& operator+=(const vec3&);
    vec3& operator-=(const vec3&);
    vec3& operator*=(double a);
    vec3& operator/=(double a);

// private members
private:
    double m_x;
    double m_y;
    double m_z;
};

const vec3 operator-(const vec3&);
const vec3 operator+(const vec3&, const vec3&);
const vec3 operator-(const vec3&, const vec3&);
const vec3 operator*(const vec3&, double);
const vec3 operator*(double, const vec3&);
const vec3 operator/(const vec3&, double);

#endif // __VEC3_H
