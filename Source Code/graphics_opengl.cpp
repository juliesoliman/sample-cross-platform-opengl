//
//   Copyright 2013 Daher Alfawares
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//


#include "graphics_opengl.hpp"
#include "graphics_utility.hpp"

#include <vector>
#include <map>
#include <memory>
#include <string>
#include <iostream>

#define USE_MULTISAMPLING
#define USE_TRILINEAR

namespace graphics { namespace opengl
{
    texture::texture(std::string name)
    {
        std::cout << "Loading texture " << name << std::endl;
        
        // load texture.
        glGenTextures(1, &glTexture);
        glBindTexture(GL_TEXTURE_2D, glTexture);
#ifdef USE_TRILINEAR
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
#else
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
#endif       
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        utility::image img(name);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data());   
        
#ifdef USE_TRILINEAR
        glGenerateMipmap(GL_TEXTURE_2D);
#endif
    }
    
    
    cubemap::cubemap( std::vector<std::string> faces )
    {
        //std::cout << "Loading cubemap " << faces[0];
        
        glGenTextures(1, &glTexture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, glTexture);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        
        //Define all 6 faces
        {
            utility::image img( faces[0] );
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data());
        }{
            utility::image img( faces[1] );
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data());
        }{
            utility::image img( faces[2] );
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data());
        }{
            utility::image img( faces[3] );
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data());
        }{
            utility::image img( faces[4] );
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data());
        }{
            utility::image img( faces[5] );
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data());
        }
    }
    
    
    program::program( std::string vertex, std::string fragment, std::vector<std::string> uniformList, std::vector<std::string> attributeList )
    {
        std::cout << "Compiling " << vertex << std::endl;
        
        // Create shader program.
        glProgram = glCreateProgram();
        
        // Create and compile vertex shader.
        std::string vertexShader = utility::content_of_file( vertex );
        GLuint vertShader = this->compile_shader( vertexShader, GL_VERTEX_SHADER );
        if( !vertShader )
        {
            std::cout << "Failed to compile vertex shader" << std::endl;
            return;
        }
        
        std::cout << "Compiling " << fragment << std::endl;
        
        // Create and compile fragment shader.
        std::string fragmentShader = utility::content_of_file( fragment );
        GLuint fragShader = this->compile_shader( fragmentShader, GL_FRAGMENT_SHADER );
        if( !fragShader )
        {
            std::cout << "Failed to compile fragment shader" << std::endl;
            return;
        }
        
        // Attach vertex shader to program.
        glAttachShader(glProgram, vertShader);
        
        // Attach fragment shader to program.
        glAttachShader(glProgram, fragShader);
        
        // Bind attribute locations.
        // This needs to be done prior to linking.
        for( size_t i=0; i< attributeList.size(); i++ )
        {
            glBindAttribLocation(glProgram, i, attributeList[i].c_str() );
            
            attributes[ attributeList[i] ] = i;
        }
        
        // Link program.
        if( !this->link_program() )
        {
            std::cout << "Failed to link program: " << glProgram << std::endl;
            
            if (vertShader)
            {
                glDeleteShader(vertShader);
                vertShader = 0;
            }
            if (fragShader)
            {
                glDeleteShader(fragShader);
                fragShader = 0;
            }
            if (glProgram)
            {
                glDeleteProgram(glProgram);
                glProgram = 0;
            }
            
            return;
        }
        
        // Get uniform locations.
        for( size_t i=0; i< uniformList.size(); i++ )
        {
            uniforms[ uniformList[i] ] = glGetUniformLocation( glProgram, uniformList[i].c_str() );
        }
        
        // Release vertex and fragment shaders.
        if (vertShader)
            glDeleteShader(vertShader);
        if (fragShader)
            glDeleteShader(fragShader);
        
        this->validate_program();
    }
    
    bool program::link_program()
    {
        GLint status;
        
        glLinkProgram(glProgram);
        
#if defined(DEBUG)
        GLint logLength;
        glGetProgramiv(glProgram, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            GLchar *log = (GLchar *)malloc(logLength);
            glGetProgramInfoLog(glProgram, logLength, &logLength, log);
            std::cout << "Program link log:\n" << log << std::endl;
            free(log);
        }
#endif
        
        glGetProgramiv(glProgram, GL_LINK_STATUS, &status);
        if (status == 0)
            return false;
        
        return true;
    }
    
    GLuint program::compile_shader( std::string shaderSource, GLenum type )
    {
        GLint status;
        GLuint shader;
        const GLchar *source = shaderSource.c_str();
        
        shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        
#if defined(DEBUG)
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            GLchar *log = (GLchar *)malloc(logLength);
            glGetShaderInfoLog(shader, logLength, &logLength, log);
            std::cout << "Shader compile log:\n" << log << std::endl;
            free(log);
        }
#endif
        
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status == 0)
        {
            glDeleteShader(shader);
            return 0;
        }
        
        return shader;
    }
    
    void program::validate_program()
    {
        GLint logLength, status;
        GLuint prog = glProgram;
        
        glValidateProgram(prog);
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            GLchar *log = (GLchar *)malloc(logLength);
            glGetProgramInfoLog(prog, logLength, &logLength, log);
            std::cout << "Program validate log:\n" << log << std::endl;
            free(log);
        }
        
        glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
    }
    
    program::~program()
    {
        if (glProgram) {
            glDeleteProgram(glProgram);
        }
    }
    
} /* opengl */ } /* da */
