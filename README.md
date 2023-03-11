# Assignment 3: A Spline Tool
### Kanishk Mishra
### February 7, 2023

---

## Format:

> Language: C++ with `<GLFW/glfw3.h> <stdlib.h> <vector> <math.h>`

> Compiled: g++ spline.cpp -o spline -lglfw -lGL

> Run With: `./spline {screen_width} {screen_height}`
    
> Test With: `./spline 1000 800`

---

## Files and Functions:
 * spline.cpp
    - `struct Point`
    - `struct Node : Point`
    - `float bezier(float A, float c1, float c2, float B, float t)`  
         * Cubic Bezier curve equation
    - `int main(int argc, char *argv[])`

---

## Functionality:
This Program creates a simple window **with the Width and Height inputted via command line** with the title **A Spline Tool**. A **Viewing Volume** of **(0, inputted width, o inputted height)** is constructed to help with proper placement of the graphic elements. The window responds to **left clicks** and the **E key**, respectively creating and deleting a **spline** connecting nodes positions with **GL_LINE_STRIP**. New nodes are created on whichever end of the spline is closest to the cursor when the **Left Mouse Button** is clicked.

The spline is drawn with **200 lines segments per curve** using a manually implemented `bezier(float A, float c1, float c2, float B, float t)` function. There are **co-linear and equi-distant handle(s)** connected to each intermediate node, the two ends only have one handle. These points are stored in `struct Node : Point` where `Point` is an **(x, y) coordinate**. The nodes objects are **stored in a vector** recorded with handle2 being a conrol point of the **preceding** and handle1 being a control point of the **proceding** curves. **4x Multisampling** is implemented for anti-aliasing in order to smoothen the resulting curve. A blue **stipple line** is drawn connecteding the **handles to the node**, clearly indicating where the control points are located.

In order to further interact with the spline, we can manipulate the positions of the handle and nodes by **holding left-click**. This interaction is done in **two iterations** of the `while` loop using **Polling, Boolean and variables**. When the user left clicks, **a check is run** to see if the user clicked on any node or handle. If an item has been clicked a **boolean value is changed**, then **the vector index and type of object is recorded**; where **0** represents the node itself, **1** is handle1 and **2** means handle2. In the next iteration this check is skipped, instead **moving the item** recorded along with the handle(s). This goes on until a release is detected on the left mouse button, **changing a boolean value, returning to the normal checks**. If you make a mistake **E can be pressed** to clear the entire screen and start anew.