# light-screenshot

A small utility for taking screenshots.

Among other benefits, it can 'freeze' the screen instantly, allowing for better area selection experience.

## Compilation Requirements
```
make clang libX11 libpng
```

## Runtime Requirements
```
maim xclip xdotool
```

## Compilation
```sh
# run:
sudo make install
```

## Help

### main application

```sh
Usage: light-screenshot [flags]

Flags:
    --help             - display help

        CAPTURING
    -i, --instant      - freeze the screen before selecting, makes sense only with '-a select',
                         DEFAULT = false
    -a, --area <area>  - specify the area to capture,
                         DEFAULT = all
    -h, --highlight    - add highlight color when selecting an area,
                         DEFAULT = false

        OUTPUT
    -s, --save         - save the screenshot to a file, return the path to the file,
                         DEFAULT = true
    -c, --clipboard    - save the screenshot to the clipboard,
                         DEFAULT = true
    -p, --path <path>  - specify the full screenshot path (including the filename),
                         PNG REQUIRED,
                         DEFAULT = $HOME/Pictures/screenshots/DATE-TIME.png
    --helper-attrs     - specify the attributes for the helper window,
                         see 'light-screenshot-helper --help' for more info,
                         DEFAULT = -Xn lscreen-helper -Xc lscreen-helper

<area> can be:
    window  - capture the active window
    select  - select an area to capture
    all     - capture the whole screen

```

### Helper application
It doesn't really make sense to use this separately, but just in case.

```sh
Usage: <png-data> | light-screenshot-helper <flags>

Shows an image from stdin above everything

Flags:
    --help       - display help

        XORG PROPERTIES
    -Xn <name>   - window name,  *
    -Xc <class>  - window class, *

```
