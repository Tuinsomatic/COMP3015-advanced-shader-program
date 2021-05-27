#include <sstream>

#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

#include <iostream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"

#include "helper/torus.h"
#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include<glad/glad.h>

#include "helper/particleutils.h"
#include "helper/noisetex.h"
using glm::vec3;
using glm::vec4;
using glm::mat4;

//THE FOLLOWING CODE IS FOR A FUTURISTIC TURRET LEFT TO DECAY ON MOORLAND AFTER A WAR

SceneBasic_Uniform::SceneBasic_Uniform() : plane(8.0f, 8.0f, 100, 100), angle(0.0f), tPrev(0.0f), rotSpeed(glm::pi<float>() / 8.0f), time(0), particleLifeTime(5.5f), nParticles(8000), 
                                           emitterPos(0, -0.5, -2.4), emitterDir(0, 0, -2), sky(100.0f){
    mesh = ObjMesh::load("media/turret.obj", true); //Constructor: set parameters for plane (size 8x8), camera angle, time variable, camera rotation speed, skybox
}

void SceneBasic_Uniform::initScene()
{
    compile();
    glEnable(GL_BLEND); //enable alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST); //enable depth comparisons and update the depth buffer

    //view = glm::lookAt(vec3(-3.0f, 1.5f, -5.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = mat4(1.0f); //create projection matrix.

    angle = glm::radians(90.0f); //set camera angle to 90*(pi/180) radians - equivalent of 90 degrees

    GLfloat verts[] = { -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f };
    GLfloat tc[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f }; 

    initBuffers(); //for particle emitter

    //the following code is for noise-----------------------------------------------------------
    cloudProg.use();
    unsigned int handle[2]; //buffers
    glGenBuffers(2, handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

    glGenVertexArrays(1, &quad); //VAOs
    glBindVertexArray(quad);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
    glEnableVertexAttribArray(0); //Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
    glEnableVertexAttribArray(2); //Texture Position

    glBindVertexArray(0);

    cloudProg.setUniform("NoiseTex", 0);

    GLuint noiseTex = NoiseTex::generate2DTex(6.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, noiseTex);
//-----------------------------------------------------------------------------------------------

    prog.use();
    //float x, z;
    //x = 2.0f * cosf((glm::two_pi<float>() / 3));
    //z = 2.0f * sinf((glm::two_pi<float>() / 3));
    //prog.setUniform("Light.Position", view * glm::vec4(x, 1.2f, z + 1.0f, 1.0f));
    //prog.setUniform("Light.L", vec3(0.3f, 0.3f, 1.8f));
    //prog.setUniform("Light.La", glm::vec3(1.0f, 1.0f, 1.0f));

    prog.setUniform("Spot.L", vec3(0.9f)); //set spotlight uniforms for shaders. L = diffuse & specular light intensity | La = ambient light intensity
    prog.setUniform("Spot.La", vec3(0.25f)); //Exponent = angular attenuation exponent | Cutoff = angle in which the spotlight stops emitting light
    prog.setUniform("Spot.Exponent", 15.0f);
    prog.setUniform("Spot.Cutoff", glm::radians(3.0f));  

    GLuint texID1 = Texture::loadTexture("media/texture/Metal.jpg"); //Load in the metal and mossy textures for the turret model and plane
    GLuint texID2 = Texture::loadTexture("media/texture/moss.png");
    
    glActiveTexture(GL_TEXTURE0); //make texture unit 0 active and bind metal to it
    glBindTexture(GL_TEXTURE_2D, texID1);

    glActiveTexture(GL_TEXTURE1); //do the same for texture 1 with moss
    glBindTexture(GL_TEXTURE_2D, texID2);

    glActiveTexture(GL_TEXTURE0);
    Texture::loadTexture("media/texture/fire.png");
    //glBindTexture(GL_TEXTURE_2D, particleTex);

    GLuint cubeTex = Texture::loadCubeMap("media/texture/cube/pisa/pisa"); //load in the dartmoor skybox (i didnt change the file names)
    glActiveTexture(GL_TEXTURE0); //make texture unit 0 active and bind the skybox to it
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);

    flatProg.use();
    flatProg.setUniform("ParticleTex", 0);
    flatProg.setUniform("LifeTime", particleLifeTime);
    flatProg.setUniform("Size", 1.0f);
    flatProg.setUniform("Gravity", vec3(0.0f, 0.0f, 0.0f));
    flatProg.setUniform("Position", emitterPos);

    fundProg.use();
    fundProg.setUniform("Color", glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));
}

void SceneBasic_Uniform::initBuffers() {
    glGenBuffers(1, &initialVelocity); //initial velity and start time buffers
    glGenBuffers(1, &startTime);

    int size = nParticles * sizeof(float); //allocate space for the buffers
    glBindBuffer(GL_ARRAY_BUFFER, initialVelocity);
    glBufferData(GL_ARRAY_BUFFER, size * 3, 0, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, startTime);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_STATIC_DRAW);
    
    //-----------------------------------------------------------------------------------------------------------------------
    glm::mat3 emitter = ParticleUtils::makeArbitraryBasis(emitterDir); //fill first velocity buffer with random values
    vec3 v(0.0f);
    float velocity, theta, phi;
    std::vector<GLfloat> data(nParticles * 3);
    for (uint32_t i = 0; i < nParticles; i++) {
        theta = glm::mix(0.0f, glm::pi<float>() / 20.0f, randFloat()); //choose random angle of projection within a cone shape
        phi = glm::mix(0.0f, glm::two_pi<float>(), randFloat());
        v.x = sinf(theta) * cosf(phi);
        v.y = cosf(theta);
        v.z = sinf(theta) * sinf(phi);

        velocity = glm::mix(1.25f, 1.5f, randFloat()); //set magnitude of velocity
        v = glm::normalize(emitter * v) * velocity;

        data[3 * i] = v.x; //fill in each value of the particle's vectors individually
        data[3 * i + 1] = v.y;
        data[3 * i + 2] = v.z;
    }

    glBindBuffer(GL_ARRAY_BUFFER, initialVelocity);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size * 3, data.data());
    //-----------------------------------------------------------------------------------------------------------------------
    float rate = particleLifeTime / nParticles; //fill in start time buffers - particles should generate one after the other
    for (int i = 0; i < nParticles; i++) {
        data[i] = rate * i;
    }
    glBindBuffer(GL_ARRAY_BUFFER, startTime);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &particles); //create and bind the particle system's buffers and VAOs
    glBindVertexArray(particles);
    glBindBuffer(GL_ARRAY_BUFFER, initialVelocity);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, startTime);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);

    glBindVertexArray(0);
}

float SceneBasic_Uniform::randFloat() {
    return rand.nextFloat();
}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/basic_uniform.vert"); //compile the program shaders and skybox shaders into the prog and skyShader GLSL programs
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();

        flatProg.compileShader("shader/flat_vert.glsl");
        flatProg.compileShader("shader/flat_frag.glsl");
        flatProg.link();

        fundProg.compileShader("shader/flat_fund_vert.glsl");
        fundProg.compileShader("shader/flat_fund_frag.glsl");
        fundProg.link();

        cloudProg.compileShader("shader/noise_vert.glsl");
        cloudProg.compileShader("shader/noise_frag.glsl");
        cloudProg.link();

        skyShader.compileShader("shader/sky.vert");
        skyShader.compileShader("shader/sky.frag");
        skyShader.link();
	} catch (GLSLProgramException &e) { //catch any errors involving a lack of either GLSL programs or specified shaders.
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
    std::printf("compiled!!!!!!!!!!");
}

void SceneBasic_Uniform::update( float t )
{
    float deltaT = t - tPrev; //rotate the camera every frame
    if (tPrev == 0.0f) {
        deltaT = 0.0f;
    }
    tPrev = t;
    angle += rotSpeed * deltaT;

    if (angle > glm::two_pi<float>()) {
        angle -= glm::two_pi<float>();
    }

    time = t;
    angle = std::fmod(angle + 0.01f, glm::two_pi<float>());
}

void SceneBasic_Uniform::render()
{
    prog.use();                                                            //use our model shader program

    glClear(GL_COLOR_BUFFER_BIT); //clear the colour and depth buffers (can also be written as glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);)
    glClear(GL_DEPTH_BUFFER_BIT);

    vec3 cameraPos = vec3(7.0f * cos(angle), 2.0f, 7.0f * sin(angle)); //set camera position using the cosine and sine of the camera angle
    view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, //create view matrix to be in the position of the camera every frame
        0.0f));                                                            //with centre coordinates being (0,0,0) and no tilt

    glm::vec4 spotPos = glm::vec4(0.0f, 50.0f, 0.0f, 1.0f); //set the position of the spotlight (i set it slightly off centre)
    prog.setUniform("Spot.Position", vec3(view * spotPos)); //set the spotlight position uniform as the spotlight position relative to the camera

    glm::mat3 normalMatrix = glm::mat3(vec3(view[0]), vec3(view[1]), vec3(view[2])); //set a normal matrix and use it to calculate the direction of the spotlight
    prog.setUniform("Spot.Direction", normalMatrix * vec3(-spotPos));                //relative to the camera

    prog.setUniform("Material.Ka", 0.5f, 0.55f, 0.5f); //surface reflectivity (rgb)
    prog.setUniform("Material.Kd", 0.2f, 0.55f, 0.9f); //diffuse reflectivity (rgb)
    prog.setUniform("Material.Ks", 0.4f, 0.4f, 0.4f); //specular reflectivity (rgb)
    prog.setUniform("Material.Shininess", 15.0f); //turret shininess (less = more)

    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f)); //create a model matrix and rotate it 90*(pi/180) radians - equivalent of 90 degrees
    setMatrices();
    mesh->render();

    prog.setUniform("Material.Ka", 0.1f, 0.4f, 0.1f);
    prog.setUniform("Material.Kd", 0.1f, 0.1f, 0.1f);
    prog.setUniform("Material.Ks", 0.1f, 0.1f, 0.1f);
    prog.setUniform("Material.Shininess", 100.0f);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -1.5f, 0.0f)); //move plane down in world space
    setMatrices();
    plane.render();

    model = mat4(1.0f);

    setSkyBox();

    fundProg.use();
    setMatrices(fundProg);

    glDepthMask(GL_FALSE);
    flatProg.use();
    setMatrices(flatProg);
    //grid.render();
    flatProg.setUniform("Time", time);
    glBindVertexArray(particles);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticles);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);

    //view = mat4(1.0f);
    drawNoise();
    glFinish();
}

void SceneBasic_Uniform::drawNoise() {
    model = mat4(1.0f);
    mat4 mv = view * model;
    cloudProg.setUniform("MVP", projection * mv);

    glBindVertexArray(quad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void SceneBasic_Uniform::resize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0,0,w,h);
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatrices() {
    glm::mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv); //multiply model * view to get model-view matrix, set uniform
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2]))); //set normal matrix using MV
    prog.setUniform("MVP", projection * mv); //calculate MVP by multiplying MV with projection    
}

void SceneBasic_Uniform::setMatrices(GLSLProgram& p) {
    glm::mat4 mv = view * model;
    p.setUniform("MV", mv);
    p.setUniform("Proj", projection);
}

void SceneBasic_Uniform::setSkyBox() {
    skyShader.use(); //use skybox shader program
    model = mat4(1.0f); //create model matrix
    mat4 mv = view * model; //create MV
    skyShader.setUniform("MVP", projection * mv); //set MVP uniform
    sky.render(); //render the skybox
}
