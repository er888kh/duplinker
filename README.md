# duplinker
Avoid duplicates in a directory by linking all duplicates together!
# building
To build this project do the following: 

```bash
meson build
cd build
ninja
```

The only dependency is cryptopp, and can be installed as bellow on fedora:

```bash
sudo dnf install cryptopp-devel # will be around 2MB
```