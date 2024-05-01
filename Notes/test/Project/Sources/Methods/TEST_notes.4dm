//%attributes = {}
$accounts:=Notes GET ACCOUNTS
$folders:=Notes GET FOLDERS
$notes:=Notes GET NOTES
$attachments:=Notes GET ATTACHMENTS

$note:=Notes Get note("x-coredata://5E8616AB-4594-4644-8F88-CE7880A4E33D/ICNote/p15")
$account:=Notes Get account("x-coredata://D219C617-A827-4FFA-B8F1-1ADC065277D6/EWSAccount/p2")
$folder:=Notes Get folder("x-coredata://D219C617-A827-4FFA-B8F1-1ADC065277D6/EWSFolder/p4")

//$folder:=Notes Create folder("x-coredata://D219C617-A827-4FFA-B8F1-1ADC065277D6/EWSFolder/p4"; "title4")
