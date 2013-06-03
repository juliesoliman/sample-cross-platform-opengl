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

#include "graphics_utility.hpp"

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>

#include <vector>
#include <map>
#include <memory>
#include <string>
#include <iostream>

namespace graphics { namespace utility
{
    std::string content_of_file( std::string fileName )
    {
        NSString *pathname = [[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:fileName.c_str()] ofType:nil];
        
        const char *source;
        
        source = (char *)[[NSString stringWithContentsOfFile:pathname encoding:NSUTF8StringEncoding error:nil] UTF8String];
        if (!source)
        {
            NSLog(@"Failed to load shader file");
            return "";
        }
        
        return source;
    }
        
    struct image::Detail
        {
            NSData *texData;
            UIImage *uiImage;
            std::auto_ptr<unsigned char> imageData;
            CGContextRef cgcontext;
        
            unsigned int imageWidth;
            unsigned int imageHeight;
        };
        
    image::image( std::string name ): detail( new image::Detail )
        {
            detail->texData = nil;
            detail->uiImage = nil;
            detail->cgcontext = nil;
            
            NSString *path = [[NSBundle mainBundle] pathForResource:[NSString stringWithCString:name.c_str() encoding:NSASCIIStringEncoding] ofType:nil];
            detail->texData = [[NSData alloc] initWithContentsOfFile:path];
            detail->uiImage = [[UIImage alloc] initWithData:detail->texData];
            if (detail->uiImage == nil)
            {
                NSLog(@"Could not load texture: %s",name.c_str());
                return;
            }
            
            detail->imageWidth = CGImageGetWidth(detail->uiImage.CGImage);
            detail->imageHeight = CGImageGetHeight(detail->uiImage.CGImage);
            CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
            detail->imageData.reset( new unsigned char[ detail->imageHeight * detail->imageWidth * 4 ] );
            detail->cgcontext = CGBitmapContextCreate( detail->imageData.get(), detail->imageWidth, detail->imageHeight, 8, 4 * detail->imageWidth, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big );
            CGColorSpaceRelease( colorSpace );
            CGContextClearRect( detail->cgcontext, CGRectMake( 0, 0, detail->imageWidth, detail->imageHeight ) );
            CGContextTranslateCTM( detail->cgcontext, 0, detail->imageHeight - detail->imageHeight );
            CGContextDrawImage( detail->cgcontext, CGRectMake( 0, 0, detail->imageWidth, detail->imageHeight ), detail->uiImage.CGImage );
        }
        
       image::~image()
        {
            CGContextRelease(detail->cgcontext);
            delete detail;
        }
        
        int   image::width() {return detail->imageWidth;}
        int   image::height(){return detail->imageHeight;}
        void* image::data()  {return detail->imageData.get();}

    namespace device
    {
        orientation::type currentOrientation()
        {
            if( UIInterfaceOrientationIsPortrait([UIApplication sharedApplication].statusBarOrientation) )
                return orientation::portrait;
            
            return orientation::landscape;
        }
    }
} /* utility */ } /* da */

