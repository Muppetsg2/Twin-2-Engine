//#ifndef SHADER2_H
//#define SHADER2_H
//
//
//#include <glad/glad.h>// do³¹cz glad, by móc korzystaæ w wszystkich wymaganych przez OpenGL funkcji</glad>
//
//#include <string>
//#include <fstream>
//#include <sstream>
//#include <iostream>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//class Shader2
//{
//private:
//    void checkShaderCompilationSuccess(GLuint shaderId)
//    {
//        GLint success;
//        GLchar infoLog[512];
//        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
//        if (!success)
//        {
//            glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
//            std::cout << shaderId << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << "\n";
//        }
//    }
//
//    void checkProgramLinkingSuccess(GLuint programId)
//    {
//        GLint success;
//        GLchar infoLog[512];
//        glGetProgramiv(programId, GL_LINK_STATUS, &success);
//        if (!success) {
//            glGetProgramInfoLog(programId, 512, NULL, infoLog);
//            std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << "\n";
//        }
//    }
//public:
//    // ID program object  
//    unsigned int shaderProgramID;
//
//    // konstruktor czyta plik shadera z dysku i tworzy go  
//    Shader2(const GLchar* vertexPath, const GLchar* geometryPath, const GLchar* fragmentPath)
//    {
//        std::string vertexCode;
//        std::string geometryCode; //////
//        std::string fragmentCode;
//        std::ifstream vShaderFile;
//        std::ifstream gShaderFile; /////
//        std::ifstream fShaderFile;
//        // zapewnij by obiekt ifstream móg³ rzucaæ wyj¹tkami  
//        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
//        gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit); ///
//        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
//        try
//        {
//            // otwórz pliki  
//            vShaderFile.open(vertexPath);
//            gShaderFile.open(geometryPath);/////
//            fShaderFile.open(fragmentPath);
//            std::stringstream vShaderStream, gShaderStream, fShaderStream; ///
//            // zapisz zawartoœæ bufora pliku do strumieni  
//            vShaderStream << vShaderFile.rdbuf();
//            gShaderStream << gShaderFile.rdbuf();///
//            fShaderStream << fShaderFile.rdbuf();
//            // zamknij uchtywy do plików  
//            vShaderFile.close();
//            gShaderFile.close();///
//            fShaderFile.close();
//            // zamieñ strumieñ w ³añcuch znaków  
//            vertexCode = vShaderStream.str();
//            geometryCode = gShaderStream.str();/////
//            fragmentCode = fShaderStream.str();
//        }
//        catch (std::ifstream::failure e)
//        {
//            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
//        }
//
//        //VERTEX
//        unsigned int vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
//        const GLchar* const vertexShaderSource = (const GLchar*)vertexCode.c_str();
//        glShaderSource(vertexShaderID, 1, &vertexShaderSource, NULL);
//        glCompileShader(vertexShaderID);
//
//        //FRAGMENT
//        const GLchar* const fragmentShaderSource = (const GLchar*)fragmentCode.c_str();
//        unsigned int fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
//        glShaderSource(fragmentShaderID, 1, &fragmentShaderSource, NULL);
//        glCompileShader(fragmentShaderID);
//
//        //GEOMETRY
//        const GLchar* const geometryShaderSource = (const GLchar*)geometryCode.c_str();
//        unsigned int geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
//        glShaderSource(geometryShaderID, 1, &geometryShaderSource, NULL);
//        glCompileShader(geometryShaderID);
//
//        checkShaderCompilationSuccess(vertexShaderID);
//        checkShaderCompilationSuccess(geometryShaderID);///
//        checkShaderCompilationSuccess(fragmentShaderID);
//
//        shaderProgramID = glCreateProgram();
//
//        //Do³¹czanie shaderów
//        glAttachShader(shaderProgramID, vertexShaderID);
//        glAttachShader(shaderProgramID, geometryShaderID); ///
//        glAttachShader(shaderProgramID, fragmentShaderID);
//        glLinkProgram(shaderProgramID);
//
//        checkShaderCompilationSuccess(shaderProgramID);
//
//
//        glDeleteShader(vertexShaderID);
//        glDeleteShader(geometryShaderID); ///
//        glDeleteShader(fragmentShaderID);
//    }
//    Shader2(const GLchar* vertexPath, const GLchar* fragmentPath)
//    {
//        std::string vertexCode;
//        std::string fragmentCode;
//        std::ifstream vShaderFile;
//        std::ifstream fShaderFile;
//        // zapewnij by obiekt ifstream móg³ rzucaæ wyj¹tkami  
//        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
//        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
//        try
//        {
//            // otwórz pliki  
//            vShaderFile.open(vertexPath);
//            fShaderFile.open(fragmentPath);
//            std::stringstream vShaderStream, fShaderStream; ///
//            // zapisz zawartoœæ bufora pliku do strumieni  
//            vShaderStream << vShaderFile.rdbuf();
//            fShaderStream << fShaderFile.rdbuf();
//            // zamknij uchtywy do plików  
//            vShaderFile.close();
//            fShaderFile.close();
//            // zamieñ strumieñ w ³añcuch znaków  
//            vertexCode = vShaderStream.str();
//            fragmentCode = fShaderStream.str();
//        }
//        catch (std::ifstream::failure e)
//        {
//            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
//        }
//
//        //VERTEX
//        unsigned int vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
//        const GLchar* const vertexShaderSource = (const GLchar*)vertexCode.c_str();
//        glShaderSource(vertexShaderID, 1, &vertexShaderSource, NULL);
//        glCompileShader(vertexShaderID);
//
//        //FRAGMENT
//        const GLchar* const fragmentShaderSource = (const GLchar*)fragmentCode.c_str();
//        unsigned int fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
//        glShaderSource(fragmentShaderID, 1, &fragmentShaderSource, NULL);
//        glCompileShader(fragmentShaderID);
//
//        checkShaderCompilationSuccess(vertexShaderID);
//        checkShaderCompilationSuccess(fragmentShaderID);
//
//        shaderProgramID = glCreateProgram();
//
//        //Do³¹czanie shaderów
//        glAttachShader(shaderProgramID, vertexShaderID);
//        glAttachShader(shaderProgramID, fragmentShaderID);
//        glLinkProgram(shaderProgramID);
//
//        checkShaderCompilationSuccess(shaderProgramID);
//
//        glDeleteShader(vertexShaderID);
//        glDeleteShader(fragmentShaderID);
//    }
//    // aktywuj shader  
//    void use()
//    {
//        glUseProgram(shaderProgramID);
//    }
//    // funkcje operuj¹ce na uniformach  
//    void setBool(const std::string& name, bool value) const
//    {
//        glUniform1i(glGetUniformLocation(shaderProgramID, name.c_str()), (int)value);
//    }
//
//    void setInt(const std::string& name, int value) const
//    {
//        glUniform1i(glGetUniformLocation(shaderProgramID, name.c_str()), value);
//    }
//
//    void setFloat(const std::string& name, float value) const
//    {
//        glUniform1f(glGetUniformLocation(shaderProgramID, name.c_str()), value);
//    }
//
//    void setVec4(const std::string& name, float* value) const
//    {
//        glUniform4fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, value);
//    }
//    void setVec3(const std::string& name, float* value) const
//    {
//        glUniform3fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, value);
//    }
//
//    void setMat4(const std::string& name, glm::mat4& value) const
//    {
//        glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
//    }
//};
//
//
//#endif // !SHADER_H
