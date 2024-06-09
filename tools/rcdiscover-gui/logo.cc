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

#include "logo.h"

#include "resources/logo_128.xpm"

#include <FL/Fl_Pixmap.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/fl_draw.H>

#include <algorithm>
#include <cmath>

namespace
{

void timerCb(void *user_data)
{
  Logo *logo=reinterpret_cast<Logo *>(user_data);
  logo->nextRotation();

  Fl::repeat_timeout(0.1, timerCb, user_data);
}

}

Logo::Logo(int x, int y, int w, int h) : Fl_Box(x, y, w, h, 0)
{
  Fl_Pixmap icon_xpm(logo_128_xpm);

  image_orig=new Fl_RGB_Image(&icon_xpm, FL_GRAY);
  image=copyScaledRotated(image_orig, w, h, 0);

  spinning=false;
  rotation=0;
}

Logo::~Logo()
{
  Fl::remove_timeout(timerCb, this);

  delete image_orig;
  delete image;
}

void Logo::startSpinning()
{
  // start spinning if not done already

  if (!spinning)
  {
    spinning=true;
    Fl::add_timeout(0.1, timerCb, this);
  }
}

void Logo::stopSpinning()
{
  if (spinning)
  {
    Fl::remove_timeout(timerCb, this);
    spinning=false;

    rotation=0;
    delete image;
    image=copyScaledRotated(image_orig, w(), h(), rotation);

    redraw();
  }
}

void Logo::nextRotation()
{
  rotation+=0.157;

  delete image;
  image=copyScaledRotated(image_orig, w(), h(), rotation);

  redraw();
}

void Logo::draw()
{
  fl_rectf(x(), y(), w(), h(), color());

  if (image)
  {
    // draw image in the middle

    int xx=x()+(w()-image->w())/2;
    int yy=y()+(h()-image->h())/2;

    image->draw(xx, yy);
  }
}

namespace
{

inline void addPixel(double tp[4], const uchar *pixel, int i, int k, int depth, int ld, double scale)
{
  const uchar *p=pixel+(k*ld+i*depth);

  for (int d=0; d<depth; d++)
  {
    tp[d]+=p[d] * scale;
  }
}

}

Fl_RGB_Image *Logo::copyScaledRotated(const Fl_RGB_Image *image, int width, int height, double a)
{
  // create target pixel array

  int depth=image->d();
  int line=std::max(image->ld(), image->w()*image->d());
  uchar *pixel=new uchar [width*height*depth];

  // scale factors

  double sx=static_cast<double>(image->w())/width;
  double sy=static_cast<double>(image->h())/height;

  double xc=image->w()/2.0;
  double yc=image->h()/2.0;

  double ca=std::cos(a);
  double sa=std::sin(a);

  // get background color for filling out of bound pixel

  uchar bg[]={0, 0, 0, 0};

  {
    unsigned background=Fl::get_color(FL_BACKGROUND_COLOR);

    bg[0]=static_cast<uchar>(background>>24);
    bg[1]=static_cast<uchar>(background>>16);
    bg[2]=static_cast<uchar>(background>>8);
    bg[3]=static_cast<uchar>(background);
  }

  // go through target pixels

  uchar *tp=pixel;
  for (int k=0; k<height; k++)
  {
    for (int i=0; i<width; i++)
    {
      // scale target pixel position to source

      double x=sx*(i+0.5);
      double y=sy*(k+0.5);

      // rotate position around image center

      if (a != 0)
      {
        x-=xc;
        y-=yc;

        double tx =  x*ca + y*sa;
        double ty = -x*sa + y*ca;

        x=tx+xc;
        y=ty+yc;
      }

      // get source pixel value bilinearly interpolated and store as target pixel

      if (x >= 0 && y >= 0 && x+1 < image->w() && y+1 < image->h())
      {
        x-=0.5;
        y-=0.5;

        x=std::max(0.0, std::min(x, image->w()-1.001));
        y=std::max(0.0, std::min(y, image->h()-1.001));

        int si=static_cast<int>(x);
        int sk=static_cast<int>(y);

        x=4*(x-si);
        y=4*(y-sk);

        double p[4]={0, 0, 0, 0};

        addPixel(p, image->array, si, sk, depth, line, (4-x)*(4-y));
        addPixel(p, image->array, si+1, sk, depth, line, x*(4-y));
        addPixel(p, image->array, si, sk+1, depth, line, (4-x)*y);
        addPixel(p, image->array, si+1, sk+1, depth, line, x*y);

        for (int d=0; d<depth; d++)
        {
          tp[d]=static_cast<uchar>(0.0625*p[d]);
        }
      }
      else
      {
        for (int d=0; d<depth; d++)
        {
          tp[d]=bg[d];
        }
      }

      tp+=depth;
    }
  }

  // create and return target image

  Fl_RGB_Image *ret=new Fl_RGB_Image(pixel, width, height, depth, width*depth);
  ret->alloc_array=1; // p is deleted by the newly created image

  return ret;
}
