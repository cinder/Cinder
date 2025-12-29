### Cinder 0.9.4dev: [libcinder.org](http://libcinder.org)

<p align="center">
  <img src="https://libcinder.org/docs/_assets/images/cinder_logo.svg" alt="Cinder Logo" width="256" height="auto"/>
</p>

**Cinder is a peer-reviewed, free, open source C++ library for creative coding.**

Please note that Cinder depends on a few submodules. The simplest way to clone it is:<br />
```
git clone --recursive https://github.com/cinder/Cinder.git
```

You might also prefer one of our [pre-packaged downloads](https://libcinder.org/download).

### Building Cinder

Debug:
```
cmake -B build
cmake --build build -j
```

Release:
```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j
```

This works on macOS, Windows, and Linux.

Cinder [guides](https://libcinder.org/docs/branch/master/guides/index.html) and [reference documentation](https://libcinder.org/docs/branch/master/reference/index.html) are available on the website.

Cinder supports macOS, Windows, and Linux. It requires a C++20 compiler: Xcode 11.3.1+ on Mac, Visual C++ 2022+ on Windows.

Cinder is released under the [Modified BSD License](COPYING). Please visit [our website](https://libcinder.org) for more information.

Also be sure to check out the [User Forum](http://discourse.libcinder.org).
