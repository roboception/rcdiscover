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

#ifndef RC_LOGO_H
#define RC_LOGO_H

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Image.H>

class Logo : public Fl_Box
{
  public:

    Logo(int x, int y, int w, int h);
    ~Logo();

    void startSpinning();
    void stopSpinning();
    void nextRotation();

  private:

    Fl_RGB_Image *copyScaledRotated(const Fl_RGB_Image *image, int width, int height, double a);

    void draw();

    Fl_RGB_Image *image_orig;
    Fl_RGB_Image *image;

    bool spinning;
    double rotation;
};

#endif
