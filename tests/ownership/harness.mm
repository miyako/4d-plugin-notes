// Ownership regressions for the Notes plugin's 4D string/picture/object handling.
// Unity build of the real 4DPlugin-Notes.cpp plus the real SDK, C_TEXT.cpp and
// support/4DPlugin-JSON.cpp, running against tests/ownership/fake_engine
// instead of 4D. Notes.app is never contacted and no Notes data is read:
// SBApplication, NSAppleScript and NSWorkspace are replaced by fakes below.
// Usage: harness [expected-failure ...]  (names of tests that must fail)
#import <AppKit/AppKit.h>
#import <ScriptingBridge/ScriptingBridge.h>
#import <Carbon/Carbon.h>

@interface FakeSBApplication : NSObject
+ (id)applicationWithBundleIdentifier:(NSString *)bundleIdentifier;
@end
@interface FakeAppleScript : NSObject
- (id)initWithSource:(NSString *)source;
- (BOOL)compileAndReturnError:(NSDictionary **)error;
- (NSAppleEventDescriptor *)executeAndReturnError:(NSDictionary **)error;
- (NSAppleEventDescriptor *)executeAppleEvent:(NSAppleEventDescriptor *)event error:(NSDictionary **)error;
@end
@interface FakeWorkspace : NSObject
+ (FakeWorkspace *)sharedWorkspace;
- (NSString *)absolutePathForAppBundleWithIdentifier:(NSString *)bundleIdentifier;
@end

#define SBApplication FakeSBApplication
#define NSAppleScript FakeAppleScript
#define NSWorkspace FakeWorkspace
#include "4DPlugin-Notes.cpp"
#undef SBApplication
#undef NSAppleScript
#undef NSWorkspace

#include "fake_engine.h"
#include <cstdio>
#include <functional>
#include <set>
#include <string>

using namespace fake4d;

static int gScripts = 0;

@implementation FakeAppleScript
- (id)initWithSource:(NSString *)source { gScripts++; return [super init]; }
- (BOOL)compileAndReturnError:(NSDictionary **)error { return NO; } // never runs a script
- (NSAppleEventDescriptor *)executeAndReturnError:(NSDictionary **)error { abort(); }
- (NSAppleEventDescriptor *)executeAppleEvent:(NSAppleEventDescriptor *)e error:(NSDictionary **)error { abort(); }
@end

@implementation FakeWorkspace
+ (FakeWorkspace *)sharedWorkspace { return nil; }
- (NSString *)absolutePathForAppBundleWithIdentifier:(NSString *)bundleIdentifier { return nil; }
@end

// Minimal stand-in for an SBElementArray of scripting objects with an "id".
@interface FakeElements : NSObject
@property (retain) NSMutableArray *items;
@end
@implementation FakeElements
- (id)init { if ((self = [super init])) _items = [[NSMutableArray alloc] init]; return self; }
- (void)dealloc { [_items release]; [super dealloc]; }
- (NSUInteger)count { return [_items count]; }
- (id)objectAtIndex:(NSUInteger)i { return [_items objectAtIndex:i]; }
- (void)insertObject:(id)o atIndex:(NSUInteger)i { [_items insertObject:o atIndex:i]; }
- (NSArray *)arrayByApplyingSelector:(SEL)sel { return [_items valueForKey:NSStringFromSelector(sel)]; }
- (id)objectWithID:(NSString *)uid {
    for (id o in _items) if ([[o valueForKey:@"id"] isEqualToString:uid]) return o;
    return nil;
}
@end

@interface FakeNote : NSObject
@property (copy) NSString *id;
@property (retain) NSDate *creationDate;
@property (retain) NSDate *modificationDate;
@end
@implementation FakeNote
- (id)initWithProperties:(NSDictionary *)props {
    if ((self = [super init])) {
        _id = [@"x-coredata://fake/ICNote/p1" copy];
        _creationDate = [[NSDate dateWithTimeIntervalSince1970:0] retain];
        _modificationDate = [_creationDate retain];
    }
    return self;
}
- (void)dealloc { [_id release]; [_creationDate release]; [_modificationDate release]; [super dealloc]; }
@end

@interface FakeFolder : NSObject
@property (copy) NSString *id;
@property (retain) FakeElements *notes;
@end
@implementation FakeFolder
- (void)dealloc { [_id release]; [_notes release]; [super dealloc]; }
@end

@interface FakeApp : NSObject
@property (retain) FakeElements *folders;
@property (retain) FakeElements *notes;
@end
@implementation FakeApp
- (void)dealloc { [_folders release]; [_notes release]; [super dealloc]; }
- (Class)classForScriptingClass:(NSString *)name { return [FakeNote class]; }
@end

static FakeApp *gApp = nil;

@implementation FakeSBApplication
+ (id)applicationWithBundleIdentifier:(NSString *)bundleIdentifier { return gApp; }
@end

static std::vector<std::string> gFailures;
#define CHECK(cond) do { if (!(cond)) gFailures.push_back(#cond); } while (0)

static PA_Unistring unistring(const char *s) {
    CUTF16String u(s, s + strlen(s));
    return PA_CreateUnistring((PA_Unichar *)u.c_str());
}

// One synthetic Notes app with folder "F1" (whose notes list also backs
// [application notes], so the created note can be found again by id).
static void setup_app() {
    gApp = [[FakeApp alloc] init];
    FakeFolder *folder = [[FakeFolder alloc] init];
    folder.id = @"F1";
    FakeElements *notes = [[FakeElements alloc] init];
    folder.notes = notes;
    gApp.notes = notes;
    [notes release];
    gApp.folders = [[[FakeElements alloc] init] autorelease];
    [gApp.folders.items addObject:folder];
    [folder release];
}

static void teardown_app() { [gApp release]; gApp = nil; }

// Calls Notes Create note through the real command with a synthetic
// (&T;&T;&T;&Y;&Y) parameter block; attachments/names are borrowed.
static PA_ObjectRef create_note(PA_Variable *attachments, PA_Variable *names) {
    PA_Unistring folder = unistring("F1"), title = unistring("t"), body = unistring("b");
    PA_Variable undef4 = PA_CreateVariable(eVK_Undefined), undef5 = PA_CreateVariable(eVK_Undefined);
    void *params[5] = {&folder, &title, &body, attachments ? attachments : &undef4, names ? names : &undef5};
    PA_ObjectRef result = NULL;
    PluginBlock block;
    block.fParameters = params;
    block.fResult = &result;
    @autoreleasepool { Notes_Create_note(&block); }
    PA_DisposeUnistring(&folder); PA_DisposeUnistring(&title); PA_DisposeUnistring(&body);
    return result;
}

// 4D array: fData is a handle to element storage, element 0 included.
struct PictureArray {
    PA_Picture items[3];
    PA_Picture *storage = items;
    PA_Variable var;
    PictureArray() {
        items[0] = NULL;
        items[1] = PA_CreatePicture(NULL, 0);
        items[2] = PA_CreatePicture(NULL, 0);
        var.fType = eVK_ArrayPicture;
        var.fFiller = 0;
        var.uValue.fArray.fCurrent = 0;
        var.uValue.fArray.fNbElements = 2;
        var.uValue.fArray.fData = (PA_Handle)&storage;
    }
    ~PictureArray() { PA_DisposePicture(items[1]); PA_DisposePicture(items[2]); }
};

struct TextArray {
    PA_Unistring items[3];
    PA_Unistring *storage = items;
    PA_Variable var;
    TextArray() {
        items[0] = unistring("");
        items[1] = unistring("one.png");
        items[2] = unistring("two.png");
        var.fType = eVK_ArrayUnicode;
        var.fFiller = 0;
        var.uValue.fArray.fCurrent = 0;
        var.uValue.fArray.fNbElements = 2;
        var.uValue.fArray.fData = (PA_Handle)&storage;
    }
    ~TextArray() { for (auto &u : items) PA_DisposeUnistring(&u); }
};

static bool note_created(PA_ObjectRef note) {
    std::string id;
    return note && get_string((Obj *)note, "id", &id) && id == "x-coredata://fake/ICNote/p1";
}

// check_leaks=false: only engine violations (double free, unknown string) count
struct Test { const char *name; std::function<void()> body; bool check_leaks = true; };

static const Test kTests[] = {
    // --- LIVE: Notes Create note (F2 via _getFolder/_createNote/_getNote -> ob_get_a) ---
    {"live_create_note_no_string_leak", [] {
        setup_app();
        PA_ObjectRef note = create_note(NULL, NULL);
        CHECK(note_created(note));
        PA_DisposeObject(note);
        teardown_app();
    }},
    {"live_ob_get_a_released", [] {
        Obj *o = new_object();
        put_string(o, "id", "abc");
        CUTF8String s;
        CHECK(ob_get_a((PA_ObjectRef)o, L"id", &s));
        CHECK(std::string((const char *)s.c_str()) == "abc");
        release(o);
    }},
    // --- LIVE: Notes Create note picture array (F1) ---
    {"live_create_note_picture_attachments_released", [] {
        setup_app();
        {
            PictureArray pictures;
            TextArray names;
            gScripts = 0;
            PA_ObjectRef note = create_note(&pictures.var, &names.var);
            CHECK(note_created(note));
            CHECK(converts() == 2);
            CHECK(gScripts == 2); // addAttachment reached for both pictures
            CHECK(picture_refcount(pictures.items[1]) == 1);
            CHECK(picture_refcount(pictures.items[2]) == 1);
            PA_DisposeObject(note);
        }
        teardown_app();
    }},
    {"live_create_note_failed_convert_released", [] {
        setup_app();
        set_convert_noop(true);
        {
            PictureArray pictures;
            PA_ObjectRef note = create_note(&pictures.var, NULL);
            CHECK(converts() == 2);
            CHECK(picture_refcount(pictures.items[1]) == 1);
            CHECK(picture_refcount(pictures.items[2]) == 1);
            PA_DisposeObject(note);
        }
        teardown_app();
    }},
    // --- DORMANT: built, no caller in this plugin ---
    {"dormant_ob_get_o_borrowed", [] {
        Obj *o = new_object(), *child = new_object();
        put_ref(o, "o", child); release(child);
        PA_ObjectRef got = ob_get_o((PA_ObjectRef)o, L"o");
        CHECK(got == (PA_ObjectRef)child);
        CHECK(refcount(child) == 1); // still owned by the parent only
        release(o);
    }},
    {"dormant_ob_get_c_borrowed", [] {
        Obj *o = new_object(), *coll = new_object(true);
        put_ref(o, "c", coll); release(coll);
        PA_CollectionRef got = ob_get_c((PA_ObjectRef)o, L"c");
        CHECK(got == (PA_CollectionRef)coll);
        CHECK(refcount(coll) == 1);
        release(o);
    }},
    // --- controls: must pass before and after ---
    {"control_create_note_unknown_folder", [] {
        setup_app();
        [gApp.folders.items removeAllObjects]; // no folder ids: ob_get_a is never reached
        PA_ObjectRef note = create_note(NULL, NULL);
        CHECK(note != NULL && !note_created(note));
        PA_DisposeObject(note);
        teardown_app();
    }},
    {"control_create_note_names_borrowed", [] {
        setup_app();
        {
            PictureArray pictures;
            TextArray names;
            PA_ObjectRef note = create_note(&pictures.var, &names.var);
            PA_Unistring u = PA_GetStringInArray(names.var, 1);
            CHECK(u.fLength == 7 && u.fString[0] == 'o');
            PA_DisposeObject(note);
        }
        teardown_app();
    }, false},
    {"control_getters_error_not_cleared", [] {
        Obj *o = new_object(), *child = new_object();
        put_string(o, "s", "x"); put_real(o, "n", 1); put_bool(o, "b", true); put_ref(o, "o", child);
        release(child);
        CUTF8String s;
        fail_next_gets(1); ob_get_a((PA_ObjectRef)o, L"s", &s);
        fail_next_gets(1); CHECK(ob_get_n((PA_ObjectRef)o, L"n") == 0.0);
        fail_next_gets(1); CHECK(!ob_get_b((PA_ObjectRef)o, L"b"));
        fail_next_gets(1); CHECK(ob_get_o((PA_ObjectRef)o, L"o") == nullptr);
        fail_next_gets(1); CHECK(ob_get_c((PA_ObjectRef)o, L"o") == nullptr);
        release(o);
    }},
    {"control_scalar_getters", [] {
        Obj *o = new_object();
        put_real(o, "n", 2.5); put_bool(o, "b", true);
        CHECK(ob_get_n((PA_ObjectRef)o, L"n") == 2.5);
        CHECK(ob_get_b((PA_ObjectRef)o, L"b"));
        CHECK(ob_get_n((PA_ObjectRef)o, L"missing") == 0.0);
        release(o);
    }},
};

int main(int argc, char **argv) {
    install();
    DateFormatter::setup();
    std::set<std::string> expected(argv + 1, argv + argc);
    int unexpected = 0, passed = 0, xfailed = 0;
    for (const Test &t : kTests) {
        reset();
        gFailures.clear();
        size_t strings = live_strings(), handles = live_handles(), objects = live_objects(),
               pictures = live_pictures();
        t.body();
        for (auto &v : violations()) gFailures.push_back("engine: " + v);
        if (!t.check_leaks) strings = live_strings(), handles = live_handles(),
                            objects = live_objects(), pictures = live_pictures();
        if (live_strings() != strings)
            gFailures.push_back("leaked strings: " + std::to_string((long)(live_strings() - strings)));
        if (live_handles() != handles) gFailures.push_back("leaked handles");
        if (live_objects() != objects)
            gFailures.push_back("leaked objects: " + std::to_string((long)(live_objects() - objects)));
        if (live_pictures() != pictures)
            gFailures.push_back("leaked pictures: " + std::to_string((long)(live_pictures() - pictures)));
        bool failed = !gFailures.empty();
        bool want = expected.erase(t.name) > 0;
        const char *verdict = failed ? (want ? "XFAIL" : "FAIL") : (want ? "XPASS" : "PASS");
        printf("%-5s %s\n", verdict, t.name);
        for (auto &f : gFailures) printf("      %s\n", f.c_str());
        if (failed != want) unexpected++;
        else if (failed) xfailed++;
        else passed++;
    }
    for (auto &name : expected) { printf("UNKNOWN expected failure %s\n", name.c_str()); unexpected++; }
    printf("passed=%d xfailed=%d unexpected=%d\n", passed, xfailed, unexpected);
    printf("%s\n", unexpected ? "RESULT: UNEXPECTED" : "RESULT: OK");
    return unexpected ? 1 : 0;
}
