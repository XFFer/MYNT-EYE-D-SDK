// Copyright 2018 Slightech Co., Ltd. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef MYNTEYE_INTERNAL_CAMERA_P_H_
#define MYNTEYE_INTERNAL_CAMERA_P_H_
#pragma once

#include "mynteye/camera.h"

#include <functional>
#include <string>
#include <map>
#include <memory>
#include <vector>

#include "mynteye/data/types_internal.h"
#include "mynteye/internal/async_callback.h"
#include "mynteye/types.h"

MYNTEYE_BEGIN_NAMESPACE

class Device;
class Channels;
class Motions;
class Streams;

class MYNTEYE_API CameraPrivate {
 public:
  using img_info_callback_t =
        std::function<void(const std::shared_ptr<ImgInfo>& info)>;
  using stream_callback_t = std::function<void(const StreamData& data)>;
  using motion_callback_t = std::function<void(const MotionData& data)>;

  CameraPrivate();
  ~CameraPrivate();

  /** Get all device infos */
  void GetDeviceInfos(std::vector<DeviceInfo>* dev_infos) const;

  /** Get all stream infos */
  void GetStreamInfos(const std::int32_t& dev_index,
      std::vector<StreamInfo>* color_infos,
      std::vector<StreamInfo>* depth_infos) const;

  /** Open camera */
  ErrorCode Open(const OpenParams& params);

  /** Whethor camera is opened or not */
  bool IsOpened() const;
  /** Check camera is opened, otherwise error */
  void CheckOpened() const;

  /** Get all device descriptors */
  std::shared_ptr<device::Descriptors> GetDescriptors() const;
  /** Get one device descriptor */
  std::string GetDescriptor(const Descriptor &desc) const;

  /** Get the intrinsics of camera */
  StreamIntrinsics GetStreamIntrinsics(const StreamMode& stream_mode, bool* ok);
  /** Get the extrinsics from left to right */
  StreamExtrinsics GetStreamExtrinsics(const StreamMode& stream_mode, bool* ok);

  /** Write camera calibration bin file */
  bool WriteCameraCalibrationBinFile(const std::string& filename);

  /** Get the intrinsics of motion */
  MotionIntrinsics GetMotionIntrinsics(bool* ok) const;
  /** Get the extrinsics from left to motion */
  MotionExtrinsics GetMotionExtrinsics(bool* ok) const;

  /** Whethor write device supported or not */
  bool IsWriteDeviceSupported() const;
  /** Write device flash */
  bool WriteDeviceFlash(
      device::Descriptors *desc,
      device::ImuParams *imu_params,
      Version *spec_version = nullptr);

  /** Enable process mode, e.g. imu assembly, temp_drift */
  void EnableProcessMode(const ProcessMode& mode);
  /** Enable process mode, e.g. imu assembly, temp_drift */
  void EnableProcessMode(const std::int32_t& mode);

  /** Whethor image info supported or not */
  bool IsImageInfoSupported() const;
  /**
   * Enable image info.
   *
   * If sync is false, indicates only can get infos from callback.
   * If sync is true, indicates can get infos from callback or access it from StreamData.
   */
  void EnableImageInfo(bool sync);
  /** Disable image info. */
  void DisableImageInfo();
  /** Whethor image info enabled or not */
  bool IsImageInfoEnabled() const;
  /** Whethor image info synced or not */
  bool IsImageInfoSynced() const;

  /** Enable stream data of certain image type */
  void EnableStreamData(const ImageType& type);
  /** Disable stream data of certain image type */
  void DisableStreamData(const ImageType& type);
  /** Whethor stream data of certain image type enabled or not */
  bool IsStreamDataEnabled(const ImageType& type) const;
  /** Has any stream data enabled */
  bool HasStreamDataEnabled() const;

  /** Get latest stream data */
  StreamData GetStreamData(const ImageType& type);
  /** Get cached stream datas */
  std::vector<StreamData> GetStreamDatas(const ImageType& type);

  /** Whethor motion datas supported or not */
  bool IsMotionDatasSupported() const;
  /**
   * Enable motion datas.
   *
   * If max_size <= 0, indicates only can get datas from callback.
   * If max_size > 0, indicates can get datas from callback or using GetMotionDatas().
   *
   * Note: if max_size > 0, the motion datas will be cached until you call GetMotionDatas().
   */
  void EnableMotionDatas(std::size_t max_size);
  /** Disable motion datas. */
  void DisableMotionDatas();
  /** Whethor motion datas enabled or not */
  bool IsMotionDatasEnabled() const;

  /** Get cached motion datas. Besides, you can also get them from callback */
  std::vector<MotionData> GetMotionDatas();

  /** Set image info callback. */
  void SetImgInfoCallback(img_info_callback_t callback, bool async);

  /** Set stream data callback. */
  void SetStreamCallback(const ImageType& type, stream_callback_t callback,
        bool async);

  /** Set motion data callback. */
  void SetMotionCallback(motion_callback_t callback, bool async);

  /** Close the camera */
  void Close();

  /** @deprecated Get camera calibration */
  std::shared_ptr<CameraCalibration> GetCameraCalibration(
      const StreamMode& stream_mode);
  /** @deprecated Get camera calibration file */
  bool GetCameraCalibrationFile(const StreamMode& stream_mode,
                                const std::string& filename);

 protected:
  std::shared_ptr<Channels> channels() const {
    return channels_;
  }

 private:
  void Init();

  void ReadDeviceFlash();

  /** Set the intrinsics of motion */
  void SetMotionIntrinsics(const MotionIntrinsics &in);
  /** Set the extrinsics from left to motion */
  void SetMotionExtrinsics(const MotionExtrinsics &ex);

  /** Start data tracking */
  bool StartDataTracking();
  /** Stop data tracking */
  void StopDataTracking();

  void NotifyDataTrackStateChanged();

  std::shared_ptr<Device> device_;
  std::shared_ptr<Channels> channels_;
  std::shared_ptr<Motions> motions_;
  std::shared_ptr<Streams> streams_;

  std::shared_ptr<device::Descriptors> descriptors_;
  std::shared_ptr<MotionIntrinsics> motion_intrinsics_;
  std::shared_ptr<MotionExtrinsics> motion_extrinsics_;
};

MYNTEYE_END_NAMESPACE

#endif  // MYNTEYE_INTERNAL_CAMERA_P_H_
