## ランゲージ

```4d
accounts:=Notes GET ACCOUNTS
```

Parameter|Type|Description
------------|------------|----
accounts|COLLECTION|collection of `account`

メモに登録されているアカウントの情報をコレクション型で返します。

例

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

Properties of ``account``:

Parameter|Type|Description
------------|------------|----
name|TEXT|
id|TEXT|`x-coredata`
folders|COLLECTION|collection of `x-coredata`

```4d
folders:=Notes GET FOLDERS
```

メモに登録されているフォルダーの情報をコレクション型で返します。

Parameter|Type|Description
------------|------------|----
folders|COLLECTION|collection of `folder`

例

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
	},
	{
		"name": "メモ",
		"id": "x-coredata://D219C617-A827-4FFA-B8F1-1ADC065277D6/EWSFolder/p4",
		"notes": [],
		"folders": []
	}
]
```

Properties of `folder`:

Parameter|Type|Description
------------|------------|----
name|TEXT|
id|TEXT|`x-coredata`
notes|COLLECTION|collection of `x-coredata`
folders|COLLECTION|collection of `x-coredata`

```4d
notes:=Notes GET NOTES
```
メモに登録されているメモの情報をコレクション型で返します。

Parameter|Type|Description
------------|------------|----
notes|COLLECTION|collection of `note`

例

```json
[
	{
		"name": "あああ",
		"id": "x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICNote/p22",
		"attachments": []
	},
	{
		"name": "いいい",
		"id": "x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICNote/p21",
		"attachments": []
	},
	{
		"name": "ううう",
		"id": "x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICNote/p15",
		"attachments": []
	}
]
```

Properties of ``note``:

Parameter|Type|Description
------------|------------|----
name|TEXT|
id|TEXT|`x-coredata`
attachments|COLLECTION|collection of `x-coredata`

```4d
attachments:=Notes GET ATTACHMENTS
```

メモに登録されている添付ファイルの情報をコレクション型で返します。

Parameter|Type|Description
------------|------------|----
attachments|COLLECTION|collection of `attachment`

```json
[
	{
		"name": "image.png",
		"id": "x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICAttachment/p23",
		"note": "x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICNote/p15"
	}
]
```

Properties of ``attachment``:

Parameter|Type|Description
------------|------------|----
name|TEXT|
id|TEXT|`x-coredata`
note|COLLECTION|`x-coredata`

```4d
note:=Notes Get note (id)
```

メモに登録されているメモの情報をオブジェクト型で返します。

Parameter|Type|Description
------------|------------|----
id|TEXT|
note|OBJECT|

例

```json
{
	"id": "x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICNote/p15",
	"folder": "x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICFolder/p10",
	"name": "ううう",
	"body": "<div>ううう</div>\n",
	"creationDate": "2019-08-07T10:42:31Z",
	"modificationDate": "2019-08-07T19:50:27Z",
	"creationLocalDate": "2019-08-07T19:42:31",
	"modificationLocalDate": "2019-08-08T04:50:27",
	"attachments": []
}
```

Properties of ``note``:

Parameter|Type|Description
------------|------------|----
id|TEXT|`x-coredata`
folder|TEXT|`x-coredata`
name|TEXT|
body|TEXT|
creationDate|TEXT|
modificationDate|TEXT|
creationLocalDate|TEXT|
modificationLocalDate|TEXT|
attachments|COLLECTION|collection of `x-coredata`










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
