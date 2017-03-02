#! /usr/bin/python

"""
 Copyright (c) 2017, The Cinder Project, All rights reserved.

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
import os
import argparse
import codecs

# Third party in libs folder
sys.path.append("libs/")

import markdown

# argument parser
parser = argparse.ArgumentParser(description='Md2Html')
parser.add_argument('inpath', nargs='?')
parser.add_argument('outpath', nargs='?')


def validateArgs(args):
    """ Validating that the file at inPath exists and is a .md file.

    Args:
        args: the commandline args, which includes the inPath and outPath
    """

    if len(sys.argv) < 2:
        print("Not enough arguments supplied.")
        return False

    # Validate input path
    inPath = args.inpath
    if args.inpath is not None:

        isFile = os.path.isfile(inPath)         # Check existence of file
        if not isFile:
            print("Nice try! Directory or file '" + inPath + "' doesn't even exist.")
            return False
        else:
            isMarkdown = inPath.endswith('.md')
            if not isMarkdown:
                print("Input file is not a markdown file (with .md extension).")
                return False
    else:
        print("No input file specified.")
        return False

    # Validate output path
    outPath = args.outpath
    if outPath is not None:

        isHtml = outPath.endswith('.html')
        if not isHtml:
            print("Output file is not a markdown file (with .html extension).")
            return False
    else:
        print("No output file specified.")
        return False

    return True;



if __name__ == "__main__":
    """ Main Function for generating an html file from a markdown file

    Args:
        inpath: location of markdown file
        outpath: location of the output html file
    """

    args = parser.parse_args()

    # Validate file's exitence and file type
    if not validateArgs(args):
        print "--- Quitting: Invalid arguments ---"
        quit()

    # Open markdown file
    inPath = args.inpath
    input_file = codecs.open(inPath, mode="r", encoding="utf-8")

    # Read markdown
    text = input_file.read()

    # Convert markdown to html
    # extensions:
    #   - toc: Links up the table of contents and adds the necessary anchors throughout the html file.
    #          http://pythonhosted.org/Markdown/extensions/toc.html
    #   - attr_list: Adds a syntax to define attributes on the various HTML elements in markdown's output.
    #          http://pythonhosted.org/Markdown/extensions/attr_list.html
    #   - fenced_code: Properly interperets github style code blocks
    #          http://pythonhosted.org/Markdown/extensions/fenced_code_blocks.html
    html = markdown.markdown(text, output_format='html5', enable_attributes='True', extensions=['markdown.extensions.toc', 'markdown.extensions.attr_list', 'markdown.extensions.fenced_code'])

    # Write html file
    outPath = args.outpath;
    output_file = codecs.open(outPath, "w")
    output_file.write(html)

    print ("Successfully create file "+outPath)
