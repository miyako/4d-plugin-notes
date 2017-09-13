# 4d-plugin-notes

Interact with the Notes app on macOS

### Platform

| carbon | cocoa | win32 | win64 |
|:------:|:-----:|:---------:|:---------:|
|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|||

### Version

<img src="https://cloud.githubusercontent.com/assets/1725068/18940649/21945000-8645-11e6-86ed-4a0f800e5a73.png" width="32" height="32" /> <img src="https://cloud.githubusercontent.com/assets/1725068/18940648/2192ddba-8645-11e6-864d-6d5692d55717.png" width="32" height="32" />

## Important 

Please use the ``v11`` (old ``RSR`` architecture) version on v15. ``ARRAY BLOB`` is not passed in compiled mode (new ``manifest.json`` architecture).

## Remarks 

The following methods are not implemented (does nothing) in this version

* ``Notes SET NOTIFICATION (method)``
* ``method:=Notes Get notification``

## Syntax

```
note:=Notes Create note (folder;title;body;attachments)
```

Parameter|Type|Description
------------|------------|----
folder|TEXT|
title|TEXT|
body|TEXT|
attachments|ARRAY PICTURE|
note|TEXT|JSON object

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
folders|ARRAY TEXT|names + JSON object in element ``0``

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
notes|ARRAY TEXT|names + JSON object in element ``0``

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
attachments|ARRAY TEXT|names + JSON object in element ``0``

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
accounts|ARRAY TEXT|names + JSON object in element ``0``

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
folder|TEXT|JSON object

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
note|TEXT|JSON object

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
folder|TEXT|JSON object

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
account|TEXT|

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
