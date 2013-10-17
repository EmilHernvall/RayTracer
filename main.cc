#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <limits>

#include "raytracer.h"
#include "vec3.h"

using namespace std;

RandomDoubles::RandomDoubles(int seed, int count)
    : m_count(count),
      m_index(0),
      m_values(new double[count])
{
    srand(seed);
    for (int i = 0; i < count; i++) {
        m_values[i] = rand() / (double)RAND_MAX;
    }

    m_index = (int)(count * (rand() / (double)RAND_MAX));
}

double RandomDoubles::next()
{
    if (m_index >= m_count) {
        m_index = 0;
    }

    return m_values[m_index++];
}

RandomDoubles::~RandomDoubles()
{
    delete[] m_values;
}

Color::Color()
    : vec3()
{
}

Color::Color(int r, int g, int b)
    : vec3(r, g, b)
{
}

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

Intersection::Intersection()
    : m_initialized(false),
      m_time(numeric_limits<double>::infinity()),
      m_hit(vec3()),
      m_normal(vec3())
{
}

int main(int argc, const char* argv[])
{
    vector<LightSource> lights;
    vector<Surface> surfaces;
    // source of randomness
    RandomDoubles random(time(NULL), 70001);
    // position of eye
    vec3 eye(-100.0, 0.0, 0.0);
    // where the eye is looking
    vec3 looking_at(0.0, 0.0, 0.0);
    // distance from eye to screen, in the direction towards looking_at
    double DISTANCE_TO_SCREEN = 50.0;
    // output size
    int IMAGE_WIDTH = 1280, IMAGE_HEIGHT = 720;
    // virtual screen size
    double SCREEN_WIDTH = 100.0,
           SCREEN_HEIGHT = (IMAGE_HEIGHT * SCREEN_WIDTH) / IMAGE_WIDTH;
    // number of samples per pixel
    int SAMPLES = 25;
    double SQRT_SAMPLES = sqrt(SAMPLES), INVERSE_SQRT_SAMPLES = 1.0/SQRT_SAMPLES;
    // color of ambient light
    Color AMBIENT_COLOR;
    double RADIANCE_SCALE = 1.0;

    // construct a basis at the screens center
    vec3 w = (eye - looking_at).normalize();
    vec3 u = vec3(0.0, 1.0, 0.0).cross(w);
    vec3 v = w.cross(u);

    // center of screen
    vec3 c = eye - DISTANCE_TO_SCREEN * w;

    for (int x = 0; x < IMAGE_WIDTH; x++) {
        for (int y = 0; y < IMAGE_HEIGHT; y++) {

            Color pixel;

            // introduce some randomness
            for (int i = 0; i < SQRT_SAMPLES; i++) {
                double b = y + INVERSE_SQRT_SAMPLES * i;
                if (SAMPLES == 1) {
                    b += 0.5;
                } else {
                    b += random.next();
                }

                b = SCREEN_WIDTH / IMAGE_WIDTH * (IMAGE_HEIGHT / 2.0 - b);

                for (int j = 0; j < SQRT_SAMPLES; j++) {
                    double a = x + INVERSE_SQRT_SAMPLES * j;
                    if (SAMPLES == 1) {
                        a += 0.5;
                    } else {
                        a += random.next();
                    }

                    a = SCREEN_WIDTH / IMAGE_WIDTH * (a - IMAGE_WIDTH / 2.0);

                    // point on the virtual screen
                    vec3 p = c + (a * u) + (b * v);

                    // direction of ray
                    vec3 d = (p - eye).normalize();

                    // find the object closest to the eye
                    Intersection intersection;
                    Intersection bestIntersection;
                    for (vector<Surface>::iterator surface = surfaces.begin();
                         surface != surfaces.end();
                         surface++) {

                        if (!surface->intersect(eye, d, intersection)) {
                            continue;
                        }

                        if (intersection.time() < bestIntersection.time()) {
                            intersection = bestIntersection;
                        }
                    }

                    // nothing more to do if we didn't get an intersection
                    if (!bestIntersection.initialized()) {
                        continue;
                    }

                    const Material& material = bestIntersection.material();
                    if (material.ambientWeight() > 0.0) {
                        pixel += RADIANCE_SCALE
                               * material.ambientWeight()
                               * AMBIENT_COLOR.mul(material.diffuseColor());
                    }

                    for (vector<LightSource>::iterator light = lights.begin();
                         light != lights.end();
                         light++) {

                        if (d.dot(bestIntersection.normal()) >= 0) {
                            // negate the direction of the normal
                            bestIntersection.normal(-bestIntersection.normal());
                        }

                        const vec3& lightLoc = light->location();
                        const Color& lightColor = light->color();
                        vec3 l = (lightLoc - bestIntersection.hit()).normalize();

                        double nDotl = l.dot(bestIntersection.normal());
                        if (nDotl <= 0) {
                            continue;
                        }

                        double maxTime = (lightLoc - bestIntersection.hit()).abs();

                        // do a second pass across all objects in the scene,
                        // and check if they shadow the object
                        bool illuminated = true;
                        Intersection lightIntersection;
                        for (vector<Surface>::iterator surface = surfaces.begin();
                             surface != surfaces.end();
                             surface++) {

                            // a single object inbetween the current object,
                            // and the light source is enough to shadow it
                            if (surface->intersect(bestIntersection.hit(),
                                                   lightLoc,
                                                   lightIntersection)) {

                                illuminated = false;
                                break;
                            }
                        }

                        if (!illuminated) {
                            continue;
                        }

                        // diffuse
                        if (material.diffuseWeight() > 0) {
                            pixel += RADIANCE_SCALE
                                   * 1.0 / M_PI
                                   * material.diffuseWeight()
                                   * nDotl
                                   * lightColor.mul(material.diffuseColor());
                        }

                        // specular
                        if (material.specularWeight() > 0) {
                            vec3 r = 2.0 * nDotl * bestIntersection.normal() - l;
                            double rDotMd = r.dot(d);
                            if (rDotMd > 0) {
                                pixel += RADIANCE_SCALE
                                       * pow(rDotMd, material.shininess())
                                       * material.specularWeight()
                                       * nDotl
                                       * lightColor.mul(material.highlightColor());
                            }
                        }

                        // reflection
                    }
                }
            }

            // set pixel to color
        }
    }

    return 0;
}