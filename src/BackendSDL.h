#ifndef POT_GENERATOR_BACKEND_SDL_H
#define POT_GENERATOR_BACKEND_SDL_H

#include "IBackend.h"
#include <SDL3/SDL.h>
#include <vector>

class BackendSDL : public IBackend {
private:
    SDL_Window* window;
    SDL_GPUDevice* device;
    SDL_GPUBuffer* vertexBuffer;
    SDL_GPUTransferBuffer* transferBuffer;
    SDL_GPUGraphicsPipeline* graphicsPipeline;
    SDL_Event e;

    Uint32 WIDTH = 0;
    Uint32 HEIGHT = 0;
    bool quit = false;


    size_t meshDataSize = 0;
    std::vector<Vertex> meshData;
public:
    int AppInit(uint32_t WINDOW_WIDTH, uint32_t WINDOW_HEIGHT) override;
    int AppUpdate() override;
    int AppQuit() override;
    int SubmitMesh(Vertex vertexData[], size_t size) override;
};

#endif //POT_GENERATOR_BACKEND_SDL_H