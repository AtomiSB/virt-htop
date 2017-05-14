# virt-htop 
Text-based user interface VM manager powered by libvirt.


## Requirements
* ncurses
* libvirt

## Build
```
mkdir build
cd build
cmake ../ -DCMAKE_BUILD_TYPE=Release
make
```

## Run
```
./virt-htop --connect qemu:///system
```

## License
![GPLv3](http://www.gnu.org/graphics/gplv3-127x51.png)

Copyright (C) 2017 Atomi

Author: Sebastian Bialobrzecki <sbb@openmailbox.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
