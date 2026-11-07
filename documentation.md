# Notes plugin — command reference

The Notes plugin lets 4D read and create content in the macOS **Notes** app. It talks to Notes through Apple Events: most commands use ScriptingBridge, and adding attachments, reading an attachment's file location, and showing a note use `NSAppleScript`. Accounts, folders, notes and attachments are identified by the `x-coredata://…` ID strings that Notes itself uses. Lists come back as a `Collection`, single items as an `Object`; dates inside them are `Text` (ISO 8601), not 4D dates.

| Command | Returns | Purpose |
|---|---|---|
| [Notes GET ACCOUNTS](#notes-get-accounts) | `Collection` | List every account, with its folder IDs |
| [Notes GET FOLDERS](#notes-get-folders) | `Collection` | List every folder, with its note and subfolder IDs |
| [Notes GET NOTES](#notes-get-notes) | `Collection` | List every note, with its attachment IDs |
| [Notes GET ATTACHMENTS](#notes-get-attachments) | `Collection` | List every attachment, with the ID of its note |
| [Notes Get account](#notes-get-account) | `Object` | One account and its folder tree |
| [Notes Get folder](#notes-get-folder) | `Object` | One folder, its subfolder tree and its notes |
| [Notes Get note](#notes-get-note) | `Object` | One note, including its HTML body and dates |
| [Notes Get attachment](#notes-get-attachment) | `Object` | One attachment, including the location of its file |
| [Notes Create folder](#notes-create-folder) | `Object` | Create a folder in an account or another folder |
| [Notes Create note](#notes-create-note) | `Object` | Create a note, optionally with picture or file attachments |
| [Notes SHOW](#notes-show) | — | Show a note in the Notes app |
| [Notes SET NOTIFICATION](#notes-set-notification) | — | Not implemented (no effect) |
| [Notes Get notification](#notes-get-notification) | `Text` | Not implemented (returns empty text) |

**Platforms:** macOS (Intel and Apple silicon).

---

## Requirements & platform notes

- **macOS only.** The commands are implemented with ScriptingBridge and AppleScript, which exist only on macOS. The bundle includes a prebuilt Windows 4DX, but this source has no Windows implementation; behaviour on Windows hasn't been verified, so don't call the commands there.
- **Automation permission.** macOS asks the user whether 4D may control Notes. The plugin triggers that request when it is loaded (at 4D startup). If the user refuses, every command silently returns an empty collection or object. To change the decision, the user enables 4D under *System Settings → Privacy & Security → Automation*, or resets all decisions with `tccutil reset AppleEvents`.
- **`NSAppleEventsUsageDescription`.** The application that hosts the plugin (4D, or your built and signed application) **must** have this key in its `Info.plist`. Without it, macOS never shows the permission prompt and the commands silently fail. See the [README](README.md) for details and screenshots.
- **Notes is driven, not read directly.** Every call sends Apple Events to the Notes app, which is much slower than reading local data. The `Get` commands fetch the IDs of every item of their kind before looking up the one you asked for, so they get slower as the library grows.
- **Failure is silent.** No command raises a 4D error. An unknown ID, a missing permission or a failure inside Notes gives an empty object `{}` or an empty collection `[]`. Test the result (for example with `OB Is empty` or `.length`) before using it.
- **IDs.** Every ID is a `Text` of the form `x-coredata://<store UUID>/<entity>/p<number>`, for example `x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICNote/p15`. Get them from the `GET` commands; they belong to one Mac's Notes database.
- **Dates.** Note and attachment objects carry each date twice, as `Text`: `creationDate`/`modificationDate` in UTC (`2019-08-07T10:42:31Z`) and `creationLocalDate`/`modificationLocalDate` in the Mac's local time zone, without a suffix (`2019-08-07T19:42:31`).
- **Processes.** The manifest marks every command thread-safe, but `NSAppleScript`, which [Notes Create note](#notes-create-note) (attachments), [Notes Get attachment](#notes-get-attachment) and [Notes SHOW](#notes-show) use, is documented by Apple as main-thread-only. Calling these from a preemptive process hasn't been verified; prefer a cooperative process.

---

## Notes GET ACCOUNTS

### Syntax

```4d
accounts:=Notes GET ACCOUNTS
```

| Parameter | Type | Description |
|---|---|---|
| Result | `Collection` | One object per account |

### Description

Returns every account configured in Notes (iCloud, "On My Mac", Exchange, …). Each element has these properties:

| Property | Type | Description |
|---|---|---|
| `name` | `Text` | Account name |
| `id` | `Text` | Account ID |
| `folders` | `Collection` | IDs (`Text`) of the account's folders, as Notes lists them for the account |

To get the folder names and the whole folder tree of one account, pass its `id` to [Notes Get account](#notes-get-account).

### Example

From the plugin's test method (`TEST_notes.4dm`):

```4d
$accounts:=Notes GET ACCOUNTS
```

Example result:

```json
[
	{
		"name": "Exchange",
		"id": "x-coredata://D219C617-A827-4FFA-B8F1-1ADC065277D6/EWSAccount/p2",
		"folders": [
			"x-coredata://D219C617-A827-4FFA-B8F1-1ADC065277D6/EWSFolder/p4"
		]
	},
	{
		"name": "iCloud",
		"id": "x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICAccount/p11",
		"folders": [
			"x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICFolder/p12",
			"x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICFolder/p10"
		]
	}
]
```

Find an account by name and read its folder tree:

```4d
$accounts:=Notes GET ACCOUNTS
$icloud:=$accounts.query("name = :1"; "iCloud")
If ($icloud.length#0)
	$account:=Notes Get account($icloud[0].id)
End if
```

---

## Notes GET FOLDERS

### Syntax

```4d
folders:=Notes GET FOLDERS
```

| Parameter | Type | Description |
|---|---|---|
| Result | `Collection` | One object per folder |

### Description

Returns the folders the Notes app lists at the application level, across all accounts. Each element has these properties:

| Property | Type | Description |
|---|---|---|
| `name` | `Text` | Folder name |
| `id` | `Text` | Folder ID |
| `notes` | `Collection` | IDs (`Text`) of the notes in this folder |
| `folders` | `Collection` | IDs (`Text`) of the folder's direct subfolders |

Folder names are not unique: each account has its own default folder, and several accounts can have folders with the same name. Use the `id`.

### Example

From the plugin's test method (`TEST_notes.4dm`):

```4d
$folders:=Notes GET FOLDERS
```

Example result:

```json
[
	{
		"name": "Notes",
		"id": "x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICFolder/p12",
		"notes": [],
		"folders": []
	},
	{
		"name": "Recently Deleted",
		"id": "x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICFolder/p10",
		"notes": [
			"x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICNote/p22",
			"x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICNote/p21",
			"x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICNote/p15"
		],
		"folders": []
	}
]
```

Count the notes in each folder:

```4d
$folders:=Notes GET FOLDERS
For each ($folder; $folders)
	ALERT($folder.name+": "+String($folder.notes.length))
End for each
```

---

## Notes GET NOTES

### Syntax

```4d
notes:=Notes GET NOTES
```

| Parameter | Type | Description |
|---|---|---|
| Result | `Collection` | One object per note |

### Description

Returns every note in every account, including notes in *Recently Deleted*. Each element has these properties:

| Property | Type | Description |
|---|---|---|
| `name` | `Text` | Note title |
| `id` | `Text` | Note ID |
| `attachments` | `Collection` | IDs (`Text`) of the note's attachments |

The body and dates are not included; use [Notes Get note](#notes-get-note) for them.

### Example

From the plugin's test method (`TEST_notes.4dm`):

```4d
$notes:=Notes GET NOTES
```

Example result:

```json
[
	{
		"name": "Shopping",
		"id": "x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICNote/p15",
		"attachments": []
	}
]
```

Read the body of every note that has attachments:

```4d
$notes:=Notes GET NOTES
For each ($item; $notes)
	If ($item.attachments.length#0)
		$note:=Notes Get note($item.id)
		// $note.body is HTML
	End if
End for each
```

---

## Notes GET ATTACHMENTS

### Syntax

```4d
attachments:=Notes GET ATTACHMENTS
```

| Parameter | Type | Description |
|---|---|---|
| Result | `Collection` | One object per attachment |

### Description

Returns every attachment in every note. Each element has these properties:

| Property | Type | Description |
|---|---|---|
| `name` | `Text` | Attachment name (usually the file name) |
| `id` | `Text` | Attachment ID |
| `note` | `Text` | ID of the note that contains the attachment |

### Example

From the plugin's test method (`TEST_notes.4dm`):

```4d
$attachments:=Notes GET ATTACHMENTS
```

Example result:

```json
[
	{
		"name": "image.png",
		"id": "x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICAttachment/p23",
		"note": "x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICNote/p15"
	}
]
```

---

## Notes Get account

### Syntax

```4d
account:=Notes Get account(id)
```

| Parameter | Type | Description |
|---|---|---|
| `id` | `Text` | Account ID, from [Notes GET ACCOUNTS](#notes-get-accounts) |
| Result | `Object` | The account, or an empty object if no account has this ID |

### Description

Returns the account with its complete folder tree:

| Property | Type | Description |
|---|---|---|
| `id` | `Text` | Account ID |
| `name` | `Text` | Account name |
| `folders` | `Collection` | Folder objects, each with `name`, `id` and `folders` (its own subfolders, recursively) |

These folder objects don't list notes; use [Notes Get folder](#notes-get-folder) for that.

### Example

From the plugin's test method (`TEST_notes.4dm`):

```4d
$account:=Notes Get account("x-coredata://D219C617-A827-4FFA-B8F1-1ADC065277D6/EWSAccount/p2")
```

Example result:

```json
{
	"id": "x-coredata://D219C617-A827-4FFA-B8F1-1ADC065277D6/EWSAccount/p2",
	"name": "Exchange",
	"folders": [
		{
			"name": "Notes",
			"id": "x-coredata://D219C617-A827-4FFA-B8F1-1ADC065277D6/EWSFolder/p4",
			"folders": []
		}
	]
}
```

---

## Notes Get folder

### Syntax

```4d
folder:=Notes Get folder(id)
```

| Parameter | Type | Description |
|---|---|---|
| `id` | `Text` | Folder ID, from [Notes GET FOLDERS](#notes-get-folders) or [Notes Get account](#notes-get-account) |
| Result | `Object` | The folder, or an empty object if no folder has this ID |

### Description

Returns the folder, its subfolder tree and the notes it contains directly:

| Property | Type | Description |
|---|---|---|
| `id` | `Text` | Folder ID |
| `name` | `Text` | Folder name |
| `folders` | `Collection` | Subfolder objects, each with `name`, `id` and `folders` (recursively) |
| `notes` | `Collection` | An object with `name` and `id` for each note directly in this folder (notes in subfolders are not included) |

### Example

From the plugin's test method (`TEST_notes.4dm`):

```4d
$folder:=Notes Get folder("x-coredata://D219C617-A827-4FFA-B8F1-1ADC065277D6/EWSFolder/p4")
```

Example result:

```json
{
	"id": "x-coredata://D219C617-A827-4FFA-B8F1-1ADC065277D6/EWSFolder/p4",
	"name": "Notes",
	"folders": [],
	"notes": [
		{
			"name": "Shopping",
			"id": "x-coredata://D219C617-A827-4FFA-B8F1-1ADC065277D6/EWSNote/p4"
		}
	]
}
```

---

## Notes Get note

### Syntax

```4d
note:=Notes Get note(id)
```

| Parameter | Type | Description |
|---|---|---|
| `id` | `Text` | Note ID, from [Notes GET NOTES](#notes-get-notes) or [Notes Get folder](#notes-get-folder) |
| Result | `Object` | The note, or an empty object if no note has this ID |

### Description

| Property | Type | Description |
|---|---|---|
| `id` | `Text` | Note ID |
| `folder` | `Text` | ID of the folder that contains the note |
| `name` | `Text` | Note title |
| `body` | `Text` | Note content as HTML, as Notes returns it (for example `<div>Shopping</div>`) |
| `creationDate` | `Text` | Creation date, UTC |
| `modificationDate` | `Text` | Last modification date, UTC |
| `creationLocalDate` | `Text` | Creation date, local time |
| `modificationLocalDate` | `Text` | Last modification date, local time |
| `attachments` | `Collection` | IDs (`Text`) of the note's attachments |

### Example

From the plugin's test method (`TEST_notes.4dm`):

```4d
$note:=Notes Get note("x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICNote/p15")
```

Example result:

```json
{
	"id": "x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICNote/p15",
	"folder": "x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICFolder/p10",
	"name": "Shopping",
	"body": "<div>Shopping</div>\n",
	"creationDate": "2019-08-07T10:42:31Z",
	"modificationDate": "2019-08-07T19:50:27Z",
	"creationLocalDate": "2019-08-07T19:42:31",
	"modificationLocalDate": "2019-08-08T04:50:27",
	"attachments": []
}
```

---

## Notes Get attachment

### Syntax

```4d
attachment:=Notes Get attachment(id)
```

| Parameter | Type | Description |
|---|---|---|
| `id` | `Text` | Attachment ID, from [Notes GET ATTACHMENTS](#notes-get-attachments) or [Notes Get note](#notes-get-note) |
| Result | `Object` | The attachment, or an empty object if no attachment has this ID |

### Description

| Property | Type | Description |
|---|---|---|
| `id` | `Text` | Attachment ID |
| `name` | `Text` | Attachment name |
| `note` | `Text` | ID of the note that contains the attachment |
| `contents` | `Text` | Location of the attachment's file inside the Notes data folder, as an HFS path (`Macintosh HD:Users:…:410.png`) |
| `contentIdentifier` | `Text` | Content ID (`cid:…`) |
| `creationDate` | `Text` | Creation date, UTC |
| `modificationDate` | `Text` | Last modification date, UTC |
| `creationLocalDate` | `Text` | Creation date, local time |
| `modificationLocalDate` | `Text` | Last modification date, local time |

`contents` is read with AppleScript (`contents of attachment … as Unicode text`). When Notes doesn't return a file for the attachment, the property may be missing or empty (inferred from the code; not tested for every attachment kind). The file lives in Notes' own data folder (`~/Library/Group Containers/group.com.apple.notes/`): copy it rather than modifying it.

### Example

From the plugin's test method (`issue_2.4dm`):

```4d
$attachment:=Notes Get attachment("x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICAttachment/p23")

SET TEXT TO PASTEBOARD:C523(JSON Stringify:C1217($attachment; *))
```

Example result:

```json
{
	"id": "x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICAttachment/p23",
	"contents": "Macintosh HD:Users:miyako:Library:Group Containers:group.com.apple.notes:Accounts:F67EF39F-73DE-4FB8-840C-6034A72B5272:Media:C955A294-CA7D-43BC-8C88-92033CFF1702:1_B8A3CD7C-9BA6-4322-A17E-C5AF8E90F3B7:410.png",
	"note": "x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICNote/p22",
	"creationDate": "2024-05-01T04:56:52Z",
	"modificationDate": "2024-05-01T04:56:52Z",
	"creationLocalDate": "2024-05-01T13:56:52",
	"modificationLocalDate": "2024-05-01T13:56:52",
	"contentIdentifier": "cid:6ADF0ACD-9233-4072-8549-AE958FD76D4A@icloud.apple.com",
	"name": "410.png"
}
```

Turn `contents` into a 4D `File` object (on macOS the HFS path is the platform path):

```4d
$attachment:=Notes Get attachment($id)
If (String($attachment.contents)#"")
	$file:=File($attachment.contents; fk platform path)
End if
```

---

## Notes Create folder

### Syntax

```4d
folder:=Notes Create folder(parent; name)
```

| Parameter | Type | Description |
|---|---|---|
| `parent` | `Text` | ID of a folder (the new folder becomes its subfolder) or of an account (the new folder is created at the top level of that account) |
| `name` | `Text` | Name of the new folder; **must** not be empty |
| Result | `Object` | The new folder, or an empty object on failure |

### Description

`parent` is first looked up as a folder ID, then as an account ID. If it matches neither, or `name` is empty, nothing is created and the result is an empty object. On success the result has two properties:

| Property | Type | Description |
|---|---|---|
| `id` | `Text` | ID of the new folder |
| `name` | `Text` | Its name, as Notes stored it |

### Example

From the plugin's test method (`TEST_notes.4dm`, where the line is commented out):

```4d
//$folder:=Notes Create folder("x-coredata://D219C617-A827-4FFA-B8F1-1ADC065277D6/EWSFolder/p4"; "title4")
```

Example result:

```json
{
	"id": "x-coredata://D219C617-A827-4FFA-B8F1-1ADC065277D6/EWSFolder/p16",
	"name": "title4"
}
```

Create a folder at the top level of the first account:

```4d
$accounts:=Notes GET ACCOUNTS
If ($accounts.length#0)
	$folder:=Notes Create folder($accounts[0].id; "Invoices")
	If (OB Is empty($folder))
		ALERT("The folder could not be created.")
	End if
End if
```

---

## Notes Create note

### Syntax

```4d
note:=Notes Create note(folder; title; body; attachments; names)
```

| Parameter | Type | Description |
|---|---|---|
| `folder` | `Text` | ID of the folder that receives the note (an account ID is not accepted) |
| `title` | `Text` | Note title |
| `body` | `Text` | Note content; Notes treats it as HTML |
| `attachments` | `Array Picture` or `Array Blob` | Files to attach; pass an empty array for none |
| `names` | `Array Text` | File name for each attachment, by element number; pass an empty array to use generated names |
| Result | `Object` | The new note, or an empty object on failure |

### Description

The note is created at the top of `folder`. Then each element of `attachments` is written to a temporary file, attached with AppleScript, and the temporary file is deleted.

- **Pictures** (`Array Picture`) are converted to PNG first. Without a name, the file is called `<generated UUID>.png`.
- **BLOBs** (`Array Blob`) are attached unchanged. Without a name, the file is called `<generated UUID>` with no extension, so pass a name with the right extension (`report.pdf`); Notes uses it to decide how to show the file.
- Element `0` of the arrays is processed too; empty pictures and empty BLOBs are skipped.
- `names{i}` names `attachments{i}`. An empty name, a missing element, or a `names` array that isn't `Array Text` gives a generated name. (Up to version 4.3.0 the element check was inverted: in-range names were ignored, and only an element at or past the end of `names` was read. Fixed after 4.3.0.)

The result describes the note as created:

| Property | Type | Description |
|---|---|---|
| `id` | `Text` | ID of the new note |
| `name` | `Text` | The `title` you passed |
| `body` | `Text` | The `body` you passed |
| `creationDate` | `Text` | Creation date, UTC |
| `modificationDate` | `Text` | Last modification date, UTC |
| `creationLocalDate` | `Text` | Creation date, local time |
| `modificationLocalDate` | `Text` | Last modification date, local time |

If `folder` isn't a known folder ID, or Notes doesn't create the note, the result is an empty object and no attachment is added. An attachment that fails is skipped without an error, and the note is still returned. To check what was attached, call [Notes Get note](#notes-get-note) with the returned `id` and look at `attachments`.

Pass both arrays even when you have no attachments; the sample code always does, and calling the command without them hasn't been verified.

### Example

From the plugin's test method (`issue_3.4dm`), attaching a picture with an explicit name:

```4d
ARRAY PICTURE:C279($images; 1)
READ PICTURE FILE:C678(Get 4D folder:C485(Current resources folder:K5:16)+"test.png"; $image)
$images{1}:=$image
ARRAY TEXT:C222($names; 1)
$names{1}:="test.png"
$note:=Notes Create note("x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICFolder/p12"; "title"; "body"; $images; $names)
SET TEXT TO PASTEBOARD:C523(JSON Stringify:C1217($note; *))
```

From the plugin's test method (`test_create.4dm`), attaching a PDF file as a BLOB:

```4d
$testFile:=Folder:C1567(fk desktop folder:K87:19).file("test.pdf")

C_OBJECT:C1216($note)

$title:="test pdf"
$body:="body body"
ARRAY BLOB:C1222($attachments;0)
ARRAY TEXT:C222($fileNames;0)

INSERT IN ARRAY:C227($attachments;1)
DOCUMENT TO BLOB:C525($testFile.platformPath;$attachments{1})

APPEND TO ARRAY:C911($fileNames;$testFile.name+$testFile.extension)

$note:=Notes Create note ("x-coredata://2BB27C30-9D8D-4501-8ADD-D16531CA22F5/ICFolder/p11";$title;$body;$attachments;$fileNames)
```

Example result:

```json
{
	"id": "x-coredata://D219C617-A827-4FFA-B8F1-1ADC065277D6/EWSNote/p4",
	"name": "title",
	"body": "body",
	"creationDate": "2024-05-01T06:05:55Z",
	"modificationDate": "2024-05-01T06:05:55Z",
	"creationLocalDate": "2024-05-01T15:05:55",
	"modificationLocalDate": "2024-05-01T15:05:55"
}
```

A note without attachments, with HTML formatting:

```4d
ARRAY PICTURE($none; 0)
ARRAY TEXT($noNames; 0)
$note:=Notes Create note($folderId; "Meeting"; "<div><b>Agenda</b></div><div>1. Budget</div>"; $none; $noNames)
```

---

## Notes SHOW

### Syntax

```4d
Notes SHOW(note)
```

| Parameter | Type | Description |
|---|---|---|
| `note` | `Text` | A note ID, or text to search for |
| Result | — | None |

### Description

- If `note` starts with `x-coredata://`, Notes is asked to show the note with that ID.
- Otherwise Notes is asked to show every note whose plain text contains `note`, one after the other.

If no note matches, nothing happens and no error is raised.

### Example

From the plugin's test method (`test_show.4dm`):

```4d
Notes SHOW("x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICNote/p22")
```

Show the notes that mention an order number:

```4d
Notes SHOW("Order 10423")
```

---

## Notes SET NOTIFICATION

### Syntax

```4d
Notes SET NOTIFICATION(method)
```

| Parameter | Type | Description |
|---|---|---|
| `method` | `Text` | **Ignored** |
| Result | — | None |

### Description

**Not implemented.** The command is declared but does nothing.

---

## Notes Get notification

### Syntax

```4d
method:=Notes Get notification
```

| Parameter | Type | Description |
|---|---|---|
| Result | `Text` | Always empty |

### Description

**Not implemented.** The command is declared but never sets a result, so you get an empty text.

---

## Error handling & troubleshooting

- **Everything comes back empty.** 4D isn't allowed to control Notes. Check *System Settings → Privacy & Security → Automation*, and that the host application has `NSAppleEventsUsageDescription` in its `Info.plist` (see [Requirements & platform notes](#requirements--platform-notes)).
- **No permission prompt appears.** The user already decided once, or the `Info.plist` key is missing. Run `tccutil reset AppleEvents` in Terminal and restart 4D to be asked again.
- **An empty object from a `Get` or `Create` command.** The ID doesn't exist (IDs belong to one Mac's Notes database), the `parent` of [Notes Create folder](#notes-create-folder) is neither a folder nor an account, its `name` was empty, or the `folder` of [Notes Create note](#notes-create-note) is an account ID instead of a folder ID.
- **A note was created but an attachment is missing.** Adding the attachment failed inside Notes, and the plugin doesn't report it. Check with [Notes Get note](#notes-get-note). The [README](README.md) records that adding attachments stopped working on macOS 10.15 Catalina; whether that still applies to your macOS version hasn't been verified here.
- **A BLOB attachment shows up as an unknown file.** It was attached without a name, so the file has no extension. Pass a name such as `invoice.pdf` in `names`.
- **Dates are off by some hours.** `creationDate` and `modificationDate` are UTC (`Z` suffix); use `creationLocalDate`/`modificationLocalDate` for the Mac's local time.
- **Calls are slow.** Every call is a round trip to the Notes app, and the `Get` commands first list every item of their kind. Keep the IDs and objects you already have instead of calling a `Get` command in a tight loop.
- **The commands do nothing on Windows.** The plugin is macOS-only.

---

## Quick reference

```4d
$accounts:=Notes GET ACCOUNTS
$folders:=Notes GET FOLDERS
$notes:=Notes GET NOTES
$attachments:=Notes GET ATTACHMENTS

$account:=Notes Get account($accounts[0].id)
$folder:=Notes Get folder($folders[0].id)
$note:=Notes Get note($notes[0].id)
$attachment:=Notes Get attachment($attachments[0].id)

$newFolder:=Notes Create folder($account.id; "New folder")

ARRAY PICTURE($images; 0)
ARRAY TEXT($names; 0)
$newNote:=Notes Create note($newFolder.id; "Title"; "<div>Body</div>"; $images; $names)

Notes SHOW($newNote.id)
```
