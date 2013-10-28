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

#import "MainViewController.h"
#import "OpenGLView.hpp"
#import "demo_flipper.hpp"
#import "demo_droid.hpp"
#import "demo_tweets.hpp"

#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

@interface MainViewController()
@property (nonatomic,retain) OpenGLView *glView;
@end

@implementation MainViewController

- (void)loadView
{
    [super loadView];
    CGRect frame = [[UIScreen mainScreen] bounds];
    
        // create the view and configure it.
    [self setGlView:[[OpenGLView alloc] initWithFrame:frame]];
    
    [self.glView setUseTransparentView:false];
    [self.glView setAccelerometerEnabled:false];
    [self.glView setDelegate:self];
    [self.glView start];
    
    [self.view addSubview:self.glView];
}

-(std::auto_ptr<graphics::app>)newGraphicsApp
{
    return std::auto_ptr<graphics::app>(demo::tweets_demo());
//    return std::auto_ptr<graphics::app>(demo::flipper_demo());
    return std::auto_ptr<graphics::app>(demo::droid_demo());
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
{
    return TRUE;
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc. that aren't in use.
}

@end


