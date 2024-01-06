#include "hw1.h"
#include "hw1_scenes.h"
#include <cmath>
using namespace hw1;

Image3 hw_1_1(const std::vector<std::string> &params) {
    // Homework 1.1: render a circle at the specified
    // position, with the specified radius and color.

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

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Real distance = sqrt(pow(x + Real(0.5) - center.x, 2) + pow(y + Real(0.5) - center.y, 2));
            if(distance <= radius) {
                img(x, y) = color;
            } else {
                img(x, y) = Vector3(0.5, 0.5, 0.5);
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
    //Bounding Box Optimzation
    for(int y = 0; y < img.height; y++){
        for(int x = 0; x < img.width; x++){
            img(x, y) = Vector3(scene.background);
        }
    }
    for (int i = 0; i < scene.objects.size(); i++) {
        auto centerX = scene.objects[i].center[0];
        auto centerY = scene.objects[i].center[1];
        auto radius = scene.objects[i].radius;
        double Bbox[4];
        //xmin
        Bbox[0] = (centerX - radius < 0) ? 0 : (centerX - radius);
        //xmax
        Bbox[1] = (centerX + radius > 0) ? img.width : (centerX + radius);
        //ymin
        Bbox[2] = (centerY - radius < 0) ? 0 : (centerY - radius);
        //ymax
        Bbox[3] = (centerY + radius > 0) ? img.height : (centerY + radius);
        for(double y = Bbox[2]; y < Bbox[3]; y++) {
            for(double x = Bbox[0]; x < Bbox[1]; x++) {
                Real distance = sqrt(pow(x + Real(0.5) - centerX, 2) + pow(y + Real(0.5) - centerY, 2));
                if(distance <= radius){
                    img(x, y) = scene.objects[i].color;
                }
            }
        }
    }
    
    //normal implementation
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3(scene.background);
            for(int i = 0; i < scene.objects.size(); i++){
                Real distance = sqrt(pow(x + Real(0.5) - scene.objects[i].center[0], 2) + pow(y + Real(0.5) - scene.objects[i].center[1], 2));
                if(distance <= scene.objects[i].radius){
                    img(x, y) = scene.objects[i].color;
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

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3(scene.background);
            for (int i = 0; i < scene.shapes.size(); i++){
                const Shape& shape = scene.shapes[i];
                if (auto *circle = std::get_if<Circle>(&shape)) {
                    Real distance = sqrt(pow(x + Real(0.5) - circle->center[0], 2) + pow(y + Real(0.5) - circle->center[1], 2));
                    if (distance <= circle->radius){
                        img(x, y) = circle->color;
                    }
                } else if (auto *rectangle = std::get_if<Rectangle>(&shape)) {
                    if(x + Real(0.5) >= rectangle->p_min.x && x + Real(0.5) <= rectangle->p_max.x && y + Real(0.5) >= rectangle->p_min.y && y + Real(0.5) <= rectangle->p_max.y){
                        img(x, y) = rectangle->color;
                    }
                } else if (auto *triangle = std::get_if<Triangle>(&shape)) {
                   Vector2 q = Vector2(x + Real(0.5), y + Real(0.5)); //current pixel center
                   Vector2 v0 = triangle->p1 - triangle->p0; // p0 -> p1
                   Vector2 v1 = triangle->p2 - triangle->p1; // p1 -> p2
                   Vector2 v2 = triangle->p0 - triangle->p2; // p2 -> p0
                   //counter clockwise rotation to find normal vectors
                   Vector2 n0 = normalize(Vector2(v0.y, -v0.x));
                   Vector2 n1 = normalize(Vector2(v1.y, -v1.x));
                   Vector2 n2 = normalize(Vector2(v2.y, -v2.x));
                   //if q is in all three negative half planes
                   if((dot((q - triangle->p0), n0) < 0 && dot((q - triangle->p1), n1) < 0 && dot((q - triangle->p2), n2) < 0) ||
                   (dot((q - triangle->p0), n0) > 0 && dot((q - triangle->p1), n1) > 0 && dot((q - triangle->p2), n2) > 0)){
                    img(x, y) = triangle->color;
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
            img(x, y) = Vector3(scene.background);
            for(int i = 0; i < scene.shapes.size(); i++){
                const Shape& shape = scene.shapes[i];
                if(auto *circle = std::get_if<Circle>(&shape)){
                    Matrix3x3 T = circle->transform;
                    Vector3 q = T * Vector3(x + Real(0.5), y + Real(0.5), 1.0);
                    Vector2 pPrime = Vector2(q.x, q.y);
                    Real distance = sqrt(pow(pPrime.x - circle->center[0], 2) + pow(pPrime.y - circle->center[1], 2));
                    if (distance <= circle->radius){
                        img(x, y) = circle->color;
                    }
                } else if (auto *rectangle = std::get_if<Rectangle>(&shape)) {
                    Matrix3x3 T = rectangle->transform;
                    Vector3 q = T * Vector3(x + Real(0.5), y + Real(0.5), 1.0);
                    Vector2 pPrime = Vector2(q.x, q.y);
                    if(pPrime.x >= rectangle->p_min.x && pPrime.x <= rectangle->p_max.x && pPrime.y >= rectangle->p_min.y && pPrime.y <= rectangle->p_max.y){
                        img(x, y) = rectangle->color;
                    }
                } else if (auto *triangle = std::get_if<Triangle>(&shape)) {
                    Matrix3x3 T = triangle->transform;
                    Vector3 q = T * Vector3(x + Real(0.5), y + Real(0.5), 1.0);
                    Vector2 pPrime = Vector2(q.x, q.y);
                    Vector2 v0 = triangle->p1 - triangle->p0; // p0 -> p1
                    Vector2 v1 = triangle->p2 - triangle->p1; // p1 -> p2
                    Vector2 v2 = triangle->p0 - triangle->p2; // p2 -> p0
                    //counter clockwise rotation to find normal vectors
                    Vector2 n0 = normalize(Vector2(v0.y, -v0.x));
                    Vector2 n1 = normalize(Vector2(v1.y, -v1.x));
                    Vector2 n2 = normalize(Vector2(v2.y, -v2.x));
                    //if q is in all three negative half planes
                    if((dot((pPrime - triangle->p0), n0) < 0 && dot((pPrime - triangle->p1), n1) < 0 && dot((pPrime - triangle->p2), n2) < 0) ||
                   (dot((pPrime - triangle->p0), n0) > 0 && dot((pPrime - triangle->p1), n1) > 0 && dot((pPrime - triangle->p2), n2) > 0)){
                        img(x, y) = triangle->color;
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

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            std::vector<Vector3> colors;
            for(int i = 0; i < 4; i++) {
                for(int j = 0; j < 4; j++) {
                    Vector3 currColor = scene.background;
                    for(int k = 0; k < scene.shapes.size(); k++){
                        const Shape& shape = scene.shapes[k];
                        if(auto *circle = std::get_if<Circle>(&shape)){
                            Matrix3x3 T = circle->transform;
                            Vector3 q = T * Vector3(x + Real(0.125) + i/4.0, y + Real(0.125) + j/4.0, 1.0);
                            Vector2 pPrime = Vector2(q.x, q.y);
                            Real distance = sqrt(pow(pPrime.x - circle->center[0], 2) + pow(pPrime.y  - circle->center[1], 2));
                            if (distance <= circle->radius){
                                currColor = circle->color;
                            }
                        } else if (auto *rectangle = std::get_if<Rectangle>(&shape)) {
                            Matrix3x3 T = rectangle->transform;
                            Vector3 q = T * Vector3(x + Real(0.125) + i/4.0, y + Real(0.125) + j/4.0, 1.0);
                            Vector2 pPrime = Vector2(q.x, q.y);
                            if(pPrime.x >= rectangle->p_min.x && pPrime.x <= rectangle->p_max.x && pPrime.y >= rectangle->p_min.y && pPrime.y <= rectangle->p_max.y){
                                currColor = rectangle->color;
                            }
                        } else if (auto *triangle = std::get_if<Triangle>(&shape)) {
                            Matrix3x3 T = triangle->transform;
                            Vector3 q = T * Vector3(x + Real(0.125) + i/4.0, y + Real(0.125) + j/4.0, 1.0);
                            Vector2 pPrime = Vector2(q.x, q.y);
                            Vector2 v0 = triangle->p1 - triangle->p0; // p0 -> p1
                            Vector2 v1 = triangle->p2 - triangle->p1; // p1 -> p2
                            Vector2 v2 = triangle->p0 - triangle->p2; // p2 -> p0
                            //counter clockwise rotation to find normal vectors
                            Vector2 n0 = normalize(Vector2(v0.y, -v0.x));
                            Vector2 n1 = normalize(Vector2(v1.y, -v1.x));
                            Vector2 n2 = normalize(Vector2(v2.y, -v2.x));
                            //if q is in all three negative half planes
                            if((dot((pPrime - triangle->p0), n0) < 0 && dot((pPrime - triangle->p1), n1) < 0 && dot((pPrime - triangle->p2), n2) < 0) ||
                            (dot((pPrime - triangle->p0), n0) > 0 && dot((pPrime - triangle->p1), n1) > 0 && dot((pPrime - triangle->p2), n2) > 0)){
                                currColor = triangle->color;
                            }
                        }
                    }
                    colors.push_back(currColor);
                }
            }
            Vector3 avgColor = Vector3(0, 0, 0);
            for(int i = 0; i < colors.size(); i++) {
                avgColor += colors[i];
            }
            img(x, y) = (avgColor / Real(16));
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

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            std::vector<Vector3> colors;
            for(int i = 0; i < 4; i++) {
                for(int j = 0; j < 4; j++) {
                    Vector3 currColor = scene.background;
                    for(int k = 0; k < scene.shapes.size(); k++){
                        const Shape& shape = scene.shapes[k];
                        if(auto *circle = std::get_if<Circle>(&shape)){
                            Matrix3x3 T = circle->transform;
                            Vector3 q = T * Vector3(x + Real(0.125) + i/4.0, y + Real(0.125) + j/4.0, 1.0);
                            Vector2 pPrime = Vector2(q.x, q.y);
                            Real distance = sqrt(pow(pPrime.x - circle->center[0], 2) + pow(pPrime.y  - circle->center[1], 2));
                            if (distance <= circle->radius){
                                currColor = (circle->color * circle->alpha) + (currColor * (1 - circle->alpha));
                            }
                        } else if (auto *rectangle = std::get_if<Rectangle>(&shape)) {
                            Matrix3x3 T = rectangle->transform;
                            Vector3 q = T * Vector3(x + Real(0.125) + i/4.0, y + Real(0.125) + j/4.0, 1.0);
                            Vector2 pPrime = Vector2(q.x, q.y);
                            if(pPrime.x >= rectangle->p_min.x && pPrime.x <= rectangle->p_max.x && pPrime.y >= rectangle->p_min.y && pPrime.y <= rectangle->p_max.y){
                                currColor = (rectangle->color * rectangle->alpha) + (currColor * (1 - rectangle->alpha));
                            }
                        } else if (auto *triangle = std::get_if<Triangle>(&shape)) {
                            Matrix3x3 T = triangle->transform;
                            Vector3 q = T * Vector3(x + Real(0.125) + i/4.0, y + Real(0.125) + j/4.0, 1.0);
                            Vector2 pPrime = Vector2(q.x, q.y);
                            Vector2 v0 = triangle->p1 - triangle->p0; // p0 -> p1
                            Vector2 v1 = triangle->p2 - triangle->p1; // p1 -> p2
                            Vector2 v2 = triangle->p0 - triangle->p2; // p2 -> p0
                            //counter clockwise rotation to find normal vectors
                            Vector2 n0 = normalize(Vector2(v0.y, -v0.x));
                            Vector2 n1 = normalize(Vector2(v1.y, -v1.x));
                            Vector2 n2 = normalize(Vector2(v2.y, -v2.x));
                            //if q is in all three negative half planes
                            if((dot((pPrime - triangle->p0), n0) < 0 && dot((pPrime - triangle->p1), n1) < 0 && dot((pPrime - triangle->p2), n2) < 0) ||
                            (dot((pPrime - triangle->p0), n0) > 0 && dot((pPrime - triangle->p1), n1) > 0 && dot((pPrime - triangle->p2), n2) > 0)){
                                currColor = (triangle->color * triangle->alpha) + (currColor * (1 - triangle->alpha));
                            }
                        }
                    }
                    colors.push_back(currColor);
                }
            }
            Vector3 avgColor = Vector3(0, 0, 0);
            for(int i = 0; i < colors.size(); i++) {
                avgColor += colors[i];
            }
            img(x, y) = (avgColor / Real(16));
        }
    }
    return img;
}
