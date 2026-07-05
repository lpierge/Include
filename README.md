This repository is for common C/C++ header (.h) files shared across multiple projects.

**Important note on projects structure:**

The Visual Studio projects I use are hardcoded to search for dependencies using absolute paths, starting from the root of a virtual L: drive. The expected directory structure for this repository is:

```text
L:\
  |-- Include\
```
   
If you want to compile any of my projects without reconfiguring the Visual Studio settings, you can map a local folder to a virtual L: drive using the Windows SUBST command.

Create a directory on your local drive, for example, C:\DEV.

Download the Include repository inside that directory to create C:\DEV\Include.

Open the Windows Command Prompt (cmd) and run the following command:

`SUBST L: C:\DEV`

Luca P.
