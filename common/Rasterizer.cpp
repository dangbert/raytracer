#include <iostream>
#include "Rasterizer.h"
#include <assert.h>
#include <math.h>
using namespace std;

////////////////////////////
///// Rasterizer class: ////
////////////////////////////
/**
 * constructs a Rasterizer object
 * filename:  name of nff file to use
 *
 * only supports rendering convex polygons (breaks them into triangles)
 */
Rasterizer::Rasterizer(std::string filename) {
    if (nff.readFile(filename) != 0) {
        cout << "error parsing " << filename << endl;
        exit(1);
    }
    cout << nff << endl;

    // store rasterizer settings:
    // vectors defining our world coordinates (origin is at nff.v_from):
    scene.eye = nff.v_from;
    scene.w = (nff.v_from - nff.v_at).normalized();
    scene.u = nff.v_up.cross(scene.w).normalized();
    scene.v = scene.w.cross(scene.u).normalized();

    // planes defining view frustum:
    scene.h =       tan(nff.v_angle / 2.0 * M_PI/180.0);
    scene.right =   scene.h;
    scene.left =   -1.0 * scene.right;
    scene.top =    scene.right / (nff.v_resolution[0]/nff.v_resolution[1]);
    scene.bottom = scene.left / (nff.v_resolution[0]/nff.v_resolution[1]);
    scene.near =   -1.0 *nff.v_hither;
    scene.far =    1000 * scene.near;
}

/**
 * renders the nff file for this object and writes the image to a file
 * filename:  name for output file (should end in .ppm)
 * debug:     whether or not to print out debug info for the render proces
 */
void Rasterizer::render(std::string filename, bool debug) {
    cout << "\nrasterizing..." << endl;
    if (debug) {
        cout << nff << endl;
    }

    // create image of pixel values
    const int HEIGHT = nff.v_resolution[1];
    const int WIDTH = nff.v_resolution[0];
    // allocate contiguous bytes for storing pixel color values
    unsigned char* pixels = new unsigned char[HEIGHT*WIDTH*3];
    // 2D array containing a linked lists of fragments for each pixel
    struct Fragment ***frags = new Fragment**[HEIGHT];
    for (int j=0; j<HEIGHT; j++) {      // iterate over rows
        frags[j] = new Fragment*[WIDTH];
        for (int i=0; i<WIDTH; i++) {   // iterate over columns
            frags[j][i] = NULL;
        }
    }

    Eigen::Matrix4d M = createMatrix(debug);    // build matrix to apply to each vertex
    vertexProcessing(M);                        // vertex processing
    cout << "there are " << triangles.size() << " triangles" << endl;

    rasterization(frags);                       // rasterization
    fragmentProcessing(frags);                  // fragment processing
    blending(pixels, frags);                    // blending

    // write image to file
    FILE *f = fopen(filename.c_str(), "wb");
    fprintf(f, "P6\n%d %d\n%d\n", WIDTH, HEIGHT, 255);
    fwrite(pixels, 1, HEIGHT*WIDTH*3, f);
    fclose(f);

    // free up memory
    delete[] pixels;
    struct Fragment *cur, *next;
    for (int j=0; j<HEIGHT; j++) {      // iterate over rows
        for (int i=0; i<WIDTH; i++) {   // iterate over columns
            cur = frags[j][i];
            if (cur == NULL)
                continue;
            next = cur->next;
            while (cur != NULL) {
                delete cur;
                cur = next;
                next = cur->next;
            }
        }
    }
    cout << "image written to " << filename << endl;
}

/**
 * returns the matrix needed to tranform points from world space to image space
 */
Eigen::Matrix4d Rasterizer::createMatrix(bool debug) {
    int nx = nff.v_resolution[0];
    int ny = nff.v_resolution[1];

    Eigen::Matrix4d M_vp, M_per, M;
    // p. 144 in textbook:
    M_vp << nx/2.0, 0, 0, (nx-1)/2.0,
            0, ny/2.0, 0, (ny-1)/2.0,
            0, 0, 1, 0,
            0, 0, 0, 1;

    // p. 155 in textbook:
    M_per << (2.0*scene.near)/(scene.right-scene.left), 0, (scene.left+scene.right)/(scene.left-scene.right), 0,
            0, (2.0*scene.near)/(scene.top-scene.bottom), (scene.bottom+scene.top)/(scene.bottom-scene.top), 0,
            0, 0, (scene.far+scene.near)/(scene.near-scene.far), (2.0*scene.far*scene.near)/(scene.far-scene.near),
            0, 0, 1, 0;

    // p. 147 in textbook:
    Eigen::MatrixXd M_cam; // dynamically resizable matrix
    M_cam.resize(3,4);
    M_cam << scene.u, scene.v, scene.w, scene.eye;
    M_cam.conservativeResize(4, 4);
    M_cam.row(3) = Eigen::Vector4d(0,0,0,1);
    M_cam = M_cam.inverse();

    M = M_vp * M_per * M_cam;
    if (debug) {
        cout << "M_vp =\n" << M_vp << endl;
        cout << "M_per=\n" << M_per << endl;
        cout << "M_cam=\n" << M_cam << endl;
        cout << "M\n" << M << endl;
    }
    return M;
}

/*
 * breaks polygon in the scene down into triangles
 * and stores them in a vector and transforms them using matrix M
 */
void Rasterizer::vertexProcessing(Eigen::Matrix4d M) {
    triangles.clear(); // vector of all triangles in scene
    // break all polygons down into triangle and store them in a vector
    for (unsigned int i=0; i<nff.surfaces.size(); i++) {
        Polygon *poly = dynamic_cast<Polygon*>(nff.surfaces[i]);
        if (poly != NULL) {
            for (unsigned int t=0; t < poly->triangles.size(); t++) {
                triangles.push_back(poly->triangles[t]);
            }
        }
    }

    // compute shading for each vertex of the triangle (while in world space)
    // apply matrix to vertices in each triangle (world space -> image space)
    for (unsigned int i=0; i<triangles.size(); i++) {
        // TODO: what to do with w coordinate?
        // are the transformed triangles in 2D space???
        for (int c=0; c<3; c++) {
            // do shading on this vertex in world space (no reflection or shadows)
            triangles[i].colors[c] = shadePoint(triangles[i], c);

            // transform vertices from world space -> image space
            Eigen::Vector4d tmp;
            tmp << triangles[i].points[c], 1;
            tmp = M * tmp;
            // TODO: wait to do the homogenous divide until after clipping (if I do clipping)
            tmp = tmp / tmp[3];
            // store the image points (but keep the world space points around)
            // the z coordinate of each imgPoint will be used for depth later
            triangles[i].imgPoints[c] = Vector3d(tmp[0]/tmp[3], tmp[1]/tmp[3], tmp[2]/tmp[3]);
        }
    }
}

/*
 * helper function for vertexProcessing()
 * shades a point on a triangle and return the color
 *
 * based on RayTracer::trace()
 */
Vector3d Rasterizer::shadePoint(Triangle &tri, int vertex) {
    Vector3d color = Vector3d(0,0,0);
    Vector3d point = tri.points[vertex];  // point being shaded

    // do shading:::
    Vector3d V, N, H, L, R;
    V = (point - scene.eye).normalized(); // unit vector from point, to the viewer
    double lightIntensity = 1.0 / sqrt(nff.lights.size());
    // iterate over lights:
    for (unsigned int i=0; i<nff.lights.size(); i++) {
        Light light = nff.lights[i];
        // unit vector pointing at light from point
        L = (light.pos - point).normalized();
        // check if light is visible (doing this creates shadows)
        // TODO: do I just leave this out for rasterization without shadows???
        //if (getHitRecord(Ray(point, L), SHADOW_BIAS, -1).dist != -1)
        //    continue; // light isn't visible

        if (tri.isPatch()) {
            N = tri.norms[vertex];    // surface normal
        }
        else {
            // if not a triangle patch, get the normal of the plane containing the triangle
            // (pass it any HitRecord object, it won't use it)
            N = tri.getNormal(HitRecord(SurfaceType::POLYGON, -1), false);
        }
        H = (L + V) / (L+V).norm(); // unit vector from point, bisecting angle between L and v
        R = -V + 2*(V.dot(N)) * N;  // reflection direction

        // compute shading (diffuse and specular components)
        // (see p.82 in textbook)
        double diffuse = max(0.0, N.dot(L));
        double specular = pow(max(0.0, N.dot(H)), tri.matr->shine);
        for (int c=0; c<3; c++) {
            color[c] += (tri.matr->Kd * tri.matr->color[c] * diffuse + tri.matr->Ks * specular) * lightIntensity;
        }
    }
    // force colors in range [0,1]
    for (int c=0; c<3; c++) {
        color[c] = min(max(color[c], 0.0), 1.0);
    }
    return color;
}


/**
 * get the hit record for this ray's intersection with a surface in the scene
 * HitRecord surfIndex is set to -1 if there is no intersection
 *
 * based on RayTracer::getHitRecord()
 * TODO: delete this function if I don't need it (if not doing shadows)
 */
HitRecord Rasterizer::getHitRecord(Ray ray, double d0, double d1) {
    HitRecord bestHit(SurfaceType::POLYGON, -1); // record of the closest hit so far
    // TODO: should we ignore shapes that aren't polygon cause we aren't rendering them?
    for (unsigned int i=0; i<nff.surfaces.size(); i++) {
        HitRecord hit = nff.surfaces[i]->intersect(ray, d0, d1);

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
 *
 */
void Rasterizer::rasterization(struct Fragment ***frags) {
    for (unsigned int i=0; i<triangles.size(); i++) {
        Triangle &tri = triangles[i];

        // compute 2D bounding box of this triangle
        //double minX=DBL_MAX, maxX=-DBL_MIN, minY=DBL_MAX, maxY=-DBL_MAX;
        double minX=tri.points[0][0], maxX=minX, minY=tri.points[0][1], maxY=minY;
        for (int c=1; c<3; c++) {
            if (tri.points[c][0] < minX)
                minX = tri.points[c][0];
            if (tri.points[c][0] > maxX)
                maxX = tri.points[c][0];

            if (tri.points[c][1] < minY)
                minY = tri.points[c][1];
            if (tri.points[c][1] > maxY)
                maxY = tri.points[c][1];
        }

        // iterate over pixels in bounding box (with corners (minX,minY) and (maxX,maxY))
        // (x,y) are the coordinates of the center of a (1x1) pixel
        for (int y=floor(minY); y<=ceil(maxY); y++) {
            for (int x=floor(minX); x<=ceil(maxX); x++) {
                // check if point (x,y) lines inside the 2D triangle formed by tri.imgPoints (ignoring the z component of each):

                // construct a triangle using tri.imgPoints as vertices, but set z values to 0
                Vector3d points[3] = {tri.imgPoints[0], tri.imgPoints[1], tri.imgPoints[2]};
                ///Vector3d normals[3] = {tri.norms[0], tri.norms[1], tri.norms[2]};
                for (int c=0; c<3; c++) {
                    tri.points[c][2] = 0;
                }
                ///Triangle tmp(points, normals);
                // check if a ray from (0,0,0) -> (x,y,0) would hit this new triangle
                HitRecord hit = Triangle(points).intersect(Ray(Vector3d(0,0,0), Vector3d(x, y, 0)));
                if (hit.t == -1)
                    continue; // (x,y) is not on the triangle
                // TODO: check hit.a, hit.B, hit.g here to check if point is on triangle edge so we can deal with cracks (holes)
                // store info about this point on the triangle (for computing this pixel's color later)
                struct Fragment *frag = new struct Fragment;
                // interpolate normal, color, and zValue at this point:
                frag->normal = hit.a * tri.norms[0] + hit.B * tri.norms[1] + hit.g * tri.norms[2];
                frag->color = hit.a * tri.colors[0] + hit.B * tri.norms[1] + hit.g * tri.norms[2];
                frag->zValue = hit.a * tri.points[0][2] + hit.B * tri.points[1][2] + hit.g * tri.points[2][2];
                // interpolate world position of intersection point:
                frag->worldPos = hit.a * tri.points[0] + hit.B * tri.points[1] + hit.g * tri.points[2];
                frag->next = NULL;

                // store frag at front of linked list for this pixel
                if (frags[y][x] == NULL) {
                    frags[y][x] = frag;
                }
                else {
                    frag->next = frags[y][x];
                    frags[y][x] = frag;
                }
            }

        }

    }

}

void Rasterizer::fragmentProcessing(struct Fragment ***frags) {
    // TODO: implement if desired
}

void Rasterizer::blending(unsigned char* pixels, struct Fragment ***frags) {
    const int HEIGHT = nff.v_resolution[1];
    const int WIDTH = nff.v_resolution[0];
    // iterate over all pixels in the image ((0,0) is the top left corner pixel)
    for (int j=0; j<HEIGHT; j++) {      // iterate over rows
        for (int i=0; i<WIDTH; i++) {   // iterate over columns
            // (i,j) is the current pixel (in image coordinates)
            // determine the color for this pixel:
            Vector3d color = nff.b_color;

            if (frags[j][i] != NULL) {
                Fragment *best=frags[j][i], *cur=best;
                while (cur != NULL) {
                    if (cur->zValue < best->zValue) {
                        best = cur;
                    }
                    cur = cur->next;
                }
                color = best->color;
                cout << "set a best color" << endl;
            }

            // set color of pixel
            for (int k=0; k<3; k++) {
                color[k] = min(max(color[k], 0.0), 1.0); // force value in range [0,1]
                pixels[j*(WIDTH*3) + (i*3) + k] = (int) (color[k] * 255);
            }
        }
    }
}
