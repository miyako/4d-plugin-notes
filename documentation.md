## ランゲージ

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
