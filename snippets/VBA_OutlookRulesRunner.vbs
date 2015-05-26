'Change the following three constants as desired
Const SHOW_PROGRESS = True
Const INCLUDE_SUBFOLDERS = False
Const UNREAD_MSGS_ONLY = 2
Dim olkApp, olkSes, olkFolder, olkRule
Set olkApp = CreateObject("Outlook.Application")
Set olkSes = olkApp.GetNamespace("MAPI")
'Change Outlook on hte next line to the name of your mail profile'
olkSes.Logon "gengp1"
Set olkFolder = olkSes.GetDefaultFolder(6)

For Each Folder In olkFolder.Folders
    If Folder.Name = "Tomorrow" Then
        Set targetFolder = Folder
    End If
Next
   
For Each olkRule In olkSes.DefaultStore.GetRules
olkRule.Execute SHOW_PROGRESS, targetFolder, INCLUDE_SUBFOLDERS
Next
Set olkRule = Nothing
Set olkFolder = Nothing
olkSes.Logoff
Set olkSes = Nothing
Set olkApp = Nothing

