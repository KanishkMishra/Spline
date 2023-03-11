#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <vector>
#include <math.h>

#define NUMLINE 200
#define NODESIZE 10

using namespace std;

// Assignment 3: A Spline Tool
// Kanishk Mishra
// February 7, 2023

// g++ spline.cpp -o spline -lglfw -lGL
// ./spline 1000 800

/* Implement data structure to save points */
struct Point {
    float x;
    float y;
};

/* Implement data structure to save Nodes */
struct Node : Point {
    bool hasHandle1, hasHandle2;
    Point handle1;
    Point handle2;
};

/* Implement data structure to save vector of Nodes */
vector<Node> nodes;

/* Calculate Bezier curve */
float bezier(float A, float c1, float c2, float B, float t)
{
    if(t < 0)
    {
        t = 0;
    } 
    else if(t > 1)
    {
        t = 1;
    }

    return powf(1 - t, 3)*A + 3*powf(1 - t, 2)*t*c1 + 3*(1 - t)*powf(t, 2)*c2 + powf(t, 3)*B;
}

int main(int argc, char *argv[])
{
    /* Assign command-line values */
    int screen_width = atoi(argv[1]);
    int screen_height = atoi(argv[2]);

    GLFWwindow* window;
    
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(screen_width, screen_height, "A Spline Tool", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Specify Viewing Volume */
    glViewport(0, 0, screen_width, screen_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, screen_width, 0, screen_height, -1, 1);

    /* Implement Anti-Aliasing */
    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);

    /* record mouse position and if mouse held */
    double m_x;
    double m_y;

    /* keep track of clicks */
    bool click = false;
    int nNode;
    int itemType;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();

		/* Render here */
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        /* Render the spline */
        glLineWidth(2);
            /* smoothen lines */
        glDisable(GL_LINE_STIPPLE);            
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBegin(GL_LINE_STRIP);

            /* set colour */
            glColor3f(0, 0, 0);

            /* plot all splines */
            if (nodes.size() > 1)
            {
                for (vector<Node>::size_type i = 0, n = nodes.size(); i < (n - 1); i++)
                {
                    Node start = nodes[i];
                    Node end = nodes[i + 1];
                    float T;

                    /* plot curve of 200 line segments */
                    for(float t = 1; t <= NUMLINE; t++)
                    {
                        T = t/NUMLINE;
                        glVertex2f(bezier(start.x, start.handle1.x, end.handle2.x, end.x, T), bezier(start.y, start.handle1.y, end.handle2.y, end.y, T));
                    }
                }
            }
        glEnd();


        /* emphasize the position of each node */
        glDisable(GL_POINT_SMOOTH);
        glPointSize(NODESIZE);
        glBegin(GL_POINTS);
            /* set colour */
            glColor3f(0, 0, 0);

            /* plot all nodes */
            for (vector<Node>::size_type i = 0, n = nodes.size(); i < n; i++)
            {  
                Node node = nodes[i];
                glVertex2f(node.x, node.y);
            }
        glEnd();


        /* emphasizes each control point */
        glPointSize(NODESIZE);
            /* round points */
        glEnable(GL_POINT_SMOOTH);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBegin(GL_POINTS);
            /* set colour */
            glColor3f(0, 0, 200);

            /* plot all handles */
            for (vector<Node>::size_type i = 0, n = nodes.size(); i < n; i++)
            {
                Node node = nodes[i];

                /* check if handles exist */
                if (node.hasHandle1)
                    glVertex2f(node.handle1.x, node.handle1.y);

                if (node.hasHandle2)
                    glVertex2f(node.handle2.x, node.handle2.y);
            }
        glEnd();


        /* Connect Handles with Nodes */
        glLineWidth(1);
        glDisable(GL_LINE_SMOOTH);
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(1, 0xAAAA);

        glBegin(GL_LINES);
            /* plot all Lines */
            glColor3f(0, 0, 200);

            for (vector<Node>::size_type i = 0, n = nodes.size(); i < n; i++)
            {
                Node node = nodes[i];

                /* check if handles exist */
                if (node.hasHandle1)
                {
                    glVertex2f(nodes[i].handle1.x, nodes[i].handle1.y);
                    glVertex2f(node.x, node.y);
                }

                if (node.hasHandle2)
                {
                    glVertex2f(nodes[i].handle2.x, nodes[i].handle2.y);
                    glVertex2f(node.x, node.y);
                }
            }
        glEnd();


        /* Update Cursor Position */
        glfwGetCursorPos(window, &m_x, &m_y);
        m_y = screen_height - m_y;

        /* Check mouse inputs */
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        {
            /* return values to default */
            click = false;
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            /* move if held down */
            if (click)
            {
                Node node = nodes[nNode];
                switch (itemType) {
                    case 0:
                        /* move node */
                        nodes[nNode].x = m_x;
                        nodes[nNode].y = m_y;

                        /* move handles along-side node */
                        if (node.hasHandle1)
                        {
                            nodes[nNode].handle1.x += (m_x - node.x);
                            nodes[nNode].handle1.y += (m_y - node.y);
                        }

                        if (node.hasHandle2)
                        {
                            nodes[nNode].handle2.x += (m_x - node.x);
                            nodes[nNode].handle2.y += (m_y - node.y);
                        }
                        break;
                    case 1:
                        /* move handle */
                        nodes[nNode].handle1.x = m_x;
                        nodes[nNode].handle1.y = m_y;

                        /* move opposite handle */
                        if (node.hasHandle2)
                        {
                            nodes[nNode].handle2.x = (2*node.x - m_x);
                            nodes[nNode].handle2.y = (2*node.y - m_y);
                        }
                        break;
                    case 2:
                        /* move handle */
                        nodes[nNode].handle2.x = m_x;
                        nodes[nNode].handle2.y = m_y;

                        /* move opposite handle */
                        if (node.hasHandle1)
                        {
                            nodes[nNode].handle1.x = (2*node.x - m_x);
                            nodes[nNode].handle1.y = (2*node.y - m_y);
                        }
                        break;
                }
            }
            else
            {
                /* check if node or handle selected */
                for (vector<Node>::size_type i = 0, n = nodes.size(); i < n; i++)
                {
                    Node node = nodes[i];

                    /* Check if over node or handle */
                    if ((((node.x + NODESIZE) >= m_x) && (m_x >= (node.x - NODESIZE))) && (((node.y + NODESIZE) >= m_y) && (m_y >= (node.y - NODESIZE))))
                    {
                        /* click registered */
                        click = true;
                        nNode = i;
                        itemType = 0;
                        break;
                    }
                    else if(node.hasHandle1 && (((node.handle1.x + NODESIZE) >= m_x) && (m_x >= (node.handle1.x - NODESIZE))) && (((node.handle1.y + NODESIZE) >= m_y) && (m_y >= (node.handle1.y - NODESIZE))))
                    {
                        /* click registered */
                        click = true;
                        nNode = i;
                        itemType = 1;
                        break;
                    }
                    else if(node.hasHandle2 && (((node.handle2.x + NODESIZE) >= m_x) && (m_x >= (node.handle2.x - NODESIZE))) && (((node.handle2.y + NODESIZE) >= m_y) && (m_y >= (node.handle2.y - NODESIZE))))
                    {
                        /* click registered */
                        click = true;
                        nNode = i;
                        itemType = 2;
                        break;
                    }
                }
            }

            /* Only make node if no other node is clicked*/
            if (!click)
            {
            
                /* create a new node */
                Node newNode;
                newNode.x = m_x;
                newNode.y = m_y;

                /* create new node's default handle*/
                Point handle;
                handle.x = m_x;
                handle.y = m_y + 50;

                /* ensure point for other is blank */
                Point nHandle;
                nHandle.x = 0;
                nHandle.y = 0;

                /* select nature of handle */
                if(nodes.size() == 0)
                {
                    newNode.handle1 = handle;
                    newNode.hasHandle1 = true;

                    newNode.handle2 = nHandle;
                    newNode.hasHandle2 = false;

                    nodes.push_back(newNode);
                }
                else
                {
                    /* Calculate distance */
                    float df = sqrtf(powf(m_x - nodes.back().x, 2) + powf(m_y - nodes.back().y,2));
                    float ds = sqrtf(powf(m_x - nodes.front().x, 2) + powf(m_y - nodes.front().y,2));
                    
                    if (ds < df)
                    {
                        /* new node closer to front */
                        newNode.handle1 = handle;
                        newNode.hasHandle1 = true;

                        newNode.handle2 = nHandle;
                        newNode.hasHandle2 = false;

                        nodes.front().handle2.x = (2*nodes.front().x - nodes.front().handle1.x);
                        nodes.front().handle2.y = (2*nodes.front().y - nodes.front().handle1.y);
                        nodes.front().hasHandle2 = true;
                        nodes.insert(nodes.begin(), newNode);
                    }
                    else
                    {
                        /* new node closer to back */
                        newNode.handle1 = nHandle;
                        newNode.hasHandle1 = false;

                        newNode.handle2 = handle;
                        newNode.hasHandle2 = true;

                        /* add handle to previous node */
                        if (nodes.size() >= 2)
                        { 
                            /* update values */
                            nodes.back().handle1.x = (2*nodes.back().x - nodes.back().handle2.x);
                            nodes.back().handle1.y = (2*nodes.back().y - nodes.back().handle2.y);
                            nodes.back().hasHandle1 = true;
                        }

                        nodes.push_back(newNode);
                    }
                }
            }
        }

        /* Check  keyboard inputs */
        if (glfwGetKey(window, 69))
        {
            nodes.clear();
        }

		/* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
