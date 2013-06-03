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

#ifndef _FLIPPER_HPP_
#define _FLIPPER_HPP_

#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

#include "graphics.hpp"
#include "shared.hpp"

#include <vector>
#include <map>
#include <memory>
#include <string>
#include <iostream>

#include "../graphics/glm/glm.hpp"
#include "../graphics/glm/gtc/matrix_transform.hpp" //translate, rotate, scale,

namespace demo {
    
    class Flipper : public graphics::app
    {
        glm::vec2           rotation;
        glm::vec2           rotationSpeed;
        float               angularVelocity; // top view only.
        float               angularRotation;
        
        glm::vec2           touch0;
        glm::vec2           touch1;
        
        float               touch0Time;
        float               touch1Time;
        
        float               zoomVelocity;
        float               zoomValue;
        
        graphics::transition      topViewTransition;
        graphics::transition      topViewRelationshipTransition;
        graphics::transition      topViewLabelTransition;
        
        graphics::slides          portraitSlides;
        graphics::slides          landscapeSlides;
        graphics::label           slideLabels;
        graphics::curves          relationshipCurves;
        std::vector<int>          relationships;
        
        int Taps;
    public:
        
        Flipper( std::vector<std::string> PortraitSlides, std::vector<std::string> LandscapeSlides, std::vector<std::string> SlideLabels, std::vector<int> Relationships )
        :
        topViewTransition(1.0f),
        topViewRelationshipTransition(2.0f),
        topViewLabelTransition(0.5f),
        portraitSlides(PortraitSlides),
        landscapeSlides(LandscapeSlides),
        slideLabels(SlideLabels),
        relationships(Relationships)
        {
            touch0 = glm::vec2(0.0f);
            
            zoomVelocity = 0.0f;
            zoomValue = -5;
            Taps = 16;
            
            topViewLabelTransition.reverse(); // start reversed.
        }
        
        void touch_began( glm::vec2 location, float globalTouchTime )
        {
            touch1 = location;
            touch1Time = touch0Time = globalTouchTime;
            
            rotationSpeed = glm::vec2(0.0f);
            zoomVelocity = 0;
            
            angularVelocity = 0;
        }
        void touch_move( glm::vec2 location, float globalMoveTime )
        {
            touch0      = touch1;
            touch0Time  = touch1Time;
            
            if( globalMoveTime == touch0Time )
                return;
            
            touch1 = location;
            touch1Time = globalMoveTime;
            
            float delta = float(touch1Time - touch0Time);
            
            // linear
            // speed is pixels per second
            rotationSpeed.x = 0.2f * (touch1.x - touch0.x) / delta;
            rotationSpeed.y = 0.2f * (touch1.y - touch0.y) / delta;
            
            // angular
            // speed is degrees per second
            using namespace glm;
            
            vec2 center( width/2.0f, height/2.0f );
            vec2 t0 = touch0 - center;
            vec2 t1 = touch1 - center;
            
            // a . b = |a| |b| cos theta
            
            float _a_b_ = length( t0 ) * length( t1 );
            float _a_dot_b_ = dot(t0 , t1);
            float cos_th = _a_dot_b_ / _a_b_;
            
            float theta = acosf( cos_th ) * 180.0f/ (float)M_PI;
            angularVelocity = theta / delta;
            
            if( determinant( mat2(t0,t1) ) > 0 )
                angularVelocity *= -1;
            
        }
        void tapped()
        {
            Taps++;
        }
        void doubleTapped()
        {
            topViewTransition.reverse();
            topViewRelationshipTransition.reverse();
            topViewLabelTransition.reverse();
            rotationSpeed.x = 2000;
        }
        void pinched( float velocity )
        {
            zoomVelocity = velocity;
        }
        
        float radius( float n )
        {
            const float d = 1.0f;
            
            float r = (n * d) / (2 * M_PI);
            
            return r ? r : 0.0001f;
        }
        
        void render( float globalTime, float globalTimeDelta )
        {
            using namespace glm;
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            
            // calculate radius.
            const float n = portraitSlides.size();
            
            float radius = Flipper::radius(n);
            
            
            // calculate rotation.
            
            if( topViewTransition.ratio() )
            {
                rotation.x += rotationSpeed.x * globalTimeDelta / (radius*2.0f);
                rotation.y += rotationSpeed.y * globalTimeDelta / (radius*2.0f);
                
            }
            else
            {
                rotation.x += angularVelocity * globalTimeDelta;
            }
            if( rotationSpeed.x > 10|| rotationSpeed.x < -10 )
                rotationSpeed.x /= 1.08f;
            
            rotationSpeed.y /= 1.08f;
            
            if( angularVelocity > 10 || angularVelocity < -10 )
                angularVelocity /= 1.08f;
            
            // view
            vec3 Camera;
            //      vec3 Camera0 = vec3(0,0,2 - (zoomValue = graphics::utility::clamp<float>(zoomValue - zoomVelocity * globalTimeDelta, -20, -1)));
            vec3 Camera0 = vec3(0,0,radius+7);
            vec3 Camera1 = vec3(0,radius+8,0);
            
            vec3 Normal;
            vec3 Normal0 = vec3(0,1,0);
            vec3 Normal1 = vec3(0,0,-1);
            
            float ratio = topViewTransition.feed( globalTimeDelta );
            
            graphics::utility::interploate( &Camera1[0], &Camera0[0], ratio, &Camera[0], 3);
            graphics::utility::interploate( &Normal1[0], &Normal0[0], ratio, &Normal[0], 3);
            
            mat4 View = lookAt( Camera, vec3(0,0,0), Normal );
            
            // projection
            float aspect;
            float aspect0 = 768 / 1024.0f;
            float aspect1 = width/height;
            
            graphics::utility::interploate( &aspect1, &aspect0, ratio, &aspect, 1);
            
            mat4 Projection = perspective(45.0f, aspect, 0.1f, 50.f);
            {
                static graphics::skybox skyBox;
                //
                mat4 skyboxView = View;
                
                skyboxView[3] = vec4( vec3(0.0f), 1.0f );
                
                // projection : wider projection lens for skybox.
                mat4 Projection = perspective(75.0f,  width/height, 0.1f, 50.f);
                
                glDisable(GL_DEPTH_TEST);
                skyBox.render( globalTime, &( Projection * skyboxView )[0][0] );
                glEnable(GL_DEPTH_TEST);
            }
            
            std::vector<vec3> curveEnds;
            for(  size_t i=0; i< relationships.size(); i+= 2 )
            {
                //make a circle
                float angle;
                
                angle = 180/n + 360*relationships[i]/n + rotation.x;
                curveEnds.push_back( vec3( radius * cos( angle*M_PI/180.0f ), 0, radius * sin( angle*M_PI/180.0f ) ));
                
                angle = 180/n + 360*relationships[i+1]/n + rotation.x;
                curveEnds.push_back( vec3( radius * cos( angle*M_PI/180.0f ), 0, radius * sin( angle*M_PI/180.0f ) ));
            }
            relationshipCurves.reload_curves( curveEnds, topViewRelationshipTransition.feed(globalTimeDelta) );
            relationshipCurves.render( globalTime, &(Projection * View )[0][0] );
            
            for( int i=0; i< n; i++ )
            {
                //make a circle
                float angle = 360*i/n + rotation.x;
                
                vec3 position = vec3( radius * cos( angle*M_PI/180.0f ), 0, radius * sin( angle*M_PI/180.0f ) );
                vec3 rotation = vec3( 0.0f, angle, 0.0f );
                
                float close = position.x * position.x;
                
                bool isInteresting = close < M_PI_2 && ((radius > M_PI_2) ? (position.z > 0) : true);
                
                if( isInteresting )
                {
                    position.z  += 4 * cosf( close )*(ratio);
                    rotation.y  += 90 * 3 * cosf( close )*(ratio);
                }
                
                // model
                mat4 ModelTranslate    = translate( mat4(1.0f), position );
                mat4 ModelRotateX      = rotate( ModelTranslate, rotation.x, vec3(-1.0f, 0.0f, 0.0f));
                mat4 Model             = rotate( ModelRotateX, rotation.y, vec3(0.0f,-1.0f, 0.0f));
                
                switch( graphics::utility::device::currentOrientation() )
                {
                    case graphics::utility::device::orientation::portrait:
                        portraitSlides.render( globalTime, &( Projection * View * Model )[0][0] , i );
                        break;
                    case graphics::utility::device::orientation::landscape:
                        landscapeSlides.render( globalTime, &( Projection * View * Model )[0][0] , i );
                        break;
                }
                
            }
            if( !topViewRelationshipTransition.ratio() )
            {
                int max_i = n * topViewLabelTransition.feed(globalTimeDelta);
                
                for( int i=0; i< n && i < max_i; i++ )
                {
                    //make a circle
                    float angle = 180/n + 360*i/n + rotation.x;
                    
                    vec3 position = vec3( (radius+0.25f) * cos( angle*M_PI/180.0f ), 0, (radius+0.25f) * sin( angle*M_PI/180.0f ) );
                    vec3 rotation = vec3( 0.0f, angle, 0.0f );
                    
                    // model
                    mat4 ModelTranslate    = translate( mat4(1.0f), position );
                    mat4 ModelRotateX      = rotate( ModelTranslate, rotation.x, vec3(-1.0f, 0.0f, 0.0f));
                    mat4 Model             = rotate( ModelRotateX, rotation.y, vec3(0.0f,-1.0f, 0.0f));
                    
                    slideLabels.render( globalTime, &( Projection * View * Model )[0][0], i );
                }
            }
            else
                topViewLabelTransition.reset();
        }
    };
    
    
    inline graphics::app *flipper_demo()
    {
        std::vector<std::string> portraitSlides;
        portraitSlides.push_back("001-p.png");
        portraitSlides.push_back("002-p.png");
        portraitSlides.push_back("003-p.png");
        portraitSlides.push_back("004-p.png");
        portraitSlides.push_back("005-p.png");
        portraitSlides.push_back("006-p.png");
        portraitSlides.push_back("007-p.png");
        portraitSlides.push_back("008-p.png");
        portraitSlides.push_back("009-p.png");
        portraitSlides.push_back("010-p.png");
        portraitSlides.push_back("001-p.png");
        portraitSlides.push_back("002-p.png");
        portraitSlides.push_back("003-p.png");
        portraitSlides.push_back("004-p.png");
        portraitSlides.push_back("005-p.png");
        portraitSlides.push_back("006-p.png");
        portraitSlides.push_back("007-p.png");
        portraitSlides.push_back("008-p.png");
        portraitSlides.push_back("009-p.png");
        portraitSlides.push_back("010-p.png");
        
        std::vector<std::string> landscapeSlides;
        landscapeSlides.push_back("001-l.png");
        landscapeSlides.push_back("002-l.png");
        landscapeSlides.push_back("003-l.png");
        landscapeSlides.push_back("004-l.png");
        landscapeSlides.push_back("005-l.png");
        landscapeSlides.push_back("006-l.png");
        landscapeSlides.push_back("007-l.png");
        landscapeSlides.push_back("008-l.png");
        landscapeSlides.push_back("009-l.png");
        landscapeSlides.push_back("010-l.png");
        landscapeSlides.push_back("001-l.png");
        landscapeSlides.push_back("002-l.png");
        landscapeSlides.push_back("003-l.png");
        landscapeSlides.push_back("004-l.png");
        landscapeSlides.push_back("005-l.png");
        landscapeSlides.push_back("006-l.png");
        landscapeSlides.push_back("007-l.png");
        landscapeSlides.push_back("008-l.png");
        landscapeSlides.push_back("009-l.png");
        landscapeSlides.push_back("010-l.png");
        
        std::vector<std::string> slideLabels;
        slideLabels.push_back("001-t.png");
        slideLabels.push_back("002-t.png");
        slideLabels.push_back("003-t.png");
        slideLabels.push_back("004-t.png");
        slideLabels.push_back("005-t.png");
        slideLabels.push_back("006-t.png");
        slideLabels.push_back("007-t.png");
        slideLabels.push_back("008-t.png");
        slideLabels.push_back("009-t.png");
        slideLabels.push_back("010-t.png");
        slideLabels.push_back("001-t.png");
        slideLabels.push_back("002-t.png");
        slideLabels.push_back("003-t.png");
        slideLabels.push_back("004-t.png");
        slideLabels.push_back("005-t.png");
        slideLabels.push_back("006-t.png");
        slideLabels.push_back("007-t.png");
        slideLabels.push_back("008-t.png");
        slideLabels.push_back("009-t.png");
        slideLabels.push_back("010-t.png");
        
        std::vector<int> relationships;
        relationships.push_back(  1 );
        relationships.push_back(  5 );
        relationships.push_back(  1 );
        relationships.push_back( 12 );
        relationships.push_back(  3 );
        relationships.push_back(  8 );
        relationships.push_back(  5 );
        relationships.push_back(  6 );
        relationships.push_back( 11 );
        relationships.push_back( 19 );
        relationships.push_back( 14 );
        relationships.push_back(  4 );
        
        return dynamic_cast<graphics::app*>( new demo::Flipper( portraitSlides, landscapeSlides, slideLabels, relationships ) );
    }
}

#endif

