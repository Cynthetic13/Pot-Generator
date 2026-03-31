#include "BackendSDL.h"

int BackendSDL::AppInit(uint32_t WINDOW_WIDTH, uint32_t WINDOW_HEIGHT) {
 // Cache Vars
 WIDTH = WINDOW_WIDTH;
 HEIGHT = WINDOW_HEIGHT;

 // Init SDL
 if (SDL_Init(SDL_INIT_VIDEO) < 0) {
  SDL_Log("SDL could not initialize: %s", SDL_GetError());
  return -1;
 }

 // Create Window
 window = SDL_CreateWindow("Flowerpot Generator", WIDTH,  HEIGHT, SDL_WINDOW_RESIZABLE);
 if (!window) {
  SDL_Log("Failed to create window: %s", SDL_GetError());
  return -1;
 }

 // Create Device
 device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, false, "vulkan");
 if (!device) {
  SDL_Log("Failed to create GPU device: %s", SDL_GetError());
  return -1;
 }

 SDL_ClaimWindowForGPUDevice(device, window);

 // Load Vertex Shader
 size_t vertexCodeSize;
 void* vertexCode = SDL_LoadFile("assets/Base.vert.spv", &vertexCodeSize);

 // Create Vertex Shader
 SDL_GPUShaderCreateInfo vertexInfo {};
 vertexInfo.code = static_cast<Uint8*>(vertexCode);
 vertexInfo.code_size = vertexCodeSize;
 vertexInfo.entrypoint = "main";
 vertexInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
 vertexInfo.stage = SDL_GPU_SHADERSTAGE_VERTEX;
 vertexInfo.num_samplers = 0;
 vertexInfo.num_storage_buffers = 0;
 vertexInfo.num_uniform_buffers = 0;

 SDL_GPUShader* vertexShader = SDL_CreateGPUShader(device, &vertexInfo);

 // Free File
 SDL_free(vertexCode);

 // Load Fragment Shader
 size_t fragmentCodeSize;
 void* fragmentCode = SDL_LoadFile("assets/Base.frag.spv", &fragmentCodeSize);

 // Create Vertex Shader
 SDL_GPUShaderCreateInfo fragmentInfo {};
 fragmentInfo.code = (Uint8*)fragmentCode;
 fragmentInfo.code_size = fragmentCodeSize;
 fragmentInfo.entrypoint = "main";
 fragmentInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
 fragmentInfo.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
 fragmentInfo.num_samplers = 0;
 fragmentInfo.num_storage_buffers = 0;
 fragmentInfo.num_uniform_buffers = 0;

 SDL_GPUShader* fragmentShader = SDL_CreateGPUShader(device, &fragmentInfo);

 // Free File
 SDL_free(fragmentCode);

 // Create Graphics Pipeline Info
 SDL_GPUGraphicsPipelineCreateInfo pipelineInfo {};
 pipelineInfo.vertex_shader = vertexShader;
 pipelineInfo.fragment_shader = fragmentShader;
 pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

 // Create Vertex Buffer Description
 SDL_GPUVertexBufferDescription vertexBufferDescription[1];
 vertexBufferDescription[0].slot = 0;
 vertexBufferDescription[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
 vertexBufferDescription[0].instance_step_rate = 0;
 vertexBufferDescription[0].pitch = sizeof(Vertex);

 pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
 pipelineInfo.vertex_input_state.vertex_buffer_descriptions = vertexBufferDescription;

 // Describe Vertex Attrib
 SDL_GPUVertexAttribute vertexAttributes[2];

 // Position
 vertexAttributes[0].buffer_slot = 0;
 vertexAttributes[0].location = 0;
 vertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
 vertexAttributes[0].offset = 0;

 // Color
 vertexAttributes[1].buffer_slot = 0;
 vertexAttributes[1].location = 1;
 vertexAttributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
 vertexAttributes[1].offset = sizeof(float) * 3;

 pipelineInfo.vertex_input_state.num_vertex_attributes = 2;
 pipelineInfo.vertex_input_state.vertex_attributes = vertexAttributes;

 // Create Color Target Description
 SDL_GPUColorTargetDescription colorTargetDescriptions[1];
 colorTargetDescriptions[0] = {};
 colorTargetDescriptions[0].blend_state.enable_blend = true;
 colorTargetDescriptions[0].blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
 colorTargetDescriptions[0].blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
 colorTargetDescriptions[0].blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
 colorTargetDescriptions[0].blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
 colorTargetDescriptions[0].blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
 colorTargetDescriptions[0].blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
 colorTargetDescriptions[0].format = SDL_GetGPUSwapchainTextureFormat(device, window);

 pipelineInfo.target_info.num_color_targets = 1;
 pipelineInfo.target_info.color_target_descriptions = colorTargetDescriptions;

 // Create the Pipeline
 graphicsPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);

 // Release Shaders
 SDL_ReleaseGPUShader(device, vertexShader);
 SDL_ReleaseGPUShader(device, fragmentShader);

 // Create Vertex Buffer
 SDL_GPUBufferCreateInfo bufferInfo {};
 bufferInfo.size = meshDataSize;
 bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
 vertexBuffer = SDL_CreateGPUBuffer(device, &bufferInfo);

 // Create Transfer Buffer
 SDL_GPUTransferBufferCreateInfo transferInfo {};
 transferInfo.size = meshDataSize;
 transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
 transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);

 // Fill Transfer Buffer
 Vertex* data = (Vertex*)SDL_MapGPUTransferBuffer(device, transferBuffer, false);

 SDL_memcpy(data, meshData.data(), meshDataSize);

 SDL_UnmapGPUTransferBuffer(device, transferBuffer);

 // Start Copy Pass
 SDL_GPUCommandBuffer* cmdbuf = SDL_AcquireGPUCommandBuffer(device);
 SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmdbuf);

 // Get Data
 SDL_GPUTransferBufferLocation location {};
 location.transfer_buffer = transferBuffer;
 location.offset = 0;

 // Where to Upload Data
 SDL_GPUBufferRegion region {};
 region.buffer = vertexBuffer;
 region.size = meshDataSize;
 region.offset = 0;

 // Upload Data
 SDL_UploadToGPUBuffer(copyPass, &location, &region, true);

 // End Copy Pass
 SDL_EndGPUCopyPass(copyPass);
 SDL_SubmitGPUCommandBuffer(cmdbuf);

 return 0;
}
int BackendSDL::AppUpdate() {
 while (!quit) {
  if (SDL_PollEvent(&e)) {
   if (e.type == SDL_EVENT_QUIT) {
    quit = true;
   }
  }

  SDL_GPUCommandBuffer* cmdbuf = SDL_AcquireGPUCommandBuffer(device);

  SDL_GPUTexture* swapchainTexture;
  SDL_WaitAndAcquireGPUSwapchainTexture(cmdbuf, window, &swapchainTexture, &WIDTH, &HEIGHT);

  if (!swapchainTexture) {
   SDL_SubmitGPUCommandBuffer(cmdbuf);
   return 0;
  }

  // Create Color Target
  SDL_GPUColorTargetInfo colorTargetInfo {};
  colorTargetInfo.clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
  colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
  colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
  colorTargetInfo.texture = swapchainTexture;

  // Begin Render/Copy Pass
  SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdbuf, &colorTargetInfo, 1, nullptr);

  // Bind Graphics Pipeline
  SDL_BindGPUGraphicsPipeline(renderPass, graphicsPipeline);

  // Bind Vertex Buffer
  SDL_GPUBufferBinding bufferBindings[1];
  bufferBindings[0].buffer = vertexBuffer;
  bufferBindings[0].offset = 0;

  SDL_BindGPUVertexBuffers(renderPass, 0, bufferBindings, 1);

  // Issue Draw Call
  int vertexCount = meshData.size();
  int triangleCount = vertexCount -2;

  SDL_DrawGPUPrimitives(renderPass, triangleCount * 3, 1, 0, 0);

  // End Render Pass
  SDL_EndGPURenderPass(renderPass);


  // Submit Command Buffer
  SDL_SubmitGPUCommandBuffer(cmdbuf);
 }
 return 0;
}

int BackendSDL::AppQuit() {
 // Release Buffers
 SDL_ReleaseGPUBuffer(device, vertexBuffer);
 SDL_ReleaseGPUTransferBuffer(device, transferBuffer);

 // Release GPU Pipeline
 SDL_ReleaseGPUGraphicsPipeline(device, graphicsPipeline);

 // Destroy GPU Device
 SDL_DestroyGPUDevice(device);

 // Destroy Window
 SDL_DestroyWindow(window);

 // Shutdown SDL
 SDL_Quit();

 return 0;
}

int BackendSDL::SubmitMesh(std::vector<Vertex>& vertexData) {
 meshData = vertexData;
 meshDataSize = meshData.size() * sizeof(Vertex);
 return 0;
}