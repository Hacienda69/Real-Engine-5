# REAL ENGINE 5

De la mano del aclamado estudio "Aprobé Motores", formado por los brillantes desarrolladores Marc Escandell y Daniel Mañas, en una sorprendente y extraordinaria colaboración (les forkeamos) con SheeeshEngine, otro motor excepcionalmente innovador creado por unos compañeros de clase, Oriol Martín (@Urii98: https://github.com/Urii98) y Xiao Shan Costajussà (@xaitta: https://github.com/xaitta), llega "Real Engine 5", un nuevo motor de videojuegos puntero en la pujante industria del entretenimiento virtual, esperamos que lo disfruten.

Link del tenedor original: https://github.com/Urii98/SheeeshEngine

Now in Engish: 

From the hand of the acclaimed studio "Aprobé Motores" ("I passed engines"), formed by the brilliant developers Marc Escandell and Daniel Mañas, in a surprising and extraordinary collaboration (we forked them) with SheeeshEngine, another exceptionally innovative engine created by some classmates, Oriol Martín (@Urii98: https://github.com/Urii98) and Xiao Shan Costajussà (@xaitta: https://github.com/xaitta), "Real Engine 5" arrives, a new cutting-edge video game engine in the booming virtual entertainment industry, we hope you enjoy it.

Original fork link: https://github.com/Urii98/SheeeshEngine

### About the authors
- Dani's GitHub account: [Dani](https://github.com/Hacienda69)
- Marc's GitHub account: [Marc](https://github.com/MrMonkey420)
- Our GitHub project: [Real Engine 5](https://github.com/Hacienda69/Real-Engine-5)


## Engine Description

The Real Engine 5 is a video game engine developed as a university project for the third-year subject of Video Game Engines. In this subject we have had to develop an engine implementing (or trying) all the functionalities that were asked of us. There have been three deliveries, each of which has demanded certain implementations, the most important of all being the last, in which each class group was assigned a high-level system in which to specialize their engine, ours being 3D physics. Below we leave as a list all the features that our engine has, separated by the delivery in which they were ordered. Below are some visual examples of the capabilities and uses of the engine and finally all the libraries that we have used.

### Basic Features

#### Game Objects
- Drag and drop game objects and textures in the Hierarchy.
- Create game objects with Game Objects Menu
- Delete game objects by pressing Delete at Inspector
- Delete selected game object at Hierarchy by pressing Delete.
- Move Game Ojects in Hierarchy by drag and drop them.

### Controls

#### Camera
- While Right clicking:
	- WASD: move forward, left, backward and right
	- QE: move up and down
	- Look around
- F: look at target
- Hold Alt:
	- Mouse Hold Left to orbit the game object selected.
- Mouse wheel to zoom in and zoom out.
- Holding SHIFT duplicates movement speed.

### 2nd Assignment Features

#### Misc.
- The ‘Street Environment’ scene is automatically loaded at the start.

#### Game Objects
- The user is able to use the inspector to modify a GameObject:
	- Hierarchy: delete, reparent, create empty and create children
	- Transform: translate, rotate and scale Game Objects.
	- Mesh: select or drop any imported mesh
	- Texture: select or drop any imported texture
	- Camera is a component with settings that can be modified.

- GameObjects can be picked from the world using the mouse.

- All meshes use a bounding volume (AABB) and can be discarded using Frustum Culling. This process is visualized in the editor.

#### Scenes
- The user can Start / Pause / Stop the simulation and recover its original state.

- The editor has 2 windows one with the scene(editor view) and another captured from a gameobject with a camera component(main game camera), each one has its own framebuffer and the user can visualize both at the same time.

#### Resource Management:
- The user can drop/import new files. Hierarchy window reacts properly
- Asset files can be deleted through the Hierarchy window

### 3rd Assignment features

Short introduction to the engine and its context (university subject).
Team photo and quick explanation of the individual work of each member.
Short explanation of last assignment sub-system, containing two animated gif that shows editor and in-game functionality.
One minute video speed-up that shows the process of authoring scenes in your editor, especially with your core technology from the last assignment (captured from the computer, not mobile).
Link to the code repository and last release to download.

### Additional funcionality
An image of the texture is displayed in the Inspector Window.
The engine supports docking.

## 3rd party libraries used

- SDL 2.0.4: https://www.libsdl.org/

- ImGui 1.90 WIP: https://github.com/ocornut/imgui/tree/docking

- Glew 2.1.0: https://glew.sourceforge.net/

- MathGeoLib 1.0: https://github.com/juj/MathGeoLib

- OpenGL: https://www.opengl.org/

- Assimp: https://assimp.org/

- PhysFS 3.0.2: https://icculus.org/physfs/

- DevIL 1.8.0: https://github.com/DentonW/DevIL

## License

MIT License

Copyright (c) 2023 Hacienda69 & MrMonkey420

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
