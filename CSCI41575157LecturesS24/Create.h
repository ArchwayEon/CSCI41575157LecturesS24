#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "GraphicsObject.h"
#include "Renderer.h"
#include "Shader.h"

class Create
{
public:
    static unsigned int Texture2D(
        unsigned char* textureData, unsigned int width, unsigned int height);
    static unsigned int TextureFromFile(const std::string& filePath);
    static unsigned char* TextureDataFromFile(
        const std::string& filePath, int& width, int& height, int& numChannels);
    static void PCNTScene1(
        std::unordered_map<std::string, std::shared_ptr<GraphicsObject>>& allObjects,
        std::shared_ptr<Renderer>& renderer, std::shared_ptr<Shader>& shader);
};

