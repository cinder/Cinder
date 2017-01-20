#! /usr/bin/python

"""
 Copyright (c) 2015, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.
z
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.

 AUTHOR: Greg Kepler | gkepler@gmail.com
"""

# -*- coding: utf-8 -*-
import sys
import codecs
import re
import xml.etree.ElementTree as ET
import json
import os
import shutil
import stat
import argparse
import posixpath
from datetime import datetime
from difflib import SequenceMatcher as SM
from posixpath import join as urljoin

# Third party in libs folder
sys.path.append("libs/")
from bs4 import BeautifulSoup, Tag, NavigableString, Comment
from pystache.renderer import Renderer, Loader

# static path vars
BASE_PATH = os.path.dirname(os.path.realpath(__file__)) + os.sep
HTML_ROOT_DIR = 'html'
XML_SOURCE_PATH = BASE_PATH + 'xml' + os.sep
HTML_DEST_PATH = BASE_PATH + HTML_ROOT_DIR + os.sep
HTML_SOURCE_PATH = BASE_PATH + 'htmlsrc' + os.sep
TEMPLATE_PATH = BASE_PATH + 'htmlsrc' + os.sep + "_templates" + os.sep
PARENT_DIR = BASE_PATH.split(os.sep + 'docs')[0]
TAG_FILE_PATH = "doxygen" + os.sep + "cinder.tag"

# TODO: These should be dynamic via doxygen generated data. perhaps from _cinder_8h.xml
file_meta = {
    "cinder_version": "",
    "doxy_version": "",
    "creation_date": str(datetime.today().date()),
    "docs_root": ""
}

parser = argparse.ArgumentParser(description='CiDocs')
parser.add_argument('path', nargs='?')
parser.add_argument('outpath', nargs='?')
parser.add_argument('-d', '--debug',
    action='store_true',
    help='show debug arguments')
parser.add_argument('-s', '--skiphtml',
    action='store_true',
    help='skip html generation')
parser.add_argument('--root',
    default=HTML_ROOT_DIR,
    help='server html root directory name')
parser.add_argument('--include-analytics',
    action='store_true',
    help='bool as to wheather to include analytics in frontend')


# various config settings
class Config(object):
    def __init__(self):
        # break on errors that would prevent the file from being generated
        self.BREAK_ON_STOP_ERRORS = True
        # whitelisted namespaces to generate pages for
        self.NAMESPACE_WHITELIST = [
            {
                "name": "cinder"
            },
            {
                "name": "glm",
                "structure_whitelist":
                [
                    {
                        "name": "typedefs",
                        "prefix_blacklist": ["lowp", "mediump", "highp"]
                    }
                ]
            }
        ]
        # blacklisted namespaces to generate pages for
        self.NAMESPACE_BLACKLIST = ["cinder::signals::detail", "cinder::audio::dsp::ooura", "cinder::detail", "glm::detail", "glm::gtc", "glm::gtx", "glm::io"]
        # blacklisted class strings - any class containing these strings will be skipped
        self.CLASS_LIST_BLACKLIST = ["glm", "@"]
        # cinder github repo path
        self.GITHUB_PATH = "http://github.com/cinder/Cinder/tree/master"
        # file that contains cinder meta data
        self.PROJECT_META_FILE = os.path.join(XML_SOURCE_PATH, "_cinder_8h.xml")

        # directory for the class template mustache file
        self.CLASS_TEMPLATE = os.path.join(TEMPLATE_PATH, "page-class-template.mustache")
        # directory for the namespace template mustache file
        self.NAMESPACE_TEMPLATE = os.path.join(TEMPLATE_PATH, "page-namespace-template.mustache")
        # directory for the namespace template mustache file
        self.GROUP_TEMPLATE = os.path.join(TEMPLATE_PATH, "page-group-template.mustache")
        # default html template mustache file
        self.HTML_TEMPLATE = os.path.join(TEMPLATE_PATH, "page-default-template.mustache")
        # guide html template mustache file
        self.GUIDE_TEMPLATE = os.path.join(TEMPLATE_PATH, "page-guide-template.mustache")
        # reference html template mustache file
        self.REFERENCE_TEMPLATE = os.path.join(TEMPLATE_PATH, "page-reference-template.mustache")
        # home page template mustache file
        self.HOME_TEMPLATE = os.path.join(TEMPLATE_PATH, "page-home-template.mustache")

        # file prefixes that indicate that the file should be parsed with the class template
        self.CLASS_FILE_PREFIXES = ["class", "struct", "interface"]
        # file prefixes that indicate that the file should be parsed with the namespace template
        self.NAMESPACE_FILE_PREFIXES = ["namespace"]
        # file prefixes that indicate that the file should be parsed with the group template
        self.GROUP_FILE_PREFIXES = ["group"]

        # configuration properties for different kinds of pages whose content is mostly dynamic from cinder.tag file
        self.DYNAMIC_PAGES_CONFIG = [
            # namespace list page
            {
                "id": "namespaces",
                "reference_html": "namespaces.html",
                "element_id": "namespace-content",
                "template": "namespace-list.mustache",
                "section": "namespaces",
                "searchable": False
            },
            # class list page
            {
                "id": "classes",
                "reference_html": "classes.html",
                "element_id": "classes-content",
                "template": "class-list.mustache",
                "section": "classes",
                "searchable": False
            },
            # glm reference page
            {
                "id": "glm",
                "reference_html": "reference/glm.html",
                "element_id": "glm-reference",
                "template": "glm-reference.mustache",
                "section": "reference",
                "searchable": True
            }
        ]

        # config for parsing glm group. In the future, we will standardize and externalize this so that we can
        # include and document additional modules
        self.GLM_MODULE_CONFIG = {
            "namespace": "glm",
            "url_prefix": "https://github.com/g-truc/glm/tree/0.9.6.3/",
            "group_keys": ["glm", "gtc", "gtx", "group__core"],
            "source_file_ext": "hpp"
        }

    def is_namespace_whitelisted(self, ns_str):
        if any([ns_str.startswith(prefix["name"]) for prefix in self.NAMESPACE_WHITELIST]):
            return True
        return None

    def is_namespace_blacklisted(self, ns_str):
        if any([ns_str.startswith(prefix) for prefix in self.NAMESPACE_BLACKLIST]):
            return True
        return False

    def get_ns_config(self, ns_str):
        for ns in self.NAMESPACE_WHITELIST:
            if ns["name"] == ns_str:
                return ns
        return None

    def get_section_config(self, sections, section_name):
        if sections:
            for sections in sections:
                if sections["name"] == section_name:
                    return sections
                return None
        return None

    def is_section_whitelisted(self, sections, section_name):
        '''
        Is the section of the page whitelisted
        :param sections: list page section configs
        :param section_name: name to check agains
        :return:
        '''
        if sections:
            for section in sections:
                if section["name"] == section_name:
                    whitelisted = True
                    break
                whitelisted = False
        else:
            whitelisted = True
        return whitelisted

# various state vars
class State(object):
    def __init__(self):
        self.html_files = []
        self.processed_html_files = False

    def add_html_file(self, file):
        self.html_files.append(file)


# convert docygen markup to html markup
tagDictionary = {
    "linebreak": "br",
    "emphasis": "em",
    "ref": "a",
    "ulink": "ulink",
    "computeroutput": "code",
    "includes": "span",
    "simplesect": "span",
    "para": "p"
}


# globals
g_tag_xml = None
g_symbolMap = None
g_search_index = None
config = Config()
state = State()


# =========================================================================================================== SYMBOL MAP

# mapping for the tag file with helper functions
class SymbolMap(object):
    def __init__(self):
        self.namespaces = {}
        self.classes = {}
        self.typedefs = {}
        self.functions = {}
        self.files = {}
        self.enums = {}
        self.groups = {}

    class Class(object):
        def __init__(self, class_tree):

            # name with namespace
            self.qualifiedName = class_tree.find('name').text
            # name without namespace
            self.name = strip_compound_name(self.qualifiedName)
            self.path = class_tree.find('filename').text
            self.base = class_tree.find('base').text if class_tree.find('base') is not None else ""
            self.is_template = True if class_tree.find('templarg') is not None else False

            self.functionList = []
            self.relatedLinks = []
            self.type_defs = []

            # Path the the description prefix
            self.prefix_content = None

            # list of tags to be added to the search index
            self.tags = []
            self.tags.append(self.name)

        def add_related_link(self, link_data):
            # check for dupes
            if not any(link.link == link_data.link for link in self.relatedLinks):
                self.relatedLinks.append(link_data)

        def define_prefix(self, content):
            self.prefix_content = content

        def add_type_def(self, type_def_obj):
            self.type_defs.append(type_def_obj)
            # add typedef string to search tags
            self.tags.append(strip_compound_name(type_def_obj.name))

        def add_function(self, fn_name, fn_obj):
            self.functionList.append(fn_obj)

            # add as a tag if not a duplicated name
            if not any(tag == fn_name for tag in self.tags):
                self.tags.append(fn_name)

    class Namespace(object):
        def __init__(self, name, file_name):
            self.name = name
            self.path = file_name
            self.functionList = []
            self.tags = []
            self.tags.append(self.name)
            self.typedefs = []

            # add all namespace parts to search tags
            for part in self.name.split("::"):
                self.tags.append(part)

        def add_function(self, fn_name, fn_obj):
            self.functionList.append(fn_obj)

            # add as a tag if not a duplicate name
            if not any(tag == fn_name for tag in self.tags):
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
            rel_path_arr = self.path.split(PARENT_DIR.replace("\\", "/"))
            self.relPath = "".join(rel_path_arr)           

    class Enum(object):
        def __init__(self, name, path):
            self.name = name
            self.path = path

    class Group(object):
        def __init__(self, tree):
            self.name = tree.find('name').text
            self.title = tree.find("title").text
            self.path = HTML_SOURCE_PATH + tree.find('filename').text
            self.src_path = (XML_SOURCE_PATH + tree.find('filename').text).replace(".html", ".xml")
            self.description = self.extract_description()
            self.functionList = []
            self.subgroup_names = []
            self.subgroups = []
            self.tags = []
            self.tags.append(strip_compound_name(self.name))
            self.prefix_content = None

        def extract_description(self):
            xml_tree = parse_xml(self.src_path)
            bs4 = BeautifulSoup()

            # use brief description if it exists
            description = markup_brief_description(bs4, xml_tree.find(r'compounddef'))

            # if not, use detailed description
            if not description:
                description = markup_description(bs4, xml_tree.find(r'compounddef'))

            # extract first sentence of description
            if description and description.text:
                first_sentence = description.text.split(". ")[0] + "."
                new_text = bs4.new_string(first_sentence)
                description.contents[0].replace_with(new_text)
            else:
                description = None

            return str(description) if str(description) else ""

        def add_function(self, fn_name, fn_obj):
            self.functionList.append(fn_obj)

            if not any(tag == fn_name for tag in self.tags):
                self.tags.append(fn_name)

    def add_function(self, ns, fn_name, fn_obj):
        self.functions[ns + "::" + fn_name] = fn_obj

    # searches the symbolMap for a given symbol, prepending cinder:: if not found as-is
    # returns a class
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
                    # log("typedef " + typedef.name + " was not shared from an existing class", 1)

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

    def find_group(self, name):
        return self.groups.get(name)

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

            # get whitelisted namespaces
            whitelisted = False
            if config.is_namespace_whitelisted(ns.name):
                whitelisted = True

            blacklisted = False
            if config.is_namespace_blacklisted(ns.name):
                blacklisted = True

            if whitelisted and not blacklisted:
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

        # key with "glm::" prepended
        elif ("glm::" + searchname) in self.typedefs:
            return self.typedefs["glm::" + searchname]

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
                if fn.name == fn_name:
                    fn_list.append(fn)

        # try with cinder::app prefix
        # TODO: refactor a bit with the ability to whitespace different namespaces test
        if len(fn_list) is 0:
            ns_search = class_name
            if class_name == "":
                ns_search = "cinder::app"
            ref_obj = g_symbolMap.find_namespace(ns_search)

            # iterate through class/namespace functions
            if ref_obj:
                for fn in ref_obj.functionList:
                    if fn.name == fn_name:
                        fn_list.append(fn)

        # iterate through glm groups
        if len(fn_list) == 0:
            for group in self.groups:
                group_ref = self.groups[group]
                for fn in group_ref.functionList:
                    if fn.name == fn_name:
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
        # if we have a bunch of options, we want to whittle it down to the best one
        if len(fn_list) > 1:
            best_score = 0

            for idx, fn in enumerate(fn_list):
                # fn_arg_len = len(fn.args)
                score = 0

                # find amount of required arguments
                fn_arg_len = 0
                for arg in fn.args:
                    if arg.find("=") < 0:
                        fn_arg_len += 1

                # if number of passed in args is the same as this function's arg length, add to the score
                if arg_len == fn_arg_len:
                    score += 0.5

                # loop through the amount of args in this function
                fn_args = fn.args[0:fn_arg_len]
                if len(fn_args) > 0:
                    for i, arg in enumerate(fn_args):
                        if i + 1 > arg_len:
                            continue
                        ratio = (SM(None, arg, args[i]).ratio())
                        score += (ratio * 2.0)

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


# ====================================================================================================== FILE DATA TYPES


class FileData(object):

    def __init__(self, tree):

        self.tree = tree  # xml file that describes the page
        self.bs4 = None  # html file of the actual page

        self.name = ""
        self.title = ""
        self.page_header = ""
        self.search_tags = []
        self.path = ""
        self.kind = ""
        self.kind_explicit = ""

    def get_content(self):
        content = {
            "name": self.name,
            "title": self.title,
            "page_header": self.page_header,
        }
        return content


class ClassFileData(FileData):

    def __init__(self, tree):
        FileData.__init__(self, tree)
        self.description = None
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
        self.namespace = "::".join(self.compoundName.split("::")[0:-1])

        ns_list = self.compoundName.split("::")
        ns_links = []
        # make list of namespace links
        for index, ns in enumerate(ns_list[:-1]):
            ns_object = g_symbolMap.find_namespace("::".join(ns_list[0:index + 1]))
            if ns_object:
                ns_link = LinkData(path_join(HTML_DEST_PATH, ns_object.path), ns)
            else:
                # add inactive link data
                ns_link = LinkData("", ns, False)
            ns_links.append(ns_link)
        self.ns_links = ns_links

    def get_content(self):
        orig_content = super(ClassFileData, self).get_content()
        content = orig_content.copy()
        class_content = {
            "name": self.stripped_name,
            "namespace": self.namespace,
            "namespace_links": self.ns_links,
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
        self.free_functions = []
        self.variables = []
        self.namespace_nav = None
        self.kind = find_file_kind(tree)
        self.kind_explicit = self.kind

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
            "free_functions": {
                "anchor": "free_functions",
                "list": self.free_functions,
                "length": len(self.free_functions)
            },
            "variables": {
                "anchor": "variables",
                "list": self.variables,
                "length": len(self.variables)
            }
        }
        content.update(ns_content)
        return content


class GroupFileData(FileData):

    def __init__(self, tree, module_config):
        FileData.__init__(self, tree)
        self.description = ""
        self.prefix = ""
        self.typedefs = []
        self.name = str(find_compound_name(tree))
        self.public_functions = []
        self.anchors = []
        self.config = module_config

        self.kind = "module"
        self.kind_explicit = self.kind

    def get_content(self):
        orig_content = super(GroupFileData, self).get_content()
        content = orig_content.copy()
        group_content = {
            "name": self.name,
            "description": self.description,
            "prefix": self.prefix,
            "typedefs": {
                "anchor": "typedefs",
                "list": self.typedefs,
                "length": len(self.typedefs)
            },
            "subgroups": {
                "anchor": "subgroups",
                "list": self.subgroups,
                "length": len(self.subgroups)
            },
            # "enumerations": {
            #     "anchor": "enumerations",
            #     "list": self.enumerations,
            #     "length": len(self.enumerations)
            # },
            "public_functions": {
                "anchor": "public-member-functions",
                "list": self.public_functions,
                "length": len(self.public_functions)
            }
            # "public_types": {
            #     "anchor": "public-types",
            #     "list": self.public_types,
            #     "length": len(self.public_types)
            # }
        }
        content.update(group_content)
        return content


class HtmlFileData(FileData):

    def __init__(self, in_path):
        FileData.__init__(self, None)

        self.html_content = ""
        self.group = None
        self.pagenav = []

        self.kind = "html"
        self.kind_explicit = self.kind
        if in_path.find("guides"+os.sep) > -1:
            self.kind_explicit = "guide"
        if in_path.find("reference"+os.sep) > -1:
            self.kind_explicit = "reference"

    def get_content(self):
        orig_content = super(HtmlFileData, self).get_content()
        content = dict(orig_content)
        template_content = {
            "html_content": self.html_content,
            "namespace_nav": str(g_namespaceNav),
            "pagenav": {
                "list": self.pagenav,
                "length": len(self.pagenav)
            }
        }
        content.update(template_content)
        return content


# ================================================================================================== Misc helper classes


class GuideConfig(object):

    def __init__(self, config_json, path, file_name):

        config_data = config_json["data"]

        # parse subnav
        subnav_list = []
        self.order = None
        if config_data["nav"]:
            for index, nav in enumerate(config_data["nav"]):
                subnav_obj = {}
                link_data = LinkData(os.path.join(path, nav["link"]), nav["label"])
                subnav = None

                # find order of file in group
                if re.match(file_name, nav["link"]):
                    self.order = index

                    # find subnav for the matched/current page if it has it
                    if nav.get("pagenav"):
                        subnav = self.parse_subnav(path, nav["pagenav"])

                subnav_obj["link_data"] = link_data
                subnav_obj["length"] = 0
                if subnav:
                    subnav_obj["length"] = len(subnav)
                    subnav_obj["subnav"] = subnav

                subnav_list.append(subnav_obj)
        self.pagenav = subnav_list

        # add keywords
        keywords = []
        metadata = config_data["metadata"]
        if metadata:
            if metadata["keywords"]:
                for k in metadata["keywords"]:
                    keywords.append(k)
        self.keywords = keywords

        # add seealso ci links
        see_also = config_data["seealso"]
        self.see_also_label = ""
        self.see_also_tags = []
        if see_also:
            self.see_also_label = config_data["seealso"]["label"]
            for ci in config_data["seealso"]["dox"]:
                self.see_also_tags.append(ci)

    # recursively parse subnav
    def parse_subnav(self, path, subnav):
        nav = []
        for menu in subnav:
            subnav_obj = {}
            link_data = LinkData(os.path.join(path, menu["link"]), menu["label"])
            local_subnav = None
            if menu.get("subnav"):
                local_subnav = self.parse_subnav(path, menu["subnav"])

            subnav_obj["link_data"] = link_data
            subnav_obj["length"] = 0
            if local_subnav:
                subnav_obj["length"] = len(local_subnav)
                subnav_obj["subnav"] = local_subnav

            nav.append(subnav_obj);
        return nav


class LinkData(object):

    def __init__(self, link=None, label=None, active=True):
        self.link = link
        self.label = label
        self.active = active


# ==================================================================================================== Utility functions


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
        # if len(arg.split(" ")) > 1:
        #     arg = " ".join(arg.split(" ")[:-1])

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
        if type(contents) is list:
            for c in contents:
                new_tag.append(clone(c))
        else:
            new_tag.append(contents)

    return new_tag


def gen_link_tag(bs4, text, link, target = "_self"):
    link_tag = gen_tag(bs4, "a", [], text)
    define_link_tag(link_tag, {"href": link})
    link_tag["target"] = target
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

    # make sure they are dirs
    src_dir = os.path.dirname(src_dir) + os.sep
    dest_dir = os.path.dirname(dest_dir) + os.sep
    new_link = relative_url(dest_dir, link)
    link_tag = gen_link_tag(bs4, text, new_link)
    return link_tag


def replace_element(bs4, element, replacement_tag):
    """
    Replaces an html element with another one, keeping the text contents.
    Use Case: Useful for replacing links with em tags or divs with spans
    :param bs4: Beautiful Soup instance doing the work
    :param element: element to change
    :param replacement_tag: new element type to change to
    :return:
    """
    if not element:
        return

    text_content = element.text
    replacement = gen_tag(bs4, replacement_tag, None, text_content)
    element.replace_with(replacement)


def get_body_content(bs4):
    return_str = ""
    for content in bs4.body.contents:
        content_utf = unicode(content).encode("utf-8", errors="replace")
        content_str = content_utf.decode("utf-8", errors="replace")
        if type(content) is Comment:
            return_str += "<!-- " + content_str + "-->"
        else:
            return_str += content_str
    return return_str


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
        log("DEFINING LINK TAG: " + str(tag), 1)
    else:
        tag["href"] = href


def parse_member_definition(bs4, member, member_name=None):
    """
    Parses a function tree and generates an object out of it
    :param bs4: beautifulsoup instance
    :param member: the member to parse
    :param member_name: the name of the class that's being parsed
    :return: the data object
    """
    if not member_name:
        member_name = member.find(r"name")
        member_name = member_name.text if member_name is not None else None

    anchor = find_member_anchor(member)

    # return type
    return_div = gen_tag(bs4, "span")
    return_markup = iterate_markup(bs4, member.find(r"type"), return_div)

    # if id has a glm group key, replace link with <em>. The links are irrelevent atm
    if any(member.attrib["id"].find(group_key) > -1 for group_key in config.GLM_MODULE_CONFIG["group_keys"]):
        if return_markup:
            replace_element(bs4, return_markup.a, "em")

    return_str = str(return_markup)

    # get args
    argstring = member.find("argsstring")
    if argstring is None:
        argstring = member.find("arglist")
    argstring_text = argstring.text if argstring is not None else ""

    # description
    description_div = markup_description(bs4, member)
    description_str = str(description_div) if len(description_div.text) > 0 else None

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
    """ Creates a new html element with the specified tag_name. "a" tags and "ulink" 
        tags are different since it generates a tags with links defined in the tree.

    Args:
        bs4: BeautifulSoup instance
        tag_name: What the new tag should be
        tree: original element tree which contains extra optional information
    """
    
    if tag_name == "a":
        new_tag = bs4.new_tag(tag_name)
        define_link_tag(new_tag, tree.attrib)
        # creates a new tag with a relative link using the data from the original tag
        # TODO: refactor define_tag and ren_link_tags. Should be able to create relative link on its own
        # new_tag = gen_rel_link_tag(bs4, "", new_tag["href"], TEMPLATE_PATH, DOXYGEN_HTML_PATH)
        new_tag = gen_link_tag(bs4, "", "../" + new_tag["href"])
    elif tag_name == "ulink":
        # ulinks are for external links
        new_tag = bs4.new_tag("a")
        new_tag = gen_link_tag(bs4, "", tree.attrib['url'], "_blank")
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
            a = gen_tag(bs4, "a", [], base.qualifiedName)
            define_link_tag(a, {'href': base.path})
            a = gen_link_tag(bs4, base.qualifiedName, path_join(HTML_DEST_PATH, a["href"]))
            li.append(a)
        else:
            li.append(base.qualifiedName)
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
        tag_name = "span"
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

    if tag == "computeroutput":
        if content:
            content = content.strip()

    if content is not None:
        content_tag.append(content)

    parent_tag.append(new_tag)
    return new_tag


def iterate_markup(bs4, tree, parent):
    if tree is None:
        return

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
        parent.append(tree.tail)
        if tree.tail.endswith(";"):
            parent.append(gen_tag(bs4, "br"))

    return current_tag


def markup_brief_description(bs4, tree, description_el=None):
    if description_el is None:
        description_el = gen_tag(bs4, "div", ["description", "content"])

    brief_desc = tree.findall(r'briefdescription/')
    if brief_desc is None:
        return
    else:
        for desc in brief_desc:
            iterate_markup(bs4, desc, description_el)

    return description_el


def markup_description(bs4, tree):
    description_el = gen_tag(bs4, "div", ["description", "content"])

    # mark up brief description first
    markup_brief_description(bs4, tree, description_el)

    # mark up detailed description next
    detailed_desc = tree.findall(r'detaileddescription/')

    if detailed_desc is not None:
        for desc in detailed_desc:
            iterate_markup(bs4, desc, description_el)

    return description_el


def replace_code_chunks(bs4):
    """
    Looks though the html and replaces any code chunks that exist
    in a paragraph and splits them up so that we can use pre tags.
    :param bs4:
    :return:
    """

    # find all the code chunks
    code_chunks = bs4.find_all("div", "programlisting")
    code_chunks += bs4.find_all("span", "programlisting")

    for chunk in code_chunks:
        pre_tag = bs4.new_tag("pre")
        code_tag = bs4.new_tag("code")
        add_class_to_tag(code_tag, "language-cpp")

        # for each code line, add a line of that text to the new div
        codeline = chunk.find_all("div", "codeline")
        codeline += chunk.find_all("span", "codeline")

        if codeline:
            for line in codeline:
                line_text = ""
                for c in line.contents:
                    if type(c) is Tag:
                        line_text += c.text
                    else:
                        line_text += c
                code_tag.append(line_text + "\n")
            pre_tag.append(code_tag)

        # replace content in code chunks
        chunk.clear()
        replacement_span = gen_tag(bs4, "span")
        replacement_span.append(pre_tag)
        chunk.append(pre_tag)


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

    if not dest_el:
        log("destination element does not exist", 1)

    update_links(src_content, src_path, src_path, dest_path)

    try:
        # copy source content into to bs4 instance so that we can copy over without messing up the source
        bs4 = BeautifulSoup(str(src_content).decode("UTF-8"))
        # copy all Tags over to dest element
        for content in bs4.body.contents:
            if type(content) is Tag:
                dest_el.append(content)
    except AttributeError as e:
        log("appending html content to element [ " + e.message + " ]", 2)



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
        ns_li["data-namespace"] = namespace

        # create link for each item
        a_tag = gen_link_tag(bs4, name, path_join(HTML_SOURCE_PATH, ns.path))

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
                    # add input
                    input_el = gen_tag(bs4, "input")
                    input_el["type"] = "checkbox"
                    input_el["id"] = "item-" + "-".join(list(node_label))

                    # root is expanded by default
                    if index == 0:
                        input_el.attrs["checked"] = "true"

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

    # tree = gen_tag(bs4, "div")
    ul = gen_tag(bs4, "ul")
    # tree.append(ul)
    add_class_to_tag(ul, "css-treeview")
    ul["id"] = "namespace-nav"

    iterate_namespace(bs4, namespaces, ul, 0, "")
    return ul


def find_typedefs_of(class_name, typedef_list):
    """
    Finds typedef objects that are shared from the given class within a given namespace
    :return: list if SymbolMap.Typedef objects
    """
    typedefs = []
    class_name = strip_compound_name(class_name)
    
    for typedef in typedef_list:
        if typedef.sharedFrom:
            if typedef.sharedFrom.name == class_name:
                typedefs.append(typedef)
    return typedefs


# ============================================================================================ File Processing Functions


def process_xml_file_definition(in_path, out_path, file_type):
    """
    Process an xml file definition, such as a class, namespace, or group
    :param in_path: xml file location
    :param out_path: final html file location
    :param file_type: "class", "namespace", or "group"
    :return:
    """

    # we dont like files that start with '_'
    if os.path.basename(in_path).startswith("_"):
        return

    # define the tree that contains all the data we need to populate this page
    tree = parse_xml(in_path)

    if tree is None:
        return

    if file_type == "class":
        if any(in_path.find(blacklisted) > -1 for blacklisted in config.CLASS_LIST_BLACKLIST):
            log("Skipping file | Class " + in_path + " blacklisted", 0)
            return

        html_template = config.CLASS_TEMPLATE
        file_data = fill_class_content(tree)
        section = "classes"
        body_class = "classes"

    elif file_type == "namespace":
        html_template = config.NAMESPACE_TEMPLATE
        file_data = fill_namespace_content(tree)

        if not file_data:
            return

        section = "namespaces"
        body_class = "namespaces"

    elif file_type == "module":
        html_template = config.GROUP_TEMPLATE
        file_data = fill_group_content(tree, config.GLM_MODULE_CONFIG)
        section = "reference"
        body_class = "reference"
    else:
        log("Skipping " + in_path, 1)
        return

    log_progress('Processing file: ' + str(in_path))

    # Generate the html file from the template and inject content
    file_content = file_data.get_content()
    bs4 = render_template(html_template, file_content)
    content_dict = {
        "page_title": file_content["title"],
        "main_content": get_body_content(bs4),
        "body_class": body_class,
        "section_namespace": "cinder",
        str("section_" + section): "true"}
    # append file meta
    content_dict.update(file_meta.copy())

    # render within main template
    bs4 = render_template(os.path.join(TEMPLATE_PATH, "master-template.mustache"), content_dict)
    # make sure all links are absolute
    update_links_abs(bs4, TEMPLATE_PATH)

    if not bs4:
        log("Skipping class due to something nasty. Bother Greg and try again some other time. Error rendering: " + in_path, 2)
        return

    # print output
    # update links in the template
    update_links(bs4, TEMPLATE_PATH + "htmlContentTemplate.html", TEMPLATE_PATH, out_path)

    # replace any code chunks with <pre> tags, which is not possible on initial creation
    replace_code_chunks(bs4)

    # link up all ci tags
    for tag in bs4.find_all('ci'):
        process_ci_tag(bs4, tag, in_path, out_path)

    # add to search index
    link_path = gen_rel_link_tag(bs4, "", out_path, HTML_SOURCE_PATH, HTML_DEST_PATH)["href"]
    add_to_search_index(bs4, link_path, file_data.kind, file_data.search_tags)

    # deactivate invalid relative links
    for link in bs4.find_all("a"):
        if link.has_attr("href") and link["href"].startswith("_"):
            # replace <a> with <span>
            dead_tag = gen_tag(bs4, "span", None, link.string)
            link.replace_with(dead_tag)

    # write the file
    write_html(bs4, out_path)


def parse_namespaces(tree, sections):
    namespaces = []
    if config.is_section_whitelisted(sections, "namespaces"):
        for member in tree.findall(r"compounddef/innernamespace"):
            link = path_join(HTML_DEST_PATH, member.attrib["refid"] + ".html")
            link_data = LinkData(link, member.text)
            namespaces.append(link_data)
    return namespaces


def parse_classes(tree, sections):
    classes = []
    if config.is_section_whitelisted(sections, "classes"):
        for member in tree.findall(r"compounddef/innerclass[@prot='public']"):
            link = member.attrib["refid"] + ".html"
            rel_link = path_join(HTML_DEST_PATH, link)
            link_data = LinkData(rel_link, member.text)

            kind = "struct" if link.startswith("struct") else "class"
            class_obj = {
                "link_data": link_data,
                "kind": kind
            }
            classes.append(class_obj)
    return classes


def parse_typedefs(bs4, tree, sections):
    typedefs = []

    if config.is_section_whitelisted(sections, "typedefs"):
        section_config = config.get_section_config(sections, "typedefs")
        if section_config:
            prefix_blacklist = section_config["prefix_blacklist"] if section_config.has_key("prefix_blacklist") else None
        else:
            prefix_blacklist = None

        for member in tree.findall(r"compounddef/sectiondef/[@kind='typedef']/memberdef/[@kind='typedef']"):
            member_name = member.find(r"name").text
            if prefix_blacklist and any(member_name.startswith(blacklisted) > 0 for blacklisted in prefix_blacklist):
                # skip this blacklisted typedef
                continue

            typedef_obj = parse_member_definition(bs4, member)
            typedefs.append(typedef_obj)
    return typedefs


def parse_enums(bs4, tree, sections):
    enums = []
    if config.is_section_whitelisted(sections, "enums"):
        for member in tree.findall(r"compounddef/sectiondef/[@kind='enum']/memberdef/[@kind='enum']"):
            member_obj = parse_enum(bs4, member)
            enums.append(member_obj)
    return enums


def parse_functions(bs4, tree, sections):
    fns = []
    if config.is_section_whitelisted(sections, "functions"):
        for member in tree.findall(r"compounddef/sectiondef/[@kind='func']/memberdef/[@kind='function']"):
            function_obj = parse_member_definition(bs4, member)
            fns.append(function_obj)
    return fns


def parse_free_functions(bs4, tree, sections):
    free_fns = []
    if config.is_section_whitelisted(sections, "free_functions"):
        for member in tree.findall(r"compounddef/sectiondef/[@kind='user-defined']/memberdef/[@kind='function']"):
            function_obj = parse_member_definition(bs4, member)
            free_fns.append(function_obj)
    return free_fns


def parse_vars(bs4, tree, sections):
    variables = []
    if config.is_section_whitelisted(sections, "variables"):
        for member in tree.findall(r"compounddef/sectiondef/[@kind='var']/memberdef/[@kind='variable']"):
            var_obj = parse_member_definition(bs4, member)
            initializer = member.find('initializer').text if member.find('initializer') is not None else None
            var_obj["definition"]["args"] = initializer
            variables.append(var_obj)
    return variables


def fill_class_content(tree):
    """
    Populates the class content object with data
    :param tree:
    :return:
    """

    bs4 = BeautifulSoup()
    file_data = ClassFileData(tree)

    include_file = ""
    include_path = ""
    include_tag = tree.find(r"compounddef/includes")
    location_tag = tree.find(r"compounddef/location")
    if location_tag is not None:
        include_path = "/".join(location_tag.attrib["file"].split("/")[1:])
    if include_tag is not None:
        include_file = include_tag.text
    class_name = file_data.name
    file_def = g_symbolMap.find_file(include_file)
    class_def = g_symbolMap.find_class(class_name)

    # class template stuff ------------------------------ #
    file_data.is_template = True if tree.find(r"compounddef/templateparamlist") is not None else False
    if file_data.is_template:
        try:
            def_name = tree.find(r"compounddef/templateparamlist/param/type")
            file_data.template_def_name = def_name.text if def_name is not None else ""
        except Exception as e:
            file_data.template_def_name = ""
            log(e.message, 1)

    if not class_def:
        log("NO CLASS OBJECT DEFINED FOR: " + class_name, 1)
        # raise
        # return

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
    if include_file and include_path:
        file_obj = g_symbolMap.find_file(include_file)
        github_path = config.GITHUB_PATH + '/include/' + include_path
        if file_obj:
            include_link = LinkData(github_path, include_path)

    file_data.includes = include_link

    # typedefs ------------------------------------------ #
    typedefs = []
    ns_obj = g_symbolMap.find_namespace(file_data.namespace)
    if ns_obj and ns_obj.typedefs:
        class_typedefs = find_typedefs_of(class_name, ns_obj.typedefs)
        if file_def is not None:
            for t in class_typedefs:
                link_data = LinkData()
                link_data.label = t.name
                link_path = path_join(HTML_DEST_PATH, t.path)
                link_data.link = link_path
                typedefs.append(link_data)
    file_data.typedefs = typedefs

    # class hierarchy ----------------------------------- #
    if class_def:
        class_hierarchy = gen_class_hierarchy(bs4, class_def)
        file_data.class_hierarchy = str(class_hierarchy) if class_hierarchy else None

    # class list ---------------------------------------- #
    classes = []
    for classDef in tree.findall(r"compounddef/innerclass[@prot='public']"):
        link_data = LinkData()
        link_data.label = strip_compound_name(classDef.text)
        link_data.link = path_join(HTML_DEST_PATH, classDef.attrib["refid"] + ".html")
        classes.append(link_data)
    file_data.classes = classes

    # related links  ------------------------------------ #
    # generated by guides and references
    related = []
    if class_def:
        if class_def.relatedLinks:
            for link_data in class_def.relatedLinks:
                related.append(link_data)

        # ci prefix / description ----------------------- #
        # if the class has a prefix, add it here
        if class_def.prefix_content:
            file_data.prefix = class_def.prefix_content

    file_data.related = related

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
            friend_link = gen_rel_link_tag(bs4, friend_class.name, friend_class.path, TEMPLATE_PATH, HTML_DEST_PATH)
            member_obj["definition"]["name"] = str(friend_link)
        friends.append(member_obj)
    file_data.friends = friends

    if class_def:
        file_data.search_tags = class_def.tags

    return file_data


def fill_namespace_content(tree):

    bs4 = BeautifulSoup()

    if tree is None:
        return

    # get common data for the file
    file_data = NamespaceFileData(tree)
    ns_def = g_symbolMap.find_namespace(file_data.name)

    if ns_def:
        if config.is_namespace_blacklisted(ns_def.name):
            log("Skipping file | Namespace " + ns_def.name + " blacklisted", 1)
            return
    else:
        log("Skipping: tree is not defined", 1)
        return

    # return result of special glm namespace content filling
    # TODO: If we get here, that means the namespace is NOT blacklisted, so this is where we check if each piece is whitelisted, if that array is empty, we assume that it's all whitelisted
    ns_config = config.get_ns_config(ns_def.name)
    if ns_config and ns_config.has_key("structure_whitelist"):
        sections = ns_config["structure_whitelist"]
    else:
        sections = None

    # if ns_def.name == "glm":
    #     return fill_glm_namespace_content(tree)

    # page title ---------------------------------------- #
    file_data.title = file_data.name

    # add namespace nav --------------------------------- #
    file_data.namespace_nav = str(g_namespaceNav)

    # add namespaces ------------------------------------ #
    file_data.namespaces = parse_namespaces(tree, sections)

    # add classes --------------------------------------- #
    file_data.classes = parse_classes(tree, sections)

    # add typedefs -------------------------------------- #
    file_data.typedefs = parse_typedefs(bs4, tree, sections)

    # add enumerations ---------------------------------- #
    file_data.enumerations = parse_enums(bs4, tree, sections)

    # functions ----------------------------------------- #
    file_data.functions = parse_functions(bs4, tree, sections)

    # free functions ------------------------------------ #
    file_data.free_functions = parse_free_functions(bs4, tree, sections)

    # variables ----------------------------------------- #
    file_data.variables = parse_vars(bs4, tree, sections)

    # define search tags
    if ns_def:
        file_data.search_tags = ns_def.tags
    else:
        file_data.search_tags = []

    file_data.search_tags.extend(["namespace"])

    return file_data


def fill_group_content(tree, module_config):
    bs4 = BeautifulSoup()
    file_data = GroupFileData(tree, module_config)

    group_name = file_data.name
    group_def = g_symbolMap.find_group(group_name)

    if not group_def:
        log("NO CLASS OBJECT DEFINED FOR: " + group_name, 1)
        # return

    # page title ---------------------------------------- #
    file_data.title = file_data.name

    # page header --------------------------------------- #
    file_data.page_header = file_data.name

    # add description ----------------------------------- #
    description = markup_description(bs4, tree.find(r'compounddef'))
    file_data.description = str(description) if description is not None else ""

    # submodules ---------------------------------------- #
    subgroups = []
    if group_def is not None:
        for subgroup in group_def.subgroups:
            subgroup_obj = {
                "label": subgroup.name,
                "link": subgroup.path
            }
            subgroups.append(subgroup_obj)
    file_data.subgroups = subgroups

    # typedefs ------------------------------------------ #
    typedefs = []
    for member in tree.findall(r"compounddef/sectiondef/[@kind='typedef']/memberdef/[@kind='typedef']"):
        typedef_obj = parse_member_definition(bs4, member)
        typedefs.append(typedef_obj)
    file_data.typedefs = typedefs

    # ci prefix / description --------------------------- #
    # if the group has a prefix, add it here
    if group_def is not None:
        if group_def.prefix_content is not None:
            file_data.prefix = group_def.prefix_content

    # # enumerations -------------------------------------- #
    # enumerations = []
    # for e in tree.findall(r"compounddef/sectiondef/memberdef[@kind='enum']"):
    #     member_obj = parse_enum(bs4, e)
    #     enumerations.append(member_obj)
    # file_data.enumerations = enumerations

    # public member Functions --------------------------- #
    public_fns = []
    public_static_fns = []
    for memberFn in tree.findall(r'compounddef/sectiondef/memberdef[@kind="function"][@prot="public"]'):

        function_obj = parse_function(bs4, memberFn, group_name)
        is_static = memberFn.attrib["static"]

        if is_static == 'yes':
            public_static_fns.append(function_obj)
        else:
            public_fns.append(function_obj)

    file_data.public_functions = public_fns
    file_data.public_static_functions = public_static_fns

    if group_def:
        file_data.search_tags = group_def.tags

    return file_data


def process_html_file(in_path, out_path):
    """ Parses an html file.
    - Adds template around the html
    - Copy original css and js links into new hmtl
    - Save html in destination dir
    """
#    log_progress('Processing file: ' + str(in_path))
    print 'Processing file: ' + str(in_path)
    # relative path in relation to the in_path (htmlsrc/)
    local_rel_path = os.path.relpath(in_path, HTML_SOURCE_PATH)
    # directory name of the path
    in_dir = os.path.dirname(in_path)
    # file name
    in_file_name = os.path.basename(in_path)

    # skip if it starts with "_", which means that it's not a first class citizen file and is supplemental
    if in_file_name.startswith("_"):
        return

    # get common data for the file
    file_data = HtmlFileData(in_path)

    # searchable by default
    is_searchable = True
    # tags for search engine
    search_tags = []

    # selected section of the website
    section = ""

    # parse guide config (if present in current directory)
    # this determines which function is used to generate dynamic page, which template to use, etc
    config_data = parse_config(in_dir, in_file_name)
    if config_data:
        # add search tags
        for k in config_data.keywords:
            search_tags.append(k)

        # plug in subnav data
        file_data.pagenav = config_data.pagenav

    # get correct template for the type of file
    template = config.HTML_TEMPLATE
    body_class = "default"
    if in_path.find("htmlsrc" + os.sep + "index.html") > -1:
        template = config.HOME_TEMPLATE
        is_searchable = False
        body_class = "section_home"
        section = "home"
    elif in_path.find("reference"+os.sep) > -1:
        template = config.REFERENCE_TEMPLATE
        body_class = "reference"
        section = "reference"
    elif in_path.find("guides"+os.sep) > -1:
        template = config.GUIDE_TEMPLATE
        body_class = "guide"
        section = "guides"

    # fill content ----------------------------------------

    # get source file body content
    orig_html = generate_bs4(in_path)

    # extract original scripts to append later
    orig_scripts = []
    for x in orig_html.findAll("script"):
        orig_scripts.append(x.extract())

    orig_links = []

    # get title
    if orig_html.head:
        if orig_html.head.title:
            file_data.title = orig_html.head.title.text

        for x in orig_html.findAll('link', rel="stylesheet"):
            orig_links.append(x.extract())

    # if there is a specific page that needs some special dynamic content, this is where we do it
    insert_div_id = ""
    dynamic_div = gen_tag(orig_html, "body")
    for data in config.DYNAMIC_PAGES_CONFIG:
        if "reference_html" in data and data["reference_html"] == local_rel_path:
            is_searchable = bool(data["searchable"])
            markup = generate_dynamic_markup(data)
            for content in markup.body.contents:
                dynamic_div.append(content)
            insert_div_id = data["element_id"]

            if "section" in data:
                section = data["section"]

    # inject dynamic content into orig_html
    if insert_div_id:
        insert_el = orig_html.find(id=insert_div_id)
        inject_html(dynamic_div, insert_el, in_path, out_path)

    # get body content out of bs4 and plug into file_data
    body_content = get_body_content(orig_html)
    file_data.html_content = body_content
    file_content = file_data.get_content()

    # render file template
    bs4 = render_template(template, file_content)
    update_links_abs(bs4, os.path.dirname(in_path))
    content_dict = {'page_title': file_content["title"], 'main_content': get_body_content(bs4), 'body_class': body_class, str("section_" + section): "true"}
    # append file meta
    content_dict.update(file_meta.copy())

    # plug everything into the master template
    bs4 = render_template(os.path.join(TEMPLATE_PATH, "master-template.mustache"), content_dict)
    # make sure all links are absolute
    update_links_abs(bs4, TEMPLATE_PATH)
    # now all links shoul be relative to out path
    update_links(bs4, TEMPLATE_PATH, in_path, out_path)

    if bs4 is None:
        log("Error generating file, so skipping: " + in_path, 2)
        return

    # get list of all the css and js links in the new bs4
    link_list = bs4.head.find_all("link")
    script_list = bs4.body.find_all("script")

    # copy any css paths that may be in the original html and paste into new file
    for link in orig_links:
        # do not add duplicates
        if any(link_item["href"] == link["href"] for link_item in link_list):
            continue

        if bs4.head:
            bs4.head.append(link)

    # append original scripts to the end
    for script in orig_scripts:
        # do not add duplicates
        if script.has_attr("src") and any(script_item.has_attr("src") and script_item["src"] == script["src"] for script_item in script_list):
            continue
        if bs4.body:
            bs4.body.append(script)

    if orig_html.head:
        if bs4.head:
            for d in orig_html.head.find_all("ci"):
                bs4.head.append(d)

        # add ci seealso tags from config to bs4 head if it's the first in a group
        if config_data and config_data.order == 0:
            seealso_label = config_data.see_also_label
            for tag in config_data.see_also_tags:

                ci_tag = gen_tag(bs4, "ci")
                ci_tag.attrs["seealso"] = ""
                ci_tag.attrs["label"] = seealso_label
                ci_tag.attrs["dox"] = tag

                bs4.head.append(ci_tag)

        # add tags from the meta keywords tag
        for meta_tag in orig_html.head.findAll(attrs={"name": "keywords"}):
            for keyword in meta_tag['content'].split(','):
                search_tags.append(keyword.encode('utf-8').strip())

        # look for any meta 'group' tags to tell us that it's part of a grpup that will need nav
        for meta_tag in orig_html.head.findAll(attrs={"name": "group"}):
            if meta_tag['content']:
                file_data.group = meta_tag['content']

    # link up all ci tags
    for tag in bs4.find_all('ci'):
        process_ci_tag(bs4, tag, in_path, out_path)

    if in_path.find("_docs/") < 0:
        if is_searchable:
            link_path = gen_rel_link_tag(bs4, "", out_path, HTML_SOURCE_PATH, HTML_DEST_PATH)["href"]
            add_to_search_index(bs4, link_path, file_data.kind_explicit, search_tags)

        state.add_html_file(file_data)
        file_data.path = out_path
        write_html(bs4, out_path)


def parse_config(path, file_name):
    # if "config.json" exists in path directory
    config_path = os.path.join(path, "config.json")
    if os.path.exists(config_path):
        # load and turn into GuideConfig object
        with open(config_path) as data_file:
            try:
                config_data = json.load(data_file)
                guide_config = GuideConfig(config_data, path, file_name)
            except Exception as e:
                log(str(e), 2)
                raise
        return guide_config
    else:
        return None


# ============================================================================================== Dynamic Page Generation


def generate_dynamic_markup(ref_data):

    # find template if it exists

    ref_id = ref_data["id"]
    if ref_id == "glm":
        return_markup = generate_glm_reference()
    elif ref_id == "namespaces":
        return_markup = generate_namespace_data()
    elif ref_id == "classes":
        return_markup = generate_class_list_data()
    else:
        return_markup = "NOTHING FOUND"
        log("No rules for generating dynamic content for id'" + ref_id + "' was found", 1)

    # plug data into template (if it exists)
    template_path = os.path.join(TEMPLATE_PATH, ref_data["template"])
    markup = render_template(template_path, return_markup)
    return markup


def generate_glm_reference():

    glm_group_data = {
        "groups": []
    }

    # add group data to glm reference data object
    for group_name in g_symbolMap.groups:
        group = g_symbolMap.find_group(group_name)
        group_data = {}
        group_data["name"] = group.title
        group_data["path"] = group.path
        group_data["description"] = group.description

        subgroups = []
        if len(group.subgroups) > 0:
            for subgroup in group.subgroups:
                subgroup_data = {}
                subgroup_data["name"] = subgroup.title
                subgroup_data["path"] = subgroup.path
                subgroup_data["description"] = subgroup.description
                subgroups.append(subgroup_data)
            group_data["subgroups"] = subgroups
            glm_group_data["groups"].append(group_data)

    return glm_group_data


def generate_namespace_data():

    ns_data = {
        "namespaces": []
    }

    namespaces = g_symbolMap.get_whitelisted_namespaces()
    for ns in namespaces:
        ns = {
            "link": ns.path,
            "label": ns.name
        }
        ns_data["namespaces"].append(ns)

    return ns_data


def generate_class_list_data():

    classlist_data = {
        "classes": []
    }

    classes = g_symbolMap.get_ordered_class_list()
    for c in classes:
        class_data = {
            "link": c.path,
            "label": c.name
        }
        classlist_data["classes"].append(class_data)

    return classlist_data


# ===================================================================================================== CI Tag Functions


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
        process_ci_prefix_tag(bs4, tag, in_path)

    # elif tag.has_attr("source"):
    #     process_ci_source_tag(bs4, tag)

    else:
        replace_ci_tag(bs4, tag, in_path, out_path)


def replace_ci_tag(bs4, link, in_path, out_path):
    ref_obj = find_ci_tag_ref(link)

    if ref_obj:
        ref_location = path_join(HTML_DEST_PATH, ref_obj.path)
        new_link = gen_rel_link_tag(bs4, link.contents, ref_location, in_path, out_path)

        # transfer tag classes to new tag
        tag_classes = link["class"] if link.has_attr("class") else None
        if tag_classes:
            for c in tag_classes:
                add_class_to_tag(new_link, c)

        add_class_to_tag(new_link, "ci")
        link.replace_with(new_link)
    else:
        log("Could not find replacement tag for ci tag: " + str(link), 1)


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
    link_data = LinkData(out_path.replace("\\", "/"), label)

    # if type(ref_obj) is SymbolMap.Class or type(ref_obj) is SymbolMap.Typedef:
    if type(ref_obj) is SymbolMap.Class:
        ref_obj.add_related_link(link_data)

    elif type(ref_obj) is SymbolMap.Namespace:
        # find all classes with that namespace and add guide to every one
        for class_obj in g_symbolMap.find_classes_in_namespace(ref_obj.name):
            class_obj.add_related_link(link_data)
    else:
        log("Could not find seealso reference for " + str(tag), 1)


def process_ci_prefix_tag(bs4, tag, in_path):
    """
    Finds the referenced tag's object if existent and adds the path to the prefix file to the class to be parsed later
    :param tag: The ci tag with a defined prefix attribute
    :param in_path: The path to the refix content
    :return:
    """
    in_path = in_path.replace('\\', '/')
    in_dir = get_path_dir(in_path)

    obj_ref = find_ci_tag_ref(tag)
    if obj_ref and type(obj_ref) is SymbolMap.Class:

        # get tag content
        prefix_content = ""
        for c in tag.contents:
            content = c.encode("utf-8", errors="replace")
            prefix_content += content

        # generate bs4 from content and update links as reltive from the template path
        # could alternatively set the absolute paths of content, which would then be turned into rel paths later
        new_bs4 = generate_bs4_from_string(prefix_content)
        update_links(new_bs4, in_dir, in_path, TEMPLATE_PATH)

        # get updated body content and assign as prefix_content
        prefix_content = ""
        for c in new_bs4.body:
            content = c.encode("utf-8", errors="replace")
            prefix_content += content

        obj_ref.define_prefix(prefix_content)


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

        # find enum link
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
        log("problem finding ci tag", 1)
        log(e.message, 1)
        return None

    return ref_obj


# ======================================================================================================== Link Updating

def path_join(path, link):
    p = path.replace('\\', '/')
    l = link.replace('\\', '/')
    sep = '/' if not p.endswith('/') else ''
    new_link = p + sep + l
    return new_link

def get_path_dir(path):
    path_parts = path.replace('\\', '/').split('/')
    # if it doesn't end with a '/', lop off the last word
    if not path.endswith('/'):
        in_dir = '/'.join(path_parts[:-1]) + '/'
    else:
        in_dir = path
    return in_dir

def update_links_abs(html, src_path):
    """
    Replace all of the relative a links with absolut links
    :param html:
    :param src_path:
    :param dest_path:
    :return:
    """

    # css links
    for link in html.find_all("link"):
        if link.has_attr("href"):
            link["href"] = update_link_abs(link["href"], src_path)

    # a links
    for a in html.find_all("a"):
        if a.has_attr("href"):
            link_href = a["href"]

            # if the link is an hpp file, lets link to the github link since we likely don't have it in our docs
            if link_href.find(config.GLM_MODULE_CONFIG["source_file_ext"]) > -1:
                a["href"] = config.GLM_MODULE_CONFIG["url_prefix"] + a.text
            else:
                a["href"] = update_link_abs(a["href"], src_path)

    # script links
    for script in html.find_all("script"):
        if script.has_attr("src"):
            script["src"] = update_link_abs(script["src"], src_path)

    # images
    for img in html.find_all("img"):
        if img.has_attr("src"):
            img["src"] = update_link_abs(img["src"], src_path)

    # iframes
    for iframe in html.find_all("iframe"):
        if iframe.has_attr("src"):
            link_src = iframe["src"]
            if link_src.startswith('javascript') or link_src.startswith('http'):
                return

            new_link = update_link_abs(link_src, src_path)
            iframe["src"] = new_link


def relative_url(in_path, link):
    """
    Generates a relative url from a absolute destination directory 
    to an absolute file path
    """

    index = 0
    SEPARATOR = "/"
    d = filter(None, in_path.replace('\\', SEPARATOR).split( SEPARATOR ))
    s = filter(None, link.replace('\\', SEPARATOR).split( SEPARATOR ))

    # FIND largest substring match
    for i, resource in enumerate( d ):
        if resource != s[i]:
            break
        index += 1

    # remainder of source
    s = s[index:]
    
    backCount = len( d ) - index

    path = "../" * backCount
    path += SEPARATOR.join( s )
    return path


def update_link_abs(link, in_path):
    """
    Update the given link to point to something relative to the new path
    :param link: The link to change
    :param in_path: the original path to the file that the link lives in
    :return:
    """

    if link.startswith("http") or link.startswith("javascript:") or link.startswith("#"):
        return link

    SEPARATOR = "/"
    in_path = in_path.replace('\\', SEPARATOR)

    index = 0
    backs = 0
    # SPLIT the url into a list of path parts
    r = in_path.split(SEPARATOR)
    r = filter(None, r)
    l = link.split(SEPARATOR)
    l = filter(None, l)

    # FIND largest substring match
    for i, resource in enumerate( r ):
        if resource != l[i]:
            break
        index += 1

    # FIND the amount of back references
    for j, back_ref in enumerate( l ):
        if back_ref != "..":
            break
        backs += 1

    if not index:
        if backs > 0:
            final = SEPARATOR.join(r[:backs*-1]) + SEPARATOR + SEPARATOR.join(l[backs:]) 
        else:
            final = SEPARATOR.join(r) + SEPARATOR + SEPARATOR.join(l)
    else:
        pre = r[:index]
        post = l[index:]
        final = SEPARATOR.join(pre) + SEPARATOR + SEPARATOR.join(post)

    return final    


def update_links(html, template_path, src_path, save_path):
    """
    Replace all of the relative a links, js links and image links and make them relative to the outpath
    :param html:
    :param template_path:
    :param dest_path:
    :return:
    """

    template_path = "/".join(template_path.replace('\\', '/').split('/'))

    # css links
    for link in html.find_all("link"):
        if link.has_attr("href"):
            link["href"] = update_link(link["href"], template_path, save_path)

    # a links
    for a in html.find_all("a"):
        if a.has_attr("href"):
            link_href = a["href"]

            # if the link is an hpp file, lets link to the github link since we likely don't have it in our docs
            if link_href.find(config.GLM_MODULE_CONFIG["source_file_ext"]) > -1:
                a["href"] = config.GLM_MODULE_CONFIG["url_prefix"] + a.text
            else:
                a["href"] = update_link(a["href"], template_path, save_path)

    # script links
    for script in html.find_all("script"):
        if script.has_attr("src"):
            script["src"] = update_link(script["src"], template_path, save_path)

    # images
    for img in html.find_all("img"):
        if img.has_attr("src"):
            img["src"] = update_link(img["src"], template_path, save_path)

    # iframes
    for iframe in html.find_all("iframe"):
        if iframe.has_attr("src"):

            link_src = iframe["src"]

            # on osx/unix
            if os.sep == "/":
                if not posixpath.isabs(link_src):
                    link_src = "/" + link_src
            if link_src.startswith('javascript') or link_src.startswith('http'):
                return

            # base dir
            src_base = src_path.split(BASE_PATH)[1].split(os.sep)[0]
            dest_base = save_path.split(BASE_PATH)[1].split(os.sep)[0]

            # get link of iframe source and replace in iframe
            new_link = update_link(link_src, template_path, save_path)
            iframe["src"] = new_link

            # define the paths of file to copy and where to copy to
            src_file = link_src
            dest_file = link_src.replace(src_base, dest_base)

            try:
                # copy file as long as the source and destination is not the same
                if SM(None, src_file, dest_file).ratio() < 1.0:
                    shutil.copy2(src_file, dest_file)
            except IOError as e:
                log("Cannot copy src_file because it doesn't exist: " + src_file, 2)
                log(e.strerror, 2)
                return
            except Exception as e:
                log("Cannot copy iframe over because of some other error", 2)
                log(e.strerror)
                return


def update_link(link, in_path, out_path):
    """
    Update the given link to point to something relative to the new path
    :param link: The link to change
    :param in_path: the original path to the file that the link lives in
    :return:
    """

    if link.startswith("http") or link.startswith("javascript:") or link.startswith("#"):
        return link

    SEPARATOR = '/'
    in_path = in_path.replace('\\', SEPARATOR)
    out_path = out_path.replace('\\', SEPARATOR)
    link = link.replace('\\', SEPARATOR)
    base_path = BASE_PATH.replace('\\', SEPARATOR)

    # if a relative path, make it absolute
    if in_path.find(base_path) < 0:
        in_path = base_path + in_path
    
    # get absolute in path
    abs_link_path = update_link_abs(link, in_path)

    # convert to relative link in relation to the out path
    src_base = in_path.split(base_path)[1].split(SEPARATOR)[0]        # likely htmlsrc
    dest_base = out_path.split(base_path)[1].split(SEPARATOR)[0]      # htmlsrc or html

    abs_dest = posixpath.dirname(out_path).replace('\\', SEPARATOR)
    abs_link = abs_link_path.replace(src_base, dest_base)
    # if not posixpath.isabs(abs_link):
        # abs_link = "/" + abs_link

    rel_link_path = relative_url(abs_dest, abs_link)

    return rel_link_path

# =============================================================================================== File Utility Functions


def generate_bs4(file_path):

    # tree = None
    try:
        with open(file_path, "rb") as html_file:
            content = html_file.read().decode("utf-8", errors="replace")
            new_content = content.encode("utf-8", errors="replace")

        # wrap in body tag if none exists
        if new_content.find("<body") < 0:
            new_content = "<body>" + new_content + "</body>"
            log("No body tag found in file: " + file_path)

        bs4 = BeautifulSoup(new_content)
        return bs4

    except Exception as e:
        log(e.message, 2)
        return None


def generate_bs4_from_string(string):

    # make sure it's a unicode object
    if type(string) != unicode:
        output_string = string.decode("utf-8", errors="replace")
    else:
        output_string = string

    # wrap in body tag if none exists
    if string.find("<body") < 0:
        output_string = "<body>" + output_string + "</body>"

    bs4 = BeautifulSoup(output_string)
    return bs4


def get_symbol_to_file_map():
    """
    Returns a dictionary from Cinder class name to file path
    """
    log("generating symbol map from tag file", 0, True)
    symbol_map = SymbolMap()


    # find classes
    class_tags = g_tag_xml.findall(r'compound/[@kind="class"]')
    for c in class_tags:
        class_obj = SymbolMap.Class(c)
        name = class_obj.qualifiedName

        # skip over blacklisted classes that belong to a blacklisted namespace
        if any(name.find(blacklisted) > -1 for blacklisted in config.CLASS_LIST_BLACKLIST):
            # print "SKIPPING " + name
            continue

        base_class = class_obj.base
        symbol_map.classes[name] = class_obj

        # find functions and add to symbol map
        members = c.findall(r"member[@kind='function']")
        for member in members:

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
        name = struct_obj.qualifiedName
        base_class = struct_obj.base

        # skip over blacklisted classes that belong to a blacklisted namespace
        if any(name.find(blacklisted) > -1 for blacklisted in config.CLASS_LIST_BLACKLIST):
            log("SKIPPING " + name, 1)
            continue

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

        # skip over blacklisted classes that belong to a blacklisted namespace
        if config.is_namespace_blacklisted(namespace_name):
            log("SKIPPING NAMESPACE: " + namespace_name, 1)
            continue

        ns_obj = SymbolMap.Namespace(namespace_name, file_name)
        symbol_map.namespaces[namespace_name] = ns_obj

        # process all typedefs in namespace
        typedef_list = add_typedefs(ns.findall(r"member/[@kind='typedef']"), namespace_name, symbol_map)
        ns_obj.typedefs = typedef_list

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
            function_obj = SymbolMap.Function(member, "")
            symbol_map.add_function("", function_obj.name, function_obj)

    # find groups
    group_tags = g_tag_xml.findall(r'compound/[@kind="group"]')
    for member in group_tags:
        group_obj = SymbolMap.Group(member)
        subgroups = member.findall('subgroup')

        # hardcode this for now since all groups are part of glm
        ns = "glm"

        # add subgroup names
        if len(subgroups) > 0:
            for subgroup in subgroups:
                group_obj.subgroup_names.append(subgroup.text)

        # find functions and add to symbol map
        functions = member.findall(r"member[@kind='function']")
        for function in functions:
            function_obj = SymbolMap.Function(function, ns)
            group_obj.add_function(function_obj.name, function_obj)
            symbol_map.add_function(ns, function_obj.name, function_obj)

        # find typedefs
        typedefs = member.findall(r"member/[@kind='typedef']")
        add_typedefs(typedefs, "glm", symbol_map)

        symbol_map.groups[group_obj.name] = group_obj

    # link up subgroups to parent groups
    for group_names in symbol_map.groups:
        group_obj = symbol_map.find_group(group_names)
        if len(group_obj.subgroup_names) > 0:
            # print group.name
            for subgroup_name in group_obj.subgroup_names:
                subgroup = symbol_map.find_group(subgroup_name)
                group_obj.subgroups.append(subgroup)

    if len(file_tags) == 0:
        log("no compound of type 'file' found in tag file. Check doxygen SHOW_FILES setting.", 1)

    return symbol_map


def add_typedefs(typedefs, ns_name, symbol_map):
    typedef_list = []
    # if ns_name == "cinder::gl"
    for typdef in typedefs:
        name = typdef.find("name").text
        type_name = typdef.find("type").text
        full_name = ns_name + "::" + name
        shared_from_class = None

        if type_name.startswith("class") > 0:
            shared_from_class = symbol_map.find_class(type_name.split("class ")[1])

        elif type_name.find("shared") > 0:
            if type_name.find("class"):
                shareds = re.findall(r"std::shared_ptr< (?:class)* *([\w]*) >", type_name)
            else:
                shareds = re.findall(r"std::shared_ptr< *([\w]*) >", type_name)

            if len(shareds) > 0:
                base = ns_name + "::" + shareds[0]
                shared_from_class = symbol_map.find_class(base)

        if not shared_from_class:
            # find based on the string in type that's not explicitly a shared_ptr
            # such as <type>SurfaceT&lt; uint8_t &gt;</type>
            shareds = re.findall(r"([A-Za-z0-9]*)", type_name)
            shared_from_class = symbol_map.find_class(shareds[0])

        file_path = typdef.find('anchorfile').text + "#" + typdef.find("anchor").text
        type_def_obj = SymbolMap.Typedef(name, type_name, file_path)

        if shared_from_class is not None and type(shared_from_class) == SymbolMap.Class:
        # if shared_from_class is not None:
            type_def_obj.sharedFrom = shared_from_class
            # let the class know that it has some typedefs associated with it
            shared_from_class.add_type_def(type_def_obj)

        symbol_map.typedefs[full_name] = type_def_obj
        typedef_list.append(type_def_obj)
    return typedef_list

def get_file_prefix(file_path):
    return os.path.splitext(os.path.basename(file_path))[0]


def get_file_extension(file_path):
    return os.path.splitext(os.path.basename(file_path))[1]


def get_file_name(file_path):
    return os.path.basename(file_path)


def parse_xml(in_path):
    """
    Opens the xml file and turns it into an ETree
    :param in_path:
    :return:
    """

    # print "parse : " + in_path
    tree = None
    try:
        with open(in_path, "rb") as xml_file:
            content = xml_file.read().decode("utf-8", errors="replace")
            new_content = content.encode("utf-8", errors="replace")
            parser = ET.XMLParser(encoding="utf-8")
            tree = ET.fromstring(new_content, parser)

    except:
        exc = sys.exc_info()[0]
        log("COULD NOT PARSE FILE: " + in_path, 2)
        log(exc, 2)
    return tree


def write_html(bs4, save_path):
    """
    Writes the html file to disk
    :param bs4:
    :param save_path:
    :return:
    """

    # prettify descriptions
    for markup in bs4.find_all("div", "description"):
        if type(markup) is Tag:
            pretty = BeautifulSoup(markup.prettify())
            if pretty is not None and markup is not None:
                markup.replaceWith(pretty)

    # convert entities in code blocks
    for c in bs4.find_all("code"):
        for child in c.children:
            # replaces with escaped code
            try:
                child_utf = unicode(child).encode("utf-8", errors="replace")
                child.replace_with(str(child_utf))
            except Exception as e:
                log("Writing HTML | " + str(e), 2)

    # enode bs4, decode, and then re-encode and write
    document = bs4.encode(formatter="html")
    document = codecs.decode(document, "utf-8", "xmlcharrefreplace")

    if not os.path.exists(os.path.dirname(save_path)):
        os.makedirs(os.path.dirname(save_path))
    with codecs.open(save_path, "w", "utf-8") as outFile:
        outFile.write(document)

def write_search_index():
    # save search index to js file
    document = "var search_index_data = " + json.dumps(g_search_index).encode('utf-8')
    # print document
    if not os.path.exists(os.path.dirname(HTML_DEST_PATH + 'search_index.js')):
        os.makedirs(os.path.dirname(HTML_DEST_PATH + 'search_index.js'))
    with codecs.open(HTML_DEST_PATH + 'search_index.js', "w", "UTF-8") as outFile:
        outFile.write(document)

def add_to_search_index(html, save_path, search_type, tags=[]):
    """
    Adds the html page to the search index
    :param html:
    :param save_path:
    :param search_type:
    :param tags:
    :return:
    """
    global g_search_index
    if not g_search_index:
        g_search_index = {"data": []}

    # creates new list from tags minus any dupes
    search_list = list(set(tags))

    search_obj = {"id": None, "title": None, "tags": []}
    search_obj["id"] = len(g_search_index["data"])
    search_obj["title"] = html.head.find("title").text if html.head.find("title") else ""
    search_obj["link"] = save_path
    search_obj["tags"] = search_list
    search_obj["type"] = search_type
    g_search_index["data"].append(search_obj)


def render_template(path, content):
    """
    Generates a BeautifulSoup instance from the template and injects content
    :param path:
    :param content:
    :return:
    """
    # try:
    # renderer = Renderer(file_encoding="utf-8", string_encoding="utf-8", decode_errors="xmlcharrefreplace")
    # renderer.search_dirs.append(TEMPLATE_PATH)
    # output = renderer.render_path(path, content)


    # print content
    # print path
    # step 1: render content in template
    content_renderer = Renderer(file_encoding="utf-8", string_encoding="utf-8", decode_errors="xmlcharrefreplace")
    content_renderer.search_dirs.append(TEMPLATE_PATH)
    output = content_renderer.render_path(path, content)

    # step 2: place rendered content into main template
    # - should have the following custom partials:
    #   - page title (define in object for page templates)
    #   - page content (rendered page content)
    #   - any other common partials that may lie outside the basic content area

    # loader = Loader()
    # template = loader.read("title")
    # title_partial = loader.load_name(os.path.join(CLASS_TEMPLATE_DIR, "title"))

    # except Exception as exc:
    #     print "\t**--------------------------------"
    #     print "\t** Warning: cannot render template"
    #     print "\t**--------------------------------"
    #     print exc
    #     print exc.message
    #     print(traceback.format_exc())
    #     exc_type, exc_obj, exc_tb = sys.exc_info()
    #     fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
    #     print(exc_type, fname, exc_tb.tb_lineno)
    #
    #     if config.BREAK_ON_STOP_ERRORS:
    #         quit()
    #     else:
    #         return

    bs4 = generate_bs4_from_string(output)
    return bs4


def get_file_type(file_prefix):
    """
    Determines the file type based on the file prefix
    :param file_prefix: prefix in file name
    :return: string indicating the type of file to parse
    """
    if is_class_type(file_prefix):
        return "class"
    elif is_namespace_type(file_prefix):
        return "namespace"
    elif is_module_type(file_prefix):
        return "module"


def is_class_type(class_str):
    """
    Tests whether the filename is a class type
    :param class_str:
    :return: Boolean
    """
    if any([class_str.startswith(prefix) for prefix in config.CLASS_FILE_PREFIXES]):
        return True
    return False


def is_namespace_type(ns_str):
    """
    Tests whether the filename is a namespace type
    :param class_str:
    :return: ns_str
    """
    if any([ns_str.startswith(prefix) for prefix in config.NAMESPACE_FILE_PREFIXES]):
        return True
    return False


def is_module_type(module_str):
    """
    Tests whether the filename is a group type
    :param module_str:
    :return: Boolean
    """
    if any([module_str.startswith(prefix) for prefix in config.GROUP_FILE_PREFIXES]):
        return True
    return False


def process_file(in_path, out_path=None):
    """ Generate documentation for a single file

        Args:
            inPath: The file to process
            outPath: The file to save the generated html file to
    """

    file_path = in_path
    file_prefix = get_file_prefix(file_path)
    is_html_file = True if get_file_extension(file_path).lower() == ".html" else False
    is_xml_file = True if get_file_extension(file_path).lower() == ".xml" else False

    if is_html_file:
        file_path = os.sep.join(in_path.split('htmlsrc'+os.sep)[1:])
        save_path = out_path if out_path is not None else HTML_DEST_PATH + file_path
    else:
        save_path = out_path if out_path is not None else HTML_DEST_PATH + get_file_prefix(in_path) + ".html"

    if is_html_file:
        # print "process: " + HTML_SOURCE_PATH + file_path
        process_html_file(HTML_SOURCE_PATH + file_path, save_path)

    elif is_xml_file:
        file_type = get_file_type(file_prefix)
        # process html directory always, since they may generate content for class or namespace reference pages
        if not state.processed_html_files and not args.skiphtml:
            process_html_dir(HTML_SOURCE_PATH)

        process_xml_file_definition(in_path, os.path.join(HTML_DEST_PATH, save_path), file_type)


def process_dir(in_path, out_path):
    """ Iterates a directory and generates documentation for each xml file
        in the directory as long as it is a class, struct or namespace

        Args:
            inPath: The directory to process
            outPath: The directory to save the generated html file to
    """

    for file_path in os.listdir(in_path):
        full_path = os.path.join(in_path, file_path)
        # if file_path.endswith(".xml"):

        if os.path.isfile(full_path):
            process_file(full_path)

        elif os.path.isdir(full_path):
            process_html_dir(full_path)


def process_html_dir(in_path):
    global state

    for path, subdirs, files in os.walk(in_path):
        path_dir = path.split(os.sep)[-1]
        if path_dir == "_templates" or path_dir == "assets":
            continue
        for name in files:
            # file_prefix = get_file_prefix(name)
            file_ext = get_file_extension(name).lower()
            if file_ext == ".html":

                if path.endswith(os.sep):
                    src_path = path[:-1]
                else:
                    src_path = path

                src_path = src_path + os.sep + name
                process_file(src_path)

    # add subnav for all guides that need them
    # process_sub_nav()

    state.processed_html_files = True


# def copyFiles( HTML_SOURCE_PATH, DOXYGEN_HTML_PATH ):
def copy_files():
    src = HTML_SOURCE_PATH
    dest = HTML_DEST_PATH

    try:
        copytree(src, dest, ignore=shutil.ignore_patterns("_templates*", "*.html"))
    except OSError as e:
        log('Directory not copied. Error:' + str(e))


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


def load_meta():
    global file_meta

    # load meta file
    meta_file = parse_xml(config.PROJECT_META_FILE)

    # get doxygen version
    file_meta["doxy_version"] = meta_file.attrib.get("version")

    # get cinder version
    for member in meta_file.findall(r'compounddef/sectiondef/memberdef[@kind="define"]'):
        if member.find(r"name").text == "CINDER_VERSION_STR":
            ver = str(member.find(r"initializer").text)
            ver = ver.replace('"', "")
            file_meta["cinder_version"] = ver

    # get docs directory
    file_meta["docs_root"] = args.root

    # include google analytics
    file_meta["include_analytics"] = args.include_analytics


def log(message, level=0, force=False):
    if level == 0 or not level:
        message_prefix = "INFO"
    elif level == 1:
        message_prefix = "WARNING"
    elif level == 2:
        message_prefix = "ERROR"

    if args.debug or force:
        print("\r    *** " + message_prefix + ": [ " + message + " ] ***")


def log_progress(message):
    sys.stdout.write('\r' + str(message))
    sys.stdout.write("\033[K")
    sys.stdout.flush()


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

    args = parser.parse_args()

    # Make sure we're compiling using pythong 2.7.6+
    version_info = sys.version_info

    #if version_info.major >= 2 and version_info.minor >= 7 and version_info.micro < 6:
    #    sys.exit("ERROR: Sorry buddy, you must use python 2.7.6+ to generate documentation. Visit https://www.python.org/downloads/ to download the latest.")
    # if sys.version

    if args.path:
        inPath = args.path
        if not os.path.isfile(inPath) and not os.path.isdir(inPath):
            log("Nice try! Directory or file '" + inPath + "' doesn't even exist, so we're going to stop right... now!", True)
            quit()

    if not os.path.exists(TAG_FILE_PATH):
        log("I got nothin' for you. The tag file [" + TAG_FILE_PATH + "] doesn't exist yet. "
            "Run Doxygen first and try me again later.", 2, True)
        quit()

    # load meta data
    load_meta()

    # Load tag file
    log("parsing tag file", 0, True)
    g_tag_xml = ET.ElementTree(ET.parse(TAG_FILE_PATH).getroot())
    # generate symbol map from tag file
    g_symbolMap = get_symbol_to_file_map()

    # copy files from htmlsrc/ to html/
    log("copying files", 0, True)
    copy_files()

    # generate namespace navigation
    g_namespaceNav = generate_namespace_nav()

    log("processing files", 0, True)
    if not args.path: # no args; run all docs
        # process_html_dir(HTML_SOURCE_PATH, "html/")
        process_dir("xml" + os.sep, "html" + os.sep)

        # save search index to json file
        write_search_index()
        log("SUCCESSFULLY GENERATED CINDER DOCS!", 0, True)
    elif args.path:
        inPath = args.path
        # process a specific file
        if os.path.isfile(inPath):
            process_file(inPath, args.outpath if len(sys.argv) > 2 else None)
            log("SUCCESSFULLY GENERATED YOUR FILE!", 0, True)
        elif os.path.isdir(inPath):
            if inPath == "htmlsrc" + os.sep:
                process_html_dir(HTML_SOURCE_PATH)
            else:
                process_dir(inPath, "html" + os.sep)
            log("SUCCESSFULLY GENERATED YOUR FILES!", 0, True)
    else:
        log("Unknown usage", 1, True)

