This repository is for common C/C++ header (.h) files shared across multiple projects.

**Important note on projects structure:**
The Visual Studio projects for my projects are hardcoded to search for dependencies using absolute paths starting from the root of a virtual L: drive. The expected directory structure for this repository is as follows:

```text
L:\
  |-- Include\
```
   
If you want to compile any of my projects without reconfiguring the Visual Studio settings, you can map a local folder to a virtual L: drive using the Windows SUBST command:
Create a directory on your local drive, for example, C:\DEV.
Download the repository Include inside that directory to obtain C:\DEV\Include.
Open the Windows Command Prompt (cmd) and run the following command:
`SUBST L: C:\DEV`
(to remove the virtual drive, use `SUBST L: /d`)

Luca P.
