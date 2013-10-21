#include "material.h"

Material::Material()
    : m_ambientWeight(0.0),
      m_diffuseWeight(0.0),
      m_specularWeight(0.0),
      m_reflectionWeight(0.0),
      m_shininess(0.0),
      m_diffuseColor(Color()),
      m_highlightColor(Color()),
      m_reflectionColor(Color())
{
}

Material::Material(double ambientWeight,
                   double diffuseWeight,
                   double specularWeight,
                   double reflectionWeight,
                   double shininess,
                   const Color& diffuseColor,
                   const Color& highlightColor,
                   const Color& reflectionColor)
    : m_ambientWeight(ambientWeight),
      m_diffuseWeight(diffuseWeight),
      m_specularWeight(specularWeight),
      m_reflectionWeight(reflectionWeight),
      m_shininess(shininess),
      m_diffuseColor(diffuseColor),
      m_highlightColor(highlightColor),
      m_reflectionColor(reflectionColor)
{
}

Material createMetal(const Color& color)
{
    return Material(0.1,
                    0.7,
                    0.3,
                    0.9,
                    3.0,
                    color,
                    Color(1.0, 1.0, 1.0),
                    color);
}

Material createPolishedMetal(const Color& color)
{
    return Material(0.0,
                    0.0,
                    0.5,
                    1.0,
                    1.0,
                    color,
                    color,
                    color);
}

Material createPlastic(const Color& color)
{
    return Material(0.1,
                    2.0,
                    1.0,
                    0.0,
                    10.0,
                    color,
                    Color(1.0, 1.0, 1.0),
                    Color(0.0, 0.0, 0.0));
}

Material createMatte(const Color& color)
{
    return Material(0.1,
                    1.0,
                    0.0,
                    0.0,
                    0.0,
                    color,
                    Color(0.0, 0.0, 0.0),
                    Color(0.0, 0.0, 0.0));

}

