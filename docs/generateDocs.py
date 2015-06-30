#! /usr/bin/python
import sys
import codecs
import copy
import re
import xml.etree.ElementTree as Et
import json
import os
import shutil
import stat
import urlparse
from bs4 import BeautifulSoup, Tag, NavigableString

BASE_PATH = os.path.dirname(os.path.realpath(__file__)) + os.sep
XML_SOURCE_PATH = BASE_PATH + 'xml' + os.sep
DOXYGEN_HTML_PATH = BASE_PATH + 'html' + os.sep
HTML_SOURCE_PATH = BASE_PATH + 'htmlsrc' + os.sep
TEMPLATE_PATH = BASE_PATH + 'htmlsrc' + os.sep + "_templates" + os.sep
PARENT_DIR = BASE_PATH.split('/docs')[0]
GITHUB_PATH = "http://github.com/cinder/Cinder/tree/master"

# state vars
PROCESSED_HTML_DIR = False

# convert docygen markup to html markup
tagDictionary = {
    "linebreak": "br",
    "emphasis": "em",
    "ref": "a",
    "computeroutput": "code",
    "includes": "span",
    "simplesect": "dl",
    "para": "p"
}

titleDictionary = {
    "namespace": "Namespace",
    "class": "Class",
    "struct": "Struct"
}

headerDictionary = {
    "namespace": "Namespace Reference",
    "class": "Class Reference",
    "class_template": "<T> Class Template Reference",
    "struct": "Struct Reference",
    "struct_template": "<T> Struct Template Reference"
}

public_function_header = {
    "class": "Public Member Functions",
    "interface": "Instance Methods"
}

public_static_function_header = {
    "class": "Static Public Member Functions",
    "interface": "Class Methods"
}

g_tag_xml = None
g_symbolMap = None
g_currentFile = None
g_search_index = None


# mapping for the tag file with helper functions
class SymbolMap(object):
    def __init__(self):
        self.namespaces = {}
        self.classes = {}
        self.typedefs = {}
        self.functions = {}
        self.files = {}
        self.enums = {}

    class Class(object):
        def __init__(self, class_tree):

            self.name = class_tree.find('name').text
            self.path = class_tree.find('filename').text
            self.base = class_tree.find('base').text if class_tree.find('base') is not None else ""
            self.is_template = True if class_tree.find('templarg') else False

            self.functionList = []
            self.relatedLinks = []
            self.type_defs = []

            # Path the the description prefix
            self.prefixPath = None

            # list of tags to be added to the search index
            self.tags = []
            self.tags.append(strip_compound_name(self.name))

        def add_related_link(self, a_link):
            self.relatedLinks.append(a_link)

        def define_prefix(self, path):
            self.prefixPath = path

        def add_type_def(self, type_def_obj):
            self.type_defs.append(type_def_obj)
            # add typedef string to search tags
            self.tags.append(strip_compound_name(type_def_obj.name))

    class Namespace(object):
        def __init__(self, name, file_name):
            self.name = name
            self.path = file_name
            self.functionList = []

    class Typedef(object):
        def __init__(self, name, type_def, path):
            self.name = name
            self.type = type_def
            self.path = path
            self.sharedFrom = None

    class Function(object):
        def __init__(self, name, base, path):
            self.name = name
            self.base = base
            self.path = path

    class File(object):
        def __init__(self, name, path, typedefs):
            self.name = name
            self.path = path
            self.typedefs = typedefs
            self.githubPath = None

            rel_path_arr = self.path.split(PARENT_DIR)
            if len(rel_path_arr) > 1:
                self.githubPath = GITHUB_PATH + self.path.split(PARENT_DIR)[-1]
            else:
                print "NO PATH FOR " + name

    class Enum(object):
        def __init__(self, name, path):
            self.name = name
            self.path = path

    # searches the symbolMap for a given symbol, prepending cinder:: if not found as-is
    def find_class(self, name):

        # replace leading ci:: with cinder:: instead
        searchname = str(name)
        if searchname.find("ci::") == 0:
            searchname = searchname.replace("ci::", "cinder::")

        # same key as name
        if searchname in self.classes:
            return self.classes[searchname]
        # key with "cinder::" prepended
        elif ("cinder::" + searchname) in self.classes:
            return self.classes["cinder::" + searchname]

        else:
            # iterate through all of the classes with namespace "cinder::" and test against just class name
            for className in self.classes:
                # if className has "cinder::" and namespace depth > 1, test against name
                if className.find("cinder") == 0 and len(className.split("::")) > 1:
                    testname = className.split("cinder::")[1].rsplit("::", 1)[-1]
                    if testname == searchname:
                        return self.classes[className]

            # check to see if the name is a typedef that is a shared_ptr to another class
            typedef = self.find_typedef(searchname)
            if typedef is not None:
                if typedef.sharedFrom is not None:
                    return typedef.sharedFrom
                else:
                    return typedef

            # check to see if parent is a typedef
            searchname_parts = searchname.split("::")
            if len(searchname_parts) > 1:

                parent_name = searchname_parts[-2]
                typedef = self.find_typedef(parent_name)

                # if parent is typedef and has a sharedFrom property, find_class against that name
                if typedef and typedef.sharedFrom:
                    return self.find_class("::".join([typedef.sharedFrom.name, searchname_parts[-1]]))

            return None

    def find_namespace(self, name):

        searchname = str(name)
        if searchname.find("ci::") == 0:
            searchname = searchname.replace("ci::", "cinder::")

        # same key as name
        if searchname in self.namespaces.keys():
            return self.namespaces.get(searchname)

        # key with "cinder::" prepended
        elif ("cinder::" + searchname) in self.namespaces.keys():
            return self.namespaces["cinder::" + searchname]

        return None

    def get_ordered_namespaces(self):
        """
        create an array of strings that include all of the namespaces and return
        :return: A list of namespace objects in alphabetical order
        """
        namespaces = []
        for nsKey in self.namespaces:
            ns = self.namespaces[nsKey]
            namespaces.append(ns)

        # sort by lowercased name
        namespaces = sorted(namespaces, key=lambda s: s.name.lower())

        return namespaces

    def get_whitelisted_namespaces(self):
        """
        create a list of namespace objects that consist of only whitelisted namespaces
        :return: An alphabetized list of namespace objects
        """
        namespaces = []
        for nsKey in self.namespaces:
            ns = self.namespaces[nsKey]
            if ns.name.startswith("ci") or ns.name.startswith("glm"):
                namespaces.append(ns)

        # sort by lowercased name
        namespaces = sorted(namespaces, key=lambda s: s.name.lower())
        return namespaces

    def find_typedef(self, name):

        searchname = str(name)
        if searchname.find("ci::") == 0:
            searchname = searchname.replace("ci::", "cinder::")

        # same key as name
        if searchname in self.typedefs.keys():
            return self.typedefs[searchname]

            # key with "cinder::" prepended
        elif ("cinder::" + searchname) in self.typedefs:
            return self.typedefs["cinder::" + searchname]

        else:
            # iterate through all of the classes with namespace "cinder::" and test against just class name
            for typedef in self.typedefs:
                if typedef.find("cinder") == 0 and len(typedef.split("::")) > 1:
                    testname = typedef.split("cinder::")[1].rsplit("::", 1)[-1]
                    if testname == searchname:
                        return self.typedefs[typedef]
        return None

    def find_function(self, name, class_obj=None):

        fn_obj = None
        # find function name without namespace and parenthesis
        fn_name = strip_compound_name(name.split('(')[0])

        if class_obj is None:
            # find parent class first
            class_parts = name.split("::")
            class_name = "::".join(class_parts[:-1])

            class_obj = g_symbolMap.find_class(class_name)

        # iterate through class functions
        if class_obj:
            for fn in class_obj.functionList:
                if fn.name == fn_name:
                    fn_obj = fn

        if not fn_obj:
            fn_obj = self.functions.get(fn_name)

        return fn_obj

    def find_file(self, name):
        return self.files.get(name)

    def find_file_typedefs(self, name):
        return self.find_file(name).typedefs

    def find_enum(self, name):
        searchname = str(name)
        if searchname.find("ci::") == 0:
            searchname = searchname.replace("ci::", "cinder::")

        # enum_obj = None
        # if ns_obj is None:
        #     # find parent class first
        #     ns_parts = name.split("::")
        #     class_name = "::".join(ns_parts[:-1])
        #     class_obj = g_symbolMap.find_class(class_name)

        # same key as name
        if searchname in self.enums.keys():
            return self.enums.get(searchname)

        # key with "cinder::" prepended
        elif ("cinder::" + searchname) in self.enums:
            return self.enums.get("cinder::" + searchname)

    def get_class_ancestors(self, name):
        result = []
        existingclass = self.find_class(name)
        while existingclass and existingclass.base:
            result.insert(0, existingclass)
            existingclass = self.find_class(existingclass.base)

        if result:
            return result
        else:
            return []

    def get_class_descendants(self, name):
        result = []
        for aClass in self.classes:
            if self.classes[aClass].base == name:
                result.append(self.classes[aClass])

        if result:
            return result
        else:
            return []

    # def get_link_for_class(self, className):
    #     """ Get the link for the definition of a class.
    #         It may include namespace or not.
    #     """
    #     # strip down to the name of the class (without namespace)
    #     return ""

    def get_ordered_class_list(self):
        """ create an array of classes that include all of the classes and return
            the array in alphabetical order """
        classes = []
        for class_key in self.classes:
            class_obj = self.classes[class_key]
            classes.append(class_obj)

        # sort by lowercased name
        return sorted(classes, key=lambda s: s.name.lower())

    def find_classes_in_namespace(self, namespace, recursive=True):
        ns_classes = []
        for class_key in self.classes:
            if recursive:
                if class_key.startswith(namespace) > 0:
                    class_obj = self.find_class(class_key)
                    ns_classes.append(class_obj)
            else:
                class_pre = get_namespace(class_key)
                if namespace == class_pre:
                    class_obj = self.find_class(class_key)
                    ns_classes.append(class_obj)
        return ns_classes


class FileData(object):
    def __init__(self, tree, bs4):

        self.tree = tree  # xml file that describes the page
        self.bs4 = bs4  # html file of the actual page

        self.contentsEl = None
        self.sideNavEl = None
        self.descriptionEl = None
        self.descriptionProseEl = None
        self.sideEl = None

        # fill compound name (with namespace if present)
        self.compoundName = str(find_compound_name(tree))

        # stripped name (w/o namespace)
        name_parts = self.compoundName.rsplit("cinder::", 1)
        if len(name_parts) > 1:
            self.name = name_parts[1]  # without "cinder::"
        else:
            self.name = name_parts[0]

        # kind of file that we are parsing (class, namespace, etc)
        self.kind = find_file_kind(tree)
        self.kind_explicit = find_file_kind_explicit(tree)

        # find any common html elements and populate common elements
        self.parse_template()

    def parse_template(self):
        self.contentsEl = self.bs4.find("div", "contents")
        if self.kind == "class" or self.kind == "struct":
            self.descriptionEl = self.bs4.find(id="description")
            self.descriptionProseEl = self.descriptionEl.find(id="description-prose")
            self.sideEl = self.descriptionEl.find("div", "side")
        self.sideNavEl = self.bs4.find(id="side-nav")

        self.populate_title()
        self.populate_header()

    def populate_title(self):
        # find tags
        head_tag = self.bs4.head
        title_tag = self.bs4.new_tag("title")

        # populate tags
        title_tag.append(titleDictionary[self.kind] + " | " + self.name)
        head_tag.insert(0, title_tag)

    def populate_header(self):
        header_tag = self.bs4.find(id="page-title")
        header_tag.string = self.compoundName + " " + headerDictionary[self.kind_explicit]

    def append_to_side_el(self, el):
        self.sideEl.append(el)

        # def set_category(self):
        #     print "SET CATEGORY"


def find_compound_name(tree):
    for compound_def in tree.iter("compounddef"):
        for compound_name in compound_def.iter("compoundname"):
            return compound_name.text


def find_file_kind(tree):
    kind = tree.find(r"compounddef").attrib['kind']
    # print "FIND FILE KIND: " + kind
    return kind


def find_file_kind_explicit(tree):
    """
    Find a more specific file kind based on the name of the file.
    So instead of just class as the tag file specifies its kind as,
    it might also be a struct or interface.
    :param tree:
    :return: string of kind
    """
    obj_id = tree.find(r"compounddef").attrib['id']
    if obj_id.startswith("struct"):
        if obj_id.endswith("_t"):
            return "struct_template"
        else:
            return "struct"

    elif obj_id.startswith("interface"):
        return "interface"

    elif obj_id.startswith("namespace"):
        return "namespace"

    else:
        if obj_id.endswith("_t"):
            return "class_template"
        else:
            return "class"


def find_compound_name_stripped(tree):
    compound_name = find_compound_name(tree)
    name = strip_compound_name(compound_name)
    return name


def strip_compound_name(full_string):
    ns_parts = full_string.split("::")
    name = "".join(ns_parts[-1])
    return name

def get_namespace(full_string):
    ns_parts = full_string.split("::")
    prefix = "::".join(ns_parts[:-1])  # parent namespace up to last ::
    return prefix

def add_class_to_tag(tag, class_name):
    tag["class"] = tag.get("class", []) + [class_name]


def gen_anchor_tag(bs4, anchor_name):
    anchor = gen_tag(bs4, "a")
    anchor["name"] = anchor_name
    return anchor


def gen_tag(bs4, tag_type, classes=None, contents=None):
    """ Generates a new html element and optionally adds classes and content

    Args:
        bs4:        beautiful soup
        tagType:    html tag/element (p, a, em, etc)
        classes:    array of strings that you want as classes for the element
        contents:   any content that you want to populate your tag with, if known
    """

    new_tag = bs4.new_tag(tag_type)

    if classes:
        for c in classes:
            add_class_to_tag(new_tag, c)

    if contents:
        new_tag.append(contents)

    return new_tag


def gen_link_tag(bs4, text, link):
    link_tag = gen_tag(bs4, "a", [], text)
    define_link_tag(link_tag, {"href": link})
    return link_tag


def define_link_tag(tag, attrib):
    ref_id = None
    href = None

    if "refid" in attrib:
        ref_id = attrib["refid"]
        href = ref_id + ".html"

    if "kindref" in attrib:
        kind = attrib["kindref"]

        if kind == "member":
            str_list = ref_id.rsplit("_1", 1)
            href = str_list[0] + ".html#" + str_list[1]

    if "linkid" in attrib:
        href = "../../include/cinder/" + attrib["linkid"]

    if "href" in attrib:
        href = attrib["href"]

    if "typedef" in attrib:
        data = attrib["typedef"]
        file_name = data.find("anchorfile").text
        anchor = data.find("anchor").text
        href = file_name + "#" + anchor

    if href is None:
        print "\t *** WARNING DEFINING LINK TAG: " + str(tag)
        # raise
    else:
        tag["href"] = href


def markup_function(bs4, fn_xml, parent, is_constructor):
    """ Mark up a function using the function definition

    create new line
    left side = return type
    right side = function name
    under right side = definition

    ---------------------------------------------------
    | returnType    | function( param1, param2, etc ) |
    ---------------------------------------------------
    |               | description                     |
    ---------------------------------------------------
    """

    fn_id = fn_xml.attrib["id"].split("_1")[-1]
    li = gen_tag(bs4, "li", ["row"])
    li.append(gen_anchor_tag(bs4, fn_id))

    # wrapper
    function_div = gen_tag(bs4, "div", ["functionDef"])
    li.append(function_div)

    # left side / return type
    if not is_constructor:
        return_div = gen_tag(bs4, "div", ["returnCol columns large-3"])
        iterate_markup(bs4, fn_xml.find(r"type"), return_div)
        function_div.append(return_div)

    # right side (function name and description)
    definition_col = gen_tag(bs4, "div", ["definitionCol columns"])

    # width is dependent on if it has the return column
    if is_constructor:
        add_class_to_tag(definition_col, "large-12")
    else:
        add_class_to_tag(definition_col, "large-9")

    function_div.append(definition_col)

    # function name
    definition_div = gen_tag(bs4, "div", ["definition"])

    name = fn_xml.find("name").text
    em_tag = gen_tag(bs4, "em", [], name)
    definition_div.append(em_tag)

    argstring = fn_xml.find("argsstring")
    if argstring is None:
        argstring = fn_xml.find("arglist")
    argstring_text = argstring.text if argstring.text is not None else ""

    definition_div.append(argstring_text)
    definition_col.append(definition_div)

    # detailed description
    description_div = markup_description(bs4, fn_xml)
    if description_div is not None:
        definition_col.append(description_div)
        add_class_to_tag(li, "expandable")

    parent.append(li)


def markup_enum(bs4, fn_xml, parent):
    """ Mark up an enum using the function definition

    create new line
    left side = return type
    right side = function name
    under right side = definition

    ---------------------------------------------------
    | returnType    | function( param1, param2, etc ) |
    ---------------------------------------------------
    |               | description                     |
    ---------------------------------------------------
    """

    li = gen_tag(bs4, "li", ["row"])

    # wrapper
    enum_div = gen_tag(bs4, "div", ["enumDef"])
    li.append(enum_div)

    # left side / return type
    left_div = gen_tag(bs4, "div", ["returnCol columns large-2"])
    left_div.append("enum")

    # right side (function name and description)
    right_div = gen_tag(bs4, "div", ["definitionCol columns"])
    add_class_to_tag(right_div, "large-12")

    enum_div.append(right_div)

    # function name
    definition_div = gen_tag(bs4, "div", ["definition"])

    em_tag = gen_tag(bs4, "em", [], fn_xml.find("name").text)
    definition_div.append(em_tag)
    right_div.append(definition_div)

    # detailed description
    description_div = markup_description(bs4, fn_xml)

    # iterate all of the enumvalues
    enum_ul = gen_tag(bs4, "ul")
    for enum in fn_xml.findall("enumvalue"):
        enum_li = gen_tag(bs4, "li", [], enum.find("name").text)
        enum_ul.append(enum_li)

    if description_div is None and len(list(enum_ul.children)) > 0:
        description_div = gen_tag(bs4, "div", ["description", "content"])
        description_div.append(enum_ul)

    if description_div is not None:
        right_div.append(description_div)
        add_class_to_tag(li, "expandable")

    fn_id = fn_xml.attrib["id"].split("_1")[-1]
    li.append(gen_anchor_tag(bs4, fn_id))
    parent.append(li)


def define_tag(bs4, tag_name, tree):
    new_tag = bs4.new_tag(tag_name)
    if tag_name == "a":
        define_link_tag(new_tag, tree.attrib)
    # elif tagName == "code":
    # addClassToTag( newTag, "language-cpp" )
    return new_tag


def gen_includes(file_def):
    bs4 = g_currentFile.bs4

    # create html from template
    side = get_template(bs4, "side-static")
    content_div = side.find("div", "content")

    # fill heading
    side.find('h4').append("#include")

    # add include link
    include_link = gen_tag(bs4, "a", None, file_def.name)
    path = file_def.githubPath

    define_link_tag(include_link, {'href': path})
    content_div.append(include_link)

    g_currentFile.append_to_side_el(side)


def gen_typedefs(bs4, type_defs):
    """ Generates the typedefs side bar, with each linking out
        to its full definition.

    Args:
        bs4: The current beautifulSoup html instance
        typeDefs: An array of SymbolMap::TypeDef Objects

    Returns:
        Empty if the typeDefs list is empty
    """

    # return if empty array
    if len(type_defs) == 0:
        return

    # create html from template
    side = get_template(bs4, "side-expandable")
    content_div = side.find("div", "content")

    # fill heading
    side.find('h4').append("Typedefs:")

    # fill list of typedefs
    type_def_ul = gen_tag(bs4, "ul")
    for typeDef in type_defs:
        type_def_li = gen_tag(bs4, "li")
        a_tag = gen_tag(bs4, "a", [], typeDef.name)
        define_link_tag(a_tag, {"href": typeDef.path})
        type_def_li.append(a_tag)
        type_def_ul.append(type_def_li)

    # plug into html
    if type_def_ul is not None:
        content_div.append(type_def_ul)

    # append to the current file's side element
    g_currentFile.append_to_side_el(side)


def iter_class_base(class_def, hierarchy):
    """ Iterates the class to find all of their base classes
        and iterate through them

    Args:
        classDef: The instance of SymbolMap::Class Object whose base we are searching for
        hierachy: The current hierachy of classes to append to if we find another base
    """

    if class_def is None or hasattr(class_def, 'name') is False:
        return False

    base = class_def.base

    if base is None:
        return False
    else:
        new_tree = g_symbolMap.find_class(base)
        # add to hierarchy if it continues
        if iter_class_base(new_tree, hierarchy) is not False:
            hierarchy.append(new_tree)


def gen_class_hierarchy(bs4, class_def):
    """ Generates the class heirarchy side bar, with each class linking
        out to its class file.

    Args:
        bs4: The current beautifulSoup html instance
        classDef: The instance of SymbolMap::Class Object that we are generating
            the hierachy for

    Returns:
        Empty if there is no base class
    """

    if class_def is None:
        return

    # first item in the list will be the original class
    hierarchy = [class_def]

    # get the class' hierarchy
    iter_class_base(class_def, hierarchy)

    if len(hierarchy) == 1:
        return

    # create html from template
    side = get_template(bs4, "side-expandable")

    # fill heading
    side.find('h4').append("Class Heirarchy:")

    # create all of the markup
    content_div = side.find('div', 'content')
    ul = gen_tag(bs4, "ul")
    add_class_to_tag(ul, "inheritence")
    content_div.append(ul)

    # go through the hierarchy and add a list item for each member
    for index, base in enumerate(reversed(hierarchy)):
        li = gen_tag(bs4, "li")
        add_class_to_tag(li, "depth" + str(index + 1))

        # link out only if a base class, not the original class
        if index < len(hierarchy) - 1:
            a = gen_tag(bs4, "a", [], base.name)
            define_link_tag(a, {'href': base.path})
            li.append(a)
        else:
            li.append(base.name)
        ul.append(li)

    # if len(hierarchy) > 2:
    #     print "GEN CLASS HIERARCHY " + class_def.name
    # print side
    # for h in hierarchy:
    # print h.name

    # append to the current file's side element
    g_currentFile.append_to_side_el(side)


def gen_class_list(bs4, tree):
    classes = tree.findall(r"compounddef/innerclass")

    if len(classes) < 1:
        return

    # create html from template
    side = get_template(bs4, "side-expandable")

    # fill heading
    side.find('h4').append("Classes:")

    # create all of the markup
    classes_ul = gen_tag(bs4, "ul", None)
    content_div = side.find('div', 'content')
    for classDef in classes:
        li = gen_tag(bs4, "li")
        classes_ul.append(li)

        a = gen_tag(bs4, "a", [], strip_compound_name(classDef.text))
        define_link_tag(a, classDef.attrib)
        li.append(a)

    content_div.append(classes_ul)

    g_currentFile.append_to_side_el(side)


def gen_related_links(bs4, class_def):
    """ Generates a new side navigation module that lists any
        related links for this class as generated via parsing
        html files

    :param bs4: The beautifulsoup instance
    :param class_def: the ClassObject that we are adding content to
    :return: None
    """

    if not class_def:
        return

    links = class_def.relatedLinks

    if len(links) == 0:
        return

    # create html from template
    side = get_template(bs4, "side-expandable")

    # fill heading
    side.find('h4').append("Related Links:")

    # create all of the markup
    ul = gen_tag(bs4, "ul", None)
    content_div = side.find('div', 'content')

    # append the links that have already been created
    for link in links:
        li = gen_tag(bs4, "li", None, link)
        ul.append(li)

    # append markup
    content_div.append(ul)
    g_currentFile.append_to_side_el(side)


def replace_tag(bs4, tree, parent_tag, content):
    tag = tree.tag
    attrib = tree.attrib
    has_parent = False
    tag_name = None

    if parent_tag and parent_tag.parent:
        has_parent = True

    # change parentTag if necessary
    if tag == "codeline":
        parent_tag = parent_tag.code

    # find html tag based on tag
    if tag == "para":
        if has_parent and parent_tag.parent.dl:
            tag_name = "dd"
        else:
            tag_name = tagDictionary[tag]
    elif tag == "sp":
        if content is None:
            content = " "
        else:
            content.append(" ")

    # get tag equivalent
    if tag in tagDictionary:
        tag_name = tagDictionary[tag]
        new_tag = define_tag(bs4, tag_name, tree)
    else:
        # TODO: replace with nothing - no new tag
        tag_name = "div"
        new_tag = define_tag(bs4, tag_name, tree)
        add_class_to_tag(new_tag, tag)

    content_tag = new_tag

    # if simplesect, construct with some content
    if tag == "simplesect":
        see_tag = bs4.new_tag("dt")
        add_class_to_tag(see_tag, "section")

        # "see also" reference
        if attrib["kind"] == "see":
            add_class_to_tag(see_tag, "see")
            see_tag.string = "See Also"
        new_tag.append(see_tag)

    if tag == "programlisting":
        code_tag = bs4.new_tag("code")
        add_class_to_tag(code_tag, "language-cpp")
        new_tag.append(code_tag)
        content_tag = code_tag

    if content is not None:
        content_tag.append(content)

    parent_tag.append(new_tag)
    return new_tag


def iterate_markup(bs4, tree, parent):
    if tree is None:
        return

    # origTag = tree.tag
    current_tag = parent
    content = None

    # add content to tag
    if tree.text is not None:
        # currentTag.append( tree.text.strip() )
        content = tree.text.strip()

    # append any new tags
    if tree.tag is not None:
        html_tag = replace_tag(bs4, tree, current_tag, content)

        # if tree parent == <p> && newTag == <pre>
        # add a new pre tag in and make that the current parent again
        current_tag = html_tag

    # iterate through children tags
    for child in list(tree):
        iterate_markup(bs4, child, current_tag)

    # tail is any extra text that isn't wrapped in another tag
    # that exists before the next tag
    if tree.tail is not None:
        parent.append(tree.tail.strip())

        if tree.tail.endswith(";"):
            parent.append(gen_tag(bs4, "br"))

    return current_tag


def markup_description(bs4, tree):
    description_el = gen_tag(bs4, "div", ["description", "content"])
    brief_desc = tree.find(r'briefdescription/')
    if brief_desc is None:
        return
    else:
        iterate_markup(bs4, tree.find(r'briefdescription/'), description_el)

    detailed_desc = tree.find(r'detaileddescription/')
    if detailed_desc is not None:
        iterate_markup(bs4, tree.find(r'detaileddescription/'), description_el)

    return description_el


def replace_code_chunks(bs4):
    """ Looks though the html and replaces any code chunks that exist
        in a paragraph and splits them up so that we can use pre tags.
    """
    # find all p tags
    p_tags = bs4.find_all("p")
    for p in p_tags:

        # if the paragraph as a "div.computeroutput"
        # codeChunks = p.find_all( "div", "computeroutput" )
        code_chunks = p.find_all("div", "programlisting")

        if len(code_chunks) > 0:

            new_tags = []
            cur_p = None

            for c in p.contents:

                # create a new paragraph to wrap content in
                if cur_p is None:
                    cur_p = gen_tag(bs4, "p")
                    new_tags.append(cur_p)

                add_to_p = True

                if type(c) == Tag:
                    if c.has_attr("class"):

                        # look for any tags with "computeroutput"
                        if "programlisting" in c["class"]:

                            # add a new pre
                            pre_tag = gen_tag(bs4, "pre")

                            for code in c.contents:

                                # clone code
                                c_clone = clone(code)

                                # code string
                                code_string = ""
                                if type(code) == Tag:

                                    # clear the original tag
                                    code.clear()

                                    # add clone to holder so that we can iterate through
                                    holder = gen_tag(bs4, "div")
                                    holder.append(c_clone)

                                    # append line breaks for each line of code
                                    for content in holder.code.contents:
                                        code_string += content.getText() + "\n"

                                if len(c_clone) > 0:
                                    code.append(code_string)
                                    pre_tag.append(code)

                            new_tags.append(pre_tag)

                            # create a new p tag for any following content to add itself to
                            cur_p = gen_tag(bs4, "p")
                            new_tags.append(cur_p)
                            add_to_p = False

                if add_to_p:
                    c_clone = clone(c)
                    cur_p.append(c_clone)

            # replace p tag with an empty span
            replacement_tag = gen_tag(bs4, "span")
            p.replace_with(replacement_tag)

            # add the newly organized tags to the new span
            for t in new_tags:
                replacement_tag.append(t)

            # unwrap / remove the span since that was only there to place this new content into
            replacement_tag.unwrap()


# clone an element
# from: http://stackoverflow.com/questions/23057631/clone-element-with-beautifulsoup/23058678#23058678
def clone(el):
    if isinstance(el, NavigableString):
        return type(el)(el)

    tag_copy = Tag(None, el.builder, el.name, el.namespace, el.nsprefix)
    # work around bug where there is no builder set
    # https://bugs.launchpad.net/beautifulsoup/+bug/1307471
    tag_copy.attrs = dict(el.attrs)
    tag_copy.index = el.index
    for attr in ('can_be_empty_element', 'hidden'):
        setattr(tag_copy, attr, getattr(el, attr))
    for child in el.contents:
        tag_copy.append(clone(child))
    return tag_copy


# pull a templated chunk of html out of the selected bs4 file
def get_template(bs4, element_id):
    templates = bs4.find_all('template')
    template = None

    for t in templates:
        # [0] is a string before the enclosed div, so used [1] instead
        if t['id'] == element_id:
            template = clone(list(t.contents)[1])
        else:
            continue

    return template

def inject_html(src_content, dest_el, src_path, dest_path ):
    """
    Append a chunk of html into a specific div
    :param src_content: The src html to be injected
    :param dest_el: The div to inject the src html into
    :param src_path: The path of the src file so that we can gix teh relative links
    :return:
    """

    update_links(src_content, src_path, dest_path)

    try:
        # append body content to dest_el
        dest_el.append(src_content.body)
    except AttributeError as e:
        print "\t *** Error appending html content to element ***"


def iterate_namespace(bs4, namespaces, tree, index, label):
    # Get namespace of previous child, unless first
    if index == 0:
        parent_ns = ""
    else:
        parent_ns = namespaces[index - 1].name

    count = index
    child_count = 0

    # iterate to find all children of parentNs
    for ns in namespaces[index:]:
        namespace = ns.name  # full namespace
        ns_parts = namespace.split("::")
        prefix = "::".join(ns_parts[:-1])  # parent namespace up to last ::

        name = "".join(ns_parts[-1])
        node_label = label + str(child_count)

        # check if derived from any parent
        parent_is_derived = has_ancestor(namespaces, namespace)

        # create a list item for the namespace
        ns_li = gen_tag(bs4, "li")

        # create link for each item
        a_tag = gen_tag(bs4, "a")
        define_link_tag(a_tag, {"href": ns.path})
        a_tag.append(name)

        # is decendent of parent namespace
        if prefix == parent_ns:

            child_count += 1

            # append to parent
            tree.append(ns_li)

            # generate new nested ul in case there are children
            ns_ul = gen_tag(bs4, "ul")
            if count < len(namespaces):

                # if there are children, add to the parent ul
                if iterate_namespace(bs4, namespaces, ns_ul, count + 1, node_label) > 0:
                    # addClassToTag( nsLi, "item" )
                    # add input
                    input_el = gen_tag(bs4, "input")
                    input_el["type"] = "checkbox"
                    input_el["id"] = "item-" + "-".join(list(node_label))
                    label_tag = gen_tag(bs4, "label")
                    label_tag["for"] = "item-" + "-".join(list(node_label))
                    label_tag.append(a_tag)

                    ns_li.insert(0, input_el)
                    ns_li.append(label_tag)
                    ns_li.append(ns_ul)
                else:
                    ns_li.append(a_tag)

        else:
            # has no direct descendent on the parent, so add it independently
            if parent_is_derived is False and index is 0:
                child_count += 1
                indie_li = gen_tag(bs4, "li")
                # indieLi.append( prefix )

                # TODO: refactor and simplify some of this stuff
                input_el = gen_tag(bs4, "input")
                input_el["type"] = "checkbox"
                input_el["id"] = "item-" + "-".join(list(node_label))
                indie_li.insert(0, input_el)

                label_tag = gen_tag(bs4, "label")
                label_tag["for"] = "item-" + "-".join(list(node_label))
                label_tag.append(prefix)
                indie_li.append(label_tag)

                indie_ul = gen_tag(bs4, "ul")
                indie_li.append(indie_ul)
                indie_ul.append(ns_li)
                ns_li.append(a_tag)

                tree.append(indie_li)

        count += 1

    return child_count


def has_ancestor(namespaces, compare_namespace):
    compare_prefix = "::".join(compare_namespace.split("::")[0])
    # hasAncestor = False
    for ns in namespaces:
        namespace = ns.name
        prefix = "::".join(namespace.split("::")[0])
        if prefix == compare_prefix and compare_namespace != namespace:
            return True

    return False


def generate_namespace_nav(bs4):
    """
    Creates a div filled with a list of namespace links
    :param bs4: The Beautiful soup instance used for dom manipulation
    :return: a new div that contains the navigation tree
    """
    namespaces = g_symbolMap.get_whitelisted_namespaces()

    tree = gen_tag(bs4, "div")
    ul = gen_tag(bs4, "ul")
    tree.append(ul)
    add_class_to_tag(tree, "css-treeview")

    iterate_namespace(bs4, namespaces, ul, 0, "")
    return tree


def process_class_xml_file(in_path, out_path, html):
    global g_currentFile
    print "Processing file: " + in_path + " > " + out_path

    # define the tree that contains all the data we need to populate this page
    tree = parse_html(html, in_path, out_path)

    # update links in the template
    update_links(html, TEMPLATE_PATH + "htmlContentTemplate.html", out_path)

    if tree is None:
        return

    # get common data for the file
    g_currentFile = FileData(tree, html)
    # compoundName = g_currentFile.compoundName
    class_name = g_currentFile.name
    kind = g_currentFile.kind_explicit

    # dictionary for subnav anchors
    subnav_anchors = []

    # find include name (header file)
    include_trees = tree.findall(r"compounddef/includes")
    include_def = None
    if len(include_trees) > 0:
        include_def = tree.findall(r"compounddef/includes")[0].text

    # find file definition for the include file
    file_def = None
    if include_def is not None:
        file_def = g_symbolMap.find_file(include_def)

    # find class definition
    class_def = g_symbolMap.find_class(class_name)

    # +-----------------------------------------+
    #  find parts in template that already exist
    # +-----------------------------------------+
    contents_el = g_currentFile.contentsEl
    side_nav_el = g_currentFile.sideNavEl

    # +-------------+
    #  Namespace Nav
    # +-------------+
    side_nav_el.append(g_namespaceNav)

    # +---------------+
    #  Side Area Nodes
    # +---------------+
    # includes
    if include_def is not None:
        gen_includes(file_def)

    # typedefs
    if file_def is not None:
        gen_typedefs(html, file_def.typedefs)
    # class hierarchy
    gen_class_hierarchy(html, class_def)
    # class list
    gen_class_list(html, tree)
    # related links (generated by guides and references)
    gen_related_links(html, class_def)
    # print classDef.seeMoreLinks

    # +-----------+
    #  Description
    # +-----------+
    # description prose
    desc_tag = markup_description(html, tree.find(r'compounddef'))
    if desc_tag is not None:
        g_currentFile.descriptionProseEl.append(desc_tag)

    # +------+
    #  Prefix
    # +------+
    # if the class has a prefix, add it here
    if hasattr(class_def, 'prefixPath') is True and class_def.prefixPath is not None:
        # inject html from prefixPath into description div
        orig_html = generate_bs4(class_def.prefixPath)
        inject_html(orig_html, g_currentFile.descriptionProseEl, class_def.prefixPath, out_path)

    # +------------+
    #  Enumerations
    # +------------+
    enumerations = tree.findall(r"compounddef/sectiondef/memberdef[@kind='enum']")
    if len(enumerations) > 0:
        drop_anchor(subnav_anchors, "enumerations", "Public Types")
        enum_section = gen_tag(html, "section")
        contents_el.append(enum_section)

        enum_section.append(gen_tag(html, "h1", [], "Public Types"))
        enum_ul = gen_tag(html, "ul")
        for c in enumerations:
            markup_enum(html, c, enum_ul)
        enum_section.append(enum_ul)
        contents_el.append(gen_tag(html, "br"))

    # +----------------+
    #  Member Functions
    # +----------------+
    # create regular and static member function ul wrappers
    ul_tag = html.new_tag("ul")
    static_ul = html.new_tag("ul")
    add_class_to_tag(static_ul, "static")

    func_count = 0
    static_func_count = 0

    # public member functions
    for memberFn in tree.findall(r'compounddef/sectiondef/memberdef[@kind="function"][@prot="public"]'):
        # split between static or not
        is_static = memberFn.attrib["static"]
        member_name = memberFn.find(r"name")
        is_constructor = False

        # determine if it is a constructor
        if member_name is not None and member_name.text == class_name:
            is_constructor = True

        # determine if static
        if is_static == 'yes':
            static_func_count += 1
            markup_function(html, memberFn, static_ul, is_constructor)
        else:
            func_count += 1
            markup_function(html, memberFn, ul_tag, is_constructor)

    # if function count > 0 add it
    if func_count > 0:
        # add anchor tag to page
        anchor = gen_anchor_tag(html, "public-member-functions")
        header_str = public_function_header.get(kind) if kind in public_function_header else public_function_header.get("class")
        subnav_anchors.append({"name": header_str, "link": anchor})
        contents_el.append(anchor)

        # public member functions
        header = html.new_tag("h1")
        header.string = header_str
        section_tag = gen_tag(html, "section")
        section_tag.append(header)
        section_tag.append(ul_tag)
        contents_el.append(section_tag)
        section_tag.append(gen_tag(html, "hr"))

    # if static function count > 0 add it
    if static_func_count > 0:
        anchor = gen_anchor_tag(html, "static-public-member-functions")
        header_str = public_static_function_header.get(kind) if kind in public_static_function_header else public_static_function_header.get("class")
        subnav_anchors.append({"name": header_str, "link": anchor})
        contents_el.append(anchor)

        # static public member functions
        header = html.new_tag("h1")
        header.string = header_str
        section_tag = gen_tag(html, "section")
        section_tag.append(header)
        section_tag.append(static_ul)
        contents_el.append(section_tag)
        section_tag.append(gen_tag(html, "hr"))

    # protected attributes
    # for memberFn in tree.findall(r'compounddef/sectiondef/memberdef[@kind="variable"][@prot="protected"]'):
    #     # split between static or not
    #     member_name = memberFn.find(r"name")
    #
    #     # determine if static
    #     if is_static == 'yes':
    #         static_func_count += 1
    #         markup_function(html, memberFn, static_ul, is_constructor)
    #     else:
    #         func_count += 1
    #         markup_function(html, memberFn, ul_tag, is_constructor)

    variables = tree.findall(r'compounddef/sectiondef/memberdef[@kind="variable"][@prot="protected"]')
    if len(variables) > 0:
        drop_anchor(subnav_anchors, "variables", "Protected Attributes")
        vars_section = gen_tag(html, "section")
        contents_el.append(vars_section)

        vars_section.append(gen_tag(html, "h1", [], "Protected Attributes"))
        vars_ul = gen_tag(html, "ul")
        for c in variables:
            type_str = c.find('type').text
            name = gen_tag(html, "div")
            name.append(gen_tag(html, "b", [], c.find('name').text))
            initializer = c.find('initializer').text if c.find('initializer') is not None else None
            description = markup_description(html, c)
            if type_str is None:
                type_str = ""
            if initializer is not None:
                name.append(" " + initializer)
            add_row_li(html, vars_ul, type_str, name, None, description)
        vars_section.append(vars_ul)
        vars_section.append(gen_tag(html, "hr"))

    # replace any code chunks with <pre> tags, which is not possible on initial creation
    replace_code_chunks(html)

    # generate subnav
    gen_sub_nav(subnav_anchors)

    # link up all ci tags
    for tag in html.find_all('ci'):
        process_ci_tag(html, tag, in_path, out_path)

    # add file to search index
    search_tags = []
    if class_def:
        search_tags = class_def.tags
    # print search_tags
    add_to_search_index(html, out_path, search_tags)

    # write the file
    write_html(html, out_path)


def gen_sub_nav(subnav_anchors):
    # +-------------------------------------------------+
    #  SubNav
    #  Fill subnav based on what is actually in the page
    # +-------------------------------------------------+
    bs4 = g_currentFile.bs4
    subnav_el = bs4.find(id="sub-nav")
    subnav_ul = gen_tag(bs4, "ul")

    # for all of the subnav anchors, add a link into the subnav list
    for anchor in subnav_anchors:
        li = gen_tag(bs4, "li")
        link = gen_tag(bs4, "a", [], anchor["name"])
        define_link_tag(link, {"href": "#" + anchor["link"]["name"]})
        li.append(link)
        subnav_ul.append(li)
    subnav_el.append(subnav_ul)


def add_row_li(bs4, container, left_content, right_content, col_breakdown=None, dropdown_content=None):
    left_col = "3"
    right_col = "9"
    if col_breakdown is not None:
        cols = col_breakdown.split("-")
        left_col = cols[0]
        right_col = cols[1]

    li = gen_tag(bs4, "li", ["row"])

    # left side
    left_div = gen_tag(bs4, "div", ["returnCol columns"])
    add_class_to_tag(left_div, "large-" + left_col)
    if left_content is not None:
        left_div.append(left_content)
        li.append(left_div)

    # right side
    right_div = gen_tag(bs4, "div", ["definitionCol columns"])
    add_class_to_tag(right_div, "large-" + right_col)
    li.append(right_div)
    right_div.append(right_content)

    container.append(li)

    # if there is dropdown content, the right side will have an additional row below it
    if dropdown_content is not None:
        right_div.append(dropdown_content)
        add_class_to_tag(li, "expandable")


def drop_anchor(anchor_list, anchor_name, link_name):
    bs4 = g_currentFile.bs4
    anchor = gen_anchor_tag(bs4, anchor_name)
    anchor_list.append({"name": link_name, "link": anchor})
    g_currentFile.contentsEl.append(anchor)


def list_namespaces(bs4, container):
    namespaces = g_symbolMap.get_ordered_namespaces()

    ul = gen_tag(bs4, "ul")
    for ns in namespaces:
        a = gen_link_tag(bs4, ns.name, ns.path)
        li = gen_tag(bs4, "li", None, a)
        ul.append(li)

    if len(ul) > 0:
        container.append(ul)


def list_classes(bs4, container):
    classes = g_symbolMap.get_ordered_class_list()

    ul = gen_tag(bs4, "ul", ["master-class-list"])
    for c in classes:
        a = gen_link_tag(bs4, c.name, c.path)
        li = gen_tag(bs4, "li", None, a)
        ul.append(li)

    if len(ul) > 0:
        container.append(ul)


def process_namespace_xml_file(in_path, out_path, html):
    global g_currentFile
    print "Processing namespace file: " + in_path + " > " + out_path

    # define the tree that contains all the data we need to populate this page
    tree = parse_html(html, in_path, out_path)
    if tree is None:
        return

    # update links in the template
    update_links(html, TEMPLATE_PATH + "htmlContentTemplate.html", out_path)

    # get common data for the file
    g_currentFile = FileData(tree, html)
    compound_name = g_currentFile.compoundName
    # className = g_currentFile.name
    # kind = g_currentFile.kind

    # dictionary for subnav anchors
    subnav_anchors = []

    # +-----------------------------------------+
    #  find parts in template that already exist
    # +-----------------------------------------+
    # contentsTag = html.find( "div", "contents" )
    contents_el = g_currentFile.contentsEl

    # +-------------+
    #  Namespace Nav
    # +-------------+
    # side area
    side_nav_tag = html.find(id="side-nav")
    side_nav_tag.append(g_namespaceNav)

    # +-----------+
    #  Description
    # +-----------+
    namespace_tag = g_symbolMap.find_namespace(compound_name)
    if namespace_tag is None:
        return

    # +----------+
    #  Namespaces
    # +----------+
    namespaces = tree.findall(r"compounddef/innernamespace")
    if len(namespaces) > 0:
        drop_anchor(subnav_anchors, "namespaces", "Namespaces")
        namespaces_section = gen_tag(html, "section")
        contents_el.append(namespaces_section)

        namespaces_ul = gen_tag(html, "ul")
        for c in namespaces:
            namespace_link_tag = gen_link_tag(html, c.text, c.attrib["refid"] + ".html")
            add_row_li(html, namespaces_ul, None, namespace_link_tag)

        namespaces_section.append(gen_tag(html, "h2", [], "Namespaces"))
        namespaces_section.append(namespaces_ul)

        contents_el.append(gen_tag(html, "br"))

    # +-------+
    #  Classes
    # +-------+
    classes = tree.findall(r"compounddef/innerclass")
    if len(classes) > 0:
        drop_anchor(subnav_anchors, "classes", "Classes")
        classes_section = gen_tag(html, "section")
        contents_el.append(classes_section)

        classes_section.append(gen_tag(html, "h2", [], "Classes"))
        class_ul = gen_tag(html, "ul")
        for c in classes:
            link = c.attrib["refid"] + ".html"
            kind = "struct" if link.startswith("struct") else "class"
            link_tag = gen_link_tag(html, strip_compound_name(c.text), link)
            add_row_li(html, class_ul, kind, link_tag, "1-11")
        classes_section.append(class_ul)

        contents_el.append(gen_tag(html, "br"))

    # +--------+
    #  TypeDefs
    # +--------+
    typedefs = tree.findall(r"compounddef/sectiondef/[@kind='typedef']/memberdef/[@kind='typedef']")
    if len(typedefs) > 0:
        drop_anchor(subnav_anchors, "typedefs", "Typedefs")
        typedef_section = gen_tag(html, "section")
        contents_el.append(typedef_section)

        typedef_section.append(gen_tag(html, "h2", [], "Typedefs"))
        typedef_ul = gen_tag(html, "ul")
        for c in typedefs:
            markup_function(html, c, typedef_ul, False)
        typedef_section.append(typedef_ul)

        contents_el.append(gen_tag(html, "br"))

    # +------------+
    #  Enumerations
    # +------------+
    enumerations = tree.findall(r"compounddef/sectiondef/[@kind='enum']/memberdef/[@kind='enum']")
    if len(enumerations) > 0:
        drop_anchor(subnav_anchors, "enumerations", "Enumerations")
        enum_section = gen_tag(html, "section")
        contents_el.append(enum_section)

        enum_section.append(gen_tag(html, "h2", [], "Enumerations"))
        enum_ul = gen_tag(html, "ul")
        for c in enumerations:
            markup_enum(html, c, enum_ul)
        enum_section.append(enum_ul)

        contents_el.append(gen_tag(html, "br"))

    # +---------+
    #  Functions
    # +---------+
    functions = tree.findall(r"compounddef/sectiondef/[@kind='func']/memberdef/[@kind='function']")
    if len(functions) > 0:
        drop_anchor(subnav_anchors, "functions", "Functions")
        function_section = gen_tag(html, "section")
        contents_el.append(function_section)

        function_section.append(gen_tag(html, "h2", [], "Functions"))
        function_ul = gen_tag(html, "ul")
        for c in functions:
            markup_function(html, c, function_ul, False)
        function_section.append(function_ul)

        contents_el.append(gen_tag(html, "br"))

    # +---------+
    #  Variables
    # +---------+
    variables = tree.findall(r"compounddef/sectiondef/[@kind='var']/memberdef/[@kind='variable']")
    if len(variables) > 0:
        drop_anchor(subnav_anchors, "variables", "Variables")
        vars_section = gen_tag(html, "section")
        contents_el.append(vars_section)

        vars_section.append(gen_tag(html, "h2", [], "Variables"))
        vars_ul = gen_tag(html, "ul")
        for c in variables:
            type_str = c.find('type').text
            name = gen_tag(html, "div")
            name.append(gen_tag(html, "b", [], c.find('name').text))
            initializer = c.find('initializer').text if c.find('initializer') is not None else None
            description = markup_description(html, c)
            if type_str is None:
                type_str = ""
            if initializer is not None:
                name.append(" " + initializer)
            add_row_li(html, vars_ul, type_str, name, None, description)
        vars_section.append(vars_ul)

    gen_sub_nav(subnav_anchors)

    # add file to search index
    add_to_search_index(html, out_path, ["namespace"])

    # write the file
    write_html(html, out_path)


def process_html_file(in_path, out_path):
    """ Parses an html file.
    - Adds template around the html
    - Copy original css and js links into new hmtl
    - Save html in destination dir
    """
    print "processHtmlFile: " + in_path

    html_template = "htmlContentTemplate.html"
    is_index = False
    if in_path.find("reference/") > -1:
        html_template = "referenceContentTemplate.html"
    elif in_path.find("guides/") > -1:
        html_template = "guidesContentTemplate.html"

    is_searchable = False
    search_tags = []

    # construct template

    if in_path.find("htmlsrc/index.html") > -1:
        is_index = True

    if is_index is True:
        bs4 = generate_bs4(in_path)

    else:
        bs4 = construct_template(["headerTemplate.html", "mainNavTemplate.html", html_template, "footerTemplate.html"])

        orig_html = generate_bs4(in_path)
        template_content_el = bs4.body.find(id="template-content")

        # update links in the template
        update_links(bs4, TEMPLATE_PATH + html_template, out_path)

        # inject html into a template content div
        inject_html(orig_html, template_content_el, in_path, out_path)

        # fill namespace list
        if in_path.find("htmlsrc/namespaces.html") > -1:
            # throw in a list of namespaces into the page
            list_namespaces(bs4, template_content_el)

        # fill class list
        elif in_path.find("htmlsrc/classes.html") > -1:
            list_classes(bs4, template_content_el)

        else:
            # add file to search index
            is_searchable = True

        # copy all js and css paths that may be in the original html and paste into new file
        for link in orig_html.find_all("link"):
            bs4.head.append(link)

        for script in orig_html.find_all("script"):
            bs4.body.append(script)

        if orig_html.head:
            for d in orig_html.head.find_all("ci"):
                bs4.head.append(d)

        # copy title over
        if orig_html.head.title:
            if not bs4.head.title:
                bs4.head.append(gen_tag(bs4, "title"))
            bs4.head.title.append(orig_html.head.title.text)

        # link up all ci tags
        for tag in bs4.find_all('ci'):
            process_ci_tag(bs4, tag, in_path, out_path)

        # add tags from the meta keywords tag
        for meta_tag in orig_html.head.findAll(attrs={"name": "keywords"}):
            for keyword in meta_tag['content'].split(','):
                search_tags.append(keyword.encode('utf-8').strip())

    if in_path.find("_docs/") < 0:
        if is_searchable:
            add_to_search_index(bs4, out_path, search_tags)

        write_html(bs4, out_path)


# -------------------------------------------------- CI TAG FUNCTIONS --------------------------------------------------

def process_ci_tag(bs4, tag, in_path, out_path):
    """
    Depending on the attributes of the ci tag, do something different
    :param bs4: The current beautiful soup instance
    :param tag: The ci tag to process
    :param in_path: The path to the current processed html file
    :param out_path: The save path to the prcessing html file
    :return:
    """
    if tag.has_attr("seealso"):
        process_ci_seealso_tag(bs4, tag, out_path)

    elif tag.has_attr("prefix"):
        process_ci_prefix_tag(tag, in_path)

    # elif tag.has_attr("source"):
    #     process_ci_source_tag(bs4, tag)

    else:
        replace_ci_tag(bs4, tag)


def replace_ci_tag(bs4, link):
    ref_obj = find_ci_tag_ref(link)

    if ref_obj:
        ref_location = DOXYGEN_HTML_PATH + ref_obj.path
        new_link = gen_link_tag(bs4, link.contents[0], ref_location)
        link.replace_with(new_link)
    else:
        print "   ** Warning: Could not find replacement tag for ci tag: " + str(link)


def process_ci_seealso_tag(bs4, tag, out_path):
    """
    Processes ci tag that is of 'seealso' type
    :param bs4: The active beautiful soup instance
    :param tag: the ci tag to find a reference for
    :param out_path: the file path
    :return: None
    """
    ref_obj = find_ci_tag_ref(tag)
    print ref_obj.name

    # get label attribute value if there is one
    if tag.has_attr("label"):
        label = tag["label"]
    # otherwise use the name of the file as the label
    else:
        label = get_file_name(out_path)

    link_tag = gen_link_tag(bs4, label, out_path)

    if type(ref_obj) is SymbolMap.Class or type(ref_obj) is SymbolMap.Typedef:
        ref_obj.add_related_link(link_tag)

    elif type(ref_obj) is SymbolMap.Namespace:
        # find all classes with that namespace and add guide to every one
        for class_obj in g_symbolMap.find_classes_in_namespace(ref_obj.name):
            class_obj.add_related_link(link_tag)
    else:
        print "  ** WARNING: Could not find seealso reference for " + str(tag)


def process_ci_prefix_tag(tag, in_path):
    """
    Finds the referenced tag's object if existent and adds the path to the prefix file to the class to be parsed later
    :param tag: The ci tag with a defined prefix attribute
    :param in_path: The path to the refix content
    :return:
    """
    obj_ref = find_ci_tag_ref(tag)
    if obj_ref and type(obj_ref) is SymbolMap.Class:
        obj_ref.define_prefix(in_path)

# TODO: add ability to replace ci tag with link to github source file
# def process_ci_source_tag(bs4, tag):
#     """
#     Replace the ci tag with a link to a source file on github
#     :param tag: the tag to find a link for
#     :return:
#     """
#     link_title = "LINK TITLE"
#     # link_url =
#     link_tag = gen_link_tag(bs4, link_title)

def find_ci_tag_ref(link):
    # get string to search against
    searchstring = ""
    if len(link.contents):
        searchstring = link.contents[0]

    if link.get('dox') is not None:
        searchstring = link.get('dox')

    ref_obj = None

    # args = searchstring.split("|")
    ns_str = "::".join(searchstring.split("::")[:-1])
    stripped_searchstring = strip_compound_name(searchstring)

    # find function link
    if link.get('kind') == 'function':

        # find parent class first
        existing_class = g_symbolMap.find_class(ns_str)
        if existing_class is not None:
            # find the function in the given class
            fn_obj = g_symbolMap.find_function(stripped_searchstring, existing_class)
            if fn_obj is not None:
                ref_obj = fn_obj

    elif link.get('kind') == 'enum':
        enum_obj = g_symbolMap.find_enum(searchstring)
        if enum_obj is not None:
            ref_obj = enum_obj

    # find class link
    else:
        existing_class = g_symbolMap.find_class(searchstring)
        if existing_class is not None:
            ref_obj = existing_class
        else:

            count = 0
            # try a bunch of other things before giving up
            while (ref_obj is None) and count < 3:
                if count == 0:
                    ref_obj = g_symbolMap.find_namespace(searchstring)
                elif count == 1:
                    ref_obj = g_symbolMap.find_function(searchstring)
                elif count == 2:
                    ref_obj = g_symbolMap.find_enum(searchstring)
                count += 1

    return ref_obj

# ----------------------------------------------- END CI TAG FUNCTIONS -------------------------------------------------

def update_links(html, src_path, dest_path):
    """
    Replace all of the relative a links, js links and image links and make them relative to the outpath
    :param html:
    :param src_path:
    :param dest_path:
    :return:
    """

    # css links
    for link in html.find_all("link"):
        if link.has_attr("href"):
            link["href"] = update_link(link["href"], src_path, dest_path)

    # a links
    for a in html.find_all("a"):
        if a.has_attr("href"):
            a["href"] = update_link(a["href"], src_path, dest_path)

    # script links
    for script in html.find_all("script"):
        if script.has_attr("src"):
            script["src"] = update_link(script["src"], src_path, dest_path)

    # images
    for img in html.find_all("img"):
        if img.has_attr("src"):
            img["src"] = update_link(img["src"], src_path, dest_path)

    # iframes
    iframe_links = []
    for iframe in html.find_all("iframe"):
        if iframe.has_attr("src"):
            link_src = iframe["src"]
            if link_src.startswith('javascript') or link_src.startswith('http'):
                return

            src_file = urlparse.urljoin(src_path, link_src)
            dest_file = src_file.replace("htmlsrc", "html")
            update_link(iframe["src"], src_path, dest_path)

            # iframe_link = update_link(iframe["src"], src_path)
            iframe["src"] = dest_file

            # iframe_links.append(iframe_link)

            # iframe_src =
            print "IFRAME LINK: " + src_file
            print "           : " + dest_file
            shutil.copy2(src_file, dest_file)

    # coy over any iframe html files, if any



def update_link(link, in_path, out_path):
    """
    Update the given link to point to something relative to the new path
    :param link: The link to change
    :param in_path: the original path to the file that the link lives in
    :return:
    """

    if link.startswith("http") or link.startswith("javascript:"):
        return link

    # if a relative path, make it absolute
    if in_path.find(BASE_PATH) < 0:
        in_path = BASE_PATH + in_path

    # get absolute in path
    abs_link_path = urlparse.urljoin(in_path, link)

    # convert to relative link in relation to the out path
    rel_link_path = os.path.relpath(abs_link_path.replace("htmlsrc", "html"), os.path.dirname(out_path))

    return rel_link_path


def construct_template(templates):
    """ Constructs a beautiful soup instance by mashing a bunch of html files together
    """
    master_template = ""
    for templatePath in templates:
        master_template += open(os.path.join(HTML_SOURCE_PATH, TEMPLATE_PATH + templatePath)).read()
    master_template.decode("UTF-8")
    bs4 = BeautifulSoup(master_template)
    return bs4


def generate_bs4(file_path):
    output_file = open(os.path.join(file_path)).read()
    output_file.decode("UTF-8")
    # print output_file

    # wrap in body tag if none exists
    if output_file.find("<body") < 0:
        output_file = "<body>" + output_file + "</body>"
        print "\t ** WARNING: No body tag found in file: " + file_path

    bs4 = BeautifulSoup(output_file)

    return bs4


def get_symbol_to_file_map():
    """
    Returns a dictionary from Cinder class name to file path
    """
    print "generating symbol map from tag file"
    symbol_map = SymbolMap()

    # find classes
    class_tags = g_tag_xml.findall(r'compound/[@kind="class"]')
    for c in class_tags:
        class_obj = SymbolMap.Class(c)
        name = class_obj.name
        base_class = class_obj.base
        # file_path = c.find('filename').text
        # base_class = c.find('base').text if c.find('base') is not None else ""
        # class_obj = SymbolMap.Class(name, base_class, file_path)
        symbol_map.classes[name] = class_obj

        # find functions and add to symbol map
        members = c.findall(r"member[@kind='function']")
        for member in members:
            fn_name = member.find("name").text
            anchor = member.find("anchor").text
            file_path = member.find("anchorfile").text + "#" + anchor
            # print "FUNCTION " + name + "::" + fn_name
            function_obj = SymbolMap.Function(fn_name, base_class, file_path)
            symbol_map.functions[name + "::" + fn_name] = function_obj
            class_obj.functionList.append(function_obj)

        # print "CLASS: " + name
        # if name == "Iter":
        #     raise

        # find enums
        for member in c.findall(r"member/[@kind='enumeration']"):
            pre = name + "::" if name is not None else ""
            enum_name = pre + member.find("name").text
            anchor = member.find("anchor").text
            path = member.find("anchorfile").text + "#" + anchor
            enum_obj = SymbolMap.Enum(enum_name, path)
            symbol_map.enums[enum_name] = enum_obj

    # find structs
    struct_tags = g_tag_xml.findall(r'compound/[@kind="struct"]')
    for s in struct_tags:
        struct_obj = SymbolMap.Class(s)
        name = struct_obj.name
        base_class = struct_obj.base

        symbol_map.classes[name] = struct_obj

        # find functions and add to symbol map
        members = s.findall(r"member[@kind='function']")
        for member in members:
            fn_name = member.find("name").text
            anchor = member.find("anchor").text
            file_path = member.find("anchorfile").text + "#" + anchor
            function_obj = SymbolMap.Function(fn_name, base_class, file_path)
            symbol_map.functions[name + "::" + fn_name] = function_obj
            struct_obj.functionList.append(function_obj)

    # find namespaces
    ns_tags = g_tag_xml.findall(r'compound/[@kind="namespace"]')

    for ns in ns_tags:
        namespace_name = ns.find('name').text
        file_name = ns.find('filename').text

        # skip namespaces with '@' in them
        if namespace_name.find('@') > -1:
            continue

        ns_obj = SymbolMap.Namespace(namespace_name, file_name)
        symbol_map.namespaces[namespace_name] = ns_obj

        for member in ns.findall(r"member/[@kind='typedef']"):
            name = member.find("name").text
            type_name = member.find("type").text
            name = namespace_name + "::" + name
            shared_from_class = None

            if type_name.startswith("class") > 0:
                shared_from_class = symbol_map.find_class(type_name.split("class ")[1])

            elif type_name.find("shared") > 0:
                shareds = re.findall(r"std::shared_ptr< (?:class)* *([\w]*) >", type_name)
                if len(shareds) > 0:
                    base = namespace_name + "::" + shareds[0]
                    shared_from_class = symbol_map.find_class(base)

            if not shared_from_class:
                # find based on the string in type that's not explicitly a shared_ptr
                # such as <type>SurfaceT&lt; uint8_t &gt;</type>
                shareds = re.findall(r"([A-Za-z]*)", type_name)
                shared_from_class = symbol_map.find_class(shareds[0])

            file_path = member.find('anchorfile').text + "#" + member.find("anchor").text
            type_def_obj = SymbolMap.Typedef(name, type_name, file_path)

            if shared_from_class is not None:
                type_def_obj.sharedFrom = shared_from_class
                # let the class know that it has some typedefs associated with it
                shared_from_class.add_type_def(type_def_obj)

            symbol_map.typedefs[name] = type_def_obj

        # find enums
        for member in ns.findall(r"member/[@kind='enumeration']"):
            name = namespace_name + "::" + member.find("name").text
            # print "ENUM: " + name
            anchor = member.find("anchor").text
            path = member.find("anchorfile").text + "#" + anchor
            enum_obj = SymbolMap.Enum(name, path)
            symbol_map.enums[name] = enum_obj

        # find functions and add to symbol map
        members = ns.findall(r"member[@kind='function']")
        for member in members:
            fn_name = member.find("name").text
            anchor = member.find("anchor").text
            file_path = member.find("anchorfile").text + "#" + anchor
            function_obj = SymbolMap.Function(fn_name, base_class, file_path)
            ns_obj.functionList.append(function_obj)

    # find files
    file_tags = g_tag_xml.findall(r'compound/[@kind="file"]')
    for f in file_tags:
        name = f.find('name').text
        # filePath = f.find('path').text + f.find('filename').text
        file_path = f.find('path').text + name
        typedefs = []

        # find typedefs for each file
        for t in f.findall(r'member[@kind="typedef"]'):
            td_name = t.find("name").text
            type_name = t.find("type").text
            type_path = t.find('anchorfile').text + "#" + t.find("anchor").text
            typedef = SymbolMap.Typedef(td_name, type_name, type_path)
            typedefs.append(typedef)

        # print "FILE PATH: " + name + " | " + filePath
        symbol_map.files[name] = SymbolMap.File(name, file_path, typedefs)

        # find typedefs for each file
        for f in f.findall(r'member[@kind="function"]'):
            fn_name = f.find("name").text
            file_path = f.find('anchorfile').text + "#" + f.find("anchor").text
            fn = SymbolMap.Function(fn_name, "", file_path)
            symbol_map.functions[fn_name] = fn

    return symbol_map


def get_file_prefix(file_path):
    return os.path.splitext(os.path.basename(file_path))[0]


def get_file_extension(file_path):
    return os.path.splitext(os.path.basename(file_path))[1]


def get_file_name(file_path):
    return os.path.basename(file_path)


def parse_html(bs4, in_path, out_path):
    tree = None
    try:
        with open(in_path, 'r') as xml_file:
            tree = Et.parse(xml_file)

    except:
        exc = sys.exc_info()[0]
        print "\n--- PARSE ERROR ---"
        print "COULD NOT PARSE FILE: " + in_path
        print exc
        # write the file with error
        bs4.append('COULD NOT PARSE FILE: ' + in_path)
        write_html(bs4, out_path)
        print "-------------------\n"

    return tree


def write_html(html, save_path):

    # convert entities in code blocks
    for c in html.find_all("code"):
        for child in c.children:
            # replaces with escaped code
            try:
                child.replace_with(str(child))
            except UnicodeEncodeError as e:
                print "\t** UNICODE ENCODE ERROR: " + e.reason
            except ValueError as e:
                print child
                print "\t** VALUE ENCODE ERROR: " + e.message

    document = html.encode( 'utf-8', formatter="html" )

    if not os.path.exists(os.path.dirname(save_path)):
        os.makedirs(os.path.dirname(save_path))
    with codecs.open(save_path, "w", "UTF-8") as outFile:
        outFile.write(document)

def write_search_index():

    # with open(DOXYGEN_HTML_PATH + 'search_index.json', 'w') as outfile:
    #     json.dump(g_search_index, outfile)

    # save search index to js file
    document = "var search_index_data = " + json.dumps(g_search_index).encode( 'utf-8' )
    # print document
    if not os.path.exists(os.path.dirname(DOXYGEN_HTML_PATH + 'search_index.js')):
        os.makedirs(os.path.dirname(DOXYGEN_HTML_PATH + 'search_index.js'))
    with codecs.open(DOXYGEN_HTML_PATH + 'search_index.js', "w", "UTF-8") as outFile:
        outFile.write(document)

def add_to_search_index(html, save_path, tags=[]):
    global g_search_index

    if not g_search_index:
        g_search_index = {"data": []}

    search_obj = {"id": None, "title": None, "tags": []}
    search_obj["id"] = len(g_search_index["data"])
    search_obj["title"] = html.head.find("title").text if html.head.find("title") else ""
    search_obj["link"] = save_path
    # content in container tag
    body = None
    if html.body.find(id="container"):
        content = ""
        try:
            content = html.body.find(id="container").get_text(" ", strip=True)
            content.replace("\t", "")
            content.replace("\n", "")
        except:
            print "can't add body content to search index"
        body = content.encode('utf-8')

    # search_obj["body"] = body
    search_obj["tags"] = tags
    g_search_index["data"].append(search_obj)


def process_file(in_path, out_path=None):
    """ Generate documentation for a single file

        Args:
            inPath: The file to process
            outPath: The file to save the generated html file to
    """
    file_path = in_path
    file_prefix = get_file_prefix(file_path)
    is_html_file = True if get_file_extension(file_path).lower() == ".html" else False

    if is_html_file:
        file_path = "/".join(in_path.split('htmlsrc/')[1:])
        save_path = out_path if out_path is not None else DOXYGEN_HTML_PATH + file_path
    else:
        save_path = out_path if out_path is not None else DOXYGEN_HTML_PATH + get_file_prefix(in_path) + ".html"

    if is_html_file:
        process_html_file(in_path, save_path)

    else:
        # process html directory always, since they may generate content for class or namespace reference pages
        if not PROCESSED_HTML_DIR:
            process_html_dir(HTML_SOURCE_PATH, DOXYGEN_HTML_PATH)

        if file_prefix.startswith("class") or file_prefix.startswith("struct") or file_prefix.startswith("interface"):
            process_class_xml_file(sys.argv[1], os.path.join(DOXYGEN_HTML_PATH, save_path),
                                   copy.deepcopy(classTemplateHtml))

        elif file_prefix.startswith("namespace"):
            process_namespace_xml_file(in_path, os.path.join(DOXYGEN_HTML_PATH, save_path),
                                       copy.deepcopy(namespaceTemplateHtml))


def process_dir(in_path, out_path):
    """ Iterates a directory and generates documentation for each xml file
        in the directory as long as it is a class, struct or namespace

        Args:
            inPath: The directory to process
            outPath: The directory to save the generated html file to
    """

    for file_path in os.listdir(in_path):
        if file_path.endswith(".xml"):
            # name minus file extension
            file_prefix = get_file_prefix(file_path)

            # process class and struct files
            if file_path.startswith("class") or file_path.startswith("struct"):
                process_class_xml_file(os.path.join(in_path, file_path), os.path.join(out_path, file_prefix + ".html"),
                                       copy.deepcopy(classTemplateHtml))

            # process namespace files
            elif file_path.startswith("namespace"):
                process_namespace_xml_file(os.path.join(in_path, file_path), os.path.join(out_path, file_prefix + ".html"),
                                           copy.deepcopy(namespaceTemplateHtml))

            else:
                print "SKIPPING: " + file_path
                # TODO: base template and just iterate do an html iteration

    # save search index to json file
    # print g_search_index
    write_search_index()


def process_html_dir(in_path, out_path):
    print "PROCESS HTML DIR"

    for path, subdirs, files in os.walk(in_path):
        path_dir = path.split("/")[-1]
        if path_dir == "_templates" or path_dir == "assets":
            continue
        for name in files:
            # file_prefix = get_file_prefix(name)
            file_ext = get_file_extension(name).lower()
            if file_ext == ".html":

                if path.endswith('/'):
                    src_path = path[:-1]
                else:
                    src_path = path

                src_path = src_path + "/" + name
                process_file(src_path)


# def copyFiles( HTML_SOURCE_PATH, DOXYGEN_HTML_PATH ):
def copy_files():
    src = HTML_SOURCE_PATH
    dest = DOXYGEN_HTML_PATH

    try:
        copytree(src, dest, ignore=shutil.ignore_patterns("_templates*", "*.html"))
    except OSError as e:
        print('Directory not copied. Error: %s' % e)


# from http://stackoverflow.com/a/22331852/680667
def copytree(src, dst, symlinks=False, ignore=None):
    """ Copies all of the files from the source directory
        to a destination directory. Pass in anything that should be ignored.
    """

    if not os.path.exists(dst):
        os.makedirs(dst)
        shutil.copystat(src, dst)
    lst = os.listdir(src)

    # make list of files and directories minus the ignored stuff
    if ignore:
        excl = ignore(src, lst)
        lst = [x for x in lst if x not in excl]

    for item in lst:
        s = os.path.join(src, item)
        d = os.path.join(dst, item)

        if symlinks and os.path.islink(s):
            if os.path.lexists(d):
                os.remove(d)
                os.symlink(os.readlink(s), d)
            try:
                st = os.lstat(s)
                mode = stat.S_IMODE(st.st_mode)
                os.lchmod(d, mode)
            except:
                pass  # lchmod not available
        elif os.path.isdir(s):
            copytree(s, d, symlinks, ignore)
        else:
            shutil.copy2(s, d)


if __name__ == "__main__":
    """ Main Function for generating html documentation from doxygen generated xml files

    Args:
        -   No arguments generates all Cinder docs. Expects Doxygen to have been run previously.
        -   Can pass in a single xml file to process by passing in path to xml file
            and optionally, the resulting html file.
            if no out path is supplied, outputs to DOXYGEN_HTML_PATH
            Ex: python xmlToHtml.py xml/classcinder_1_1_surface_t.xml
        -   Can alternatively pass in a directory to process by providing the xml directory
            Ex: python xmlToHtml.py xml/ html/
    """

    # Make sure we're compiling using pythong 2.7.6+
    version_info = sys.version_info
    #if version_info.major >= 2 and version_info.minor >= 7 and version_info.micro < 6:
    #    sys.exit("ERROR: Sorry buddy, you must use python 2.7.6+ to generate documentation. Visit https://www.python.org/downloads/ to download the latest.")
    # if sys.version

    # Load tag file
    print "parsing tag file"
    g_tag_xml = Et.ElementTree(Et.parse("doxygen/cinder.tag").getroot())
    # generate symbol map from tag file
    g_symbolMap = get_symbol_to_file_map()

    # construct full template out of multiple html templates
    classTemplateHtml = construct_template(
        ["headerTemplate.html", "mainNavTemplate.html", "cinderClassTemplate.html", "footerTemplate.html"])

    namespaceTemplateHtml = construct_template(
        ["headerTemplate.html", "mainNavTemplate.html", "cinderNamespaceTemplate.html", "footerTemplate.html"])

    # copy files from htmlsrc/ to html/
    copy_files()

    # generate namespace navigation
    g_namespaceNav = generate_namespace_nav(classTemplateHtml)

    if len(sys.argv) == 1: # no args; run all docs
        process_html_dir(HTML_SOURCE_PATH, "html/")
        process_dir("xml/", "html/")
    elif len(sys.argv) == 2:
        inPath = sys.argv[1]
        # process a specific file
        if os.path.isfile(inPath):
            process_file(inPath, sys.argv[2] if len(sys.argv) > 2 else None)
    else:
        print "Unknown usage"
