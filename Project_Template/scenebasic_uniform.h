#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "helper/plane.h"
#include "helper/objmesh.h"

#include "helper/texture.h"

#include "helper/skybox.h"
#include "helper/random.h"
#include "helper/grid.h"



class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog, flatProg, fundProg;
    Random rand;
    GLSLProgram skyShader, cloudProg;
    GLuint initialVelocity, startTime, particles, nParticles, quad;
    //Grid grid;
    glm::mat4 rotationMatrix;
    glm::vec3 emitterPos, emitterDir, lightPos;
    Plane plane;
    std::unique_ptr<ObjMesh> mesh;
    SkyBox sky;
    float angle, tPrev, rotSpeed, time, particleLifeTime;

    void setMatrices();
    void compile();
    void initBuffers();
    float randFloat();
    void setMatrices(GLSLProgram&);
    void drawNoise();

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void setSkyBox();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
