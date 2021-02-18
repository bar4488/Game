//
// Created by bar44 on 11/3/2020.
//

#ifndef GOME_CHUNK_H
#define GOME_CHUNK_H


#include <mutex>
#include <glm/glm.hpp>
#include "graphics/VertexArray.h"
#include "graphics/VertexBuffer.h"
#include "graphics/IndexBuffer.h"
#include "graphics/Renderer.h"
#include "graphics/TextureBuffer.h"
#include "utils/noise/PerlinNoise.h"

static const unsigned int CHUNK_SIZE = 16;
static const unsigned int CHUNK_HEIGHT = 64;

enum face_dir : unsigned char
{
	up,
	down,
	right,
	left,
	forward,
	backward	
};
struct block_face
{
    glm::tvec3<unsigned char> position; // 3 bytes
    face_dir direction; // byte
    unsigned char texture; // byte
}; // 5 bytes

class Chunk {
public:
    explicit Chunk(siv::PerlinNoise noise, glm::vec3 position);
    explicit Chunk(siv::PerlinNoise noise, glm::vec3 position, VertexArray *vao);
    void LoadPosition(glm::vec3 position);
    void CalculateMesh();
    glm::vec3 GetPositionChunkSpace();
    glm::vec3 GetPositionWorldSpace();
    glm::vec3 GetCenterWorldSpace();
    unsigned int GetVisibleFacesCount();
    void Bind();
    void Unbind();
    ~Chunk();
public:
    bool m_Dirty;
    bool m_Meshed;
    bool m_Loaded;
    std::mutex m_Lock;
private:
    void InitializeChunk();
    void Draw(Renderer* renderer);
private:
    siv::PerlinNoise m_Noise;
    glm::vec3 m_Position;
    unsigned int m_ChunkData[CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE];
    std::vector<block_face> m_VisibleFaces;
    VertexArray m_VertexArray;
    TextureBuffer m_TextureBuffer;
    friend class ChunkManager;
};


#endif //GOME_CHUNK_H
