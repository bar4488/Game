//
// Created by bar44 on 11/3/2020.
//

#ifndef GOME_CHUNK_H
#define GOME_CHUNK_H


#include <glm/glm.hpp>
#include "graphics/VertexArray.h"
#include "graphics/VertexBuffer.h"
#include "graphics/IndexBuffer.h"

static const unsigned int CHUNK_SIZE = 16;
static const unsigned int CHUNK_HEIGHT = 32;

class Chunk {
public:
    explicit Chunk(glm::vec3 position);
    explicit Chunk(glm::vec3 position, VertexArray *vao, VertexBuffer *vb, IndexBuffer *ib);
    unsigned int GetIndicesCount();
    void CalculateIndices();
    void CalculateVisibleVertices();
    void LoadPosition(glm::vec3 position);
    glm::vec3 GetPositionChunkSpace();
    glm::vec3 GetPositionWorldSpace();
    unsigned int GetVisibleBlocksCount();
    void Bind();
    void Unbind();
    ~Chunk();
public:
    bool m_Active;
private:
    glm::vec3 m_Position;
    unsigned int m_ChunkData[CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE];
    std::vector<uint32_t> m_VisibleBlocks;
    IndexBuffer *m_IndexBuffer;
    VertexBuffer *m_VertexBuffer;
    VertexArray *m_VertexArray;
};


#endif //GOME_CHUNK_H
