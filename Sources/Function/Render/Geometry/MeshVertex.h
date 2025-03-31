#pragma once

#include<memory>
#include<functional>

#include"../../../Core/Math.h"

namespace focus
{
    typedef struct Vertex
    {
        Vector3 pos;
        Vector3 color;
        Vector2 texCoord;
        Vector3 normal;

        bool operator==(const Vertex& other) const
        {
            return pos == other.pos && color == other.color && texCoord == other.texCoord;
        }
    }Vertex;
}

namespace std
{
    template<> struct hash<focus::Vertex>
    {
        size_t operator()(focus::Vertex const& vertex) const
        {
            return ((hash<focus::Vector3>()(vertex.pos) ^ (hash<focus::Vector3>()(vertex.color) << 1)) >> 1) ^ (hash<focus::Vector2>()(vertex.texCoord) << 1);
        }
    };
}