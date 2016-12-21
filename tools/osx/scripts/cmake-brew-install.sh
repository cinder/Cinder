#!/bin/bash
brew ls --versions cmake > /dev/null

if [ $? != 0 ]; then
  # Install cmake
  brew update
  brew install cmake
fi
