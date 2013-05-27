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

#ifndef _GRAPHICS_OPENGL_HPP_
#define _GRAPHICS_OPENGL_HPP_


#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include <string>
#include <map>
#include <vector>

 namespace graphics { namespace opengl
{
    
    class texture
    {
        GLuint glTexture;
    public:
        
        texture(std::string name);
        operator GLuint () const { return glTexture; }
    };
    
    class cubemap
    {
        GLuint glTexture;
    public:
        
        explicit cubemap( std::vector<std::string> faces );            
        operator GLuint () const { return glTexture; }
    };
    
    class program
    {
        GLuint glProgram;
        
    public:       
        std::map<std::string,GLuint> attributes;
        std::map<std::string,GLuint> uniforms;
        
        program( std::string vertex, std::string fragment, std::vector<std::string> uniformList, std::vector<std::string> attributeList );
        virtual ~program();
        
        operator GLuint () const { return glProgram; }
        
    private:
        bool   link_program();        
        GLuint compile_shader( std::string shaderSource, GLenum type );            
        void   validate_program();            
        
    };
    
    
    class frame_buffer
    {
        // The pixel dimensions of the CAEAGLLayer.
        int framebufferWidth;
        int framebufferHeight;
        
        // The OpenGL ES names for the framebuffer and renderbuffer used to render to this view.
        unsigned int defaultFramebuffer, colorRenderbuffer, depthRenderbuffer;
        unsigned int sampleFramebuffer, sampleColorRenderbuffer, sampleDepthRenderbuffer;
        
        
    };
    
} /* opengl */ } /* da */

#endif
