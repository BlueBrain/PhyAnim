#include "Texture.h"

#include <GL/glew.h>

#include "lodepng.h"

namespace examples
{
Texture::Texture(const std::string& fileName,
        uint32_t width,
        uint32_t height)
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
        uint32_t error = lodepng::decode(image, _width, _height, texName.c_str());

        if(error)
        {    
            std::string errorMsg("decoder error ");
            errorMsg.append(std::to_string(error));
            errorMsg.append(": ");
            errorMsg.append(lodepng_error_text(error));
            throw std::runtime_error(errorMsg);
        }
        
        uint32_t size = _width * _height;
        uint32_t numChannels = image.size() / size; 

        std::cout << texName << ": " <<  _width << "x" << _height << "x" << numChannels << std::endl; 

        float* rawData = (float*)malloc(sizeof(float)*size*3);
    
   
        for (uint32_t j = 0; j < _height; ++j)
        {
            for (uint32_t i = 0; i < _width; ++i)
            {
                rawData[(j * _width + i) * 3] = image[(j * _width + i) * numChannels] / 255.0f;
                rawData[(j * _width + i) * 3 + 1] = image[(j * _width + i) * numChannels + 1] / 255.0f;
                rawData[(j * _width + i) * 3 + 2] = image[(j * _width + i) * numChannels + 2] / 255.0f;
            }
        }

   
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + tex, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_FLOAT, rawData);
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        
    // glGenerateMipmap(GL_TEXTURE_2D);
}

}
