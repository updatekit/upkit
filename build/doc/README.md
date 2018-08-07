#Documentation Guidelines

The **libpull** documentation is build using the following tools:

- [Doxygen](http://www.stack.nl/~dimitri/doxygen/)
- [Sphinx](http://www.sphinx-doc.org/en/master/)
- [Breathe](https://github.com/michaeljones/breathe)

## Documentation Overview

The documentation is partially contained in the `build/doc` folder
and partially included as Doxygen comments in the code.

The documentation is written partially using the [reStructuredText](http://docutils.sourceforge.net/rst.html)
markup language and part using MarkDown, to exploit the adavantages
of both. In fact, writing MarkDown is much simpler and easier to maintain
compared to reStructuredText. However, the latter allows to build
complex structures and indexes.

The documentation generation performs logically the following steps:

- Extract XML form the code using Doxygen;
- Parse XML using Breathe and generate reStructuredText files;
- Parse reStructuredText and MarkDown to build the finally documentation using
  sphinx.

## Building the documentation

We build the documentation automatically using [Read The Docs](https://readthedocs.org).
The tools recursively searches for a `conf.py` script in the whole repository
and executes sphinx inside of that folder. In the `conf.py` script you can
find instructions to invoke Doxygen and to configure Breathe.

If you want to build the documentation locally you need to have the following
tools installed:

- Doxygen >= 1.8.13
- Sphinx >= 1.7.6
- Breathe >= 4.9.1

You will find a list of the required python packages in the
`requirements.txt` file.

Once you have all the dependencies installed you can build the documentation
using the Makefile contained in the `build/doc` folder.

You can see all the available targets invoking `make`. If you want to build
the HTML documentation you can use `html` target, such as:

```
make target
```

## Documentation CI

.. image:: https://readthedocs.org/projects/libpull/badge/?version=latest

You can see the state of the current documentation by analyzing the
[Read The Docs builds for libpull](https://readthedocs.org/projects/libpull/).
Moreover, we are building the documentation on Travis to be sure all functions
are documented.



- 
