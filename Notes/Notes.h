/*
 * Notes.h
 */

#import <AppKit/AppKit.h>
#import <ScriptingBridge/ScriptingBridge.h>


@class NotesApplication, NotesDocument, NotesWindow, NotesAccount, NotesFolder, NotesNote, NotesAttachment;

enum NotesSaveOptions {
	NotesSaveOptionsYes = 'yes ' /* Save the file. */,
	NotesSaveOptionsNo = 'no  ' /* Do not save the file. */,
	NotesSaveOptionsAsk = 'ask ' /* Ask the user whether or not to save the file. */
};
typedef enum NotesSaveOptions NotesSaveOptions;

enum NotesPrintingErrorHandling {
	NotesPrintingErrorHandlingStandard = 'lwst' /* Standard PostScript error handling */,
	NotesPrintingErrorHandlingDetailed = 'lwdt' /* print a detailed report of PostScript errors */
};
typedef enum NotesPrintingErrorHandling NotesPrintingErrorHandling;

enum NotesSaveableFileFormat {
	NotesSaveableFileFormatNativeFormat = 'item' /* Native format */
};
typedef enum NotesSaveableFileFormat NotesSaveableFileFormat;

@protocol NotesGenericMethods

- (void) closeSaving:(NotesSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_ as:(NotesSaveableFileFormat)as;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.
- (void) duplicateTo:(SBObject *)to withProperties:(NSDictionary *)withProperties;  // Copy an object.
- (void) moveTo:(SBObject *)to;  // Move an object to a new location.

@end



/*
 * Standard Suite
 */

// The application's top-level scripting object.
@interface NotesApplication : SBApplication

//- (SBElementArray<NotesDocument *> *) documents;
- (SBElementArray *) documents;
//- (SBElementArray<NotesWindow *> *) windows;
- (SBElementArray *) windows;

@property (copy, readonly) NSString *name;  // The name of the application.
@property (readonly) BOOL frontmost;  // Is this the active application?
@property (copy, readonly) NSString *version;  // The version number of the application.

- (id) open:(id)x;  // Open a document.
- (void) print:(id)x withProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) quitSaving:(NotesSaveOptions)saving;  // Quit the application.
- (BOOL) exists:(id)x;  // Verify that an object exists.

@end

// A document.
@interface NotesDocument : SBObject <NotesGenericMethods>

@property (copy, readonly) NSString *name;  // Its name.
@property (readonly) BOOL modified;  // Has it been modified since the last save?
@property (copy, readonly) NSURL *file;  // Its location on disk, if it has one.


@end

// A window.
@interface NotesWindow : SBObject <NotesGenericMethods>

@property (copy, readonly) NSString *name;  // The title of the window.
- (NSInteger) id;  // The unique identifier of the window.
@property NSInteger index;  // The index of the window, ordered front to back.
@property NSRect bounds;  // The bounding rectangle of the window.
@property (readonly) BOOL closeable;  // Does the window have a close button?
@property (readonly) BOOL miniaturizable;  // Does the window have a minimize button?
@property BOOL miniaturized;  // Is the window minimized right now?
@property (readonly) BOOL resizable;  // Can the window be resized?
@property BOOL visible;  // Is the window visible right now?
@property (readonly) BOOL zoomable;  // Does the window have a zoom button?
@property BOOL zoomed;  // Is the window zoomed right now?
@property (copy, readonly) NotesDocument *document;  // The document whose contents are displayed in the window.


@end



/*
 * Notes Suite
 */

// the Notes application
@interface NotesApplication (NotesSuite)

//- (SBElementArray<NotesAccount *> *) accounts;
- (SBElementArray *) accounts;
//- (SBElementArray<NotesFolder *> *) folders;
- (SBElementArray *) folders;
//- (SBElementArray<NotesNote *> *) notes;
- (SBElementArray *) notes;
//- (SBElementArray<NotesAttachment *> *) attachments;
- (SBElementArray *) attachments;

@end

// a Notes account
@interface NotesAccount : SBObject <NotesGenericMethods>

//- (SBElementArray<NotesFolder *> *) folders;
- (SBElementArray *) folders;

@property (copy) NSString *name;  // the name of the account
- (NSString *) id;  // the unique identifier of the account


@end

// a folder containing notes
@interface NotesFolder : SBObject <NotesGenericMethods>

//- (SBElementArray<NotesFolder *> *) folders;
- (SBElementArray *) folders;
//- (SBElementArray<NotesNote *> *) notes;
- (SBElementArray *) notes;

@property (copy) NSString *name;  // the name of the folder
- (NSString *) id;  // the unique identifier of the folder
@property (copy, readonly) id container;  // the container of the folder


@end

// a note in the Notes application
@interface NotesNote : SBObject <NotesGenericMethods>

//- (SBElementArray<NotesAttachment *> *) attachments;
- (SBElementArray *) attachments;

@property (copy) NSString *name;  // the name of the note (normally the first line of the body)
- (NSString *) id;  // the unique identifier of the note
@property (copy, readonly) NotesFolder *container;  // the folder of the note
@property (copy) NSString *body;  // the HTML content of the note
@property (copy, readonly) NSDate *creationDate;  // the creation date of the note
@property (copy, readonly) NSDate *modificationDate;  // the modification date of the note


@end

// an attachment in a note
@interface NotesAttachment : SBObject <NotesGenericMethods>

@property (copy, readonly) NSString *name;  // the name of the attachment
- (NSString *) id;  // the unique identifier of the attachment
@property (copy, readonly) NotesNote *container;  // the note containing the attachment
@property (copy, readonly) NSString *contentIdentifier;  // the content-id URL in the note's HTML


@end

