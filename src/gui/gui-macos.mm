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

#import <Cocoa/Cocoa.h>

@interface MyView : NSView
@end

@implementation MyView

- (void)addTextView
{
    // Set up a frame for the NSTextView
    NSRect textViewFrame = NSMakeRect(10, 10, 300, 100); // Adjust as necessary

    // Create an NSScrollView to contain the NSTextView
    NSScrollView *scrollView = [[NSScrollView alloc] initWithFrame:textViewFrame];
    [scrollView setHasVerticalScroller:YES];
    [scrollView setHasHorizontalScroller:NO];
    [scrollView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];

    // Create the NSTextView
    NSTextView *textView = [[NSTextView alloc] initWithFrame:scrollView.bounds];
    [textView setMinSize:NSMakeSize(0.0, scrollView.frame.size.height)];
    [textView setMaxSize:NSMakeSize(FLT_MAX, FLT_MAX)];
    [textView setVerticallyResizable:YES];
    [textView setHorizontallyResizable:NO];
    [textView setAutoresizingMask:NSViewWidthSizable];
    [[textView textContainer] setContainerSize:NSMakeSize(scrollView.frame.size.width, FLT_MAX)];
    [[textView textContainer] setWidthTracksTextView:YES];
    [textView setString:@"This is a sample text."]; // Optional: initial content

    // Add the NSTextView to the NSScrollView
    [scrollView setDocumentView:textView];

    // Finally, add the NSScrollView (with embedded NSTextView) to self (NSView)
    [self addSubview:scrollView];
}

@end

namespace free_audio::cvp::gui
{
namespace detail
{
struct MacOSGUIProvider : GuiProvider
{
    MyView *view;
    MacOSGUIProvider(const logFn_t &logFn) : GuiProvider(logFn)
    {
        logFmt(CLAP_LOG_INFO, "MacOS GUI Created");
    }

    virtual bool setParent(const clap_window_t *parent)
    {
        logFmt(CLAP_LOG_INFO, "MacOS GUI setParent adding view");
        view = [[MyView alloc] init];
        [view addTextView];

        NSView *p = (NSView *)parent->cocoa;
        [p addSubview:view];
        return true;
    }
};
} // namespace detail

std::unique_ptr<GuiProvider>
createGuiProvider(std::function<void(clap_log_severity, const std::string &)> logFn)
{
    return std::make_unique<detail::MacOSGUIProvider>(logFn);
}
} // namespace free_audio::cvp::gui