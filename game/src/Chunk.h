//
// Created by bar44 on 11/3/2020.
//

#ifndef GOME_CHUNK_H
#define GOME_CHUNK_H


#include <mutex>
#include <glm/glm.hpp>
#include "graphics/VertexArray.h"
#include "graphics/IndexBuffer.h"
#include "graphics/Renderer.h"
#include "graphics/TextureBuffer.h"
#include "FastNoise/FastNoise.h"

class ChunkManager;

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
    unloaded, // chunk was just initialized
	loaded, // m_ChunkData is initialized
	meshed, // m_VisibleFaces is ready but not on gpu
	dirty, // the chunk was changed and should be remeshed, we can still draw it
	dirty_meshed, // the chunk was changed and should be sent to gpu, we can still draw it
    active // the chunk is active and no more work is needed
};

class Chunk {
public:
    explicit Chunk(ChunkManager* mgr, FastNoise::SmartNode<> noise, glm::ivec2 position);
    explicit Chunk(ChunkManager* mgr, FastNoise::SmartNode<> noise, glm::ivec2 position, VertexArray *vao);
    void SetPosition(glm::ivec2 position);
    void LoadData();
    void CalculateMesh();
    void LoadMesh();
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
    ChunkManager* m_Manager;
    FastNoise::SmartNode<> m_Noise;
    unsigned char m_HeighestBlock;
    glm::ivec2 m_Position;
    unsigned int m_ChunkData[CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE];
    std::vector<block_face> m_VisibleFaces;
    int m_FacesCount = 0;
    VertexArray m_VertexArray;
    TextureBuffer m_TextureBuffer{GL_R8UI};
    friend class ChunkManager;
};


#endif //GOME_CHUNK_H
