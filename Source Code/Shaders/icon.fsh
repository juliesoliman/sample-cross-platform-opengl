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

uniform sampler2D texture;
varying highp vec2 texcoordVarying;
varying lowp vec4 color;

void main()
{
    gl_FragColor =  texture2D( texture, texcoordVarying );
    
    if( gl_FragColor.r < 0.1 && gl_FragColor.g < 0.1 )
        discard;//gl_FragColor = color;

    gl_FragColor = color;
    
    if( !gl_FrontFacing )
        gl_FragColor *= vec4(0.5);
}
