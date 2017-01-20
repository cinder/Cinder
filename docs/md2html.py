# -*- coding: utf-8 -*-
import sys
import os
import argparse
import codecs

# Third party in libs folder
sys.path.append("libs/")

import markdown

parser = argparse.ArgumentParser(description='Md2Html')
parser.add_argument('path', nargs='?')
parser.add_argument('outpath', nargs='?')



def log(message, level=0):
    if level == 0 or not level:
        message_prefix = "INFO"
    elif level == 1:
        message_prefix = "WARNING"
    elif level == 2:
        message_prefix = "ERROR"

    print("\r    *** " + message_prefix + ": [ " + message + " ] ***")


if __name__ == "__main__":
    """ Main Function for generating an html from a markdown file

    Args:
        - path
        - outpath
    """

    args = parser.parse_args()

    print( args.path )
    print( args.outpath )




    if args.path:
        inPath = args.path
        if not os.path.isfile(inPath) and not os.path.isdir(inPath):
            log("Nice try! Directory or file '" + inPath + "' doesn't even exist, so we're going to stop right... now!", True)
            quit()

    if os.path.isfile(args.path):
        input_file = codecs.open(inPath, mode="r", encoding="utf-8")

    log("processing files", 0)


    # read markdown
    text = input_file.read()

    # HeaderId
    # markdown.extensions.headerid

    html = markdown.markdown(text, output_format='html5', enable_attributes='True', extensions=['markdown.extensions.toc', 'markdown.extensions.attr_list', 'markdown.extensions.fenced_code'])

    # output
    output_file = codecs.open( args.outpath, "w")
    output_file.write(html)