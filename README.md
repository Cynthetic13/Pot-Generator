# Flower Pot Generator
This is a proof of concept for dynamically generating a 3D flower pot for 3D printing.

## How it works
Using the on-screen controls, you can decide the size of a pot, and the different points where the width of the pot changes.

This will dynamically generate a pot using these values for 3D printing, which is displayed on the right hand side of the screen.

After clicking export, this will generate a '.obj' file as a plain text model to be used with 3D software, and an '.stl' file to be used for 3D printing.

## Tech Specs
- C++ 26
- Windows and Linux Crossplatform Support

## Task List
- [x] SDL3 Setup
- [ ] Render Base Pot
- [ ] Implement dearimgui
- [ ] Allow Adjusting the Pot in Realtime
- [ ] Export Function to .obj
- [ ] Export Function to .stl
- [ ] Support custom shapes
- [ ] WebGL or similar for rendering backend (SDL3 currently doesn't support Web)

## Custom Shapes
- [ ] Flat Top
- [ ] Rounded Top
- [ ] Cube Shape
- [ ] Circle Bottom
- [ ] Preset Shapes (Herringbone Pattern, Triangulated Pattern, Weave Pattern, etc)
- [ ] Texture loading (generate a heightmap from a texture to use on the surface of the mesh)

## Under Consideration
- [ ] Procedural Texture Generation (more advanced feature for creative indviduals, and allows more flexibility, but might be hard to properly stress test)
- [ ] Built-in stress testing to match the 3D printing overhand specs (generates a heatmap that will show the user where the hardest parts of the print will be, red will make it not usable)
- [ ] Proper layering to represent the final product (concrete layers more than plastic 3D printing)
- [ ] Albedo and Normal Texture Maps to allow the user to see something similar to what the final product will look like (UV unwrapping it might be the complex part)
- [ ] Cube Maps or something similar to give a person an idea of what it might look like in different environments (requires PBR shader and properly sourced PBR texture maps)
- [ ] Multi-threading (I don't think this is needed since it's a single object, but it might be nice to offload either rendering or generation onto another thread, or multi-threaded)

## The future
This is only a proof of concept in C++, i will either port this to Javascript using Three.js, or compile it to WebAssembly as a library for Javascript/Node.js

# Building
## Linux
Requires:
`SDL3`
`cmake`

Optional:
If Building Shaders: `sdl3_shadercross`

### Arch
**Required:**
```
sudo pacman -Syu
sudo pacman -S --needed base-devel cmake sdl3
```

**Optional:**
```
yay -S sdl3_shadercross-git
```

### Ubuntu/Debian
**Required:**
```
sudo apt update
sudo apt install build-essential cmake libsdl3-dev
```

**Optional:** Build with CMake [SDL_shadercross Github](github.com/libsdl-org/SDL_shadercross)
> [!NOTE]
> This has only been tested on Arch Linux


**Get Source Code:**
```
git clone https://github.com/Cynthetic13/Pot-Generator.git
cd Pot-Generator
```

**Building:**
```
cmake .
````

## Windows
TODO...
