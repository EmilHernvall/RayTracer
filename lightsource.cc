#include "lightsource.h"

LightSource::LightSource()
    : m_location(vec3()),
      m_radius(0.0),
      m_color(Color())
{
}

LightSource::LightSource(const vec3& location, double radius, const Color& color)
    : m_location(location),
      m_radius(radius),
      m_color(color)
{
}
