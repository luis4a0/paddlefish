# Paddlefish

Paddlefish is a simple, lightweight library to create PDF files. It is
designed to be as simple as possible. The output PDF contains the header,
some global objects, the pages with their local objects and one xref table.
Paddlefish only produces PDF version 1.4.

The architecture is, as said above, as simple as it can be. There are
global objects, encapsulated in the Object class. Currently, a global
object can be a font or a color profile. Non-global objects are objects
local to a given page, such as images and text strings. The basic
difference between global and local objects is when an object number is
assigned to them. For global objects, an object number is assigned when the
object is created. For instance, the function which creates a color space
returns an identifier, which can be used to reference the object later. On
the other hand, a local object only receives an object number when it is
written on the page, that is, only when the final document is produced.

## Quick start

The library can be configured, built and installed with CMake. The examples
provided in the `examples/` folder can be inspiring to understand how the
library works.

## Output file structure

Objects on the output PDF are written in the following order. Object 1 is
the document catalog, 2 contains the document outlines, 3 is the page
dictionary, 4 describes page labels and 5 is the information dictionary.
These five objects are the document header.

From object 6, global objects are written. Then, each page with its objects
is written. Finally, the cross-reference table and the document trailer.

All objects have generation number 0, this means paddlefish generates the
file at once, and it is not capable of editing. Thus, the document contains
only one cross-reference table.

## Zlib

If present, `zlib` implements the flate encoding of some parts of the PDF.
If not, the output will be anyway perfectly valid. To disable the use of
zlib, set `PADDLEFISH_USE_ZLIB` to `OFF` in CMake. This can be useful to see
the uncompressed contents of a produced PDF with any text editor.

## Images

Even paddlefish supporting JPEG encoding, it does not depend on this lib.
The size of the input image should be specified and the JPEG file contents
are copied straight into the output PDF.

Images are better input describing their pixels and soft mask. Using JPEG
files as input, a soft mask cannot be specified.

Also supported are 1-bit image masks, commonly known as stencil masks.

## Color spaces

Device RGB and gray, ICC-based, CalGray, CalRGB and indexed color spaces
are currently supported by paddlefish.

## Fonts

We support the 14 standard Type 1 fonts. Support for TrueType fonts is
planned, ideally through Font Forge.

## Graphics states

We implemented graphics states because it is the only way to specify fill
and stroke opacity. Graphics states are for us global objects.

## Optional content groups (OCG)

Paddlefish implements marked content. This is implemented as described in
the 1.5 version of the standard, although we keep marking PDF as 1.4. This
was done this way because readers only compliant with version 1.4 ignore
the objects of version 1.5.

Objects related to OCG are different to the rest: OCG needs some global
objects as well as local objects. This makes their implementation tricky;
they are declared in the page and, when it is added to the document, OCG
information is transferred, and the object numbers of the OCG's computed.

## Why the is the lib named that way?

I grepped for `p.*d.*f.*` a file with thousand of words and choose
'paddlefish' because I love the river and everything related to it.

## Using, contributing, requesting features

If you find this library useful, I'd be happy to hear about how you use it.
If you think the software can be improved by adding/changing something,
please let me know. I'll appreciate any comment regarding the library.
