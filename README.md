# Vici

Vici, the one and only worst version control system known to man.

## What/Who is it made for?

Myself.

## What language is it written in?

English.

## Why did you choose this project to create?

I was bored, and I figured that the best way to spend my time was to create a copy of Git.

## Why C++?

I like the filesystem and fstream headers. And it was good practice for Object Oriented Programming.

## Use

You'll probably get errors if you delete any files or folders, so probably don't touch those.

As of now, there are no built-in ways of adding, removing, or editing files. So I guess you'll have to just do it on your own. ¯\\_(ツ)_/¯

I probably will add some basic commands to add and remove files, so don't worry.

Windows exclusive features will be mentioned in `vici.bat`, along with an editable motd!

Oh, and the help command will show you all the commands, but here they are anyways:

---

## Commands

| Command                       | Description                                        |
|------------------------------|----------------------------------------------------|
| `new <repo_name>`            | Create a new repository                            |
| `cr <repo_name>`             | Change/select repository                           |
| `commit [msg]`               | Commit current state with optional message         |
| `log`                        | List all commits for current repo                  |
| `checkout <version>`         | Restore repo to a previous version                 |
| `delrepo <repo_name>`        | Delete a repository and its versions               |
| `delver <version>`           | Delete a specific version from current repo        |
| `status`                     | Show current repo and latest commit                |
| `ls`                         | List files in the current repo's `.curr` folder    |
| `!!`                         | Execute the last command again                     |
| `help`                       | Show this help message                             |
| `exit`                       | Exit the program                                   |
| `cls`                        | Clear the screen                                   |

---

## `.viciignore` Syntax

| Pattern                      | Effect                                                                              |
|-----------------------------|-------------------------------------------------------------------------------------|
| `<file_name>`               | Ignores files with the same name in the root directory                             |
| `<dir_name/>`               | Ignores folders with the same name in the root directory                           |
| `<dir_name/*>`              | Ignores all files in that folder, but not the folder itself                        |
| `<dir_name/file_name>`      | Ignores specific files or paths, subdirectories will also work.
