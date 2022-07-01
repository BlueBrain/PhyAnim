# pip3 install glfw
# pip3 install PyOpenGL PyOpenGL_accelerate

import copy
import glfw
import glfw.GLFW as GLFWCONST
from numpy import float32
from render import *


class MorphoRender:

    def __init__(self):
        self.width = 1000
        self.height = 600

        if not self.init_window():
            return None
        glfw.set_key_callback(self.window, self.key_callback)

        glfw.make_context_current(self.window)
        self.engine = Engine(
            self.width, self.height, Vec3(255.0, 0.2, 0.8))

    def init_window(self):
        if not glfw.init():
            return False

        glfw.window_hint
        glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 3)
        glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 3)
        glfw.window_hint(glfw.OPENGL_FORWARD_COMPAT, GL_TRUE)
        glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)

        self.window = glfw.create_window(
            self.width, self.height, "Morphology Render", None, None)

        if self.window is None:
            print("Window creation failed")
            glfw.terminate()
            return False

        print("Window created with size " +
              str(self.width) + "x" + str(self.height))
        return True

    def run(self):
        glfw.make_context_current(self.window)

        while not glfw.window_should_close(self.window):
            glfw.poll_events()
            self.engine.render()
            glfw.swap_buffers(self.window)
        glfw.terminate()

    def key_callback(self, window, key, scancode, action, mods):
        if key == glfw.KEY_SPACE:
            if action == glfw.PRESS or action == glfw.REPEAT:
                print("BOOM!!")


if __name__ == "__main__":
    a = Vec3(1.0, 2.0, 3.0)
    ac = Vec3(a)
    
    print(a)

    a = Vec3(a)
    print(a)
    print(Vec3(1))
    print(Vec3(np.array((1,3), dtype=float32)))

    print(Vec3(1,2,3))
    print(a+a)

    print(a-a)
    print(a*10)
    print(a/10)

    print(a.dot(a))
    print(a.norm())
    print(a.normalized())

    # b = Vec3(1.0, 2.0, 1.0)
    

    # print ("a==ac -> " + str(a==ac))

    # print ("a!=ac -> " + str(a!=ac))
    # print ("a==b -> " + str(a==b))
    # print ("a!=b -> " + str(a!=b))

    # print(a+b)
    # print(a-b)
    # print(a*20)
    # print(a/2)
    # print(a.dot(b))

    # print(a.norm())
    # b = a.normalized()
    # print(b)


    # print(type(a.data))
    # print(a.data)



    # b.normalize()
    # print(str(type(b)) + ": " + str(b))

    # b = Vec3()
    # b.normalize()
    # print(b)

    morphoRender = MorphoRender()
    morphoRender.run()
