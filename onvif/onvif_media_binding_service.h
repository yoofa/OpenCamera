/*
 * onvif_media_binding_service.h
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef ONVIF_MEDIA_BINDING_SERVICE_H
#define ONVIF_MEDIA_BINDING_SERVICE_H

#include "third_party/onvif_srvd/src/generated/soapMediaBindingService.h"

namespace ave {

class OnvifMediaBindingService : public MediaBindingService {
 public:
  OnvifMediaBindingService(struct soap* soap);
  virtual ~OnvifMediaBindingService();

  //
  // Service operations are listed below: you should define these
  // Note: compile with -DWITH_PURE_VIRTUAL to declare pure virtual methods
  //
  /// Web service operation 'GetServiceCapabilities' implementation, should
  /// return SOAP_OK or error code
  virtual int GetServiceCapabilities(
      _trt__GetServiceCapabilities* trt__GetServiceCapabilities,
      _trt__GetServiceCapabilitiesResponse& trt__GetServiceCapabilitiesResponse)
      override;
  //
  /// Web service operation 'GetVideoSources' implementation, should return
  /// SOAP_OK or error code
  virtual int GetVideoSources(
      _trt__GetVideoSources* trt__GetVideoSources,
      _trt__GetVideoSourcesResponse& trt__GetVideoSourcesResponse) override;
  //
  /// Web service operation 'GetAudioSources' implementation, should return
  /// SOAP_OK or error code
  virtual int GetAudioSources(
      _trt__GetAudioSources* trt__GetAudioSources,
      _trt__GetAudioSourcesResponse& trt__GetAudioSourcesResponse) override;
  //
  /// Web service operation 'GetAudioOutputs' implementation, should return
  /// SOAP_OK or error code
  virtual int GetAudioOutputs(
      _trt__GetAudioOutputs* trt__GetAudioOutputs,
      _trt__GetAudioOutputsResponse& trt__GetAudioOutputsResponse) override;
  //
  /// Web service operation 'CreateProfile' implementation, should return
  /// SOAP_OK or error code
  virtual int CreateProfile(
      _trt__CreateProfile* trt__CreateProfile,
      _trt__CreateProfileResponse& trt__CreateProfileResponse) override;
  //
  /// Web service operation 'GetProfile' implementation, should return SOAP_OK
  /// or error code
  virtual int GetProfile(
      _trt__GetProfile* trt__GetProfile,
      _trt__GetProfileResponse& trt__GetProfileResponse) override;
  //
  /// Web service operation 'GetProfiles' implementation, should return SOAP_OK
  /// or error code
  virtual int GetProfiles(
      _trt__GetProfiles* trt__GetProfiles,
      _trt__GetProfilesResponse& trt__GetProfilesResponse) override;
  //
  /// Web service operation 'AddVideoEncoderConfiguration' implementation,
  /// should return SOAP_OK or error code
  virtual int AddVideoEncoderConfiguration(
      _trt__AddVideoEncoderConfiguration* trt__AddVideoEncoderConfiguration,
      _trt__AddVideoEncoderConfigurationResponse&
          trt__AddVideoEncoderConfigurationResponse) override;
  //
  /// Web service operation 'AddVideoSourceConfiguration' implementation, should
  /// return SOAP_OK or error code
  virtual int AddVideoSourceConfiguration(
      _trt__AddVideoSourceConfiguration* trt__AddVideoSourceConfiguration,
      _trt__AddVideoSourceConfigurationResponse&
          trt__AddVideoSourceConfigurationResponse) override;
  //
  /// Web service operation 'AddAudioEncoderConfiguration' implementation,
  /// should return SOAP_OK or error code
  virtual int AddAudioEncoderConfiguration(
      _trt__AddAudioEncoderConfiguration* trt__AddAudioEncoderConfiguration,
      _trt__AddAudioEncoderConfigurationResponse&
          trt__AddAudioEncoderConfigurationResponse) override;
  //
  /// Web service operation 'AddAudioSourceConfiguration' implementation, should
  /// return SOAP_OK or error code
  virtual int AddAudioSourceConfiguration(
      _trt__AddAudioSourceConfiguration* trt__AddAudioSourceConfiguration,
      _trt__AddAudioSourceConfigurationResponse&
          trt__AddAudioSourceConfigurationResponse) override;
  //
  /// Web service operation 'AddPTZConfiguration' implementation, should return
  /// SOAP_OK or error code
  virtual int AddPTZConfiguration(
      _trt__AddPTZConfiguration* trt__AddPTZConfiguration,
      _trt__AddPTZConfigurationResponse& trt__AddPTZConfigurationResponse)
      override;
  //
  /// Web service operation 'AddVideoAnalyticsConfiguration' implementation,
  /// should return SOAP_OK or error code
  virtual int AddVideoAnalyticsConfiguration(
      _trt__AddVideoAnalyticsConfiguration* trt__AddVideoAnalyticsConfiguration,
      _trt__AddVideoAnalyticsConfigurationResponse&
          trt__AddVideoAnalyticsConfigurationResponse) override;
  //
  /// Web service operation 'AddMetadataConfiguration' implementation, should
  /// return SOAP_OK or error code
  virtual int AddMetadataConfiguration(
      _trt__AddMetadataConfiguration* trt__AddMetadataConfiguration,
      _trt__AddMetadataConfigurationResponse&
          trt__AddMetadataConfigurationResponse) override;
  //
  /// Web service operation 'AddAudioOutputConfiguration' implementation, should
  /// return SOAP_OK or error code
  virtual int AddAudioOutputConfiguration(
      _trt__AddAudioOutputConfiguration* trt__AddAudioOutputConfiguration,
      _trt__AddAudioOutputConfigurationResponse&
          trt__AddAudioOutputConfigurationResponse) override;
  //
  /// Web service operation 'AddAudioDecoderConfiguration' implementation,
  /// should return SOAP_OK or error code
  virtual int AddAudioDecoderConfiguration(
      _trt__AddAudioDecoderConfiguration* trt__AddAudioDecoderConfiguration,
      _trt__AddAudioDecoderConfigurationResponse&
          trt__AddAudioDecoderConfigurationResponse) override;
  //
  /// Web service operation 'RemoveVideoEncoderConfiguration' implementation,
  /// should return SOAP_OK or error code
  virtual int RemoveVideoEncoderConfiguration(
      _trt__RemoveVideoEncoderConfiguration*
          trt__RemoveVideoEncoderConfiguration,
      _trt__RemoveVideoEncoderConfigurationResponse&
          trt__RemoveVideoEncoderConfigurationResponse) override;
  //
  /// Web service operation 'RemoveVideoSourceConfiguration' implementation,
  /// should return SOAP_OK or error code
  virtual int RemoveVideoSourceConfiguration(
      _trt__RemoveVideoSourceConfiguration* trt__RemoveVideoSourceConfiguration,
      _trt__RemoveVideoSourceConfigurationResponse&
          trt__RemoveVideoSourceConfigurationResponse) override;
  //
  /// Web service operation 'RemoveAudioEncoderConfiguration' implementation,
  /// should return SOAP_OK or error code
  virtual int RemoveAudioEncoderConfiguration(
      _trt__RemoveAudioEncoderConfiguration*
          trt__RemoveAudioEncoderConfiguration,
      _trt__RemoveAudioEncoderConfigurationResponse&
          trt__RemoveAudioEncoderConfigurationResponse) override;
  //
  /// Web service operation 'RemoveAudioSourceConfiguration' implementation,
  /// should return SOAP_OK or error code
  virtual int RemoveAudioSourceConfiguration(
      _trt__RemoveAudioSourceConfiguration* trt__RemoveAudioSourceConfiguration,
      _trt__RemoveAudioSourceConfigurationResponse&
          trt__RemoveAudioSourceConfigurationResponse) override;
  //
  /// Web service operation 'RemovePTZConfiguration' implementation, should
  /// return SOAP_OK or error code
  virtual int RemovePTZConfiguration(
      _trt__RemovePTZConfiguration* trt__RemovePTZConfiguration,
      _trt__RemovePTZConfigurationResponse& trt__RemovePTZConfigurationResponse)
      override;
  //
  /// Web service operation 'RemoveVideoAnalyticsConfiguration' implementation,
  /// should return SOAP_OK or error code
  virtual int RemoveVideoAnalyticsConfiguration(
      _trt__RemoveVideoAnalyticsConfiguration*
          trt__RemoveVideoAnalyticsConfiguration,
      _trt__RemoveVideoAnalyticsConfigurationResponse&
          trt__RemoveVideoAnalyticsConfigurationResponse) override;
  //
  /// Web service operation 'RemoveMetadataConfiguration' implementation, should
  /// return SOAP_OK or error code
  virtual int RemoveMetadataConfiguration(
      _trt__RemoveMetadataConfiguration* trt__RemoveMetadataConfiguration,
      _trt__RemoveMetadataConfigurationResponse&
          trt__RemoveMetadataConfigurationResponse) override;
  //
  /// Web service operation 'RemoveAudioOutputConfiguration' implementation,
  /// should return SOAP_OK or error code
  virtual int RemoveAudioOutputConfiguration(
      _trt__RemoveAudioOutputConfiguration* trt__RemoveAudioOutputConfiguration,
      _trt__RemoveAudioOutputConfigurationResponse&
          trt__RemoveAudioOutputConfigurationResponse) override;
  //
  /// Web service operation 'RemoveAudioDecoderConfiguration' implementation,
  /// should return SOAP_OK or error code
  virtual int RemoveAudioDecoderConfiguration(
      _trt__RemoveAudioDecoderConfiguration*
          trt__RemoveAudioDecoderConfiguration,
      _trt__RemoveAudioDecoderConfigurationResponse&
          trt__RemoveAudioDecoderConfigurationResponse) override;
  //
  /// Web service operation 'DeleteProfile' implementation, should return
  /// SOAP_OK or error code
  virtual int DeleteProfile(
      _trt__DeleteProfile* trt__DeleteProfile,
      _trt__DeleteProfileResponse& trt__DeleteProfileResponse) override;
  //
  /// Web service operation 'GetVideoSourceConfigurations' implementation,
  /// should return SOAP_OK or error code
  virtual int GetVideoSourceConfigurations(
      _trt__GetVideoSourceConfigurations* trt__GetVideoSourceConfigurations,
      _trt__GetVideoSourceConfigurationsResponse&
          trt__GetVideoSourceConfigurationsResponse) override;
  //
  /// Web service operation 'GetVideoEncoderConfigurations' implementation,
  /// should return SOAP_OK or error code
  virtual int GetVideoEncoderConfigurations(
      _trt__GetVideoEncoderConfigurations* trt__GetVideoEncoderConfigurations,
      _trt__GetVideoEncoderConfigurationsResponse&
          trt__GetVideoEncoderConfigurationsResponse) override;
  //
  /// Web service operation 'GetAudioSourceConfigurations' implementation,
  /// should return SOAP_OK or error code
  virtual int GetAudioSourceConfigurations(
      _trt__GetAudioSourceConfigurations* trt__GetAudioSourceConfigurations,
      _trt__GetAudioSourceConfigurationsResponse&
          trt__GetAudioSourceConfigurationsResponse) override;
  //
  /// Web service operation 'GetAudioEncoderConfigurations' implementation,
  /// should return SOAP_OK or error code
  virtual int GetAudioEncoderConfigurations(
      _trt__GetAudioEncoderConfigurations* trt__GetAudioEncoderConfigurations,
      _trt__GetAudioEncoderConfigurationsResponse&
          trt__GetAudioEncoderConfigurationsResponse) override;
  //
  /// Web service operation 'GetVideoAnalyticsConfigurations' implementation,
  /// should return SOAP_OK or error code
  virtual int GetVideoAnalyticsConfigurations(
      _trt__GetVideoAnalyticsConfigurations*
          trt__GetVideoAnalyticsConfigurations,
      _trt__GetVideoAnalyticsConfigurationsResponse&
          trt__GetVideoAnalyticsConfigurationsResponse) override;
  //
  /// Web service operation 'GetMetadataConfigurations' implementation, should
  /// return SOAP_OK or error code
  virtual int GetMetadataConfigurations(
      _trt__GetMetadataConfigurations* trt__GetMetadataConfigurations,
      _trt__GetMetadataConfigurationsResponse&
          trt__GetMetadataConfigurationsResponse) override;
  //
  /// Web service operation 'GetAudioOutputConfigurations' implementation,
  /// should return SOAP_OK or error code
  virtual int GetAudioOutputConfigurations(
      _trt__GetAudioOutputConfigurations* trt__GetAudioOutputConfigurations,
      _trt__GetAudioOutputConfigurationsResponse&
          trt__GetAudioOutputConfigurationsResponse) override;
  //
  /// Web service operation 'GetAudioDecoderConfigurations' implementation,
  /// should return SOAP_OK or error code
  virtual int GetAudioDecoderConfigurations(
      _trt__GetAudioDecoderConfigurations* trt__GetAudioDecoderConfigurations,
      _trt__GetAudioDecoderConfigurationsResponse&
          trt__GetAudioDecoderConfigurationsResponse) override;
  //
  /// Web service operation 'GetVideoSourceConfiguration' implementation, should
  /// return SOAP_OK or error code
  virtual int GetVideoSourceConfiguration(
      _trt__GetVideoSourceConfiguration* trt__GetVideoSourceConfiguration,
      _trt__GetVideoSourceConfigurationResponse&
          trt__GetVideoSourceConfigurationResponse) override;
  //
  /// Web service operation 'GetVideoEncoderConfiguration' implementation,
  /// should return SOAP_OK or error code
  virtual int GetVideoEncoderConfiguration(
      _trt__GetVideoEncoderConfiguration* trt__GetVideoEncoderConfiguration,
      _trt__GetVideoEncoderConfigurationResponse&
          trt__GetVideoEncoderConfigurationResponse) override;
  //
  /// Web service operation 'GetAudioSourceConfiguration' implementation, should
  /// return SOAP_OK or error code
  virtual int GetAudioSourceConfiguration(
      _trt__GetAudioSourceConfiguration* trt__GetAudioSourceConfiguration,
      _trt__GetAudioSourceConfigurationResponse&
          trt__GetAudioSourceConfigurationResponse) override;
  //
  /// Web service operation 'GetAudioEncoderConfiguration' implementation,
  /// should return SOAP_OK or error code
  virtual int GetAudioEncoderConfiguration(
      _trt__GetAudioEncoderConfiguration* trt__GetAudioEncoderConfiguration,
      _trt__GetAudioEncoderConfigurationResponse&
          trt__GetAudioEncoderConfigurationResponse) override;
  //
  /// Web service operation 'GetVideoAnalyticsConfiguration' implementation,
  /// should return SOAP_OK or error code
  virtual int GetVideoAnalyticsConfiguration(
      _trt__GetVideoAnalyticsConfiguration* trt__GetVideoAnalyticsConfiguration,
      _trt__GetVideoAnalyticsConfigurationResponse&
          trt__GetVideoAnalyticsConfigurationResponse) override;
  //
  /// Web service operation 'GetMetadataConfiguration' implementation, should
  /// return SOAP_OK or error code
  virtual int GetMetadataConfiguration(
      _trt__GetMetadataConfiguration* trt__GetMetadataConfiguration,
      _trt__GetMetadataConfigurationResponse&
          trt__GetMetadataConfigurationResponse) override;
  //
  /// Web service operation 'GetAudioOutputConfiguration' implementation, should
  /// return SOAP_OK or error code
  virtual int GetAudioOutputConfiguration(
      _trt__GetAudioOutputConfiguration* trt__GetAudioOutputConfiguration,
      _trt__GetAudioOutputConfigurationResponse&
          trt__GetAudioOutputConfigurationResponse) override;
  //
  /// Web service operation 'GetAudioDecoderConfiguration' implementation,
  /// should return SOAP_OK or error code
  virtual int GetAudioDecoderConfiguration(
      _trt__GetAudioDecoderConfiguration* trt__GetAudioDecoderConfiguration,
      _trt__GetAudioDecoderConfigurationResponse&
          trt__GetAudioDecoderConfigurationResponse) override;
  //
  /// Web service operation 'GetCompatibleVideoEncoderConfigurations'
  /// implementation, should return SOAP_OK or error code
  virtual int GetCompatibleVideoEncoderConfigurations(
      _trt__GetCompatibleVideoEncoderConfigurations*
          trt__GetCompatibleVideoEncoderConfigurations,
      _trt__GetCompatibleVideoEncoderConfigurationsResponse&
          trt__GetCompatibleVideoEncoderConfigurationsResponse) override;
  //
  /// Web service operation 'GetCompatibleVideoSourceConfigurations'
  /// implementation, should return SOAP_OK or error code
  virtual int GetCompatibleVideoSourceConfigurations(
      _trt__GetCompatibleVideoSourceConfigurations*
          trt__GetCompatibleVideoSourceConfigurations,
      _trt__GetCompatibleVideoSourceConfigurationsResponse&
          trt__GetCompatibleVideoSourceConfigurationsResponse) override;
  //
  /// Web service operation 'GetCompatibleAudioEncoderConfigurations'
  /// implementation, should return SOAP_OK or error code
  virtual int GetCompatibleAudioEncoderConfigurations(
      _trt__GetCompatibleAudioEncoderConfigurations*
          trt__GetCompatibleAudioEncoderConfigurations,
      _trt__GetCompatibleAudioEncoderConfigurationsResponse&
          trt__GetCompatibleAudioEncoderConfigurationsResponse) override;
  //
  /// Web service operation 'GetCompatibleAudioSourceConfigurations'
  /// implementation, should return SOAP_OK or error code
  virtual int GetCompatibleAudioSourceConfigurations(
      _trt__GetCompatibleAudioSourceConfigurations*
          trt__GetCompatibleAudioSourceConfigurations,
      _trt__GetCompatibleAudioSourceConfigurationsResponse&
          trt__GetCompatibleAudioSourceConfigurationsResponse) override;
  //
  /// Web service operation 'GetCompatibleVideoAnalyticsConfigurations'
  /// implementation, should return SOAP_OK or error code
  virtual int GetCompatibleVideoAnalyticsConfigurations(
      _trt__GetCompatibleVideoAnalyticsConfigurations*
          trt__GetCompatibleVideoAnalyticsConfigurations,
      _trt__GetCompatibleVideoAnalyticsConfigurationsResponse&
          trt__GetCompatibleVideoAnalyticsConfigurationsResponse) override;
  //
  /// Web service operation 'GetCompatibleMetadataConfigurations'
  /// implementation, should return SOAP_OK or error code
  virtual int GetCompatibleMetadataConfigurations(
      _trt__GetCompatibleMetadataConfigurations*
          trt__GetCompatibleMetadataConfigurations,
      _trt__GetCompatibleMetadataConfigurationsResponse&
          trt__GetCompatibleMetadataConfigurationsResponse) override;
  //
  /// Web service operation 'GetCompatibleAudioOutputConfigurations'
  /// implementation, should return SOAP_OK or error code
  virtual int GetCompatibleAudioOutputConfigurations(
      _trt__GetCompatibleAudioOutputConfigurations*
          trt__GetCompatibleAudioOutputConfigurations,
      _trt__GetCompatibleAudioOutputConfigurationsResponse&
          trt__GetCompatibleAudioOutputConfigurationsResponse) override;
  //
  /// Web service operation 'GetCompatibleAudioDecoderConfigurations'
  /// implementation, should return SOAP_OK or error code
  virtual int GetCompatibleAudioDecoderConfigurations(
      _trt__GetCompatibleAudioDecoderConfigurations*
          trt__GetCompatibleAudioDecoderConfigurations,
      _trt__GetCompatibleAudioDecoderConfigurationsResponse&
          trt__GetCompatibleAudioDecoderConfigurationsResponse) override;
  //
  /// Web service operation 'SetVideoSourceConfiguration' implementation, should
  /// return SOAP_OK or error code
  virtual int SetVideoSourceConfiguration(
      _trt__SetVideoSourceConfiguration* trt__SetVideoSourceConfiguration,
      _trt__SetVideoSourceConfigurationResponse&
          trt__SetVideoSourceConfigurationResponse) override;
  //
  /// Web service operation 'SetVideoEncoderConfiguration' implementation,
  /// should return SOAP_OK or error code
  virtual int SetVideoEncoderConfiguration(
      _trt__SetVideoEncoderConfiguration* trt__SetVideoEncoderConfiguration,
      _trt__SetVideoEncoderConfigurationResponse&
          trt__SetVideoEncoderConfigurationResponse) override;
  //
  /// Web service operation 'SetAudioSourceConfiguration' implementation, should
  /// return SOAP_OK or error code
  virtual int SetAudioSourceConfiguration(
      _trt__SetAudioSourceConfiguration* trt__SetAudioSourceConfiguration,
      _trt__SetAudioSourceConfigurationResponse&
          trt__SetAudioSourceConfigurationResponse) override;
  //
  /// Web service operation 'SetAudioEncoderConfiguration' implementation,
  /// should return SOAP_OK or error code
  virtual int SetAudioEncoderConfiguration(
      _trt__SetAudioEncoderConfiguration* trt__SetAudioEncoderConfiguration,
      _trt__SetAudioEncoderConfigurationResponse&
          trt__SetAudioEncoderConfigurationResponse) override;
  //
  /// Web service operation 'SetVideoAnalyticsConfiguration' implementation,
  /// should return SOAP_OK or error code
  virtual int SetVideoAnalyticsConfiguration(
      _trt__SetVideoAnalyticsConfiguration* trt__SetVideoAnalyticsConfiguration,
      _trt__SetVideoAnalyticsConfigurationResponse&
          trt__SetVideoAnalyticsConfigurationResponse) override;
  //
  /// Web service operation 'SetMetadataConfiguration' implementation, should
  /// return SOAP_OK or error code
  virtual int SetMetadataConfiguration(
      _trt__SetMetadataConfiguration* trt__SetMetadataConfiguration,
      _trt__SetMetadataConfigurationResponse&
          trt__SetMetadataConfigurationResponse) override;
  //
  /// Web service operation 'SetAudioOutputConfiguration' implementation, should
  /// return SOAP_OK or error code
  virtual int SetAudioOutputConfiguration(
      _trt__SetAudioOutputConfiguration* trt__SetAudioOutputConfiguration,
      _trt__SetAudioOutputConfigurationResponse&
          trt__SetAudioOutputConfigurationResponse) override;
  //
  /// Web service operation 'SetAudioDecoderConfiguration' implementation,
  /// should return SOAP_OK or error code
  virtual int SetAudioDecoderConfiguration(
      _trt__SetAudioDecoderConfiguration* trt__SetAudioDecoderConfiguration,
      _trt__SetAudioDecoderConfigurationResponse&
          trt__SetAudioDecoderConfigurationResponse) override;
  //
  /// Web service operation 'GetVideoSourceConfigurationOptions' implementation,
  /// should return SOAP_OK or error code
  virtual int GetVideoSourceConfigurationOptions(
      _trt__GetVideoSourceConfigurationOptions*
          trt__GetVideoSourceConfigurationOptions,
      _trt__GetVideoSourceConfigurationOptionsResponse&
          trt__GetVideoSourceConfigurationOptionsResponse) override;
  //
  /// Web service operation 'GetVideoEncoderConfigurationOptions'
  /// implementation, should return SOAP_OK or error code
  virtual int GetVideoEncoderConfigurationOptions(
      _trt__GetVideoEncoderConfigurationOptions*
          trt__GetVideoEncoderConfigurationOptions,
      _trt__GetVideoEncoderConfigurationOptionsResponse&
          trt__GetVideoEncoderConfigurationOptionsResponse) override;
  //
  /// Web service operation 'GetAudioSourceConfigurationOptions' implementation,
  /// should return SOAP_OK or error code
  virtual int GetAudioSourceConfigurationOptions(
      _trt__GetAudioSourceConfigurationOptions*
          trt__GetAudioSourceConfigurationOptions,
      _trt__GetAudioSourceConfigurationOptionsResponse&
          trt__GetAudioSourceConfigurationOptionsResponse) override;
  //
  /// Web service operation 'GetAudioEncoderConfigurationOptions'
  /// implementation, should return SOAP_OK or error code
  virtual int GetAudioEncoderConfigurationOptions(
      _trt__GetAudioEncoderConfigurationOptions*
          trt__GetAudioEncoderConfigurationOptions,
      _trt__GetAudioEncoderConfigurationOptionsResponse&
          trt__GetAudioEncoderConfigurationOptionsResponse) override;
  //
  /// Web service operation 'GetMetadataConfigurationOptions' implementation,
  /// should return SOAP_OK or error code
  virtual int GetMetadataConfigurationOptions(
      _trt__GetMetadataConfigurationOptions*
          trt__GetMetadataConfigurationOptions,
      _trt__GetMetadataConfigurationOptionsResponse&
          trt__GetMetadataConfigurationOptionsResponse) override;
  //
  /// Web service operation 'GetAudioOutputConfigurationOptions' implementation,
  /// should return SOAP_OK or error code
  virtual int GetAudioOutputConfigurationOptions(
      _trt__GetAudioOutputConfigurationOptions*
          trt__GetAudioOutputConfigurationOptions,
      _trt__GetAudioOutputConfigurationOptionsResponse&
          trt__GetAudioOutputConfigurationOptionsResponse) override;
  //
  /// Web service operation 'GetAudioDecoderConfigurationOptions'
  /// implementation, should return SOAP_OK or error code
  virtual int GetAudioDecoderConfigurationOptions(
      _trt__GetAudioDecoderConfigurationOptions*
          trt__GetAudioDecoderConfigurationOptions,
      _trt__GetAudioDecoderConfigurationOptionsResponse&
          trt__GetAudioDecoderConfigurationOptionsResponse) override;
  //
  /// Web service operation 'GetGuaranteedNumberOfVideoEncoderInstances'
  /// implementation, should return SOAP_OK or error code
  virtual int GetGuaranteedNumberOfVideoEncoderInstances(
      _trt__GetGuaranteedNumberOfVideoEncoderInstances*
          trt__GetGuaranteedNumberOfVideoEncoderInstances,
      _trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse&
          trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse) override;
  //
  /// Web service operation 'GetStreamUri' implementation, should return SOAP_OK
  /// or error code
  virtual int GetStreamUri(
      _trt__GetStreamUri* trt__GetStreamUri,
      _trt__GetStreamUriResponse& trt__GetStreamUriResponse) override;
  //
  /// Web service operation 'StartMulticastStreaming' implementation, should
  /// return SOAP_OK or error code
  virtual int StartMulticastStreaming(
      _trt__StartMulticastStreaming* trt__StartMulticastStreaming,
      _trt__StartMulticastStreamingResponse&
          trt__StartMulticastStreamingResponse) override;
  //
  /// Web service operation 'StopMulticastStreaming' implementation, should
  /// return SOAP_OK or error code
  virtual int StopMulticastStreaming(
      _trt__StopMulticastStreaming* trt__StopMulticastStreaming,
      _trt__StopMulticastStreamingResponse& trt__StopMulticastStreamingResponse)
      override;
  //
  /// Web service operation 'SetSynchronizationPoint' implementation, should
  /// return SOAP_OK or error code
  virtual int SetSynchronizationPoint(
      _trt__SetSynchronizationPoint* trt__SetSynchronizationPoint,
      _trt__SetSynchronizationPointResponse&
          trt__SetSynchronizationPointResponse) override;
  //
  /// Web service operation 'GetSnapshotUri' implementation, should return
  /// SOAP_OK or error code
  virtual int GetSnapshotUri(
      _trt__GetSnapshotUri* trt__GetSnapshotUri,
      _trt__GetSnapshotUriResponse& trt__GetSnapshotUriResponse) override;
  //
  /// Web service operation 'GetVideoSourceModes' implementation, should return
  /// SOAP_OK or error code
  virtual int GetVideoSourceModes(
      _trt__GetVideoSourceModes* trt__GetVideoSourceModes,
      _trt__GetVideoSourceModesResponse& trt__GetVideoSourceModesResponse)
      override;
  //
  /// Web service operation 'SetVideoSourceMode' implementation, should return
  /// SOAP_OK or error code
  virtual int SetVideoSourceMode(
      _trt__SetVideoSourceMode* trt__SetVideoSourceMode,
      _trt__SetVideoSourceModeResponse& trt__SetVideoSourceModeResponse)
      override;
  //
  /// Web service operation 'GetOSDs' implementation, should return SOAP_OK or
  /// error code
  virtual int GetOSDs(_trt__GetOSDs* trt__GetOSDs,
                      _trt__GetOSDsResponse& trt__GetOSDsResponse) override;
  //
  /// Web service operation 'GetOSD' implementation, should return SOAP_OK or
  /// error code
  virtual int GetOSD(_trt__GetOSD* trt__GetOSD,
                     _trt__GetOSDResponse& trt__GetOSDResponse) override;
  //
  /// Web service operation 'GetOSDOptions' implementation, should return
  /// SOAP_OK or error code
  virtual int GetOSDOptions(
      _trt__GetOSDOptions* trt__GetOSDOptions,
      _trt__GetOSDOptionsResponse& trt__GetOSDOptionsResponse) override;
  //
  /// Web service operation 'SetOSD' implementation, should return SOAP_OK or
  /// error code
  virtual int SetOSD(_trt__SetOSD* trt__SetOSD,
                     _trt__SetOSDResponse& trt__SetOSDResponse) override;
  //
  /// Web service operation 'CreateOSD' implementation, should return SOAP_OK or
  /// error code
  virtual int CreateOSD(
      _trt__CreateOSD* trt__CreateOSD,
      _trt__CreateOSDResponse& trt__CreateOSDResponse) override;
  //
  /// Web service operation 'DeleteOSD' implementation, should return SOAP_OK or
  /// error code
  virtual int DeleteOSD(
      _trt__DeleteOSD* trt__DeleteOSD,
      _trt__DeleteOSDResponse& trt__DeleteOSDResponse) override;

 private:
  /* data */
};

}  // namespace ave

#endif /* !ONVIF_MEDIA_BINDING_SERVICE_H */
