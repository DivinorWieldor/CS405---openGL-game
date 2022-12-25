#ifndef MESH_H
#define MESH_H
#define MAX_BONE_INFLUENCE 4

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include <string>
#include <vector>
using namespace std;


struct Vertex{
    glm::vec3 Position;// position
    glm::vec3 Normal;// normal
    glm::vec2 TexCoords;// texCoords
    glm::vec3 Tangent;// tangent
    glm::vec3 Bitangent;// bitangent
    int m_BoneIDs[MAX_BONE_INFLUENCE];//bone indexes which will influence this vertex
    float m_Weights[MAX_BONE_INFLUENCE];//weights from each bone
};
struct Texture{
    unsigned int id;
    string type;
    string path;
};

class Mesh{
public:
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;

    // constructor
    Mesh(const vector<Vertex> &vertices_, const vector<unsigned int> &indices_, const vector<Texture> &textures_) : 
        vertices(vertices_), indices(indices_), textures(textures_){
        setupMesh();//set vertex buffers and its attribute pointers
    }
    
    void Draw(const Shader& shader){// render mesh
        // bind appropriate textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;

        for(unsigned int i = 0; i < textures.size(); i++){
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            
            string number; //retrieve texture number (the N in diffuse_textureN)
            string name = textures[i].type;
            if(name == "texture_diffuse")       number = std::to_string(diffuseNr++);
            else if(name == "texture_specular") number = std::to_string(specularNr++);
            else if(name == "texture_normal")   number = std::to_string(normalNr++);
            else if(name == "texture_height")   number = std::to_string(heightNr++);

            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);//set sampler to correct texture unit
            glBindTexture(GL_TEXTURE_2D, textures[i].id);//bind texture
        }

        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);//set everything back to defaults
    }


private:
    unsigned int VBO, EBO;// render data 

    void setupMesh(){//VBO/EBO initialization
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);//vertex attribute object
        glGenBuffers(1, &VBO);//vertex buffer object
        glGenBuffers(1, &EBO);//edge buffer object

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);//load into vBuffer
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);//copy user defined data to bound buffer

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);//eliminates need for duplicate vertices
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        //Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        //normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        //texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        //tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        //bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        //ID
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
        //weights
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
        glBindVertexArray(0);
    }
};
#endif