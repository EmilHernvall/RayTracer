#include <cmath>
#include <string>
#include <sstream>
#include "vec3.h"

vec3::vec3()
    : m_x(0.0),
      m_y(0.0),
      m_z(0.0)
{
}

vec3::vec3(const vec3& vec)
    : m_x(vec.x()),
      m_y(vec.y()),
      m_z(vec.z())
{
}

vec3::vec3(double x, double y, double z)
    : m_x(x),
      m_y(y),
      m_z(z)
{
}

double vec3::abs() const
{
    return sqrt(abs2());
}

double vec3::abs2() const
{
    return m_x*m_x + m_y*m_y + m_z*m_z;
}

vec3 vec3::normalize() const
{
    return *this / abs();
}

std::string vec3::repr() const
{
    std::stringstream s;
    s << "{ x=" << m_x << " y=" << m_y << " z=" << m_z << " }";
    return s.str();
}

vec3 vec3::mul(const vec3& vec) const
{
    return vec3(m_x * vec.x(),
                m_y * vec.y(),
                m_z * vec.z());
}

double vec3::dot(const vec3& vec) const
{
    return m_x*vec.x() + m_y*vec.y() + m_z*vec.z();
}

vec3 vec3::cross(const vec3& vec) const
{
    return vec3(m_y * vec.z() - m_z * vec.y(),
                m_z * vec.x() - m_x * vec.z(),
                m_x * vec.y() - m_y * vec.x());
}

vec3& vec3::operator=(const vec3& vec)
{
    m_x = vec.x();
    m_y = vec.y();
    m_z = vec.z();
    return *this;
}

vec3& vec3::operator+=(const vec3& vec)
{
    m_x += vec.x();
    m_y += vec.y();
    m_z += vec.z();
    return *this;
}

vec3& vec3::operator-=(const vec3& vec)
{
    m_x -= vec.x();
    m_y -= vec.y();
    m_z -= vec.z();
    return *this;
}

vec3& vec3::operator*=(double a)
{
    m_x = a*m_x;
    m_y = a*m_y;
    m_z = a*m_z;
    return *this;
}

vec3& vec3::operator/=(double a)
{
    m_x = m_x/a;
    m_y = m_y/a;
    m_z = m_z/a;
    return *this;
}

const vec3 operator-(const vec3& a)
{
    return vec3(-a.x(), -a.y(), -a.z());
}

const vec3 operator+(const vec3& a, const vec3& b)
{
    return vec3(a.x() + b.x(), a.y() + b.y(), a.z() + b.z());
}

const vec3 operator-(const vec3& a, const vec3& b)
{
    return vec3(a.x() - b.x(), a.y() - b.y(), a.z() - b.z());
}

const vec3 operator*(const vec3& a, double k)
{
    return vec3(k*a.x(), k*a.y(), k*a.z());
}

const vec3 operator*(double k, const vec3& a)
{
    return vec3(k*a.x(), k*a.y(), k*a.z());
}

const vec3 operator/(const vec3& a, double k)
{
    return vec3(a.x()/k, a.y()/k, a.z()/k);
}
