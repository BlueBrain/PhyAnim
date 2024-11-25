/* Copyright (c) 2020-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Juan Jose Garcia <juanjose.garcia@epfl.ch>
 * This file is part of PhyAnim <https://github.com/BlueBrain/PhyAnim>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Texture.h"

#include <GL/glew.h>

#include <iostream>

#include "lodepng.h"
namespace phyanim
{
namespace graphics
{
Texture::Texture(const std::string& fileName, uint32_t width, uint32_t height)
    : _width(width)
    , _height(height)
{
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    // glTexImage2D(GL_TEXTURE_CUBE_MAP, 0, GL_RGB, _width, _height, 0, GL_RGB,
    //                 GL_UNSIGNED_BYTE, NULL);

    std::cout << id << std::endl;
    loadPng(fileName);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Texture::loadPng(const std::string& fileName)
{
    for (uint32_t tex = 0; tex < 6; ++tex)
    {
        std::string texName = fileName;
        texName.append(std::to_string(tex));
        texName.append(".png");
        std::vector<unsigned char> image;
        uint32_t error =
            lodepng::decode(image, _width, _height, texName.c_str());

        if (error)
        {
            std::string errorMsg("decoder error ");
            errorMsg.append(std::to_string(error));
            errorMsg.append(": ");
            errorMsg.append(lodepng_error_text(error));
            throw std::runtime_error(errorMsg);
        }

        uint32_t size = _width * _height;
        uint32_t numChannels = image.size() / size;

        std::cout << texName << ": " << _width << "x" << _height << "x"
                  << numChannels << std::endl;

        float* rawData = (float*)malloc(sizeof(float) * size * 3);

        for (uint32_t j = 0; j < _height; ++j)
        {
            for (uint32_t i = 0; i < _width; ++i)
            {
                uint32_t inId = (j * _width + i) * numChannels;
                uint32_t outId = (j * _width + _width - 1 - i) * 3;
                rawData[outId] = image[inId] / 255.0f;
                rawData[outId + 1] = image[inId + 1] / 255.0f;
                rawData[outId + 2] = image[inId + 2] / 255.0f;
            }
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + tex, 0, GL_RGB, _width,
                     _height, 0, GL_RGB, GL_FLOAT, rawData);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // glGenerateMipmap(GL_TEXTURE_2D);
}

}  // namespace graphics
}  // namespace phyanim
