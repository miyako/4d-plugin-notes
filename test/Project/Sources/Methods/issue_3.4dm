//%attributes = {}
ARRAY PICTURE:C279($images; 1)
READ PICTURE FILE:C678(Get 4D folder:C485(Current resources folder:K5:16)+"test.png"; $image)
$images{1}:=$image
ARRAY TEXT:C222($names; 1)
$names{1}:="test.png"
$note:=Notes Create note("x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICFolder/p12"; "title"; "body"; $images; $names)
SET TEXT TO PASTEBOARD:C523(JSON Stringify:C1217($note; *))
