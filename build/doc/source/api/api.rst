.. _api:

API Documentation
#################

The API documentation is generated using Doxygen_. to create a XML
rappresentation from the sources. The XML is used by Breathe to
generate the corresponding *rst*, used by Sphinx to generate the final
documentation hosted on Read The Docs.

You can also generate the API documentation locally, by using the Doxyfile
located in **build/doc/source/Doxyfile**

Libpull is logically organized in the following modules:

.. _Doxygen: https://github.com/doxygen/doxygen

.. toctree::
   :maxdepth: 1

   common.rst
   memory.rst
   network.rst
   security.rst
   agents.rst
