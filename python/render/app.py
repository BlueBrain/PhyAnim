import glfw
from render.render import *
import time
import platform


class App:

    def __init__(self):
        self.width = 600
        self.height = 600
        if not self.__init_window():
            return None

        self.__init_callbacks()
        self.scene = Scene()
        self.iMat = Mat4()
        self.iMat.identity()

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
        glfw.make_context_current(self.window)
        return True

    def __init_callbacks(self):
        self.translate = False
        self.rotate = False
        self.x = 0
        self.y = 0
        self.shift = False
        glfw.set_key_callback(self.window, self.__key_callback)
        glfw.set_window_size_callback(self.window, self.__resize_callback)
        glfw.set_scroll_callback(self.window, self.__scroll_callback)
        glfw.set_mouse_button_callback(
            self.window, self.__mouse_button_callback)
        glfw.set_cursor_pos_callback(
            self.window, self.__cursor_position_callback)

    def __key_callback(self, window, key, scancode, action, mods):
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

    def __scroll_callback(self, window, x, y):
        if (y > 0):
            self.scene.radius *= 0.9
        else:
            self.scene.radius *= 1.1

    def __mouse_button_callback(self, window, button, action, mods):
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

    def __cursor_position_callback(self, window, x, y):
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

    def __resize_callback(self, window, width, height):
        glfw.make_context_current(self.window)
        self.scene.ratio = float(width)/height
        if platform.system() != 'Darwin':
            glViewport(0, 0, width, height)

    def set_background(self):
        program = ShaderProgram([("shaders/background.vert", ShaderType.VERTEX),
                                 ("shaders/background.frag", ShaderType.FRAGMENT)], GL_TRIANGLES)
        self.scene.add_model((QuadMesh(), program, self.iMat))

    def add_models(self, params=None):
        program = ShaderProgram(
            [("shaders/quads_tess_const.vert", ShaderType.VERTEX),
             ("shaders/quads_tess.tesc", ShaderType.TESS_CONTROL),
             ("shaders/quads_tess.tese", ShaderType.TESS_EVALUATION),
             ("shaders/quads_tess.frag", ShaderType.FRAGMENT)], GL_PATCHES)
        self.scene.add_model((SphereMesh(), program, self.iMat))
        self.scene.distance = 50.0
        self.scene.level = 60

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
