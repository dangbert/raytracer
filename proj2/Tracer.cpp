#include <iostream>
#include "Tracer.h"
#include <assert.h>
#include <math.h>
using namespace std;

////////////////////////////
//////// Ray class: ////////
////////////////////////////
/*
 * print out Ray object for debugging
 */
std::ostream &operator<<(std::ostream &sout, const Ray &ray) {
    sout << "Ray: ";
    sout << "(" << ray.eye[0] << "," << ray.eye[1] << "," << ray.eye[2] << ") -> ";
    sout << "(" << ray.dir[0] << "," << ray.dir[1] << "," << ray.dir[2] << ")";
    return sout;
}


////////////////////////////
///// HitRecord class: /////
////////////////////////////
/*
 * print out HitRecord object for debugging
 */
std::ostream &operator<<(std::ostream &sout, const HitRecord &hit) {
    sout << "HitRecord: ";
    sout << "sType=" << static_cast<std::underlying_type<SurfaceType>::type>(hit.sType) << ", t=" << hit.t << ", dist=" << hit.dist;
    if (hit.t != -1) {
        sout << ", point=" << "(" << hit.point[0] << "," << hit.point[1] << "," << hit.point[2] << ")";
        if (hit.sType == SurfaceType::TRIANGLE)
            sout << ", B=" << hit.B << ", " << "g=" << hit.g << ", triIndex=" << hit.triIndex;
    }
    sout << endl;
    return sout;
}


////////////////////////////
///// RayTracer class: /////
////////////////////////////
/**
 * constructs a RayTracer object
 * filename:  name of nff file to use
 */
RayTracer::RayTracer(std::string filename) {
    if (nff.readFile(filename) != 0) {
        cout << "error parsing " << filename << endl;
        exit(1);
    }
    cout << nff << endl;
}

/**
 * renders the nff file for this object and writes the image to a file
 * filename:  name for output file (should end in .ppm)
 * debug:     whether or not to print out debug info for the render proces
 *
 * for this project, FOV is the whole image
 *   (including the edge of pixels on each side rather than their centers)
 *   and pixels are square
 */
void RayTracer::render(std::string filename, int bounces, bool debug) {
    /*
     * NOTE:
     * world coordinates:  3D coordinate system (which surfaces are defined in respect to)
     * camera coordinates: 3D coordinate system where:
     *                     origin is the focal point (v_from)
     *                     center of virtual image plane is (0,0,d)
     * image coordinates:  (i,j) discrete location of pixel in image
     */
    cout << "\nrendering..." << endl;
    if (debug) {
        cout << nff << endl;
    }
    // vectors defining our world coordinates (origin is at nff.v_from)
    Vector3d w = (nff.v_from - nff.v_at).normalized();
    Vector3d u = nff.v_up.cross(w).normalized();
    Vector3d v = w.cross(u).normalized();

    // distance from origin to center of virtual image plane
    double d = (nff.v_at - nff.v_from).norm();
    // width (and height) of a pixel on the virtual image plane
    //     deltaX = real world width of plane / x resolution
    double deltaX = 2 * (d * tan(nff.v_angle/2 * M_PI/180)) / nff.v_resolution[0];

    // position (in camera coordinates) of CENTER of top left corner pixel
    // locations are offset from the center of the image (0,0,0) (in camera coordinates)
    //                 deltaX * (num pixels to move)
    double initX = 0 - deltaX * ((double) nff.v_resolution[0]/2 - (double) 1/2);
    double initY = 0 + deltaX * ((double) nff.v_resolution[1]/2 - (double) 1/2);

    if (debug) {
        printf("w: (%g, %g, %g)\n", w[0], w[1], w[2]);
        printf("u: (%g, %g, %g)\n", u[0], u[1], u[2]);
        printf("v: (%g, %g, %g)\n", v[0], v[1], v[2]);
        cout << "d: " << d << endl;
        cout << "deltaX: " << deltaX << endl;
        cout << "initX: " << initX << endl;
        cout << "initY: " << initY << endl << endl;
    }

    // image of pixel values
    const int HEIGHT = nff.v_resolution[1];
    const int WIDTH = nff.v_resolution[0];
    // allocate contiguous bytes for storing pixel color values
    unsigned char* pixels = new unsigned char[HEIGHT*WIDTH*3];

    // iterate over all pixels in the image ((0,0) is the top left corner pixel)
    for (int j=0; j<HEIGHT; j++) {      // iterate over rows
        for (int i=0; i<WIDTH; i++) {   // iterate over columns
            // (i,j) is the current pixel (in image coordinates)
            // center of this pixel (in camera coordinates)
            double x = initX + deltaX*i;
            double y = initY - deltaX*j;
            double z = -(nff.v_from - nff.v_at).norm();
            // transform (center of this pixel) camera coordinates -> world coordinates
            Vector3d pos = x*u + y*v + z*w + nff.v_from;

            // construct ray from camera through center of pixel
            Ray ray = Ray(nff.v_from, pos-nff.v_from);
            // get the color for this pixel
            Vector3d color = trace(ray, bounces);
            // set color of pixel
            for (int k=0; k<3; k++) {
                color[k] = min(max(color[k], 0.0), 1.0); // force value in range [0,1]
                pixels[j*(WIDTH*3) + (i*3) + k] = (int) (color[k] * 255);
            }
        }
    }

    // write image to file
    FILE *f = fopen(filename.c_str(), "wb");
    fprintf(f, "P6\n%d %d\n%d\n", WIDTH, HEIGHT, 255);
    fwrite(pixels, 1, HEIGHT*WIDTH*3, f);
    fclose(f);
    delete[] pixels;
    cout << "image written to " << filename << endl;
}

/**
 * return the color of the first object hit by this ray
 * (returns the background color if no object is hit)
 *
 * Ray:     the Ray being shot through the scene
 * bounces: number of times to reflect ray after first intersection
 * debug:   whether to print extra info for debugging
 */
Vector3d RayTracer::trace(Ray ray, int bounces, bool debug) {
    HitRecord hit = getHitRecord(ray, nff.v_hither, -1, debug);
    if (hit.surfIndex == -1) // no intersection
        return nff.b_color;

    // ray intersected with an object
    //return matr->color; // if we're not doing any shading at all
    // TODO: if we add matr field to HitRecord, just use that...
    Material *matr = nff.surfaces[hit.surfIndex]->matr;
    Vector3d localColor = Vector3d(0,0,0);

    // do shading:::
    // iterate over lights:
    Vector3d V, N, H, L, R;
    // unit vector from point of intersection to origin of ray
    V = (hit.point - ray.eye).normalized(); // TODO: should this always point at viewer???
    //V = (hit.point - nff.v_from).normalized();
    double lightIntensity = 1.0 / sqrt(nff.lights.size());
    for (unsigned int i=0; i<nff.lights.size(); i++) {
        Light light = nff.lights[i];
        // unit vector pointing at light from point of intersection on surface
        L = (light.pos - hit.point).normalized();
        // check if light is visible (doing this creates shadows)
        if (getHitRecord(Ray(hit.point, L), SHADOW_BIAS, -1, debug).dist != -1)
            continue; // light isn't visible

        N = nff.surfaces[hit.surfIndex]->getNormal(hit); // surface normal
        H = (L + V) / (L+V).norm(); // unit vector from hit.point bisecting angle between L and v
        R = -V + 2*(V.dot(N)) * N;  // reflection direction

        // compute shading (diffuse and specular components)
        double diffuse = max(0.0, N.dot(L));
        double specular = pow(max(0.0, N.dot(H)), matr->shine);
        for (int c=0; c<3; c++) {
            localColor[c] += (matr->Kd * matr->color[c] * diffuse + matr->Ks * specular) * lightIntensity;
        }
    }
    if (bounces-1 > 0) {
        localColor += matr->Ks * trace(Ray(hit.point, R), bounces-1);
    }
    return localColor;
}

/**
 * get the hit record for this ray's intersection with a surface in the scene
 * HitRecord surfIndex is set to -1 if there is no intersection
 */
HitRecord RayTracer::getHitRecord(Ray ray, double d0, double d1, bool debug) {
    HitRecord bestHit(SurfaceType::POLYGON, -1); // record of the closest hit so far
    for (unsigned int i=0; i<nff.surfaces.size(); i++) {
        HitRecord hit = nff.surfaces[i]->intersect(ray, d0, d1, debug);

        // check if we need to update bestHit
        if (hit.dist != -1 && (bestHit.dist == -1 || hit.dist < bestHit.dist)) {
            // we had our first intersection, or found a closer intersection
            bestHit = hit;
            bestHit.surfIndex = i; // index of closest surface intersected by this ray
        }
    }
    return bestHit;
}

/**
 * for fun
 * output multiple frames on the scene in a moving animation
 *
 * filename:   first part of filename
 *             (<number>.ppm will be appended for each frame)
 * debug:      whether to print out extra info for debugging
 */
void RayTracer::animate(std::string filename, bool debug) {
    int count = 0;
    int bounces = 5;
    /*
    for (double h=0; h<10; h+=0.1) {
        std::string tmp = "";
        std::stringstream ss;
        ss << filename << count << ".ppm";
        ss >> tmp;

        nff.v_hither = h;
        render(tmp, debug);
        count++;
    }
    */
    Vector3d w = (nff.v_from - nff.v_at).normalized();
    Vector3d u = nff.v_up.cross(w).normalized();
    Vector3d v = w.cross(u).normalized();

    Vector3d start = nff.v_from;
    for (double a=0; a<100; a+=1) {
        double angle = (2*M_PI) * (a/100);
        std::string tmp = "";
        std::stringstream ss;
        ss << filename << count << ".ppm";
        ss >> tmp;

        Vector3d axis = v;
        Vector3d c = nff.v_at; // center of rotation

        Vector3d point = start;
        Eigen::Affine3d A = Eigen::Translation3d(c) * Eigen::AngleAxisd(angle, axis) * Eigen::Translation3d(-c);
        //Eigen::Vector4d point4 = point.conservativeResize(4);
        //point4(3)=1;
        Eigen::Vector4d point4;
        point4[0] = point[0];
        point4[1] = point[1];
        point4[2] = point[2];
        point4[3] = 1;
        point4=(A.matrix() * point4);
        //point = point4.conservativeResize(3);

        point[0] = point4[0] / point4[3];
        point[1] = point4[1] / point4[3];
        point[2] = point4[2] / point4[3];


        nff.v_from = point;

        // create frame
        render(tmp, bounces, debug);
        count++;
    }
}
