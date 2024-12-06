#pragma once

#include<unordered_map>

#include"Mesh.h"
#include"../Materials/BlinnPhongMaterial.h"
#include"../Texture.h"

namespace FOCUS
{
	class Model
	{
    public:
        std::vector<Mesh> _meshes;
        std::vector<std::shared_ptr<BlinnPhongMaterial>> _materials;
	};

    std::shared_ptr<Model> loadModel(std::string modelPath)
    {
        std::shared_ptr<Model> model = std::make_shared<Model>();

        // find mlt path
        std::string basePath;
        size_t pos = modelPath.rfind('/');
        if (pos != std::string::npos)
        {
            basePath = modelPath.substr(0, pos);
        }

        // load obj data
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.c_str(), basePath.c_str()))
        {
            throw std::runtime_error(warn + err);
        }

        // handle material
        for (tinyobj::material_t& mat : materials)
        {
            std::shared_ptr<BlinnPhongMaterial> material = std::make_shared<BlinnPhongMaterial>();

            material->_name = mat.name;
            material->_ambient = mat.ambient[0];
            material->_diffuse = mat.diffuse[0];
            material->_specular = mat.specular[0];
            material->_roughness = mat.shininess;
            material->_ior = mat.ior;

            material->_ambientTex = mat.ambient_texname;
            material->_diffuseTex = mat.diffuse_texname;
            material->_specularTex = mat.specular_texname;

            if (mat.diffuse_texname.compare(""))
            {
                material->_basicTexture = loadTexture((basePath + '/' + mat.diffuse_texname).c_str());
            }

            model->_materials.push_back(material);
        }

        // handle every shape
        std::vector<std::vector<tinyobj::index_t>> allSubFace;
        allSubFace.resize(materials.size() + 1);

        for (uint32_t i = 0; i < shapes.size(); ++i)
        {
            uint32_t indexCount = shapes[i].mesh.indices.size();
            uint32_t faceCount = shapes[i].mesh.num_face_vertices.size();

            uint32_t indexOffset = 0;

            for (uint32_t j = 0; j < faceCount; ++j)
            {
                int materialIndex = shapes[i].mesh.material_ids[j];

                std::vector<tinyobj::index_t>& subIdx = allSubFace[materialIndex + 1];
                subIdx.push_back(shapes[i].mesh.indices[indexOffset++]);
                subIdx.push_back(shapes[i].mesh.indices[indexOffset++]);
                subIdx.push_back(shapes[i].mesh.indices[indexOffset++]);
            }
        }

        // handle mesh
        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        for (std::vector<tinyobj::index_t>& subIdx : allSubFace)
        {
            Mesh mesh;

            uint32_t count = 0;
            for (tinyobj::index_t& index : subIdx)
            {
                Vertex vertex{};

                if (attrib.vertices.size() > 0)
                {
                    vertex.pos =
                    {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };
                }

                if (attrib.texcoords.size() > 0)
                {
                    vertex.texCoord =
                    {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                    };
                }

                if (attrib.normals.size() > 0)
                {
                    vertex.normal =
                    {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    };
                }

                vertex.color = { 1.0f, 1.0f, 1.0f };

                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }

                indices.push_back(uniqueVertices[vertex]);
            }

            mesh._indices = indices;
            mesh._vertices = vertices;
            mesh._material = model->_materials[count];

            model->_meshes.push_back(mesh);

            count++;
        }

        return model;
    }
}