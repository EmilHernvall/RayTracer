#ifndef __MATERIAL_H_
#define __MATERIAL_H_

#include "vec3.h"
#include "color.h"

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
    inline void diffuseColor(const Color& v) { m_diffuseColor = v; }

    inline const Color& highlightColor() const { return m_highlightColor; }
    inline void highlightColor(const Color& v) { m_highlightColor = v; }

    inline const Color& reflectionColor() const { return m_reflectionColor; }
    inline void reflectionColor(const Color& v) { m_reflectionColor = v; }

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

Material createMetal(const Color& color);
Material createPolishedMetal(const Color& color);
Material createPlastic(const Color& color);
Material createMatte(const Color& color);

#endif // __MATERIAL_H_
