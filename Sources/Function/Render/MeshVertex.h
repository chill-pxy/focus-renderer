#pragma once

#include<memory>
#include<functional>

#include"../../Core/Math.h"

namespace FOCUS
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
    template<> struct hash<FOCUS::Vertex>
    {
        size_t operator()(FOCUS::Vertex const& vertex) const
        {
            return ((hash<FOCUS::Vector3>()(vertex.pos) ^ (hash<FOCUS::Vector3>()(vertex.color) << 1)) >> 1) ^ (hash<FOCUS::Vector2>()(vertex.texCoord) << 1);
        }
    };
}