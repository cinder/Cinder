import pystache
from pystache.locator import Locator
import os
from pystache.loader import Loader
from pystache.renderer import Renderer
# from pystache.tests.examples.readme import SayHello

TEMPLATE_DIR = os.path.join(os.path.dirname(os.path.realpath(__file__)) + os.sep, "htmlsrc/_templates/")

if __name__ == "__main__":
    # print pystache.render('Hi {{person}}!', {'person': 'Mom'})
    # hello = SayHello()
    # renderer = pystache.Renderer()

    # path = get_data_path('htmlsrc/_templates/say_hello.mustache')
    # locator = Locator()
    # path = locator.find_file('template.txt', ["htmlsrc/_templates/"])
    # print renderer.render(hello)

    # fill side nav object
    side_nav_content = {
        "include": {
            "label": "Surface.h",
            "link": "Surface.html"
        },
        "typedefs": [
            {
                "label": "Surface",
                "link": "Surface.html"
            },
            {
                "label": "Surface8u",
                "link": "Surface.html"
            },
            {
                "label": "Surface8uRef",
                "link": "Surface.html"
            }
        ],
        "classes": [
            {
                "label": "ConstIter",
                "link": "const-iter.html"
            },
            {
                "label": "Iter",
                "link": "iter.html"
            }
        ],
        "related": [
            {
                "label": "Images in Cinder",
                "link": "guide/images/index.html"
            },
            {
                "label": "Path2d Guide",
                "link": "guide/path2d/index.html"
            }
        ]
    }

    # attach side nav to main page

    loader = Loader()
    path = os.path.join(TEMPLATE_DIR, "class_template.mustache")
    renderer = Renderer()
    renderer.search_dirs.append(TEMPLATE_DIR)

    class_obj = {
        'class_type': 'class',
        'class_title': 'Surface T',
        'class_name': 'Surface T',
        'description': "This is a description",
        'side_nav_content': side_nav_content
    }
    # print class_obj.get('side_nav_content').get('include')
    actual = renderer.render_path(path, class_obj)
    print actual