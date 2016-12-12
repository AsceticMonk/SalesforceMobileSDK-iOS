/*
 Copyright (c) 2014-present, salesforce.com, inc. All rights reserved.
 
 Redistribution and use of this software in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice, this list of conditions
 and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of
 conditions and the following disclaimer in the documentation and/or other materials provided
 with the distribution.
 * Neither the name of salesforce.com, inc. nor the names of its contributors may be used to
 endorse or promote products derived from this software without specific prior written
 permission of salesforce.com, inc.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#import "SFForcePlugin.h"
#import "CDVPlugin+SFAdditions.h"

@implementation SFForcePlugin

- (void)runCommand:(CDVPluginResult* (^)(NSDictionary *argsDict))block command:(CDVInvokedUrlCommand*)command
{
    NSDate *startTime = [NSDate date];
    NSString* callbackId = command.callbackId;
    /* NSString* jsVersionStr = */[self getVersion:command.methodName withArguments:command.arguments];
    NSDictionary *argsDict = [self getArgument:command.arguments atIndex:0];
    
    [self log:SFLogLevelDebug format:@"%@ called.", command.methodName];
 
    __weak typeof(self) weakSelf = self;
    [self.commandDelegate runInBackground:^{
        __strong typeof(weakSelf) strongSelf = weakSelf;
        CDVPluginResult* result = block(argsDict);
        [strongSelf log:SFLogLevelDebug format:@"%@ returning after %f secs.", command.methodName, -[startTime timeIntervalSinceNow]];
        [strongSelf.commandDelegate sendPluginResult:result callbackId:callbackId];
    }];
}

@end
