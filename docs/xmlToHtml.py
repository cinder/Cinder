#! /usr/bin/python
import sys
# import os
import codecs
# import shutil
import copy
import re
import xml.etree.ElementTree as Et
import os
import shutil
import stat
from bs4 import BeautifulSoup, Tag, NavigableString
# from distutils.dir_util import copy_tree

XML_SOURCE_PATH = os.path.dirname(os.path.realpath(__file__)) + os.sep + 'xml' + os.sep
DOXYGEN_HTML_PATH = os.path.dirname(os.path.realpath(__file__)) + os.sep + 'html' + os.sep
HTML_SOURCE_PATH = os.path.dirname(os.path.realpath(__file__)) + os.sep + 'htmlsrc' + os.sep
TEMPLATE_PATH = os.path.dirname(os.path.realpath(__file__)) + os.sep + 'htmlsrc' + os.sep + "_templates" + os.sep
PARENT_DIR = os.path.dirname(os.path.realpath(__file__)).split('/docs')[0]
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
    "class": "<T> Class Template Reference",
    "struct": "Struct Reference"
}

tagXml = None
g_symbolMap = None
g_currentFile = None


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
        def __init__(self, name, base, file_name):
            self.name = name
            self.base = base
            self.path = file_name
            self.functionList = []
            self.relatedLinks = []

        def add_related_link(self, a_link):
            self.relatedLinks.append(a_link)

    class Namespace(object):
        def __init__(self, name, file_name):
            self.name = name
            self.path = file_name

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
                self.githubPath = GITHUB_PATH + self.path.split(PARENT_DIR)[1]

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

            return None

    def find_namespace(self, name):
        return self.namespaces.get(name)

    def get_ordered_namespaces(self):
        # create an array of strings that include all of the namespaces and return
        # the array in alphabetical order
        namespaces = []
        for nsKey in self.namespaces:
            ns = self.namespaces[nsKey]
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
        if class_obj is not None:
            # iterate through class functions
            for fn in class_obj.functionList:
                if fn.name == name:
                    return fn
        else:
            return self.functions.get(name)

    def find_file(self, name):
        return self.files.get(name)

    def find_file_typedefs(self, name):
        return self.find_file(name).typedefs

    def find_enum(self, name):
        searchname = str(name)
        if searchname.find("ci::") == 0:
            searchname = searchname.replace("ci::", "cinder::")

        # same key as name
        if searchname in self.enums:
            return self.enums[searchname]

        # key with "cinder::" prepended
        elif ("cinder::" + searchname) in self.enums:
            return self.enums["cinder::" + searchname]

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


class FileData(object):
    def __init__(self, tree, bs4):

        self.tree = tree  # xml file that describes the page
        self.bs4 = bs4    # html file of the actual page

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

        # find any common html elements and populate common elements
        self.parse_template()

    def parse_template(self):
        self.contentsEl = self.bs4.find("div", "contents")
        if self.kind == "class" or self.kind == "struct":
            self.descriptionEl = self.bs4.find(id="description")
            self.descriptionProseEl = self.descriptionEl.find("div", "prose")
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
        header_tag.string = self.compoundName + " " + headerDictionary[self.kind]

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
    print "FIND FILE KIND: " + kind
    return kind


def find_compound_name_stripped(tree):
    compound_name = find_compound_name(tree)
    name = strip_compound_name(compound_name)
    return name


def strip_compound_name(full_string):
    ns_parts = full_string.split("::")
    # prefix = "::".join(ns_parts[:-1])  # parent namespace up to last ::
    name = "".join(ns_parts[-1])
    return name


def add_class_to_tag(tag, className):
    tag["class"] = tag.get("class", []) + [className]


def gen_anchor_tag(bs4, anchorName):
    anchor = gen_tag(bs4, "a")
    anchor["name"] = anchorName
    return anchor


def gen_tag(bs4, tagType, classes=None, contents=None):
    """ Generates a new html element and optionally adds classes and content

    Args:
        bs4:        beautiful soup
        tagType:    html tag/element (p, a, em, etc)
        classes:    array of strings that you want as classes for the element
        contents:   any content that you want to populate your tag with, if known
    """

    newTag = bs4.new_tag(tagType)

    if classes:
        for c in classes:
            add_class_to_tag(newTag, c)

    if contents:
        newTag.append(contents)

    return newTag


def gen_link_tag(bs4, text, link):
    linkTag = gen_tag(bs4, "a", [], text)
    define_link_tag(linkTag, {"href": link})
    return linkTag


def define_link_tag(tag, attrib):
    refId = None
    kind = None
    href = None

    if ("refid" in attrib):
        refId = attrib["refid"]
        href = refId + ".html"

    if ("kindref" in attrib):
        kind = attrib["kindref"]

        if kind == "member":
            strList = refId.rsplit("_1", 1)
            href = strList[0] + ".html#" + strList[1]

    if "linkid" in attrib:
        href = "../../include/cinder/" + attrib["linkid"]

    if "href" in attrib:
        href = attrib["href"]

    if "typedef" in attrib:
        data = attrib["typedef"]
        fileName = data.find("anchorfile").text
        anchor = data.find("anchor").text
        href = fileName + "#" + anchor

    if href is None:
        print "ERROR DEFINING LINK TAG"
    else:
        tag["href"] = href


def markup_function(bs4, fnXml, parent, isConstructor):
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

    li = gen_tag(bs4, "li", ["row"])

    # wrapper
    functionDiv = gen_tag(bs4, "div", ["functionDef"])
    li.append(functionDiv)

    # left side / return type
    if not isConstructor:
        returnDiv = gen_tag(bs4, "div", ["returnCol columns large-3"])
        iterate_markup(bs4, fnXml.find(r"type"), returnDiv)
        functionDiv.append(returnDiv)

    # right side (function name and description)
    definitionCol = gen_tag(bs4, "div", ["definitionCol columns"])

    # width is dependent on if it has the return column
    if isConstructor:
        add_class_to_tag(definitionCol, "large-12")
    else:
        add_class_to_tag(definitionCol, "large-9")

    functionDiv.append(definitionCol)

    # function name
    definitionDiv = gen_tag(bs4, "div", ["definition"])

    name = fnXml.find("name").text
    emTag = gen_tag(bs4, "em", [], name)
    definitionDiv.append(emTag)

    argstring = fnXml.find("argsstring")
    if argstring is None:
        argstring = fnXml.find("arglist")
    argstringText = argstring.text if argstring.text is not None else ""

    definitionDiv.append(argstringText)
    definitionCol.append(definitionDiv)

    # detailed description
    descriptionDiv = markup_description(bs4, fnXml)
    if descriptionDiv is not None:
        definitionCol.append(descriptionDiv)
        add_class_to_tag(li, "expandable")
    # else :
    # print "NO DESCRIPTION"

    id = fnXml.attrib["id"].split("_1")[-1]
    li.append(gen_anchor_tag(bs4, id))
    parent.append(li)


def markup_enum(bs4, fnXml, parent):
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
    enumDiv = gen_tag(bs4, "div", ["enumDef"])
    li.append(enumDiv)

    # left side / return type
    leftDiv = gen_tag(bs4, "div", ["returnCol columns large-2"])
    leftDiv.append("enum")

    # right side (function name and description)
    rightDiv = gen_tag(bs4, "div", ["definitionCol columns"])
    add_class_to_tag(rightDiv, "large-12")

    enumDiv.append(rightDiv)

    # function name
    definitionDiv = gen_tag(bs4, "div", ["definition"])

    emTag = gen_tag(bs4, "em", [], fnXml.find("name").text)
    definitionDiv.append(emTag)
    rightDiv.append(definitionDiv)

    # detailed description
    descriptionDiv = markup_description(bs4, fnXml)

    # iterate all of the enumvalues
    enumUl = gen_tag(bs4, "ul")
    for enum in fnXml.findall("enumvalue"):
        enumLi = gen_tag(bs4, "li", [], enum.find("name").text)
        enumUl.append(enumLi)

    if descriptionDiv is None and len(list(enumUl.children)) > 0:
        descriptionDiv = gen_tag(bs4, "div", ["description", "content"])
        descriptionDiv.append(enumUl)

    if descriptionDiv is not None:
        rightDiv.append(descriptionDiv)
        add_class_to_tag(li, "expandable")

    id = fnXml.attrib["id"].split("_1")[-1]
    li.append(gen_anchor_tag(bs4, id))
    parent.append(li)


def define_tag(bs4, tagName, tree):
    newTag = bs4.new_tag(tagName)
    if tagName == "a":
        define_link_tag(newTag, tree.attrib)
    # elif tagName == "code":
    # addClassToTag( newTag, "language-cpp" )
    return newTag


def gen_includes(fileDef):
    bs4 = g_currentFile.bs4

    # create html from template
    side = get_template(bs4, "side-static")
    contentDiv = side.find("div", "content")

    # fill heading
    side.find('h4').append("#include")

    # add include link
    includeLink = gen_tag(bs4, "a", None, fileDef.name)
    path = fileDef.githubPath
    # if path is None:
    #     fileDef.path
    define_link_tag(includeLink, {'href': path})
    contentDiv.append(includeLink)

    g_currentFile.append_to_side_el(side)


def gen_typedefs(bs4, typeDefs):
    """ Generates the typedefs side bar, with each linking out
        to its full definition.

    Args:
        bs4: The current beautifulSoup html instance
        typeDefs: An array of SymbolMap::TypeDef Objects

    Returns:
        Empty if the typeDefs list is empty
    """

    # return if empty array
    if len(typeDefs) == 0:
        return

    # create html from template
    side = get_template(bs4, "side-expandable")
    contentDiv = side.find("div", "content")

    # fill heading
    side.find('h4').append("Typedefs:")

    # fill list of typedefs
    typeDefUl = gen_tag(bs4, "ul")
    for typeDef in typeDefs:
        typeDefLi = gen_tag(bs4, "li")
        aTag = gen_tag(bs4, "a", [], typeDef.name)
        define_link_tag(aTag, {"href": typeDef.path})
        typeDefLi.append(aTag)
        typeDefUl.append(typeDefLi)

    # plug into html
    if typeDefUl is not None:
        contentDiv.append(typeDefUl)

    # append to the current file's side element
    g_currentFile.append_to_side_el(side)


def iter_class_base(classDef, hierarchy):
    """ Iterates the class to find all of their base classes
        and iterate through them

    Args:
        classDef: The instance of SymbolMap::Class Object whose base we are searching for
        hierachy: The current hierachy of classes to append to if we find another base
    """

    if classDef is None or hasattr(classDef, 'name') == False:
        return False

    base = classDef.base

    if base is None:
        return False
    else:
        newTree = g_symbolMap.find_class(base)
        # add to hierarchy if it continues
        if (iter_class_base(newTree, hierarchy) is not False):
            hierarchy.append(newTree)


def gen_class_hierarchy(bs4, classDef):
    """ Generates the class heirarchy side bar, with each class linking
        out to its class file.

    Args:
        bs4: The current beautifulSoup html instance
        classDef: The instance of SymbolMap::Class Object that we are generating
            the hierachy for

    Returns:
        Empty if there is no base class
    """

    if classDef is None:
        return

    # first item in the list will be the original class
    hierarchy = [classDef]

    # get the class' hierarchy
    iter_class_base(classDef, hierarchy)

    if len(hierarchy) == 1:
        return

    # create html from template
    side = get_template(bs4, "side-expandable")

    # fill heading
    side.find('h4').append("Class Heirarchy:")

    # create all of the markup
    contentDiv = side.find('div', 'content')
    ul = gen_tag(bs4, "ul")
    add_class_to_tag(ul, "inheritence")
    contentDiv.append(ul)

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

    if len(hierarchy) > 2:
        print "GEN CLASS HIERARCHY " + classDef.name
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
    classesUl = gen_tag(bs4, "ul", None)
    contentDiv = side.find('div', 'content')
    for classDef in classes:
        li = gen_tag(bs4, "li")
        classesUl.append(li)

        a = gen_tag(bs4, "a", [], strip_compound_name(classDef.text))
        define_link_tag(a, classDef.attrib)
        li.append(a)

    contentDiv.append(classesUl)

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


def replace_tag(bs4, tree, parentTag, content):
    tag = tree.tag
    attrib = tree.attrib
    hasParent = False
    contentTag = None

    if parentTag and parentTag.parent:
        hasParent = True

    # change parentTag if necessary
    if tag == "codeline":
        parentTag = parentTag.code

    # find html tag based on tag
    if tag == "para":
        if hasParent and parentTag.parent.dl:
            tagName = "dd"
        else:
            tagName = tagDictionary[tag]
    elif tag == "sp":
        if content is None:
            content = " "
        else:
            content.append(" ")

    # get tag equivalent
    if tag in tagDictionary:
        tagName = tagDictionary[tag]
        newTag = define_tag(bs4, tagName, tree)
    else:
        # TODO: replace with nothing - no new tag
        tagName = "div"
        newTag = define_tag(bs4, tagName, tree)
        add_class_to_tag(newTag, tag)

    contentTag = newTag

    # if simplesect, construct with some content
    if (tag == "simplesect"):
        seeTag = bs4.new_tag("dt")
        add_class_to_tag(seeTag, "section")

        # "see also" reference
        if (attrib["kind"] == "see"):
            add_class_to_tag(seeTag, "see")
            seeTag.string = "See Also"
        newTag.append(seeTag)

    if tag == "programlisting":
        codeTag = bs4.new_tag("code")
        add_class_to_tag(codeTag, "language-cpp")
        newTag.append(codeTag)
        contentTag = codeTag

    if content is not None:
        contentTag.append(content)

    parentTag.append(newTag)
    return newTag


def iterate_markup(bs4, tree, parent):
    if tree == None:
        return

    origTag = tree.tag
    currentTag = parent
    content = None

    # add content to tag
    if tree.text != None:
        # currentTag.append( tree.text.strip() )
        content = tree.text.strip()

    # append any new tags
    if tree.tag != None:
        htmlTag = replace_tag(bs4, tree, currentTag, content)

        # if tree parent == <p> && newTag == <pre>
        # add a new pre tag in and make that the current parent again
        currentTag = htmlTag

    # iterate through children tags
    for child in list(tree):
        output = iterate_markup(bs4, child, currentTag)

    # tail is any extra text that isn't wrapped in another tag
    # that exists before the next tag
    if tree.tail != None:
        parent.append(tree.tail.strip())

        if tree.tail.endswith(";"):
            parent.append(gen_tag(bs4, "br"))

    return currentTag


def markup_description(bs4, tree):
    description_el = gen_tag(bs4, "div", ["description", "content"])
    briefDesc = tree.find(r'briefdescription/')
    if briefDesc is None:
        return
    else:
        iterate_markup(bs4, tree.find(r'briefdescription/'), description_el)

    detailedDesc = tree.find(r'detaileddescription/')
    if detailedDesc is not None:
        iterate_markup(bs4, tree.find(r'detaileddescription/'), description_el)

    return description_el


def replace_code_chunks(bs4):
    """ Looks though the html and replaces any code chunks that exist
        in a paragraph and splits them up so that we can use pre tags.
    """
    # find all p tags
    pTags = bs4.find_all("p")
    for p in pTags:

        # if the paragraph as a "div.computeroutput"
        # codeChunks = p.find_all( "div", "computeroutput" )
        codeChunks = p.find_all("div", "programlisting")

        if (len(codeChunks) > 0):

            newTags = []
            curP = None

            for c in p.contents:

                # create a new paragraph to wrap content in
                if curP is None:
                    curP = gen_tag(bs4, "p")
                    newTags.append(curP)

                addToP = True

                if type(c) == Tag:
                    if c.has_attr("class"):

                        # look for any tags with "computeroutput"
                        if "programlisting" in c["class"]:

                            # add a new pre
                            preTag = gen_tag(bs4, "pre")

                            for code in c.contents:

                                # clone code
                                cClone = clone(code)

                                # code string
                                codeString = ""
                                if type(code) == Tag:

                                    # clear the original tag
                                    code.clear()

                                    # add clone to holder so that we can iterate through
                                    holder = gen_tag(bs4, "div")
                                    holder.append(cClone)

                                    # append line breaks for each line of code
                                    for content in holder.code.contents:
                                        codeString += content.getText() + "\n"

                                if len(cClone) > 0:
                                    code.append(codeString)
                                    preTag.append(code)

                            newTags.append(preTag)

                            # create a new p tag for any following content to add itself to
                            curP = gen_tag(bs4, "p")
                            newTags.append(curP)
                            addToP = False

                if addToP:
                    cClone = clone(c)
                    curP.append(cClone)

            # replace p tag with an empty span
            replacementTag = gen_tag(bs4, "span")
            p.replace_with(replacementTag)

            # add the newly organized tags to the new span
            for t in newTags:
                replacementTag.append(t)

            # unwrap / remove the span since that was only there to place this new content into
            replacementTag.unwrap()


# clone an element
# from: http://stackoverflow.com/questions/23057631/clone-element-with-beautifulsoup/23058678#23058678
def clone(el):
    if isinstance(el, NavigableString):
        return type(el)(el)

    copy = Tag(None, el.builder, el.name, el.namespace, el.nsprefix)
    # work around bug where there is no builder set
    # https://bugs.launchpad.net/beautifulsoup/+bug/1307471
    copy.attrs = dict(el.attrs)
    copy.index = el.index
    for attr in ('can_be_empty_element', 'hidden'):
        setattr(copy, attr, getattr(el, attr))
    for child in el.contents:
        copy.append(clone(child))
    return copy


# pull a templated chunk of html out of the selected bs4 file
def get_template(bs4, elementId):
    templates = bs4.find_all('template')
    template = None

    for t in templates:
        # [0] is a string before the enclosed div, so used [1] instead
        if t['id'] == elementId:
            template = clone(list(t.contents)[1])
        else:
            continue

    return template


def iterate_namespace(bs4, namespaces, tree, index, label):
    # Get namespace of previous child, unless first
    if index == 0:
        parentNs = ""
    else:
        parentNs = namespaces[index - 1].name

    thisNamespace = namespaces[index].name

    count = index
    childCount = 0

    # iterate to find all children of parentNs
    for ns in namespaces[index:]:
        namespace = ns.name  # full namespace
        nsParts = namespace.split("::")
        prefix = "::".join(nsParts[:-1])  # parent namespace up to last ::
        name = "".join(nsParts[-1])
        nodeLabel = label + str(childCount)

        # check if derived from any parent
        parentIsDerived = has_ancestor(namespaces, namespace)

        # create a list item for the namespace
        nsLi = gen_tag(bs4, "li")

        # create link for each item
        aTag = gen_tag(bs4, "a")
        define_link_tag(aTag, {"href": ns.path})
        aTag.append(name)

        # is decendent of parent namespace
        if prefix == parentNs:

            childCount += 1

            # append to parent
            tree.append(nsLi)

            # generate new nested ul in case there are children
            nsUl = gen_tag(bs4, "ul")
            if count + 1 < len(namespaces):

                # if there are children, add to the parent ul
                if iterate_namespace(bs4, namespaces, nsUl, count + 1, nodeLabel) > 0:
                    # addClassToTag( nsLi, "item" )
                    # add input
                    inputEl = gen_tag(bs4, "input")
                    inputEl["type"] = "checkbox"
                    inputEl["id"] = "item-" + "-".join(list(nodeLabel))
                    labelTag = gen_tag(bs4, "label")
                    labelTag["for"] = "item-" + "-".join(list(nodeLabel))
                    labelTag.append(aTag)

                    nsLi.insert(0, inputEl)
                    nsLi.append(labelTag)
                    nsLi.append(nsUl)
                else:
                    nsLi.append(aTag)

        else:
            # has no direct descendent on the parent, so add it independently
            if parentIsDerived == False and index == 0:
                childCount += 1
                indieLi = gen_tag(bs4, "li")
                # indieLi.append( prefix )

                # TODO: refactor and simplify some of this stuff
                inputEl = gen_tag(bs4, "input")
                inputEl["type"] = "checkbox"
                inputEl["id"] = "item-" + "-".join(list(nodeLabel))
                indieLi.insert(0, inputEl)

                labelTag = gen_tag(bs4, "label")
                labelTag["for"] = "item-" + "-".join(list(nodeLabel))
                labelTag.append(prefix)
                indieLi.append(labelTag)

                indieUl = gen_tag(bs4, "ul")
                indieLi.append(indieUl)
                indieUl.append(nsLi)
                nsLi.append(aTag)

                tree.append(indieLi)

        count += 1

    return childCount


def has_ancestor(namespaces, compareNamespace):
    comparePrefix = "::".join(compareNamespace.split("::")[0])
    hasAncestor = False
    for ns in namespaces:
        namespace = ns.name
        prefix = "::".join(namespace.split("::")[0])
        if prefix == comparePrefix and compareNamespace != namespace:
            return True

    return False


def generate_namespace_nav(bs4):
    namespaces = g_symbolMap.get_ordered_namespaces()

    tree = gen_tag(bs4, "div")
    ul = gen_tag(bs4, "ul")
    tree.append(ul)
    add_class_to_tag(tree, "css-treeview")

    # for ns in namespaces :
    # namespace = ns.find('name').text

    iterate_namespace(bs4, namespaces, ul, 0, "")
    return tree


def process_class_xml_file(inPath, outPath, html):
    global g_currentFile
    print "Processing file: " + inPath + " > " + outPath

    # define the tree that contains all the data we need to populate this page
    tree = parse_html(html, inPath, outPath)
    if tree == None:
        return

    # get common data for the file
    g_currentFile = FileData(tree, html)
    compoundName = g_currentFile.compoundName
    className = g_currentFile.name
    kind = g_currentFile.kind

    # dictionary for subnav anchors
    subnavAnchors = []

    # find include name (header file)
    includeTrees = tree.findall(r"compounddef/includes")
    includeDef = None
    if len(includeTrees) > 0:
        includeDef = tree.findall(r"compounddef/includes")[0].text

    # find file definition for the include file
    fileDef = None
    if includeDef != None:
        fileDef = g_symbolMap.find_file(includeDef)

    # find class definition
    classDef = g_symbolMap.find_class(className)

    # +-----------------------------------------+
    #  find parts in template that already exist
    # +-----------------------------------------+
    contentsEl = g_currentFile.contentsEl
    sideNavEl = g_currentFile.sideNavEl

    # +-------------+
    #  Namespace Nav
    # +-------------+
    sideNavEl.append(g_namespaceNav)

    # +---------------+
    #  Side Area Nodes
    # +---------------+
    # includes
    if includeDef != None:
        gen_includes(fileDef)

    # typedefs
    if fileDef != None:
        gen_typedefs(html, fileDef.typedefs)
    # class hierarchy
    gen_class_hierarchy(html, classDef)
    # class list
    gen_class_list(html, tree)
    # related links (generated by guides and references)
    gen_related_links(html, classDef)
    # print classDef.seeMoreLinks

    # +-----------+
    #  Description
    # +-----------+
    # description prose
    descTag = markup_description(html, tree.find(r'compounddef'))
    if descTag is not None:
        g_currentFile.descriptionProseEl.append(descTag)

    # +----------------+
    #  Member Functions
    # +----------------+
    # create regular and static member function ul wrappers
    ulTag = html.new_tag("ul")
    staticUlTag = html.new_tag("ul")
    add_class_to_tag(staticUlTag, "static")

    funcCount = 0
    staticFuncCount = 0

    # public member functions
    for memberFn in tree.findall(r'compounddef/sectiondef/memberdef[@kind="function"][@prot="public"]'):
        # split between static or not
        isStatic = memberFn.attrib["static"]
        memberName = memberFn.find(r"name")
        isConstructor = False

        # determine if it is a constructor
        if memberName is not None and memberName.text == className:
            isConstructor = True

        # determine if static
        if isStatic == 'yes':
            staticFuncCount += 1
            markup_function(html, memberFn, staticUlTag, isConstructor)
        else:
            funcCount += 1
            markup_function(html, memberFn, ulTag, isConstructor)

    # if function count > 0 add it
    if funcCount > 0:
        # add anchor tag to page
        anchor = gen_anchor_tag(html, "public-member-functions")
        subnavAnchors.append({"name": "Public Member Functions", "link": anchor})
        contentsEl.append(anchor)

        # public member functions
        header = html.new_tag("h1")
        header.string = "Public Member Functions"
        sectionTag = gen_tag(html, "section")
        sectionTag.append(header)
        sectionTag.append(ulTag)
        contentsEl.append(sectionTag)
        sectionTag.append(gen_tag(html, "hr"))

    # if static function count > 0 add it
    if staticFuncCount > 0:
        anchor = gen_anchor_tag(html, "static-public-member-functions")
        subnavAnchors.append({"name": "Static Public Member Functions", "link": anchor})
        contentsEl.append(anchor)

        # static public member functions
        header = html.new_tag("h1")
        header.string = "Static Public Member Functions"
        sectionTag = gen_tag(html, "section")
        sectionTag.append(header)
        sectionTag.append(staticUlTag)
        contentsEl.append(sectionTag)

    # replace any code chunks with <pre> tags, which is not possible on initial creation
    replace_code_chunks(html)

    # generate subnav
    gen_sub_nav(subnavAnchors)

    # write the file
    write_html(html, outPath)


def gen_sub_nav(subnavAnchors):
    # +-------------------------------------------------+
    #  SubNav
    #  Fill subnav based on what is actually in the page
    # +-------------------------------------------------+
    bs4 = g_currentFile.bs4
    subnavEl = bs4.find(id="sub-nav")
    subnavUl = gen_tag(bs4, "ul")

    # for all of the subnav anchors, add a link into the subnav list
    for anchor in subnavAnchors:
        li = gen_tag(bs4, "li")
        link = gen_tag(bs4, "a", [], anchor["name"])
        define_link_tag(link, {"href": "#" + anchor["link"]["name"]})
        li.append(link)
        subnavUl.append(li)
    subnavEl.append(subnavUl)


def add_row_li(bs4, container, leftContent, rightContent, colBreakdown=None, dropdownContent=None):
    leftCol = "3"
    rightCol = "9"
    if colBreakdown is not None:
        cols = colBreakdown.split("-")
        leftCol = cols[0]
        rightCol = cols[1]

    li = gen_tag(bs4, "li", ["row"])

    # left side
    leftDiv = gen_tag(bs4, "div", ["returnCol columns"])
    add_class_to_tag(leftDiv, "large-" + leftCol)
    if leftContent is not None:
        leftDiv.append(leftContent)
        li.append(leftDiv)

    # right side
    rightDiv = gen_tag(bs4, "div", ["definitionCol columns"])
    add_class_to_tag(rightDiv, "large-" + rightCol)
    li.append(rightDiv)
    rightDiv.append(rightContent)

    container.append(li)

    # if there is dropdown content, the right side will have an additional row below it
    if dropdownContent is not None:
        rightDiv.append(dropdownContent)
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

    # get common data for the file
    g_currentFile = FileData(tree, html)
    compoundName = g_currentFile.compoundName
    className = g_currentFile.name
    kind = g_currentFile.kind

    # dictionary for subnav anchors
    subnavAnchors = []

    # +-----------------------------------------+
    #  find parts in template that already exist
    # +-----------------------------------------+
    # contentsTag = html.find( "div", "contents" )
    contentsEl = g_currentFile.contentsEl

    # +-------------+
    #  Namespace Nav
    # +-------------+
    # side area
    sideNavTag = html.find(id="side-nav")
    sideNavTag.append(g_namespaceNav)

    # +-----------+
    #  Description
    # +-----------+
    namespaceTag = g_symbolMap.find_namespace(compoundName)
    if namespaceTag is None:
        return

    # +----------+
    #  Namespaces
    # +----------+
    namespaces = tree.findall(r"compounddef/innernamespace")
    if len(namespaces) > 0:
        drop_anchor(subnavAnchors, "namespaces", "Namespaces")
        namespacesSection = gen_tag(html, "section")
        contentsEl.append(namespacesSection)

        namespacesUl = gen_tag(html, "ul")
        for c in namespaces:
            namespaceLinkTag = gen_link_tag(html, c.text, c.attrib["refid"] + ".html")
            add_row_li(html, namespacesUl, None, namespaceLinkTag)

        namespacesSection.append(gen_tag(html, "h2", [], "Namespaces"))
        namespacesSection.append(namespacesUl)

        contentsEl.append(gen_tag(html, "br"))

    # +-------+
    #  Classes
    # +-------+
    classes = tree.findall(r"compounddef/innerclass")
    if len(classes) > 0:
        drop_anchor(subnavAnchors, "classes", "Classes")
        classesSection = gen_tag(html, "section")
        contentsEl.append(classesSection)

        classesSection.append(gen_tag(html, "h2", [], "Classes"))
        classUl = gen_tag(html, "ul")
        for c in classes:
            link = c.attrib["refid"] + ".html"
            kind = "struct" if link.startswith("struct") else "class"
            linkTag = gen_link_tag(html, strip_compound_name(c.text), link)
            add_row_li(html, classUl, kind, linkTag, "1-11")
        classesSection.append(classUl)

        contentsEl.append(gen_tag(html, "br"))

    # +--------+
    #  TypeDefs
    # +--------+
    typedefs = tree.findall(r"compounddef/sectiondef/[@kind='typedef']/memberdef/[@kind='typedef']")
    if len(typedefs) > 0:
        drop_anchor(subnavAnchors, "typedefs", "Typedefs")
        typedefSection = gen_tag(html, "section")
        contentsEl.append(typedefSection)

        typedefSection.append(gen_tag(html, "h2", [], "Typedefs"))
        typedefUl = gen_tag(html, "ul")
        for c in typedefs:
            markup_function(html, c, typedefUl, False)
        typedefSection.append(typedefUl)

        contentsEl.append(gen_tag(html, "br"))

    # +------------+
    #  Enumerations
    # +------------+
    enumerations = tree.findall(r"compounddef/sectiondef/[@kind='enum']/memberdef/[@kind='enum']")
    if len(enumerations) > 0:
        drop_anchor(subnavAnchors, "enumerations", "Enumerations")
        enumSection = gen_tag(html, "section")
        contentsEl.append(enumSection)

        enumSection.append(gen_tag(html, "h2", [], "Enumerations"))
        enumUl = gen_tag(html, "ul")
        for c in enumerations:
            markup_enum(html, c, enumUl)
        enumSection.append(enumUl)

        contentsEl.append(gen_tag(html, "br"))

    # +---------+
    #  Functions
    # +---------+
    functions = tree.findall(r"compounddef/sectiondef/[@kind='func']/memberdef/[@kind='function']")
    if len(functions) > 0:
        drop_anchor(subnavAnchors, "functions", "Functions")
        functionSection = gen_tag(html, "section")
        contentsEl.append(functionSection)

        functionSection.append(gen_tag(html, "h2", [], "Functions"))
        functionUl = gen_tag(html, "ul")
        for c in functions:
            markup_function(html, c, functionUl, False)
        functionSection.append(functionUl)

        contentsEl.append(gen_tag(html, "br"))

    # +---------+
    #  Variables
    # +---------+
    vars = tree.findall(r"compounddef/sectiondef/[@kind='var']/memberdef/[@kind='variable']")
    if len(vars) > 0:
        drop_anchor(subnavAnchors, "variables", "Variables")
        varsSection = gen_tag(html, "section")
        contentsEl.append(varsSection)

        varsSection.append(gen_tag(html, "h2", [], "Variables"))
        varsUl = gen_tag(html, "ul")
        for c in vars:
            typeStr = c.find('type').text
            name = gen_tag(html, "div")
            name.append(gen_tag(html, "b", [], c.find('name').text))
            initializer = c.find('initializer').text if c.find('initializer') is not None else None
            description = markup_description(html, c)
            if typeStr == None:
                typeStr = ""
            if initializer is not None:
                name.append(" " + initializer)
            add_row_li(html, varsUl, typeStr, name, None, description)
        varsSection.append(varsUl)

    gen_sub_nav(subnavAnchors)

    # write the file
    write_html(html, out_path)


def process_html_file(inPath, outPath):
    """ Parses an html file.
    - Adds template around the html
    - Copy original css and js links into new hmtl
    - Save html in destination dir
    """
    print "processHtmlFile"

    htmlTemplate = "htmlContentTemplate.html"
    isIndex = False
    if inPath.find("reference/") > -1:
        htmlTemplate = "referenceContentTemplate.html"
    elif inPath.find("guides/") > -1:
        htmlTemplate = "guidesContentTemplate.html"

    # construct template

    if inPath == "htmlsrc/index.html":
        isIndex = True

    if isIndex is True:
        bs4 = generate_bs4(inPath)
    else:

        bs4 = construct_template(["headerTemplate.html", "mainNavTemplate.html", htmlTemplate, "footerTemplate.html"])

        # parse original html file
        origHtml = generate_bs4(inPath)

        # replace all of the bs4 css and js links and make them relative to the outpath
        for link in bs4.find_all("link"):
            if link.has_attr("href"):
                link["href"] = update_link(link["href"], outPath)

        for a in bs4.find_all("a"):
            if a.has_attr("href"):
                a["href"] = update_link(a["href"], outPath)

        for script in bs4.find_all("script"):
            if script.has_attr("src"):
                script["src"] = update_link(script["src"], outPath)

        for img in bs4.find_all("img"):
            if img.has_attr("src"):
                img["src"] = update_link(img["src"], outPath)

        # plug original html content into template
        template_content_el = bs4.body.find(id="template-content")
        template_content_el.append(origHtml.body)

        # fill namespace list
        if inPath == "htmlsrc/namespaces.html":
            # throw in a list of namespaces into the page
            list_namespaces(bs4, template_content_el)

        # fill class list
        elif inPath == "htmlsrc/classes.html":
            list_classes(bs4, template_content_el)

        # copy all js and css paths that may be in the original html and paste into new file
        for link in origHtml.find_all("link"):
            bs4.head.append(link)

        for script in origHtml.find_all("script"):
            bs4.body.append(script)

        if origHtml.head:
            for d in origHtml.head.find_all("d"):
                bs4.head.append(d)

        # link up all d tags
        for tag in bs4.find_all('d'):
            process_d_tag(bs4, tag, outPath)

        # for tag in origHtml.find_all('d'):
        #     print tag
        #     if tag.has_attr("seealso"):
        #
        #         print "FOUND ONE"
        #         process_d_ref_tag(bs4, tag)
        #         # set page data
        #         # setSection( bs4, "" )

    write_html(bs4, outPath)


def process_d_tag( bs4, tag, outPath ):

    # find type
    if tag.has_attr("seealso"):
        process_d_seealso_tag(bs4, tag, outPath)
    elif tag.has_attr("prefix"):
        print "TODO: PARSE PREFIX D TAGS"
    else:
        replace_d_tag(bs4, tag)

def replace_d_tag(bs4, link):

    ref_obj = find_d_tag_ref(link)

    if ref_obj:
        ref_location = DOXYGEN_HTML_PATH + ref_obj.path
        new_link = gen_link_tag(bs4, link.contents[0], ref_location)
        link.replace_with(new_link)
    else:
        print "   ** Warning: Could not find Doxygen tag for " + str(link)


def process_d_seealso_tag(bs4, tag, out_path):
    """
    Processes d tag that is of 'seealso' type
    :param bs4: The active beautiful soup instance
    :param tag: the d tag to find a reference for
    :param out_path: the file path
    :return: None
    """
    ref_obj = find_d_tag_ref(tag)
    if type(ref_obj) is SymbolMap.Class or type(ref_obj) is SymbolMap.Typedef:
        # get label attribute value if there is one
        if tag.has_attr("label"):
            label = tag["label"]
        # otherwise use the name of the file as the label
        else:
            label = get_file_name(out_path)

        link_tag = gen_link_tag(bs4, label, out_path)
        ref_obj.add_related_link(link_tag)
    else:
        print "  ** WARNING: Could not find seealso reference for " + str(tag)


def find_d_tag_ref(link):
    # get string to search against
    searchstring = ""
    if len(link.contents):
        searchstring = link.contents[0]

    if link.get('dox') is not None:
        searchstring = link.get('dox')

    ref_obj = None

    args = searchstring.split("|")

    # find function link
    if link.get('kind') == 'function':

        # find parent function first
        existingClass = g_symbolMap.find_class(args[0])
        if existingClass is not None:

            # find the function in the given class
            fnObj = g_symbolMap.find_function(args[1], existingClass, )
            if fnObj is not None:
                ref_obj = fnObj

    elif link.get('kind') == 'enum':
        enumObj = g_symbolMap.find_enum(searchstring)
        if enumObj is not None:
            ref_obj = enumObj

    # find class link
    else:
        existingClass = g_symbolMap.find_class(searchstring)
        if existingClass is not None:
            ref_obj = existingClass

    return ref_obj

def update_link(link, path):
    if link.startswith("http") or link.startswith("javascript:"):
        return link
    depth = len(path.split("html/")[1].split("/")) - 1
    pathPrepend = ""
    for i in range(depth):
        pathPrepend += "../"
    return pathPrepend + link


def construct_template(templates):
    """ Constructs a beautiful soup instance by mashing a bunch of html files together
    """
    masterTemplate = ""
    for templatePath in templates:
        masterTemplate += open(os.path.join(HTML_SOURCE_PATH, TEMPLATE_PATH + templatePath)).read()
    masterTemplate.decode("UTF-8")
    return BeautifulSoup(masterTemplate)


def generate_bs4(filePath):
    ouputFile = open(os.path.join(filePath)).read()
    ouputFile.decode("UTF-8")
    return BeautifulSoup(ouputFile)


def get_symbol_to_file_map(tagDom):
    """ Returns a dictionary from Cinder class name to file path
    """
    print "generating symbol map from tag file"
    symbolMap = SymbolMap()

    # find classes
    classTags = tagXml.findall(r'compound/[@kind="class"]')
    for c in classTags:
        name = c.find('name').text
        filePath = c.find('filename').text
        baseClass = c.find('base').text if c.find('base') is not None else ""
        classObj = SymbolMap.Class(name, baseClass, filePath)
        symbolMap.classes[name] = classObj
        # print name

        # find functions and add to symbol map
        members = c.findall(r"member[@kind='function']")
        for member in members:
            fnName = member.find("name").text
            anchor = member.find("anchor").text
            filePath = member.find("anchorfile").text + "#" + anchor
            # print "FUNCTION " + name+"::"+fnName
            functionObj = SymbolMap.Function(fnName, baseClass, filePath)
            symbolMap.functions[name + "::" + fnName] = functionObj
            classObj.functionList.append(functionObj)

    # find structs
    structTags = tagXml.findall(r'compound/[@kind="struct"]')
    for s in structTags:
        name = s.find('name').text
        filePath = s.find('filename').text
        baseClass = s.find('base').text if s.find('base') is not None else ""
        structObj = SymbolMap.Class(name, baseClass, filePath)
        symbolMap.classes[name] = structObj

        # find functions and add to symbol map
        members = s.findall(r"member[@kind='function']")
        for member in members:
            fnName = member.find("name").text
            anchor = member.find("anchor").text
            filePath = member.find("anchorfile").text + "#" + anchor
            functionObj = SymbolMap.Function(fnName, baseClass, filePath)
            symbolMap.functions[name + "::" + fnName] = functionObj
            structObj.functionList.append(functionObj)

    # find namespaces
    nsTags = tagXml.findall(r'compound/[@kind="namespace"]')

    for ns in nsTags:
        namespaceName = ns.find('name').text
        fileName = ns.find('filename').text

        # skip namespaces with '@' in them
        if namespaceName.find('@') > -1:
            continue

        symbolMap.namespaces[namespaceName] = SymbolMap.Namespace(namespaceName, fileName)

        for member in ns.findall(r"member/[@kind='typedef']"):
            name = member.find("name").text
            type = member.find("type").text
            name = namespaceName + "::" + name
            shared_from_class = None

            if type.find("shared") > 0:
                shareds = re.findall(r"std::shared_ptr< (?:class)* *([\w]*) >", type)
                if len(shareds) > 0:
                    base = namespaceName + "::" + shareds[0]
                    shared_from_class = symbolMap.find_class(base)

            if not shared_from_class:
                # find based on the string in type that's not explicitly a shared_ptr
                # such as <type>SurfaceT&lt; uint8_t &gt;</type>
                shareds = re.findall(r"([A-Za-z]*)", type)
                shared_from_class = symbolMap.find_class(shareds[0])

            filePath = member.find('anchorfile').text + "#" + member.find("anchor").text
            typeDefObj = SymbolMap.Typedef(name, type, filePath)

            if shared_from_class is not None:
                typeDefObj.sharedFrom = shared_from_class

            symbolMap.typedefs[name] = typeDefObj

        # find enums
        for member in ns.findall(r"member/[@kind='enumeration']"):
            name = namespaceName + "::" + member.find("name").text
            # print "ENUM: " + name
            anchor = member.find("anchor").text
            path = member.find("anchorfile").text + "#" + anchor
            enumObj = SymbolMap.Enum(name, path)
            symbolMap.enums[name] = enumObj

    # find files
    fileTags = tagXml.findall(r'compound/[@kind="file"]')
    for f in fileTags:
        name = f.find('name').text
        # filePath = f.find('path').text + f.find('filename').text
        filePath = f.find('path').text + name
        typedefs = []

        # find typedefs for each file
        for t in f.findall(r'member[@kind="typedef"]'):
            tdName = t.find("name").text
            type = t.find("type").text
            filePath = t.find('anchorfile').text + "#" + t.find("anchor").text
            typedef = SymbolMap.Typedef(tdName, type, filePath)
            typedefs.append(typedef)

        # print "FILE PATH: " + name + " | " + filePath
        symbolMap.files[name] = SymbolMap.File(name, filePath, typedefs)

    return symbolMap


def get_file_prefix(file_path):
    return os.path.splitext(os.path.basename(file_path))[0]


def get_file_extension(file_path):
    return os.path.splitext(os.path.basename(file_path))[1]


def get_file_name(file_path):
    return os.path.basename(file_path)


def parse_html(bs4, inPath, outPath):
    tree = None
    try:
        xml_file = codecs.open(inPath, "r", "UTF-8")
        tree = Et.parse(xml_file)

    except:
        exc = sys.exc_info()[0]
        print "\n--- PARSE ERROR ---"
        print "COULD NOT PARSE FILE: " + inPath
        print exc
        # write the file with error
        bs4.append('COULD NOT PARSE FILE: ' + inPath)
        write_html(bs4, outPath)
        print "-------------------\n"

    return tree


def write_html(html, savePath):
    if not os.path.exists(os.path.dirname(savePath)):
        os.makedirs(os.path.dirname(savePath))
    with codecs.open(savePath, "w", "UTF-8") as outFile:
        outFile.write(html.prettify())


def process_file(inPath, outPath=None):
    """ Generate documentation for a single file

        Args:
            inPath: The file to process
            outPath: The file to save the generated html file to
    """
    file = inPath
    filePrefix = get_file_prefix(file)
    isHtmlFile = True if get_file_extension(file).lower() == ".html" else False

    if isHtmlFile:
        filePath = "/".join(inPath.split('htmlsrc/')[1:])
        savePath = outPath if outPath is not None else DOXYGEN_HTML_PATH + filePath
    else:
        savePath = outPath if outPath is not None else DOXYGEN_HTML_PATH + get_file_prefix(inPath) + ".html"

    if isHtmlFile:
        process_html_file(inPath, savePath)

    else:
        # process html directory always, since they may generate content for class or namespace reference pages
        if not PROCESSED_HTML_DIR:
            process_html_dir(HTML_SOURCE_PATH, DOXYGEN_HTML_PATH)
        
        if filePrefix.startswith("class") or filePrefix.startswith("struct"):
            process_class_xml_file(sys.argv[1], os.path.join(DOXYGEN_HTML_PATH, savePath),
                                copy.deepcopy(classTemplateHtml))

        elif filePrefix.startswith("namespace"):
            process_namespace_xml_file(inPath, os.path.join(DOXYGEN_HTML_PATH, savePath),
                                    copy.deepcopy(namespaceTemplateHtml))


def process_dir(inPath, outPath):
    """ Iterates a directory and generates documentation for each xml file
        in the directory as long as it is a class, struct or namespace

        Args:
            inPath: The directory to process
            outPath: The directory to save the generated html file to
    """

    for file in os.listdir(inPath):
        if file.endswith(".xml"):
            # name minus file extension
            filePrefix = get_file_prefix(file)

            # process class and struct files
            if file.startswith("class") or file.startswith("struct"):
                process_class_xml_file(os.path.join(inPath, file), os.path.join(outPath, filePrefix + ".html"),
                                    copy.deepcopy(classTemplateHtml))

            # process namespace files
            elif file.startswith("namespace"):
                process_namespace_xml_file(os.path.join(inPath, file), os.path.join(outPath, filePrefix + ".html"),
                                        copy.deepcopy(namespaceTemplateHtml))

            else:
                print "SKIPPING: " + file
            # TODO: base template and just iterate do an html iteration


def process_html_dir(inPath, outPath):
    print "PROCESS HTML DIR"

    for path, subdirs, files in os.walk(inPath):
        dir = path.split("/")[-1]
        if dir == "_templates" or dir == "assets":
            continue
        for name in files:
            filePrefix = get_file_prefix(name)
            fileExt = get_file_extension(name).lower()
            if fileExt == ".html":

                if path.endswith('/'):
                    srcPath = path[:-1]
                else:
                    srcPath = path

                srcPath = srcPath + "/" + name
                process_file(srcPath)


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
        -   Can pass in a single xml file to process by passing in path to xml file
            and optionally, the resulting html file.
            if no out path is supplied, outputs to DOXYGEN_HTML_PATH
            Ex: python xmlToHtml.py xml/classcinder_1_1_surface_t.xml
        -   Can alternatively pass in a directory to process by providing the xml directory
            Ex: python xmlToHtml.py xml/ html/
    """

    # Load tag file
    print "parsing tag file"
    tagXml = Et.ElementTree(Et.parse("doxygen/cinder.tag").getroot())
    # generate symbol map from tag file
    g_symbolMap = get_symbol_to_file_map(tagXml)

    # construct full template out of multiple html templates
    classTemplateHtml = construct_template(
        ["headerTemplate.html", "mainNavTemplate.html", "cinderClassTemplate.html", "footerTemplate.html"])
    namespaceTemplateHtml = construct_template(
        ["headerTemplate.html", "mainNavTemplate.html", "cinderNamespaceTemplate.html", "footerTemplate.html"])

    # copy files from htmlsrc/ to html/
    copy_files()

    # generate namespace navigation
    g_namespaceNav = generate_namespace_nav(classTemplateHtml)

    inPath = sys.argv[1]
    # process a specific file
    if os.path.isfile(inPath):
        process_file(inPath, sys.argv[2] if len(sys.argv) > 2 else None)

    # process a directory
    elif os.path.isdir(inPath):
        if len(sys.argv) == 3:
            process_html_dir(HTML_SOURCE_PATH, sys.argv[2])
            process_dir(inPath, sys.argv[2])

    else:
        print "Unknown usage"