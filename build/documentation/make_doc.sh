#!/bin/sh -e
# Script to update documentation

ROOTDIR=$(cd $(dirname $0)/../.. && pwd -P)
WIKI_REPO="https://github.com/AntonioLangiu/Pull-IoT.wiki.git"
DOC_PATH="$ROOTDIR/build/documentation"
DOXYFILE="$DOC_PATH/Doxyfile"
WIKI_PATH="$DOC_PATH/wiki"
XML_PATH="$DOC_PATH/xml"
API_PATH="$DOC_PATH/wiki/api"

func_doxygen() {
    echo "Generating doxygen documentation..."
    doxygen $DOXYFILE
    echo "Generating doxygen documentation...done"
}

func_moxygen() {
    echo "Parsing generated documentation to create wiki..."
    ./node_modules/.bin/moxygen --groups -t ./template --output $API_PATH/%s-Module.md $XML_PATH
    echo "Parsing generated documentation to create wiki...done"
}

echo "Updating Wiki documentation..."
if [ ! -d "$WIKI_PATH" ]; then
    git clone $WIKI_REPO $WIKI_PATH
fi
echo "Updating Wiki documentation...done"

func_doxygen
if [ "$1" = "-w" ]; then
    func_moxygen
fi
