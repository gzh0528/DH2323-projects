#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLUT/GLUT.h>
#include <shader_m.h>
#include <camera.h>
#include <model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
void MySphere();
void skybox_render(Shader& skybox_shader,glm::mat4 &view,glm::mat4 &projection);
unsigned int load_skybox(vector<string> box_face);

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
//camera
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
bool firstMouse = true;
//time
float deltaTime = 0.0f;
float lastFrame = 0.0f;
const float PI=3.1415926;

//skybox path
vector<string> box_faces{
    "/Users/zehuag/Desktop/islandpj/test_model/test_model/resource/right.jpg",
    "/Users/zehuag/Desktop/islandpj/test_model/test_model/resource/left.jpg",
    "/Users/zehuag/Desktop/islandpj/test_model/test_model/resource/top.jpg",
    "/Users/zehuag/Desktop/islandpj/test_model/test_model/resource/bottom.jpg",
    "/Users/zehuag/Desktop/islandpj/test_model/test_model/resource/front.jpg",
    "/Users/zehuag/Desktop/islandpj/test_model/test_model/resource/back.jpg"
};
//skybox cube positions
float skybox_vertex[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "My Opengl Project", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    const char * dragon_vs="#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNormal;\n"
    "out vec3 Normal;\n"
    "out vec3 Pos;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "    Normal = mat3(transpose(inverse(model))) * aNormal;\n"
    "    Pos = vec3(model * vec4(aPos, 1.0));\n"
    "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "}\n\0";
    
    const char *dragon_fs="#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoords;\n"
    "in vec3 Normal;\n"
    "in vec3 Pos;\n"
    "uniform vec3 camPos;\n"
    "uniform samplerCube skybox_tex;\n"
    "void main()\n"
    "{\n"
    "    vec3 I=normalize(Pos-camPos);\n"
    "    vec3 R_f=refract(I,normalize(Normal),1.0/1.33);\n"
    "    FragColor = vec4(texture(skybox_tex, R_f).rgb,1.0);\n"
    "}\n\0";
    const char * whale_vs="#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNormal;\n"
    "out vec3 Normal;\n"
    "out vec3 Pos;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "    Normal = mat3(transpose(inverse(model))) * aNormal;\n"
    "    Pos = vec3(model * vec4(aPos, 1.0));\n"
    "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "}\n\0";
    
    const char *whale_fs="#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoords;\n"
    "in vec3 Normal;\n"
    "in vec3 Pos;\n"
    "uniform vec3 camPos;\n"
    "uniform samplerCube skybox_tex;\n"
    "void main()\n"
    "{\n"
    "    vec3 I=normalize(Pos-camPos);\n"
    "    vec3 R=reflect(I,normalize(Normal));\n"
    "    FragColor = vec4(texture(skybox_tex, R).rgb,1.0);\n"
    "}\n\0";
    
    const char * PBR_fs="#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoords;\n"
    "in vec3 WorldPos;\n"
    "in vec3 Normal;\n"
    "uniform float roughness;\n"
    "uniform float metal;\n"
    "uniform float ao;//white\n"
    "uniform vec3 albedo;\n"
    "uniform vec3 light_Positions[4];\n"
    "uniform vec3 light_colors[4];\n"
    "uniform vec3 camPos;\n"
    "const float PI = 3.1415926;\n"
    "vec3 Fresnel(vec3 H,vec3 V, vec3 F0)\n"
    "{\n"
    "    float theta=clamp(dot(H, V), 0.0, 1.0);\n"
    "    return F0 + (1.0 - F0) * pow(max(1.0 - theta, 0.0), 5.0);\n"
    "}\n"
    "float GschGGX(vec3 N,vec3 V,float roughness)\n"
    "{\n"
    "    float k=(roughness+1.0)*(roughness+1.0)/8.0;\n"
    "    float ndv=max(dot(N,V),0.0);\n"
    "    return ndv/((1.0-k)*ndv+k);\n"
    "}\n"
    "float G_smith(vec3 N, vec3 V, vec3 L, float r)\n"
    "{\n"
    "    return GschGGX(N,V,r)*GschGGX(N,L,r);\n"
    "}\n"
    "float DGGX(vec3 N,vec3 H,float roughness)\n"
    "{\n"
    "    float a = roughness*roughness;\n"
    "    float ndh=max(dot(N,H),0.0);\n"
    "    float dnorm=ndh*ndh*(a*a-1.0)+1.0;\n"
    "    float denorm=PI*dnorm*dnorm;\n"
    "    return a*a/(max(denorm, 0.0000001));\n"
    "}\n"
    "void main()\n"
    "{\n"
    "    vec3 N=normalize(Normal);\n"
    "    vec3 V=normalize(camPos-WorldPos);\n"
    "    vec3 F0=vec3(0.04);\n"
    "    F0=mix(F0,albedo,metal);\n"
    "    vec3 Light = vec3(0.0);\n"
    "    for(int i = 0; i < 4; ++i)\n"
    "    {\n"
    "            vec3 L = normalize(light_Positions[i] - WorldPos);\n"
    "            vec3 H = normalize(V + L);\n"
    "            float dist = length(light_Positions[i] - WorldPos);\n"
    "            float attenuation = 1.0 / (dist * dist);\n"
    "            vec3 radiance = light_colors[i] * attenuation;\n"
    "            float NDF = DGGX(N, H, roughness);\n"
    "            float G = G_smith(N, V, L, roughness);\n"
    "            vec3 F = Fresnel(H,V,F0);\n"
    "            vec3 ks = F;\n"
    "            vec3 kd = vec3(1.0)- ks;\n"
    "            kd *=(1.0 - metal);\n"
    "            vec3 nom = NDF * G * F;\n"
    "            float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);\n"
    "            vec3 specular=nom/max(denom, 0.001);\n"
    "            Light += (kd * albedo/PI + specular) * radiance * (max(dot(N, L), 0.0));\n"
    "        }\n"
    "    vec3 ambient = vec3(0.03)*albedo*ao;\n"
    "    vec3 color=Light+ambient;\n"
    "    color=color/(color + vec3(1.0));\n"
    "    color=pow(color, vec3(1.0/2.2));\n"
    "    FragColor=vec4(color,1.0);\n"
    "}\n\0";

    const char * PBR_vs="#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoords;\n"
    "layout (location = 2) in vec3 aNormal;\n"
    "out vec2 TexCoords;\n"
    "out vec3 WorldPos;\n"
    "out vec3 Normal;\n"
    "uniform mat4 projection;\n"
    "uniform mat4 view;\n"
    "uniform mat4 model;\n"
    "void main()\n"
    "{\n"
    "    TexCoords = aTexCoords;\n"
    "    WorldPos = vec3(model * vec4(aPos, 1.0));\n"
    "    Normal = mat3(model) * aNormal;\n"
    "    gl_Position = projection * view *vec4(WorldPos, 1.0);\n"
    "}\n\0";
    
    //a sphere light source
    const char* skybox_vs="#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "out vec3 TexCoords;\n"
    "void main()\n"
    "{\n"
    "    TexCoords=aPos;\n"
    "    gl_Position = projection * view * vec4(aPos, 1.0);\n"
    "}\n\0";
    
    const char * skybox_fs="#version 330 core\n"
    "in vec3 TexCoords;\n"
    "out vec4 FragColor;\n"
    "uniform samplerCube skybox_tex;\n"
    "void main()\n"
    "{\n"
    "    FragColor = texture(skybox_tex,TexCoords);\n"
    "}\n\0";
    
    const char * phong_vs="#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNormal;\n"
    "out vec3 FragPos;\n"
    "out vec3 Normal;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "    FragPos = vec3(model * vec4(aPos, 1.0));\n"
    "    Normal = mat3(transpose(inverse(model))) * aNormal;\n"
    "    gl_Position = projection * view * vec4(FragPos, 1.0);\n"
    "}\n\0";
    
    const char * phong_fs="#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 Normal;\n"
    "in vec3 FragPos;\n"
    "uniform vec3 lightPos;\n"
    "uniform vec3 viewPos;\n"
    "uniform vec3 lightColor;\n"
    "uniform vec3 albedo;\n"
    "void main()\n"
    "{\n"    "    float ambientPow = 0.1;\n"
    "    vec3 ambient_light = ambientPow * lightColor;\n"
    "    vec3 light_dir = normalize(lightPos - FragPos);\n"
    "    float diffuse = max(dot(normalize(Normal), light_dir), 0.0);\n"
    "    vec3 diffuse_light = diffuse * lightColor;\n"
    "    float specularPow = 0.5;\n"
    "    vec3 view_dir = normalize(viewPos - FragPos);\n"
    "    vec3 reflect_dir = reflect(-light_dir, normalize(Normal));\n"
    "    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);\n"
    "    vec3 specular_light = specularPow * spec * lightColor;\n"
    "    vec3 result = (ambient_light + diffuse_light + specular_light) * objectColor;\n"
    "    FragColor = vec4(result, 1.0);\n"
    "}\n\0";
    Shader shader(PBR_vs, PBR_fs);//PBR shader
    Shader ocean_shader(whale_vs,whale_fs);//reflection shader
    Shader dragon_shader(dragon_vs,dragon_fs);//refraction shader
    Shader skybox_shader(skybox_vs,skybox_fs);//environment mapping shader
    //Shader phong_shader(phong_vs,phong_fs);//phong shader
    //PBR shader setting
    shader.use();
    shader.setVec3("albedo", 0.0f, 0.0f, 0.5f);
    shader.setFloat("ao", 1.0f);
    //Phong shader setting
    //phong_shader.use();
    //phong_shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    // lights
    // ------
    glm::vec3 lightPositions[] = {
        glm::vec3(-10.0f,  10.0f, 10.0f),
        glm::vec3( 10.0f,  10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, 10.0f),
        glm::vec3( 10.0f, -10.0f, 10.0f),
    };
    glm::vec3 lightColors[] = {
        glm::vec3(200.0f, 200.0f, 300.0f),
        glm::vec3(200.0f, 200.0f, 200.0f),
        glm::vec3(200.0f, 200.0f, 300.0f),
        glm::vec3(200.0f, 200.0f, 300.0f)
    };
    int nr_rows = 4;
    int nr_cols = 4;
    float space = 5.0;

    // initialize static shader uniforms before rendering
    // --------------------------------------------------
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    shader.use();
    //shader.setVec3("albedo", glm::vec3(0.5f,0.0f,0.0f));
    shader.setMat4("projection", projection);
    //phong_shader.setMat4("projection", projection);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //render 3d sphere
        
        shader.use();
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("view", view);
        shader.setVec3("camPos", camera.Position);
        //phong_shader.use();
        //phong_shader.setMat4("view", view);
        //evaluate Phong shader and the PBR shader
        
        
        
        
        
        float angle=PI/4;
        int tag=1;
        for (int row = 0; row < nr_rows; ++row)
        {
            shader.setFloat("metal", (float)1 / (float)nr_rows);
            for (int col = 0; col < nr_cols; ++col)
            {
                glm::mat4 model = glm::mat4(1.0f);
                // set roughness
                shader.setFloat("roughness", glm::clamp((float)col / (float)nr_cols, 0.05f, 1.0f));
                model = glm::translate(model, camera.Position-glm::vec3(0.03f,0.0f,0.5f));//0.5
                model= glm::scale(model,glm::vec3(0.02f, 0.02f, 0.02f));//2
                model = glm::translate(model, glm::vec3(
                                    (col - (nr_cols / 2)) * space+space,
                                    (row - (nr_rows / 2)) * space,
                                    0.0f
                                ));
                //model =glm::translate(model, glm::vec3(space*cos(angle),space*sin(angle),0.0f));
                glm::vec3 new_light_pos =lightPositions[col]+glm::vec3(cos(angle),sin(angle),0.0f);
                //model =glm::translate(model, glm::vec3(0.0f,-space*cos(angle),-space*sin(angle)));
                
                
                
                //glm::vec3 newPos = lightPositions[col] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
                //newPos = lightPositions[col];
                shader.setVec3("light_Positions[" + std::to_string(col) + "]", new_light_pos);
                shader.setVec3("light_colors[" + std::to_string(col) + "]", lightColors[col]);
                //shader.setVec3("light_Positions["+to_string(col)+"]", lightPositions[col]);
                //shader.setVec3("light_colors["+to_string(col)+"]", lightColors[col]);
                model=glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0f, 1.0f, 0.0f));
                shader.setMat4("model", model);
                //renderSphere();
                //phong_shader.setVec3("lightPos", new_light_pos);
                //phong_shader.setVec3("lightColor", lightColors[col]);
                //phong_shader.setMat4("model", model);
                //phong_shader.setVec3("viewPos", camera.Position);
                if(col==1)
                    shader.setVec3("albedo", 0.0f, 0.5f, 0.0f);
                else if (col==2)
                    shader.setVec3("albedo", 0.0f, 0.0f, 0.5f);
                else
                    shader.setVec3("albedo", 0.5f,0.0f,0.0f);
                if(row==0||col==0||row==nr_rows-1||col==nr_cols-1)
                //if(col==nr_cols-1)
                MySphere();
                angle+=PI/4;
            }
        }
        
        Model ourModel("/Users/zehuag/Desktop/islandpj/test_model/test_model/resource/Ocean obj/uploads_files_2435836_Glass_whale.obj");
        Model dragon_Model("/Users/zehuag/Desktop/islandpj/test_model/test_model/resource/Ocean obj/dragon.obj");
        
        //render my model
        ocean_shader.use();
        //model = glm::mat4(1.0f);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, camera.Position-glm::vec3(0.03f,0.0f,0.3f));
        model=glm::scale(model,glm::vec3(0.1f, 0.1f, 0.1f));
        model = glm::translate(model, glm::vec3(2.0f, -0.5f, 0.0f));
        model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
        float timeValue= glfwGetTime();//时间
        model =glm::rotate(model, glm::radians(10.0f),glm::vec3(0.0f, 0.0f, 1.0f));
        ocean_shader.setMat4("model", model);
        unsigned int skybox_texture = load_skybox(box_faces);
        ocean_shader.setInt("skybox_tex", 0);
        ocean_shader.setMat4("projection", projection);
        //view=glm::rotate(view, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ocean_shader.setMat4("view", view);
        ocean_shader.setVec3("lightColor_m", 1.0f, 1.0f, 1.0f);
        ocean_shader.setFloat("time",deltaTime);
        ocean_shader.setVec3("camPos", camera.Position);
        ourModel.Draw(ocean_shader);
        
        //render my dragon
        dragon_shader.use();
       model = glm::mat4(1.0f);
        model = glm::translate(model, camera.Position-glm::vec3(0.03f,0.0f,0.3f));
        model=glm::scale(model,glm::vec3(0.05f, 0.05f, 0.05f));
        model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.9f, 0.9f, 0.9f));
        model =glm::rotate(model, glm::radians(20.0f),glm::vec3(0.0f, 1.0f, 0.0f));
        dragon_shader.setMat4("model", model);
        skybox_texture = load_skybox(box_faces);
        dragon_shader.setInt("skybox_tex", 0);
        dragon_shader.setMat4("projection", projection);
        //view=glm::rotate(view, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        dragon_shader.setMat4("view", view);
        dragon_shader.setVec3("lightColor_m", 1.0f, 1.0f, 1.0f);
        dragon_shader.setFloat("time",deltaTime);
        dragon_shader.setVec3("camPos", camera.Position);
        dragon_Model.Draw(dragon_shader);
        
        //render my skybox
        skybox_render(skybox_shader,view,projection);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}


//load texture from file
unsigned int loadTexture(char const * path)
{
    unsigned int t_id;
    glGenTextures(1, &t_id);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, t_id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed " << path << std::endl;
        stbi_image_free(data);
    }
    return t_id;
}


// my function to load local skybox
unsigned int load_skybox(vector<string> box_face)
{
    //bind skybox texture
    unsigned int t_id;
    glGenTextures(1,&t_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP,t_id);
    //load
    int w, h, nrC;
    for (int i = 0; i < box_face.size(); i++)
        {
            unsigned char *data = stbi_load(box_face[i].c_str(), &w, &h, &nrC, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE,data);
                stbi_image_free(data);
            }
            else
            {
                cout << "Unable to open" << box_face[i] << endl;
                stbi_image_free(data);
            }
        }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return t_id;
}

void skybox_render(Shader& skybox_shader,glm::mat4 &view,glm::mat4 &projection)
{
    unsigned int VAO,VBO;
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertex), &skybox_vertex, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    unsigned int skybox_texture = load_skybox(box_faces);
    skybox_shader.use();
    skybox_shader.setInt("skybox_tex", 0);
    //save time consumption
    glDepthFunc(GL_LEQUAL);
    view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
    skybox_shader.setMat4("view", view);
    skybox_shader.setMat4("projection", projection);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP,skybox_texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}


void MySphere()
{
        unsigned int sphere_VAO = 0;
        unsigned int index_count;
        glGenVertexArrays(1, &sphere_VAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        const unsigned int X_SEG = 64;
        const unsigned int Y_SEG = 64;
        const float PI = 3.14159265359;
        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;
        for (unsigned int y = 0; y <= Y_SEG; ++y)
        {
            for (unsigned int x = 0; x <= X_SEG; ++x)
            {
                float xSeg = (float)x / (float)X_SEG;
                float ySeg = (float)y / (float)Y_SEG;
                float xPos = std::cos(xSeg * 2.0f * PI) * std::sin(ySeg * PI);
                float yPos = std::cos(ySeg * PI);
                float zPos = std::sin(xSeg * 2.0f * PI) * std::sin(ySeg * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSeg, ySeg));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool tag = false;
        //set up the index for rendering
        for (unsigned int y = 0; y < Y_SEG; ++y)
        {
            if (!tag)
            {
                for (unsigned int x = 0; x <= X_SEG; ++x)
                {
                    indices.push_back(y       * (X_SEG + 1) + x);
                    indices.push_back((y + 1) * (X_SEG + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEG; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEG + 1) + x);
                    indices.push_back(y       * (X_SEG + 1) + x);
                }
            }
            tag = !tag;
        }
        index_count = indices.size();
        //vertex array
        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
        }
        
        //bind buffers and rendering
        glBindVertexArray(sphere_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        float stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
    glBindVertexArray(sphere_VAO);
    glDrawElements(GL_TRIANGLE_STRIP, index_count, GL_UNSIGNED_INT, 0);
}

unsigned int SVAO = 0;
unsigned int index_count;
void MySphere_re()
{
    if (SVAO == 0)
    {
    const float PI=3.1415926;
    unsigned int VBO,EBO;
    unsigned int SVAO;
    unsigned int index_count;
    glGenVertexArrays(1,&SVAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);
    
    vector<glm::vec3> pos3;
    vector<glm::vec3> normals;
    vector<glm::vec2> uv;
    vector<unsigned int> indices;
    
    unsigned int X_seg=64;
    unsigned int Y_seg=64;
    //populate normals pos uv
    for(unsigned int y=0;y<=Y_seg;y++)
    {
        for(unsigned int x=0;x<=X_seg;x++)
        {
            float xseg = (float)x / (float)X_seg;
            float yseg = (float)y / (float)Y_seg;
            float xPos = cos(xseg * 2.0f * PI) * sin(yseg * PI);
            float yPos = cos(yseg * PI);
            float zPos = sin(xseg* 2.0f * PI) * sin(yseg * PI);


            //float xPos = cos(((float)x * 2.0f * PI)* sin(((float)y * PI)/((float)Y_seg)/(float)X_seg)) ;
            //float yPos = cos((float)y * PI/(float)Y_seg);
            //float zPos = sin(((float)x * 2.0f * PI)* sin(((float)y * PI)/(float)Y_segX/(float)X_seg);
            pos3.push_back(glm::vec3(xPos, yPos, zPos));
            normals.push_back(glm::vec3(xPos, yPos, zPos));//normal = pos3
            uv.push_back(glm::vec2(xseg, yseg));
        }
    }
    int tag=-1;
    for(unsigned int y=0;y<Y_seg;y++)
    {
            if(tag==-1)
            {
                for(unsigned int x=0;x<=X_seg;x++)
                {
                indices.push_back(x+y*(X_seg+1));
                indices.push_back(x+(y+1)*(X_seg+1));
                }
            }
            else
            {
                for(unsigned int x=X_seg;x>=0;--x)
                {
                indices.push_back(x+(y+1)*(X_seg+1));
                indices.push_back(x+(y)*(X_seg+1));
                }
            }
        tag=-tag;
    
    }
    index_count=indices.size();
    vector<float> vertex_array;
    for(unsigned int i=0;i<pos3.size();i++)
    {
        vertex_array.push_back(pos3[i].x);
        vertex_array.push_back(pos3[i].y);
        vertex_array.push_back(pos3[i].z);
        if (uv.size() > 0)
                    {
        vertex_array.push_back(uv[i].x);
        vertex_array.push_back(uv[i].y);
                    }
        if(normals.size()>0)
        {
        vertex_array.push_back(normals[i].x);
        vertex_array.push_back(normals[i].y);
        vertex_array.push_back(normals[i].z);
        }
    }
    //VA
    glBindVertexArray(SVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertex_array.size() * sizeof(float), &vertex_array[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    float total_size = (3 + 2 + 3) * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, total_size, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, total_size,(void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, total_size, (void*)(5 * sizeof(float)));
    }
    glBindVertexArray(SVAO);
    glDrawElements(GL_TRIANGLE_STRIP, index_count, GL_UNSIGNED_INT, 0);
}

