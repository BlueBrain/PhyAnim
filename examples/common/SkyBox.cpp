

#include "SkyBox.h"

#include <GL/glew.h>

namespace examples
{


const std::string vSkySource(
    "#version 400\n"
    "in vec3 inPos;"
    "out vec3 pos;"
    "uniform mat4 view;"
    "uniform mat4 proj;"
    "void main(void) {"
    "pos = (inPos);"
    // "pos = (vec4(inPos, 0.0)).xyz;"
    "gl_Position = proj * vec4((view * vec4(inPos, 0.0)).xyz, 1.0);}");

const std::string fSkySource(
    "#version 400\n"
    "in vec3 pos;"
    "out vec4 color;"
    "uniform samplerCube sky;"
    "void main(void){"
    "float inc = 0.01;"
    "vec4 difuse = texture(sky, pos);"
    "vec3 incVec = vec3(inc, 0.0, 0.0);" 
    "difuse += texture(sky, pos + incVec);"
    "incVec = vec3(-inc, 0.0, 0.0);" 
    "difuse += texture(sky, pos + incVec);"
    "incVec = vec3(0.0, inc, 0.0);" 
    "difuse += texture(sky, pos + incVec);"
    "incVec = vec3(0.0, -inc, 0.0);" 
    "difuse += texture(sky, pos + incVec);"

    "difuse *= 0.1;"
    "color = difuse;}");



SkyBox::SkyBox()
{
    _program = new RenderProgram(vSkySource, "", fSkySource);
    _uView = glGetUniformLocation(_program->id, "view");
    _uProj = glGetUniformLocation(_program->id, "proj");
    std::string texPath(DATAPATH);
    texPath.append("/img/cells");
    _texture = new Texture(texPath);
   

    uint32_t numPos = 72;
    float pos[numPos];
    pos[0] = -1.0f; pos[1] = 1.0f; pos[2] = -1.0f;
    pos[3] = -1.0f; pos[4] = -1.0f; pos[5] = -1.0f;
    pos[6] = 1.0f; pos[7] = -1.0f; pos[8] = -1.0f;
    pos[9] = 1.0f; pos[10] = 1.0f; pos[11] = -1.0f;

    pos[12] = -1.0f; pos[13] = 1.0f; pos[14] = 1.0f;
    pos[15] = -1.0f; pos[16] = -1.0f; pos[17] = 1.0f;
    pos[18] = -1.0f; pos[19] = -1.0f; pos[20] = -1.0f;
    pos[21] = -1.0f; pos[22] = 1.0f; pos[23] = -1.0f;

    pos[24] = 1.0f; pos[25] = 1.0f; pos[26] = -1.0f;
    pos[27] = 1.0f; pos[28] = -1.0f; pos[29] = -1.0f;
    pos[30] = 1.0f; pos[31] = -1.0f; pos[32] = 1.0f;
    pos[33] = 1.0f; pos[34] = 1.0f; pos[35] = 1.0f;


    pos[36] = 1.0f; pos[37] = 1.0f; pos[38] = 1.0f;
    pos[39] = 1.0f; pos[40] = -1.0f; pos[41] = 1.0f;
    pos[42] = -1.0f; pos[43] = -1.0f; pos[44] = 1.0f;
    pos[45] = -1.0f; pos[46] = 1.0f; pos[47] = 1.0f;

    pos[48] = -1.0f; pos[49] = 1.0f; pos[50] = 1.0f;
    pos[51] = -1.0f; pos[52] = 1.0f; pos[53] = -1.0f;
    pos[54] = 1.0f; pos[55] = 1.0f; pos[56] = -1.0f;
    pos[57] = 1.0f; pos[58] = 1.0f; pos[59] = 1.0f;

    pos[60] = -1.0f; pos[61] = -1.0f; pos[62] = -1.0f;
    pos[63] = -1.0f; pos[64] = -1.0f; pos[65] = 1.0f;
    pos[66] = 1.0f; pos[67] = -1.0f; pos[68] = 1.0f;
    pos[69] = 1.0f; pos[70] = -1.0f; pos[71] = -1.0f;



    _numIndices = 36;
    uint32_t indices[_numIndices];
    indices[0] = 0; indices[1] = 1; indices[2] = 2;
    indices[3] = 0; indices[4] = 2; indices[5] = 3;

    indices[6] = 4; indices[7] = 5; indices[8] = 6;
    indices[9] = 4; indices[10] = 6; indices[11] = 7;

    indices[12] = 8; indices[13] = 9; indices[14] = 10;
    indices[15] = 8; indices[16] = 10; indices[17] = 11;

    indices[18] = 12; indices[19] = 13; indices[20] = 14;
    indices[21] = 12; indices[22] = 14; indices[23] = 15;

    indices[24] = 16; indices[25] = 17; indices[26] = 18;
    indices[27] = 16; indices[28] = 18; indices[29] = 19;

    indices[30] = 20; indices[31] = 21; indices[32] = 22;
    indices[33] = 20; indices[34] = 22; indices[35] = 23;


    glGenVertexArrays(1, &_vao);
    unsigned int vbos[2];
    glGenBuffers(2, vbos);
    
    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numPos, pos,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                sizeof(unsigned int) * _numIndices, indices,
                GL_STATIC_DRAW);

    glBindVertexArray(0);


}

void SkyBox::render(Camera* camera)
{
    glDepthMask(GL_FALSE);
    _program->use();
    Eigen::Matrix4f view = camera->viewMatrix().cast<float>();    
    Eigen::Matrix4f proj = camera->projectionMatrix().cast<float>();

    glUniformMatrix4fv(_uView, 1, GL_FALSE, view.data());        
    glUniformMatrix4fv(_uProj, 1, GL_FALSE, proj.data()); 
    glBindVertexArray(_vao);        
    glActiveTexture(GL_TEXTURE0);       
    glBindTexture(GL_TEXTURE_CUBE_MAP, _texture->id);
    glDrawElements(GL_TRIANGLES, _numIndices, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glDepthMask(GL_TRUE);
}

}
