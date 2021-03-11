//
// Created by bar44 on 10/29/2020.
//

#include "VertexBufferLayout.h"

VertexBufferLayout::VertexBufferLayout(): m_Stride(0) {

}

template <>
void VertexBufferLayout::Push<unsigned>(unsigned count, unsigned location)
{
	m_Elements.push_back({GL_UNSIGNED_INT, count, location, false});
	m_Stride += count * glutls::GetSizeOfType(GL_UNSIGNED_INT);
}

template <>
void VertexBufferLayout::Push<float>(unsigned count, unsigned location)
{
	m_Elements.push_back({GL_FLOAT, count, location, false});
	m_Stride += count * glutls::GetSizeOfType(GL_FLOAT);
}

template <>
void VertexBufferLayout::Push<unsigned char>(unsigned count, unsigned location)
{
	m_Elements.push_back({GL_UNSIGNED_BYTE, count, location, false});
	m_Stride += count * glutls::GetSizeOfType(GL_UNSIGNED_BYTE);
}

template <>
void VertexBufferLayout::Push<char>(unsigned count, unsigned location)
{
	m_Elements.push_back({GL_BYTE, count, location, false});
	m_Stride += count * glutls::GetSizeOfType(GL_BYTE);
}

template <>
void VertexBufferLayout::Push<int>(unsigned count, unsigned location)
{
	m_Elements.push_back({GL_INT, count, location, false});
	m_Stride += count * glutls::GetSizeOfType(GL_INT);
}
