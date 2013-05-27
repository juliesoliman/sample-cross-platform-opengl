//
//  Created by mmalc Crawford on 11/18/10.
//  Copyright 2010 Apple Inc. All rights reserved.
//

// Modifications:
//   Modified by Daher Alfawares on 12/20/12
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

#import <UIKit/UIKit.h>
#include <memory>
#include "graphics.hpp"

@protocol OpenGLViewDelegate <NSObject>
- (std::auto_ptr<graphics::app>)newGraphicsApp;
@end

// Original copy provided by Apple has been modified by Daher Alfawares.
// This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
// The view content is basically an EAGL surface you render your OpenGL scene into.
// Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
@interface  OpenGLView : UIView {
    
    NSInteger   animationFrameInterval;
    
    // The pixel dimensions of the CAEAGLLayer.
    int framebufferWidth;
    int framebufferHeight;
    
    // The OpenGL ES names for the framebuffer and renderbuffer used to render to this view.
    unsigned int defaultFramebuffer;
    unsigned int colorRenderbuffer;
    unsigned int depthRenderbuffer;
    unsigned int textureRenderBuffer;
    unsigned int sampleFramebuffer;
    unsigned int sampleColorRenderbuffer;
    unsigned int sampleDepthRenderbuffer;
    
    // the opengl app
    std::auto_ptr<graphics::app>  openglApp;
}

    // setup these parameters before you call 'start'
@property (nonatomic, assign) bool  useTransparentView;             // default is true
@property (nonatomic, assign) bool  accelerometerEnabled;           // default is false
@property (nonatomic, assign) float accelerometerFrequency;         // default is 20Hz
@property (nonatomic, assign) float accelerometerFilteringFactor;   // default is .3

@property (nonatomic, assign)                         id<OpenGLViewDelegate> delegate;
@property (nonatomic, readonly, getter=isAnimating) BOOL                          animating;
@property (nonatomic)                               NSInteger                     animationFrameInterval;


- (void)start; // initializes opengl and starts the animation.

- (void)startAnimation;
- (void)stopAnimation;

@end
