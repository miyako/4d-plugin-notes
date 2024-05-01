//%attributes = {}
$accounts:=Notes GET ACCOUNTS
$folders:=Notes GET FOLDERS
$notes:=Notes GET NOTES
$attachments:=Notes GET ATTACHMENTS

$note:=Notes Get note("x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICNote/p15")
$account:=Notes Get account("x-coredata://D219C617-A827-4FFA-B8F1-1ADC065277D6/EWSAccount/p2")
$folder:=Notes Get folder("x-coredata://D219C617-A827-4FFA-B8F1-1ADC065277D6/EWSFolder/p4")

$folder:=Notes Create folder("x-coredata://D219C617-A827-4FFA-B8F1-1ADC065277D6/EWSFolder/p4"; "title4")
SET TEXT TO PASTEBOARD:C523(JSON Stringify:C1217($folder; *))




ARRAY PICTURE:C279($images; 1)
READ PICTURE FILE:C678(Get 4D folder:C485(Current resources folder:K5:16)+"test.png"; $image)
$images{1}:=$image
ARRAY TEXT:C222($names; 1)
$names{1}:="test"
$note:=Notes Create note("x-coredata://D219C617-A827-4FFA-B8F1-1ADC065277D6/EWSFolder/p4"; "title"; "body"; $images; $names)
SET TEXT TO PASTEBOARD:C523(JSON Stringify:C1217($note; *))

$attachment:=Notes Get attachment("x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICAttachment/p23")

$folders:=$folders.query("name == :1"; "Notes")










