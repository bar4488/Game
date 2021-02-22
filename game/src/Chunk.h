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
static const unsigned int CHUNK_HEIGHT = 256;

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

enum chunk_state
{
	dirty,
	meshed,
	loaded
};

class Chunk {
public:
    explicit Chunk(siv::PerlinNoise noise, glm::ivec2 position);
    explicit Chunk(siv::PerlinNoise noise, glm::ivec2 position, VertexArray *vao);
    void SetPosition(glm::ivec2 position);
    void LoadMesh();
    void LoadData();
    glm::ivec2 GetPositionChunkSpace();
    glm::ivec3 GetPositionWorldSpace();
    glm::ivec3 GetCenterWorldSpace();
    unsigned int GetVisibleFacesCount();
    // basically returns the highest block
    unsigned char GetHeight();
    void Bind();
    void Unbind();
    ~Chunk();
public:
    chunk_state m_CurrentState;
    std::mutex m_Lock;
private:
    void InitializeChunk();
    void Draw(Renderer* renderer);
private:
    siv::PerlinNoise m_Noise;
    unsigned char m_HeighestBlock;
    glm::ivec2 m_Position;
    unsigned int m_ChunkData[CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE];
    std::vector<block_face> m_VisibleFaces;
    VertexArray m_VertexArray;
    TextureBuffer m_TextureBuffer{GL_R8UI};
    friend class ChunkManager;
};


#endif //GOME_CHUNK_H
