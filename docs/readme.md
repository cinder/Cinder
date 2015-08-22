# Build Docs
The build process requires Python 2.7 and [Doxygen](www.doxygen.org/) 1.8.10, which can be downloaded from [the doxygen website](http://www.stack.nl/~dimitri/doxygen/) or installed via [homebrew](http://brew.sh/) on OS X.


### Stylesheets
Compiled stylesheets are included in _htmlsrc/\_assets/css_. [Stylus](http://learnboost.github.io/stylus/) is the CSS preprocessor of choice, which can be installed via [`npm`](https://www.npmjs.com/).

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


For more details on Cinder Docs read the [Building Docs](htmlsrc/guides/docs/building_docs.html) and [Documenting Cinder](htmlsrc/guides/docs/documenting_cinder.html) Guides.
