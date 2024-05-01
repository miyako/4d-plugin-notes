//%attributes = {}
$attachment:=Notes Get attachment("x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICAttachment/p23")

SET TEXT TO PASTEBOARD:C523(JSON Stringify:C1217($attachment; *))