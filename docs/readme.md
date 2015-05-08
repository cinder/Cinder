### To build the docs:
- Install Doxygen from [the doxygen website](http://www.stack.nl/~dimitri/doxygen/)
- Open doxygen/Doxyfile in Doxygen
- In Doxygen, click on the "Run" tab and then click the "Run Doxygen" button. This will generate all of the xml files you need to build the docs.
- To build the html files, run xmlToHtml.py xml/ html/

### To build the stylsheets:
- install homebrew
- install npm
- from the stylsrc dir, run `npm install`
- then run `gulp build`