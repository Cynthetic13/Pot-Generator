#ifndef POT_GENERATOR_BACKEND_H
#define POT_GENERATOR_BACKEND_H

#include <iostream>
#include <vector>

class IBackend {
public:
    struct Vertex {
        float x, y, z;      // Position
        float r, g, b, a;   // Color
    };

    virtual int AppInit(uint32_t WINDOW_WIDTH, uint32_t WINDOW_HEIGHT) = 0;
    virtual int AppUpdate() = 0;
    virtual int AppQuit() = 0;
    virtual int SubmitMesh(std::vector<Vertex>& vertexData) = 0;
};

#endif //POT_GENERATOR_BACKEND_H