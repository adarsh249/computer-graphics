#include "hw2.h"
#include "hw2_scenes.h"
using namespace hw2;

Image3 hw_2_1(const std::vector<std::string> &params) {
    // Homework 2.1: render a single 3D triangle

    Image3 img(640 /* width */, 480 /* height */);

    Vector3 p0{0, 0, -1};
    Vector3 p1{1, 0, -1};
    Vector3 p2{0, 1, -1};
    Real s = 1; // scaling factor of the view frustrum
    Vector3 color = Vector3{1.0, 0.5, 0.5};
    Real z_near = 1e-6; // distance of the near clipping plane
    Real a = Real(img.width)/Real(img.height);
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
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            std::vector<Vector3> colors;
            for(int i = 0; i < 4; i++) {
                for(int j = 0; j < 4; j++) {
                    Vector3 currColor = Vector3{0.5, 0.5, 0.5};
                    Vector2 p0p = Vector2(-p0.x/p0.z, -p0.y/p0.z);
                    Vector2 p1p = Vector2(-p1.x/p1.z, -p1.y/p1.z);
                    Vector2 p2p = Vector2(-p2.x/p2.z, -p2.y/p2.z);

                    Vector2 p0s = Vector2(img.width*((p0p.x + (s * a))/(2*s*a)), img.height*(1.0 - (p0p.y + s)/(2 * s)));
                    Vector2 p1s = Vector2(img.width*((p1p.x + (s * a))/(2*s*a)), img.height*(1.0 - (p1p.y + s)/(2 * s)));
                    Vector2 p2s = Vector2(img.width*((p2p.x + (s * a))/(2*s*a)), img.height*(1.0 - (p2p.y + s)/(2 * s)));
                    Vector2 q = Vector2(x + Real(0.125) + i/4.0, y + Real(0.125) + j/4.0);
                    Vector2 v0 = p1s - p0s; // p0 -> p1
                    Vector2 v1 = p2s - p1s; // p1 -> p2
                    Vector2 v2 = p0s - p2s; // p2 -> p0
                    //counter clockwise rotation to find normal vectors
                    Vector2 n0 = normalize(Vector2(v0.y, -v0.x));
                    Vector2 n1 = normalize(Vector2(v1.y, -v1.x));
                    Vector2 n2 = normalize(Vector2(v2.y, -v2.x));
                    //if q is in all three negative half planes or positive half planes
                    if((dot((q - p0s), n0) < 0 && dot((q - p1s), n1) < 0 && dot((q - p2s), n2) < 0) ||
                    (dot((q - p0s), n0) > 0 && dot((q - p1s), n1) > 0 && dot((q - p2s), n2) > 0)){
                        currColor = color;
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
    Real a = Real(img.width)/Real(img.height);
    TriangleMesh mesh = meshes[scene_id];
    //std::vector<std::vector<Real>> z_min(img.height * 4, std::vector<Real>(img.width*4, std::numeric_limits<Real>::infinity()));
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            std::vector<Vector3> colors;
            for(int i = 0; i < 4; i++) {
                for(int j = 0; j < 4; j++) {
                    Vector3 currColor = Vector3(0.5, 0.5, 0.5);
                    Real z_min = -std::numeric_limits<Real>::infinity();
                    for(int k = 0; k < mesh.faces.size(); k++) {
                        //get OG vertices
                        Vector3 p0 = Vector3(mesh.vertices[mesh.faces[k].x]);
                        Vector3 p1 = Vector3(mesh.vertices[mesh.faces[k].y]);
                        Vector3 p2 = Vector3(mesh.vertices[mesh.faces[k].z]);

                        //project vertices
                        Vector2 p0p = Vector2(-p0.x/p0.z, -p0.y/p0.z);
                        Vector2 p1p = Vector2(-p1.x/p1.z, -p1.y/p1.z);
                        Vector2 p2p = Vector2(-p2.x/p2.z, -p2.y/p2.z);
                        //screen space
                        Vector2 p0s = Vector2(img.width*((p0p.x + (s * a))/(2*s*a)), img.height*(1.0 - (p0p.y + s)/(2 * s)));
                        Vector2 p1s = Vector2(img.width*((p1p.x + (s * a))/(2*s*a)), img.height*(1.0 - (p1p.y + s)/(2 * s)));
                        Vector2 p2s = Vector2(img.width*((p2p.x + (s * a))/(2*s*a)), img.height*(1.0 - (p2p.y + s)/(2 * s)));

                        Vector2 q = Vector2(x + Real(0.125) + i/4.0, y + Real(0.125) + j/4.0);
                        Vector3 p = Vector3(q.x, q.y, 0.0);

                        Vector2 v0 = p1s - p0s; // p0 -> p1
                        Vector2 v1 = p2s - p1s; // p1 -> p2
                        Vector2 v2 = p0s - p2s; // p2 -> p0

                        //counter clockwise rotation to find normal vectors
                        Vector2 n0 = normalize(Vector2(v0.y, -v0.x));
                        Vector2 n1 = normalize(Vector2(v1.y, -v1.x));
                        Vector2 n2 = normalize(Vector2(v2.y, -v2.x));
                        //if q is in all three negative half planes or positive half planes
                        if((dot((q - p0s), n0) < 0 && dot((q - p1s), n1) < 0 && dot((q - p2s), n2) < 0) ||
                        (dot((q - p0s), n0) > 0 && dot((q - p1s), n1) > 0 && dot((q - p2s), n2) > 0)) {
                            //convert current pixel center from screen to camera
                            //Vector3 pC = Vector3((Real(p.x*2*s*a)/Real(img.width))-Real(s*a), 2*s*(1-(Real(p.y)/Real(img.height)))-s, 0.0);
                            //screenspace converted to Vector3
                            Vector3 p0temp = Vector3(p0s.x, p0s.y, 0.0);
                            Vector3 p1temp = Vector3(p1s.x, p1s.y, 0.0);
                            Vector3 p2temp = Vector3(p2s.x, p2s.y, 0.0);
                            //projected barycentric
                            Real bDenom = Real(length(cross(p1temp - p0temp, p2temp - p0temp))) / Real(2);
                            Real b0p = Real(length(cross(p1temp - p, p2temp - p)) / Real(2))/bDenom;
                            Real b1p = Real(length(cross(p - p0temp, p2temp - p0temp)) / Real(2))/bDenom;
                            Real b2p = Real(length(cross(p1temp - p0temp, p - p0temp)) / Real(2))/bDenom;
                            //original barycentric
                            Real bOGDenom = (b0p/p0.z) + (b1p/p1.z) + (b2p/p2.z);
                            Real b0 = (b0p/p0.z)/bOGDenom;
                            Real b1 = (b1p/p1.z)/bOGDenom;
                            Real b2 = (b2p/p2.z)/bOGDenom;
                            //get depth
                            Real depth = b0*p0.z + b1*p1.z + b2*p2.z;
                            //interpolate
                            if(depth >= z_min) {
                                z_min = depth;
                                currColor = mesh.face_colors[k];
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
    Real a = Real(img.width) / Real(img.height);
    TriangleMesh mesh = meshes[scene_id];

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            std::vector<Vector3> colors;
            for(int i = 0; i < 4; i++) {
                for(int j = 0; j < 4; j++) {
                    Vector3 currColor = Vector3(0.5, 0.5, 0.5);
                    Real z_min = -std::numeric_limits<Real>::infinity();
                    for(int k = 0; k < mesh.faces.size(); k++) {
                        //get OG vertices
                        Vector3 p0 = Vector3(mesh.vertices[mesh.faces[k].x]);
                        Vector3 p1 = Vector3(mesh.vertices[mesh.faces[k].y]);
                        Vector3 p2 = Vector3(mesh.vertices[mesh.faces[k].z]);

                        //project vertices
                        Vector2 p0p = Vector2(-p0.x/p0.z, -p0.y/p0.z);
                        Vector2 p1p = Vector2(-p1.x/p1.z, -p1.y/p1.z);
                        Vector2 p2p = Vector2(-p2.x/p2.z, -p2.y/p2.z);
                        //screen space
                        Vector2 p0s = Vector2(img.width*((p0p.x + (s * a))/(2*s*a)), img.height*(1.0 - (p0p.y + s)/(2 * s)));
                        Vector2 p1s = Vector2(img.width*((p1p.x + (s * a))/(2*s*a)), img.height*(1.0 - (p1p.y + s)/(2 * s)));
                        Vector2 p2s = Vector2(img.width*((p2p.x + (s * a))/(2*s*a)), img.height*(1.0 - (p2p.y + s)/(2 * s)));

                        Vector2 q = Vector2(x + Real(0.125) + i/4.0, y + Real(0.125) + j/4.0);
                        Vector3 p = Vector3(q.x, q.y, 0.0);

                        Vector2 v0 = p1s - p0s; // p0 -> p1
                        Vector2 v1 = p2s - p1s; // p1 -> p2
                        Vector2 v2 = p0s - p2s; // p2 -> p0

                        //counter clockwise rotation to find normal vectors
                        Vector2 n0 = normalize(Vector2(v0.y, -v0.x));
                        Vector2 n1 = normalize(Vector2(v1.y, -v1.x));
                        Vector2 n2 = normalize(Vector2(v2.y, -v2.x));
                        //if q is in all three negative half planes or positive half planes
                        if((dot((q - p0s), n0) < 0 && dot((q - p1s), n1) < 0 && dot((q - p2s), n2) < 0) ||
                        (dot((q - p0s), n0) > 0 && dot((q - p1s), n1) > 0 && dot((q - p2s), n2) > 0)) {
                            //convert current pixel center from screen to camera
                            //Vector3 pC = Vector3((Real(p.x*2*s*a)/Real(img.width))-Real(s*a), 2*s*(1-(Real(p.y)/Real(img.height)))-s, 0.0);
                            //screenspace converted to Vector3
                            Vector3 p0temp = Vector3(p0s.x, p0s.y, 0.0);
                            Vector3 p1temp = Vector3(p1s.x, p1s.y, 0.0);
                            Vector3 p2temp = Vector3(p2s.x, p2s.y, 0.0);
                            //projected barycentric
                            Real bDenom = Real(length(cross(p1temp - p0temp, p2temp - p0temp))) / Real(2);
                            Real b0p = Real(length(cross(p1temp - p, p2temp - p)) / Real(2))/bDenom;
                            Real b1p = Real(length(cross(p - p0temp, p2temp - p0temp)) / Real(2))/bDenom;
                            Real b2p = Real(length(cross(p1temp - p0temp, p - p0temp)) / Real(2))/bDenom;
                            //original barycentric
                            Real bOGDenom = (b0p/p0.z) + (b1p/p1.z) + (b2p/p2.z);
                            Real b0 = (b0p/p0.z)/bOGDenom;
                            Real b1 = (b1p/p1.z)/bOGDenom;
                            Real b2 = (b2p/p2.z)/bOGDenom;
                            //get depth
                            Real depth = b0*p0.z + b1*p1.z + b2*p2.z;
                            //interpolate
                            if(depth >= z_min) {
                                z_min = depth;
                                currColor = b0*mesh.vertex_colors[mesh.faces[k].x] + b1*mesh.vertex_colors[mesh.faces[k].y] + b2*mesh.vertex_colors[mesh.faces[k].z];
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

Image3 hw_2_4(const std::vector<std::string> &params) {
    // Homework 2.4: render a scene with transformation
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.camera.resolution.x,
               scene.camera.resolution.y);
    Real a = Real(img.width)/Real(img.height);
    Real s = scene.camera.s;
    Real z_near = scene.camera.z_near;
    //std::cout<<"A"<<a<<"\n";
    Matrix4x4 V = inverse(scene.camera.cam_to_world);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            std::vector<Vector3> colors;
            for(int i = 0; i < 4; i++) {
                for(int j = 0; j < 4; j++) {
                    for(int k = 0; k < scene.meshes.size(); k++) {
                        Matrix4x4 M = scene.meshes[k].model_matrix;
                        Vector3 currColor = scene.background;
                        Real z_min = -std::numeric_limits<Real>::infinity();
                        TriangleMesh mesh = scene.meshes[k];
                        for(int m = 0; m < mesh.faces.size(); m++) {
                            //get OG Vertices
                            Vector3 p0 = Vector3(mesh.vertices[mesh.faces[m].x]);
                            Vector3 p1 = Vector3(mesh.vertices[mesh.faces[m].y]);
                            Vector3 p2 = Vector3(mesh.vertices[mesh.faces[m].z]);
                            
                            //homogenous
                            Vector4 p0Prime = Vector4(p0.x, p0.y, p0.z, 1.0);
                            Vector4 p1Prime = Vector4(p1.x, p1.y, p1.z, 1.0);
                            Vector4 p2Prime = Vector4(p2.x, p2.y, p2.z, 1.0);
                            
                            //convert to world space
                            Vector4 p0m = M * p0Prime;
                            Vector4 p1m = M * p1Prime;
                            Vector4 p2m = M * p2Prime;
                            
                            //convert to camera space
                            Vector4 p0v = V * p0m;
                            Vector4 p1v = V * p1m;
                            Vector4 p2v = V * p2m;
                            //camera to projected
                            Vector4 p0p = Vector4(-p0v.x/p0v.z, -p0v.y/p0v.z, -1.0/p0v.z, 1.0);
                            Vector4 p1p = Vector4(-p1v.x/p1v.z, -p1v.y/p1v.z, -1.0/p1v.z, 1.0);
                            Vector4 p2p = Vector4(-p2v.x/p2v.z, -p2v.y/p2v.z, -1.0/p2v.z, 1.0);
                            
                            //projected to screen
                            Vector2 p0s = Vector2(img.width*((p0p.x + (s * a))/Real(2*s*a)), img.height*(1.0 - (p0p.y + s)/Real(2 * s)));
                            Vector2 p1s = Vector2(img.width*((p1p.x + (s * a))/Real(2*s*a)), img.height*(1.0 - (p1p.y + s)/Real(2 * s)));
                            Vector2 p2s = Vector2(img.width*((p2p.x + (s * a))/Real(2*s*a)), img.height*(1.0 - (p2p.y + s)/Real(2 * s)));

                            Vector2 q = Vector2(x + Real(0.125) + i/4.0, y + Real(0.125) + j/4.0); //current pixel center
                            Vector3 p = Vector3(q.x, q.y, 0.0);

                            Vector2 v0 = p1s - p0s; // p0 -> p1
                            Vector2 v1 = p2s - p1s; // p1 -> p2
                            Vector2 v2 = p0s - p2s; // p2 -> p0
                            //counter clockwise rotation to find normal vectors
                            Vector2 n0 = normalize(Vector2(v0.y, -v0.x));
                            Vector2 n1 = normalize(Vector2(v1.y, -v1.x));
                            Vector2 n2 = normalize(Vector2(v2.y, -v2.x));
                            
                            //if q is in all three negative half planes or positive half planes
                            if((dot((q - p0s), n0) < 0 && dot((q - p1s), n1) < 0 && dot((q - p2s), n2) < 0) ||
                            (dot((q - p0s), n0) > 0 && dot((q - p1s), n1) > 0 && dot((q - p2s), n2) > 0)) {
                                //screenspace converted to Vector3
                                Vector3 p0temp = Vector3(p0s.x, p0s.y, 0.0);
                                Vector3 p1temp = Vector3(p1s.x, p1s.y, 0.0);
                                Vector3 p2temp = Vector3(p2s.x, p2s.y, 0.0);
                                
                                //projected barycentric
                                Real bDenom = Real(length(cross(p1temp - p0temp, p2temp - p0temp))) / Real(2);
                                Real b0p = Real(length(cross(p1temp - p, p2temp - p)) / Real(2))/bDenom;
                                Real b1p = Real(length(cross(p - p0temp, p2temp - p0temp)) / Real(2))/bDenom;
                                Real b2p = Real(length(cross(p1temp - p0temp, p - p0temp)) / Real(2))/bDenom;
                                //original barycentric
                                Real bOGDenom = (b0p/p0v.z) + (b1p/p1v.z) + (b2p/p2v.z);
                                Real b0 = (b0p/p0v.z)/bOGDenom;
                                Real b1 = (b1p/p1v.z)/bOGDenom;
                                Real b2 = (b2p/p2v.z)/bOGDenom;
                                //get depth
                                Real depth = b0*p0v.z + b1*p1v.z + b2*p2v.z;
                                if(depth >= z_min) {
                                    z_min = depth;
                                    currColor = b0*mesh.vertex_colors[mesh.faces[m].x] + b1*mesh.vertex_colors[mesh.faces[m].y] + b2*mesh.vertex_colors[mesh.faces[m].z];
                                }
                            }
                        }
                        colors.push_back(currColor);
                    }
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

