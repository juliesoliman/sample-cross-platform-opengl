//
//  Created by mmalc Crawford on 11/18/10.
//  Copyright 2010 Apple Inc. All rights reserved.
//
//  Modified by Daher Alfawares on 6/14/12.
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

#import "OpenGLView.hpp"
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

@interface  OpenGLView ()
- (void)createFramebuffer;
- (void)deleteFramebuffer;

@end

@interface  OpenGLView ()
    
@property (nonatomic, assign) float globalTime;
@property (nonatomic, assign) float globalTimeDelta;
@property (nonatomic, assign) CADisplayLink *displayLink;
@property (nonatomic) UIAccelerationValue *accel;
@property (nonatomic, retain) EAGLContext *context;

- (void)setFramebuffer;
- (BOOL)presentFramebuffer;

@end



@implementation  OpenGLView


// You must implement this method
+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

//The EAGL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:.
- (id)initWithFrame:(CGRect)frame
{   
    if ((self = [super initWithFrame: frame])) {
        self.useTransparentView             = true;
        self.accelerometerFrequency         = 20.0; // Hz
        self.accelerometerFilteringFactor   = 0.3;
        self.multipleTouchEnabled           = true;
        self.contentScaleFactor             = [UIScreen mainScreen].scale;
    }
    return self;
}

-(void)start
{
    CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
    
    if( self.useTransparentView )
    {
        eaglLayer.opaque = NO;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    }
    else
    {
        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking,
                                        kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
                                        nil];
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }
    
    EAGLContext *aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    
    if (!aContext)
        NSLog(@"Failed to create ES context");
    else if (![EAGLContext setCurrentContext:aContext])
        NSLog(@"Failed to set ES context current");
    
    [( OpenGLView *)self setContext:aContext];
    [( OpenGLView *)self setFramebuffer];
    
        // grab a new opengl app
    openglApp = [self.delegate newGraphicsApp];
    
    
    UIPinchGestureRecognizer *pinch = [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(viewWasPinched:)];
    // UIPanGestureRecognizer *pan = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(viewWasPanned:)];
    
    [self addGestureRecognizer:pinch];
    
    _animating = FALSE;
    animationFrameInterval = 1;
    self.displayLink = nil;
    
    [self setAutoresizingMask:UIViewAutoresizingFlexibleWidth|UIViewAutoresizingFlexibleHeight];
    
    UITapGestureRecognizer* tap= [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(viewWasTapped:)];
    [tap setNumberOfTapsRequired:1];
    [tap setNumberOfTouchesRequired:2];
    [self addGestureRecognizer:tap];
    
    UITapGestureRecognizer* doubleTap= [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(viewWasDoubleTapped:)];
    [doubleTap setNumberOfTapsRequired:2];
    [doubleTap setNumberOfTouchesRequired:1];
    [self addGestureRecognizer:doubleTap];

    if( self.accelerometerEnabled )
    {
        //Configure and start accelerometer
        [[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / self.accelerometerFrequency)];
        [[UIAccelerometer sharedAccelerometer] setDelegate:(id)self];
        self.accel = (UIAccelerationValue*)calloc(3, sizeof(UIAccelerationValue));
    }
    
    // add observer for background events.
    [[NSNotificationCenter defaultCenter] addObserver: self
                                             selector: @selector(didEnterBackground:)
                                                 name:UIApplicationDidEnterBackgroundNotification
                                               object:nil];
    
    [[NSNotificationCenter defaultCenter] addObserver: self
                                             selector: @selector(willEnterForeground:)
                                                 name:UIApplicationWillEnterForegroundNotification
                                               object:nil];
    
    [self startAnimation];
    
}

- (void)didEnterBackground:(NSNotification*)note
{
    [self stopAnimation];
}

- (void)willEnterForeground:(NSNotification*)note
{
    [self startAnimation];
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver: self
                                                    name:UIApplicationDidEnterBackgroundNotification
                                                  object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver: self
                                                    name:UIApplicationWillEnterForegroundNotification
                                                  object:nil];
    
    [self deleteFramebuffer];    

    if( self.accelerometerEnabled )
        free(self.accel);
    
    // Tear down context.
    if ([EAGLContext currentContext] == self.context)
        [EAGLContext setCurrentContext:nil];
}


- (void)accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration
{
    if( self.accelerometerEnabled )
    {
        //Use a basic low-pass filter to only keep the gravity in the accelerometer values
        self.accel[0] = acceleration.x * self.accelerometerFilteringFactor + self.accel[0] * (1.0 - self.accelerometerFilteringFactor);
        self.accel[1] = acceleration.y * self.accelerometerFilteringFactor + self.accel[1] * (1.0 - self.accelerometerFilteringFactor);
        self.accel[2] = acceleration.z * self.accelerometerFilteringFactor + self.accel[2] * (1.0 - self.accelerometerFilteringFactor);
    }
}


- (void)setContext:(EAGLContext *)newContext
{
    if (self.context != newContext) {
        [self deleteFramebuffer];
        
        _context = newContext;
        
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)createFramebuffer
{
    if (self.context && !defaultFramebuffer) {
        [EAGLContext setCurrentContext:self.context];
        
        // Create default framebuffer object.
        glGenFramebuffers(1, &defaultFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
        
        // Create color render buffer and allocate backing store.
        glGenRenderbuffers(1, &colorRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
        [self.context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, static_cast<GLint*>(&framebufferWidth) );
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, static_cast<GLint*>(&framebufferHeight) );
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);

        // Create depth render buffer
        glGenRenderbuffers(1, &depthRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, framebufferWidth, framebufferHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
        
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
        

        if( self.multisamplingEnabled )
        {
            // multisampling:
            glGenFramebuffers(1, &sampleFramebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, sampleFramebuffer);
            
            glGenRenderbuffers(1, &sampleColorRenderbuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, sampleColorRenderbuffer);
            glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, 4, GL_RGBA8_OES, framebufferWidth, framebufferHeight);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, sampleColorRenderbuffer);
            
            glGenRenderbuffers(1, &sampleDepthRenderbuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, sampleDepthRenderbuffer);
            glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT16, framebufferWidth, framebufferHeight);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, sampleDepthRenderbuffer);
            
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
        }
        
        // Create texture render buffer
#ifdef USE_TEXTURE_BUFFER
        // create the texture
        glGenTextures(1, &textureRenderBuffer);
        glBindTexture(GL_TEXTURE_2D, textureRenderBuffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureRenderBuffer, 0);
        
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            NSLog(@"Failed to make texture framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
        
#endif
    }
}

- (void)deleteFramebuffer
{
    if (self.context) {
        [EAGLContext setCurrentContext:self.context];
        
        if (defaultFramebuffer) {
            glDeleteFramebuffers(1, &defaultFramebuffer);
            defaultFramebuffer = 0;
        }
        
        if (colorRenderbuffer) {
            glDeleteRenderbuffers(1, &colorRenderbuffer);
            colorRenderbuffer = 0;
        }
        
        if (depthRenderbuffer) {
            glDeleteRenderbuffers(1, &depthRenderbuffer);
            depthRenderbuffer = 0;
        }
        
        if (textureRenderBuffer) {
            glDeleteTextures(1, &textureRenderBuffer);
            textureRenderBuffer = 0;
        }
    }
}

- (void)setFramebuffer
{
    if (self.context) {
        [EAGLContext setCurrentContext:self.context];
        
        if (!defaultFramebuffer)
            [self createFramebuffer];
#ifdef USE_MULTISAMPLING
        glBindFramebuffer(GL_FRAMEBUFFER, sampleFramebuffer);
        glViewport(0, 0, framebufferWidth, framebufferHeight);
#else        
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
        glViewport(0, 0, framebufferWidth, framebufferHeight);
#endif
        
#ifdef USE_TEXTURE_BUFFER
            // use texture buffer.
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureRenderBuffer, 0);
#endif
    }
}

- (BOOL)presentFramebuffer
{
    BOOL success = FALSE;
    
    if (self.context) {
        [EAGLContext setCurrentContext:self.context];
        
        
#ifdef USE_TEXTURE_BUFFER
            // switch to render buffer.
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);
        glBindTexture(GL_TEXTURE_2D, textureRenderBuffer);
            // post process.
        
        static graphics::bloom bloomEffect;
        
        bloomEffect.render(textureRenderBuffer);
#endif
    
        if( self.multisamplingEnabled )
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER_APPLE, defaultFramebuffer);
            glBindFramebuffer(GL_READ_FRAMEBUFFER_APPLE, sampleFramebuffer);
            glResolveMultisampleFramebufferAPPLE();
            
            const GLenum discards[]  = {GL_COLOR_ATTACHMENT0,GL_DEPTH_ATTACHMENT};
            glDiscardFramebufferEXT(GL_READ_FRAMEBUFFER_APPLE,2,discards);
        }
        
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
        success = [self.context presentRenderbuffer:GL_RENDERBUFFER];
    }
    
    return success;
}

- (void)layoutSubviews
{
    // The framebuffer will be re-created at the beginning of the next setFramebuffer method call.
    [self deleteFramebuffer];
}

- (void) viewWasPinched:(UIPinchGestureRecognizer *)gestureRecognizer;
{
    openglApp->pinched(gestureRecognizer.velocity);
}

- (void) viewWasPanned:(UIPanGestureRecognizer *)gestureRecognizer;
{
}

- (void)viewWasTapped:(id)i
{
    openglApp->tapped();
}
- (void)viewWasDoubleTapped:(id)i
{
    openglApp->doubleTapped();
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    if( [touches count] )
    {
        NSArray *touchArray = [touches allObjects];
        UIGestureRecognizer *touch = [touchArray objectAtIndex:0];
        CGPoint touchLocation = [touch locationInView:touch.view];
        
            // refine the touch for retina.
        touchLocation.x *= self.contentScaleFactor;
        touchLocation.y *= self.contentScaleFactor;
        
            // send the touch in the opengl space to the app.
        openglApp->touch_began( glm::vec2( touchLocation.x, touchLocation.y ), self.globalTime );
    }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    if( [touches count] )
    {
        NSArray *touchArray = [touches allObjects];
        UIGestureRecognizer *touch = [touchArray objectAtIndex:0];
        
        CGPoint touchLocation = [touch locationInView:touch.view];
        
            // refine the touch for retina.
        touchLocation.x *= self.contentScaleFactor;
        touchLocation.y *= self.contentScaleFactor;
        
            // send the touch in the opengl space to the app.
        openglApp->touch_move( glm::vec2( touchLocation.x, touchLocation.y ), self.globalTime );
    }
}

- (NSInteger)animationFrameInterval
{
    return animationFrameInterval;
}

- (void)setAnimationFrameInterval:(NSInteger)frameInterval
{
    /*
	 Frame interval defines how many display frames must pass between each time the display link fires.
	 The display link will only fire 30 times a second when the frame internal is two on a display that refreshes 60 times a second. The default frame interval setting of one will fire 60 times a second when the display refreshes at 60 times a second. A frame interval setting of less than one results in undefined behavior.
	 */
    if (frameInterval >= 1) {
        animationFrameInterval = frameInterval;
        
        if (self.animating) {
            [self stopAnimation];
            [self startAnimation];
        }
    }
}

- (void)startAnimation
{
    if (!self.animating) {
        CADisplayLink *aDisplayLink = [[UIScreen mainScreen] displayLinkWithTarget:self selector:@selector(drawFrame)];
        [aDisplayLink setFrameInterval:animationFrameInterval];
        [aDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        self.displayLink = aDisplayLink;
        
        _animating = TRUE;
    }
}

- (void)stopAnimation
{
    if (self.animating) {
        _animating = FALSE;
        [self.displayLink invalidate];
        self.displayLink = nil;
    }
}

- (void)drawFrame
{
    [self setFramebuffer];
    
    if ([self.context API] != kEAGLRenderingAPIOpenGLES2)
        return;
    
    self.globalTimeDelta    = self.globalTime - static_cast<float>( CACurrentMediaTime() );
    self.globalTime         = static_cast<float>( CACurrentMediaTime() );
    
    openglApp->width   = self.bounds.size.width * [UIScreen mainScreen].scale;
    openglApp->height  = self.bounds.size.height * [UIScreen mainScreen].scale;
    openglApp->render( self.globalTime, self.globalTimeDelta );
    
    [self presentFramebuffer];
}


@end

