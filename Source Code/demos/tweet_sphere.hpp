//
//  Tweets.h
//  Sample iOS OpenGL
//
//  Created by Julie Soliman on 10/28/13.
//  Copyright (c) 2013 Daher Alfawares. All rights reserved.
//

#ifndef Sample_iOS_OpenGL_Tweets_h
#define Sample_iOS_OpenGL_Tweets_h


#include "graphics.hpp"

namespace graphics
{

class tweet_sphere
{

        std::auto_ptr<float> data_vertecies;
        std::auto_ptr<float> data_texcoords;
        size_t size;
        static const int step;
        opengl::texture worldMap;
        opengl::program worldProgram;
        
    public:
        tweet_sphere():
        worldMap("world.png"),
        worldProgram("sphere.vsh","sphere.fsh",
                     utility::make_vector<std::string>()("time")("projection"),
                     utility::make_vector<std::string>()("position")("texcoord")
                     )
        {
            std::vector<float> vertecies;
            std::vector<float> texcoords;
            for( int theta = 0; theta < 180; theta+=this->step )
                for( int alpha = 0; alpha < 360; alpha+=this->step )
                {
                    
                    // TODO add vertices based on tweet coordinates
                    vertecies.push_back( 0.8f * sin(theta*3.14159265f/180.0f) * sin(alpha*3.14159265f/180.0f) );    // y = r sin(theta) sin(alpha)
                    vertecies.push_back( 0.7f * cos(theta*3.14159265f/180.0f) );                                    // z = r cos(theta)
                    vertecies.push_back( 0.8f * sin(theta*3.14159265f/180.0f) * cos(alpha*3.14159265f/180.0f) );    // x = r sin(theta) cos(alpha)
                    
                    texcoords.push_back(   alpha/360.0f ); // ccw
                    texcoords.push_back(   theta/180.0f );
                }
            
            this->size = vertecies.size()/3;
            this->data_vertecies.reset( new float[vertecies.size()] );
            std::copy( vertecies.begin(), vertecies.end(), this->data_vertecies.get() );
            
            this->data_texcoords.reset( new float[texcoords.size()] );
            std::copy( texcoords.begin(), texcoords.end(), this->data_texcoords.get() );
        }
        
        
        void render( GLfloat globalTime, GLfloat * matModelViewProjection )
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, worldMap );
            glUseProgram( worldProgram );
            
            // Update uniform value.
            glUniform1f(worldProgram.uniforms["time"], (GLfloat)globalTime*5.0f);
            glUniformMatrix4fv(worldProgram.uniforms["projection"], 1, GL_FALSE, matModelViewProjection);
            
            // Update attribute values.
            glVertexAttribPointer( worldProgram.attributes["position"], 3, GL_FLOAT, 0, 0, *this/*interploate(FlatSphere, Sphere, ratio, buffer, bufferSize)*/ );
            glEnableVertexAttribArray( worldProgram.attributes["position"] );
            glVertexAttribPointer( worldProgram.attributes["texcoord"], 2, GL_FLOAT, true, 0, this->texcoord() );
            glEnableVertexAttribArray( worldProgram.attributes["texcoord"]);
            
            glDrawArrays(GL_POINTS, 0, this->numberOfVertecies() );
        }
        
        operator float*(){return this->data_vertecies.get();}
        float* texcoord(){return this->data_texcoords.get();}
        size_t numberOfVertecies(){ return this->size; }
        
    };
    
}

#endif
