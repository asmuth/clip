Architecture
============

This page contains a high level explanation of the internal steps performed to
render an illustration. The intended target audience are PlotFX developers and
users that wish to "look behind the curtain". If you're just trying to get started
with using PlotFX, it is not nesseccarily recommended to read this page first as
all the nitty gritty details might be confusing at fist.


### Step 1: Configuration

### Step 2: Layouting

### Step 3: Rendering

Before the document, consisting of a tree of `Element`s is rendered, a layouting
step is performed.



Elements
--------

### Element Tree

### Element Interface



Layout Rules
------------

Note that the layouting rules described in this section are not exposed to normal
users of the PlotFX program. ... normal users are not required to understand these.

### Virtual Box Model

Each elements specifies it's preferred 'placement'. The placement dscribes how
the element wishes to be placed with relation to a parent bounding box. It can
take the values `LAYER` or `BLOCK`.

To understand what these values mean consider the following "virtual" box. The
virtual box is divided into concentric regions:

    [figure]

If an element requests the `LAYER` placement it will be drawn onto the inner
box. All elements with `LAYER` placement will simply be drawn on top of each
other in the order in which they were specified.

If an element requests the `BLOCK` (exclusive) placement, some exclusive
space for the element will be reserved in the outer box.

*Usually*, the size of the outer box is constrained; in that case reserving space
for a block element means shrinking the layer region and growing the block region,
i.e. subtracing space from the inner box. However, when either the width or the
height of the parent box are unspecified, space for BLOCK elements can be reserved
by simply growing the outer box and leaving the inner box untouched.


### Element placement

The process of placing elements on their final positions in the document is called
'placement'.

The placement process is, in principle, a simple depth-first traversal
of the element tree. At each step all sibling elements are placed into the
bounding box of their parent element. Note that the placement process assumes
that an absolute document size is already known.

Let's first consider the simple case where the element tree has a depth of one,
i.e. it contains the virtual root node and a list of sibling elements which
descend from the root node.

The initial parent bounding box is by definition set to the size of the screen. To
find the bounding boxes for the elements, we first consider all children that
have requested BLOCK placement. For each element, we reserve a sub-region of
current bounding box for the element and then remove the sub-region from the
parent bounding box. This is done in the order in which the block elements were
defined.

Once we have placed all BLOCK elements using this method, we simply copy the
remaining parent bounding box to children that have requested `LAYER` placement.

In other words:
  - Initialize the parent bounding box to the size of the parent element
  - For each element with block placement, subtract the element box from the
    parent bounding box
  - Once all block elements are placed, assign the remaining parent bounding
    box to all layer elements.

This process continues recursively for nested elements (i.e. elements that have
children). Note that while each element has to respect the layout rules described
above when placing its children it is free to use any parent bounding that is a
sub-region of it's own bounding box. It is also allowed to use a different parent
bounding box for each child element. This is required to implement layout container
elements such as `grid` or `legend`.


### Finding the document size

Illustrations usually have a fixed desired output size, so in most cases the
document size is simply defined by the user.

FIXME: describe how we come up with auto width/height


### Alignment with the inner bounding box



### Difference to CSS

Most readers will know CSS so it might be useful to point out the similarities
and differences between PlotFX and HTML/CSS, since both lay out elements as a
nested tree of boxes and have very similar syntax.

However, that is pretty much where the similarities end. Since CSS is (also) intended
for creating text documents, it is based around a concept of universal "page flow",
i.e. by default block elements in CSS will be stacked vertically from top to bottom,
similar to the way rows of text are placed one after another other in a page.

This concept of page flow, or text layout, is entirely absent from PlotFX.
PlotFX is not a document layouting system, so it does make sense to assume "row
by row" as a default element placement strategy. Most of the rest of CSS's layout
features build from this core assumption so they are equally unapplicable to PlotFX.

Instead of being stacked horizontally within the parent box, like in CSS, boxes
in PlotFX will, by default, simply be drawn on top of each other with each one
filling up the entirety of the parent box.

This is a much more useful default behaviour for creating illustrations where in
many cases an element more closely resembles the concept of a 'layer' as found in
graphics editing software such as Photoshop or Illustrator than that of a row of
text.

In the cases where you *do* want a behaviour that is more similar to CSS, i.e.
you want multiple elements to be placed in a non-overlapping fashion, PlotFX gives
you two options:

  - Set an absolute position and size (relative to the parent element) on each
    of the elements and manually ensure the assigned regions don't overlap. This
    is somewhat similar to 'position: absolute' in CSS.

  - Use an explicit layout element such as 'hbox', 'vbox' or 'grid'. This is
    similar in spirit to an HTML 'table' element and also similar to the approach
    taken by many GUI frameworks

