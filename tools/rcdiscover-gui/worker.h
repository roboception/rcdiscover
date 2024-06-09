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

#ifndef WORKER_H
#define WORKER_H

#include "check_window.h"

#include "rc_genicam_api/device.h"

#include <atomic>

/**
  Discover suitable devices and add them to the given list.

  NOTE: The operation takes some time and should therefore be run as thread.

  @param ilist   Liste of available interfaces.
  @param list    Output list of devices.
  @param buffer  Buffer for providing information for the user.
  @param running Variable that will be set to false at the end of this function.
*/

void discoverWorker(const std::vector<std::shared_ptr<rcg::Interface> > &ilist, DeviceList *list,
  Fl_Text_Buffer *buffer, std::atomic_bool &running);

/**
  Reset name of left and right camera to default.

  NOTE: The operation takes some time and should therefore be run as thread.

  @param ldev    Left camera.
  @param rdev    Right camera.
  @param running Variable that will be set to false at the end of this function.
*/

void resetNameWorker(rcg::Device *ldev, rcg::Device *rdev, std::atomic_bool &running);

/**
  Store backup file.

  NOTE: The operation takes some time and should therefore be run as thread.

  @param backup_file Name of backup file to be stored.
  @param ldev        Left camera.
  @param rdev        Right camera.
  @param running     Variable that will be set to false at the end of this
                     function.
*/

void storeBackupWorker(const char *backup_file, rcg::Device *ldev, rcg::Device *rdev,
  std::atomic_bool &running);

/**
  Resetting a device from backup file.

  NOTE: The operation takes some time and should therefore be run as thread.

  @param ilist       Liste of available interfaces for discovering the devices.
  @param backup_file Name of backup file to be loaded.
  @param running     Variable that will be set to false at the end of this
                     function.
*/

void resetFromFileWorker(const std::vector<std::shared_ptr<rcg::Interface> > &ilist,
  const char *backup_file, std::atomic_bool &running);

/**
  Stroing calibration data on the left camera.

  NOTE: The operation takes some time and should therefore be run as thread.

  @param ldev    Left camera.
  @param calib   Calibration to be stored on the camera.
  @param running Variable that will be set to false at the end of this
                 function.
*/

void setCalibrationWorker(rcg::Device *ldev, const gutil::Properties *calib,
  std::atomic_bool &running);

#endif
