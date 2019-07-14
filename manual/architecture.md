Architecture
============

This page describes some of the "high-level" internal parts of fviz that are
not mentioned elsewhere in the documentation, but which are helpful to understand
how things fit together in the codebase.

The intended target audience are fviz developers and users that wish to "look
behind the curtain". If you're just trying to get started with using fviz, it
is not nesseccarily recommended or helpful to read this page.


Code Structure
--------------

   - Graphics Backend
   - Parser
   - Elements


Data Flow
---------

This section gives a brief overview of the internal steps performed to
render an illustration to the screen. 

### Step 1: Parsing & Configuration

   - Read config file
   - Read parameters/variables
   - Run configure functions to build up element tree

### Step 2: Layout

   - Input to this step is the config for each element
   - Run layout functions to find bounding boxes for all elements

### Step 3: Rendering

   - Input to this step is the config and the bounding box for each element
   - Run draw functions for all elements; draw functions perform low-level layout
     such as placing of text and individual shapes, call the rendering backend.


Elements
--------

### Element Tree

### Element Interface



Layout Rules
------------

It's imporant to point out that normal users of the fviz program will not have
to understand the layouting rules described below. If you are new to fviz, please
don't let this section be the only thing you read -- it makes things sound much
more complex than they are!

### Virtual Box Model

Each elements specifies its preferred 'placement'. The placement describes how
the element wishes to be placed with relation to a parent bounding box.

The placement parameter can take six different values with correspond to the six
(partially) overlapping sections of the parent bounding box shown in the illustration
below.

    [figure]

If an element requests the `LAYER` placement it will be drawn onto the inner
box. All elements with `LAYER` placement will simply be drawn on top of each
other in the order in which they were specified.

If an element requests the `BLOCK` (exclusive) placement, some exclusive
space for the element will be reserved in the outer box.

*Usually*, the size of the outer box is constrained; in that case reserving space
for a block element means shrinking the layer region and growing the block region,
i.e. subtracing space from the inner box. However, when either the width or the
height of the parent box are unspecified, space for BLOCK elements may be reserved
by simply growing the outer box and leaving the inner box untouched.


### Element placement

The process of placing elements on their final positions in the document is called
'placement'.

The placement process is, in principle, a simple depth-first traversal
of the element tree. At each step all sibling elements are placed into the
bounding box of their parent element. Note that the placement process assumes
that an absolute document size is already known.

### Finding the document size

Illustrations usually have a fixed desired output size, so in most cases the
document size is simply defined by the user.

FIXME: describe how we come up with auto width/height


### Difference to CSS

Most readers will know CSS so it might be useful to point out the similarities
and differences between fviz and HTML/CSS, since both lay out elements as a
nested tree of boxes and have very similar syntax.

However, that is pretty much where the similarities end. Since CSS is (also) intended
for creating text documents, it is based around a concept of universal "page flow",
i.e. by default block elements in CSS will be stacked vertically from top to bottom,
similar to the way rows of text are placed one after another other in a page.

This concept of page flow, or text layout, is entirely absent from fviz.
fviz is not a document layouting system, so it does make sense to assume "row
by row" as a default element placement strategy. Most of the rest of CSS's layout
features build from this core assumption so they are equally unapplicable to fviz.

Instead of being stacked horizontally within the parent box, like in CSS, boxes
in fviz will, by default, simply be drawn on top of each other with each one
filling up the entirety of the parent box.

This is a much more useful default behaviour for creating illustrations where in
many cases an element more closely resembles the concept of a 'layer' as found in
graphics editing software such as Photoshop or Illustrator than that of a row of
text.

In the cases where you *do* want a behaviour that is more similar to CSS, i.e.
you want multiple elements to be placed in a non-overlapping fashion, fviz gives
you two options:

  - Set an absolute position and size (relative to the parent element) on each
    of the elements and manually ensure the assigned regions don't overlap. This
    is somewhat similar to 'position: absolute' in CSS.

  - Use an explicit layout element such as 'hbox', 'vbox' or 'grid'. This is
    similar in spirit to an HTML 'table' element and also similar to the approach
    taken by many GUI frameworks



Units & Unit Conversion
-----------------------


