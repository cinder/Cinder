# Build Docs
The build process requires [`npm`](https://www.npmjs.com/) and [Doxygen](www.doxygen.org/), both can be installed via [homebrew](http://brew.sh/) on OS X.

### Stylesheets
* Install [`npm`](https://www.npmjs.com/) via homebrew
```sh
$ brew install npm
```
* Install stylesheet-building dependencies; from _docs/stylesrc_ run:
```sh
$ npm install
```
* Build stylesheets; from _docs/stylesrc_ run:
```sh
$ ./node_modules/.bin/gulp build
```

### HTML files
* Install [Doxygen](www.doxygen.org/) via homebrew
```sh
$ brew install doxygen
```
* Generate XML class descriptions from source files; from _docs/doxygen_ run:
```sh
$ doxygen Doxyfile
```
* Generate HTML files from XML descriptors; from _docs_ run:
```sh
$ python generateDocs.py
```
* Docs can now be viewed by opening _docs/html/index.html_ in a local browser.
