# 4d-plugin-notes

Interact with the Notes app on macOS

**Bug**: It is no longer possible to add an attachment in macOS Catalina.

see:

https://hookproductivity.com/help2/integration/hook-and-macos-10-15-catalina-notes-app/
https://forum.latenightsw.com/t/notes-app-scripting-dictionary-defective-on-catalina/2136

``applescript
tell application "Notes"
  note id "x-coredata://2BB27C30-9D8D-4501-8ADD-D16531CA22F5/ICNote/p51"
  (*
  returns:
  show id "x-coredata://2BB27C30-9D8D-4501-8ADD-D16531CA22F5/ICNote/p51" of application "Notes"
  it is not possible to use this as a specifier; bummer! (error 1700)
  *)
  
end tell
```

### Platform

| carbon | cocoa | win32 | win64 |
|:------:|:-----:|:---------:|:---------:|
||<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|||

### Version

<img src="https://cloud.githubusercontent.com/assets/1725068/18940649/21945000-8645-11e6-86ed-4a0f800e5a73.png" width="32" height="32" /> <img src="https://cloud.githubusercontent.com/assets/1725068/18940648/2192ddba-8645-11e6-864d-6d5692d55717.png" width="32" height="32" /> <img src="https://user-images.githubusercontent.com/1725068/41266195-ddf767b2-6e30-11e8-9d6b-2adf6a9f57a5.png" width="32" height="32" />

[before-mojave](https://github.com/miyako/4d-plugin-notes/tree/before-mojave) branch uses 10.13 SDK

### Releases

[3.0](https://github.com/miyako/4d-plugin-notes/releases/tag/3.0)

## Important

This version uses [``AEDeterminePermissionToAutomateTarget``](https://developer.apple.com/documentation/coreservices/3025784-aedeterminepermissiontoautomatet?language=objc) from the 10.14 SDK

c.f. https://www.felix-schwarz.org/blog/2018/08/new-apple-event-apis-in-macos-mojave

This tool may help sign your application

[4d-utility-build-application](https://github.com/miyako/4d-utility-build-application)

If access has been previously denied by user, the application will not ask again.

The use must manually enable it.

Alternatively, you can clear all permissions for Automation.

```
tccutil reset AppleEvents
```

before

<img width="668" alt="2019-03-05 16 01 49" src="https://user-images.githubusercontent.com/1725068/53786763-084a2d00-3f60-11e9-8f6a-b75c88ddb5e9.png">

after

<img width="668" alt="2019-03-05 16 02 40" src="https://user-images.githubusercontent.com/1725068/53786792-1f891a80-3f60-11e9-8911-fab9d6c6466a.png">

The main application must have the following keys in ``Info.plist``

**Key**: Privacy - AppleEvents Sending Usage Description (``NSAppleEventsUsageDescription``)  

**Value**: Reason to access private data

If the entry does not exist, plugin calls will silently fail.

<img width="765" alt="2019-03-05 16 06 08" src="https://user-images.githubusercontent.com/1725068/53787121-206e7c00-3f61-11e9-8801-9080083e5d4f.png">

If the app is signed, you obviously have to sign it again after editing ``Info.plist``.  

If the key exists, the system will ask the user for permission on the first attempt.

<img width="420" alt="2019-03-05 16 09 11" src="https://user-images.githubusercontent.com/1725068/53787180-4d229380-3f61-11e9-8d38-d046e9107d55.png">

You will also need to codesign the built app (with the ``--deep`` option, since some frameworks are unsigned).

To sign, you may have to ``xattr -rc`` and first clean all files inside the app.

## Architecture

This plugin uses several different ways to access Notes. 

### ScriptingBridge

The file ``Notes.h`` was generated by ``sdef``.

```
sdef /Applications/Notes.app | sdp -fh --basename Notes
```

Most functions ask the Notes app to complete the task by sending ``obj-c`` messaged over [ScriptingBridge](https://developer.apple.com/documentation/scriptingbridge).

### NSAppleScript

Adding attachments seems impossible via ScriptingBridge. So [NSAppleScript](https://developer.apple.com/documentation/foundation/nsapplescript) is used instead.

### SQLite

The path of attachment files is likewise not exposed via AppleScript. So direct access to the backend SQLite database is performed.

* Example of connecting to the Notes backend SQLite database.

```
sqlite3 /Users/miyako/Library/Group\ Containers/group.com.apple.notes/NoteStore.sqlite
```

---

## Syntax

```
note:=Notes Create note (folder;title;body;attachments{;$names})
```

Parameter|Type|Description
------------|------------|----
folder|TEXT|
title|TEXT|
body|TEXT|
attachments|ARRAY PICTURE| or ARRAY BLOB
names|ARRAY TEXT|
note|OBJECT|

Properties of ``note``:

* ``id``:``string``
* ``name``:``string``
* ``body``:``string``
* ``creationDate``:``string``
* ``creationLocalDate``:``string``
* ``modificationDate``:``string``
* ``modificationLocalDate``:``string``

```
Notes GET FOLDERS (folders)
```

Parameter|Type|Description
------------|------------|----
folders|COLLECTION|

Properties of ``folders``:

* ``folders``:``array`` of ``folder``

Properties of ``folder``:

* ``id``:``string``
* ``name``:``string``
* ``notes``:``array`` of ``string`` (id)
* ``folders``:``array`` of ``folder`` (non-recursive)

```
Notes GET NOTES (notes)
```

Parameter|Type|Description
------------|------------|----
notes|COLLECTION|

Properties of ``notes``:

* ``notes``:``array`` of ``note``

Properties of ``note``:

* ``id``:``string``
* ``name``:``string``
* ``attachments``:``array`` of ``string`` (id)

```
Notes GET ATTACHMENTS (attachments)
```

Parameter|Type|Description
------------|------------|----
attachments|COLLECTION|

Properties of ``attachments``:

* ``attachments``:``array`` of ``attachment``

Properties of ``attachment``:

* ``id``:``string``
* ``name``:``string``
* ``note``:``string`` (id)

```
Notes GET ACCOUNTS (accounts)
```

Parameter|Type|Description
------------|------------|----
accounts|COLLECTION|

Properties of ``accounts``:

* ``accounts``:``array`` of ``account``

Properties of ``account``:

* ``id``:``string``
* ``name``:``string``
* ``folders``:``array`` of ``folder`` (non-recursive)

```
folder:=Notes Create folder (parent;name)
```

Parameter|Type|Description
------------|------------|----
parent|TEXT|
name|TEXT|
folder|OBJECT|

Properties of ``folder``:

* ``id``:``string``
* ``name``:``string``

```
path:=Notes Get attachment (id)
```

Parameter|Type|Description
------------|------------|----
attachment|TEXT|
path|TEXT|

**Note**: This methods call ``sqlite3`` to resolve the attachment path.

```
note:=Notes Get note (id)
```

Parameter|Type|Description
------------|------------|----
id|TEXT|
note|OBJECT|

Properties of ``note``:

* ``id``:``string``
* ``name``:``string``
* ``body``:``string``
* ``folder``:``string`` (id)
* ``creationDate``:``string``
* ``creationLocalDate``:``string``
* ``modificationDate``:``string``
* ``modificationLocalDate``:``string``
* ``attachments``:``array`` of ``string`` (id)

```
folder:=Notes Get folder (id)
```

Parameter|Type|Description
------------|------------|----
id|TEXT|
folder|OBJECT| 

Properties of ``folder``:

* ``id``:``string``
* ``name``:``string``
* ``folders``:``array`` of ``folder``(recursive) 
* ``notes``:``array`` of ``note`` (top level only)

Properties of ``note``:

* ``id``:``string``
* ``name``:``string``

```
account:=Notes Get account (id)
```

Parameter|Type|Description
------------|------------|----
id|TEXT|
account|OBJECT|

Properties of ``account``:

* ``id``:``string``
* ``name``:``string``
* ``folders``:``array`` of ``folder``(recursive) 

---

```
Notes SET NOTIFICATION (method)
```

Parameter|Type|Description
------------|------------|----
method|TEXT|

```
method:=Notes Get notification
```

Parameter|Type|Description
------------|------------|----
method|TEXT|

## Remarks 

The following methods are not implemented (does nothing) in this version

* ``Notes SET NOTIFICATION (method)``
* ``method:=Notes Get notification``
