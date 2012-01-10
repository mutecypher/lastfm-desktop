
#include <Cocoa/Cocoa.h>

#include <Breakpad/Breakpad.h>

#include "CrashReporter.h"

@interface Breakpad : NSObject {
   BreakpadRef breakpad;
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender;
@end

@implementation Breakpad
static BreakpadRef InitBreakpad(void) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
  BreakpadRef breakpad = 0;
  NSDictionary *plist = [[NSBundle mainBundle] infoDictionary];
  if (plist) {
    // Note: version 1.0.0.4 of the framework changed the type of the argument
    // from CFDictionaryRef to NSDictionary * on the next line:
    breakpad = BreakpadCreate(plist);
  }
  [pool release];
  return breakpad;
}

- (void)awakeFromNib {
  breakpad = InitBreakpad();
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender {
  BreakpadRelease(breakpad);
  return NSTerminateNow;
}
@end

Breakpad* gBreakpad;

unicorn::CrashReporter::CrashReporter(QObject *parent) :
    QObject(parent)
{
    gBreakpad = [Breakpad new];
    [[NSApplication sharedApplication] setDelegate: gBreakpad];

    [gBreakpad awakeFromNib];

}

unicorn::CrashReporter::~CrashReporter()
{

}
