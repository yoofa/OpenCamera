/*
 * onvif_ptz_binding_service.cc
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "onvif_ptz_binding_service.h"

#include "ServiceContext.h"
#include "base/logging.h"
#include "onvif_server.h"
#include "smacros.h"
#include "stools.h"

namespace ave {

OnvifPTZBindingService::OnvifPTZBindingService(struct soap* soap)
    : PTZBindingService(soap) {}

OnvifPTZBindingService::~OnvifPTZBindingService() {}

int OnvifPTZBindingService::GetServiceCapabilities(
    _tptz__GetServiceCapabilities* tptz__GetServiceCapabilities,
    _tptz__GetServiceCapabilitiesResponse&
        tptz__GetServiceCapabilitiesResponse) {
  SOAP_EMPTY_HANDLER(tptz__GetServiceCapabilities, "PTZ");
}

int OnvifPTZBindingService::GetConfigurations(
    _tptz__GetConfigurations* tptz__GetConfigurations,
    _tptz__GetConfigurationsResponse& tptz__GetConfigurationsResponse) {
  AVE_LOG(LS_INFO) << "PTZ: " << __FUNCTION__;
  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->service_info_.get();

  tt__PTZConfiguration* pPTZCfg = ctx->GetPTZConfiguration(soap);
  if (pPTZCfg != NULL) {
    tptz__GetConfigurationsResponse.PTZConfiguration.push_back(pPTZCfg);
  }
  return SOAP_OK;
}

int GetPTZPreset(struct soap* soap, tt__PTZPreset* ptzp, int number) {
  ptzp->token = soap_new_std__string(soap);
  *ptzp->token = std::to_string(number);
  ptzp->Name = soap_new_std__string(soap);
  *ptzp->Name = std::to_string(number);

  ptzp->PTZPosition = soap_new_tt__PTZVector(soap);
  ptzp->PTZPosition->PanTilt = soap_new_req_tt__Vector2D(soap, 0.0f, 0.0f);
  ptzp->PTZPosition->Zoom = soap_new_req_tt__Vector1D(soap, 1.0f);

  return SOAP_OK;
}

int OnvifPTZBindingService::GetPresets(
    _tptz__GetPresets* tptz__GetPresets,
    _tptz__GetPresetsResponse& tptz__GetPresetsResponse) {
  UNUSED(tptz__GetPresets);
  AVE_LOG(LS_INFO) << "PTZ: " << __FUNCTION__;

  soap_default_std__vectorTemplateOfPointerTott__PTZPreset(
      soap, &tptz__GetPresetsResponse._tptz__GetPresetsResponse::Preset);
  for (int i = 0; i < 8; i++) {
    tt__PTZPreset* ptzp;
    ptzp = soap_new_tt__PTZPreset(soap);
    tptz__GetPresetsResponse.Preset.push_back(ptzp);
    GetPTZPreset(this->soap, ptzp, i);
  }

  return SOAP_OK;
}

int OnvifPTZBindingService::SetPreset(
    _tptz__SetPreset* tptz__SetPreset,
    _tptz__SetPresetResponse& tptz__SetPresetResponse) {
  SOAP_EMPTY_HANDLER(tptz__SetPreset, "PTZ");
}

int OnvifPTZBindingService::RemovePreset(
    _tptz__RemovePreset* tptz__RemovePreset,
    _tptz__RemovePresetResponse& tptz__RemovePresetResponse) {
  SOAP_EMPTY_HANDLER(tptz__RemovePreset, "PTZ");
}

int OnvifPTZBindingService::GotoPreset(
    _tptz__GotoPreset* tptz__GotoPreset,
    _tptz__GotoPresetResponse& tptz__GotoPresetResponse) {
  UNUSED(tptz__GotoPresetResponse);
  AVE_LOG(LS_INFO) << "PTZ: " << __FUNCTION__;

  std::string preset_cmd;

  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->service_info_.get();

  if (tptz__GotoPreset == NULL) {
    return SOAP_OK;
  }
  if (tptz__GotoPreset->ProfileToken.c_str() == NULL) {
    return SOAP_OK;
  }
  if (tptz__GotoPreset->PresetToken.c_str() == NULL) {
    return SOAP_OK;
  }

  if (!ctx->get_ptz_node()->get_move_preset().empty()) {
    preset_cmd = ctx->get_ptz_node()->get_move_preset().c_str();
  } else {
    return SOAP_OK;
  }

  std::string template_str_t("%t");

  auto it_t = preset_cmd.find(template_str_t, 0);

  if (it_t != std::string::npos) {
    preset_cmd.replace(it_t, template_str_t.size(),
                       tptz__GotoPreset->PresetToken.c_str());
  }

  system(preset_cmd.c_str());

  return SOAP_OK;
}

int OnvifPTZBindingService::GetStatus(
    _tptz__GetStatus* tptz__GetStatus,
    _tptz__GetStatusResponse& tptz__GetStatusResponse) {
  SOAP_EMPTY_HANDLER(tptz__GetStatus, "PTZ");
}

int OnvifPTZBindingService::GetConfiguration(
    _tptz__GetConfiguration* tptz__GetConfiguration,
    _tptz__GetConfigurationResponse& tptz__GetConfigurationResponse) {
  AVE_LOG(LS_INFO) << "PTZ: " << __FUNCTION__;
  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->service_info_.get();

  tt__PTZConfiguration* pPTZCfg = ctx->GetPTZConfiguration(soap);
  if (pPTZCfg) {
    tptz__GetConfigurationResponse.PTZConfiguration = pPTZCfg;
  }
  return SOAP_OK;
}

int GetPTZNode(struct soap* soap, tt__PTZNode* ptzn) {
  ptzn->token = "PTZNodeToken";
  ptzn->Name = soap_new_std__string(soap);
  *ptzn->Name = "PTZ";

  ptzn->SupportedPTZSpaces = soap_new_tt__PTZSpaces(soap);
  ;
  soap_default_std__vectorTemplateOfPointerTott__Space2DDescription(
      soap, &ptzn->SupportedPTZSpaces
                 ->tt__PTZSpaces::RelativePanTiltTranslationSpace);
  soap_default_std__vectorTemplateOfPointerTott__Space1DDescription(
      soap,
      &ptzn->SupportedPTZSpaces->tt__PTZSpaces::RelativeZoomTranslationSpace);
  soap_default_std__vectorTemplateOfPointerTott__Space2DDescription(
      soap,
      &ptzn->SupportedPTZSpaces->tt__PTZSpaces::ContinuousPanTiltVelocitySpace);
  soap_default_std__vectorTemplateOfPointerTott__Space1DDescription(
      soap,
      &ptzn->SupportedPTZSpaces->tt__PTZSpaces::ContinuousZoomVelocitySpace);
  soap_default_std__vectorTemplateOfPointerTott__Space1DDescription(
      soap, &ptzn->SupportedPTZSpaces->tt__PTZSpaces::PanTiltSpeedSpace);
  soap_default_std__vectorTemplateOfPointerTott__Space1DDescription(
      soap, &ptzn->SupportedPTZSpaces->tt__PTZSpaces::ZoomSpeedSpace);

  auto ptzs1 = soap_new_tt__Space2DDescription(soap);
  ptzn->SupportedPTZSpaces->RelativePanTiltTranslationSpace.push_back(ptzs1);

  auto ptzs2 = soap_new_tt__Space1DDescription(soap);
  ptzn->SupportedPTZSpaces->RelativeZoomTranslationSpace.push_back(ptzs2);

  auto ptzs3 = soap_new_tt__Space2DDescription(soap);
  ptzn->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace.push_back(ptzs3);

  auto ptzs4 = soap_new_tt__Space1DDescription(soap);
  ptzn->SupportedPTZSpaces->ContinuousZoomVelocitySpace.push_back(ptzs4);

  auto ptzs5 = soap_new_tt__Space1DDescription(soap);
  ptzn->SupportedPTZSpaces->PanTiltSpeedSpace.push_back(ptzs5);

  auto ptzs6 = soap_new_tt__Space1DDescription(soap);
  ptzn->SupportedPTZSpaces->ZoomSpeedSpace.push_back(ptzs6);

  ptzs1->URI =
      "http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationGenericSpace";
  ptzs1->XRange = soap_new_req_tt__FloatRange(soap, -1.0f, 1.0f);
  ptzs1->YRange = soap_new_req_tt__FloatRange(soap, -1.0f, 1.0f);

  ptzs2->URI =
      "http://www.onvif.org/ver10/tptz/ZoomSpaces/TranslationGenericSpace";
  ptzs2->XRange = soap_new_req_tt__FloatRange(soap, -1.0f, 1.0f);

  ptzs3->URI =
      "http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace";
  ptzs3->XRange = soap_new_req_tt__FloatRange(soap, -1.0f, 1.0f);
  ptzs3->YRange = soap_new_req_tt__FloatRange(soap, -1.0f, 1.0f);

  ptzs4->URI =
      "http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace";
  ptzs4->XRange = soap_new_req_tt__FloatRange(soap, -1.0f, 1.0f);

  ptzs5->URI =
      "http://www.onvif.org/ver10/tptz/PanTiltSpaces/GenericSpeedSpace";
  ptzs5->XRange = soap_new_req_tt__FloatRange(soap, 0.0f, 1.0f);

  ptzs6->URI =
      "http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace";
  ptzs6->XRange = soap_new_req_tt__FloatRange(soap, 0.0f, 1.0f);

  ptzn->MaximumNumberOfPresets = 8;
  ptzn->HomeSupported = true;
  ptzn->FixedHomePosition = (bool*)soap_malloc(soap, sizeof(bool));
  soap_s2bool(soap, "true", ptzn->FixedHomePosition);

  return SOAP_OK;
}

int OnvifPTZBindingService::GetNodes(
    _tptz__GetNodes* tptz__GetNodes,
    _tptz__GetNodesResponse& tptz__GetNodesResponse) {
  UNUSED(tptz__GetNodes);
  AVE_LOG(LS_INFO) << "PTZ: " << __FUNCTION__;

  soap_default_std__vectorTemplateOfPointerTott__PTZNode(
      soap, &tptz__GetNodesResponse._tptz__GetNodesResponse::PTZNode);
  tt__PTZNode* ptzn;
  ptzn = soap_new_tt__PTZNode(soap);
  tptz__GetNodesResponse.PTZNode.push_back(ptzn);
  GetPTZNode(this->soap, ptzn);

  return SOAP_OK;
}

int OnvifPTZBindingService::GetNode(
    _tptz__GetNode* tptz__GetNode,
    _tptz__GetNodeResponse& tptz__GetNodeResponse) {
  UNUSED(tptz__GetNode);
  AVE_LOG(LS_INFO) << "PTZ: " << __FUNCTION__;

  tptz__GetNodeResponse.PTZNode = soap_new_tt__PTZNode(this->soap);
  GetPTZNode(this->soap, tptz__GetNodeResponse.PTZNode);

  return SOAP_OK;
}

int OnvifPTZBindingService::SetConfiguration(
    _tptz__SetConfiguration* tptz__SetConfiguration,
    _tptz__SetConfigurationResponse& tptz__SetConfigurationResponse) {
  SOAP_EMPTY_HANDLER(tptz__SetConfiguration, "PTZ");
}

int OnvifPTZBindingService::GetConfigurationOptions(
    _tptz__GetConfigurationOptions* tptz__GetConfigurationOptions,
    _tptz__GetConfigurationOptionsResponse&
        tptz__GetConfigurationOptionsResponse) {
  AVE_LOG(LS_INFO) << "PTZ: " << __FUNCTION__;
  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->service_info_.get();

  tptz__GetConfigurationOptionsResponse.PTZConfigurationOptions =
      ctx->GetPTZConfigurationOptions(soap);
  return SOAP_OK;
}

int OnvifPTZBindingService::GotoHomePosition(
    _tptz__GotoHomePosition* tptz__GotoHomePosition,
    _tptz__GotoHomePositionResponse& tptz__GotoHomePositionResponse) {
  UNUSED(tptz__GotoHomePosition);
  UNUSED(tptz__GotoHomePositionResponse);
  AVE_LOG(LS_INFO) << "PTZ: " << __FUNCTION__;

  std::string preset_cmd;

  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->service_info_.get();

  if (tptz__GotoHomePosition == NULL) {
    return SOAP_OK;
  }
  if (tptz__GotoHomePosition->ProfileToken.c_str() == NULL) {
    return SOAP_OK;
  }

  if (!ctx->get_ptz_node()->get_move_preset().empty()) {
    preset_cmd = ctx->get_ptz_node()->get_move_preset().c_str();
  } else {
    return SOAP_OK;
  }

  std::string template_str_t("%t");

  auto it_t = preset_cmd.find(template_str_t, 0);

  if (it_t != std::string::npos) {
    preset_cmd.replace(it_t, template_str_t.size(), "1");
  }

  system(preset_cmd.c_str());

  return SOAP_OK;
}

int OnvifPTZBindingService::SetHomePosition(
    _tptz__SetHomePosition* tptz__SetHomePosition,
    _tptz__SetHomePositionResponse& tptz__SetHomePositionResponse) {
  SOAP_EMPTY_HANDLER(tptz__SetHomePosition, "PTZ");
}

int OnvifPTZBindingService::ContinuousMove(
    _tptz__ContinuousMove* tptz__ContinuousMove,
    _tptz__ContinuousMoveResponse& tptz__ContinuousMoveResponse) {
  UNUSED(tptz__ContinuousMoveResponse);
  AVE_LOG(LS_INFO) << "PTZ: " << __FUNCTION__;

  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->service_info_.get();

  if (tptz__ContinuousMove == NULL) {
    return SOAP_OK;
  }
  if (tptz__ContinuousMove->Velocity == NULL) {
    return SOAP_OK;
  }
  if (tptz__ContinuousMove->Velocity->PanTilt == NULL) {
    return SOAP_OK;
  }

  if (tptz__ContinuousMove->Velocity->PanTilt->x > 0) {
    system(ctx->get_ptz_node()->get_move_right().c_str());
  } else if (tptz__ContinuousMove->Velocity->PanTilt->x < 0) {
    system(ctx->get_ptz_node()->get_move_left().c_str());
  }
  if (tptz__ContinuousMove->Velocity->PanTilt->y > 0) {
    system(ctx->get_ptz_node()->get_move_up().c_str());
  } else if (tptz__ContinuousMove->Velocity->PanTilt->y < 0) {
    system(ctx->get_ptz_node()->get_move_down().c_str());
  }

  return SOAP_OK;
}

int OnvifPTZBindingService::RelativeMove(
    _tptz__RelativeMove* tptz__RelativeMove,
    _tptz__RelativeMoveResponse& tptz__RelativeMoveResponse) {
  UNUSED(tptz__RelativeMoveResponse);
  AVE_LOG(LS_INFO) << "PTZ: " << __FUNCTION__;

  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->service_info_.get();

  if (tptz__RelativeMove == NULL) {
    return SOAP_OK;
  }
  if (tptz__RelativeMove->Translation == NULL) {
    return SOAP_OK;
  }
  if (tptz__RelativeMove->Translation->PanTilt == NULL) {
    return SOAP_OK;
  }

  if (tptz__RelativeMove->Translation->PanTilt->x > 0) {
    system(ctx->get_ptz_node()->get_move_right().c_str());
    usleep(300000);
    system(ctx->get_ptz_node()->get_move_stop().c_str());
  } else if (tptz__RelativeMove->Translation->PanTilt->x < 0) {
    system(ctx->get_ptz_node()->get_move_left().c_str());
    usleep(300000);
    system(ctx->get_ptz_node()->get_move_stop().c_str());
  }
  if (tptz__RelativeMove->Translation->PanTilt->y > 0) {
    system(ctx->get_ptz_node()->get_move_up().c_str());
    usleep(300000);
    system(ctx->get_ptz_node()->get_move_stop().c_str());
  } else if (tptz__RelativeMove->Translation->PanTilt->y < 0) {
    system(ctx->get_ptz_node()->get_move_down().c_str());
    usleep(300000);
    system(ctx->get_ptz_node()->get_move_stop().c_str());
  }

  return SOAP_OK;
}

int OnvifPTZBindingService::SendAuxiliaryCommand(
    _tptz__SendAuxiliaryCommand* tptz__SendAuxiliaryCommand,
    _tptz__SendAuxiliaryCommandResponse& tptz__SendAuxiliaryCommandResponse) {
  SOAP_EMPTY_HANDLER(tptz__SendAuxiliaryCommand, "PTZ");
}

int OnvifPTZBindingService::AbsoluteMove(
    _tptz__AbsoluteMove* tptz__AbsoluteMove,
    _tptz__AbsoluteMoveResponse& tptz__AbsoluteMoveResponse) {
  SOAP_EMPTY_HANDLER(tptz__AbsoluteMove, "PTZ");
}

int OnvifPTZBindingService::Stop(_tptz__Stop* tptz__Stop,
                                 _tptz__StopResponse& tptz__StopResponse) {
  UNUSED(tptz__Stop);
  UNUSED(tptz__StopResponse);
  AVE_LOG(LS_INFO) << "PTZ: " << __FUNCTION__;

  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->service_info_.get();

  system(ctx->get_ptz_node()->get_move_stop().c_str());

  return SOAP_OK;
}

int OnvifPTZBindingService::GetPresetTours(
    _tptz__GetPresetTours* tptz__GetPresetTours,
    _tptz__GetPresetToursResponse& tptz__GetPresetToursResponse) {
  SOAP_EMPTY_HANDLER(tptz__GetPresetTours, "PTZ");
}

int OnvifPTZBindingService::GetPresetTour(
    _tptz__GetPresetTour* tptz__GetPresetTour,
    _tptz__GetPresetTourResponse& tptz__GetPresetTourResponse) {
  SOAP_EMPTY_HANDLER(tptz__GetPresetTour, "PTZ");
}

int OnvifPTZBindingService::GetPresetTourOptions(
    _tptz__GetPresetTourOptions* tptz__GetPresetTourOptions,
    _tptz__GetPresetTourOptionsResponse& tptz__GetPresetTourOptionsResponse) {
  SOAP_EMPTY_HANDLER(tptz__GetPresetTourOptions, "PTZ");
}

int OnvifPTZBindingService::CreatePresetTour(
    _tptz__CreatePresetTour* tptz__CreatePresetTour,
    _tptz__CreatePresetTourResponse& tptz__CreatePresetTourResponse) {
  SOAP_EMPTY_HANDLER(tptz__CreatePresetTour, "PTZ");
}

int OnvifPTZBindingService::ModifyPresetTour(
    _tptz__ModifyPresetTour* tptz__ModifyPresetTour,
    _tptz__ModifyPresetTourResponse& tptz__ModifyPresetTourResponse) {
  SOAP_EMPTY_HANDLER(tptz__ModifyPresetTour, "PTZ");
}

int OnvifPTZBindingService::OperatePresetTour(
    _tptz__OperatePresetTour* tptz__OperatePresetTour,
    _tptz__OperatePresetTourResponse& tptz__OperatePresetTourResponse) {
  SOAP_EMPTY_HANDLER(tptz__OperatePresetTour, "PTZ");
}

int OnvifPTZBindingService::RemovePresetTour(
    _tptz__RemovePresetTour* tptz__RemovePresetTour,
    _tptz__RemovePresetTourResponse& tptz__RemovePresetTourResponse) {
  SOAP_EMPTY_HANDLER(tptz__RemovePresetTour, "PTZ");
}

int OnvifPTZBindingService::GetCompatibleConfigurations(
    _tptz__GetCompatibleConfigurations* tptz__GetCompatibleConfigurations,
    _tptz__GetCompatibleConfigurationsResponse&
        tptz__GetCompatibleConfigurationsResponse) {
  SOAP_EMPTY_HANDLER(tptz__GetCompatibleConfigurations, "PTZ");
}

int OnvifPTZBindingService::GeoMove(
    _tptz__GeoMove* tptz__GeoMove,
    _tptz__GeoMoveResponse& tptz__GeoMoveResponse) {
  SOAP_EMPTY_HANDLER(tptz__GeoMove, "PTZ");
}

int OnvifPTZBindingService::MoveAndStartTracking(
    _tptz__MoveAndStartTracking* tptz__MoveAndStartTracking,
    _tptz__MoveAndStartTrackingResponse& tptz__MoveAndStartTrackingResponse) {
  SOAP_EMPTY_HANDLER(tptz__MoveAndStartTracking, "PTZ");
}

}  // namespace ave
