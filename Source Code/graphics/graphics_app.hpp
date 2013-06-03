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

#ifndef _GRAPHICS_APP_
#define _GRAPHICS_APP_

#include "glm/glm.hpp"

namespace graphics
{
    class app
    {
    public:
        float width;
        float height;
        
        struct uses
        {
            enum uses_e {
                touchs  = 0x000000001,
                pinch   = 0x000000002,
            };
        };
        
        virtual void render( float globalTime, float globalTimeDelta )
        {
        }
        virtual void touch_began( glm::vec2 location, float globalTouchTime )
        {
        }
        virtual void touch_move( glm::vec2 location, float globalMoveTime )
        {
        }
        virtual void pinched( float velocity )
        {
        }
        virtual void tapped()
        {
        }
        virtual void doubleTapped()
        {
        }
    };
} /* graphics */

#endif