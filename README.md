# sofren
A simple API which easily will setup a back buffer for CPU rendering and render it to a window with D3D9. See examples/main.cpp for example. demos/raycast.cpp implements Lode Vandevenne's textured raycaster program: lodev.org.

# Usage
Include sofren_d3d9.cpp, create a window in Win32, pass the HWND to D3D9SofRenInit. Call D3D9SofRenBegin which returns a screen buffer. Then you can do whatever rendering you want on that buffer. Call D3D9SofRenEnd and then call D3D9SofRenBlit to see results in the window. Call D3D9SofRenRelease on program exit.
