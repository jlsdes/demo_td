# Demo TD
A simple tower defence game made using OpenGL.

The main goal of this project is for me to learn more about graphics programming in generanl, and OpenGL specifically as well.
I am also just trying to practise programming, and learning various other things at the same time.

## Building

This project can be built using CMake (version >= 4.0).
Running CMake with the provided CMakeLists.txt file will automatically fetch the dependencies.
These may have their own dependencies and thus require some installs (e.g. I had to install `xorg-dev`).

Besides the C++ standard library, I am using the following libraries:
- [GLAD](https://github.com/ValtoLibraries/GLAD/tree/master)
- [GLFW](https://www.glfw.org/)
- [GLM](https://github.com/g-truc/glm)

I can build and run this project on my own computer, but I have not tried any other systems so I cannot give any guarantees there.
As mentioned above, the main goal of this project is for me to learn and not to start selling/distributing anything, so I won't be putting that much effort into supporting different systems.

## Currently

I have taken a short break, while also thinking about the general setup of the program.

Some thought:
- I quite like the Entity-Component-System architecture, even though my implementation does have some issues. I think it should suffice.
- Initialising/handling levels/menus isn't great at the moment. This is my current priority to figure out.
