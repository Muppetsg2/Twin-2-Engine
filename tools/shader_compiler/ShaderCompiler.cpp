#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "config.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD

//#include "config.h"


// Define the file extension for shader program files
constexpr char SHADER_PROGRAM_EXTENSION[] = ".shpr";
// Define the file extension for compiled shader binary files
constexpr char SHADER_BINARY_EXTENSION[] = ".shdr";

// Function to compile a shader from source
GLuint compileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Error compiling shader: " << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

// Function to create a shader program from vertex and fragment shaders
GLuint createShaderProgram(const std::string& vertexSource, const std::string& fragmentSource) {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // Check for linking errors
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Error linking shader program: " << infoLog << std::endl;
        glDeleteProgram(program);
        return 0;
    }

    // Clean up
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

// Function to load shader source from file
std::string loadShaderSource(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
void checkShaderCompilationSuccess(GLuint shaderId)
{
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
        std::cout << shaderId << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << "\n";
    }
}

void checkProgramLinkingSuccess(GLuint programId)
{
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programId, 512, NULL, infoLog);
        std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << "\n";
    }
}

int main() {// Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL Window", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Load OpenGL functions using GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    //bool err = !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    //
    //if (err)
    //{
    //    spdlog::error("Failed to initialize OpenGL loader!");
    //    return false;
    //}
    // Define origin folders to search for shader program files
    std::vector<std::string> originFolders = { ORIGIN_FOLDER };
    //std::vector<std::string> originFolders = { "C:\\Users\\matga\\Desktop\\Mateusz\\Studia\\Semestr_VI\\PSGK\\Engine\\Twin-2-Engine"};

    // Search for shader program files
    for (const auto& folder : originFolders) {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(folder)) {
            if (entry.path().extension() == SHADER_PROGRAM_EXTENSION) {
                std::ifstream shaderFile(entry.path());
                if (shaderFile.is_open()) {
                    // Read shader program file
                    std::string shaderProgramName, vertexShaderPath, fragmentShaderPath;
                    std::getline(shaderFile, shaderProgramName);
                    std::getline(shaderFile, vertexShaderPath);
                    std::getline(shaderFile, fragmentShaderPath);

                    // Load shader sources
                    std::string vertexSource = loadShaderSource(folder + "/" + vertexShaderPath);
                    std::string fragmentSource = loadShaderSource(folder + "/" + fragmentShaderPath);


                    //VERTEX
                    unsigned int vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
                    const GLchar* const vertexShaderSource = (const GLchar*)vertexSource.c_str();
                    glShaderSource(vertexShaderID, 1, &vertexShaderSource, NULL);
                    glCompileShader(vertexShaderID);

                    //FRAGMENT
                    const GLchar* const fragmentShaderSource = (const GLchar*)fragmentSource.c_str();
                    unsigned int fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
                    glShaderSource(fragmentShaderID, 1, &fragmentShaderSource, NULL);
                    glCompileShader(fragmentShaderID);

                    checkShaderCompilationSuccess(vertexShaderID);
                    checkShaderCompilationSuccess(fragmentShaderID);
                    // Create shader program
                    //GLuint shaderProgram = createShaderProgram(vertexSource, fragmentSource);
                    GLuint shaderProgram = glCreateProgram();

                    //Do³¹czanie shaderów
                    glAttachShader(shaderProgram, vertexShaderID);
                    glAttachShader(shaderProgram, fragmentShaderID);
                    glLinkProgram(shaderProgram);
                    checkShaderCompilationSuccess(shaderProgram);
                    checkProgramLinkingSuccess(shaderProgram);


                    glDeleteShader(vertexShaderID);
                    glDeleteShader(fragmentShaderID);

                    if (shaderProgram != 0) {
                        //glLinkProgram(shaderProgram);
                        // Get program binary
                        GLint binaryLength;
                        glGetProgramiv(shaderProgram, GL_PROGRAM_BINARY_LENGTH, &binaryLength);
                        std::vector<GLchar> binaryData(binaryLength);
                        GLenum binaryFormat;
                        glGetProgramBinary(shaderProgram, binaryLength, nullptr, &binaryFormat, binaryData.data());

                        // Save binary to file
                        std::string outputFilePath = ORIGIN_FOLDER + std::string("/Shaders/") + shaderProgramName + SHADER_BINARY_EXTENSION;
                        std::cout << "Binary format: " << binaryFormat << std::endl;
                        std::ofstream binaryFile;// (outputFilePath, std::ios::binary);
                        try {
                            std::filesystem::create_directories(outputFilePath.substr(0, outputFilePath.find_last_of('/')));
                           // return true;
                        }
                        catch (const std::filesystem::filesystem_error& e) {
                            std::cerr << "Failed to create directories: " << e.what() << std::endl;
                            //return false;
                        }
                        binaryFile.open(outputFilePath, std::ios::binary);
                        if (binaryFile.is_open())
                        {
                            std::cout << "Output file opened\n";
                        }
                        else
                        {

                            std::cout << "Output file not opened\n";
                        }
                        binaryFile.write(binaryData.data(), binaryLength);
                        binaryFile.close();

                        std::cout << "Compiled and saved shader program binary: " << outputFilePath << " | " << entry.path() << std::endl;

                        // Cleanup
                        glDeleteProgram(shaderProgram);
                    }
                }
                else {
                    std::cerr << "Failed to open shader program file: " << entry.path() << std::endl;
                }
            }
        }
    }

    return 0;
}
