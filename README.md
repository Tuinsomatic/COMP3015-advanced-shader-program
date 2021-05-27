# COMP3015-advanced-shader-program

Visual Studio Version: 2019 16.8.5
Operating System:      Windows 10

How does it work?
-main.cpp calls upon SceneBasic_Uniform to make a scene
-Shader programs are compiled
-Scene is initialised by allocating memory buffers to various data and setting uniform variables to be used by shaders
-For every frame, every position is rendered by every vertex shader in a layered structure.
-Each vertex shader passes on its current open position to its corresponding fragment shader which outputs a final colour to display at each pixel on the screen.
-Vertex and fragment shaders in this program do the following things:
1. Texture a custom model and a plane
2. Shine a spotlight on the models
3. Create a non-permanent particle fountain
4. Create a cloudy noise effect at the starting position of the particle fountain
5. Create a skybox

My shader program was imspired by Elia, as mentioned in my demonstration video. Once I learned how to implement everything into the program, I messed around with them to suit the project best. I also removed lots of code that I wouldn't need to achieve the visuals I had.
I attempted a PBR effect on the model too but couldn't get it working in time for the deadline.
My program is unique because of the alterations I made to various shader techniques, such as removing gravity from the particle fountain or implementing the cloud effect as part of the 3D scene instead of a visual affect to the eyes. The concept of an abandoned turret suddenly firing is unique too. But most of the shader techniques I used can be found in many other projects, which makes the project rather less unique.

Github repository: https://github.com/Tuinsomatic/COMP3015-advanced-shader-program

Unlisted Youtube code walkthrough: https://youtu.be/WXIoOxPCxXM
Youtube video transcript:          https://drive.google.com/file/d/19OwR7mT3tEXmcIuLKajvy2M7mWX9M5B2/view?usp=sharing
