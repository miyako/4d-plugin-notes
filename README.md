# 4d-plugin-notes

Interact with the Notes app on macOS

### Platform

| carbon | cocoa | win32 | win64 |
|:------:|:-----:|:---------:|:---------:|
|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|||

### Version

<img src="https://cloud.githubusercontent.com/assets/1725068/18940649/21945000-8645-11e6-86ed-4a0f800e5a73.png" width="32" height="32" /> <img src="https://cloud.githubusercontent.com/assets/1725068/18940648/2192ddba-8645-11e6-864d-6d5692d55717.png" width="32" height="32" />

## Syntax

```
note:=Notes Create note (folder;title;body;attachments;names)
```

Parameter|Type|Description
------------|------------|----
folder|TEXT|
title|TEXT|
body|TEXT|
attachments|ARRAY BLOB|
names|ARRAY TEXT|

```
Notes GET FOLDERS (folders)
```

Parameter|Type|Description
------------|------------|----
folders|ARRAY TEXT|

```
Notes GET NOTES (notes)
```

Parameter|Type|Description
------------|------------|----
notes|ARRAY TEXT|

```
Notes GET ATTACHMENTS (attachments)
```

Parameter|Type|Description
------------|------------|----
attachments|ARRAY TEXT|

```
Notes GET ACCOUNTS (accounts)
```

Parameter|Type|Description
------------|------------|----
accounts|ARRAY TEXT|

```
folder:=Notes Create folder (account;name)
```

Parameter|Type|Description
------------|------------|----
account|TEXT|
name|TEXT|
folder|TEXT|

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

```
json:=Notes Get attachment (attachment)
```

Parameter|Type|Description
------------|------------|----
attachment|TEXT|
json|TEXT|

```
json:=Notes Get note (note)
```

Parameter|Type|Description
------------|------------|----
note|TEXT|
json|TEXT|

```
json:=Notes Get folder (folder)
```

Parameter|Type|Description
------------|------------|----
folder|TEXT|
json|TEXT|

```
json:=Notes Get account (account)
```

Parameter|Type|Description
------------|------------|----
account|TEXT|
json|TEXT|
