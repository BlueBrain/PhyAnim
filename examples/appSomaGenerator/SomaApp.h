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

#ifndef __EXAMPLES_SOMA_APP__
#define __EXAMPLES_SOMA_APP__

#include "../common/GLFWApp.h"
// #include "SomaGenerator.h"

namespace examples
{
class SomaApp : public GLFWApp
{
public:
    SomaApp(int argc, char** argv);

protected:
    void _actionLoop();

    void _keyCallback(GLFWwindow* window,
                      int key,
                      int scancode,
                      int action,
                      int mods);

    bool _anim;

    std::mutex _animMutex;
};

}  // namespace examples

#endif
