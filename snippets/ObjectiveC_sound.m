#import <AudioToolbox/AudioToolbox.h>
 
NSURL *sound;
@property (readwrite) CFURLRef soundFileURLRef;
@property (readonly) SystemSoundID soundObjectTapButton;
 
// Init
sound = [[NSBundle mainBundle] URLForResource: @"tap"
withExtension: @"aif"];
AudioServicesCreateSystemSoundID ((__bridge CFURLRef)sound, &_soundObjectTapButton);
 
// Play
AudioServicesPlaySystemSound(_soundObjectTapButton);
