

#include "SkyBox.h"

#include <GL/glew.h>

namespace examples
{


const std::string vSkySource(
    "#version 400\n"
    "in vec3 inPos;"
    "out vec3 pos;"
    "uniform mat4 projViewModel;"
    "void main(void) {"
    "pos = (inPos * 0.5) + vec3(0.5);"
    "gl_Position = vec4(inPos, 1.0);}");

const std::string fSkySource(
    "#version 400\n"
    "in vec3 pos;"
    "out vec4 color;"
    "uniform sampler2D colorTex;"
    "void main(void){"
    "float inc = 0.001;"
    "vec4 difuse = texture(colorTex, pos.xy);"
    "vec2 incVec = vec2(inc, 0.0);" 
    "difuse += texture(colorTex, pos.xy + incVec);"
    "incVec = vec2(-inc, 0.0);" 
    "difuse += texture(colorTex, pos.xy + incVec);"
    "incVec = vec2(0.0, inc);" 
    "difuse += texture(colorTex, pos.xy + incVec);"
    "incVec = vec2(0.0, -inc);" 
    "difuse += texture(colorTex, pos.xy + incVec);"
    "difuse *= 0.1;"
    "color = difuse;}");



SkyBox::SkyBox()
{
    _program = new RenderProgram(vSkySource, "", fSkySource);
    _uProjView = glGetUniformLocation(_program->id, "projViewModel");
    _texture = new Texture("data/img/cells.png");
   

    float pos[12];
    pos[0] = -1.0f; pos[1] = 1.0f; pos[2] = -1.0f;
    pos[3] = -1.0f; pos[4] = -1.0f; pos[5] = -1.0f;
    pos[6] = 1.0f; pos[7] = -1.0f; pos[8] = -1.0f;
    pos[9] = 1.0f; pos[10] = 1.0f; pos[11] = -1.0f;


    uint32_t indices[6];
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 0;
    indices[4] = 2;
    indices[5] = 3;

    glGenVertexArrays(1, &_vao);
    unsigned int vbos[2];
    glGenBuffers(2, vbos);
    
    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, pos,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                sizeof(unsigned int) * 6, indices,
                GL_STATIC_DRAW);

    glBindVertexArray(0);


}

void SkyBox::render(Camera* camera)
{

    _program->use();
    // glUniformMatrix4fv(_uProjView, 1, GL_FALSE,
    //                    m.data());        
    glBindTexture(GL_TEXTURE_2D, _texture->id);
    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

}
