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

#ifndef _SHARED_HPP_
#define _SHARED_HPP_

#include "graphics.hpp"

namespace graphics
{
    class sphere
    {
        std::auto_ptr<float> data_vertecies;
        std::auto_ptr<float> data_texcoords;
        size_t size;
        static const int step;
        opengl::texture worldMap;
        opengl::program worldProgram;
        
    public:
        sphere():
        worldMap("world.png"),
        worldProgram("sphere.vsh","sphere.fsh",
                     utility::make_vector<std::string>()("time")("projection")("texture"),
                     utility::make_vector<std::string>()("position")("texcoord")
                     )
        {
            std::vector<float> vertecies;
            std::vector<float> texcoords;
            for( int theta = 0; theta < 180; theta+=this->step )
                for( int alpha = 0; alpha < 360; alpha+=this->step )
                {
                    vertecies.push_back( 0.8f * sin(theta*3.14159265f/180.0f) * sin(alpha*3.14159265f/180.0f) );    // y = r sin(theta) sin(alpha)
                    vertecies.push_back( 0.7f * cos(theta*3.14159265f/180.0f) );                                    // z = r cos(theta)
                    vertecies.push_back( 0.8f * sin(theta*3.14159265f/180.0f) * cos(alpha*3.14159265f/180.0f) );    // x = r sin(theta) cos(alpha)
                    
                    texcoords.push_back( 1-alpha/360.0f ); // ccw
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
    
    class flat_sphere
    {
        std::auto_ptr<float> data_vertecies;
        std::auto_ptr<float> data_texcoords;
        size_t size;
        static const int step;
        
    public:
        flat_sphere(){
            
            std::vector<float> vertecies;
            std::vector<float> texcoords;
            
            for( int theta = 0; theta < 180; theta+=this->step )
                for( int alpha = 0; alpha < 360; alpha+=this->step )
                {
                    vertecies.push_back( 1.0f - (alpha/360.0f)*2.0f );    // x = r sin(theta) cos(alpha)
                    vertecies.push_back( 0.5f - (theta/180.0f)*1.0f );    // y = r sin(theta) sin(alpha)
                    vertecies.push_back( 0.0f );                          // z = r cos(theta)
                    
                    texcoords.push_back( 1-alpha/360.0f ); // ccw
                    texcoords.push_back(   theta/180.0f );
                }
            
            this->size = vertecies.size()/3;
            this->data_vertecies.reset( new float[vertecies.size()] );
            std::copy( vertecies.begin(), vertecies.end(), this->data_vertecies.get() );
            
            this->data_texcoords.reset( new float[texcoords.size()] );
            std::copy( texcoords.begin(), texcoords.end(), this->data_texcoords.get() );
            
        }
        
        operator float*(){return this->data_vertecies.get();}
        float* texcoord(){return this->data_texcoords.get();}
        size_t numberOfVertecies(){ return this->size; }
    };
    
    
    
    class sphere_lines
    {
        std::auto_ptr<float> data_vertecies;
        std::auto_ptr<float> data_texcoords;
        size_t size;
        static const float step;
        static const float lengthFactor;
        opengl::texture worldMap;
        opengl::program worldProgram;
        
    public:
        sphere_lines():
        worldMap("world.png"),
        worldProgram("sphere_lines.vsh","sphere_lines.fsh",
                     utility::make_vector<std::string>()("time")("projection")("texture"),
                     utility::make_vector<std::string>()("position")("texcoord") )
        {
            
            std::vector<float> vertecies;
            std::vector<float> texcoords;
            
            for( float theta = 0; theta < 180; theta+=this->step )
                for( float alpha = 0; alpha < 360; alpha+=this->step )
                {
                    vertecies.push_back( 0.8f * sin(theta*3.14159265f/180.0f) * sin(alpha*3.14159265f/180.0f) );    // y = r sin(theta) sin(alpha)
                    vertecies.push_back( 0.7f * cos(theta*3.14159265f/180.0f) );                                    // z = r cos(theta)
                    vertecies.push_back( 0.8f * sin(theta*3.14159265f/180.0f) * cos(alpha*3.14159265f/180.0f) );    // x = r sin(theta) cos(alpha)
                    
                    texcoords.push_back( 1-alpha/360.0f ); // ccw
                    texcoords.push_back(   theta/180.0f );
                    
                    // upper end
                    vertecies.push_back( lengthFactor * 0.8f * sin(theta*3.14159265f/180.0f) * sin(alpha*3.14159265f/180.0f) );    // y = r sin(theta) sin(alpha)
                    vertecies.push_back( lengthFactor * 0.7f * cos(theta*3.14159265f/180.0f) );                                    // z = r cos(theta)
                    vertecies.push_back( lengthFactor * 0.8f * sin(theta*3.14159265f/180.0f) * cos(alpha*3.14159265f/180.0f) );    // x = r sin(theta) cos(alpha)
                    
                    texcoords.push_back( 1-alpha/360.0f ); // ccw
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
            glEnableVertexAttribArray(worldProgram.attributes["texcoord"]);
            
            glDrawArrays(GL_TRIANGLE_STRIP, 0, this->numberOfVertecies() );
        }
        
        operator float*(){return this->data_vertecies.get();}
        float* texcoord(){return this->data_texcoords.get();}
        size_t numberOfVertecies(){ return this->size; }
    };
    
    
    class icons_sphere
    {
        std::auto_ptr<float> data_vertecies;
        std::auto_ptr<float> data_texcoords;
        size_t size;
        static const float step;    // degrees
        opengl::program worldProgram;
        std::vector<opengl::texture> icons;
        
    public:
        icons_sphere():
        worldProgram("icon.vsh","icon.fsh",
                     utility::make_vector<std::string>()("time")("projection"),
                     utility::make_vector<std::string>()("position")("texcoord") ),
        icons( utility::make_vector<opengl::texture>()
              (opengl::texture("01-refresh.png"))
              (opengl::texture("02-redo.png"))
              (opengl::texture("03-loopback.png"))
              (opengl::texture("04-squiggle.png"))
              (opengl::texture("05-shuffle.png"))
              (opengl::texture("06-magnify.png"))
              (opengl::texture("07-map-marker.png"))
              (opengl::texture("08-chat.png"))
              (opengl::texture("09-chat-2.png"))
              (opengl::texture("10-medical.png"))
              (opengl::texture("11-clock.png"))
              (opengl::texture("12-eye.png"))
              (opengl::texture("13-target.png"))
              (opengl::texture("14-tag.png"))
              (opengl::texture("15-tags.png"))
              (opengl::texture("16-line-chart.png"))
              (opengl::texture("17-bar-chart.png"))
              (opengl::texture("18-envelope.png"))
              (opengl::texture("19-gear.png"))
              (opengl::texture("20-gear2.png"))
              (opengl::texture("21-skull.png"))
              (opengl::texture("22-skull-n-bones.png"))
              (opengl::texture("23-bird.png"))
              (opengl::texture("24-gift.png"))
              (opengl::texture("25-weather.png"))
              (opengl::texture("26-bandaid.png"))
              (opengl::texture("27-planet.png"))
              (opengl::texture("28-star.png"))
              (opengl::texture("29-heart.png"))
              (opengl::texture("30-key.png"))
              (opengl::texture("31-ipod.png"))
              (opengl::texture("32-iphone.png"))
              (opengl::texture("33-cabinet.png"))
              (opengl::texture("34-coffee.png"))
              (opengl::texture("35-shopping.png"))
              (opengl::texture("36-toolbox.png"))
              (opengl::texture("37-suitcase.png"))
              (opengl::texture("38-airplane.png"))
              (opengl::texture("39-spraycan.png"))
              (opengl::texture("40-inbox.png"))
              (opengl::texture("41-picture-frame.png"))
              (opengl::texture("42-photos.png"))
              (opengl::texture("43-film-roll.png"))
              (opengl::texture("44-shoebox.png"))
              (opengl::texture("45-movie-1.png"))
              (opengl::texture("46-movie-2.png"))
              (opengl::texture("47-fuel.png"))
              (opengl::texture("48-fork-and-knife.png"))
              (opengl::texture("49-battery.png"))
              (opengl::texture("50-beaker.png"))
              (opengl::texture("51-outlet.png"))
              (opengl::texture("52-pine-tree.png"))
              (opengl::texture("53-house.png"))
              (opengl::texture("54-lock.png"))
              (opengl::texture("55-network.png"))
              (opengl::texture("56-cloud.png"))
              (opengl::texture("57-download.png"))
              (opengl::texture("58-bookmark.png"))
              (opengl::texture("59-flag.png"))
              (opengl::texture("60-signpost.png"))
              (opengl::texture("61-brightness.png"))
              (opengl::texture("62-contrast.png"))
              (opengl::texture("63-runner.png"))
              (opengl::texture("64-zap.png"))
              (opengl::texture("65-note.png"))
              (opengl::texture("66-microphone.png"))
              (opengl::texture("67-tshirt.png"))
              (opengl::texture("68-paperclip.png"))
              (opengl::texture("69-display.png"))
              (opengl::texture("70-tv.png"))
              (opengl::texture("71-compass.png"))
              (opengl::texture("72-pin.png"))
              (opengl::texture("73-radar.png"))
              (opengl::texture("74-location.png"))
              (opengl::texture("75-phone.png"))
              (opengl::texture("76-baby.png"))
              (opengl::texture("77-ekg.png"))
              (opengl::texture("78-stopwatch.png"))
              (opengl::texture("79-medical-bag.png"))
              (opengl::texture("80-shopping-cart.png"))
              (opengl::texture("81-dashboard.png"))
              (opengl::texture("82-dog-paw.png"))
              (opengl::texture("83-calendar.png"))
              (opengl::texture("84-lightbulb.png"))
              (opengl::texture("85-trophy.png"))
              (opengl::texture("86-camera.png"))
              (opengl::texture("87-wine-glass.png"))
              (opengl::texture("88-beer-mug.png"))
              (opengl::texture("89-dumbell.png"))
              (opengl::texture("90-life-buoy.png"))
              (opengl::texture("91-beaker-2.png"))
              (opengl::texture("92-test-tube.png"))
              (opengl::texture("93-thermometer.png"))
              (opengl::texture("94-pill.png"))
              (opengl::texture("95-equalizer.png"))
              (opengl::texture("96-book.png"))
              (opengl::texture("97-puzzle.png"))
              (opengl::texture("98-palette.png"))
              (opengl::texture("99-umbrella.png"))
              (opengl::texture("100-coffee.png"))
              (opengl::texture("101-gameplan.png"))
              (opengl::texture("102-walk.png"))
              (opengl::texture("103-map.png"))
              (opengl::texture("104-index-cards.png"))
              (opengl::texture("105-piano.png"))
              (opengl::texture("106-sliders.png"))
              (opengl::texture("107-widescreen.png"))
              (opengl::texture("108-badge.png"))
              (opengl::texture("109-chicken.png"))
              (opengl::texture("110-bug.png"))
              (opengl::texture("111-user.png"))
              (opengl::texture("112-group.png"))
              (opengl::texture("113-navigation.png"))
              (opengl::texture("114-balloon.png"))
              (opengl::texture("115-bow-and-arrow.png"))
              (opengl::texture("116-controller.png"))
              (opengl::texture("117-todo.png"))
              (opengl::texture("118-coat-hanger.png"))
              (opengl::texture("119-piggy-bank.png"))
              (opengl::texture("120-headphones.png"))
              (opengl::texture("121-landscape.png"))
              (opengl::texture("122-stats.png"))
              (opengl::texture("123-id-card.png"))
              (opengl::texture("124-bullhorn.png"))
              (opengl::texture("125-food.png"))
              (opengl::texture("126-moon.png"))
              (opengl::texture("127-sock.png"))
              (opengl::texture("128-bone.png"))
              (opengl::texture("129-golf.png"))
              (opengl::texture("130-dice.png"))
              (opengl::texture("131-tower.png"))
              (opengl::texture("132-ghost.png"))
              (opengl::texture("133-ufo.png"))
              (opengl::texture("134-viking.png"))
              (opengl::texture("135-hockeymask.png"))
              (opengl::texture("136-tractor.png"))
              (opengl::texture("137-presentation.png"))
              (opengl::texture("138-scales.png"))
              (opengl::texture("139-flags.png"))
              (opengl::texture("140-gradhat.png"))
              (opengl::texture("141-lamp.png"))
              (opengl::texture("142-wine-bottle.png"))
              (opengl::texture("143-liquor-bottle.png"))
              (opengl::texture("144-martini.png"))
              (opengl::texture("145-persondot.png"))
              (opengl::texture("146-gavel.png"))
              (opengl::texture("147-popsicle.png"))
              (opengl::texture("148-doghouse.png"))
              (opengl::texture("149-windmill.png"))
              (opengl::texture("150-sailboat.png"))
              (opengl::texture("151-telescope.png"))
              (opengl::texture("152-rolodex.png"))
              (opengl::texture("153-guitar.png"))
              (opengl::texture("154-necktie.png"))
              (opengl::texture("155-bowtie.png"))
              (opengl::texture("156-controlpad.png"))
              (opengl::texture("157-wrench.png"))
              (opengl::texture("158-wrench-2.png"))
              (opengl::texture("159-voicemail.png"))
              (opengl::texture("160-voicemail-2.png"))
              (opengl::texture("161-calculator.png"))
              (opengl::texture("162-receipt.png"))
              (opengl::texture("163-glasses-1.png"))
              (opengl::texture("164-glasses-2.png"))
              (opengl::texture("165-glasses-3.png"))
              (opengl::texture("167-upload-photo.png"))
              (opengl::texture("168-upload-photo-2.png"))
              (opengl::texture("169-8ball.png"))
              (opengl::texture("170-butterfly.png"))
              (opengl::texture("171-sun.png"))
              (opengl::texture("172-pricetag.png"))
              (opengl::texture("173-eyedropper.png"))
              (opengl::texture("174-imac.png"))
              (opengl::texture("175-macbook.png"))
              (opengl::texture("176-ipad.png"))
              (opengl::texture("177-building.png"))
              (opengl::texture("178-city.png"))
              (opengl::texture("179-notepad.png"))
              (opengl::texture("180-stickynote.png"))
              (opengl::texture("181-hammer.png"))
              (opengl::texture("183-genie-lamp.png"))
              (opengl::texture("184-warning.png"))
              (opengl::texture("185-printer.png"))
              (opengl::texture("186-ruler.png"))
              (opengl::texture("187-pencil.png"))
              (opengl::texture("188-paint-roller.png"))
              (opengl::texture("189-plant.png"))
              (opengl::texture("190-bank.png"))
              (opengl::texture("191-collection.png"))
              (opengl::texture("192-credit-card.png"))
              (opengl::texture("193-location-arrow.png"))
              (opengl::texture("194-note-2.png"))
              (opengl::texture("195-barcode.png"))
              (opengl::texture("196-radiation.png"))
              (opengl::texture("197-card-diamonds.png"))
              (opengl::texture("198-card-spades.png"))
              (opengl::texture("199-card-hearts.png"))
              (opengl::texture("200-card-clubs.png"))
              )
        {
            float diff = 0.04f;
            std::vector<float> vertecies;
            std::vector<float> texcoords;
            
            for( float theta = this->step; theta < 180-this->step; theta+=this->step/2.0f )
                for( float alpha = 0; alpha < 355; alpha+=this->step-10*sin(theta*3.14159265f/180.0f) )
                {
                    float Theta = theta*3.14159265f/180.0f;
                    float Alpha = alpha*3.14159265f/180.0f;
                    float x0 = /*0.8f **/ sinf(Theta) * sinf(Alpha);
                    float y0 = /*0.7f **/ cosf(Theta);
                    float z0 = /*0.8f **/ sinf(Theta) * cosf(Alpha);
                    
                    float cos_q = cosf(Alpha);
                    float sin_q = sinf(Alpha);
                    
                    float cos_t = sinf(-Theta);
                    float sin_t = cosf(-Theta);
                    
                    //(0,0)
                    {
                        float v[] = { -diff, -diff, 0.2f };
                        float &x = v[0];
                        float &y = v[1];
                        float &z = v[2];
                        
                        float x2,y2,z2;
                        y2 = y * cos_t - z * sin_t;
                        z2 = y * sin_t + z * cos_t;
                        x2 = x;
                        
                        float x1,y1,z1;
                        z1 = z2 * cos_q - x2 * sin_q;
                        x1 = z2 * sin_q + x2 * cos_q;
                        y1 = y2;
                        
                        
                        vertecies.push_back( x1+x0 );
                        vertecies.push_back( y1+y0 );
                        vertecies.push_back( z1+z0 );
                        texcoords.push_back( 0.0f );
                        texcoords.push_back( 0.0f );
                    }
                    
                    //(0,1)
                    {
                        float v[] = { -diff, diff, 0.2f };
                        float &x = v[0];
                        float &y = v[1];
                        float &z = v[2];
                        
                        float x2,y2,z2;
                        y2 = y * cos_t - z * sin_t;
                        z2 = y * sin_t + z * cos_t;
                        x2 = x;
                        
                        float x1,y1,z1;
                        z1 = z2 * cos_q - x2 * sin_q;
                        x1 = z2 * sin_q + x2 * cos_q;
                        y1 = y2;
                        
                        
                        vertecies.push_back( x1+x0 );
                        vertecies.push_back( y1+y0 );
                        vertecies.push_back( z1+z0 );
                        texcoords.push_back( 0.0f );
                        texcoords.push_back( 1.0f );
                    }
                    
                    //(1,0)
                    {
                        float v[] = { diff, -diff,  0.2f };
                        float &x = v[0];
                        float &y = v[1];
                        float &z = v[2];
                        
                        float x2,y2,z2;
                        y2 = y * cos_t - z * sin_t;
                        z2 = y * sin_t + z * cos_t;
                        x2 = x;
                        
                        float x1,y1,z1;
                        z1 = z2 * cos_q - x2 * sin_q;
                        x1 = z2 * sin_q + x2 * cos_q;
                        y1 = y2;
                        
                        
                        vertecies.push_back( x1+x0 );
                        vertecies.push_back( y1+y0 );
                        vertecies.push_back( z1+z0 );
                        texcoords.push_back( 1.0f );
                        texcoords.push_back( 0.0f );
                    }
                    
                    //(1,1)
                    {
                        float v[] = { diff, diff, 0.2f };
                        float &x = v[0];
                        float &y = v[1];
                        float &z = v[2];
                        
                        float x2,y2,z2;
                        y2 = y * cos_t - z * sin_t;
                        z2 = y * sin_t + z * cos_t;
                        x2 = x;
                        
                        float x1,y1,z1;
                        z1 = z2 * cos_q - x2 * sin_q;
                        x1 = z2 * sin_q + x2 * cos_q;
                        y1 = y2;
                        
                        
                        vertecies.push_back( x1+x0 );
                        vertecies.push_back( y1+y0 );
                        vertecies.push_back( z1+z0 );
                        texcoords.push_back( 1.0f );
                        texcoords.push_back( 1.0f );
                        
                    }
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
            glUseProgram( worldProgram );
            
            // Update uniform value.
            glUniform1f(worldProgram.uniforms["time"], (GLfloat)globalTime*5.0f);
            glUniformMatrix4fv(worldProgram.uniforms["projection"], 1, GL_FALSE, matModelViewProjection);
            
            // Update attribute values.
            glVertexAttribPointer( worldProgram.attributes["position"], 3, GL_FLOAT, 0, 0, *this/*interploate(FlatSphere, Sphere, ratio, buffer, bufferSize)*/ );
            glEnableVertexAttribArray( worldProgram.attributes["position"] );
            glVertexAttribPointer( worldProgram.attributes["texcoord"], 2, GL_FLOAT, true, 0, this->texcoord() );
            glEnableVertexAttribArray(worldProgram.attributes["texcoord"]);
            
            int Size = numberOfVertecies()/4;
            for( int i=0; i< Size; i++ )
            {
                glBindTexture(GL_TEXTURE_2D, icons[i %icons.size() ] );
                glDrawArrays(GL_TRIANGLE_STRIP, i*4, 4 );
            }
        }
        
        operator float*(){return this->data_vertecies.get();}
        float* texcoord(){return this->data_texcoords.get();}
        size_t numberOfVertecies(){ return this->size; }
        
    };
    
    
    class slides
    {
        opengl::texture slideBoxTexture;
        opengl::program slideBoxProgram;
        
        std::vector<opengl::texture> slideTextures;
        
        GLint stride;
        
        GLuint vertexBuffer;
        GLuint elementBuffer;
        GLuint elementCount;
        
        GLuint faceVertexBuffer;
        GLuint faceElementBuffer;
        GLuint faceElementCount;
        
    public:
        slides(std::vector<std::string> textures ):
        slideBoxTexture("slideBox.png"),
        slideBoxProgram("slide_box.vsh","slide_box.fsh",
                        utility::make_vector<std::string>()("projection")("texture"),
                        utility::make_vector<std::string>()("position")("texcoord"))
        {
            for( int i=0; i<textures.size(); i++ )
            {
                slideTextures.push_back( opengl::texture(textures[i]) );
            }
            
            
            const float width  = 0.768f;
            const float height = 1.024f;
            const float length = 0.100f;
            
            // Cube structure. 100 floats.
            GLuint vertexCount = 100;
            GLfloat cubeStructure[] =
            {
                 0.50 * width, -0.50 * height, 0.50 * length, -0.00, 0.00,
                 0.50 * width, -0.50 * height,-0.50 * length,  0.33, 0.00,
                -0.50 * width, -0.50 * height,-0.50 * length,  0.33, 0.33,
                -0.50 * width, -0.50 * height, 0.50 * length, -0.00, 0.33,
                 0.50 * width,  0.50 * height, 0.50 * length,  0.67, 0.33,
                 0.50 * width, -0.50 * height, 0.50 * length,  0.33, 0.33,
                -0.50 * width, -0.50 * height, 0.50 * length,  0.33, 0.00,
                -0.50 * width,  0.50 * height, 0.50 * length,  0.67, 0.00,
                 0.50 * width,  0.50 * height,-0.50 * length,  0.67, 0.67,
                 0.50 * width, -0.50 * height,-0.50 * length,  0.33, 0.67,
                -0.50 * width,  0.50 * height,-0.50 * length,  0.67, 1.00,
                -0.50 * width, -0.50 * height,-0.50 * length,  0.33, 1.00,
                -0.50 * width,  0.50 * height, 0.50 * length,  0.33, 1.00,
                -0.50 * width, -0.50 * height, 0.50 * length, -0.00, 1.00,
                -0.50 * width, -0.50 * height,-0.50 * length, -0.00, 0.67,
                -0.50 * width,  0.50 * height,-0.50 * length,  0.33, 0.67,
                -0.50 * width,  0.50 * height,-0.50 * length, -0.00, 0.67,
                 0.50 * width,  0.50 * height,-0.50 * length, -0.00, 0.33,
                 0.50 * width,  0.50 * height, 0.50 * length,  0.33, 0.33,
                -0.50 * width,  0.50 * height, 0.50 * length,  0.33, 0.67,
            };
            
            
            // Cube indices. 36 floats.
            elementCount = 36;
            GLushort cubeIndices[] =
            {
                0,  1,  2,
                2,  3,  0,
                4,  5,  6,
                6,  7,  4,
                8,  9,  5,
                5,  4,  8,
                10, 11,  9,
                9,  8, 10,
                12, 13, 14,
                14, 15, 12,
                16, 17, 18,
                18, 19, 16,
            };
            
            // Define the stride to the "cubeStructure".
            stride = 5 * sizeof(GLfloat);
            
            glGenBuffers(1, &vertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(GLfloat), cubeStructure, GL_STATIC_DRAW);
            
            glGenBuffers(1, &elementBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementCount * sizeof(GLushort), cubeIndices, GL_STATIC_DRAW);
            
            
            // face
            GLfloat faceStructure[]=
            {
                -0.48 * width, -0.48 * height,  0.51 * length, 0,1,
                0.48 * width, -0.48 * height,  0.51 * length, 1,1,
                -0.48 * width,  0.48 * height,  0.51 * length, 0,0,
                0.48 * width,  0.48 * height,  0.51 * length, 1,0,
                
                0.48 * width, -0.48 * height, -0.51 * length, 0,1,
                -0.48 * width, -0.48 * height, -0.51 * length, 1,1,
                0.48 * width,  0.48 * height, -0.51 * length, 0,0,
                -0.48 * width,  0.48 * height, -0.51 * length, 1,0,
            };
            faceElementCount = 3*4;
            GLushort faceElements[]=
            {
                0,1,2,
                2,1,3,
                4,5,6,
                5,6,7,
            };
            
            glGenBuffers(1, &faceVertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, faceVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, 5*8* sizeof(GLfloat), faceStructure, GL_STATIC_DRAW);
            
            glGenBuffers(1, &faceElementBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceElementBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*4* sizeof(GLushort), faceElements, GL_STATIC_DRAW);
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
        
        void render( GLfloat globalTime, GLfloat * matModelViewProjection, int index )
        {
            glUseProgram(slideBoxProgram);
            
            glUniformMatrix4fv(slideBoxProgram.uniforms["projection"], 1, GL_FALSE, matModelViewProjection);
            
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
            
            glVertexAttribPointer( slideBoxProgram.attributes["position"], 3, GL_FLOAT, GL_FALSE, stride, (void *) 0);
            glEnableVertexAttribArray( slideBoxProgram.attributes["position"] );
            
            glVertexAttribPointer( slideBoxProgram.attributes["texcoord"], 2, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(GLfloat)));
            glEnableVertexAttribArray( slideBoxProgram.attributes["texcoord"] );
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, slideBoxTexture);
            glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_SHORT, (void *) 0);
            
            //face
            glBindBuffer(GL_ARRAY_BUFFER, faceVertexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceElementBuffer);
            
            glVertexAttribPointer( slideBoxProgram.attributes["position"], 3, GL_FLOAT, GL_FALSE, stride, (void *) 0);
            glEnableVertexAttribArray( slideBoxProgram.attributes["position"] );
            
            glVertexAttribPointer( slideBoxProgram.attributes["texcoord"], 2, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(GLfloat)));
            glEnableVertexAttribArray( slideBoxProgram.attributes["texcoord"] );
            
            glBindTexture(GL_TEXTURE_2D, slideTextures[index]);
            glDrawElements(GL_TRIANGLES, faceElementCount, GL_UNSIGNED_SHORT, (void *) 0);
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
        
        std::size_t size() const { return slideTextures.size(); }
        virtual ~slides()
        {
            glDeleteBuffers(1, &vertexBuffer);
            glDeleteBuffers(1, &elementBuffer);
            glDeleteBuffers(1, &faceVertexBuffer);
            glDeleteBuffers(1, &faceElementBuffer);
        }
    };
    
    
    class label
    {
        opengl::program labelProgram;
        std::vector<opengl::texture> textures;
        GLuint labelVertexBuffer;
        GLuint labelElementBuffer;
        GLuint labelElementCount;
        GLint stride;
    public:
        label(std::vector<std::string> labelTextures):stride(5*sizeof(GLfloat)),
        labelProgram("slide_label.vsh","slide_label.fsh",
                     utility::make_vector<std::string>()("projection")("texture"),
                     utility::make_vector<std::string>()("position")("texcoord"))
        {
            for( int i=0; i<labelTextures.size(); i++ )
            {
                textures.push_back( opengl::texture(labelTextures[i]) );
            }
            
            const float width  = 1.5f;
            const float height = 1.0f;
            const float length = 1.5f;
            
            // label
            GLfloat labelStructure[]=
            {
                0.48 * width, 0.48 * height, -0.51 * length, 1, 0,
                -0.48 * width, 0.48 * height, -0.51 * length, 0, 0,
                0.48 * width, 0.48 * height,  0.51 * length, 1, 1,
                -0.48 * width, 0.48 * height,  0.51 * length, 0, 1,
            };
            labelElementCount = 3*2;
            GLushort labelElements[]=
            {
                0,1,2,
                1,2,3,
            };
            
            glGenBuffers(1, &labelVertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, labelVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, 5*4* sizeof(GLfloat), labelStructure, GL_STATIC_DRAW);
            
            glGenBuffers(1, &labelElementBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, labelElementBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, labelElementCount * sizeof(GLushort), labelElements, GL_STATIC_DRAW);
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
        
        void render( GLfloat globalTime, GLfloat * matModelViewProjection, int index )
        {
            glUseProgram(labelProgram);
            
            glUniformMatrix4fv(labelProgram.uniforms["projection"], 1, GL_FALSE, matModelViewProjection);
            
            // labels.
            glBindBuffer(GL_ARRAY_BUFFER, labelVertexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, labelElementBuffer);
            
            glVertexAttribPointer( labelProgram.attributes["position"], 3, GL_FLOAT, GL_FALSE, stride, (void *) 0);
            glEnableVertexAttribArray( labelProgram.attributes["position"] );
            
            glVertexAttribPointer( labelProgram.attributes["texcoord"], 2, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(GLfloat)));
            glEnableVertexAttribArray( labelProgram.attributes["texcoord"] );
            
            glBindTexture(GL_TEXTURE_2D, textures[index]);
            glDrawElements(GL_TRIANGLES, labelElementCount, GL_UNSIGNED_SHORT, (void *) 0);
            
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
        virtual ~label()
        {
            
            glDeleteBuffers(1, &labelVertexBuffer);
            glDeleteBuffers(1, &labelElementBuffer);
            
        }
    };
    
    struct curve
    {
        std::vector<float> vertecies;
        
        curve( glm::vec3 start, glm::vec3 end, float ratio )
        {
            using namespace glm;
            
            vec3 pass = vec3(0.0f);
            
            start += 0.5f * -normalize( vec3(start) );
            end   += 0.5f * -normalize( vec3(end) );
            
            vertecies.push_back( start[0] );
            vertecies.push_back( start[1] );
            vertecies.push_back( start[2] );
            
            for( float i=1; i< 40; i++ )
            {
                //Quadratic Bézier curve
                float t = i/40.0f;
                if( t > (1-ratio) )
                    return;
                
                vec3& p0 = start;
                vec3& p1 = pass;
                vec3& p2 = end;
                
                float _1_minus_t = 1-t;
                float _1_minus_t_square = _1_minus_t * _1_minus_t;
                
                // B(t) = (1-t)^2 p0 + 2(1-t) t p1 + t^2 p2
                vec3 point = _1_minus_t_square * p0 + 2 * _1_minus_t * t * p1 + t * t * p2;
                
                vertecies.push_back( point[0] );
                vertecies.push_back( point[1] );
                vertecies.push_back( point[2] );
            }
            
            
            vertecies.push_back( end[0] );
            vertecies.push_back( end[1] );
            vertecies.push_back( end[2] );
            
        }
    };
    
    class curves
    {
        opengl::program curveProgram;
        std::vector<curve> curveList;
        
    public:
        curves():
        curveProgram("curve.vsh","curve.fsh",utility::make_vector<std::string>()("projection"),utility::make_vector<std::string>()("position"))
        {
        }
        
        void reload_curves( std::vector<glm::vec3> relationships, float ratio )
        {
            curveList.clear();
            
            for( int i=0; i< relationships.size(); i++,i++ )
                curveList.push_back( curve(relationships[i],relationships[i+1],ratio) );
        }
        
        void render( float msec, float* projection )
        {
            glUseProgram(curveProgram);
            
            glUniformMatrix4fv( curveProgram.uniforms["projection"], 1, GL_FALSE, projection );
            
            glLineWidth(2.0f);
            for( int i=0; i< curveList.size(); i++ )
            {
                glVertexAttribPointer( curveProgram.attributes["position"], 3, GL_FLOAT, GL_FALSE, 0, &curveList[i].vertecies[0] );
                glEnableVertexAttribArray( curveProgram.attributes["position"] );
                
                glDrawArrays( GL_LINE_STRIP, 0, curveList[i].vertecies.size()/3 );
            }
        }
    };
    
    class skybox
    {
        GLuint vertexBuffer;
        GLuint elementBuffer;
        
        opengl::program skyboxProgram;
        opengl::cubemap skyboxCubeMap;
    public:
        skybox():
        skyboxProgram("skybox.vsh","skybox.fsh",utility::make_vector<std::string>()("projection"),utility::make_vector<std::string>()("position") ),
        skyboxCubeMap( utility::make_vector<std::string>()("cm-px.png")("cm-nx.png")("cm-py.png")("cm-ny.png")("cm-pz.png")("cm-nz.png") )
        {
            float vertices[24] = {
                -1.0, -1.0,  1.0,
                1.0, -1.0,  1.0,
                -1.0,  1.0,  1.0,
                1.0,  1.0,  1.0,
                -1.0, -1.0, -1.0,
                1.0, -1.0, -1.0,
                -1.0,  1.0, -1.0,
                1.0,  1.0, -1.0,
            };
            glGenBuffers(1, &vertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            
            GLubyte indices[14] = {0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1};
            
            glGenBuffers(1, &elementBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        }
        
        void render( float t, float* projection )
        {
            glUseProgram(skyboxProgram);
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubeMap);
            
            glUniformMatrix4fv(skyboxProgram.uniforms["projection"], 1, GL_FALSE, projection);
            
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
            glVertexAttribPointer(skyboxProgram.attributes["position"], 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
            glEnableVertexAttribArray(skyboxProgram.attributes["position"]);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
            glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_BYTE, (void*)0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    };
    
    class transition
    {
        float duration;
        float value;
        
        bool reversed;
    public:
        transition( float Duration ):duration(Duration),value(0),reversed(false)
        {
        }
        
        void reverse()
        {
            reversed = !reversed;
        }
        
        float ratio()
        {
            return value / duration;
        }
        
        float feed( float delta )
        {
            if( reversed )
                value += delta;
            else
                value -= delta;
            
            value = graphics::utility::clamp<float>( value, 0, duration );
            return ratio();
        }
        
        void reset()
        {
            value = 0;
        }
    };
    
    class bloom
    {
        opengl::program bloomProgram;
    public:
        bloom():bloomProgram("bloom.vsh","bloom.fsh",std::vector<std::string>(),utility::make_vector<std::string>()("position")("texcoord") )
        {
        }
        
        void render(GLuint texture)
        {
            static GLfloat vex[]=
            {
                -1,-1, 0,
                1,-1, 0,
                -1, 1, 0,
                1, 1, 0,
            };
            
            static GLfloat tex[]=
            {
                0,0,
                1,0,
                0,1,
                1,1,
            };
            
            glm::mat4 projection(1);
            glVertexAttribPointer( bloomProgram.attributes["position"], 3*4, GL_FLOAT, GL_FALSE, 0, (void *) vex );
            glEnableVertexAttribArray( bloomProgram.attributes["position"] );
            
            glVertexAttribPointer( bloomProgram.attributes["texcoord"], 2*4, GL_FLOAT, GL_FALSE, 0, (void *) tex );
            glEnableVertexAttribArray( bloomProgram.attributes["texcoord"] );
            
            glBindTexture(GL_TEXTURE_2D, texture );
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    };
}

#endif // _SHARED_HPP_

