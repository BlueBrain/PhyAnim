# pip install glfw numpy PyOpenGL PyOpenGL_accelerate

import copy
from math import ceil
from threading import currentThread
import glfw
from numpy import float32
from render.render import *
from data.morphology import *
from data.loaders import *
import sys
import os
import platform
import time


class MorphoRender:

    def __init__(self, paths):
        self.width = 600
        self.height = 600

        self.translate = False
        self.rotate = False
        self.x = 0
        self.y = 0
        self.shift = False
        if not self.__init_window():
            return None
        glfw.set_key_callback(self.window, self.key_callback)
        glfw.set_window_size_callback(self.window, self.resize_callback)
        glfw.set_scroll_callback(self.window, self.scroll_callback)
        glfw.set_mouse_button_callback(self.window, self.mouse_button_callback)
        glfw.set_cursor_pos_callback(
            self.window, self.cursor_position_callback)

        glfw.make_context_current(self.window)
        print(glGetString(GL_VERSION))

        self.background_color = Vec3(1)
        self.scene = Scene()

        # Shaders init

        program_triangles = ShaderProgram([("shaders/triangles.vert", ShaderType.VERTEX),
                                          ("shaders/triangles.frag", ShaderType.FRAGMENT)], GL_TRIANGLES)
        program_triangles_tess = ShaderProgram(
            [("shaders/triangles_tess.vert", ShaderType.VERTEX),
             ("shaders/triangles_tess.tc", ShaderType.TESS_CONTROL),
             ("shaders/triangles_tess.te", ShaderType.TESS_EVALUATION),
             ("shaders/triangles_tess.frag", ShaderType.FRAGMENT)], GL_PATCHES)

        program_triangles_tess_const = ShaderProgram(
            [("shaders/triangles_tess_const.vert", ShaderType.VERTEX),
             ("shaders/triangles_tess.tc", ShaderType.TESS_CONTROL),
             ("shaders/triangles_tess.te", ShaderType.TESS_EVALUATION),
             ("shaders/triangles_tess.frag", ShaderType.FRAGMENT)], GL_PATCHES)

        program_lines = ShaderProgram([("shaders/lines.vert", ShaderType.VERTEX),
                                       ("shaders/lines.frag", ShaderType.FRAGMENT)], GL_LINES)
        program_background = ShaderProgram([("shaders/background.vert", ShaderType.VERTEX),
                                            ("shaders/background.frag", ShaderType.FRAGMENT)], GL_TRIANGLES)

        self.scene.add_model((QuadMesh(), program_background, Mat4()))

        total_paths = float(len(paths))
        num_paths = 0
        mesh_loaded = 0
        displaceX = 0.0
        displaceY = 0.0
        meshes_per_row = ceil(math.sqrt(len(paths)))
        prev_time = time.time()
        for path in paths:
            num_paths += 1
            filename, file_extension = os.path.splitext(path)
            if file_extension == ".swc":
                # t = time.time()
                morpho = load_swc(path)
                # now = time.time()
                # print("Morphology load in: " +
                #       "{: .2f}".format(now - t) + " seconds")
                # t = time.time()
                mesh = mesh_from_morpho(morpho)
                r = mesh.aabb.radius()
                # displaceX += r
                self.scene.add_model((mesh, program_triangles_tess,
                                      Mat4([1, 0, 0, displaceX,
                                            0, 1, 0, displaceY-r,
                                            0, 0, 1, 0,
                                            0, 0, 0, 1])))
                # now = time.time()
                # print("Mesh reconstructed in: " +
                #       "{: .2f}".format(now - t) + " seconds")
                mesh = soma_mesh_from_morpho(morpho)
                self.scene.add_model((mesh, program_triangles_tess_const,
                                      Mat4([1, 0, 0, displaceX,
                                            0, 1, 0, displaceY-r,
                                            0, 0, 1, 0,
                                            0, 0, 0, 1])))

                # displaceX += r
                mesh_loaded += 1
            elif file_extension == ".obj":
                mesh = load_obj(path)
                if mesh is not None:
                    displaceX += mesh.aabb.radius()
                    self.scene.add_model((mesh, program_triangles,
                                          Mat4([1, 0, 0, displaceX,
                                                0, 1, 0, displaceY-mesh.aabb.radius(),
                                                0, 0, 1, 0,
                                                0, 0, 0, 1])))

                    displaceX += mesh.aabb.radius()
                    mesh_loaded += 1
            if mesh_loaded % meshes_per_row == 0:
                # displaceY = self.scene.aabb.min.y
                displaceX = 0.0
            print("\rLoading meshes " +
                  str(int(num_paths/total_paths*100))+"%", end='')

        print()
        num_triangles = 0
        for model in self.scene.models:
            num_triangles += model[0].num_triangles/3
        print("Loaded " + str(mesh_loaded) + " meshes with " +
              str(num_triangles/1000.0) + "K triangles in " +
              "{:.2f}".format(time.time() - prev_time) + " seconds")
        self.scene.distance = 50.0
        self.scene.level = 10

    def __init_window(self):
        if not glfw.init():
            return False
        glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 4)
        glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 1)
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
        glfw.swap_interval(0)
        fps = 0
        prev_time = time.time()
        while not glfw.window_should_close(self.window):
            self.scene.render()
            fps += 1
            current_time = time.time()
            step = current_time - prev_time
            if step > 1.0:
                prev_time = current_time
                print("\rFPS: " + str(int(fps/step)) + "    lod: " +
                      str(self.scene.level) + "    distance: " +
                      str(self.scene.distance) + "    ", end='')
                fps = 0
            glfw.swap_buffers(self.window)
            glfw.poll_events()
        glfw.terminate()

    def key_callback(self, window, key, scancode, action, mods):

        if action == glfw.PRESS or action == glfw.REPEAT:
            r = self.scene.radius * 0.05
            rot = self.scene.rotation.transpose()
            if key == glfw.KEY_D:
                self.scene.target += rot*Vec3(r, 0, 0)
            elif key == glfw.KEY_A:
                self.scene.target += rot*Vec3(-r, 0, 0)
            elif key == glfw.KEY_W:
                self.scene.target += rot*Vec3(0.0, 0, -r)
            elif key == glfw.KEY_S:
                self.scene.target += rot*Vec3(0.0, 0, r)
            elif key == glfw.KEY_M:
                self.scene.render_mode()
            elif key == glfw.KEY_LEFT_SHIFT:
                self.shift = True
            elif key == glfw.KEY_UP:
                self.scene.level += 1
            elif key == glfw.KEY_DOWN:
                self.scene.level -= 1
            elif key == glfw.KEY_RIGHT:
                self.scene.distance *= 1.1
            elif key == glfw.KEY_LEFT:
                self.scene.distance *= 0.9
        elif action == glfw.RELEASE:
            if key == glfw.KEY_LEFT_SHIFT:
                self.shift = False

    def scroll_callback(self, window, x, y):
        if (y > 0):
            self.scene.radius *= 0.9
        else:
            self.scene.radius *= 1.1

    def mouse_button_callback(self, window, button, action, mods):
        if button == glfw.MOUSE_BUTTON_LEFT or button == glfw.MOUSE_BUTTON_MIDDLE:
            if action == glfw.PRESS:
                if self.shift:
                    self.rotate = True
                else:
                    self.translate = True
                (self.x, self.y) = glfw.get_cursor_pos(window)
            else:
                self.rotate = False
                self.translate = False
        if button == glfw.MOUSE_BUTTON_RIGHT:
            if action == glfw.PRESS:
                self.rotate = True
                (self.x, self.y) = glfw.get_cursor_pos(window)
            else:
                self.rotate = False

    def cursor_position_callback(self, window, x, y):
        x_diff = x - self.x
        y_diff = y - self.y
        if self.translate:
            r = self.scene.radius * 0.005
            rot = self.scene.rotation.transpose()
            self.scene.target += rot * Vec3(-r*x_diff, r*y_diff, 0)
        elif self.rotate:
            rot = rotation_from_yaw_pitch(x_diff*0.01, y_diff*0.01)
            self.scene.rotation = rot * self.scene.rotation

        self.x = x
        self.y = y

    def resize_callback(self, window, width, height):
        glfw.make_context_current(self.window)
        self.scene.ratio = float(width)/height
        if platform.system() != 'Darwin':
            glViewport(0, 0, width, height)


if __name__ == "__main__":

    if len(sys.argv) < 2:
        print("Error. Use: " + sys.argv[0] + " file[.swc]")
        exit()
    morphoRender = MorphoRender(sys.argv[1:])

    morphoRender.run()
