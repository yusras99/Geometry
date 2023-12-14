#  Data File Format [version 1]
#	2023-02-23

This preliminary version of the documentation exclusively deals with
Point and Segment objects
This data format is unshamedly inspired by the OBJ file format.

1. Comments
Everything following a # sign is a comment.
Blank lines are skipped.

2. Wod Bounds
The file starts with 4 lines specifying the bounds of the world: the min 
and max values of the world coordinates.  Format:

XMIN = -5
XMAX = 7
YMIN = -2
YMAX = 3.5

The space characters around the = sign are (at this point) part of the
format.  Note that specifying extremely different dimensions along the
x and y directions may result the app setting in silly proportions to
its window, or rejecting the input file outright. 

3. List of Points
The file continues with a list of points defined by their coordinates,
one point per line.
The line starts by either the letter "p" or the letter "v" (I am not
fully committed on the "point" vs. "vertex" choice), followed one or 
more blank characters, then by the x and y cordinates of the point, 
separated by one or more blank characters. For example:

v 3.4 12.5
p 6.1 8.2

These points will be listed in the Point class's master Point list, 
with an index starting at 0.

4. List of Segments  
Next in the file is a list of segments defined by the indices of their
endpoints, one segment per line. The line starts with the letter "s", 
followed one or more blank characters, then by indices of the segment's
two endpoints, separated by one or more blank characters. For example:

s 0 7
s 3 1


