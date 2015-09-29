import pystache
from pystache.locator import Locator
import os
from pystache.loader import Loader
from pystache.renderer import Renderer
# from pystache.tests.examples.readme import SayHello

TEMPLATE_DIR = os.path.join(os.path.dirname(os.path.realpath(__file__)) + os.sep, "htmlsrc/_templates/")
CLASS_TEMPLATE_DIR = os.path.join(TEMPLATE_DIR, "class/")

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
        "typedefs": {
            "list": [
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
            "length": 3
        },
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

    # loader = Loader()
    # path = os.path.join(TEMPLATE_DIR, "class-template.mustache")
    # renderer = Renderer()
    # renderer.search_dirs.append(TEMPLATE_DIR)
    #
    class_obj = {
        'class_type': 'class',
        'class_title': 'Surface T',
        'name': 'Surface T',
        'description': "This is a description",
        'side_nav_content': side_nav_content
    }
    # print class_obj.get('side_nav_content').get('include')
    # actual = renderer.render_path(path, class_obj)

    # test_obj = {
    #     'title': 'A fucking title'
    # }
    #
    # title_partial = {'page-title': 'Some cinder crap | {{title}}'}
    # renderer = Renderer(partials=title_partial)
    # renderer.search_dirs.append(TEMPLATE_DIR)
    # path = os.path.join(TEMPLATE_DIR, "master-template.mustache")
    # actual = renderer.render_path(path, test_obj)
    # print actual



    # step 1: render content in template
    content_renderer = Renderer()
    path = os.path.join(CLASS_TEMPLATE_DIR, "main-content-template.mustache")
    content_renderer.search_dirs.append(TEMPLATE_DIR)
    rendered_content = content_renderer.render_path(path, class_obj)


    # step 2: place rendered content into main template
    # - should have the following custom partials:
    #   - page title (define in object for page templates)
    #   - page content (rendered page content)
    #   - any other common partials that may lie outside the basic content area

    test_obj = {
        'title': class_obj["name"]
    }

    loader = Loader()
    # template = loader.read("title")
    title_partial = loader.load_name(os.path.join(CLASS_TEMPLATE_DIR, "title"))
    partials = {'page-title': title_partial, 'main-content': rendered_content}
    renderer = Renderer(partials=partials)

    path = os.path.join(TEMPLATE_DIR, "master-template.mustache")
    actual = renderer.render_path(path, test_obj)
    print actual