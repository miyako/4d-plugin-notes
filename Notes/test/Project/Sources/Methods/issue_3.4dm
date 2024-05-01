//%attributes = {}
ARRAY PICTURE:C279($images; 1)
READ PICTURE FILE:C678(Get 4D folder:C485(Current resources folder:K5:16)+"test.png"; $image)
$images{1}:=$image
ARRAY TEXT:C222($names; 1)
$names{1}:="test"
$note:=Notes Create note("x-coredata://D219C617-A827-4FFA-B8F1-1ADC065277D6/EWSFolder/p4"; "title"; "body"; $images; $names)
SET TEXT TO PASTEBOARD:C523(JSON Stringify:C1217($note; *))
