# Vici

Vici, the one and only worst version control system known to man.

## Who is it made/designed for?

Myself.

## What language is it written in?

English.

## Why did you choose this project to create?

I was bored, and I figured that the best way to spend my time was to create a copy of Git.

## Why C++?

I like the filesystem and fstream headers. And it was good practice for Object Oriented Programming.

## Use

You must add the folder containing viciw.exe and vici.bat to your PATH variables.

You'll probaly get errors if you delete any files or folders, so probaly don't touch those.

As of now, there are no built in ways of adding, removing, or editing files. So I guess you'll have to just do it on your own. ¯\\_(ツ)_/¯

I probaly will add some basic commands to add and remove files, so don't worry.

Oh, and the help command will show you all the commands but here they are anyways:

## Commands

new <repo_name>         Create a new repository

cr <repo_name>          Change/select repository\n

commit [msg]            Commit current state with optional message\n

log                     List all commits for current repo\n

checkout <version>      Restore repo to a previous version\n

delrepo <repo_name>     Delete a repository and its versions\n

delver <version>        Delete a specific version from current repo\n

status                  Show current repo and latest commit\n

ls                      List files in the current repo's .curr folder\n

help                    Show this help message\n

exit                    Exit the program\n
