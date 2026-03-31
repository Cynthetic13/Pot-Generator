#include <SDL3/SDL.h>
#include <iostream>

static const char* BasePath = nullptr;
void InitializeAssetLoader() {
    BasePath = SDL_GetBasePath();
    std::cout << "Base Path: " << BasePath << std::endl;
}

SDL_GPUShader* LoadShader(SDL_GPUDevice* device, const char* shaderFilename, Uint32 samplerCount,
                        Uint32 uniformBufferCount, Uint32 storageBufferCount, Uint32 storageTextureCount);

struct Vertex {
    float x, y, z;      // vec3 position
    float r, g, b, a;   // vec4 color
};

static Vertex vertices[] {
    {0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
    { -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f },
    {0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f }
};

int main() {
    // Start SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialized! SDL_Error: %s", SDL_GetError());
        return -1;
    }

    // Attach to GPU
    SDL_GPUDevice* device = SDL_CreateGPUDevice(
        SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL,
        false,
        nullptr);
    if (!device) {
        SDL_Log("CreateDevice failed");
        return -1;
    }

    // Create Window
    SDL_Window* window = SDL_CreateWindow("Pot Gen", 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) {
        SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // Claim Window
    if (!SDL_ClaimWindowForGPUDevice(device, window)) {
        SDL_Log("Failed to claim window!");
        return -1;
    }

    // Create Renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        SDL_Log("Renderer could not be created! SDL_Error: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    InitializeAssetLoader();

    // Load our Shaders
    SDL_GPUShader* vertexShader = LoadShader(device, "assets/Base.vert", 0, 0, 0, 0);
    if (!vertexShader) {
        SDL_Log("Failed to create vertex shader!");
        return -1;
    }

    SDL_GPUShader* fragmentShader = LoadShader(device, "assets/Base.frag", 0, 0, 0, 0);
    if (!fragmentShader) {
        SDL_Log("Failed to create fragment shader!");
        return -1;
    }

    // Pipeline Creation
    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo{};

    SDL_GPUColorTargetDescription colorTarget{};
    colorTarget.format = SDL_GetGPUSwapchainTextureFormat(device, window);

    pipelineCreateInfo.target_info.color_target_descriptions = &colorTarget;
    pipelineCreateInfo.target_info.num_color_targets = 1;

    pipelineCreateInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;

    pipelineCreateInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipelineCreateInfo.vertex_shader = vertexShader;
    pipelineCreateInfo.fragment_shader = fragmentShader;

    SDL_GPUGraphicsPipeline* pipeline = nullptr;
    pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineCreateInfo);
    if (!pipeline) {
        SDL_Log("Failed to create fill pipeline!");
        return -1;
    }

    // Release Shaders
    SDL_ReleaseGPUShader(device, vertexShader);
    SDL_ReleaseGPUShader(device, fragmentShader);

    SDL_Event e;
    bool quit = false;

    // Main Loop
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        // Command Buffer
        SDL_GPUCommandBuffer* cmdbuf = SDL_AcquireGPUCommandBuffer(device);
        if (!cmdbuf) {
            SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
            return -1;
        }

        // Swapchain Texture
        SDL_GPUTexture* swapchainTexture;
        if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmdbuf, window, &swapchainTexture, nullptr, nullptr)) {
            SDL_Log("WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
            return -1;
        }

        if (swapchainTexture != nullptr) {
            SDL_GPUColorTargetInfo colorTargetInfo = { 0 };
            colorTargetInfo.texture = swapchainTexture;
            colorTargetInfo.clear_color = (SDL_FColor){ 0.0f, 0.0f, 0.0f, 1.0f };
            colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
            colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

            SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdbuf, &colorTargetInfo, 1, nullptr);

            SDL_BindGPUGraphicsPipeline(renderPass, pipeline);
            SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);

            SDL_EndGPURenderPass(renderPass);
        }

        SDL_SubmitGPUCommandBuffer(cmdbuf);
    }

    // Cleanup
    SDL_ReleaseGPUGraphicsPipeline(device, pipeline);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

SDL_GPUShader* LoadShader(SDL_GPUDevice* device, const char* shaderFilename, Uint32 samplerCount,
                        Uint32 uniformBufferCount, Uint32 storageBufferCount, Uint32 storageTextureCount) {
    SDL_GPUShaderStage stage;
    if (SDL_strstr(shaderFilename, ".vert")) { // Load Vertex Shader
        stage = SDL_GPU_SHADERSTAGE_VERTEX;
    } else if (SDL_strstr(shaderFilename, ".frag")) { // Load Fragment Shader
        stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    } else {
        SDL_Log("Invalid shader stage!");
        return nullptr;
    }

    char fullPath[256];
    SDL_GPUShaderFormat backendFormats = SDL_GetGPUShaderFormats(device);
    SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;
    const char* entrypoint;

    // Check backend of shader
    if (backendFormats & SDL_GPU_SHADERFORMAT_SPIRV) {
        SDL_snprintf(fullPath, sizeof(fullPath), "%s%s.spv", BasePath, shaderFilename);
        format = SDL_GPU_SHADERFORMAT_SPIRV;
        entrypoint = "main";
    } else if (backendFormats & SDL_GPU_SHADERFORMAT_MSL) {
        SDL_snprintf(fullPath, sizeof(fullPath), "%s%s.msl", BasePath, shaderFilename);
        format = SDL_GPU_SHADERFORMAT_MSL;
        entrypoint = "main0";
    } else if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL) {
        SDL_snprintf(fullPath, sizeof(fullPath), "%s%s.dxil", BasePath, shaderFilename);
        format = SDL_GPU_SHADERFORMAT_DXIL;
        entrypoint = "main";
    } else {
        SDL_Log("%s", "Unrecognized backend shader format!");
        return nullptr;
    }

    // Load the shader code
    size_t codeSize;
    void* code = SDL_LoadFile(fullPath, &codeSize);
    if (!code) {
        SDL_Log("Failed to load shader from disk! %s", fullPath);
        return nullptr;
    }

    SDL_GPUShaderCreateInfo shaderInfo = {
        .code_size = codeSize,
        .code = (const Uint8*) code,
        .entrypoint = entrypoint,
        .format = format,
        .stage = stage,
        .num_samplers = samplerCount,
        .num_storage_textures = storageTextureCount,
        .num_storage_buffers = storageBufferCount,
        .num_uniform_buffers = uniformBufferCount
    };

    // Create Shader
    SDL_GPUShader* shader = SDL_CreateGPUShader(device, &shaderInfo);
    if (!shader) {
        SDL_Log("Failed to create shader!");
        SDL_free(code);
        return nullptr;
    }

    // Cleanup and return shader
    SDL_free(code);
    return shader;
}