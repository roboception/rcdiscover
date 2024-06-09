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

#include "worker.h"

#include "branding/branding.h"

#include "rc_genicam_api/system.h"
#include "rc_genicam_api/interface.h"

#include "check_calib.h"
#include "check_license.h"
#include "util.h"

#include <rclic/verify.h>

#include <Fl/fl_ask.H>

#include <fstream>

namespace
{

bool isSupportedCamera(const std::shared_ptr<rcg::Device> &dev, const char *vendor_model[])
{
  bool found=false;
  for (size_t i=0; !found && vendor_model[i] != 0; i+=2)
  {
    found=(dev->getVendor() == vendor_model[i] &&
           dev->getModel().compare(0, strlen(vendor_model[i+1]), vendor_model[i+1]) == 0);
  }

  return found;
}

}

void discoverWorker(const std::vector<std::shared_ptr<rcg::Interface> > &ilist, DeviceList *list,
  Fl_Text_Buffer *buffer, std::atomic_bool &running)
{
  try
  {
    // get names and serial numbers of all connected cameras that might belong
    // to an rc_viscore regarding vendor and model name

    const char *vendor_model[]=
    {
      "MATRIX VISION GmbH", "mvBlueCOUGAR",
      "MATRIX VISION GmbH", "mvBlueFOX3",
      "Basler", "a2A2448-23gcBAS",
      "Basler", "a2A2448-23gmBAS",
      0, 0
    };

    std::vector<std::shared_ptr<rcg::Device> > devices;
    bool multiple_interfaces=false;

    for (size_t i=0; i<ilist.size() && !multiple_interfaces; i++)
    {
      try
      {
        ilist[i]->open();
        std::vector<std::shared_ptr<rcg::Device> > list=ilist[i]->getDevices();

        for (size_t k=0; k<list.size() && !multiple_interfaces; k++)
        {
          if (isSupportedCamera(list[k], vendor_model))
          {
            // check that serial number does not yet exist (if it does, then the
            // camera is reachable via two interface, e.g. LAN and WLAN)

            std::string sn=list[k]->getSerialNumber();
            for (size_t j=0; j<devices.size(); j++)
            {
              if (devices[j]->getSerialNumber() == sn)
              {
                multiple_interfaces=true;
                break;
              }
            }

            devices.push_back(list[k]);
          }
        }
      }
      catch (const std::exception &ex)
      {
        std::ostringstream out;

        Fl::lock();
        fl_alert("%s", ex.what());
        Fl::unlock();
      }
    }

    // the user must ensure that the cameras are not reachable via multiple
    // interfaces

    if (multiple_interfaces)
    {
      std::ostringstream out;

      out << "Found cameras with identical serial numbers. This indicates that ";
      out << "cameras can be reached through different network interfaces, e.g. ";
      out << "LAN and WLAN. Please ensure that cameras are only visible through ";
      out << "one interface, e.g. by turning off WLAN, and try again.";

      Fl::lock();
      buffer->text(out.str().c_str());
      Fl::unlock();
    }
    else
    {
      // find left/right pairs according to names

      bool noaccess=false;
      bool misconfigured=false;
      std::vector<std::shared_ptr<rcg::Device> > all=devices;

      for (size_t i=0; i<all.size(); i++)
      {
        std::string lname;
        if (all[i])
        {
          lname=all[i]->getUserDefinedName();
          if (lname.size() == 0) lname=all[i]->getDisplayName();
        }

        if (lname.size() >= 1 && lname.back() == 'L')
        {
          for (size_t k=0; k<all.size(); k++)
          {
            std::string rname;
            if (all[k])
            {
              rname=all[k]->getUserDefinedName();
              if (rname.size() == 0) rname=all[k]->getDisplayName();
            }

            if (lname.size() == rname.size() && rname.back() == 'R' &&
                lname.compare(0, lname.size()-1, rname, 0, lname.size()-1) == 0)
            {
              // check that model is exactly the same

              if (all[i]->getModel() == all[k]->getModel())
              {
                // check that each names are unique

                bool found=false;
                for (size_t j=0; !found && j<all.size(); j++)
                {
                  if (all[j] && j != i && j != k)
                  {
                    std::string name=all[j]->getUserDefinedName();
                    if (name.size() == 0) name=all[j]->getDisplayName();

                    if (name == lname || name == rname)
                    {
                      found=true;
                    }
                  }
                }

                if (!found)
                {
                  // add rc_viscore to table

                  std::shared_ptr<gutil::Properties> calib;

                  std::string snleft=all[i]->getSerialNumber();
                  std::string snright=all[k]->getSerialNumber();
                  std::string calib_status=getCalibrationStatus(calib, all[i]);
                  std::string license_status=getLicenseStatus(all[i], all[k]);

                  Fl::lock();
                  list->add(lname.substr(0, lname.size()-1).c_str(),
                    snleft.c_str(), snright.c_str(), calib_status.c_str(),
                    license_status.c_str(), all[i], all[k], calib);
                  Fl::unlock();

                  if ((calib_status != "ok" && calib_status != "no access") ||
                    (license_status != "ok" && license_status != "no access"))
                  {
                    if ((calib_status == "no access" || calib_status == "no nodemap") &&
                      (license_status == "no access" || license_status == "no nodemap"))
                    {
                      noaccess=true;
                    }
                    else
                    {
                      misconfigured=true;
                    }
                  }

                  // remove devices from list

                  all[i].reset();
                  all[k].reset();
                }
                else
                {
                  std::ostringstream out;
                  out << "Error: Cameras " << lname << " (" << all[i]->getSerialNumber() <<
                    ") and " << rname << " (" << all[k]->getSerialNumber() <<
                    " would match by name, but names are not unique!\n\n";

                  Fl::lock();
                  buffer->text(out.str().c_str());
                  Fl::unlock();
                }
              }
              else
              {
                std::ostringstream out;
                out << "Error: Cameras " << lname << " (" << all[i]->getSerialNumber() <<
                  ") and " << rname << " (" << all[k]->getSerialNumber() <<
                  " would match by name, but have different model!\n\n";

                Fl::lock();
                buffer->text(out.str().c_str());
                Fl::unlock();
              }
            }
          }
        }
      }

      // find cameras with license file and search for corresponding left camera

      bool wrong_name=false;

      for (size_t k=0; k<all.size(); k++)
      {
        if (all[k])
        {
          // check if camera contains license with valid signature and get SN of
          // left camera

          std::string lsn=getLeftSNFromLicense(all[k]);

          if (lsn.size() > 0)
          {
            // find left camera

            for (size_t i=0; i<all.size(); i++)
            {
              if (all[i] && i != k && all[i]->getSerialNumber() == lsn)
              {
                // add rc_viscore to table

                std::shared_ptr<gutil::Properties> calib;

                std::string snleft=all[i]->getSerialNumber();
                std::string snright=all[k]->getSerialNumber();
                std::string calibration_status=getCalibrationStatus(calib, all[i]);

                Fl::lock();
                list->add("?", snleft.c_str(), snright.c_str(),
                  calibration_status.c_str(), "ok", all[i], all[k], calib);
                Fl::unlock();

                if (calibration_status != "ok")
                {
                  if (calibration_status == "no access" || calibration_status == "no nodemap")
                  {
                    noaccess=true;
                  }
                  else
                  {
                    misconfigured=true;
                  }
                }
                else
                {
                  wrong_name=true;
                }

                // remove devices from list

                all[i].reset();
                all[k].reset();
              }
            }
          }
        }
      }

      // remove shared pointers of removed devices

      {
        size_t i=0;
        while (i < all.size())
        {
          if (all[i])
          {
            i++;
          }
          else
          {
            all.erase(all.begin()+i);
          }
        }
      }

      // print suggestions

      if (misconfigured)
      {
        std::ostringstream out;

        out << "Misconfigured " RC_VISCORE_NAME ". Request factory backup file from Roboception\n";
        out << "and choose 'Reset from file ...' in menu.\n\n";

        Fl::lock();
        buffer->text(out.str().c_str());
        Fl::unlock();
      }

      if (wrong_name)
      {
        Fl::lock();
        buffer->text("Select " RC_VISCORE_NAME " with name '?' and press 'Reset Name'\n\n");
        Fl::unlock();
      }

      if (noaccess)
      {
        std::ostringstream out;

        out << "Cannot access cameras. Please ensure that they are not used by any\n";
        out << "other application.\n\n";

        Fl::lock();
        buffer->text(out.str().c_str());
        Fl::unlock();
      }

      if (all.size() > 0)
      {
        std::ostringstream out;

        if (all.size() == 1)
        {
          out << "The following camera may belong to an " RC_VISCORE_NAME ", but only one\n";
          out << "is visible. Check network connection and configuration (see\n";
          out << "troubleshooting section of the manual).\n\n";
        }
        else
        {
          out << "The following cameras may belong to an " RC_VISCORE_NAME ". Check on the label of\n";
          out << "the " RC_VISCORE_NAME " if both serial numbers are listed below. If not, check\n";
          out << "network connection and configuration (see troubleshooting section of the\n";
          out << "manual). If the serial numbers of both cameras are listed, request a\n";
          out << "backup file from Roboception and choose 'Reset from file ...' in menu.\n";
          out << "\n\n";
        }

        for (size_t i=0; i<all.size(); i++)
        {
          out << all[i]->getSerialNumber() << "\n";
        }

        Fl::lock();
        buffer->text(out.str().c_str());
        Fl::unlock();
      }

      if (!misconfigured && !noaccess && !wrong_name && all.size() == 0)
      {
        if (devices.size() > 0)
        {
          Fl::lock();
          buffer->text("All discovered " RC_VISCORE_NAME " are ok.\n");
          Fl::unlock();
        }
        else
        {
          std::ostringstream out;
          out << "No " RC_VISCORE_NAME " connected.\n\n";
          out << "Check connection and network settings of cameras with other tools.\n";

          Fl::lock();
          buffer->text(out.str().c_str());
          Fl::unlock();
        }
      }
    }

    // close all interfaces

    for (size_t i=0; i<ilist.size(); i++)
    {
      ilist[i]->close();
    }
  }
  catch (...)
  {
    Fl::lock();
    fl_alert("Unknown exception in discover function!");
    Fl::unlock();
  }

  // send thread done event

  running=false;
}

void resetNameWorker(rcg::Device *ldev, rcg::Device *rdev, std::atomic_bool &running)
{
  try
  {
    try
    {
      // connect to device and get nodemap

      ldev->open(rcg::Device::CONTROL);
      rdev->open(rcg::Device::CONTROL);

      resetNames(ldev->getRemoteNodeMap(), rdev->getRemoteNodeMap());
    }
    catch (const std::exception &ex)
    {
      Fl::lock();
      fl_alert("%s", ex.what());
      Fl::unlock();
    }

    // close devices

    if (ldev) ldev->close();
    if (rdev) rdev->close();
  }
  catch (...)
  {
    Fl::lock();
    fl_alert("Unknown exception in reset name function!");
    Fl::unlock();
  }

  // send thread done event

  running=false;
}

void storeBackupWorker(const char *backup_file, rcg::Device *ldev, rcg::Device *rdev,
  std::atomic_bool &running)
{
  try
  {
    try
    {
      // connect to device and get nodemap

      ldev->open(rcg::Device::CONTROL);
      rdev->open(rcg::Device::CONTROL);

      std::shared_ptr<GenApi::CNodeMapRef> lmap=ldev->getRemoteNodeMap();
      std::shared_ptr<GenApi::CNodeMapRef> rmap=rdev->getRemoteNodeMap();

      // get calibration and license file

      std::string calib=rcg::loadFile(lmap, getUserFileName(lmap).c_str(), true);
      std::string license=rcg::loadFile(rmap, getUserFileName(rmap).c_str(), true);

      // store backup file

      std::ofstream out;
      out.exceptions(std::ifstream::failbit | std::ifstream::badbit);

      out.open(backup_file);

      out << calib;

      if (calib.size() > 0 && calib[calib.size()-1] != '\n')
      {
        out << "\n";
      }

      out << "====================\n";
      out << license;

      out.close();

      // show message to check calibration

      Fl::lock();
      fl_message("Backup of " RC_VISCORE_NAME " successfully stored!");
      Fl::unlock();
    }
    catch (const std::exception &ex)
    {
      Fl::lock();
      fl_alert("Error '%s'", ex.what());
      Fl::unlock();
    }

    // close devices

    if (ldev) ldev->close();
    if (rdev) rdev->close();
  }
  catch (...)
  {
    Fl::lock();
    fl_alert("Unknown exception in update worker!");
    Fl::unlock();
  }

  running=false;
}

void resetFromFileWorker(const std::vector<std::shared_ptr<rcg::Interface> > &ilist,
  const char *backup_file, std::atomic_bool &running)
{
  try
  {
    // load file and split into calibration and license

    std::string all;

    try
    {
      std::ifstream in;
      in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

      in.open(backup_file);
      std::ostringstream out;
      out << in.rdbuf();
      all=out.str();
    }
    catch (const std::exception &ex)
    {
      Fl::lock();
      fl_alert("Cannot open file '%s'", backup_file);
      Fl::unlock();

      // send thread done event

      running=false;
      return;
    }

    int i1=0, i2=0;

    for (size_t i=0; i<all.size(); i++)
    {
      if (i1 >= 0)
      {
        if (all[i] == '\n')
        {
          i2=i;
          break;
        }
        else if (all[i] != '=')
        {
          i1=-1;
        }
      }
      else if (all[i] == '\n' && i < all.size() && all[i+1] == '=')
      {
        i1=i;
      }
    }

    if (i2 == 0)
    {
      Fl::lock();
      fl_alert("Invalid " RC_VISCORE_NAME " backup file '%s'", backup_file);
      Fl::unlock();

      // send thread done event

      running=false;
      return;
    }

    std::string calib=all.substr(0, i1);
    std::string license=all.substr(i2+1);

    // validate license and get SN of left and right camera

    std::string snleft;
    std::string snright;

    try
    {
      rcl::Verify verify(license);
      std::string licdata=verify.verify();

      std::istringstream in(licdata);
      std::string line;

      while (!in.eof())
      {
        std::getline(in, line);

        if (line.compare(0, 6, "extra=") == 0 && line.size() > 16)
        {
          if (line.compare(line.size()-10, 10, ",type=left") == 0)
          {
            snleft=line.substr(6, line.size()-6-10);
          }
          else if (line.compare(line.size()-11, 11, ",type=right") == 0)
          {
            snright=line.substr(6, line.size()-6-11);
          }
        }
      }
    }
    catch (const std::exception &ex)
    {
      Fl::lock();
      fl_alert("Invalid " RC_VISCORE_NAME " backup file '%s': %s", backup_file, ex.what());
      Fl::unlock();

      // send thread done event

      running=false;
      return;
    }

    // find devices

    std::shared_ptr<rcg::Device> ldev;
    std::shared_ptr<rcg::Device> rdev;

    try
    {
      for (size_t i=0; i<ilist.size(); i++)
      {
        if (!ldev) ldev=ilist[i]->getDevice(snleft.c_str());
        if (!rdev) rdev=ilist[i]->getDevice(snright.c_str());
      }

      if (ldev && rdev)
      {
        // connect to device and get nodemap

        ldev->open(rcg::Device::CONTROL);
        rdev->open(rcg::Device::CONTROL);

        std::shared_ptr<GenApi::CNodeMapRef> lmap=ldev->getRemoteNodeMap();
        std::shared_ptr<GenApi::CNodeMapRef> rmap=rdev->getRemoteNodeMap();

        // set names of both cameras

        resetNames(lmap, rmap);

        // store calibration file

        rcg::saveFile(lmap, getUserFileName(lmap).c_str(), calib, true);

        // store license file

        rcg::saveFile(rmap, getUserFileName(rmap).c_str(), license, true);

        // show message to check calibration

        Fl::lock();
        fl_message(RC_VISCORE_NAME " successfully reset to factory defaults. You must check calibration before working with this camera!");
        Fl::unlock();
      }
      else
      {
        Fl::lock();
        fl_alert("Cannot find devices '%s' and '%s'", snleft.c_str(), snright.c_str());
        Fl::unlock();
      }
    }
    catch (const std::exception &ex)
    {
      Fl::lock();
      fl_alert("Error '%s'", ex.what());
      Fl::unlock();
    }

    // close devices

    if (ldev) ldev->close();
    if (rdev) rdev->close();
  }
  catch (...)
  {
    Fl::lock();
    fl_alert("Unknown exception in update worker!");
    Fl::unlock();
  }

  // send thread done event

  running=false;
}

void setCalibrationWorker(rcg::Device *ldev, const gutil::Properties *calib,
  std::atomic_bool &running)
{
  try
  {
    try
    {
      // connect to device and get nodemap

      ldev->open(rcg::Device::CONTROL);

      std::shared_ptr<GenApi::CNodeMapRef> lmap=ldev->getRemoteNodeMap();

      // convert calibration properties to string

      std::ostringstream calibdata;
      calib->save(calibdata);

      // get calibration and license file

      rcg::saveFile(lmap, getUserFileName(lmap).c_str(), calibdata.str(), true);

      // show message to check calibration

      Fl::lock();
      fl_message("Data stored successfully.");
      Fl::unlock();
    }
    catch (const std::exception &ex)
    {
      Fl::lock();
      fl_alert("Error '%s'", ex.what());
      Fl::unlock();
    }

    // close devices

    if (ldev) ldev->close();
  }
  catch (...)
  {
    Fl::lock();
    fl_alert("Unknown exception in set calibration worker!");
    Fl::unlock();
  }

  running=false;
}
