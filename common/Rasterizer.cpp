#include <iostream>
#include "Rasterizer.h"
#include <assert.h>
#include <math.h>
#include <iomanip>
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
    // TODO: now flip the image horizontally as well???
    nff.v_up = -1 * nff.v_up; // hack to flip the image (so it's not upside down)

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
    Fragment ***frags = new Fragment**[HEIGHT];
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
    Fragment *cur, *next;
    for (int j=0; j<HEIGHT; j++) {      // iterate over rows
        for (int i=0; i<WIDTH; i++) {   // iterate over columns
            cur = frags[j][i];
            if (cur == NULL)
                continue;
            while (cur != NULL) {
                next = cur->next;
                delete cur;
                cur = next;
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
        Triangle &tri = triangles[i];           // current triangle
        for (int c=0; c<3; c++) {
            // do shading on current vertex in world space (no reflection or shadows)
            tri.colors[c] = shadePoint(tri, c);

            // transform vertices from world space -> image space
            Eigen::Vector4d tmp;
            tmp << tri.points[c], 1;
            tmp = M * tmp; // map this point to image coordinates
            // TODO: consider waiting to do the homogenous divide until after clipping (if I do clipping)
            tmp = tmp / tmp[3];
            // store the image points (but keep the world space points around)
            // (the z coordinate of each imgPoint will be used for depth ordering later)
            tri.imgPoints[c] = Vector3d(tmp[0], tmp[1], tmp[2]);
        }
    }
}

/*
 * helper function for vertexProcessing()
 * shades a vertex of a triangle and returns the color
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
 *
 */
void Rasterizer::rasterization(struct Fragment ***frags) {
    int fragCount = 0;
    for (unsigned int i=0; i<triangles.size(); i++) {
        Triangle &tri = triangles[i];

        // compute 2D bounding box of this triangle's imgPoints (z coordinates don't matter)
        //double minX=DBL_MAX, maxX=-DBL_MIN, minY=DBL_MAX, maxY=-DBL_MAX;
        double minX=tri.imgPoints[0][0], maxX=minX, minY=tri.imgPoints[0][1], maxY=minY;
        for (int c=1; c<3; c++) {
            if (tri.imgPoints[c][0] < minX)
                minX = tri.imgPoints[c][0];
            if (tri.imgPoints[c][0] > maxX)
                maxX = tri.imgPoints[c][0];

            if (tri.imgPoints[c][1] < minY)
                minY = tri.imgPoints[c][1];
            if (tri.imgPoints[c][1] > maxY)
                maxY = tri.imgPoints[c][1];
        }
        minX = floor(minX);
        minY = floor(minY);
        maxX = ceil(maxX);
        maxY = ceil(maxY);

        int skipCount = 0;

        // iterate over pixels in bounding box (with corners (minX,minY) and (maxX,maxY))
        // (x,y) are the coordinates of the center of a (1x1) pixel
        for (int y=(int) minY; y<=(int) maxY; y++) {
            for (int x=(int) minX; x<=(int) maxX; x++) {
                // ensure (x,y) is on the image (this is instead of clipping)
                if (!(0<=x && x<nff.v_resolution[0] && 0<=y && y<nff.v_resolution[1])) {
                    skipCount++;
                    continue;
                }
                // get fragment for this pixel
                Fragment *frag = getFrag(x, y, tri);
                if (frag == NULL) // pixel doesn't overlap tri
                    continue;
                fragCount++;

                // store fragment at front of: linked list for this pixel
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
    printf("created %d fragments\n", fragCount);
}

/**
 * (helper function for rasterization())
 * checks if pixel overlaps this triangle and returns a fragment for this pixel
 * (NULL if pixel doesn't overlap triangle)
 *
 * (x,y) is the coordinates of the pixel (center of pixel in image space)
 * tri:  triangle to check if this pixel overlaps (i.e. (x,y) is inside)
 * TODO: create static function in Triangle class to check if 2d point is inside 2d triangle
 */
Fragment *Rasterizer::getFrag(int x, int y, Triangle &tri) {
    Fragment *frag = NULL;

    // check if point (x,y) lines inside the 2D triangle formed by tri.imgPoints (ignoring the z component of each):
    double Ax = tri.imgPoints[0][0], Ay = tri.imgPoints[0][1];
    double Bx = tri.imgPoints[1][0], By = tri.imgPoints[1][1];
    double Cx = tri.imgPoints[2][0], Cy = tri.imgPoints[2][1];
    // compute a, B, g
    double B = ((Ay-Cy)*x + (Cx-Ax)*y + Ax*Cy - Cx*Ay) / ((Ay-Cy)*Bx + (Cx-Ax)*By + Ax*Cy -Cx*Ay);
    double g = ((Ay-By)*x + (Bx-Ax)*y + Ax*By - Bx*Ay) / ((Ay-By)*Cx + (Bx-Ax)*Cy + Ax*By - Bx*Ay);
    double a = 1.0 - B - g;
    double g_num = ((Ay-By)*x + (Bx-Ax)*y + Ax*By - Bx*Ay);
    double g_denom = ((Ay-By)*Cx + (Bx-Ax)*Cy + Ax*By - Bx*Ay);

    //  TODO: check a, B, g here to check if point is on triangle edge so we can deal with cracks (holes)
    if (a>=0 && B>=0 && g>=0) {
        // store info about this point on the triangle (for computing this pixel's color later)
        frag = new Fragment;
        // interpolate worldPos, normal, color, and zValue at this point:
        frag->worldPos = a * tri.imgPoints[0]    + B * tri.imgPoints[1]    + g * tri.imgPoints[2];
        frag->normal =   a * tri.norms[0]        + B * tri.norms[1]        + g * tri.norms[2];
        frag->color =    a * tri.colors[0]       + B * tri.colors[1]       + g * tri.colors[2];
        frag->zValue =   a * tri.imgPoints[0][2] + B * tri.imgPoints[1][2] + g * tri.imgPoints[2][2];
        frag->next = NULL;
    }
    return frag;
}

void Rasterizer::fragmentProcessing(Fragment ***frags) {
    // TODO: implement if desired
}

void Rasterizer::blending(unsigned char* pixels, Fragment ***frags) {
    const int HEIGHT = nff.v_resolution[1];
    const int WIDTH = nff.v_resolution[0];
    // iterate over all pixels in the image ((0,0) is the top left corner pixel)
    int count = 0;
    for (int j=0; j<HEIGHT; j++) {      // iterate over rows
        for (int i=0; i<WIDTH; i++) {   // iterate over columns
            // (i,j) is the current pixel (in image coordinates)
            // determine the color for this pixel:
            Vector3d color = nff.b_color;

            if (frags[j][i] != NULL) {
                count++;
                Fragment *best=frags[j][i], *cur=best;
                while (cur != NULL) {
                    // TODO: why isn't it < instead of > here?
                    if (cur->zValue > best->zValue) {
                        best = cur;
                    }
                    cur = cur->next;
                }
                color = best->color;
            }

            // set color of pixel
            for (int k=0; k<3; k++) {
                color[k] = min(max(color[k], 0.0), 1.0); // force value in range [0,1]
                pixels[j*(WIDTH*3) + (i*3) + k] = (int) (color[k] * 255);
            }
        }
    }
    printf("colored %d pixels\n", count);
}
