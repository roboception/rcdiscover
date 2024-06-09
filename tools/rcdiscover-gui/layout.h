/*
 * Roboception GmbH
 * Munich, Germany
 * www.roboception.com
 *
 * Copyright (c) 2024 Roboception GmbH
 * All rights reserved
 *
 * Author: Heiko Hirschmueller
 */

#ifndef RC_LAYOUT_H
#define RC_LAYOUT_H

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Widget.H>

#include <algorithm>

#define GAP_SIZE 10

/**
  Computes the x coordinate of a widget to the right of the last added widget.
*/

inline int addRightX(int gap=GAP_SIZE)
{
  int n=Fl_Group::current()->children();
  if (n > 0)
  {
    Fl_Widget *c=Fl_Group::current()->child(n-1);
    return c->x()+c->w()+gap;
  }

  return gap;
}

/**
  Computes the y coordinate of a widget to the right of the last added widget.
*/

inline int addRightY(int gap=GAP_SIZE)
{
  int n=Fl_Group::current()->children();
  if (n > 0)
  {
    Fl_Widget *c=Fl_Group::current()->child(n-1);
    return c->y();
  }

  return gap;
}

/**
  Computes the x coordinate of a widget in the next line.
*/

inline int addBelowX(int gap=GAP_SIZE)
{
  return gap;
}

/**
  Computes the y coordinate of a widget to the next line.
*/

inline int addBelowY(int gap=GAP_SIZE)
{
  int ret=gap;
  int n=Fl_Group::current()->children();
  for (int i=0; i<n; i++)
  {
    Fl_Widget *c=Fl_Group::current()->child(i);
    ret=std::max(ret, c->y()+c->h()+gap);
  }

  return ret;
}

/**
  The size of the current group must be set before adding all children,
  especially resizable ones. This function can be used to check if the
  size of the group has been set correctly. A warning is printed if this
  is not the case.
*/

inline void checkGroupSize(const char *group_name)
{
  int n=Fl_Group::current()->children();
  if (n > 0)
  {
    Fl_Group *g=Fl_Group::current();
    Fl_Widget *c=g->child(0);

    int xmin=c->x(), xmax=c->x()+c->w();
    int ymin=c->y(), ymax=c->y()+c->h();

    for (int i=1; i<n; i++)
    {
      c=g->child(i);
      xmin=std::min(xmin, c->x());
      xmax=std::max(xmax, c->x()+c->w());
      ymin=std::min(ymin, c->y());
      ymax=std::max(ymax, c->y()+c->h());
    }

    if ((g->w() != xmax+xmin && g->w() != xmax+xmin+GAP_SIZE) ||
      (g->h() != ymax+ymin && g->h() != ymax+ymin+GAP_SIZE))
    {
      Fl::warning("Size of group '%s' is %dx%d, but should be %dx%d!",
        group_name, g->w(), g->h(), xmax+xmin, ymax+ymin);
    }
  }
}

// Can be used to replace the first two arguments in the constructor of a new
// widget

#define ADD_RIGHT_XY addRightX(), addRightY()
#define ADD_BELOW_XY addBelowX(), addBelowY()

#endif
