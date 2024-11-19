PhyAnim
===============================================
Physic based animation library


## Building

PhyAnim is developed, maintained and run on Linux-based operating systems, being tested mainly on Ubuntu. The following platforms and build environments have been tested:

* Linux: Ubuntu 20.04, Ubuntu 22.04

### System dependencies

The following components must be installed on the system where PhyAnim will be built:

* GCC 7.5 or higher (Requires C++ 17 support)
* CMake 3.1 or higher
* Make or Ninja build systems
* Git
* Package config
* OpenGL
* GLEW
* glm
* Eigen3

PhyAnim uses further dependencies, but it will download them by itself during build.

* Libigl  2.5.0 (https://github.com/libigl/libigl/releases/tag/v2.5.0)

Optionally, to build with full functionallity and examples, the following components are required.

* OpenMP
* GLFW3
* sonata
* MorphIO


### Build command

Once the given dependencies are installed, PhyAnim can be cloned and built as follows:

    $ git clone https://github.com/BlueBrain/PhyAnim.git
    $ cd PhyAnim && mkdir build && cd build
    $ cmake .. -DCMAKE_BUILD_TYPE=Release 
    $ make -j

This will build the core of PhyAnim and the examples.