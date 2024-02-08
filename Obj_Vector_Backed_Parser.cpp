#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

struct vec4 {
    float x = 0.0, y = 0.0, z = 0.0, w = 0.0;
};

struct vec3 {
    float x = 0.0, y = 0.0, z = 0.0;
};

struct vertex_index_info {
    long v = 0, vt = 0, vn = 0;
};

struct VertexInfo
{
    float x = 0, y = 0, z = 0;
    float normal_x = 0, normal_y = 0, normal_z = 0;
    float u = 0, v = 0;
};

struct Triangle
{
    struct VertexInfo v1;
    struct VertexInfo v2;
    struct VertexInfo v3;
};

std::ostream &operator<<(std::ostream &stream, struct vec4 v) {
    stream <<"("<<v.x<<", "<<v.y<<", "<<v.z<<", "<<v.w<<")";
    return stream;
}

std::ostream &operator<<(std::ostream &stream, struct vec3 v) {
    stream <<"("<<v.x<<", "<<v.y<<", "<<v.z<<")";
    return stream;
}

// Function Prototypes
void printVertexInfo(struct VertexInfo &vertex_info);
void printTriangle( struct Triangle &triangle);
//void printVertexIndicesInfo(struct vertex_index_info &vii);
void buildVertexInfo(struct VertexInfo &vertex_info, struct vertex_index_info &vtx_i_info, std::vector<vec4> &vertices, std::vector<vec3> &textures, std::vector<vec3> &normals);

std::vector<Triangle> triangulate(std::string triangulation_mode, std::vector<vertex_index_info> &vertex_indices, std::vector<vec4> &vertices, std::vector<vec3> &textures, std::vector<vec3> &normals) {

    std::vector<Triangle> triangles;

    auto itr = vertex_indices.begin();

    if(triangulation_mode == "TRIANGLES_STRIP") {
        while (itr != vertex_indices.end() && (itr + 2) != vertex_indices.end()) {
            // Construct a triangle
            struct Triangle triangle;

            // First of three
            buildVertexInfo(triangle.v1, *itr, vertices, textures, normals);

            // Second of three
            buildVertexInfo(triangle.v2, *(itr + 1), vertices, textures, normals);

            // Third of three
            buildVertexInfo(triangle.v3, *(itr + 2), vertices, textures, normals);

            // Append triangle
            triangles.push_back(triangle);

            // Advance to the next triangle's start vertex
            itr++;
        }
    } else if (triangulation_mode == "TRIANGLES_FAN") {

        // Maintain a reference to the radiation origin
        vertex_index_info vii_origin = *itr;

        // Increment to the next vertex
        itr++;

        // Until the very last index info
        while (itr != vertex_indices.end() && (itr + 1) != vertex_indices.end()) {

            // Construct a triangle
            struct Triangle triangle;

            // First of three (Always radiating from the first vertex)
            buildVertexInfo(triangle.v1, vii_origin, vertices, textures, normals);

            // Second of three
            buildVertexInfo(triangle.v2, *itr, vertices, textures, normals);

            // Third of three
            buildVertexInfo(triangle.v3, *(itr + 1), vertices, textures, normals);

            // Append triangle
            triangles.push_back(triangle);

            // Advance the iterator in two's
            itr++;
        }
    } else {

    }

    return triangles;
}


void buildVertexInfo(struct VertexInfo &vertex_info, struct vertex_index_info &vtx_i_info, std::vector<vec4> &vertices, std::vector<vec3> &textures, std::vector<vec3> &normals) {
    vertex_info.x = vertices[vtx_i_info.v - 1].x;
    vertex_info.y = vertices[vtx_i_info.v - 1].y;
    vertex_info.z = vertices[vtx_i_info.v - 1].z;
    vertex_info.normal_x = normals[vtx_i_info.vn - 1].x;
    vertex_info.normal_y = normals[vtx_i_info.vn - 1].y;
    vertex_info.normal_z = normals[vtx_i_info.vn - 1].z;
    vertex_info.u = textures[vtx_i_info.vt - 1].x;
    vertex_info.v = textures[vtx_i_info.vt - 1].y;
}

std::vector<Triangle> parseObjFile(const std::string& filename)
{
    std::ifstream objFile(filename);
    if (!objFile.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        // Handle error appropriately
        exit(EXIT_FAILURE);
    }

    std::vector<Triangle> triangles;
    std::vector<vec4> vertices;
    std::vector<vec3> normals;
    std::vector<vec3> textures;

    std::string line;
    while (std::getline(objFile, line))
    {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "v")
        {
            vec4 v4;
            iss >> v4.x >> v4.y >> v4.z >> v4.w;
            vertices.push_back(v4);
        }
        else if (token == "vn")
        {
            vec3 v3;
            iss >> v3.x >> v3.y >> v3.z;
            normals.push_back(v3);
        }
        else if (token == "vt")
        {
            vec3 v3;
            iss >> v3.x >> v3.y >> v3.z;
            textures.push_back(v3);
        }
        else if (token == "f")
        {
            std::string vertex_texture_normal_token;

            std::vector<vertex_index_info> vertex_indices;

            while(iss >> vertex_texture_normal_token) {
                long vertexIndex, texelIndex, normalIndex;
                std::istringstream index_stream(vertex_texture_normal_token);

                // While the face has more vertices
                while(index_stream >> vertexIndex) {
                    if(index_stream.peek() == '/') {
                        index_stream.ignore(); // Ignore the first '/'
                        if(index_stream.peek() == '/') {
                            index_stream.ignore(); //  Ignore second '/'
                            texelIndex = 0; // Missing texel index
                            index_stream >> normalIndex; // Read normal index
                        } else {
                            index_stream >> texelIndex; // Read the texel index
                            if(index_stream.peek() == '/') {
                                index_stream.ignore(); // Ignore second '/'
                                index_stream >> normalIndex;
                            } else {
                                // Missing normal index
                                normalIndex = 0; // Set to zero
                            }
                        }
                    } else {
                        // Missing texture and normal indices
                        texelIndex = 0;
                        normalIndex = 0;
                    }
                }

                // Construct a vertex indices info structure
                struct vertex_index_info index_info;
                index_info.v = vertexIndex;
                index_info.vt = texelIndex;
                index_info.vn = normalIndex;
                vertex_indices.push_back(index_info);
            }

            // Triangulate the face' vertices
            std::vector<Triangle> face_triangles = triangulate("TRIANGLES_STRIP", vertex_indices, vertices, textures, normals);

            // Hold all subsequent triangles in a global store
            auto itr = face_triangles.begin();
            while (itr != face_triangles.end()) {

                // Push for storage
                triangles.push_back(*itr);

                itr++;
            }
        }
    }

    return triangles;
}

void printVertexIndicesInfo(struct vertex_index_info &vii) {
    std::cout<<vii.v<<" / "<<vii.vt<<" / "<<vii.vn<<std::endl;
}

void printTriangles(std::vector<Triangle> &triangles)
{
    auto itr = triangles.begin();

    while(itr != triangles.end()) {
        // Print triangle
        printTriangle(*itr);

        // Advance pointer
        itr++;
    }
}

void printTriangle(struct Triangle &triangle) {
    std::cout << "t:" << std::endl;
    printVertexInfo(triangle.v1);
    printVertexInfo(triangle.v2);
    printVertexInfo(triangle.v3);
}

void printVertexInfo(struct VertexInfo &vertex_info) {
    std::cout << "v:" << std::endl;
    std::cout << "x=" << vertex_info.x << std::endl;
    std::cout << "y=" << vertex_info.y << std::endl;
    std::cout << "z=" << vertex_info.z << std::endl;
    std::cout << "nx=" << vertex_info.normal_x << std::endl;
    std::cout << "ny=" << vertex_info.normal_y << std::endl;
    std::cout << "nz=" << vertex_info.normal_z << std::endl;
    std::cout << "u=" << vertex_info.u << std::endl;
    std::cout << "v=" << vertex_info.v << std::endl;
}

int main()
{
    std::string objFilename = "Shuttlecock.obj";
    std::vector<Triangle> triangles = parseObjFile(objFilename);

    std::cout<<triangles.size()<<std::endl;

    printTriangles(triangles);

    return 0;
}
