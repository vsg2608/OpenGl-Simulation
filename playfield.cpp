    // Include standard headers
    #include <stdio.h>
    #include <stdlib.h>
    #include <vector>
    #include <cstdlib>
    #include <math.h>

    // Include GLEW
    #include <GL/glew.h>
    // Include GLFW
    #include <GLFW/glfw3.h>
    GLFWwindow* window;
    #define GLM_ENABLE_EXPERIMENTAL
    // Include GLM
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
    #include <glm/gtx/transform.hpp>
    #include <glm/gtc/matrix_transform.hpp>
    #include <glm/gtc/quaternion.hpp>
    #include <glm/gtx/quaternion.hpp>
    #include <glm/gtx/euler_angles.hpp>
    #include <glm/gtx/norm.hpp>
    using namespace glm;

    #include "common/shader.cpp"
    #include "common/texture.cpp"
    #include "common/controls.cpp"
    #include "common/objloader.cpp"
    #include "common/vboindexer.cpp"
    #include "common/tangentspace.cpp"

    vec3 gPosition1(0.0f, 0.0f, 0.0f); // Position of terrain
    vec3 gOrientation1;

    vec3 gPosition2( 0.0f, 3.0f, 8.0f); // Position of flag
    quat gOrientation2;

    vec3 gPosition3( 0.0f, .2f, 0.0f); // Position of cyborg
    quat gOrientation3;
    vec3 gPosition4( 0.0f, 0.0f, 0.0f); // Position of sky
    quat gOrientation4;

    float tan_angle = tan(M_PI/12);


    int main( void )
    {
        // Initialise GLFW
        if( !glfwInit() )
        {
            fprintf( stderr, "Failed to initialize GLFW\n" );
            getchar();
            return -1;
        }

        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Open a window and create its OpenGL context
        window = glfwCreateWindow( 1024, 768, "Tutorial 08 - Basic Shading", NULL, NULL);
        if( window == NULL ){
            fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
            getchar();
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);

        // Initialize GLEW
        glewExperimental = true; // Needed for core profile
        if (glewInit() != GLEW_OK) {
            fprintf(stderr, "Failed to initialize GLEW\n");
            getchar();
            glfwTerminate();
            return -1;
        }

        // Ensure we can capture the escape key being pressed below
        glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
        // Hide the mouse and enable unlimited mouvement
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Set the mouse at the center of the screen
        glfwPollEvents();
        glfwSetCursorPos(window, 1024/2, 768/2);

        // Dark blue background
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        // Enable depth test
        glEnable(GL_DEPTH_TEST);
        // Accept fragment if it closer to the camera than the former one
        glDepthFunc(GL_LESS);

        // Cull triangles which normal is not towards the camera
        //glEnable(GL_CULL_FACE);

        GLuint VertexArrayID;
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);

        // Create and compile our GLSL program from the shaders
        GLuint programID = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );

        // Get a handle for our "MVP" uniform
        GLuint MatrixID = glGetUniformLocation(programID, "MVP");
        GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
        GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

        // Load the texture
        GLuint Texture_terrain = loadDDS("objects/ice.dds");
        GLuint Texture_flag = loadDDS("objects/flag.dds");
        GLuint Texture_cyborg = loadDDS("objects/ice.dds");
        GLuint Texture_sky = loadDDS("objects/sky.dds");

        // Get a handle for our "myTextureSampler" uniform
        GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

        // Read our flag object .obj file
        std::vector<glm::vec3> vertices_flag;
        std::vector<glm::vec2> uvs_flag;
        std::vector<glm::vec3> normals_flag;
        bool res1 = loadOBJ("objects/flag.obj", vertices_flag, uvs_flag, normals_flag);

        // Read our terrain object .obj file
        std::vector<glm::vec3> vertices_terrain;
        std::vector<glm::vec2> uvs_terrain;
        std::vector<glm::vec3> normals_terrain;
        bool res2 = loadOBJ("objects/terrain2.obj", vertices_terrain, uvs_terrain, normals_terrain);

        // Read our cyborg object .obj file
        std::vector<glm::vec3> vertices_cyborg;
        std::vector<glm::vec2> uvs_cyborg;
        std::vector<glm::vec3> normals_cyborg;
        bool res3 = loadOBJ("objects/SW Storm trooper1.obj", vertices_cyborg, uvs_cyborg, normals_cyborg);

        // Read our sky object .obj file
        std::vector<glm::vec3> vertices_sky;
        std::vector<glm::vec2> uvs_sky;
        std::vector<glm::vec3> normals_sky;
        bool res4 = loadOBJ("objects/sky.obj", vertices_sky, uvs_sky, normals_sky);



        // Load it into a VBO

        //vbo for terrain
        GLuint vertexbuffer_terrain;
        glGenBuffers(1, &vertexbuffer_terrain);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_terrain);
        glBufferData(GL_ARRAY_BUFFER, vertices_terrain.size() * sizeof(glm::vec3), &vertices_terrain[0], GL_STATIC_DRAW);

        GLuint uvbuffer_terrain;
        glGenBuffers(1, &uvbuffer_terrain);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_terrain);
        glBufferData(GL_ARRAY_BUFFER, uvs_terrain.size() * sizeof(glm::vec2), &uvs_terrain[0], GL_STATIC_DRAW);

        GLuint normalbuffer_terrain;
        glGenBuffers(1, &normalbuffer_terrain);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_terrain);
        glBufferData(GL_ARRAY_BUFFER, normals_terrain.size() * sizeof(glm::vec3), &normals_terrain[0], GL_STATIC_DRAW);

        // VBO for flag
        GLuint vertexbuffer_flag;
        glGenBuffers(1, &vertexbuffer_flag);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_flag);
        glBufferData(GL_ARRAY_BUFFER, vertices_flag.size() * sizeof(glm::vec3), &vertices_flag[0], GL_STATIC_DRAW);

        GLuint uvbuffer_flag;
        glGenBuffers(1, &uvbuffer_flag);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_flag);
        glBufferData(GL_ARRAY_BUFFER, uvs_flag.size() * sizeof(glm::vec2), &uvs_flag[0], GL_STATIC_DRAW);

        GLuint normalbuffer_flag;
        glGenBuffers(1, &normalbuffer_flag);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_flag);
        glBufferData(GL_ARRAY_BUFFER, normals_flag.size() * sizeof(glm::vec3), &normals_flag[0], GL_STATIC_DRAW);

        // VBO for cyborg
        GLuint vertexbuffer_cyborg;
        glGenBuffers(1, &vertexbuffer_cyborg);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_cyborg);
        glBufferData(GL_ARRAY_BUFFER, vertices_cyborg.size() * sizeof(glm::vec3), &vertices_cyborg[0], GL_STATIC_DRAW);

        GLuint uvbuffer_cyborg;
        glGenBuffers(1, &uvbuffer_cyborg);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_cyborg);
        glBufferData(GL_ARRAY_BUFFER, uvs_cyborg.size() * sizeof(glm::vec2), &uvs_cyborg[0], GL_STATIC_DRAW);

        GLuint normalbuffer_cyborg;
        glGenBuffers(1, &normalbuffer_cyborg);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_cyborg);
        glBufferData(GL_ARRAY_BUFFER, normals_cyborg.size() * sizeof(glm::vec3), &normals_cyborg[0], GL_STATIC_DRAW);

        // VBO for sky
        GLuint vertexbuffer_sky;
        glGenBuffers(1, &vertexbuffer_sky);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_sky);
        glBufferData(GL_ARRAY_BUFFER, vertices_sky.size() * sizeof(glm::vec3), &vertices_sky[0], GL_STATIC_DRAW);

        GLuint uvbuffer_sky;
        glGenBuffers(1, &uvbuffer_sky);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_sky);
        glBufferData(GL_ARRAY_BUFFER, uvs_sky.size() * sizeof(glm::vec2), &uvs_sky[0], GL_STATIC_DRAW);

        GLuint normalbuffer_sky;
        glGenBuffers(1, &normalbuffer_sky);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_sky);
        glBufferData(GL_ARRAY_BUFFER, normals_sky.size() * sizeof(glm::vec3), &normals_sky[0], GL_STATIC_DRAW);

        // Get a handle for our "LightPosition" uniform
        glUseProgram(programID);
        GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
        float pos=0;

        do{

            // Clear the screen
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



            // Compute the MVP matrix from keyboard and mouse input
            computeMatricesFromInputs();
            glm::mat4 ProjectionMatrix = getProjectionMatrix();
            glm::mat4 ViewMatrix = getViewMatrix();



            ////// Start of the rendering of the first object //////
            // Use our shader
            glUseProgram(programID);

            glm::vec3 lightPos = glm::vec3(0,250,0);
            glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
            glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]); // This one doesn't change between objects, so this can be done once for all objects that use "programID"

            gOrientation1.y=M_PI/2;
            glm::mat4 ModelMatrix1 = glm::mat4(1.0);
            glm::mat4 TranslationMatrix1 = glm::translate(ModelMatrix1, glm::vec3(gPosition1));
            glm::mat4 RotationMatrix1 = eulerAngleYXZ(gOrientation1.y, gOrientation1.x, gOrientation1.z);
            glm::mat4 ScalingMatrix1 = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
            ModelMatrix1 = TranslationMatrix1 * RotationMatrix1 * ScalingMatrix1;
            glm::mat4 MVP1 = ProjectionMatrix * ViewMatrix * ModelMatrix1;

            // Send our transformation to the currently bound shader,
            // in the "MVP" uniform
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP1[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix1[0][0]);
            glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

            // Bind our texture in Texture Unit 0
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Texture_terrain);
            // Set our "myTextureSampler" sampler to use Texture Unit 0
            glUniform1i(TextureID, 0);

            // 1rst attribute buffer : vertices
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_terrain);
            glVertexAttribPointer(
                0,                  // attribute
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
            );

            // 2nd attribute buffer : UVs
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_terrain);
            glVertexAttribPointer(
                1,                                // attribute
                2,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
            );

            // 3rd attribute buffer : normals
            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_terrain);
            glVertexAttribPointer(
                2,                                // attribute
                3,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
            );

            // Draw the triangles !
            glDrawArrays(GL_TRIANGLES, 0, vertices_terrain.size() );


            ////// End of rendering of the first object //////
            ////// Start of the rendering of the second object //////

            glUseProgram(programID);
      /*    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
            glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
    */
            // Bind our texture in Texture Unit 0
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Texture_flag);
            // Set our "myTextureSampler" sampler to use Texture Unit 0
            glUniform1i(TextureID, 0);
            gPosition2.z=0;
            for(int i=0;i<50;i++)
            {
                // As an example, rotate arount the vertical axis at 180°/sec
                gOrientation2.y = (rand()%3)*3.14159/12 ;
                gPosition2.z+=-10;
                int k=15;
                for(int j=0;j<2;j++)
                {
                    k=k*-1;
                    gPosition2.x=10*sin(0.02*3.14159*gPosition2.z)+k ;
                    gPosition2.y=gPosition2.z*tan_angle+0.2;
                    // BUT the Model matrix is different (and the MVP too)
                    glm::mat4 ModelMatrix2 = glm::mat4(1.0);
                    glm::mat4 TranslationMatrix2 = glm::translate(ModelMatrix2, glm::vec3(gPosition2));
                    glm::mat4 RotationMatrix2 = eulerAngleYXZ(gOrientation2.y, gOrientation2.x, gOrientation2.z);
                    glm::mat4 ScalingMatrix2 = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, .5f, .5f));
                    ModelMatrix2 = TranslationMatrix2 * RotationMatrix2 * ScalingMatrix2;
                    glm::mat4 MVP2 = ProjectionMatrix * ViewMatrix * ModelMatrix2;

                    // Send our transformation to the currently bound shader,
                    // in the "MVP" uniform
                    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP2[0][0]);
                    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix1[0][0]);
                    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);


                    // 1rst attribute buffer : vertices
                    glEnableVertexAttribArray(0);
                    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_flag);
                    glVertexAttribPointer(
                        0,                  // attribute
                        3,                  // size
                        GL_FLOAT,           // type
                        GL_FALSE,           // normalized?
                        0,                  // stride
                        (void*)0            // array buffer offset
                    );

                    // 2nd attribute buffer : UVs
                    glEnableVertexAttribArray(1);
                    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_flag);
                    glVertexAttribPointer(
                        1,                                // attribute
                        2,                                // size
                        GL_FLOAT,                         // type
                        GL_FALSE,                         // normalized?
                        0,                                // stride
                        (void*)0                          // array buffer offset
                    );

                    // 3rd attribute buffer : normals
                    glEnableVertexAttribArray(2);
                    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_flag);
                    glVertexAttribPointer(
                        2,                                // attribute
                        3,                                // size
                        GL_FLOAT,                         // type
                        GL_FALSE,                         // normalized?
                        0,                                // stride
                        (void*)0                          // array buffer offset
                    );


                    // Draw the triangles !
                    glDrawArrays(GL_TRIANGLES, 0, vertices_flag.size() );
                }
            }

            ////// Start of the rendering of the third object //////

            glUseProgram(programID);
      /*      glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
            glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
    */
            // Bind our texture in Texture Unit 0
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Texture_cyborg);
            // Set our "myTextureSampler" sampler to use Texture Unit 0
            glUniform1i(TextureID, 0);

            // As an example, rotate arount the vertical axis at 180°/sec
                //gOrientation3.y = 3.14159f ;

                gPosition3.z+=-0.25;
                gPosition3.x=10*sin(0.02*3.14159*gPosition3.z);
                gPosition3.y=gPosition3.z*tan_angle + .2;
                gOrientation3.x = M_PI/12*(1+0.1*sin(0.02*3.14159*gPosition3.z));
                gOrientation3.z = -M_PI/12*(0.1*sin(0.02*3.14159*gPosition3.z));
                gOrientation3.y = 3*3.14159*(1+ .1*sin(0.02*3.14159*gPosition3.z));
            // BUT the Model matrix is different (and the MVP too)
            glm::mat4 ModelMatrix3 = glm::mat4(1.0);
            glm::mat4 TranslationMatrix3 = glm::translate(ModelMatrix3, glm::vec3(gPosition3));
            glm::mat4 RotationMatrix3 = eulerAngleYXZ(gOrientation3.y, gOrientation3.x, gOrientation3.z);
            glm::mat4 ScalingMatrix3 = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
            ModelMatrix3 = TranslationMatrix3 * RotationMatrix3 * ScalingMatrix3;
            glm::mat4 MVP3 = ProjectionMatrix * ViewMatrix * ModelMatrix3;

            // Send our transformation to the currently bound shader,
            // in the "MVP" uniform
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP3[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix3[0][0]);
            glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);


            // 1rst attribute buffer : vertices
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_cyborg);
            glVertexAttribPointer(
                0,                  // attribute
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
            );

            // 2nd attribute buffer : UVs
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_cyborg);
            glVertexAttribPointer(
                1,                                // attribute
                2,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
            );

            // 3rd attribute buffer : normals
            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_cyborg);
            glVertexAttribPointer(
                2,                                // attribute
                3,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
            );


            // Draw the triangles !
            glDrawArrays(GL_TRIANGLES, 0, vertices_cyborg.size() );

            ////// Start of the rendering of the sky object //////

            glUseProgram(programID);
      /*      glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
            glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
    */
            // Bind our texture in Texture Unit 0
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Texture_sky);
            // Set our "myTextureSampler" sampler to use Texture Unit 0
            glUniform1i(TextureID, 0);

            // As an example, rotate arount the vertical axis at 180°/sec
            // BUT the Model matrix is different (and the MVP too)
            glm::mat4 ModelMatrix4 = glm::mat4(1.0);
            glm::mat4 TranslationMatrix4 = glm::translate(ModelMatrix4, glm::vec3(gPosition4));
            glm::mat4 RotationMatrix4 = eulerAngleYXZ(gOrientation4.y, gOrientation4.x, gOrientation4.z);
            glm::mat4 ScalingMatrix4 = glm::scale(glm::mat4(1.0f), glm::vec3(100.0f, 100.0f, 100.0f));
            ModelMatrix4 = TranslationMatrix4 * RotationMatrix4 * ScalingMatrix4;
            glm::mat4 MVP4 = ProjectionMatrix * ViewMatrix * ModelMatrix4;

            // Send our transformation to the currently bound shader,
            // in the "MVP" uniform
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP4[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix4[0][0]);
            glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);


            // 1rst attribute buffer : vertices
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_sky);
            glVertexAttribPointer(
                0,                  // attribute
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
            );

            // 2nd attribute buffer : UVs
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_sky);
            glVertexAttribPointer(
                1,                                // attribute
                2,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
            );

            // 3rd attribute buffer : normals
            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_sky);
            glVertexAttribPointer(
                2,                                // attribute
                3,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
            );


            // Draw the triangles !
            glDrawArrays(GL_TRIANGLES, 0, vertices_sky.size() );

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(2);


            // Swap buffers
            glfwSwapBuffers(window);
            glfwPollEvents();


        } // Check if the ESC key was pressed or the window was closed
        while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
               glfwWindowShouldClose(window) == 0 );

        // Cleanup VBO and shader
        glDeleteBuffers(1, &vertexbuffer_flag);
        glDeleteBuffers(1, &uvbuffer_flag);
        glDeleteBuffers(1, &normalbuffer_flag);
        glDeleteProgram(programID);
        glDeleteTextures(1, &Texture_flag);
        glDeleteVertexArrays(1, &VertexArrayID);

        // Close OpenGL window and terminate GLFW
        glfwTerminate();

        return 0;
    }

