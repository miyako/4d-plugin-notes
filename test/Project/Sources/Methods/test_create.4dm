//%attributes = {}
$testFile:=Folder:C1567(fk desktop folder:K87:19).file("test.pdf")

C_OBJECT:C1216($note)

$title:="test pdf"
$body:="body body"
ARRAY BLOB:C1222($attachments;0)
ARRAY TEXT:C222($fileNames;0)

INSERT IN ARRAY:C227($attachments;1)
DOCUMENT TO BLOB:C525($testFile.platformPath;$attachments{1})

APPEND TO ARRAY:C911($fileNames;$testFile.name+$testFile.extension)

$note:=Notes Create note ("x-coredata://2BB27C30-9D8D-4501-8ADD-D16531CA22F5/ICFolder/p11";$title;$body;$attachments;$fileNames)
