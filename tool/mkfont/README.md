# Tool to embed font atlases into C files

How to use it:

 - Generate atlas using [https://snowb.org/]
 - Export as `BMFont TEXT`
 - Run `mkfont.py <resulting ZIP> <out header file> <out source file>`

This tool requires `Pillow` to work.

# Symbols current resource file was packed with

To paste into the tool.

```
0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!№;%:?*()_+-=.,/|"'@#$^&{}[]АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдежзийклмнопрстуфхцчшщъыьэюя
```
