#include "hw2.h"
#include "hw2_scenes.h"

using namespace hw2;

Image3 hw_2_1(const std::vector<std::string> &params) {
    // Homework 2.1: render a single 3D triangle

    Image3 img(640 /* width */, 480 /* height */);

    Vector3 p0{0, 0, -1}; //first point on 3d triangle
    Vector3 p1{1, 0, -1}; //second point on tri
    Vector3 p2{0, 1, -1}; //third point on tri
    Real s = 1; // scaling factor of the view frustrum; controls the size of our image
    Vector3 color = Vector3{1.0, 0.5, 0.5}; // color of the triangle/shape
    Real z_near = 1e-6; // distance of the near clipping plane
    Real a = img.width / img.height; // a or aspect ratio

    // for every parameter in our array, check what the parameter is and assign variables as indicated
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-p0") {
            p0.x = std::stof(params[++i]);
            p0.y = std::stof(params[++i]);
            p0.z = std::stof(params[++i]);
        } else if (params[i] == "-p1") {
            p1.x = std::stof(params[++i]);
            p1.y = std::stof(params[++i]);
            p1.z = std::stof(params[++i]);
        } else if (params[i] == "-p2") {
            p2.x = std::stof(params[++i]);
            p2.y = std::stof(params[++i]);
            p2.z = std::stof(params[++i]);
        } else if (params[i] == "-color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        }
    }

    // Image rendering; for every pixel in the image, make it a certain color.
    // p0, p1, p2 are vertices of the triangle.
    // xpp = img.width * ((xp + s*a) / (2*s*a)); xpp is x'' and xp is x'
    // ypp = -img.height * ((yp + s) / (2*s)); ypp is y'' and yp is y'
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{0.5, 0.5, 0.5}; // The pixel at coordinate x,y is the default color of the background (gray)
            Vector2 pCenter = Vector2{x + Real(0.5), y + Real(0.5)}; // Pixel center for pixel (x,y) = (x+0.5, y+0.5)
            Vector3 pxCenter = Vector3{pCenter.x, pCenter.y, 1.0};
            
            // p' is a projected point (x', y') in the camera space represented by pp0, pp1, pp2. We use Eq 1 to get these.
            Vector2 pp0 = {p0.x/-p0.z, p0.y/-p0.z};
            Vector2 pp1 = {p1.x/-p1.z, p1.y/-p1.z};
            Vector2 pp2 = {p2.x/-p2.z, p2.y/-p2.z};

            // We want this point inside of the screen space instead to render it.
            Real xpp0 = img.width * ((pp0.x + s*a) / (Real(2)*s*a)); // x'' for the first vertex on the triangle
            Real xpp1 = img.width * ((pp1.x + s*a) / (Real(2)*s*a)); // x'' for the second vertex on the triangle
            Real xpp2 = img.width * ((pp2.x + s*a) / (Real(2)*s*a)); // x'' for the third vertex on the triangle
            Real ypp0 = -img.height * ((pp0.y + s) / (Real(2)*s)); // y'' for the first vertex on the triangle
            Real ypp1 = -img.height * ((pp1.y + s) / (Real(2)*s)); // y'' for the second vertex on the triangle
            Real ypp2 = -img.height * ((pp2.y + s) / (Real(2)*s)); // y'' for the third vertex on the triangle

            // p'' is a point (x'', y'') in the screen space represented by ppp0, ppp1, ppp2. We use Eq 3 and Eq 4 to get these.
            Vector2 ppp0 = {xpp0, ypp0}; 
            Vector2 ppp1 = {xpp1, ypp1}; 
            Vector2 ppp2 = {xpp2, ypp2};

            Vector2 e01 = ppp1 - ppp0; // Edge direction between point 0 and 1
            Vector2 e12 = ppp2 - ppp1;
            Vector2 e20 = ppp0 - ppp2;
            Vector2 q1 = pCenter - ppp0; // Vector from point "q" to the point p0
            Vector2 q2 = pCenter - ppp1;
            Vector2 q3 = pCenter - ppp2;
            Vector2 n01 = {-e01.y, e01.x}; // Normal vector to first direction
            Vector2 n12 = {-e12.y, e12.x};
            Vector2 n20 = {-e20.y, e20.x};
            bool d1 = dot(q1, n01) >= 0;
            bool d2 = dot(q2, n12) >= 0;
            bool d3 = dot(q3, n20) >= 0;
            bool allPos = d1 && d2 && d3;
            bool allNeg = !d1 && !d2 && !d3;
            if (allPos || allNeg)
            {                           // If the point is in the positive half of all half-planes,
                img(x, y) = color; // The pixel at coordinate x,y is the triangle's color
            }
        }
    }

    // Antialiasing as a postprocess of rendered image
    for (int y = 0; y < img.height; y++){
        for (int x = 0; x < img.width; x++){
            // img(x, y) = scene.background; // The pixel at coordinate x,y is the color of the background
            Vector3 pxColor;
            // For each subpixel, 
            for (Real j = 0.125; j < 1; j += 0.25) {
                for (Real i = 0.125; i < 1; i += 0.25) {
                    Vector2 pCenter = Vector2{Real(x+i), Real(y+j)}; // Pixel center for subpixel (i,j)
                    Vector3 subColor = img(pCenter.x,pCenter.y);
                    pxColor += subColor;
                }
            }
            pxColor /= Real(16);
            img(x,y) = pxColor;
        }
    }
    return img;
}

Image3 hw_2_2(const std::vector<std::string> &params) {
    // Homework 2.2: render a triangle mesh

    Image3 img(640 /* width */, 480 /* height */);

    Real s = 1; // scaling factor of the view frustrum
    Real z_near = 1e-6; // distance of the near clipping plane
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        } else if (params[i] == "-scene_id") {
            scene_id = std::stoi(params[++i]);
        }
    }

    TriangleMesh mesh = meshes[scene_id];
    UNUSED(mesh); // silence warning, feel free to remove this

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
    }
    return img;
}

Image3 hw_2_3(const std::vector<std::string> &params) {
    // Homework 2.3: render a triangle mesh with vertex colors

    Image3 img(640 /* width */, 480 /* height */);

    Real s = 1; // scaling factor of the view frustrum
    Real z_near = 1e-6; // distance of the near clipping plane
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        } else if (params[i] == "-scene_id") {
            scene_id = std::stoi(params[++i]);
        }
    }

    TriangleMesh mesh = meshes[scene_id];
    UNUSED(mesh); // silence warning, feel free to remove this

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
    }
    return img;
}

Image3 hw_2_4(const std::vector<std::string> &params) {
    // Homework 2.4: render a scene with transformation
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.camera.resolution.x,
               scene.camera.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
    }
    return img;
}

