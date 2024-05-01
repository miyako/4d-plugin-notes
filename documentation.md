# ランゲージ

## リスト

* Notes GET ACCOUNTS
* Notes GET FOLDERS
* Notes GET NOTES
* Notes GET ATTACHMENTS

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
folders|COLLECTION|collection of `folder` (non-recursive)

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
note|COLLECTION|collection of `x-coredata`

## オブジェクト

* Notes Get account
* Notes Get folder
* Notes Get note
* Notes Get attachment

```4d
account:=Notes Get account (id)
```

メモに登録されているアカウントの情報をオブジェクト型で返します。

Parameter|Type|Description
------------|------------|----
id|TEXT|`x-coredata`
account|OBJECT|

例

```json
{
	"id": "x-coredata://D219C617-A827-4FFA-B8F1-1ADC065277D6/EWSAccount/p2",
	"name": "Exchange",
	"folders": [
		{
			"name": "メモ",
			"id": "x-coredata://D219C617-A827-4FFA-B8F1-1ADC065277D6/EWSFolder/p4",
			"folders": []
		}
	]
}
```

Properties of ``account``:

Parameter|Type|Description
------------|------------|----
name|TEXT|
id|TEXT|
folders|COLLECTION|collection of `folder` (recursive)

```4d
folder:=Notes Get folder (id)
```

メモに登録されているフォルダーの情報をオブジェクト型で返します。

Parameter|Type|Description
------------|------------|----
id|TEXT|
folder|OBJECT| 

例

```json
{
	"id": "x-coredata://D219C617-A827-4FFA-B8F1-1ADC065277D6/EWSFolder/p4",
	"name": "メモ",
	"folders": [],
	"notes": []
}
```

Properties of ``folder``:

Parameter|Type|Description
------------|------------|----
id|TEXT|
name|TEXT|
folders|COLLETION|collection of `folder` (recursive) 
notes|COLLETION|collection of `note` (top level only)

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

```4d
path:=Notes Get attachment (id)
```

メモに登録されている添付ファイルの情報をオブジェクト型で返します。

Parameter|Type|Description
------------|------------|----
attachment|TEXT|
path|TEXT|

🐞添付ファイルの取得に問題があります。

## 作成

```4d
note:=Notes Create note (folder;title;body;attachments{;$names})
```

オブジェクト型からメモを作成します。

Parameter|Type|Description
------------|------------|----
folder|TEXT|`x-coredata`
title|TEXT|
body|TEXT|
attachments|ARRAY PICTURE or ARRAY BLOB|添付ファイル
names|ARRAY TEXT|添付ファイル名
note|OBJECT|

例

```json

```

Properties of ``note``:

Parameter|Type|Description
------------|------------|----
id|TEXT|`x-coredata`
name|TEXT|
body|TEXT|
creationDate|TEXT|
modificationDate|TEXT|
creationLocalDate|TEXT|
modificationLocalDate|TEXT|

🐞添付ファイルの追加に問題があります。

```4d
folder:=Notes Create folder (parent;name)
```

オブジェクト型からフォルダーを作成します。

Parameter|Type|Description
------------|------------|----
parent|TEXT|`x-coredata`
name|TEXT|フォルダー名
folder|OBJECT|`folder`

例

```json
{
	"id": "x-coredata://D219C617-A827-4FFA-B8F1-1ADC065277D6/EWSFolder/p16",
	"name": "title4"
}
```

Properties of ``folder``:

Parameter|Type|Description
------------|------------|----
id|TEXT|`x-coredata`
name|TEXT|フォルダー名