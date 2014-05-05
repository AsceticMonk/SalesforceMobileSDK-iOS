/*
 Copyright (c) 2012, salesforce.com, inc. All rights reserved.
 Author: Kevin Hawkins
 
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

#import <Foundation/Foundation.h>
#import <SalesforceOAuth/SFOAuthCoordinator.h>
#import <SalesforceOAuth/SFOAuthInfo.h>
#import "SFUserAccountManager.h"
#import "SFIdentityCoordinator.h"

@class SFAuthorizingViewController;
@class SFAuthenticationManager;
@class SFAuthenticationViewHandler;
@class SFAuthErrorHandler;
@class SFAuthErrorHandlerList;
@class SFLoginHostUpdateResult;

/**
 Callback block definition for OAuth completion callback.
 */
typedef void (^SFOAuthFlowSuccessCallbackBlock)(SFOAuthInfo *);

/**
 Callback block definition for OAuth failure callback.
 */
typedef void (^SFOAuthFlowFailureCallbackBlock)(SFOAuthInfo *, NSError *);

/**
 Delegate protocol for SFAuthenticationManager events and callbacks.
 */
@protocol SFAuthenticationManagerDelegate <NSObject>

@optional

/**
 Called when the authentication manager is starting the auth process with an auth view.
 @param manager The instance of SFAuthenticationManager making the call.
 */
- (void)authManagerWillBeginAuthWithView:(SFAuthenticationManager *)manager;

/**
 Called when the auth view starts its load.
 @param manager The instance of SFAuthenticationManager making the call.
 */
- (void)authManagerDidStartAuthWebViewLoad:(SFAuthenticationManager *)manager;

/**
 Called when the auth view load has finished.
 @param manager The instance of SFAuthenticationManager making the call.
 */
- (void)authManagerDidFinishAuthWebViewLoad:(SFAuthenticationManager *)manager;

/**
 Called when the auth manager is going to display the auth view.
 @param manager The instance of SFAuthenticationManager making the call.
 @param view The instance of the auth view to be displayed.
 */
- (void)authManager:(SFAuthenticationManager *)manager willDisplayAuthWebView:(UIWebView *)view;

/**
 Called after the auth manager has successfully authenticated.
 @param manager The instance of SFAuthenticationManager making the call.
 @param credentials The newly-authenticated credentials.
 @param info The auth info associated with authentication.
 */
- (void)authManagerDidAuthenticate:(SFAuthenticationManager *)manager credentials:(SFOAuthCredentials *)credentials authInfo:(SFOAuthInfo *)info;

/**
 Called after the auth manager has successfully authenticated and finished retrieving the identity information.
 @param manager The instance of SFAuthenticationManager making the call.
 @param info The auth info associated with authentication.
 */
- (void)authManagerDidFinish:(SFAuthenticationManager *)manager info:(SFOAuthInfo *)info;

/**
 Called after the auth manager had failed to authenticate.
 @param manager The instance of SFAuthenticationManager making the call.
 @param error The error
 @param info The auth info associated with authentication.
 */
- (void)authManagerDidFail:(SFAuthenticationManager *)manager error:(NSError*)error info:(SFOAuthInfo *)info;

/**
 Called when the auth manager wants to determine if the network is available (best guest).
 @param manager The instance of SFAuthenticationManager making the call.
 @return YES if the network is available, NO otherwise
 */
- (BOOL)authManagerIsNetworkAvailable:(SFAuthenticationManager*)manager;

/**
 Called when the login host changes from one value to another.
 @param manager The instance of SFAuthenticationManager making the call.
 @param updateResult The results of the update, including previous login host, new login host, and whether there was
 an actual change.
 */
- (void)authManager:(SFAuthenticationManager *)manager didChangeLoginHost:(SFLoginHostUpdateResult *)updateResult;

/**
 Called after the auth manager logs out.
 @param manager The instance of SFAuthenticationManager making the call.
 */
- (void)authManagerDidLogout:(SFAuthenticationManager *)manager;

@end

/**
 Identifies the notification for the user before being logged out of the application.
 */
extern NSString * const kSFUserWillLogoutNotification;

/**
 Identifies the notification for the user being logged out of the application.
 */
extern NSString * const kSFUserLogoutNotification;

/**
 Identifies the notification for the user being logged in to the application.
 */
extern NSString * const kSFUserLoggedInNotification;

/**
 Identifies the notification when the authentication manager has finished
 successfully to authorize the user and fetched the identity information.
 */
extern NSString * const kSFAuthenticationManagerFinishedNotification;

/**
 This class handles all the authentication related tasks, which includes login, logout and session refresh
 */
@interface SFAuthenticationManager : NSObject <SFOAuthCoordinatorDelegate, SFIdentityCoordinatorDelegate, SFUserAccountManagerDelegate>

/**
 Alert view for displaying auth-related status messages.
 */
@property (nonatomic, strong) UIAlertView *statusAlert;

/**
 The view controller used to present the authentication dialog.
 */
@property (nonatomic, strong) SFAuthorizingViewController *authViewController;

/**
 Whether or not the application is currently in the process of authenticating.
 */
@property (nonatomic, readonly) BOOL authenticating;

/** Do we have a current valid Salesforce session?
 You may use KVO in your app to monitor session validity.
 */
@property (nonatomic, readonly) BOOL haveValidSession;

/**
 Returns YES if the logout is requested by the app settings
 */
@property (nonatomic, readonly) BOOL logoutSettingEnabled;

/**
 If this property is set, the authentication manager will swap a "blank" view in place
 of the currently displayed view when the app goes into the background, to protect sensitive displayed
 data from being captured in an image file by iOS.  This view will be swapped out for the original
 view when the app enters the foreground.  This property is set to YES by default.
 
 @see snapshotView
 */
@property (nonatomic, assign) BOOL useSnapshotView;

/**
 A view to be swapped in for the currently displayed view when the app enters the background, to prevent
 iOS from capturing sensitive data into an image file.  By default, this will be an opaque white screen,
 but you can set this property to any UIView, prior to app backgrounding, to use that view instead.
 
 @see useSnapshotView which toggles this behavior.
 */
@property (nonatomic, strong) UIView *snapshotView;

/**
 The preferred passcode provider to use.  In this release, defaults to
 kSFPasscodeProviderPBKDF2.  See SFPasscodeProviderManager.
 NOTE: If you wanted to set your own provider, you could do the following:
         id<SFPasscodeProvider> *myProvider = [[MyProvider alloc] initWithProviderName:myProviderName];
         [SFPasscodeProviderManager addPasscodeProvider:myProvider];
         [SFAuthenticationManager sharedManager].preferredPasscodeProvider = myProviderName;
 */
@property (nonatomic, copy) NSString *preferredPasscodeProvider;

/**
 The class instance to be used to instantiate the singleton.
 */
+ (void)setInstanceClass:(Class)class;

/**
 The singleton instance of the SFAuthenticationManager class.
 */
+ (instancetype)sharedManager;

/**
 The property denoting the block that will handle the display and dismissal of the authentication view.
 You can override this handler if you want to have a custom work flow for displaying the authentication
 view.  If you'd simply prefer to display the view in your own style, you can leave this property set
 to the default, and override the authViewController property with your style changes.
 */
@property (nonatomic, strong) SFAuthenticationViewHandler *authViewHandler;

/**
 The auth handler for invalid credentials.
 */
@property (nonatomic, readonly) SFAuthErrorHandler *invalidCredentialsAuthErrorHandler;

/**
 The auth handler for Connected App version errors.
 */
@property (nonatomic, readonly) SFAuthErrorHandler *connectedAppVersionAuthErrorHandler;

/**
 The auth handler for failures due to network connectivity.
 */
@property (nonatomic, readonly) SFAuthErrorHandler *networkFailureAuthErrorHandler;

/**
 The generic auth handler for any unhandled errors.
 */
@property (nonatomic, readonly) SFAuthErrorHandler *genericAuthErrorHandler;

/**
 The list of auth error handler filters to pass each authentication error through.  You can add or
 remove items from this list to change the flow of auth error handling.
 */
@property (nonatomic, strong) SFAuthErrorHandlerList *authErrorHandlerList;

/**
 The OAuth Coordinator associated with the current account.
 */
@property (nonatomic, strong) SFOAuthCoordinator *coordinator;

/**
 The Identity Coordinator associated with the current account.
 */
@property (nonatomic, strong) SFIdentityCoordinator *idCoordinator;

/**
 * Whether or not there is a mobile pin code policy configured for this app.
 * @return YES if so, NO if not.
 */
@property (nonatomic, readonly) BOOL mobilePinPolicyConfigured;

/**
 Adds a delegate to the list of authentication manager delegates.
 @param delegate The delegate to add to the list.
 */
- (void)addDelegate:(id<SFAuthenticationManagerDelegate>)delegate;

/**
 Removes a delegate from the delegate list.  No action is taken if the delegate does not exist.
 */
- (void)removeDelegate:(id<SFAuthenticationManagerDelegate>)delegate;

/**
 Kick off the login process for either the current user, or a new user if the current user is not
 configured.
 @param completionBlock The block of code to execute when the authentication process successfully completes.
 @param failureBlock The block of code to execute when the authentication process has a fatal failure.
 @return YES if this call kicks off the authentication process.  NO if an authentication process has already
 started, in which case subsequent requests are queued up to have their completion or failure blocks executed
 in succession.
 */
- (BOOL)loginWithCompletion:(SFOAuthFlowSuccessCallbackBlock)completionBlock
                    failure:(SFOAuthFlowFailureCallbackBlock)failureBlock;

/**
 Kick off the login process for the given user.
 @param completionBlock The block of code to execute when the authentication process successfully completes.
 @param failureBlock The block of code to execute when the authentication process has a fatal failure.
 @param 
 @return YES if this call kicks off the authentication process.  NO if an authentication process has already
 started, in which case subsequent requests are queued up to have their completion or failure blocks executed
 in succession.
 */
- (BOOL)loginWithCompletion:(SFOAuthFlowSuccessCallbackBlock)completionBlock
                    failure:(SFOAuthFlowFailureCallbackBlock)failureBlock
                    account:(SFUserAccount *)account;

/**
 Forces a logout from the current account, redirecting the user to the login process.
 This throws out the OAuth refresh token.
 */
- (void)logout;

/**
 Performs a logout on the specified user.  Note that if the user is not the current user of the app, the
 specified user's authenticated state will be removed, but no other action will otherwise interrupt the
 current app state.
 @param user The user to log out.
 */
- (void)logoutUser:(SFUserAccount *)user;

/**
 Cancels an in-progress authentication.  In-progress authentication state will be cleared.
 */
- (void)cancelAuthentication;

/**
 Notification handler for when the app finishes launching.
 @param notification The notification data associated with the event.
 */
- (void)appDidFinishLaunching:(NSNotification *)notification;

/**
 Notification handler for when the app enters the foreground.
 @param notification The notification data associated with the event.
 */
- (void)appWillEnterForeground:(NSNotification *)notification;

/**
 Notification handler for when the app enters the background.
 @param notification The notification data associated with the event.
 */
- (void)appDidEnterBackground:(NSNotification *)notification;

/**
 Notification handler for when the app will be terminated.
 @param notification The notification data associated with the event.
 */
- (void)appWillTerminate:(NSNotification *)notification;

/**
 Clears session cookie data from the cookie store, and sets a new session cookie based on the
 OAuth credentials.
 */
+ (void)resetSessionCookie;

/**
 Creates an absolute URL to frontdoor with the given destination URL.
 @param returnUrl The destination URL to hit after going through frontdoor.
 @param isEncoded Whether or not the returnUrl value is URL-encoded.
 @return An NSURL object representing the configured frontdoor URL.
 */
+ (NSURL *)frontDoorUrlWithReturnUrl:(NSString *)returnUrl returnUrlIsEncoded:(BOOL)isEncoded;

/**
 Whether or not the given URL can be identified as a redirect to the login URL, loaded when the
 session expires.
 @param url The URL to evaluate.
 @return YES if the URL matches the login redirect URL pattern, NO otherwise.
 */
+ (BOOL)isLoginRedirectUrl:(NSURL *)url;

/**
 Determines whether an error is due to invalid auth credentials.
 @param error The error to check against an invalid credentials error.
 @return YES if the error is due to invalid credentials, NO otherwise.
 */
+ (BOOL)errorIsInvalidAuthCredentials:(NSError *)error;

/**
 Remove any cookies with the given names from the given domains.
 @param cookieNames The names of the cookies to remove.
 @param domainNames The names of the domains where the cookies are set.
 */
+ (void)removeCookies:(NSArray *)cookieNames fromDomains:(NSArray *)domainNames;

/**
 Remove all cookies from the cookie store.
 */
+ (void)removeAllCookies;

/**
 Adds the access (session) token cookie to the web view, for authentication.
 @param domain The domain on which to set the cookie.
 */
+ (void)addSidCookieForDomain:(NSString*)domain;

@end