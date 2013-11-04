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
    int IMAGE_WIDTH = 2560, IMAGE_HEIGHT = 1440;
    // virtual screen size
    double SCREEN_WIDTH = 100.0,
           SCREEN_HEIGHT = (IMAGE_HEIGHT * SCREEN_WIDTH) / IMAGE_WIDTH;
    // number of samples per pixel
    int SAMPLES = 100;
    double INVERSE_SAMPLES = 1.0/SAMPLES,
           SQRT_SAMPLES = sqrt(SAMPLES),
           INVERSE_SQRT_SAMPLES = 1.0/SQRT_SAMPLES;
    // max number of reflection steps
    int MAX_REFLECTION_STEPS = 10;
    double MIN_COLOR_INTENSITY = 1.0 / 256.0;
    // color of ambient light
    Color AMBIENT_COLOR(1.0, 1.0, 1.0);
    double RADIANCE_SCALE = 1.0;

    FILE* fh = fopen("earth_10k.png", "r");
    gdImage* map = gdImageCreateFromPng(fh);
    fclose(fh);

    //FILE* fh2 = fopen("moon_4k.png", "r");
    //gdImage* moon = gdImageCreateFromPng(fh2);
    //fclose(fh2);

    FILE* fh3 = fopen("earthlights_10k.png", "r");
    gdImage* earthLights = gdImageCreateFromPng(fh3);
    fclose(fh3);

    FILE* fh4 = fopen("earthspec_10k.png", "r");
    gdImage* earthSpec = gdImageCreateFromPng(fh4);
    fclose(fh4);

    int num_frames = 1;
    for (int nr = 0; nr < num_frames; nr++) {

    cout << "rendering frame " << nr << endl;

    // setup target image
    gdImage* img = gdImageCreateTrueColor(IMAGE_WIDTH, IMAGE_HEIGHT);
    gdImageFill(img, 0, 0, 0);

    // define scene
    Material MIRROR = createPolishedMetal(Color(0.90, 0.90, 0.90));
    Material RED_METAL = createMetal(Color(1.0, 0.0, 0.0));
    Material GREEN_PLASTIC = createPlastic(Color(0.0, 1.0, 0.0));
    Material GREEN_METAL = createMetal(Color(0.0, 1.0, 0.0));
    Material BLUE_PLASTIC = createPlastic(Color(0.0, 0.0, 1.0));
    Material BLUE_METAL = createMetal(Color(0.0, 0.0, 1.0));
    Material YELLOW_MATTE = createMatte(Color(1.0, 1.0, 0.0));
    Material BLUE_MATTE = createMatte(Color(0.1, 0.1, 0.7));
    Material WHITE_MATTE = createMatte(Color(1.0, 1.0, 1.0));

    vector<LightSource> lights;

    //double rot_theta = (nr % 40)/40.0 * 2.0 * M_PI;
    double rot_theta = -15*M_PI/20.0;

    double distance_to_earth = 1e10;
    double plane_diff = distance_to_earth*sin(23.439*M_PI/180.0);
    lights.push_back(LightSource(vec3(distance_to_earth, plane_diff, 0.0),
                                 10000.0,
                                 Color(1.0, 1.0, 0.5)));
    vector<Surface*> surfaces;
    surfaces.push_back(new Planet(vec3(0.0, 0.0, 0.0),
                                  100.0,
                                  BLUE_MATTE,
                                  map,
                                  earthLights,
                                  earthSpec,
                                  rot_theta));
    //surfaces.push_back(new Planet(vec3(-200.0, 0.0, -200.0),
    //                              25.0,
    //                              WHITE_MATTE,
    //                              moon,
    //                              NULL,
    //                              NULL,
    //                              0.0));

    // position of eye
    //double eye_theta = (nr % 100)/100.0 * 2.0 * M_PI;
    double eye_theta = 13*M_PI/20.0;
    double distance = 300.0;
    vec3 eye(distance*cos(eye_theta), 300.0, distance*sin(eye_theta));

    // where the eye is looking
    vec3 looking_at(0.0, 0.0, 0);

    // construct a basis at the screens center
    vec3 w = (eye - looking_at).normalize();
    vec3 u = vec3(0.0, 1.0, 0.0).cross(w);
    vec3 v = w.cross(u);

    // center of screen
    vec3 c = eye - DISTANCE_TO_SCREEN * w;

    for (int x = 0; x < IMAGE_WIDTH; x++) {
        if (num_frames == 1) {
            cout << x << " out of " << IMAGE_WIDTH << " done." << endl;
        }

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
                                   * f.mul(AMBIENT_COLOR.mul(material.ambientColor()));
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

    char out_name[256];
    sprintf(out_name, "earth/earth%d.png", nr);

    cout << "Saving " << out_name << endl;

    FILE* outFh = fopen(out_name, "w");
    gdImagePng(img, outFh);
    gdImageDestroy(img);
    fclose(outFh);

    for (vector<Surface*>::iterator surface = surfaces.begin();
         surface != surfaces.end();
         surface++) {

        delete *surface;
    }

    }

    return 0;
}
