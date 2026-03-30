# Pot-Generator
This is a proof of concept for generating a 3D model file for 3D printing.

## How it works
Using the on-screen controls, you can decide the size of a pot, and the different points where the width of the pot changes.
This will dynamically generate a pot using these values for 3D printing, which is displayed on the right hand side of the screen.
After clicking export, this will generate a '.obj' file as a plain text model to be used with other software, and an '.stl' file to be used for 3D printing.

## Libs
SDL3 (windowing and OpenGL framework)
dearimgui (GUI)
glm (simple header only math lib)

## The future
As this is a proof of concept with the language I'm most familiar with, I plan on porting this to Javascript using Three.js to allow imbedding into a website
