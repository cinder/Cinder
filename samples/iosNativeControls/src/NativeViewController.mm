#import "NativeViewController.h"

#include "cinder/app/AppCocoaTouch.h"

@interface NativeViewController ()

@property (nonatomic) UIButton *infoButton;

- (NSArray *)tabBarItems;
- (void)barButtonTapped:(UIBarButtonItem *)sender;
- (void)addEmptyViewControllerToFront;
- (void)infoButtonWasTapped:(id)sender;

@end

@interface MyTableViewController : UITableViewController

@property (nonatomic, weak ) UINavigationController *parentNavigationController;

@end

@implementation NativeViewController

// note: these synthesizers aren't necessary with Clang 3.0 since it will autogenerate the same thing, but it is added for clarity
@synthesize infoButton = _infoButton;
@synthesize infoButtonCallback = _infoButtonCallback;

// -------------------------------------------------------------------------------------------------
#pragma mark - Public: Adding CinderView to Heirarchy

// Note: changing the parent viewcontroller may inhibit the App's orientation related signals.
// To regain signals like willRotate and didRotate, emit them from this view controller

// Get CinderView's parent view controller so we can add it to a our stack, then set some navigation items
- (void)addCinderViewToFront
{
	UIViewController *cinderViewParent = ci::app::getWindow()->getNativeViewController();

	self.viewControllers = @[cinderViewParent];

	cinderViewParent.title = @"CinderView's parent";
	cinderViewParent.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:self.infoButton];
	cinderViewParent.toolbarItems = [self tabBarItems];
}

// Get this app's CinderView and add it as a child in our view heirarchy, in this case the left nav bar button.
// Manually resizing is necessary.
- (void)addCinderViewAsBarButton
{
	[self addEmptyViewControllerToFront];
	UIViewController *front = [self.viewControllers objectAtIndex:0];
	UIView *cinderView = (__bridge UIView *)ci::app::getWindow()->getNative();
	cinderView.frame = CGRectMake( 0, 0, 60, self.navigationBar.frame.size.height );
	front.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:cinderView];
}

// -------------------------------------------------------------------------------------------------
#pragma mark - UIViewController overridden methods

- (void)viewDidLoad
{
	[super viewDidLoad];

	self.toolbarHidden = NO;
	UIColor *tintColor = [UIColor colorWithRed:0.0f green:0.0f blue:0.0f alpha:1.0f];
	self.navigationBar.tintColor = tintColor;

	self.toolbar.tintColor = tintColor;

	_infoButton = [UIButton buttonWithType:UIButtonTypeInfoLight];
	[_infoButton addTarget:self action:@selector(infoButtonWasTapped:) forControlEvents:UIControlEventTouchUpInside];
}

- (void)willAnimateRotationToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration
{
	NSLog(@"%@ will rotate", NSStringFromClass([self class]));
	ci::app::AppCocoaTouch::get()->emitWillRotate();
}

- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
	ci::app::AppCocoaTouch::get()->emitDidRotate();
}

// pre iOS 6
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
{
	return YES;
}

// iOS 6+
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 60000
- (NSUInteger)supportedInterfaceOrientations
{
	return UIInterfaceOrientationMaskAll;
}
#endif

// -------------------------------------------------------------------------------------------------
#pragma mark - Private UI

- (void)addEmptyViewControllerToFront
{
	UIViewController *emptyViewController = [UIViewController new];
	emptyViewController.title = @"Empty VC";
	emptyViewController.view.backgroundColor = [UIColor clearColor];

	self.viewControllers = @[emptyViewController];
	emptyViewController.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:self.infoButton];
	emptyViewController.toolbarItems = [self tabBarItems];
}

- (NSArray *)tabBarItems
{
	UIBarButtonItem *button1 = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemCompose target:self action: @selector(barButtonTapped:)];
	UIBarButtonItem *button2 = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemBookmarks target:self action: @selector(barButtonTapped:)];
	UIBarButtonItem *button3 = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemSearch target:self action: @selector(barButtonTapped:)];
	UIBarButtonItem *button4 = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemCamera target:self action: @selector(barButtonTapped:)];
	UIBarButtonItem *button5 = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemAdd target:self action: @selector(barButtonTapped:)];
	UIBarButtonItem *flexibleSpace = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil];

	button1.tag = 1;
	button2.tag = 2;
	button3.tag = 3;
	button4.tag = 4;
	button5.tag = 5;

	return @[button1, flexibleSpace, button2, flexibleSpace, button3, flexibleSpace, button4, flexibleSpace, button5];
}

- (void)barButtonTapped:(UIBarButtonItem *)sender
{
	MyTableViewController *tableViewController = [[MyTableViewController alloc] initWithStyle:UITableViewStylePlain];
	tableViewController.parentNavigationController = self;
	tableViewController.tableView.backgroundColor = [UIColor colorWithRed:1.0 green:1.0 blue:1.0 alpha:0.2];
	tableViewController.title = [NSString stringWithFormat:@"Button %d", sender.tag];
	[self pushViewController:tableViewController animated:YES];
}

- (void)infoButtonWasTapped:(id)sender
{
	if( _infoButtonCallback ) {
		_infoButtonCallback() ;
	}
}

@end

@implementation MyTableViewController

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
	static NSString *MyIdentifier = @"cell";
	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:MyIdentifier];
	if( cell == nil ) {
		cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:MyIdentifier];
		cell.backgroundView = [UIView new];
		ci::Color color( ci::CM_HSV, indexPath.row / 20.0f, 0.8f, 0.8f );
		cell.backgroundView.backgroundColor = [UIColor colorWithRed:color.r green:color.g blue:color.b alpha:1.0];
		cell.selectionStyle = UITableViewCellSelectionStyleGray;
	}
	cell.textLabel.text = [NSString stringWithFormat:@"%d", indexPath.row];
	return cell;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
	return 20;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
	UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
	UIColor *color = cell.backgroundView.backgroundColor;

	self.parentNavigationController.navigationBar.tintColor = color;
	self.parentNavigationController.toolbar.tintColor = color;
	[cell setSelected:NO animated:YES];
}

@end