#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>

using std::string;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
//#include "config.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD

//#include "config.h"
#define ORIGIN_FOLDER "./ShadersOrigin/"

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

bool isWhitespace(char c) {
    return std::isspace(static_cast<unsigned char>(c));
}

bool isEmptyOrWhitespace(const std::string& str) {
    return std::all_of(str.begin(), str.end(), isWhitespace);
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
    //std::vector<std::string> originFolders = { ORIGIN_FOLDER };
    //std::cout << "Path: " << std::filesystem::current_path();
    std::vector<std::string> originFolders = { "ShadersOrigin" };
    //std::vector<std::string> originFolders = { "C:\\Users\\matga\\Desktop\\Mateusz\\Studia\\Semestr_VI\\PSGK\\Engine\\Twin-2-Engine"};

    // Search for shader program files
    for (const auto& folder : originFolders) {
        std::filesystem::path path = folder;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            if (entry.path().extension() == SHADER_PROGRAM_EXTENSION) {
                std::ifstream shaderFile(entry.path());
                if (shaderFile.is_open()) {
                    // Read shader program file
                    std::string shaderProgramName;// , vertexShaderPath, fragmentShaderPath;
                    std::getline(shaderFile, shaderProgramName);
                    //std::getline(shaderFile, vertexShaderPath);
                    //std::getline(shaderFile, fragmentShaderPath);

                    GLuint shaderProgram = glCreateProgram();

                    std::string line;
                    std::list<unsigned int> shaderIds;
                    while (!shaderFile.eof())
                    {
                        std::getline(shaderFile, line);

                        std::cout << line << std::endl;
                        if (isEmptyOrWhitespace(line))
                        {
                            continue;
                        }

                        string extension = line.substr(line.size() - 4, 4);

                        unsigned int shaderType;
                        if (extension == "vert")
                        {
                            shaderType = GL_VERTEX_SHADER;
                        }
                        else if (extension == "frag")
                        {
                            shaderType = GL_FRAGMENT_SHADER;
                        }
                        else if (extension == "geom")
                        {
                            shaderType = GL_GEOMETRY_SHADER;
                        }
                        else
                        {
                            std::cerr << "Unrecogniced extension in shader program encountered.\n" << "Path: " << entry.path() << std::endl;
                            return 1;
                        }

                        std::string shaderSource = loadShaderSource(folder + "/" + line);

                        unsigned int shaderId = glCreateShader(shaderType);
                        const GLchar* const cstrShaderSource = (const GLchar*)shaderSource.c_str();
                        glShaderSource(shaderId, 1, &cstrShaderSource, NULL);
                        glCompileShader(shaderId);

                        checkShaderCompilationSuccess(shaderId);

                        glAttachShader(shaderProgram, shaderId);

                        shaderIds.push_back(shaderId);
                    }
                    glLinkProgram(shaderProgram);
                    checkProgramLinkingSuccess(shaderProgram);

                    //// Load shader sources
                    //std::string vertexSource = loadShaderSource(folder + "/" + vertexShaderPath);
                    //std::string fragmentSource = loadShaderSource(folder + "/" + fragmentShaderPath);
                    //
                    //
                    ////VERTEX
                    //unsigned int vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
                    //const GLchar* const vertexShaderSource = (const GLchar*)vertexSource.c_str();
                    //glShaderSource(vertexShaderID, 1, &vertexShaderSource, NULL);
                    //glCompileShader(vertexShaderID);
                    //
                    ////FRAGMENT
                    //const GLchar* const fragmentShaderSource = (const GLchar*)fragmentSource.c_str();
                    //unsigned int fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
                    //glShaderSource(fragmentShaderID, 1, &fragmentShaderSource, NULL);
                    //glCompileShader(fragmentShaderID);
                    //
                    //checkShaderCompilationSuccess(vertexShaderID);
                    //checkShaderCompilationSuccess(fragmentShaderID);
                    
                    // Create shader program
                    //GLuint shaderProgram = createShaderProgram(vertexSource, fragmentSource);

                    //Do³¹czanie shaderów
                    //glAttachShader(shaderProgram, vertexShaderID);
                    //glAttachShader(shaderProgram, fragmentShaderID);
                    //glLinkProgram(shaderProgram);
                    //checkShaderCompilationSuccess(shaderProgram);

                    for (unsigned int id : shaderIds)
                    {
                        glDeleteShader(id);
                    }
                    //glDeleteShader(fragmentShaderID);

                    if (shaderProgram != 0) {
                        //glLinkProgram(shaderProgram);
                        // Get program binary
                        GLint binaryLength;
                        glGetProgramiv(shaderProgram, GL_PROGRAM_BINARY_LENGTH, &binaryLength);
                        std::vector<GLchar> binaryData(binaryLength);
                        GLenum binaryFormat;
                        glGetProgramBinary(shaderProgram, binaryLength, nullptr, &binaryFormat, binaryData.data());

                        // Save binary to file
                        std::string outputFilePath = ORIGIN_FOLDER + std::string("/res/CompiledShaders/") + shaderProgramName + SHADER_BINARY_EXTENSION;
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
                            //std::cout << "Output file opened\n";
                        }
                        else
                        {

                            std::cout << "Output file not opened\n";
                        }
                        binaryFile.write(binaryData.data(), binaryLength);
                        binaryFile.close();

                        std::cout << "Compiled and saved shader program binary: " << outputFilePath << std::endl;
                        std::cout << "Binary format: " << binaryFormat << std::endl;

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
