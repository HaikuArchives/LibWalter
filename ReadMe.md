# LibWalter v0.1

LibWalter is a code collection which aims to fill in the gaps in the BeOS API, released under the MIT license. Designed to be compatible with BeOS R5, Zeta 1.x, and Haiku.

## Index:

#### 1. Included classes and usage notes
#### 2. How to use LibWalter
#### 3. How to contribute

### 1. Included classes and usage notes

#### AutoTextControl

BTextControl is simply inadequate for some purposes, such as sending update messages every time the user changes the text. AutoTextControl fixes this and is designed to be a drop-in replacement for BTextControl. You shouldn't need to call SetFilter, but the method is there should you need to do so. In addition to better change notification, AutoTextControl can set a limit on the number of characters in the text box, which is probably more often what you really want to do instead of limiting by byte count.

#### BarberPole

When you want to show the user that your app is busy doing something and you can't get a total for a regular progress bar, you can use this class to show the user that your app is busy doing something and isn't just frozen. Set the colors with Set[High|Low]Color. The fancy style, which is the default, has a 3D kind of effect and the simple one is "flat". Note that if you use this class, you should also, if possible, provide the user another way to see progress, such as a BStringView which shows a numeric count or something.

#### BitmapButton

Most people have a need for a button which has a picture on it at some point in time or another, and more often than not, they tend to just throw together some code to get the job done and little else. This class is a simple push button which can be disabled and shows keyboard focus. Note that it takes ownership of BBitmaps passed to it, so don't delete them.

#### BitmapListItem & BitmapMenuItem

These are also quite handy to have around. Now you can have icons in your lists and menus. Just don't delete the BBitmaps you pass to them.

#### BitmapView

This class is mostly meant to be able to show a picture with the right shape. It also comes with some whistles and bells which may come in handy in some instances. It can accept (or refuse) drops from ShowImage and Tracker. The user can paste in a picture from the clipboard. The BitmapView control can also resize dropped/pasted pictures (and keep the aspect ratio) so that they are no larger than a specified size. SetStyle allows you to have (or not have) a border around the image. It can also send a message to a target when the user modifies its picture by removing it or dropping/pasting a new picture into it. This class does NOT take ownership of bitmaps passed to it, so please take care to respond to these messages if you allow the user to change drop/paste images or you will have a memory leak on your hands.

#### ButtonDecorator

This class is useful to draw a picture with a label inside a BView. Support various alignment options and can store different pictures of different size and different states (e.g., normal, disabled, mouse over...). Currently supports only bitmaps, but SVG support is planned; widgets based on this class (like PictureButton or ToolbarButton) will get SVG support "automatically".

#### ColorWell

Now you have a simple BControl-based class which accepts color drops. Note that this class does not have its own color picker in order to allow you, the developer, the opportunity to customize color selection to your app.

If you do decide to implement a popup color picker for the ColorWell object in your app, there are some guidelines to follow for consistency's sake. Open the window on double-click. The Picker's window should be a regular window, and do not use an OK/Cancel/Apply button method -- clicking on a color in the picker should automatically update the ColorWell object in real time.

#### ComboBox

For the longest time, BeOS has not had a dropdown list which works like Windows. We developers have had to make do with BMenuField, which is not an ideal substitution. This is the ComboBox class that has been floating around the Internet for years, but with bugfixes.

#### Cursor

This is not a class, but a set of functions to help you give a custom cursor to any view, window, or the whole app. They track the mouse moviment and automatically change the mouse cursor. There's an option to "impose" the window cursor to all the window's views, or the app cursor to all the windows. Do not use the original BApplication's or BView' SetCursor() methods if you intend to use these functions.

#### DToolTip

Finally there is a free-as-in-freedom tooltip class that is simple to use! Get the singleton instance, point it at a BView you want to have a tooltip, and you're good to go!

#### EasyPath

Working with file paths using BPath, BEntry, and entry_refs can be a chore. EasyPath greatly reduces the amount of effort required in storage-intensive tasks. It is more memory-intensive than other solutions because it is string-based, but it greatly eases development headaches.

#### FontPanel

There have been lots of ways to choose fonts in programs on BeOS. All of them stink because they lack the flexibility and consistency that a single font chooser window can offer. It works in a way similar to BFilePanel when it comes to behavior like hiding after clicking OK, the modification message, and so on.

#### PathBox

This is merely a control, comprised of a BTextControl, a BButton, and a BFilePanel, which provides a simple way for a user to specify a path either by typing one in or browsing for one and optionally validating the user input for you.

#### PictureButton

A button which can show a picture and a label, with various options. Based on ButtonDecorator.

#### Spinner

A Spinner control is a combination of text box and vertical arrow buttons to increment and decrement numbers, such as the font size in the FontPanel. While Zeta has one, R5 and Haiku do not. This control provides cross-platform consistency and API simplicity.

#### SplitterView

Many times have developers written a quick-and-dirty class to split a view into two child views and allow the user to move the divider. SplitterView is such a class, complete with a resize cursor when the user moves the mouse over it. The main view can be split into more than just 2 children -- 3 or more is possible and usage is simply through AddChild like any other BView.

#### TextFile

Many times reading a text file one line at a time, is needed for one reason or another. This BFile-derived class adds one method: ReadLine(), which spits back the next \n terminated line using buffered disk access and removes the linefeed for you. Wildly convenient for reading config files. :-)

#### Toolbar, ToolbarItem, ToolbarControl, ToolbarButton & ToolbarSeparator

A classic toolbar view. Can contain several items such as buttons and separators, and you can design your own by subclassing the appropriate class. Items can be arranged in rows or columns (also in multiple lines) and there are several options for both the developer and the final user.

### How to use LibWalter

LibWalter is in a pre-alpha state, so we don't provide a precompiled library to avoid confusion. However, you can compile LibWalter and link it with your application executable, or just take only the classes you need. To get the LibWalter source code from OSDrawer you can download the Nightly SVN Tree Snapshot, or pull it via anonymous SVN.

Once you obtained the source, to compile the library you need Jam (download it from the Haiku website) or the Paladin IDE (download from BeBits or Haikuware). To build with jam, open a Terminal window, go to the directory where you placed the sources, run ./configure and then jam; this will make both the library and the test applications. To build Paladin, double-click on libwalter.pld and choose Make from the Build menu.
After you have the library (libwalter.a) compiled, it is recommended to copy it and its headers to your application's source directory to avoid incompatibility with future versions of the library. If you're using BeIDE, add the library to your project by dropping it into the project window, then add the path to the header files to the project build options.

Alternatively, you can add only the classes you need by adding the relative sources to your project.  Depending on your compiler options, you may get some warnings; it is usually safe to ignore them.


### How to contribute

If you want to help improve LibWalter, you can simply start by fixing the issues in the bug tracker or just send us an pull request with your change.
