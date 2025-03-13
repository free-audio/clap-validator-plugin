/*
 * Clap Validator Plugin
 *
 * A expanding set of plugins to test your host
 *
 * Copyright 2024-2025,Various authors, as described in the github
 * transaction log.
 *
 * This source repo is released under the MIT license,
 * The source code and license are at https://github.com/free-audio/clap-validator-plugin
 */

#include "gui.h"
#include <clap/clap.h>
#include <fmt/core.h>

#import <Cocoa/Cocoa.h>
#import <CoreFoundation/CoreFoundation.h>
#include "cvp-plugin-base.h"

@interface MyView : NSView
{
    NSTextView *textView, *transportView;
}
@end

@implementation MyView

- (void)addTextView:(NSRect)frame
{
    // Set up a frame for the NSTextView
    NSRect textViewFrame = frame; // Adjust as necessary
    textViewFrame.size.height -= 100;

    NSRect transportFrame = frame;
    transportFrame.size.height = 100;
    transportFrame.origin.y += textViewFrame.size.height;

    // Create an NSScrollView to contain the NSTextView
    NSScrollView *scrollView = [[NSScrollView alloc] initWithFrame:textViewFrame];
    [scrollView setHasVerticalScroller:YES];
    [scrollView setHasHorizontalScroller:NO];
    [scrollView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];

    // Create the NSTextView
    textView = [[NSTextView alloc] initWithFrame:scrollView.bounds];
    [textView setMinSize:NSMakeSize(0.0, scrollView.frame.size.height)];
    [textView setMaxSize:NSMakeSize(FLT_MAX, FLT_MAX)];
    [textView setVerticallyResizable:YES];
    [textView setHorizontallyResizable:NO];
    [textView setAutoresizingMask:NSViewWidthSizable];
    [textView setFont:[NSFont monospacedSystemFontOfSize:12.0 weight:NSFontWeightRegular]];
    [[textView textContainer] setContainerSize:NSMakeSize(scrollView.frame.size.width, FLT_MAX)];
    [[textView textContainer] setWidthTracksTextView:YES];
    [textView setString:@"Initial Text"]; // Optional: initial content

    // Add the NSTextView to the NSScrollView
    [scrollView setDocumentView:textView];

    // Finally, add the NSScrollView (with embedded NSTextView) to self (NSView)
    [self addSubview:scrollView];

    transportView = [[NSTextView alloc] initWithFrame:transportFrame];
    [transportView setFont:[NSFont monospacedSystemFontOfSize:12.0 weight:NSFontWeightRegular]];
    [[transportView textContainer] setContainerSize:NSMakeSize(transportFrame.size.width, FLT_MAX)];
    [transportView setString:@"Transport Info"]; // Optional: initial content
    [self addSubview:transportView];
}

- (void)setContents:(NSString *)string
{
    [textView setString:string];
}

- (void)setTransportContents:(NSString *)string
{
    [transportView setString:string];
}

@end

namespace free_audio::cvp::gui
{
namespace detail
{

static void timerCallback(CFRunLoopTimerRef timer, void *info);

struct MacOSGUIProvider : GuiProvider
{
    MyView *view;
    CFRunLoopTimerRef timer;
    MacOSGUIProvider(CVPClap *par) : GuiProvider(par)
    {
        logFmt(CLAP_LOG_INFO, "MacOS GUI Created");
        repullLog();

        auto kInterval = 10; // ms
        CFRunLoopTimerContext context = {};
        context.info = this;
        timer = CFRunLoopTimerCreate(kCFAllocatorDefault,
                                     CFAbsoluteTimeGetCurrent() + (kInterval * 0.001f),
                                     kInterval * 0.001f, 0, 0, timerCallback, &context);
        if (timer)
            CFRunLoopAddTimer(CFRunLoopGetCurrent(), timer, kCFRunLoopCommonModes);
    }

    ~MacOSGUIProvider()
    {
        CFRunLoopRemoveTimer(CFRunLoopGetCurrent(), timer, kCFRunLoopDefaultMode);
        CFRelease(timer);
    }

    bool setParent(const clap_window_t *parent) override
    {
        logFmt(CLAP_LOG_INFO, "MacOS GUI setParent adding view");
        view = [[MyView alloc] init];

        NSView *p = (NSView *)parent->cocoa;
        auto frame = [p frame];

        [view addTextView:frame];

        [p addSubview:view];

        repullLog();
        return true;
    }

    void onIdle()
    {
        bool expected{true};
        if (logDirty.compare_exchange_strong(expected, false))
        {
            repullLog();
        }

        auto ts = parent->transportQueue.dequeueMostRecent();
        if (ts.has_value())
        {
            auto tsi = transportString(*ts);
            [view setTransportContents:[NSString stringWithUTF8String:tsi.c_str()]];
        }
    };

    void repullLog()
    {
        std::ostringstream oss;
        for (auto &l : parent->logLines)
            oss << l << "\n";
        [view setContents:[NSString stringWithUTF8String:oss.str().c_str()]];
    }
};

inline static void timerCallback(CFRunLoopTimerRef timer, void *info)
{
    auto that = static_cast<MacOSGUIProvider *>(info);
    that->onIdle();
}
} // namespace detail

std::unique_ptr<GuiProvider> createGuiProvider(CVPClap *par)
{
    return std::make_unique<detail::MacOSGUIProvider>(par);
}
} // namespace free_audio::cvp::gui