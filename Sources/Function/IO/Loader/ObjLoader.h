#pragma once

#include<tiny_obj_loader.h>

#include"ModelLoader.h"

namespace focus
{
	class ObjLoader : public ModelLoader
	{
	public:
		ObjLoader() = default;
		~ObjLoader() = default;

		std::shared_ptr<Model> loadModel(const std::string& modelPath, std::shared_ptr<MaterialManager> materialManager, std::string setname = "undefined") override
		{
            std::shared_ptr<Model> model = std::make_shared<Model>();

            // find mlt path
            std::string basePath;
            size_t pos = modelPath.rfind('/');
            size_t pos2 = modelPath.rfind('\\');
            pos = std::max(pos, pos2);
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

            if (materials.size() == 0)
            {
                auto name = "Default Material";
                std::shared_ptr<Texture> texture = loadTexture(RESOURCE_PATH"Asset/Images/white.png");
                auto material = materialManager->createMaterial(name, texture);

                material->_name = name;
                material->_roughness = 0.95;
                material->_metallic = 0.15;

                model->_materials.push_back(material);
            }

            // handle material
            for (tinyobj::material_t& mat : materials)
            {
                std::shared_ptr<PhysicalMaterial> material;

                if (mat.diffuse_texname.compare(""))
                {
                    std::shared_ptr<Texture> texture = loadTexture((basePath + '/' + mat.diffuse_texname).c_str());
                    material = materialManager->createMaterial(mat.name, texture);

                    material->_name = mat.name;
                    material->_ambient = mat.ambient[0];
                    material->_diffuse = mat.diffuse[0];
                    material->_specular = mat.specular[0];
                    material->_roughness = shininessToRoughness(mat.shininess);
                    material->_shinness = mat.shininess;
                    material->_ior = mat.ior;
                    material->_metallic = mat.metallic;

                    material->_ambientTex = mat.ambient_texname;
                    material->_diffuseTex = mat.diffuse_texname;
                    material->_specularTex = mat.specular_texname;

                    model->_materials.push_back(material);
                }
                else
                {
                    std::shared_ptr<Texture> texture = loadTexture(RESOURCE_PATH"Asset/Images/white.png");
                    material = materialManager->createMaterial(mat.name, texture);

                    material->_name = mat.name;
                    material->_ambient = mat.ambient[0];
                    material->_diffuse = mat.diffuse[0];
                    material->_specular = mat.specular[0];
                    material->_roughness = shininessToRoughness(mat.shininess);
                    material->_shinness = mat.shininess;
                    material->_ior = mat.ior;
                    material->_metallic = mat.metallic;

                    material->_ambientTex = mat.ambient_texname;
                    material->_diffuseTex = mat.diffuse_texname;
                    material->_specularTex = mat.specular_texname;

                    model->_materials.push_back(material);
                }
            }

            // handle every shape
            std::vector<std::vector<tinyobj::index_t>> allSubFace;
            std::vector<std::string> name;
            std::unordered_map<std::string, int> nameTable{};

            if (materials.size() == 0)
            {
                allSubFace.resize(1);
                name.resize(1);
            }
            else
            {
                allSubFace.resize(materials.size());
                name.resize(materials.size());
            }

            for (uint32_t i = 0; i < shapes.size(); ++i)
            {
                uint32_t indexCount = shapes[i].mesh.indices.size();
                uint32_t faceCount = shapes[i].mesh.num_face_vertices.size();

                uint32_t indexOffset = 0;

                for (uint32_t j = 0; j < faceCount; ++j)
                {
                    int materialIndex = shapes[i].mesh.material_ids[j];
                    if (materialIndex < 0)
                    {
                        materialIndex = 0;
                    }
                    std::vector<tinyobj::index_t>& subIdx = allSubFace[materialIndex];
                    subIdx.push_back(shapes[i].mesh.indices[indexOffset++]);
                    subIdx.push_back(shapes[i].mesh.indices[indexOffset++]);
                    subIdx.push_back(shapes[i].mesh.indices[indexOffset++]);
                    if (name[materialIndex] == "")
                    {
                        if (nameTable.find(shapes[i].name) != nameTable.end())
                        {
                            nameTable[shapes[i].name] = 1;
                            name[materialIndex] = shapes[i].name;
                        }
                        else
                        {
                            nameTable[shapes[i].name] = nameTable[shapes[i].name] + 1;
                            name[materialIndex] = shapes[i].name + std::to_string(nameTable[shapes[i].name]);
                        }
                    }
                }
            }

            // handle mesh
            std::unordered_map<Vertex, uint32_t> uniqueVertices{};
            uint32_t count = 0;

            for (std::vector<tinyobj::index_t>& subIdx : allSubFace)
            {
                std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

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

                mesh->_indices = indices;
                mesh->_vertices = vertices;
                mesh->_material = model->_materials[count];
                mesh->_name = name[count];
                mesh->_scale = Vector3(1, 1, 1);

                model->_meshes.push_back(mesh);

                if (count < model->_materials.size())
                {
                    count++;
                }

                model->_name = setname;
            }

            return model;
		}
    };
}