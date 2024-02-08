#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
using namespace std;

void checkstatus(ifstream &in);

struct VertextInfo
{
    // vertex
    float x;
    float y;
    float z;

    // normal
    float normal_x;
    float normal_y;
    float normal_z;

    // texture coordinates
    float u;
    float v;
};

struct Triangle
{
    VertextInfo v1;
    VertextInfo v2;
    VertextInfo v3;
};

void displayVertextInfo(struct VertextInfo *vertex_info) {
    cout << "v:"<<endl;

    // Insert the vertex coordinates
    cout << "x=" << vertex_info->x <<endl;
    cout << "y=" << vertex_info->y <<endl;
    cout << "z=" << vertex_info->z <<endl;

    // Insert the vertex normal's coordinates
    cout << "nx=" << vertex_info->normal_x <<endl;
    cout << "ny=" << vertex_info->normal_y <<endl;
    cout << "nz=" << vertex_info->normal_z <<endl;

    // Insert texture coordinates
    cout << "u=" << vertex_info->u <<endl;
    cout << "v=" << vertex_info->v <<endl;
}

void displayTriangle(struct Triangle *triangle) {
    cout << "t:" << endl;
    displayVertextInfo(&triangle->v1);
    displayVertextInfo(&triangle->v2);
    displayVertextInfo(&triangle->v3);
}

void initializeVertexInfo(char *group, char *control_string, int control_offset, struct VertextInfo *vertex_info) {

    unsigned int j = 0, k = 0, r = 0;

    float v_components[4];
    j = 0;
    r = 0;
    char component[80];
    // While not at the end of a null terminated string
    for(k=control_offset; k<strlen(control_string) + 1; k++) {
        if(control_string[k] == ' ' || control_string[k] == '\0') {
            r = 0;
            v_components[j] = atof(component);
            j++;
        } else {
            component[r] = control_string[k];
            r++;
        }
    }

    if(!strcmp(group, "v")) { // Geometric vertex
        vertex_info->x = v_components[0];
        vertex_info->y = v_components[1];
        vertex_info->z = v_components[2];
    } else if(!strcmp(group, "vn")) { // Vertex normal
        vertex_info->normal_x = v_components[0];
        vertex_info->normal_y = v_components[1];
        vertex_info->normal_z = v_components[2];
    } else if(!strcmp(group, "vt")) { // Texture coordinate
        vertex_info->u = v_components[0];
        vertex_info->v = v_components[1];
    }
}

int parseOBJ(char *filename, struct Triangle triangles_buffer[]) {

    // OBJ file input stream
    ifstream obj_f(filename, ios::in);

    checkstatus(obj_f);

    char VTX[] = "v", NVTX[] = "vn", TVTX[] = "vt";
    unsigned int numberOfTriangles = 0, vertices = 0, vertexCount = 0, normals = 0, normalsCount = 0, texels = 0, texelCount = 0;
    int count = -1;

    while(obj_f) {
        char line[255];
        obj_f.getline(line, 255);

        // Extracting only vertex related lines
        if(*line == 'v') {
            if(*(line + 1) == ' ') { // Geometric vertex
                vertices++;
                if(vertexCount == 0) {
                    initializeVertexInfo(VTX, line, 2, &(triangles_buffer[numberOfTriangles].v1));
                    vertexCount = (vertexCount + 1) % 3;
                    // Increment triangle count
                    count++;
                } else if(vertexCount == 1) {
                    initializeVertexInfo(VTX, line, 2, &(triangles_buffer[numberOfTriangles].v2));
                    vertexCount = (vertexCount + 1) % 3;
                } else if(vertexCount == 2) {
                    initializeVertexInfo(VTX, line, 2, &(triangles_buffer[numberOfTriangles].v3));
                    vertexCount = (vertexCount + 1) % 3;

                    // Advance to next triangle index
                    numberOfTriangles++;
                }
            } else if(line[1] == 'n') { // Vertex normal
                switch(normalsCount)
                {
                    case 0:
                        initializeVertexInfo(NVTX, line, 3, &triangles_buffer[normals].v1);
                        normalsCount = (normalsCount + 1) % 3;
                        break;
                    case 1:
                        initializeVertexInfo(NVTX, line, 3, &triangles_buffer[normals].v2);
                        normalsCount = (normalsCount + 1) % 3;
                        break;
                    case 2:
                        initializeVertexInfo(NVTX, line, 3, &triangles_buffer[normals].v3);
                        normalsCount = (normalsCount + 1) % 3;

                        // Advance normals index
                        normals++;
                        break;
                }
            } else if(line[1] == 't') { // Texture coordinate (texel)
                switch(texelCount)
                {
                    case 0:
                        initializeVertexInfo(TVTX, line, 3, &triangles_buffer[texels].v1);
                        texelCount = (texelCount + 1) % 3;
                        break;
                    case 1:
                        initializeVertexInfo(TVTX, line, 3, &triangles_buffer[texels].v2);
                        texelCount = (texelCount + 1) % 3;
                        break;
                    case 2:
                        initializeVertexInfo(TVTX, line, 3, &triangles_buffer[texels].v3);
                        texelCount = (texelCount + 1) % 3;

                        // Advance texels index
                        texels++;
                        break;
                }
            } else {
                // Other vertex related lines
                continue;
            }
        } else {
            // Non-vertex lines
            continue;
        }
    }

    obj_f.close();

    return count + 1;
}

void checkstatus(ifstream &in) {
    ios::iostate i;

    i = in.rdstate();

    if(i & ios::eofbit) {
        cout<<"EOF encountered\n";
    } else if(i & ios::failbit) {
        cout<<"Non-Fatal I/O error\n";
    } else if(i & ios::badbit) {
        cout<<"Fata I/O error\n";
    } else {
        //cout<<"Success\n";
    }
}


int main()
{
    char filename[] = "Shuttlecock.obj";
    int nTriangles = 50000;
    // An array of triangles
    Triangle allTriangles[nTriangles];

    int createdTriangles = parseOBJ(filename, allTriangles);

    for(int i=0; i<createdTriangles; i++) {
        displayTriangle(&allTriangles[i]);
    }

    // Obtaining a joint list of all triangles' vertext information
    //VertextInfo *verticesOfTriangles = (VertextInfo *) allTriangles;

    return 0;
}