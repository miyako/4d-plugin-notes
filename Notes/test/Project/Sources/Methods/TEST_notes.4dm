//%attributes = {}
$accounts:=Notes GET ACCOUNTS

$folders:=Notes GET FOLDERS
$folders:=$folders.query("name == :1"; "Notes")

$folder:=Notes Create folder($folders[0].id; "title4")

$notes:=Notes GET NOTES
$attachments:=Notes GET ATTACHMENTS
$account:=Notes Get account("x-coredata://2BB27C30-9D8D-4501-8ADD-D16531CA22F5/ICAccount/p10")
$folder:=Notes Get folder("x-coredata://2BB27C30-9D8D-4501-8ADD-D16531CA22F5/ICFolder/p11")

ARRAY PICTURE:C279($images; 1)
READ PICTURE FILE:C678(Get 4D folder:C485(Current resources folder:K5:16)+"test.png"; $image)
$images{1}:=$image
ARRAY TEXT:C222($names; 1)
$names{1}:="test"

$note:=Notes Create note("x-coredata://2BB27C30-9D8D-4501-8ADD-D16531CA22F5/ICFolder/p11"; "title"; "body"; $images; $names)

$note:=Notes Get note("x-coredata://2BB27C30-9D8D-4501-8ADD-D16531CA22F5/ICNote/p13")
$attachment:=Notes Get attachment("x-coredata://2BB27C30-9D8D-4501-8ADD-D16531CA22F5/ICAttachment/p15")




