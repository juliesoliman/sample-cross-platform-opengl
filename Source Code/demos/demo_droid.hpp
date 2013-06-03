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

#ifndef _DEMO_DROID_HPP_
#define _DEMO_DROID_HPP_

#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

#include "graphics.hpp"
#include "shared.hpp"


namespace demo
{
    
    class Droid : public graphics::app
    {
        glm::vec2 rotation;
        glm::vec2 rotationSpeed;
        
        glm::vec2 touchBegin;
        float touchBeginTime;
        
        glm::vec2 touch;
        float touchTime;
        
        float zoomVelocity;
        float zoomValue;
        
        graphics::icons_sphere    icons;
//        graphics::sphere    icons;
        
    public:
        Droid()
        {
            touchBegin = glm::vec2(0.0f);
            
            zoomVelocity = 0.0f;
            zoomValue = -5;
        }
        
        ~Droid()
        {
            zoomValue = 0;
        }
        
        void touch_began( glm::vec2 location, float globalTouchTime )
        {
            touch = location;
            touchTime = touchBeginTime = globalTouchTime;
            
            rotationSpeed = glm::vec2(0.0f);
            zoomVelocity = 0;
            
        }
        void touch_move( glm::vec2 location, float globalMoveTime )
        {
            touchBegin      = touch;
            touchBeginTime  = touchTime;
            
            if( globalMoveTime == touchBeginTime )
                return;
            
            touch = location;
            touchTime = globalMoveTime;
            
            float delta = float(touchTime - touchBeginTime);
            
            //speed is pixels per second
            rotationSpeed.x = 0.2f * (touch.x - touchBegin.x) / delta;
            rotationSpeed.y = 0.2f * (touch.y - touchBegin.y) / delta;
            
        }
        void pinched( float velocity )
        {
            zoomVelocity = velocity;
        }
        
        void render( float globalTime, float globalTimeDelta )
        {
            // update
            rotation.x += rotationSpeed.x * globalTimeDelta;
            rotation.y += rotationSpeed.y * globalTimeDelta;
            
            if( rotationSpeed.x > 10|| rotationSpeed.x < -10 )
                rotationSpeed.x /= 1.08f;
            
            rotationSpeed.y /= 1.08f;
            
            using namespace glm;
            
            
            const vec3 X_Axis(1.0f,0.0f,0.0f);
            const vec3 Y_Axis(0.0f,1.0f,0.0f);
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            
            vec3 Location(0.0f,0.0f, zoomValue = clamp<float>(zoomValue - zoomVelocity * globalTimeDelta + 0.001*sin(globalTime/2.0f), -20, -1));
            if( rotation.y >  70 ) rotation.y =  70;
            if( rotation.y < -70 ) rotation.y = -70;
            vec3 Rotate( rotation.x, rotation.y, 0.0f );
            
            // model
            mat4 ModelTranslate    = translate( mat4(1.0f), Location );
            mat4 ModelRotateY      = rotate( ModelTranslate,-Rotate.y, X_Axis );
            mat4 Model             = rotate( ModelRotateY,-Rotate.x, Y_Axis );
            
            // view
            mat4 View              = lookAt( vec3(0,0,2), Location, vec3(0,1,0) );
            
            // projection
            mat4 Projection        = perspective(45.0f, width/height, 0.1f, 100.f);
            
            icons.render( globalTime, &( Projection * View * Model )[0][0] );
        }
    };

    inline graphics::app *droid_demo()
    {
        return dynamic_cast<graphics::app*>( new Droid );
    }
}

#endif
