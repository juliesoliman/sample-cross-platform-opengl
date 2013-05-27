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

#ifndef _GRAPHICS_UTILITY_HPP_
#define _GRAPHICS_UTILITY_HPP_

#include <vector>

 namespace graphics { namespace utility
{
    std::string content_of_file( std::string fileName );
    
    template<typename _Type>
    class make_vector        
    {
        std::vector<_Type> vec;
    public:
        
        make_vector<_Type>& operator ()( _Type obj ){ vec.push_back(obj); return *this; }
        operator std::vector<_Type> () { return vec; }
    };
    
    
    inline float* interploate( float* a, float* b, float ratio, float *result, float size )
    {
        for( int i=0; i< size; i++ )
        {
            float &x0 = a[i];
            float &x1 = b[i];
            float &x = result[i];
            
            x = x0 + (x1-x0)*ratio;
        }
        return result;
    }
    
    template<typename type> type clamp( type v, type min, type max )
    {
        return v < min ? min : v > max ? max : v;
    }

    class image
    {
        struct Detail;
        struct Detail * detail;
        
    public:
        
        image( std::string name );
        ~image();
        
        int width();
        int height();
        void* data();
    };
    
    namespace device {
        struct orientation
        {
            enum orientation_e {
                portrait,
                landscape
            };
            typedef enum orientation_e type;
        };
        
        orientation::type currentOrientation();
    }
    
} /* utility */ } /* da */

#endif

