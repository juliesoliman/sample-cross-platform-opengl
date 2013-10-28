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

uniform float time;
uniform mat4 projection;

attribute vec4 position;
attribute vec2 texcoord;

varying vec2 texcoordVarying;
varying vec4 color;

void main()
{
    gl_PointSize = 4.0;
    
    gl_Position = projection * position;
 
    //gl_Position.y += 0.05 * ( sin( time*5.0 + position.x * 4.0 ) + sin( time*5.0 + position.y *8.0 ) );
    
    texcoordVarying.s = texcoord.s;// - 0.02 * time;
    texcoordVarying.t = texcoord.t;
    
    color.r = 0.8 * sin( 0.25 * ( time + position.x * 4.0 ) + position.y * 5.0 );
    color.b = 0.8 * cos( 0.25 * ( time + position.y * 8.0 ) + position.y * 2.0 );
    color.g = 0.8 * sin( 0.25 * ( time + position.x * 0.2 ) + position.y * 8.0 );
    color.a = 1.0;
    
    float zero = 0.0;
    if( color.r < zero ) color.r *= -1.0;
    if( color.g < zero ) color.g *= -1.0;
    if( color.b < zero ) color.b *= -1.0;
}


