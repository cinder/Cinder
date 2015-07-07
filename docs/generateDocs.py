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
from difflib import SequenceMatcher as SM
from bs4 import BeautifulSoup, Tag, NavigableString
from pystache.renderer import Renderer

BASE_PATH = os.path.dirname(os.path.realpath(__file__)) + os.sep
XML_SOURCE_PATH = BASE_PATH + 'xml' + os.sep
DOXYGEN_HTML_PATH = BASE_PATH + 'html' + os.sep
HTML_SOURCE_PATH = BASE_PATH + 'htmlsrc' + os.sep
TEMPLATE_PATH = BASE_PATH + 'htmlsrc' + os.sep + "_templates" + os.sep
PARENT_DIR = BASE_PATH.split('/docs')[0]
GITHUB_PATH = "http://github.com/cinder/Cinder/tree/master"

# state vars
PROCESSED_HTML_DIR = False


class Config(object):
    def __init__(self):
        # skip html directory parsing - speeds up debugging
        self.SKIP_HTML_PARSING = False
        # break on errors that would prevent the file from being generated
        self.BREAK_ON_STOP_ERRORS = False
        # directory for the class template mustache file
        self.CLASS_TEMPLATE = os.path.join(TEMPLATE_PATH, "class_template.mustache")
        # directory for the namespace template mustache file
        self.NAMESPACE_TEMPLATE = os.path.join(TEMPLATE_PATH, "namespace_template.mustache")
        # default html template mustache file
        self.HTML_TEMPLATE = os.path.join(TEMPLATE_PATH, "default_template.mustache")
        # guide html template mustache file
        self.GUIDE_TEMPLATE = os.path.join(TEMPLATE_PATH, "guide_template.mustache")
        # reference html template mustache file
        self.REFERENCE_TEMPLATE = os.path.join(TEMPLATE_PATH, "reference_template.mustache")

        self.HOME_TEMPLATE = os.path.join(TEMPLATE_PATH, "home-template.mustache")


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
config = Config()


# ================================================== START SYMBOL MAP ==================================================

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

        def add_related_link(self, link_data):
            self.relatedLinks.append(link_data)

        def define_prefix(self, path):
            self.prefixPath = path

        def add_type_def(self, type_def_obj):
            self.type_defs.append(type_def_obj)
            # add typedef string to search tags
            self.tags.append(strip_compound_name(type_def_obj.name))

        def add_function(self, fn_name, fn_obj):
            self.functionList.append(fn_obj)
            self.tags.append(fn_name)

    class Namespace(object):
        def __init__(self, name, file_name):
            self.name = name
            self.path = file_name
            self.functionList = []
            self.tags = []
            self.tags.append(self.name)

        def add_function(self, fn_name, fn_obj):
            self.functionList.append(fn_obj)
            self.tags.append(fn_name)

    class Typedef(object):
        def __init__(self, name, type_def, path):
            self.name = name
            self.type = type_def
            self.path = path
            self.sharedFrom = None

    class Function(object):
        def __init__(self, member_tree, base_class=None):
            anchor = member_tree.find("anchor").text
            self.name = member_tree.find("name").text
            self.base = base_class
            self.path = member_tree.find("anchorfile").text + "#" + anchor
            self.args = parse_arg_list(member_tree.find("arglist").text)

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

    def add_function(self, ns, fn_name, fn_obj):
        self.functions[ns + "::" + fn_name] = fn_obj
        # self.functionList.append(fn_obj)


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

    def find_function(self, name, argstring=""):

        # find function name without namespace and parenthesis
        fn_name = strip_compound_name(name.split('(')[0])

        # find args and amt of args
        args = parse_arg_list(str(argstring))
        arg_len = len(args)

        # non-optional arguments for the function
        req_arg_len = 0
        for arg in args:
            if arg.find("=") < 0:
                req_arg_len += 1

        # find parent class first
        class_parts = name.split("(")[0].split("::")
        class_name = "::".join(class_parts[:-1])
        ref_obj = g_symbolMap.find_class(class_name)

        # if we can't find a matching function, try a namespace
        if ref_obj is None:
            ns_search = class_name
            if class_name == "":
                ns_search = "cinder"
            ref_obj = g_symbolMap.find_namespace(ns_search)

        # iterate through class/namespace functions
        fn_list = []
        if ref_obj:
            for fn in ref_obj.functionList:
                if re.match(fn.name, fn_name):
                    fn_list.append(fn)
        # else:
        #     for fn_key in self.functions:
        #         # print self.func
        #         # print self.functions[fn].name
        #         fn = self.functions[fn_key]
        #         if fn.name == fn_name:
        #             fn_list.append(fn)
        #             print "found match"
        #             print fn.args

        # no functions found in class or namespaces, try search by name
        if len(fn_list) == 0:
            fn_obj = self.functions.get(fn_name)
            fn_list.append(fn_obj)

        fn_index = 0
        # if we have a bunch of options, we want to widdle it down to the best one
        if len(fn_list) > 1:
            best_score = 0

            for idx, fn in enumerate(fn_list):
                # fn_arg_len = len(fn.args)
                score = 0

                # amount of required arguments
                fn_arg_len = 0
                for arg in fn.args:
                    if arg.find("=") < 0:
                        fn_arg_len += 1

                # if number of args is greater than required, score goes up for less difference
                if arg_len == fn_arg_len:
                   score += 1.0

                fn_args = fn.args[0:fn_arg_len]
                if len(fn_args) > 0:
                    for i, arg in enumerate(fn_args):
                        if i + 1 > len(args):
                            continue
                        score += (SM(None, arg, args[i]).ratio()) * 2.0

                if score > best_score:
                    fn_index = idx
                    best_score = score

        found_function = fn_list[fn_index] if len(fn_list) > 0 else None
        return found_function

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


# --------------------------------------------------- END SYMBOL MAP ---------------------------------------------------

class LinkData(object):

    def __init__(self, link=None, label=None):
        self.link = link
        self.label = label


def convert_rel_path(link, src_dir, dest_dir):
    """
    Converts a relative path from one directory to another
    :param link: Link to convert
    :param src_dir: current relative directory
    :param dest_dir: destination relative directory
    :return: link with new relative path
    """

    if link.startswith("http") or link.startswith("javascript:"):
        return link

    # if a relative path, make it absolute
    if src_dir.find(BASE_PATH) < 0:
        src_dir = BASE_PATH + src_dir

    # get absolute in path, converted from htmlsrc to html
    abs_src_path = urlparse.urljoin(src_dir, link).replace("htmlsrc", "html")
    # destination absolute path
    abs_dest_path = os.path.join(os.path.dirname(dest_dir), link)

    new_link = os.path.relpath(abs_dest_path, os.path.dirname(abs_src_path))
    return new_link


class FileData(object):

    def __init__(self, tree):

        self.tree = tree  # xml file that describes the page
        self.bs4 = None  # html file of the actual page

        self.name = ""
        self.title = ""
        self.page_header = ""

    def get_content(self):
        content = {
            "name": self.name,
            "title": self.title,
            "page_header": self.page_header
        }
        return content


class ClassFileData(FileData):

    def __init__(self, tree):
        FileData.__init__(self, tree)
        self.description = ""
        self.is_template = False
        self.template_def_name = ""
        self.includes = None
        self.typedefs = []
        self.classes = []
        self.related = []
        self.namespace_nav = None
        self.prefix = ""
        self.enumerations = []
        self.public_functions = []
        self.public_types = []
        self.public_static_functions = []
        self.anchors = []
        self.protected_functions = []
        self.protected_attrs = []
        self.class_hierarchy = None
        self.friends = []

        # fill compound name (with namespace if present)
        self.compoundName = str(find_compound_name(tree))
        self.stripped_name = strip_compound_name(self.compoundName)

        # stripped name (w/o namespace)
        name_parts = self.compoundName.rsplit("cinder::", 1)
        if len(name_parts) > 1:
            self.name = name_parts[1]  # without "cinder::"
        else:
            self.name = name_parts[0]

        # kind of file that we are parsing (class, namespace, etc)
        self.kind = find_file_kind(tree)
        self.kind_explicit = find_file_kind_explicit(tree)

    def get_content(self):
        orig_content = super(ClassFileData, self).get_content()
        content = orig_content.copy()
        class_content = {
            "name": self.stripped_name,
            "description": self.description,
            "is_template": self.is_template,
            "template_def_name": self.template_def_name,
            "side_nav_content": {
                "include": self.includes,
                "typedefs": {
                    "list": self.typedefs,
                    "length": len(self.typedefs)
                },
                "class_hierarchy": self.class_hierarchy,
                "classes": {
                    "list": self.classes,
                    "length": len(self.classes)
                },
                "related": {
                    "list": self.related,
                    "length": len(self.related)
                }
            },
            "namespace_nav": self.namespace_nav,
            "prefix": self.prefix,
            "enumerations": {
                "anchor": "enumerations",
                "list": self.enumerations,
                "length": len(self.enumerations)
            },
            "public_functions": {
                "anchor": "public-member-functions",
                "list": self.public_functions,
                "length": len(self.public_functions)
            },
            "public_static_functions": {
                "anchor": "public-static-functions",
                "list": self.public_static_functions,
                "length": len(self.public_static_functions)
            },
            "protected_functions": {
                "anchor": "protected-functions",
                "list": self.protected_functions,
                "length": len(self.protected_functions)
            },
            "protected_attrs": {
                "anchor": "protected-attrs",
                "list": self.protected_attrs,
                "length": len(self.protected_attrs)
            },
            "public_types": {
                "anchor": "public-types",
                "list": self.public_types,
                "length": len(self.public_types)
            },
            "friends": {
                "anchor": "friends",
                "list": self.friends,
                "length": len(self.friends)
            }
        }
        content.update(class_content)
        return content


class NamespaceFileData(FileData):

    def __init__(self, tree):
        FileData.__init__(self, tree)

        # stripped name (w/o namespace)
        self.compoundName = str(find_compound_name(tree))
        self.name = self.compoundName

        self.namespaces = []
        self.classes = []
        self.typedefs = []
        self.enumerations = []
        self.functions = []
        self.variables = []
        self.namespace_nav = None
        self.kind = find_file_kind(tree)

    def get_content(self):
        orig_content = super(NamespaceFileData, self).get_content()
        content = orig_content.copy()
        ns_content = {
            "namespace_nav": self.namespace_nav,
            "namespaces": {
                "anchor": "namespaces",
                "list": self.namespaces,
                "length": len(self.namespaces)
            },
            "classes": {
                "anchor": "classes",
                "list": self.classes,
                "length": len(self.classes)
            },
            "typedefs": {
                "anchor": "typedefs",
                "list": self.typedefs,
                "length": len(self.typedefs)
            },
            "enumerations": {
                "anchor": "enumerations",
                "list": self.enumerations,
                "length": len(self.enumerations)
            },
            "public_functions": {
                "anchor": "functions",
                "list": self.functions,
                "length": len(self.functions)
            },
            "variables": {
                "anchor": "variables",
                "list": self.variables,
                "length": len(self.variables)
            }
        }
        content.update(ns_content)
        return content


class HtmlFileData(FileData):

    def __init__(self):
        FileData.__init__(self, None)

        self.html_content = ""

    def get_content(self):
        orig_content = super(HtmlFileData, self).get_content()
        content = orig_content.copy()
        template_content = {
            "html_content": self.html_content
        }
        content.update(template_content)
        return content


def find_compound_name(tree):
    for compound_def in tree.iter("compounddef"):
        for compound_name in compound_def.iter("compoundname"):
            return compound_name.text


def find_file_kind(tree):
    kind = tree.find(r"compounddef").attrib['kind']
    return kind


def find_member_anchor(member):
    """
    Parses out the anchor tag from a member
    """
    anchor_str = member.attrib["id"].split("_1")[-1]
    return anchor_str


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


def parse_arg_list(arg_string):

    # replace any commas in < and > enclosures with a temporary delim *** so that they
    # don't get in the way when splitting args
    arg_list = re.sub(r'(<\s\S*)(,)(\s\S* *>)', r'\1***\3', arg_string)
    # split the args into a list
    args = arg_list[1:-1].split(', ')

    # strip white space
    args = map(str.strip, args)
    stripped_args = []

    for indx, arg in enumerate(args):
        is_optional = arg.find("=") > -1

        # if there is more than one word, take the last one off
        if len(arg.split(" ")) > 1:
            arg = " ".join(arg.split(" ")[:-1])

        # we only want the new list to include required args
        if not is_optional:
            # replace the temp delimeter with a comma again
            arg = arg.replace("***", ",")
            stripped_args.append(arg)
    # filter empty strings
    stripped_args = filter(None, stripped_args)

    return stripped_args

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


def gen_rel_link_tag(bs4, text, link, src_dir, dest_dir):
    """
    Generates a link tag that was relative to the source directory, but should now be relative to the destination directory
    :param bs4: beautifulsoup instance
    :param text: text of link
    :param link: relative link
    :param src_dir: original source directory
    :param dest_dir: destination source directory
    :return: the link tag
    """
    new_link = convert_rel_path(link, src_dir, dest_dir)
    link_tag = gen_link_tag(bs4, text, new_link)
    return link_tag


def extract_anchor(element):
    if element.attrib["id"]:
        return element.attrib["id"].split("_1")[-1]
    else:
        return None


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
    else:
        tag["href"] = href


def parse_member_definition(bs4, member, member_name=None):
    """
    Parses a function tree and generates an object out of it
    :param bs4: beautifulsoup instance
    :param member: the member to parse
    :param class_name: the name of the class that's being parsed
    :return: the data object
    """
    if not member_name:
        member_name = member.find(r"name")
        member_name = member_name.text if member_name is not None else None

    anchor = find_member_anchor(member)

    # return type
    return_div = gen_tag(bs4, "span")
    return_str = str(iterate_markup(bs4, member.find(r"type"), return_div))

    # get args
    argstring = member.find("argsstring")
    if argstring is None:
        argstring = member.find("arglist")
    argstring_text = argstring.text if argstring is not None else ""

    # description
    description_div = markup_description(bs4, member)
    description_str = str(description_div) if description_div is not None else None

    member_obj = {
        "name": member_name,
        "return": return_str,
        "anchor": anchor,
        "definition": {
            "name": member_name,
            "args": argstring_text
        },
        "description": description_str
    }

    return member_obj

def parse_function(bs4, member, class_name=None):

    member_name = member.find(r"name")
    member_name = member_name.text if member_name is not None else None
    is_constructor = False

    # determine if it is a constructor
    if class_name is not None:
        if member_name is not None and member_name == strip_compound_name(class_name):
            is_constructor = True

    member_obj = parse_member_definition(bs4, member, member_name)
    member_obj["is_constructor"] = is_constructor
    return member_obj


def parse_enum(bs4, member):

    member_obj = parse_member_definition(bs4, member)
    values = []
    for val in member.findall("enumvalue"):
        enum_name = val.find("name").text
        values.append({"name": enum_name})

    member_obj["values"] = values
    member_obj["return"] = "enum"
    return member_obj


def define_tag(bs4, tag_name, tree):

    if tag_name == "a":
        new_tag = bs4.new_tag(tag_name)
        define_link_tag(new_tag, tree.attrib)
        # creates a new tag with a relative link using the data from the original tag
        # TODO: refactor define_tag and ren_link_tags. Should be able to create relative link on its own
        new_tag = gen_rel_link_tag(bs4, "", new_tag["href"], TEMPLATE_PATH, DOXYGEN_HTML_PATH)
    else:
        new_tag = bs4.new_tag(tag_name)
    return new_tag


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
    """ Generates the class hierarchy side bar, with each class linking
        out to its class file.

    Args:
        bs4: The current beautifulSoup html instance
        classDef: The instance of SymbolMap::Class Object that we are generating
            the hierachy for

    Returns:
        Empty if there is no base class
        Ul if there is hierarchy
    """

    if class_def is None:
        return

    # first item in the list will be the original class
    hierarchy = []

    # get the class' hierarchy
    iter_class_base(class_def, hierarchy)
    hierarchy.append(class_def)

    if len(hierarchy) == 1:
        return

    # create all of the markup
    ul = gen_tag(bs4, "ul")
    add_class_to_tag(ul, "inheritence")

    # go through the hierarchy and add a list item for each member
    # for index, base in enumerate(reversed(hierarchy)):
    for index, base in enumerate(hierarchy):
        li = gen_tag(bs4, "li")
        add_class_to_tag(li, "depth" + str(index + 1))

        # link out only if a base class, not the original class
        if index < len(hierarchy) - 1:
            a = gen_tag(bs4, "a", [], base.name)
            define_link_tag(a, {'href': base.path})
            a = gen_rel_link_tag(bs4, base.name, a["href"], TEMPLATE_PATH, DOXYGEN_HTML_PATH)
            li.append(a)
        else:
            li.append(base.name)
        ul.append(li)

    return ul


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

    # add content to tag as is ( no stripping of whitespace )
    if tree.text is not None:
        content = tree.text

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


def inject_html(src_content, dest_el, src_path, dest_path):
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
        a_tag = gen_rel_link_tag(bs4, name, ns.path, TEMPLATE_PATH, DOXYGEN_HTML_PATH)
        # a_tag = gen_tag(bs4, "a")
        # define_link_tag(a_tag, {"href": ns.path})
        # a_tag.append(name)

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


def generate_namespace_nav():
    """
    Creates a div filled with a list of namespace links
    :param bs4: The Beautiful soup instance used for dom manipulation
    :return: a new div that contains the navigation tree
    """
    bs4 = BeautifulSoup()
    namespaces = g_symbolMap.get_whitelisted_namespaces()

    tree = gen_tag(bs4, "div")
    ul = gen_tag(bs4, "ul")
    tree.append(ul)
    add_class_to_tag(tree, "css-treeview")

    iterate_namespace(bs4, namespaces, ul, 0, "")
    return tree


def process_class_xml_file(in_path, out_path):
    global g_currentFile
    print "Processing file: " + in_path + " > " + out_path

    # define the tree that contains all the data we need to populate this page
    tree = parse_xml(in_path)
    bs4 = BeautifulSoup()

    if tree is None:
        return

    # get common data for the file
    file_data = ClassFileData(tree)
    g_currentFile = file_data
    include_file = ""
    include_tag = tree.find(r"compounddef/includes")
    if include_tag is not None:
        include_file = include_tag.text
    class_name = g_currentFile.name
    file_def = g_symbolMap.find_file(include_file)
    class_def = g_symbolMap.find_class(class_name)
    # kind = g_currentFile.kind_explicit

    # class template stuff ------------------------------ #
    file_data.is_template = True if tree.find(r"compounddef/templateparamlist") is not None else False
    if file_data.is_template:
        try:
            # print Et.dump(tree.find(r"compounddef/templateparamlist/param"))
            def_name = tree.find(r"compounddef/templateparamlist/param/defname")
            file_data.template_def_name = def_name.text if def_name is not None else ""
        except:
            file_data.template_def_name = ""

    if not class_def:
        print "\t** Warning: NO CLASS OBJECT DEFINED FOR: " + class_name
        return

    # page title ---------------------------------------- #
    file_data.title = file_data.name

    # add namespace nav --------------------------------- #
    file_data.namespace_nav = str(g_namespaceNav)

    # page header --------------------------------------- #
    file_data.page_header = file_data.compoundName

    # add description ----------------------------------- #
    description = markup_description(bs4, tree.find(r'compounddef'))
    file_data.description = str(description) if description is not None else ""

    # includes ------------------------------------------ #
    include_link = None

    if include_file:
        file_obj = g_symbolMap.find_file(include_file)
        if file_obj:
            include_link = LinkData(file_obj.path, include_file)
    file_data.includes = include_link

    # typedefs ------------------------------------------ #
    typedefs = []
    if file_def is not None:
        for t in file_def.typedefs:
            link_data = LinkData()
            link_data.label = t.name
            link_data.link = convert_rel_path(t.path, TEMPLATE_PATH, DOXYGEN_HTML_PATH)
            typedefs.append(link_data)
    file_data.typedefs = typedefs

    # class hierarchy ----------------------------------- #
    class_hierarchy = gen_class_hierarchy(bs4, class_def)
    file_data.class_hierarchy = str(class_hierarchy) if class_hierarchy else None

    # class list ---------------------------------------- #
    classes = []
    for classDef in tree.findall(r"compounddef/innerclass"):
        link_data = LinkData()
        link_data.label = strip_compound_name(classDef.text)
        link_data.link = convert_rel_path(classDef.attrib["refid"] + ".html", TEMPLATE_PATH, DOXYGEN_HTML_PATH)
        classes.append(link_data)
    file_data.classes = classes

    # related links  ------------------------------------ #
    # generated by guides and references
    related = []
    for link_data in class_def.relatedLinks:
        related.append(link_data)
    file_data.related = related

    # ci prefix / description --------------------------- #
    # if the class has a prefix, add it here
    if hasattr(class_def, 'prefixPath') is True and class_def.prefixPath is not None:
        prefix_html = generate_bs4(class_def.prefixPath).body
        prefix_content = ""
        for c in prefix_html.contents:
            prefix_content += str(c)
        file_data.prefix = prefix_content

    # enumerations -------------------------------------- #
    enumerations = []
    for e in tree.findall(r"compounddef/sectiondef/memberdef[@kind='enum']"):
        member_obj = parse_enum(bs4, e)
        enumerations.append(member_obj)
    file_data.enumerations = enumerations

    # TODO: Look into and re-evaluate if this is needed or not since the definitions are all over the map and may be an edge case
    # public types -------------------------------------- #
    # public_types = []
    # # for member in tree.findall(r"compounddef/sectiondef/memberdef[@kind='typedef']"):
    # for member in tree.findall(r"compounddef/sectiondef[@kind='public-type']/memberdef[@prot='public']"):
    #
    #     member_obj = None
    #     print member.attrib["kind"]
    #     if member.attrib["kind"] == "enum":
    #         member_obj = parse_member_definition(bs4, member)
    #         member_obj["return"] = "enum"
    #         enum_link = gen_link_tag(bs4, member_obj["name"], "#"+find_member_anchor(member))
    #         member_obj["definition"]["name"] = str(enum_link)
    #     else:
    #         member_obj = parse_function(bs4, member, class_name)
    #     print member.attrib["kind"]
    #     print member.find("name").text
    #
    #     if member_obj is None:
    #         continue
    #
    #     public_types.append(member_obj)
    #
    # file_data.public_types = public_types

    # public member Functions --------------------------- #
    public_fns = []
    public_static_fns = []
    for memberFn in tree.findall(r'compounddef/sectiondef/memberdef[@kind="function"][@prot="public"]'):

        function_obj = parse_function(bs4, memberFn, class_name)
        is_static = memberFn.attrib["static"]

        if is_static == 'yes':
            public_static_fns.append(function_obj)
        else:
            public_fns.append(function_obj)

    file_data.public_functions = public_fns
    file_data.public_static_functions = public_static_fns

    # protected member functions ------------------------ #
    protected_functions = []
    for member in tree.findall(r'compounddef/sectiondef/memberdef[@kind="function"][@prot="protected"]'):
        function_obj = parse_function(bs4, member, class_name)
        protected_functions.append(function_obj)
    file_data.protected_functions = protected_functions

    # protected attributes ------------------------------ #
    protected_attrs = []
    for v in tree.findall(r'compounddef/sectiondef/memberdef[@kind="variable"][@prot="protected"]'):
        member_obj = parse_member_definition(bs4, v)
        protected_attrs.append(member_obj)
    file_data.protected_attrs = protected_attrs

    # friends ------------------------------------------- #
    friends = []
    for member in tree.findall(r'compounddef/sectiondef/memberdef[@kind="friend"]'):
        member_obj = parse_member_definition(bs4, member)

        # replace name with link to class
        friend_class = g_symbolMap.find_class(member_obj["name"])

        # link up friend, if class exists
        if friend_class:
            friend_link = gen_rel_link_tag(bs4, friend_class.name, friend_class.path, TEMPLATE_PATH, DOXYGEN_HTML_PATH)
            member_obj["definition"]["name"] = str(friend_link)
        friends.append(member_obj)
    file_data.friends = friends

    # Generate the html file from the template and inject content
    bs4 = render_template(config.CLASS_TEMPLATE, file_data.get_content())
    if not bs4:
        print "\t** ERROR: Skipping class due to something nasty. Bother Greg and try again some other time. Error rendering: " + in_path
        return

    # print output
    # update links in the template
    update_links(bs4, TEMPLATE_PATH + "htmlContentTemplate.html", out_path)

    # replace any code chunks with <pre> tags, which is not possible on initial creation
    replace_code_chunks(bs4)

    # link up all ci tags
    for tag in bs4.find_all('ci'):
        process_ci_tag(bs4, tag, in_path, out_path)

    # add file to search index
    search_tags = []
    if class_def:
        search_tags = class_def.tags

    link_path = gen_rel_link_tag(bs4, "", out_path, HTML_SOURCE_PATH, DOXYGEN_HTML_PATH)["href"]
    add_to_search_index(bs4, link_path, search_tags)

    # # write the file
    write_html(bs4, out_path)


def drop_anchor(anchor_list, anchor_name, link_name):
    bs4 = g_currentFile.bs4
    anchor = gen_anchor_tag(bs4, anchor_name)
    anchor_list.append({"name": link_name, "link": anchor})
    g_currentFile.contentsEl.append(anchor)


def list_namespaces(bs4):
    namespaces = g_symbolMap.get_ordered_namespaces()

    ul = gen_tag(bs4, "ul")
    for ns in namespaces:
        a = gen_link_tag(bs4, ns.name, ns.path)
        li = gen_tag(bs4, "li", None, a)
        ul.append(li)

    return ul


def list_classes(bs4):
    classes = g_symbolMap.get_ordered_class_list()

    ul = gen_tag(bs4, "ul", ["master-class-list"])
    for c in classes:
        a = gen_link_tag(bs4, c.name, c.path)
        li = gen_tag(bs4, "li", None, a)
        ul.append(li)

    return ul


def process_namespace_xml_file(in_path, out_path):
    global g_currentFile
    print "Processing namespace file: " + in_path + " > " + out_path

    # define the tree that contains all the data we need to populate this page
    tree = parse_xml(in_path)
    bs4 = BeautifulSoup()

    if tree is None:
        return

    # get common data for the file
    file_data = NamespaceFileData(tree)
    g_currentFile = file_data
    ns_def = g_symbolMap.find_namespace(file_data.name)

    # page title ---------------------------------------- #
    file_data.title = file_data.name

    # add namespace nav --------------------------------- #
    file_data.namespace_nav = str(g_namespaceNav)

    # add namespaces ------------------------------------ #
    namespaces = []
    for member in tree.findall(r"compounddef/innernamespace"):
        link = convert_rel_path(member.attrib["refid"] + ".html", TEMPLATE_PATH, DOXYGEN_HTML_PATH)
        link_data = LinkData(link, member.text)
        namespaces.append(link_data)
    file_data.namespaces = namespaces

    # add classes ------------------------------------ #
    classes = []
    for member in tree.findall(r"compounddef/innerclass"):
        link = member.attrib["refid"] + ".html"
        rel_link = convert_rel_path(link, TEMPLATE_PATH, DOXYGEN_HTML_PATH)
        link_data = LinkData(rel_link, member.text)

        kind = "struct" if link.startswith("struct") else "class"
        class_obj = {
            "link_data": link_data,
            "kind": kind
        }
        classes.append(class_obj)
    file_data.classes = classes

    # add typedefs -------------------------------------- #
    typedefs = []
    for member in tree.findall(r"compounddef/sectiondef/[@kind='typedef']/memberdef/[@kind='typedef']"):
        typedef_obj = parse_member_definition(bs4, member)
        typedefs.append(typedef_obj)
    file_data.typedefs = typedefs

    # add enumerations ---------------------------------- #
    enumerations = []
    for member in tree.findall(r"compounddef/sectiondef/[@kind='enum']/memberdef/[@kind='enum']"):
        member_obj = parse_enum(bs4, member)
        enumerations.append(member_obj)
    file_data.enumerations = enumerations

    # functions ----------------------------------------- #
    fns = []
    for member in tree.findall(r"compounddef/sectiondef/[@kind='func']/memberdef/[@kind='function']"):
        function_obj = parse_member_definition(bs4, member)
        fns.append(function_obj)
    file_data.functions = fns

    # variables ----------------------------------------- #
    variables = []
    for member in tree.findall(r"compounddef/sectiondef/[@kind='var']/memberdef/[@kind='variable']"):
        var_obj = parse_member_definition(bs4, member)
        initializer = member.find('initializer').text if member.find('initializer') is not None else None
        var_obj["definition"]["args"] = initializer
        variables.append(var_obj)
    file_data.variables = variables

    bs4 = render_template(config.NAMESPACE_TEMPLATE, file_data.get_content())

    # update links in the template
    update_links(bs4, TEMPLATE_PATH + "htmlContentTemplate.html", out_path)

    # add file to search index
    search_tags = ["namespace"]
    if ns_def:
        search_tags = ns_def.tags

    link_path = gen_rel_link_tag(bs4, "", out_path, HTML_SOURCE_PATH, DOXYGEN_HTML_PATH)["href"]

    # add file to search index
    add_to_search_index(bs4, link_path, search_tags)

    # write the file
    write_html(bs4, out_path)


def process_html_file(in_path, out_path):
    """ Parses an html file.
    - Adds template around the html
    - Copy original css and js links into new hmtl
    - Save html in destination dir
    """
    print "processHtmlFile: " + in_path

    # get common data for the file
    file_data = HtmlFileData()
    g_currentFile = file_data
    is_searchable = False
    search_tags = []

    # get correct template
    template = config.HTML_TEMPLATE
    if in_path.find("htmlsrc/index.html") > -1:
        template = config.HOME_TEMPLATE
    elif in_path.find("reference/") > -1:
        template = config.REFERENCE_TEMPLATE
    elif in_path.find("guides/") > -1:
        template = config.GUIDE_TEMPLATE

    # FILL CONTENT

    # if index
    # if in_path.find("htmlsrc/index.html") > -1:
    #     bs4 = generate_bs4(in_path)
    # else:

    orig_html = generate_bs4(in_path)
    # update_links(orig_html, in_path, out_path)

    # fill namespace list
    if in_path.find("htmlsrc/namespaces.html") > -1:
        # throw in a list of namespaces into the page
        ns_list = list_namespaces(orig_html)
        orig_html.body.append(ns_list)

    # fill class list
    elif in_path.find("htmlsrc/classes.html") > -1:
        class_list = list_classes(orig_html)
        orig_html.body.append(class_list)

    else:
        # add file to search index
        is_searchable = True
        body_content = ""
        for content in orig_html.body.contents:
            body_content += str(content)

    # copy title over
    if orig_html.head and orig_html.head.title:
        file_data.title = orig_html.head.title.text

    bs4 = render_template(template, file_data.get_content())
    update_links(bs4, TEMPLATE_PATH + "guidesContentTemplate.html", out_path)

    template_content_el = bs4.body.find(id="template-content")

    # inject html into a template content div
    inject_html(orig_html, template_content_el, in_path, out_path)

    if bs4 is None:
        print "\t** ERROR: Error generating file, so skipping: " + in_path
        return

    # copy all js and css paths that may be in the original html and paste into new file
    for link in orig_html.find_all("link"):
        if bs4.head:
            bs4.head.append(link)

    for script in orig_html.find_all("script"):
        if bs4.body:
            bs4.body.append(script)

    if orig_html.head and bs4.head:

        for d in orig_html.head.find_all("ci"):
            bs4.head.append(d)

        # add tags from the meta keywords tag
        for meta_tag in orig_html.head.findAll(attrs={"name": "keywords"}):
            for keyword in meta_tag['content'].split(','):
                search_tags.append(keyword.encode('utf-8').strip())

    # link up all ci tags
    for tag in bs4.find_all('ci'):
        process_ci_tag(bs4, tag, in_path, out_path)

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
        replace_ci_tag(bs4, tag, in_path, out_path)


def replace_ci_tag(bs4, link, in_path, out_path):
    ref_obj = find_ci_tag_ref(link)

    if ref_obj:
        ref_location = DOXYGEN_HTML_PATH + ref_obj.path
        # new_link = gen_link_tag(bs4, link.contents[0], ref_location)
        new_link = gen_rel_link_tag(bs4, link.contents[0], ref_location, in_path, out_path)
        link.replace_with(new_link)
        # print "\tSuccess: " + str(new_link)
    else:
        print "\t** Warning: Could not find replacement tag for ci tag: " + str(link)


def process_ci_seealso_tag(bs4, tag, out_path):
    """
    Processes ci tag that is of 'seealso' type
    :param bs4: The active beautiful soup instance
    :param tag: the ci tag to find a reference for
    :param out_path: the file path
    :return: None
    """
    ref_obj = find_ci_tag_ref(tag)

    # get label attribute value if there is one
    if tag.has_attr("label"):
        label = tag["label"]
    # otherwise use the name of the file as the label
    else:
        label = get_file_name(out_path)

    # link_tag = gen_link_tag(bs4, label, out_path)
    link_data = LinkData(out_path, label)

    if type(ref_obj) is SymbolMap.Class or type(ref_obj) is SymbolMap.Typedef:
        ref_obj.add_related_link(link_data)

    elif type(ref_obj) is SymbolMap.Namespace:
        # find all classes with that namespace and add guide to every one
        for class_obj in g_symbolMap.find_classes_in_namespace(ref_obj.name):
            class_obj.add_related_link(link_data)
    else:
        print "\t** WARNING: Could not find seealso reference for " + str(tag)


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
    is_function = searchstring.find("(") > -1 or link.get('kind') == 'function'
    if is_function:
        arg_string = searchstring[searchstring.find("("):]
        if len(arg_string) == 0:
            arg_string = "()"

    try:
        # find function link
        if is_function:
            fn_obj = g_symbolMap.find_function(searchstring, arg_string)
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

    except Exception as e:
        print "\t** WARNING: problem finding ci tag"
        print "\t" + e.message
        return None

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

            try:
                shutil.copy2(src_file, dest_file)
            except IOError as e:
                print "\t ** ERROR: Cannot copy src_file because it doesn't exist"
                print e.strerror
                return
            except Exception as e:
                print "\t ** ERROR: Cannot copy iframe over because of some other error"
                print e.strerror
                return

    # coy over any iframe html files, if any


def update_link(link, in_path, out_path):
    """
    Update the given link to point to something relative to the new path
    :param link: The link to change
    :param in_path: the original path to the file that the link lives in
    :return:
    """

    if link.startswith("http") or link.startswith("javascript:") or link.startswith("#"):
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
    output_file.decode("utf-8")
    # print output_file

    # wrap in body tag if none exists
    if output_file.find("<body") < 0:
        output_file = "<body>" + output_file + "</body>"
        print "\t ** WARNING: No body tag found in file: " + file_path

    bs4 = BeautifulSoup(output_file)

    return bs4

def generate_bs4_from_string(string):
    string.decode("UTF-8")
    # print output_file

    # wrap in body tag if none exists
    if string.find("<body") < 0:
        output_file = "<body>" + string + "</body>"
        print "\t ** WARNING: No body tag found "

    bs4 = BeautifulSoup(string)
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
            # print "FUNCTION " + name + "::" + fn_name
            # function_obj = SymbolMap.Function(fn_name, base_class, args, file_path)
            function_obj = SymbolMap.Function(member, base_class)
            # symbol_map.functions[name + "::" + function_obj.name] = function_obj
            symbol_map.add_function(name, function_obj.name, function_obj)
            class_obj.add_function(function_obj.name, function_obj)

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
            # fn_name = member.find("name").text
            # anchor = member.find("anchor").text
            # file_path = member.find("anchorfile").text + "#" + anchor
            # args = member.find("argsstring").text if member.find("argsstring") else ""
            # function_obj = SymbolMap.Function(fn_name, base_class, args, file_path)
            function_obj = SymbolMap.Function(member, base_class)
            # symbol_map.functions[name + "::" + function_obj.name] = function_obj
            symbol_map.add_function(name, function_obj.name, function_obj)
            struct_obj.add_function(function_obj.name, function_obj)

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
            # fn_name = member.find("name").text
            # anchor = member.find("anchor").text
            # file_path = member.find("anchorfile").text + "#" + anchor
            # args = member.find("argsstring").text if member.find("argsstring") else ""
            # function_obj = SymbolMap.Function(fn_name, base_class, args, file_path)
            function_obj = SymbolMap.Function(member, base_class)
            ns_obj.functionList.append(function_obj)
            ns_obj.add_function(function_obj.name, function_obj)


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

        # print "FILE PATH: " + name + " | " + file_path
        symbol_map.files[name] = SymbolMap.File(name, file_path, typedefs)

        # find functions for each file
        for member in f.findall(r'member[@kind="function"]'):
            # fn_name = f.find("name").text
            # file_path = f.find('anchorfile').text + "#" + f.find("anchor").text
            # args = member.find("argsstring").text if member.find("argsstring") else ""
            # fn = SymbolMap.Function(fn_name, "", args, file_path)
            function_obj = SymbolMap.Function(member, "")
            # symbol_map.functions[function_obj.name] = function_obj
            symbol_map.add_function("", function_obj.name, function_obj)
    if len(file_tags) == 0:
        print "\t** Warning: no compound of type 'file' found in tag file. Check doxygen SHOW_FILES setting."

    return symbol_map


def get_file_prefix(file_path):
    return os.path.splitext(os.path.basename(file_path))[0]


def get_file_extension(file_path):
    return os.path.splitext(os.path.basename(file_path))[1]


def get_file_name(file_path):
    return os.path.basename(file_path)


def parse_xml(in_path):
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
        # bs4.append('COULD NOT PARSE FILE: ' + in_path)
        # write_html(bs4, out_path)
        print "-------------------\n"

    return tree


def write_html(html, save_path):

    # prettify descriptions
    for markup in html.find_all("div", "description"):
        if type(markup) is Tag:
            pretty = BeautifulSoup(markup.prettify())
            if pretty is not None and markup is not None:
                markup.replaceWith(pretty)

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
    with codecs.open(save_path, "w", "utf-8") as outFile:
        outFile.write(document)

def write_search_index():
    # save search index to js file
    document = "var search_index_data = " + json.dumps(g_search_index).encode('utf-8')
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
    search_obj["tags"] = tags
    g_search_index["data"].append(search_obj)


def render_template(path, content):

     # Generate the html file from the template and inject content
    renderer = Renderer()
    renderer.search_dirs.append(TEMPLATE_PATH)

    try:
        output = renderer.render_path(path, content)
    except:
        exc = sys.exc_info()[0]
        print "\t**--------------------------------"
        print "\t** Warning: cannot render template"
        print "\t**--------------------------------"
        print exc
        if config.BREAK_ON_STOP_ERRORS:
            raise
        else:
            return

    # print output
    # print "OUTPUT: " + output
    bs4 = generate_bs4_from_string(output)
    return bs4


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
        # print "process: " + HTML_SOURCE_PATH + file_path
        process_html_file(HTML_SOURCE_PATH + file_path, save_path)

    else:
        # process html directory always, since they may generate content for class or namespace reference pages
        if not PROCESSED_HTML_DIR and not config.SKIP_HTML_PARSING:
            process_html_dir(HTML_SOURCE_PATH, DOXYGEN_HTML_PATH)

        if file_prefix.startswith("class") or file_prefix.startswith("struct") or file_prefix.startswith("interface"):
            process_class_xml_file(sys.argv[1], os.path.join(DOXYGEN_HTML_PATH, save_path))

        elif file_prefix.startswith("namespace"):
            process_namespace_xml_file(in_path, os.path.join(DOXYGEN_HTML_PATH, save_path))


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
                process_class_xml_file(os.path.join(in_path, file_path), os.path.join(out_path, file_prefix + ".html"))

            # process namespace files
            elif file_path.startswith("namespace"):
                process_namespace_xml_file(os.path.join(in_path, file_path), os.path.join(out_path, file_prefix + ".html"))

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

    # copy files from htmlsrc/ to html/
    copy_files()

    # generate namespace navigation
    g_namespaceNav = generate_namespace_nav()

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
