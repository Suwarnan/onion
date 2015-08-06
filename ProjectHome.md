## New project page ##

Since Google Code is about to close down, onion is now hosted at corpus.tools: http://corpus.tools/wiki/Onion

## About onion ##

Onion (ONe Instance ONly) is a tool for removing duplicate parts from large collections of texts.

## Installation ##
### Prerequisites ###
  * 64-bit CPU architecture
  * [libjudy](http://judy.sourceforge.net/) (>=1.0.5)
### Configuration and installation ###
  1. Download the sources:
```
wget -O onion-1.2.tar.gz 'https://docs.google.com/uc?authuser=0&id=0B4SxKw5O_gLHUXZhOHBzUDNwcXM&export=download'
```
  1. Extract the downloaded file:
```
tar xzvf onion-1.2.tar.gz
```
  1. Configure the package by editing `onion-1.2/Makefile.config`:
    * set `PREFIX` (or `INSTALL_BIN` and `INSTALL_DATA`) according to where you want the executables and data (docs) installed
    * if you have libjudy installed in a non-standard path you need to:
      * set `JUDY_INC` to where `Judy.h` is located
      * set `JUDY_LIB` to where `libJudy.a` is located
  1. Install the package (you may need sudo or a root shell for the last command):
```
cd onion-1.2/
make
make install
```

## Quick start ##
```
onion -s <documents.vert >deduplicated_documents.vert
```
There's also an [usage example](UsageExample.md) on a sample input.

For usage information see:
```
onion -h
man onion
```

## Acknowledgements ##
This software has been developed at the [Natural Language Processing Centre](http://nlp.fi.muni.cz/en/nlpc) of [Masaryk University in Brno](http://nlp.fi.muni.cz/en) with a financial support from [PRESEMT](http://presemt.eu/) and [Lexical Computing Ltd](http://lexicalcomputing.com/). It also relates to author's [PhD research](http://is.muni.cz/th/45523/fi_d/phdthesis.pdf).