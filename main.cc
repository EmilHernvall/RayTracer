#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <limits>
#include <gd.h>
#include <cstdio>

#include "raytracer.h"
#include "vec3.h"
#include "random.h"
#include "color.h"
#include "material.h"
#include "lightsource.h"
#include "surface.h"

using namespace std;

int main(int argc, const char* argv[])
{
    // source of randomness
    RandomDoubles random(time(NULL), 70001);
    // distance from eye to screen, in the direction towards looking_at
    double DISTANCE_TO_SCREEN = 100.0;
    // output size
    int IMAGE_WIDTH = 1920, IMAGE_HEIGHT = 1080;
    // virtual screen size
    double SCREEN_WIDTH = 100.0,
           SCREEN_HEIGHT = (IMAGE_HEIGHT * SCREEN_WIDTH) / IMAGE_WIDTH;
    // number of samples per pixel
    int SAMPLES = 9;
    double INVERSE_SAMPLES = 1.0/SAMPLES,
           SQRT_SAMPLES = sqrt(SAMPLES),
           INVERSE_SQRT_SAMPLES = 1.0/SQRT_SAMPLES;
    // max number of reflection steps
    int MAX_REFLECTION_STEPS = 10;
    double MIN_COLOR_INTENSITY = 1.0 / 256.0;
    // color of ambient light
    Color AMBIENT_COLOR(1.0, 1.0, 1.0);
    double RADIANCE_SCALE = 1.0;

    // setup target image
    gdImage* img = gdImageCreateTrueColor(IMAGE_WIDTH, IMAGE_HEIGHT);

    // define scene
    Material MIRROR = createPolishedMetal(Color(0.90, 0.90, 0.90));
    Material RED_METAL = createMetal(Color(1.0, 0.0, 0.0));
    Material GREEN_PLASTIC = createPlastic(Color(0.0, 1.0, 0.0));
    Material GREEN_METAL = createMetal(Color(0.0, 1.0, 0.0));
    Material BLUE_PLASTIC = createPlastic(Color(0.0, 0.0, 1.0));
    Material BLUE_METAL = createMetal(Color(0.0, 0.0, 1.0));
    Material YELLOW_MATTE = createMatte(Color(1.0, 1.0, 0.0));
    Material BLUE_MATTE = createMatte(Color(0.1, 0.1, 0.7));

    vector<LightSource> lights;
    lights.push_back(LightSource(vec3(500.0, 0.0, 500.0),
                                 10.0,
                                 Color(1.0, 1.0, 0.5)));
    vector<Surface*> surfaces;
    surfaces.push_back(new Planet(vec3(0.0, 0.0, 0.0),
                                  100.0,
                                  BLUE_MATTE,
                                  "map.png"));

    // position of eye
    double theta = M_PI - 5.0 * M_PI / 5.0;
    vec3 eye(400*cos(theta), 100.0, 400*sin(theta));

    // where the eye is looking
    vec3 looking_at(0.0, 0.0, 0);

    // construct a basis at the screens center
    vec3 w = (eye - looking_at).normalize();
    vec3 u = vec3(0.0, 1.0, 0.0).cross(w);
    vec3 v = w.cross(u);

    // center of screen
    vec3 c = eye - DISTANCE_TO_SCREEN * w;

    for (int x = 0; x < IMAGE_WIDTH; x++) {
        cout << x << " out of " << IMAGE_WIDTH << " done." << endl;

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

                    // pew, pew, pew
                    vec3 d = (p - eye).normalize();

                    vec3 f(RADIANCE_SCALE, RADIANCE_SCALE, RADIANCE_SCALE);
                    vec3 o = eye;
                    for (int k = 0; k < MAX_REFLECTION_STEPS; k++) {

                        // find the object closest to the eye
                        Intersection intersection;
                        Intersection bestIntersection;
                        for (vector<Surface*>::iterator surface = surfaces.begin();
                             surface != surfaces.end();
                             surface++) {

                            if (!(*surface)->intersect(o,
                                                       d,
                                                       numeric_limits<double>::infinity(),
                                                       intersection)) {
                                continue;
                            }

                            if (intersection.time() < bestIntersection.time()) {
                                bestIntersection = intersection;
                            }
                        }

                        // nothing more to do if we didn't get an intersection
                        if (!bestIntersection.initialized()) {
                            break;
                        }

                        //pixel += Color(1.0, 1.0, 1.0);

                        const Material& material = bestIntersection.material();
                        if (material.ambientWeight() > 0.0) {
                            pixel += material.ambientWeight()
                                   * f.mul(AMBIENT_COLOR.mul(material.diffuseColor()));
                        }

                        for (vector<LightSource>::iterator light = lights.begin();
                             light != lights.end();
                             light++) {

                            if (d.dot(bestIntersection.normal()) >= 0) {
                                // negate the direction of the normal
                                bestIntersection.normal(-bestIntersection.normal());
                            }

                            double r1 = (random.next() - 0.5) * light->radius(),
                                   r2 = (random.next() - 0.5) * light->radius(),
                                   r3 = (random.next() - 0.5) * light->radius();
                            const vec3& lightLoc = light->location()
                                                 + vec3(r1, r2, r3);

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
                            for (vector<Surface*>::iterator surface = surfaces.begin();
                                 surface != surfaces.end();
                                 surface++) {

                                // a single object inbetween the current object,
                                // and the light source is enough to shadow it
                                if ((*surface)->intersect(bestIntersection.hit(),
                                                          l,
                                                          maxTime,
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
                                pixel += material.diffuseWeight()
                                       * nDotl
                                       * f.mul(lightColor.mul(material.diffuseColor()));
                            }

                            // specular
                            if (material.specularWeight() > 0) {
                                vec3 r = 2.0 * nDotl * bestIntersection.normal() - l;
                                double rDotMd = -r.dot(d);
                                if (rDotMd > 0) {
                                    pixel += pow(rDotMd, material.shininess())
                                           * material.specularWeight()
                                           * nDotl
                                           * f.mul(lightColor.mul(material.highlightColor()));
                                }
                            }
                        }

                        // reflection
                        if (material.reflectionWeight() > 0) {
                            f = material.reflectionWeight()
                              * f.mul(material.reflectionColor());
                            if (f.x() < MIN_COLOR_INTENSITY &&
                                f.y() < MIN_COLOR_INTENSITY &&
                                f.z() < MIN_COLOR_INTENSITY) {

                                break;
                            }

                            const vec3& n = bestIntersection.normal();
                            d = d - (2.0*d.dot(n)) * n;
                            o = bestIntersection.hit();
                        } else {
                            break;
                        }
                    }
                }
            }

            // set pixel to color
            pixel /= SAMPLES * lights.size();
            gdImageSetPixel(img, x, y, pixel.rgb());
        }
    }

    FILE* outFh = fopen("test.png", "w");
    gdImagePng(img, outFh);
    gdImageDestroy(img);

    return 0;
}
