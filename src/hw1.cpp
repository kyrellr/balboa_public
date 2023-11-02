#include "hw1.h"
#include "hw1_scenes.h"

using namespace hw1;

Image3 hw_1_1(const std::vector<std::string> &params) {
    // Homework 1.1: render a circle at the specified
    // position, with the specified radius and color.
    // Given: Circle center, circle radius, and circle color
    // 

    Image3 img(640 /* width */, 480 /* height */);

    Vector2 center = Vector2{img.width / 2 + Real(0.5), img.height / 2 + Real(0.5)};
    Real radius = 100.0;
    Vector3 color = Vector3{1.0, 0.5, 0.5};
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-center") {
            Real x = std::stof(params[++i]);
            Real y = std::stof(params[++i]);
            center = Vector2{x, y};
        } else if (params[i] == "-radius") {
            radius = std::stof(params[++i]);
        } else if (params[i] == "-color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        }
    }

    // For each pixel (at coordinate x,y),
    // If the pixel center hits the circle, then make it the circle color.
    // Else, let its color be white.
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector2 pCenter = Vector2{x + Real(0.5), y + Real(0.5)}; // Pixel center for pixel (x,y) = (x+0.5, y+0.5)
            Real distance = sqrt(pow(pCenter.x - center.x, 2) + pow(pCenter.y - center.y, 2)); // distance from the pixel center to the circle center

            if (distance <= radius) { // If the distance is equal to or less than the radius (hits circle)
                img(x,y) = color; // The pixel at coordinate x,y is the circle's color
            } else {
                img(x, y) = Vector3{0.5, 0.5, 0.5}; // The pixel at coordinate x,y is color 0.5, 0.5, 0.5 = gray (background)
            }

        }
    }
    return img;
}

Image3 hw_1_2(const std::vector<std::string> &params) {
    // Homework 1.2: render multiple circles
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    int scene_id = std::stoi(params[0]);
    const CircleScene &scene = hw1_2_scenes[scene_id];

    Image3 img(scene.resolution.x, scene.resolution.y);

    // For each pixel (at coordinate x,y),
    //     The background is initialized to be the scene's background color.
    //     For each circle,
    //          if pixel center hits circle, pixel is circle color.
    //          if another circle is closer to the pixel center, overwrite pixel color with that circle's color.
    // Assume circles are ordered from farthest to closest.  
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = scene.background; // The pixel at coordinate x,y is the color of the background
            Vector2 pCenter = Vector2{x + Real(0.5), y + Real(0.5)}; // Pixel center for pixel (x,y) = (x+0.5, y+0.5)
            for (int c = 0; c < scene.objects.size(); c++) {
                Circle currCircle = scene.objects[c];
                Real distance = sqrt(pow(pCenter.x - currCircle.center.x, 2) + pow(pCenter.y - currCircle.center.y, 2)); // distance from the pixel center to the current circle center
                
                if (distance <= currCircle.radius) { // If the distance is equal to or less than the radius (hits current circle)
                    img(x, y) = currCircle.color; // The pixel at coordinate x,y is the current circle's color
                }
            }
        }
    }
    return img;
}

Image3 hw_1_3(const std::vector<std::string> &params) {
    // Homework 1.3: render multiple shapes
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    // For each pixel, 
    //     First initialize the background to be the scene's background color.
    //     Also, get the pixel center.
    //     For each shape (circle, rectangle, triangle),
    //         if the pixel center hits the shape, then change the pixel color to that shape's color
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = scene.background; // The pixel at coordinate x,y is the color of the background
            Vector2 pCenter = Vector2{x + Real(0.5), y + Real(0.5)}; // Pixel center for pixel (x,y) = (x+0.5, y+0.5)

            for (Shape shape : scene.shapes) { // For each shape
                if (auto *circle = std::get_if<Circle>(&shape)) { // If the shape is a Circle,
                    Real distance = sqrt(pow(pCenter.x - circle->center.x, 2) + pow(pCenter.y - circle->center.y, 2)); // distance from the pixel center to the current circle center        
                    if (distance <= circle->radius) { // If the distance is equal to or less than the radius (hits current circle)
                        img(x, y) = circle->color; // The pixel at coordinate x,y is the current circle's color
                    }
                } 
                else if (auto *rectangle = std::get_if<Rectangle>(&shape)) { // If the shape is a Rectangle,
                    bool inXRange = (pCenter.x >= rectangle->p_min.x && pCenter.x <= rectangle->p_max.x); // if pixel center is within the min x and max x
                    bool inYRange = (pCenter.y >= rectangle->p_min.y && pCenter.y <= rectangle->p_max.y); // if pixel center is within the min y and max y
                    if (inXRange && inYRange) {
                        img(x, y) = rectangle->color; // The pixel at coordinate x,y is the rectangle's color
                    }
                } 
                else if (auto *triangle = std::get_if<Triangle>(&shape)) { // If the shape is a Triangle,
                    Vector2 e01 = triangle->p1 - triangle->p0; // Edge direction between point 0 and 1
                    Vector2 e12 = triangle->p2 - triangle->p1; 
                    Vector2 e20 = triangle->p0 - triangle->p2;
                    Vector2 q1 = pCenter - triangle->p0; // Vector from point "q" to the point p0
                    Vector2 q2 = pCenter - triangle->p1;
                    Vector2 q3 = pCenter - triangle->p2;
                    Vector2 n01 = {-e01.y, e01.x}; // Normal vector to first direction
                    Vector2 n12 = {-e12.y, e12.x};
                    Vector2 n20 = {-e20.y, e20.x};
                    bool d1 = dot(q1, n01) >= 0;
                    bool d2 = dot(q2, n12) >= 0;
                    bool d3 = dot(q3, n20) >= 0;
                    bool allPos = d1 && d2 && d3;
                    bool allNeg = !d1 && !d2 && !d3;
                    if (allPos || allNeg) { // If the point is in the positive half of all half-planes,
                        img(x, y) = triangle->color; // The pixel at coordinate x,y is the triangle's color
                    }
                }
            }
        } 
    }
    return img;
}

Image3 hw_1_4(const std::vector<std::string> &params) {
    // Homework 1.4: render transformed shapes
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = scene.background; // The pixel at coordinate x,y is the color of the background
            Vector2 pCenter = Vector2{x + Real(0.5), y + Real(0.5)}; // Pixel center for pixel (x,y) = (x+0.5, y+0.5)
            Vector3 pxCenter = Vector3{pCenter.x, pCenter.y, 1.0};

            for (Shape shape : scene.shapes) { // For each shape
                Matrix3x3 F = get_transform(shape);
                Vector3 invpCenter = inverse(F) * pxCenter;
                pCenter = {invpCenter.x, invpCenter.y};
                if (auto *circle = std::get_if<Circle>(&shape)) { // If the shape is a Circle,
                    Real distance = sqrt(pow(pCenter.x - circle->center.x, 2) + pow(pCenter.y - circle->center.y, 2)); // distance from the pixel center to the current circle center        
                    if (distance <= circle->radius) { // If the distance is equal to or less than the radius (hits current circle)
                        img(x, y) = circle->color; // The pixel at coordinate x,y is the current circle's color
                    }
                } 
                else if (auto *rectangle = std::get_if<Rectangle>(&shape)) { // If the shape is a Rectangle,
                    bool inXRange = (pCenter.x >= rectangle->p_min.x && pCenter.x <= rectangle->p_max.x); // if pixel center is within the min x and max x
                    bool inYRange = (pCenter.y >= rectangle->p_min.y && pCenter.y <= rectangle->p_max.y); // if pixel center is within the min y and max y
                    if (inXRange && inYRange) {
                        img(x, y) = rectangle->color; // The pixel at coordinate x,y is the rectangle's color
                    }
                } 
                else if (auto *triangle = std::get_if<Triangle>(&shape)) { // If the shape is a Triangle,
                    Vector2 e01 = triangle->p1 - triangle->p0; // Edge direction between point 0 and 1
                    Vector2 e12 = triangle->p2 - triangle->p1; 
                    Vector2 e20 = triangle->p0 - triangle->p2;
                    Vector2 q1 = pCenter - triangle->p0; // Vector from point "q" to the point p0
                    Vector2 q2 = pCenter - triangle->p1;
                    Vector2 q3 = pCenter - triangle->p2;
                    Vector2 n01 = {-e01.y, e01.x}; // Normal vector to first direction
                    Vector2 n12 = {-e12.y, e12.x};
                    Vector2 n20 = {-e20.y, e20.x};
                    bool d1 = dot(q1, n01) >= 0;
                    bool d2 = dot(q2, n12) >= 0;
                    bool d3 = dot(q3, n20) >= 0;
                    bool allPos = d1 && d2 && d3;
                    bool allNeg = !d1 && !d2 && !d3;
                    if (allPos || allNeg) { // If the point is in the positive half of all half-planes,
                        img(x, y) = triangle->color; // The pixel at coordinate x,y is the triangle's color
                    }
                }
            }
        }
    }
    return img;
}

Image3 hw_1_5(const std::vector<std::string> &params) {
    // Homework 1.5: antialiasing
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    // For each pixel,
    for (int y = 0; y < img.height; y++){
        for (int x = 0; x < img.width; x++){
            img(x, y) = scene.background; // The pixel at coordinate x,y is the color of the background
            Vector3 pxColor = Vector3{0.0, 0.0, 0.0};
            // For each subpixel, 
            for (Real j = 0.125; j < 1; j += 0.25) {
                for (Real i = 0.125; i < 1; i += 0.25) {
                    Vector2 pCenter = Vector2{Real(x+i), Real(y+j)}; // Pixel center for subpixel (i,j)
                    Vector3 pxCenter = Vector3{pCenter.x, pCenter.y, 1.0};
                    Vector3 subColor;

                    // For each shape,
                    for (Shape shape : scene.shapes) {
                        Matrix3x3 F = get_transform(shape);
                        Vector3 invpCenter = inverse(F) * pxCenter;
                        pCenter = {invpCenter.x, invpCenter.y};
                        if (auto *circle = std::get_if<Circle>(&shape)) { // If the shape is a Circle,
                            Real distance = sqrt(pow(pCenter.x - circle->center.x, 2) + pow(pCenter.y - circle->center.y, 2)); // distance from the pixel center to the current circle center        
                            if (distance <= circle->radius) { // If the distance is equal to or less than the radius (hits current circle)
                                subColor = circle->color; // The subpixel at coordinate x,y is the current circle's color
                            }
                        } 
                        else if (auto *rectangle = std::get_if<Rectangle>(&shape)) { // If the shape is a Rectangle,
                            bool inXRange = (pCenter.x >= rectangle->p_min.x && pCenter.x <= rectangle->p_max.x); // if pixel center is within the min x and max x
                            bool inYRange = (pCenter.y >= rectangle->p_min.y && pCenter.y <= rectangle->p_max.y); // if pixel center is within the min y and max y
                            if (inXRange && inYRange) {
                                subColor = rectangle->color; // The subpixel at coordinate x,y is the rectangle's color
                            }
                        } 
                        else if (auto *triangle = std::get_if<Triangle>(&shape)) { // If the shape is a Triangle,
                            Vector2 e01 = triangle->p1 - triangle->p0; // Edge direction between point 0 and 1
                            Vector2 e12 = triangle->p2 - triangle->p1; 
                            Vector2 e20 = triangle->p0 - triangle->p2;
                            Vector2 q1 = pCenter - triangle->p0; // Vector from point "q" to the point p0
                            Vector2 q2 = pCenter - triangle->p1;
                            Vector2 q3 = pCenter - triangle->p2;
                            Vector2 n01 = {-e01.y, e01.x}; // Normal vector to first direction
                            Vector2 n12 = {-e12.y, e12.x};
                            Vector2 n20 = {-e20.y, e20.x};
                            bool d1 = dot(q1, n01) >= 0;
                            bool d2 = dot(q2, n12) >= 0;
                            bool d3 = dot(q3, n20) >= 0;
                            bool allPos = d1 && d2 && d3;
                            bool allNeg = !d1 && !d2 && !d3;
                            if (allPos || allNeg) { // If the point is in the positive half of all half-planes or negative half of all half-planes,
                                subColor = triangle->color; // The subpixel at coordinate x,y is the triangle's color
                            }
                        }
                    }
                    pxColor += subColor;
                }
            }
            pxColor /= Real(16);
            img(x,y) = pxColor;
        }
    }
    return img;
}

Image3 hw_1_6(const std::vector<std::string> &params) {
    // Homework 1.6: alpha blending
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);
    // Rendering image with alpha blending
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = scene.background; // The pixel at coordinate x,y is the color of the background
            Vector2 pCenter = Vector2{x + Real(0.5), y + Real(0.5)}; // Pixel center for pixel (x,y) = (x+0.5, y+0.5)
            Vector3 pxCenter = Vector3{pCenter.x, pCenter.y, 1.0};

            for (Shape shape : scene.shapes) { // For each shape
                // c0*alpha0 --> [shape].color (vector 3x1) * alpha (real 1x1) = 3x1 color
                // final color = (alpha)*(foreground color) + (1 - alpha)*(background color)
                Matrix3x3 F = get_transform(shape);
                Vector3 invpCenter = inverse(F) * pxCenter; 
                pCenter = {invpCenter.x, invpCenter.y}; 
                Vector3 backgroundColor = img(x,y);
                Vector3 foregroundColor = get_color(shape);
                if (auto *circle = std::get_if<Circle>(&shape)) { // If the shape is a Circle,
                    Vector3 finalColor = circle->alpha * foregroundColor + (1 - circle->alpha) * backgroundColor; 
                    Real distance = sqrt(pow(pCenter.x - circle->center.x, 2) + pow(pCenter.y - circle->center.y, 2)); // distance from the pixel center to the current circle center        
                    if (distance <= circle->radius) { // If the distance is equal to or less than the radius (hits current circle)
                        img(x, y) = finalColor; // The pixel at coordinate x,y is the current circle's color
                    }
                } 
                else if (auto *rectangle = std::get_if<Rectangle>(&shape)) { // If the shape is a Rectangle,
                    Vector3 finalColor = rectangle->alpha * foregroundColor + (1 - rectangle->alpha) * backgroundColor;
                    bool inXRange = (pCenter.x >= rectangle->p_min.x && pCenter.x <= rectangle->p_max.x); // if pixel center is within the min x and max x
                    bool inYRange = (pCenter.y >= rectangle->p_min.y && pCenter.y <= rectangle->p_max.y); // if pixel center is within the min y and max y
                    if (inXRange && inYRange) {
                        img(x, y) = finalColor; // The pixel at coordinate x,y is the rectangle's color
                    }
                } 
                else if (auto *triangle = std::get_if<Triangle>(&shape)) { // If the shape is a Triangle,
                    Vector3 finalColor = triangle->alpha * foregroundColor + (1 - triangle->alpha) * backgroundColor;
                    Vector2 e01 = triangle->p1 - triangle->p0; // Edge direction between point 0 and 1
                    Vector2 e12 = triangle->p2 - triangle->p1; 
                    Vector2 e20 = triangle->p0 - triangle->p2;
                    Vector2 q1 = pCenter - triangle->p0; // Vector from point "q" to the point p0
                    Vector2 q2 = pCenter - triangle->p1;
                    Vector2 q3 = pCenter - triangle->p2;
                    Vector2 n01 = {-e01.y, e01.x}; // Normal vector to first direction
                    Vector2 n12 = {-e12.y, e12.x};
                    Vector2 n20 = {-e20.y, e20.x};
                    bool d1 = dot(q1, n01) >= 0;
                    bool d2 = dot(q2, n12) >= 0;
                    bool d3 = dot(q3, n20) >= 0;
                    bool allPos = d1 && d2 && d3;
                    bool allNeg = !d1 && !d2 && !d3;
                    if (allPos || allNeg) { // If the point is in the positive half of all half-planes,
                        img(x, y) = finalColor; // The pixel at coordinate x,y is the triangle's color
                    }
                }
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
