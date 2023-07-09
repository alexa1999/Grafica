#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Rubik.h"
#include <random>
#include <iomanip>
#include "AllSolver/solve.h"
#include "AllSolver/random.h"
#include <iostream>
//#include "Polygon.h"
using namespace std;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
constexpr int FLOAT_MIN = 0;
constexpr int FLOAT_MAX = 1;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
//--------------------------------------------------------------
const char *vertexShaderSource ="#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "layout (location = 2) in vec2 aTexCoord;"
    "out vec3 ourColor;\n"
    "out vec2 TexCoord;\n"
    "uniform mat4 view;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = view * vec4(aPos, 8.0);\n"
    "   //gl_Position = view * vec4(aPos, 1.0);\n"
    "   ourColor = aColor;\n"
    "   TexCoord = aTexCoord;\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D texture1;\n"
    "void main()\n"
    "{\n"
        "FragColor =  vec4(ourColor, 1.0f)*texture(texture1, TexCoord) ;\n"
    "   //FragColor = vec4(ourColor, 1.0f);\n"
    "}\n\0";
//--------------------------------------------------------------
//variables
float camera_pos_x=0.0f,camera_pos_y=0.0f,camera_pos_z=0.0f;
glm::vec3 cameraPos   = glm::vec3(camera_pos_x,camera_pos_y,camera_pos_z);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

float yaw = 0.0f;
float pitch = 0;
float cam_x=0,cam_y=0;
float cameraSpeed =1.8; 
string condition_input="N";
vector<string> reg_mov;
vector<string> solution_cubo;
///Objetos
///Objetos
//--------animar_figuras 
class animar_figuras{
	public:
    float velocidad_animacion= 0.01;
    int reg_movimientos=0;
    string estado_animar_figuras="M";
    
    vector <float> vertices_animar_figuras{
       -0.6,  0,    0,       //A
        -0.4,      0.5,      0,                 //B
        -0.2,   0,    0,
        0.6,      0,    0,
        -0.01,    -0.4,   0,
        0.3,    -1.1,   0,
        -0.4,      -0.7,  0,
        -1.1,   -1.1,   0,
        -0.8,   -0.4,   0,
        -1.4,     0,    0,
        -0.6,  0,    0,
		     /*0.0, 0.0, 0.0,   // Centro
    0.0, 0.75, 0.0,   // Punto superior
    0.0, -0.75, 0.0,  // Punto inferior
    -0.75, 0.0, 0.0,  // Punto izquierdo
    0.75, 0.0, 0.0,   // Punto derecho
    -0.525, 0.525, 0.0,  // Punto superior izquierdo
    0.525, 0.525, 0.0,   // Punto superior derecho
    -0.525, -0.525, 0.0, // Punto inferior izquierdo
    0.525, -0.525, 0.0,  // Punto inferior derecho
    0.0, 0.0, 0.0  // Centro (*/

        
    };
    Polygon* pol_animar_figuras;
    animar_figuras(float R, float G, float B){
        pol_animar_figuras=new Polygon(this->vertices_animar_figuras, R, G, B);
    }
    //----------------------------------
    string animacion_animar_figuras(int pos_negativo, string eje){
        if(estado_animar_figuras=="M"){
            //if(reg_movimientos!=2700){//0.002
            if(reg_movimientos!=500){
                pol_animar_figuras->noob_translate(0,velocidad_animacion,0);
                reg_movimientos+=1;
                return "M";
            }
            reg_movimientos=0;
            velocidad_animacion=0.2;
            return "G";
        }
        if(estado_animar_figuras=="G"){
            if(reg_movimientos<500){
                if(eje=="X"){
                    pol_animar_figuras->pro_rotation_x(pos_negativo * velocidad_animacion);
                }
                if(eje=="Z"){
                    pol_animar_figuras->pro_rotation_z(pos_negativo * velocidad_animacion);
                }
                
				if(eje=="Y"){
					pol_animar_figuras->pro_rotation_x(pos_negativo * velocidad_animacion);
					
				}
				reg_movimientos+=1;
                return "G";
            }
            reg_movimientos=0;
            velocidad_animacion=0.01;
            return "B";
        }
        if(estado_animar_figuras=="B"){
            if(reg_movimientos!=100){
                pol_animar_figuras->noob_translate(0,-velocidad_animacion,0);
                reg_movimientos+=1;
                return "B";
            }
            return "N";
        }
    }
    void draw(GLFWwindow* window,bool wired=false){
        pol_animar_figuras->draw(window,wired);
    }
};
int pos_vect_animar_figuras=0;
vector<animar_figuras*> animar_figuras_v;
float random() {
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<float> distr(FLOAT_MIN, FLOAT_MAX);
    setprecision(1);

    return distr(eng);
}


//Polygon estrella(vertices_estrella,1,1,0);
Rubik c;

///-------------------------------------------------------------
void processInput(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    //////////////////////////////////////////////////////////////////////////////////////
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
        pitch -= (cameraSpeed );
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
        pitch += (cameraSpeed );
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
        yaw += (cameraSpeed );
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        yaw -= (cameraSpeed );
    }
    //////////////////////////////////////////////////////////////////////////////////////
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && condition_input=="N"){
        reg_mov.push_back("R");
        //cout<<"INICIA ANIMACION R"<<endl;
        condition_input="R";
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && condition_input=="N"){
        reg_mov.push_back("L");
        //cout<<"INICIA ANIMACION L"<<endl;
        condition_input="L";
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && condition_input=="N"){
        reg_mov.push_back("U");
        //cout<<"INICIA ANIMACION U"<<endl;
        condition_input="U";
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && condition_input=="N"){
        reg_mov.push_back("D");
        //cout<<"INICIA ANIMACION D"<<endl;
        condition_input="D";
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && condition_input=="N"){
        reg_mov.push_back("F");
        //cout<<"INICIA ANIMACION F"<<endl;
        condition_input="F";
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && condition_input=="N"){
        reg_mov.push_back("B");
        //cout<<"INICIA ANIMACION B"<<endl;
        condition_input="B";
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && condition_input=="N"){
        //cout<<"INICIA ANIMACION B"<<endl;
        //condition_input="E";
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && condition_input=="N"){
		
        condition_input="A";
        string tempo=to_cube_not(reg_mov);
        reg_mov.clear();
        
        solution_cubo=get_solution(tempo);
        //for(int i=0;i<solution_cubo.size();++i){
          //      cout<<solution_cubo[i]<<" ";
        //}
        cout<<endl;
    }
}
//--------------------------------------------------------------
int main(){
     animar_figuras_v.reserve(24);
	 float R, G, B;
     for (int i = 0; i < 24; ++i) {
        R = random();
        G = random();
        B = random();

        animar_figuras* animar_figuras_point = new animar_figuras(R,G,B);
        animar_figuras_v.push_back(animar_figuras_point);
    }
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGL(glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
    //-------------------------------------------------------------
     // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success){
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success){
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success){
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glUseProgram(shaderProgram);
    //-------------------------------------------------------------

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // load and create a texture 
    // -------------------------
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);  
    unsigned char *data = stbi_load("F:/LabCompiler_CG/glfw-master/OwnProjects/Cubo-rubick2/emoji1.jpg", &width, &height, &nrChannels, 0);
    
    if (data){   
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
        std::cout << "Failed to load texture" << std::endl;
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    //menu
    cout<<"Girar en sentido horario"<<endl;
    cout<<"Con la tecla U la cara UP"<<endl;
    cout<<"Con la tecla D la cara DOWN"<<endl;
    cout<<"Con la tecla L la cara LEFT"<<endl;
    cout<<"Con la tecla R la cara RIGHT"<<endl;
    cout<<"Con la tecla F la cara FRONT"<<endl;
    cout<<"Con la tecla B la cara BACK"<<endl;
    cout<<"---------------------------------"<<endl;
    cout<<"Se puede mover la camara con las flechas direccionales"<<endl;
    cout<<"---------------------------------"<<endl;
    cout<<"Con la tecla K Se resolvera el cubo"<<endl;
    // -----------
    while (!glfwWindowShouldClose(window)){
        // input
        // -----
        glfwSetKeyCallback(window, processInput);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //camera
        glm::mat4 R = glm::yawPitchRoll(glm::radians(yaw), glm::radians(pitch), 0.0f);

        cameraFront = glm::vec3(R * glm::vec4(0, 0, -1, 0));
        cameraUp = glm::vec3(R * glm::vec4(0, 1, 0, 0));
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        // entradas
        if(condition_input=="R"){
            condition_input=c.animation_R();
        }
        if(condition_input=="U"){
            condition_input=c.animation_U();
        }
        if(condition_input=="D"){
            condition_input=c.animation_D();
        }
        if(condition_input=="F"){
            condition_input=c.animation_F();
        }
        if(condition_input=="B"){
            condition_input=c.animation_B();
        }
        if(condition_input=="L"){
            condition_input=c.animation_L();
        }
        if(condition_input=="A"){
            condition_input=c.aplicar_solucion(solution_cubo);
        }
        if(condition_input=="E"){
           float R, G, B;
            animar_figuras_v[pos_vect_animar_figuras]->estado_animar_figuras=animar_figuras_v[pos_vect_animar_figuras]->animacion_animar_figuras(15,"X");
            animar_figuras_v[pos_vect_animar_figuras+1]->estado_animar_figuras=animar_figuras_v[pos_vect_animar_figuras+1]->animacion_animar_figuras(-8,"X");
            animar_figuras_v[pos_vect_animar_figuras+2]->estado_animar_figuras=animar_figuras_v[pos_vect_animar_figuras+2]->animacion_animar_figuras(4,"Z");
            animar_figuras_v[pos_vect_animar_figuras+3]->estado_animar_figuras=animar_figuras_v[pos_vect_animar_figuras+3]->animacion_animar_figuras(-30,"Z");
			animar_figuras_v[pos_vect_animar_figuras+4]->estado_animar_figuras=animar_figuras_v[pos_vect_animar_figuras+4]->animacion_animar_figuras(20,"Y");
			animar_figuras_v[pos_vect_animar_figuras+5]->estado_animar_figuras=animar_figuras_v[pos_vect_animar_figuras+5]->animacion_animar_figuras(-20,"Y");
            if(animar_figuras_v[pos_vect_animar_figuras]->estado_animar_figuras=="N"){
                if(pos_vect_animar_figuras==24){
                    //animar_figuras_v.clear();
                    for(int i=0;i<24;++i){
                       animar_figuras *animar_figuras_point=new animar_figuras(R,G,B);
                        animar_figuras_v.push_back(animar_figuras_point);
                    }
                    pos_vect_animar_figuras=0;
                    condition_input="N";
					animar_figuras_v.clear();
                }
                else 
                    pos_vect_animar_figuras+=6;
            }
        }
        //glDrawArrays(GL_TRIANGLE_FAN, 0, 4);   
        c.draw(window);
        for(int i=0;i<24;++i){
            animar_figuras_v[i]->draw(window);
        }
        
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
