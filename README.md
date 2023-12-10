# REAL ENGINE 5

### (2nd Assignment Features down below!!)

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

### Features

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

### Additional funcionality
An image of the texture is displayed in the Inspector Window.
The engine supports docking.

### Additional Comments
We have managed to carry out all the assignments, the only one that we have not been able to do was the display of the vertex normals, on the other hand, the display of the face normals it's done.

### 2ndFeatures

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

### Additional Comments
We have not managed to do:
- CFF
- Scene Serialization
- Proper Resource Manager

## 3rd party libraries used

- SDL 2.0.4: https://www.libsdl.org/

- ImGui 1.90 WIP: https://github.com/ocornut/imgui/tree/docking

- Glew 2.1.0: https://glew.sourceforge.net/

- MathGeoLib 1.0: https://github.com/juj/MathGeoLib

- OpenGL: https://www.opengl.org/

- Assimp: https://assimp.org/

- PhysFS 3.0.2: https://icculus.org/physfs/

- DevIL 1.8.0: https://github.com/DentonW/DevIL
