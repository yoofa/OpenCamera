/*
 * onvif_ptz_binding_service.h
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef ONVIF_PTZ_BINDING_SERVICE_H
#define ONVIF_PTZ_BINDING_SERVICE_H

#include "third_party/onvif_srvd/src/generated/soapPTZBindingService.h"

namespace ave {
class OnvifPTZBindingService : public PTZBindingService {
 public:
  OnvifPTZBindingService(struct soap* soap);
  virtual ~OnvifPTZBindingService();

  //
  // Service operations are listed below: you should define these
  // Note: compile with -DWITH_PURE_VIRTUAL to declare pure virtual methods
  //
  /// Web service operation 'GetServiceCapabilities' implementation, should
  /// return SOAP_OK or error code
  virtual int GetServiceCapabilities(
      _tptz__GetServiceCapabilities* tptz__GetServiceCapabilities,
      _tptz__GetServiceCapabilitiesResponse&
          tptz__GetServiceCapabilitiesResponse) override;
  //
  /// Web service operation 'GetConfigurations' implementation, should return
  /// SOAP_OK or error code
  virtual int GetConfigurations(
      _tptz__GetConfigurations* tptz__GetConfigurations,
      _tptz__GetConfigurationsResponse& tptz__GetConfigurationsResponse)
      override;
  //
  /// Web service operation 'GetPresets' implementation, should return SOAP_OK
  /// or error code
  virtual int GetPresets(
      _tptz__GetPresets* tptz__GetPresets,
      _tptz__GetPresetsResponse& tptz__GetPresetsResponse) override;
  //
  /// Web service operation 'SetPreset' implementation, should return SOAP_OK or
  /// error code
  virtual int SetPreset(
      _tptz__SetPreset* tptz__SetPreset,
      _tptz__SetPresetResponse& tptz__SetPresetResponse) override;
  //
  /// Web service operation 'RemovePreset' implementation, should return SOAP_OK
  /// or error code
  virtual int RemovePreset(
      _tptz__RemovePreset* tptz__RemovePreset,
      _tptz__RemovePresetResponse& tptz__RemovePresetResponse) override;
  //
  /// Web service operation 'GotoPreset' implementation, should return SOAP_OK
  /// or error code
  virtual int GotoPreset(
      _tptz__GotoPreset* tptz__GotoPreset,
      _tptz__GotoPresetResponse& tptz__GotoPresetResponse) override;
  //
  /// Web service operation 'GetStatus' implementation, should return SOAP_OK or
  /// error code
  virtual int GetStatus(
      _tptz__GetStatus* tptz__GetStatus,
      _tptz__GetStatusResponse& tptz__GetStatusResponse) override;
  //
  /// Web service operation 'GetConfiguration' implementation, should return
  /// SOAP_OK or error code
  virtual int GetConfiguration(
      _tptz__GetConfiguration* tptz__GetConfiguration,
      _tptz__GetConfigurationResponse& tptz__GetConfigurationResponse) override;
  //
  /// Web service operation 'GetNodes' implementation, should return SOAP_OK or
  /// error code
  virtual int GetNodes(
      _tptz__GetNodes* tptz__GetNodes,
      _tptz__GetNodesResponse& tptz__GetNodesResponse) override;
  //
  /// Web service operation 'GetNode' implementation, should return SOAP_OK or
  /// error code
  virtual int GetNode(_tptz__GetNode* tptz__GetNode,
                      _tptz__GetNodeResponse& tptz__GetNodeResponse) override;
  //
  /// Web service operation 'SetConfiguration' implementation, should return
  /// SOAP_OK or error code
  virtual int SetConfiguration(
      _tptz__SetConfiguration* tptz__SetConfiguration,
      _tptz__SetConfigurationResponse& tptz__SetConfigurationResponse) override;
  //
  /// Web service operation 'GetConfigurationOptions' implementation, should
  /// return SOAP_OK or error code
  virtual int GetConfigurationOptions(
      _tptz__GetConfigurationOptions* tptz__GetConfigurationOptions,
      _tptz__GetConfigurationOptionsResponse&
          tptz__GetConfigurationOptionsResponse) override;
  //
  /// Web service operation 'GotoHomePosition' implementation, should return
  /// SOAP_OK or error code
  virtual int GotoHomePosition(
      _tptz__GotoHomePosition* tptz__GotoHomePosition,
      _tptz__GotoHomePositionResponse& tptz__GotoHomePositionResponse) override;
  //
  /// Web service operation 'SetHomePosition' implementation, should return
  /// SOAP_OK or error code
  virtual int SetHomePosition(
      _tptz__SetHomePosition* tptz__SetHomePosition,
      _tptz__SetHomePositionResponse& tptz__SetHomePositionResponse) override;
  //
  /// Web service operation 'ContinuousMove' implementation, should return
  /// SOAP_OK or error code
  virtual int ContinuousMove(
      _tptz__ContinuousMove* tptz__ContinuousMove,
      _tptz__ContinuousMoveResponse& tptz__ContinuousMoveResponse) override;
  //
  /// Web service operation 'RelativeMove' implementation, should return SOAP_OK
  /// or error code
  virtual int RelativeMove(
      _tptz__RelativeMove* tptz__RelativeMove,
      _tptz__RelativeMoveResponse& tptz__RelativeMoveResponse) override;
  //
  /// Web service operation 'SendAuxiliaryCommand' implementation, should return
  /// SOAP_OK or error code
  virtual int SendAuxiliaryCommand(
      _tptz__SendAuxiliaryCommand* tptz__SendAuxiliaryCommand,
      _tptz__SendAuxiliaryCommandResponse& tptz__SendAuxiliaryCommandResponse)
      override;
  //
  /// Web service operation 'AbsoluteMove' implementation, should return SOAP_OK
  /// or error code
  virtual int AbsoluteMove(
      _tptz__AbsoluteMove* tptz__AbsoluteMove,
      _tptz__AbsoluteMoveResponse& tptz__AbsoluteMoveResponse) override;
  //
  /// Web service operation 'GeoMove' implementation, should return SOAP_OK or
  /// error code
  virtual int GeoMove(_tptz__GeoMove* tptz__GeoMove,
                      _tptz__GeoMoveResponse& tptz__GeoMoveResponse) override;
  //
  /// Web service operation 'Stop' implementation, should return SOAP_OK or
  /// error code
  virtual int Stop(_tptz__Stop* tptz__Stop,
                   _tptz__StopResponse& tptz__StopResponse) override;
  //
  /// Web service operation 'GetPresetTours' implementation, should return
  /// SOAP_OK or error code
  virtual int GetPresetTours(
      _tptz__GetPresetTours* tptz__GetPresetTours,
      _tptz__GetPresetToursResponse& tptz__GetPresetToursResponse) override;
  //
  /// Web service operation 'GetPresetTour' implementation, should return
  /// SOAP_OK or error code
  virtual int GetPresetTour(
      _tptz__GetPresetTour* tptz__GetPresetTour,
      _tptz__GetPresetTourResponse& tptz__GetPresetTourResponse) override;
  //
  /// Web service operation 'GetPresetTourOptions' implementation, should return
  /// SOAP_OK or error code
  virtual int GetPresetTourOptions(
      _tptz__GetPresetTourOptions* tptz__GetPresetTourOptions,
      _tptz__GetPresetTourOptionsResponse& tptz__GetPresetTourOptionsResponse)
      override;
  //
  /// Web service operation 'CreatePresetTour' implementation, should return
  /// SOAP_OK or error code
  virtual int CreatePresetTour(
      _tptz__CreatePresetTour* tptz__CreatePresetTour,
      _tptz__CreatePresetTourResponse& tptz__CreatePresetTourResponse) override;
  //
  /// Web service operation 'ModifyPresetTour' implementation, should return
  /// SOAP_OK or error code
  virtual int ModifyPresetTour(
      _tptz__ModifyPresetTour* tptz__ModifyPresetTour,
      _tptz__ModifyPresetTourResponse& tptz__ModifyPresetTourResponse) override;
  //
  /// Web service operation 'OperatePresetTour' implementation, should return
  /// SOAP_OK or error code
  virtual int OperatePresetTour(
      _tptz__OperatePresetTour* tptz__OperatePresetTour,
      _tptz__OperatePresetTourResponse& tptz__OperatePresetTourResponse)
      override;
  //
  /// Web service operation 'RemovePresetTour' implementation, should return
  /// SOAP_OK or error code
  virtual int RemovePresetTour(
      _tptz__RemovePresetTour* tptz__RemovePresetTour,
      _tptz__RemovePresetTourResponse& tptz__RemovePresetTourResponse) override;
  //
  /// Web service operation 'GetCompatibleConfigurations' implementation, should
  /// return SOAP_OK or error code
  virtual int GetCompatibleConfigurations(
      _tptz__GetCompatibleConfigurations* tptz__GetCompatibleConfigurations,
      _tptz__GetCompatibleConfigurationsResponse&
          tptz__GetCompatibleConfigurationsResponse) override;
  //
  /// Web service operation 'MoveAndStartTracking' implementation, should return
  /// SOAP_OK or error code
  virtual int MoveAndStartTracking(
      _tptz__MoveAndStartTracking* tptz__MoveAndStartTracking,
      _tptz__MoveAndStartTrackingResponse& tptz__MoveAndStartTrackingResponse)
      override;

 private:
  /* data */
};
}  // namespace ave
#endif /* !ONVIF_PTZ_BINDING_SERVICE_H */
