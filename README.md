# Nova

Presentation editor for TrangorgeOS. WYSIWYG editing with cosmic dark UI.

## Features
- WYSIWYG editing directly on the slide (like OnlyOffice)
- Editable title, subtitle, body directly in the canvas
- Drag & drop images from desktop
- Resize images with 8 handles (Shift for aspect ratio)
- Image cropping (Crop X/Y/W/H in 0..1)
- Rich formatting: bold, italic, underline, H1-H3, lists
- Theme switcher (Cosmic / Dark / Light)
- Slide thumbnails in sidebar
- Fullscreen presentation (F5, arrows to navigate)
- Export to PDF
- Save/load as .nova (JSON)
- Cosmic dark UI with magenta/green accents

## Build

    mkdir build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build . -j$(nproc)
    ./nova

## Install (from release)

    tar -xzf nova-linux-v0.1.0.tar.gz
    cd nova-linux-v0.1.0
    ./install.sh
    nova

## Requirements
- Arch: sudo pacman -S qt6-base qt6-webengine
- Debian/Ubuntu: sudo apt install qt6-base-dev qt6-webengine-dev

## License
GPL-3.0
