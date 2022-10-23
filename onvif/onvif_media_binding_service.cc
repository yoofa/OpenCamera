/*
 * onvif_media_binding_service.cc
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "onvif_media_binding_service.h"

#include "ServiceContext.h"
#include "base/logging.h"
#include "onvif_server.h"
#include "smacros.h"
#include "stools.h"

namespace avp {

OnvifMediaBindingService::OnvifMediaBindingService(struct soap* soap)
    : MediaBindingService(soap) {}

OnvifMediaBindingService::~OnvifMediaBindingService() {}

int OnvifMediaBindingService::GetServiceCapabilities(
    _trt__GetServiceCapabilities* trt__GetServiceCapabilities,
    _trt__GetServiceCapabilitiesResponse& trt__GetServiceCapabilitiesResponse) {
  UNUSED(trt__GetServiceCapabilities);
  LOG(LS_INFO) << "Media: " << __FUNCTION__;

  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->mServicesInfo.get();
  trt__GetServiceCapabilitiesResponse.Capabilities =
      ctx->getMediaServiceCapabilities(this->soap);

  return SOAP_OK;
}

int OnvifMediaBindingService::GetVideoSources(
    _trt__GetVideoSources* trt__GetVideoSources,
    _trt__GetVideoSourcesResponse& trt__GetVideoSourcesResponse) {
  UNUSED(trt__GetVideoSources);
  LOG(LS_INFO) << "Media: " << __FUNCTION__;

  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->mServicesInfo.get();

  auto profiles = ctx->get_profiles();

  for (auto it = profiles.cbegin(); it != profiles.cend(); ++it) {
    trt__GetVideoSourcesResponse.VideoSources.push_back(
        it->second.get_video_src(this->soap));
    // Get the video source from the 1st profile
    break;
  }

  return SOAP_OK;
}

int OnvifMediaBindingService::GetAudioSources(
    _trt__GetAudioSources* trt__GetAudioSources,
    _trt__GetAudioSourcesResponse& trt__GetAudioSourcesResponse) {
  SOAP_EMPTY_HANDLER(trt__GetAudioSources, "Media");
}

int OnvifMediaBindingService::GetAudioOutputs(
    _trt__GetAudioOutputs* trt__GetAudioOutputs,
    _trt__GetAudioOutputsResponse& trt__GetAudioOutputsResponse) {
  SOAP_EMPTY_HANDLER(trt__GetAudioOutputs, "Media");
}

int OnvifMediaBindingService::CreateProfile(
    _trt__CreateProfile* trt__CreateProfile,
    _trt__CreateProfileResponse& trt__CreateProfileResponse) {
  SOAP_EMPTY_HANDLER(trt__CreateProfile, "Media");
}

int OnvifMediaBindingService::GetProfile(
    _trt__GetProfile* trt__GetProfile,
    _trt__GetProfileResponse& trt__GetProfileResponse) {
  LOG(LS_INFO) << "Media:" << __FUNCTION__
               << "   get profile:" << trt__GetProfile->ProfileToken.c_str();

  int ret = SOAP_FAULT;

  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->mServicesInfo.get();
  auto profiles = ctx->get_profiles();
  auto it = profiles.find(trt__GetProfile->ProfileToken);

  if (it != profiles.end()) {
    trt__GetProfileResponse.Profile = it->second.get_profile(this->soap);
    ret = SOAP_OK;
  }

  return ret;
}

int OnvifMediaBindingService::GetProfiles(
    _trt__GetProfiles* trt__GetProfiles,
    _trt__GetProfilesResponse& trt__GetProfilesResponse) {
  UNUSED(trt__GetProfiles);
  LOG(LS_INFO) << "Media: " << __FUNCTION__;

  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->mServicesInfo.get();

  auto profiles = ctx->get_profiles();

  for (auto it = profiles.cbegin(); it != profiles.cend(); ++it) {
    trt__GetProfilesResponse.Profiles.push_back(
        it->second.get_profile(this->soap));
  }

  return SOAP_OK;
}

int OnvifMediaBindingService::AddVideoEncoderConfiguration(
    _trt__AddVideoEncoderConfiguration* trt__AddVideoEncoderConfiguration,
    _trt__AddVideoEncoderConfigurationResponse&
        trt__AddVideoEncoderConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__AddVideoEncoderConfiguration, "Media");
}

int OnvifMediaBindingService::AddVideoSourceConfiguration(
    _trt__AddVideoSourceConfiguration* trt__AddVideoSourceConfiguration,
    _trt__AddVideoSourceConfigurationResponse&
        trt__AddVideoSourceConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__AddVideoSourceConfiguration, "Media");
}

int OnvifMediaBindingService::AddAudioEncoderConfiguration(
    _trt__AddAudioEncoderConfiguration* trt__AddAudioEncoderConfiguration,
    _trt__AddAudioEncoderConfigurationResponse&
        trt__AddAudioEncoderConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__AddAudioEncoderConfiguration, "Media");
}

int OnvifMediaBindingService::AddAudioSourceConfiguration(
    _trt__AddAudioSourceConfiguration* trt__AddAudioSourceConfiguration,
    _trt__AddAudioSourceConfigurationResponse&
        trt__AddAudioSourceConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__AddAudioSourceConfiguration, "Media");
}

int OnvifMediaBindingService::AddPTZConfiguration(
    _trt__AddPTZConfiguration* trt__AddPTZConfiguration,
    _trt__AddPTZConfigurationResponse& trt__AddPTZConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__AddPTZConfiguration, "Media");
}

int OnvifMediaBindingService::AddVideoAnalyticsConfiguration(
    _trt__AddVideoAnalyticsConfiguration* trt__AddVideoAnalyticsConfiguration,
    _trt__AddVideoAnalyticsConfigurationResponse&
        trt__AddVideoAnalyticsConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__AddVideoAnalyticsConfiguration, "Media");
}

int OnvifMediaBindingService::AddMetadataConfiguration(
    _trt__AddMetadataConfiguration* trt__AddMetadataConfiguration,
    _trt__AddMetadataConfigurationResponse&
        trt__AddMetadataConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__AddMetadataConfiguration, "Media");
}

int OnvifMediaBindingService::AddAudioOutputConfiguration(
    _trt__AddAudioOutputConfiguration* trt__AddAudioOutputConfiguration,
    _trt__AddAudioOutputConfigurationResponse&
        trt__AddAudioOutputConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__AddAudioOutputConfiguration, "Media");
}

int OnvifMediaBindingService::AddAudioDecoderConfiguration(
    _trt__AddAudioDecoderConfiguration* trt__AddAudioDecoderConfiguration,
    _trt__AddAudioDecoderConfigurationResponse&
        trt__AddAudioDecoderConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__AddAudioDecoderConfiguration, "Media");
}

int OnvifMediaBindingService::RemoveVideoEncoderConfiguration(
    _trt__RemoveVideoEncoderConfiguration* trt__RemoveVideoEncoderConfiguration,
    _trt__RemoveVideoEncoderConfigurationResponse&
        trt__RemoveVideoEncoderConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__RemoveVideoEncoderConfiguration, "Media");
}

int OnvifMediaBindingService::RemoveVideoSourceConfiguration(
    _trt__RemoveVideoSourceConfiguration* trt__RemoveVideoSourceConfiguration,
    _trt__RemoveVideoSourceConfigurationResponse&
        trt__RemoveVideoSourceConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__RemoveVideoSourceConfiguration, "Media");
}

int OnvifMediaBindingService::RemoveAudioEncoderConfiguration(
    _trt__RemoveAudioEncoderConfiguration* trt__RemoveAudioEncoderConfiguration,
    _trt__RemoveAudioEncoderConfigurationResponse&
        trt__RemoveAudioEncoderConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__RemoveAudioEncoderConfiguration, "Media");
}

int OnvifMediaBindingService::RemoveAudioSourceConfiguration(
    _trt__RemoveAudioSourceConfiguration* trt__RemoveAudioSourceConfiguration,
    _trt__RemoveAudioSourceConfigurationResponse&
        trt__RemoveAudioSourceConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__RemoveAudioSourceConfiguration, "Media");
}

int OnvifMediaBindingService::RemovePTZConfiguration(
    _trt__RemovePTZConfiguration* trt__RemovePTZConfiguration,
    _trt__RemovePTZConfigurationResponse& trt__RemovePTZConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__RemovePTZConfiguration, "Media");
}

int OnvifMediaBindingService::RemoveVideoAnalyticsConfiguration(
    _trt__RemoveVideoAnalyticsConfiguration*
        trt__RemoveVideoAnalyticsConfiguration,
    _trt__RemoveVideoAnalyticsConfigurationResponse&
        trt__RemoveVideoAnalyticsConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__RemoveVideoAnalyticsConfiguration, "Media");
}

int OnvifMediaBindingService::RemoveMetadataConfiguration(
    _trt__RemoveMetadataConfiguration* trt__RemoveMetadataConfiguration,
    _trt__RemoveMetadataConfigurationResponse&
        trt__RemoveMetadataConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__RemoveMetadataConfiguration, "Media");
}

int OnvifMediaBindingService::RemoveAudioOutputConfiguration(
    _trt__RemoveAudioOutputConfiguration* trt__RemoveAudioOutputConfiguration,
    _trt__RemoveAudioOutputConfigurationResponse&
        trt__RemoveAudioOutputConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__RemoveAudioOutputConfiguration, "Media");
}

int OnvifMediaBindingService::RemoveAudioDecoderConfiguration(
    _trt__RemoveAudioDecoderConfiguration* trt__RemoveAudioDecoderConfiguration,
    _trt__RemoveAudioDecoderConfigurationResponse&
        trt__RemoveAudioDecoderConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__RemoveAudioDecoderConfiguration, "Media");
}

int OnvifMediaBindingService::DeleteProfile(
    _trt__DeleteProfile* trt__DeleteProfile,
    _trt__DeleteProfileResponse& trt__DeleteProfileResponse) {
  SOAP_EMPTY_HANDLER(trt__DeleteProfile, "Media");
}

int OnvifMediaBindingService::GetVideoSourceConfigurations(
    _trt__GetVideoSourceConfigurations* trt__GetVideoSourceConfigurations,
    _trt__GetVideoSourceConfigurationsResponse&
        trt__GetVideoSourceConfigurationsResponse) {
  UNUSED(trt__GetVideoSourceConfigurations);
  LOG(LS_INFO) << "Media: " << __FUNCTION__;

  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->mServicesInfo.get();

  auto profiles = ctx->get_profiles();

  for (auto it = profiles.cbegin(); it != profiles.cend(); ++it) {
    tt__VideoSourceConfiguration* vsc =
        it->second.get_video_src_cnf(this->soap);
    trt__GetVideoSourceConfigurationsResponse.Configurations.push_back(vsc);
  }

  return SOAP_OK;
}

int OnvifMediaBindingService::GetVideoEncoderConfigurations(
    _trt__GetVideoEncoderConfigurations* trt__GetVideoEncoderConfigurations,
    _trt__GetVideoEncoderConfigurationsResponse&
        trt__GetVideoEncoderConfigurationsResponse) {
  UNUSED(trt__GetVideoEncoderConfigurations);
  LOG(LS_INFO) << "Media: " << __FUNCTION__;

  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->mServicesInfo.get();

  auto profiles = ctx->get_profiles();

  for (auto it = profiles.cbegin(); it != profiles.cend(); ++it) {
    tt__VideoEncoderConfiguration* vec =
        it->second.get_video_enc_cfg(this->soap);
    trt__GetVideoEncoderConfigurationsResponse.Configurations.push_back(vec);
  }

  return SOAP_OK;
}

int OnvifMediaBindingService::GetAudioSourceConfigurations(
    _trt__GetAudioSourceConfigurations* trt__GetAudioSourceConfigurations,
    _trt__GetAudioSourceConfigurationsResponse&
        trt__GetAudioSourceConfigurationsResponse) {
  SOAP_EMPTY_HANDLER(trt__GetAudioSourceConfigurations, "Media");
}

int OnvifMediaBindingService::GetAudioEncoderConfigurations(
    _trt__GetAudioEncoderConfigurations* trt__GetAudioEncoderConfigurations,
    _trt__GetAudioEncoderConfigurationsResponse&
        trt__GetAudioEncoderConfigurationsResponse) {
  SOAP_EMPTY_HANDLER(trt__GetAudioEncoderConfigurations, "Media");
}

int OnvifMediaBindingService::GetVideoAnalyticsConfigurations(
    _trt__GetVideoAnalyticsConfigurations* trt__GetVideoAnalyticsConfigurations,
    _trt__GetVideoAnalyticsConfigurationsResponse&
        trt__GetVideoAnalyticsConfigurationsResponse) {
  SOAP_EMPTY_HANDLER(trt__GetVideoAnalyticsConfigurations, "Media");
}

int OnvifMediaBindingService::GetMetadataConfigurations(
    _trt__GetMetadataConfigurations* trt__GetMetadataConfigurations,
    _trt__GetMetadataConfigurationsResponse&
        trt__GetMetadataConfigurationsResponse) {
  SOAP_EMPTY_HANDLER(trt__GetMetadataConfigurations, "Media");
}

int OnvifMediaBindingService::GetAudioOutputConfigurations(
    _trt__GetAudioOutputConfigurations* trt__GetAudioOutputConfigurations,
    _trt__GetAudioOutputConfigurationsResponse&
        trt__GetAudioOutputConfigurationsResponse) {
  SOAP_EMPTY_HANDLER(trt__GetAudioOutputConfigurations, "Media");
}

int OnvifMediaBindingService::GetAudioDecoderConfigurations(
    _trt__GetAudioDecoderConfigurations* trt__GetAudioDecoderConfigurations,
    _trt__GetAudioDecoderConfigurationsResponse&
        trt__GetAudioDecoderConfigurationsResponse) {
  SOAP_EMPTY_HANDLER(trt__GetAudioDecoderConfigurations, "Media");
}

int OnvifMediaBindingService::GetVideoSourceConfiguration(
    _trt__GetVideoSourceConfiguration* trt__GetVideoSourceConfiguration,
    _trt__GetVideoSourceConfigurationResponse&
        trt__GetVideoSourceConfigurationResponse) {
  LOG(LS_INFO) << "Media: " << __FUNCTION__;

  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->mServicesInfo.get();

  auto profiles = ctx->get_profiles();

  for (auto it = profiles.cbegin(); it != profiles.cend(); ++it) {
    if (trt__GetVideoSourceConfiguration->ConfigurationToken ==
        it->second.get_video_src_cnf(this->soap)->token) {
      tt__VideoSourceConfiguration* vsc =
          it->second.get_video_src_cnf(this->soap);
      trt__GetVideoSourceConfigurationResponse.Configuration = vsc;
      break;
    }
  }

  return SOAP_OK;
}

int OnvifMediaBindingService::GetVideoEncoderConfiguration(
    _trt__GetVideoEncoderConfiguration* trt__GetVideoEncoderConfiguration,
    _trt__GetVideoEncoderConfigurationResponse&
        trt__GetVideoEncoderConfigurationResponse) {
  LOG(LS_INFO) << "Media: " << __FUNCTION__;

  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->mServicesInfo.get();

  auto profiles = ctx->get_profiles();

  for (auto it = profiles.cbegin(); it != profiles.cend(); ++it) {
    if (trt__GetVideoEncoderConfiguration->ConfigurationToken ==
        it->second.get_video_enc_cfg(this->soap)->token) {
      tt__VideoEncoderConfiguration* vec =
          it->second.get_video_enc_cfg(this->soap);
      trt__GetVideoEncoderConfigurationResponse.Configuration = vec;
      break;
    }
  }

  return SOAP_OK;
}

int OnvifMediaBindingService::GetAudioSourceConfiguration(
    _trt__GetAudioSourceConfiguration* trt__GetAudioSourceConfiguration,
    _trt__GetAudioSourceConfigurationResponse&
        trt__GetAudioSourceConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__GetAudioSourceConfiguration, "Media");
}

int OnvifMediaBindingService::GetAudioEncoderConfiguration(
    _trt__GetAudioEncoderConfiguration* trt__GetAudioEncoderConfiguration,
    _trt__GetAudioEncoderConfigurationResponse&
        trt__GetAudioEncoderConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__GetAudioEncoderConfiguration, "Media");
}

int OnvifMediaBindingService::GetVideoAnalyticsConfiguration(
    _trt__GetVideoAnalyticsConfiguration* trt__GetVideoAnalyticsConfiguration,
    _trt__GetVideoAnalyticsConfigurationResponse&
        trt__GetVideoAnalyticsConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__GetVideoAnalyticsConfiguration, "Media");
}

int OnvifMediaBindingService::GetMetadataConfiguration(
    _trt__GetMetadataConfiguration* trt__GetMetadataConfiguration,
    _trt__GetMetadataConfigurationResponse&
        trt__GetMetadataConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__GetMetadataConfiguration, "Media");
}

int OnvifMediaBindingService::GetAudioOutputConfiguration(
    _trt__GetAudioOutputConfiguration* trt__GetAudioOutputConfiguration,
    _trt__GetAudioOutputConfigurationResponse&
        trt__GetAudioOutputConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__GetAudioOutputConfiguration, "Media");
}

int OnvifMediaBindingService::GetAudioDecoderConfiguration(
    _trt__GetAudioDecoderConfiguration* trt__GetAudioDecoderConfiguration,
    _trt__GetAudioDecoderConfigurationResponse&
        trt__GetAudioDecoderConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__GetAudioDecoderConfiguration, "Media");
}

int OnvifMediaBindingService::GetCompatibleVideoEncoderConfigurations(
    _trt__GetCompatibleVideoEncoderConfigurations*
        trt__GetCompatibleVideoEncoderConfigurations,
    _trt__GetCompatibleVideoEncoderConfigurationsResponse&
        trt__GetCompatibleVideoEncoderConfigurationsResponse) {
  LOG(LS_INFO) << "Media: " << __FUNCTION__;

  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->mServicesInfo.get();

  auto profiles = ctx->get_profiles();

  for (auto it = profiles.cbegin(); it != profiles.cend(); ++it) {
    if (trt__GetCompatibleVideoEncoderConfigurations->ProfileToken ==
        it->second.get_profile(this->soap)->token) {
      tt__VideoEncoderConfiguration* vec =
          it->second.get_video_enc_cfg(this->soap);
      trt__GetCompatibleVideoEncoderConfigurationsResponse.Configurations
          .push_back(vec);
      break;
    }
  }

  return SOAP_OK;
}

int OnvifMediaBindingService::GetCompatibleVideoSourceConfigurations(
    _trt__GetCompatibleVideoSourceConfigurations*
        trt__GetCompatibleVideoSourceConfigurations,
    _trt__GetCompatibleVideoSourceConfigurationsResponse&
        trt__GetCompatibleVideoSourceConfigurationsResponse) {
  LOG(LS_INFO) << "Media: " << __FUNCTION__;

  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->mServicesInfo.get();

  auto profiles = ctx->get_profiles();

  for (auto it = profiles.cbegin(); it != profiles.cend(); ++it) {
    if (trt__GetCompatibleVideoSourceConfigurations->ProfileToken ==
        it->second.get_profile(this->soap)->token) {
      tt__VideoSourceConfiguration* vsc =
          it->second.get_video_src_cnf(this->soap);
      trt__GetCompatibleVideoSourceConfigurationsResponse.Configurations
          .push_back(vsc);
      break;
    }
  }

  return SOAP_OK;
}

int OnvifMediaBindingService::GetCompatibleAudioEncoderConfigurations(
    _trt__GetCompatibleAudioEncoderConfigurations*
        trt__GetCompatibleAudioEncoderConfigurations,
    _trt__GetCompatibleAudioEncoderConfigurationsResponse&
        trt__GetCompatibleAudioEncoderConfigurationsResponse) {
  SOAP_EMPTY_HANDLER(trt__GetCompatibleAudioEncoderConfigurations, "Media");
}

int OnvifMediaBindingService::GetCompatibleAudioSourceConfigurations(
    _trt__GetCompatibleAudioSourceConfigurations*
        trt__GetCompatibleAudioSourceConfigurations,
    _trt__GetCompatibleAudioSourceConfigurationsResponse&
        trt__GetCompatibleAudioSourceConfigurationsResponse) {
  SOAP_EMPTY_HANDLER(trt__GetCompatibleAudioSourceConfigurations, "Media");
}

int OnvifMediaBindingService::GetCompatibleVideoAnalyticsConfigurations(
    _trt__GetCompatibleVideoAnalyticsConfigurations*
        trt__GetCompatibleVideoAnalyticsConfigurations,
    _trt__GetCompatibleVideoAnalyticsConfigurationsResponse&
        trt__GetCompatibleVideoAnalyticsConfigurationsResponse) {
  SOAP_EMPTY_HANDLER(trt__GetCompatibleVideoAnalyticsConfigurations, "Media");
}

int OnvifMediaBindingService::GetCompatibleMetadataConfigurations(
    _trt__GetCompatibleMetadataConfigurations*
        trt__GetCompatibleMetadataConfigurations,
    _trt__GetCompatibleMetadataConfigurationsResponse&
        trt__GetCompatibleMetadataConfigurationsResponse) {
  SOAP_EMPTY_HANDLER(trt__GetCompatibleMetadataConfigurations, "Media");
}

int OnvifMediaBindingService::GetCompatibleAudioOutputConfigurations(
    _trt__GetCompatibleAudioOutputConfigurations*
        trt__GetCompatibleAudioOutputConfigurations,
    _trt__GetCompatibleAudioOutputConfigurationsResponse&
        trt__GetCompatibleAudioOutputConfigurationsResponse) {
  SOAP_EMPTY_HANDLER(trt__GetCompatibleAudioOutputConfigurations, "Media");
}

int OnvifMediaBindingService::GetCompatibleAudioDecoderConfigurations(
    _trt__GetCompatibleAudioDecoderConfigurations*
        trt__GetCompatibleAudioDecoderConfigurations,
    _trt__GetCompatibleAudioDecoderConfigurationsResponse&
        trt__GetCompatibleAudioDecoderConfigurationsResponse) {
  SOAP_EMPTY_HANDLER(trt__GetCompatibleAudioDecoderConfigurations, "Media");
}

int OnvifMediaBindingService::SetVideoSourceConfiguration(
    _trt__SetVideoSourceConfiguration* trt__SetVideoSourceConfiguration,
    _trt__SetVideoSourceConfigurationResponse&
        trt__SetVideoSourceConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__SetVideoSourceConfiguration, "Media");
}

int OnvifMediaBindingService::SetVideoEncoderConfiguration(
    _trt__SetVideoEncoderConfiguration* trt__SetVideoEncoderConfiguration,
    _trt__SetVideoEncoderConfigurationResponse&
        trt__SetVideoEncoderConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__SetVideoEncoderConfiguration, "Media");
}

int OnvifMediaBindingService::SetAudioSourceConfiguration(
    _trt__SetAudioSourceConfiguration* trt__SetAudioSourceConfiguration,
    _trt__SetAudioSourceConfigurationResponse&
        trt__SetAudioSourceConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__SetAudioSourceConfiguration, "Media");
}

int OnvifMediaBindingService::SetAudioEncoderConfiguration(
    _trt__SetAudioEncoderConfiguration* trt__SetAudioEncoderConfiguration,
    _trt__SetAudioEncoderConfigurationResponse&
        trt__SetAudioEncoderConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__SetAudioEncoderConfiguration, "Media");
}

int OnvifMediaBindingService::SetVideoAnalyticsConfiguration(
    _trt__SetVideoAnalyticsConfiguration* trt__SetVideoAnalyticsConfiguration,
    _trt__SetVideoAnalyticsConfigurationResponse&
        trt__SetVideoAnalyticsConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__SetVideoAnalyticsConfiguration, "Media");
}

int OnvifMediaBindingService::SetMetadataConfiguration(
    _trt__SetMetadataConfiguration* trt__SetMetadataConfiguration,
    _trt__SetMetadataConfigurationResponse&
        trt__SetMetadataConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__SetMetadataConfiguration, "Media");
}

int OnvifMediaBindingService::SetAudioOutputConfiguration(
    _trt__SetAudioOutputConfiguration* trt__SetAudioOutputConfiguration,
    _trt__SetAudioOutputConfigurationResponse&
        trt__SetAudioOutputConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__SetAudioOutputConfiguration, "Media");
}

int OnvifMediaBindingService::SetAudioDecoderConfiguration(
    _trt__SetAudioDecoderConfiguration* trt__SetAudioDecoderConfiguration,
    _trt__SetAudioDecoderConfigurationResponse&
        trt__SetAudioDecoderConfigurationResponse) {
  SOAP_EMPTY_HANDLER(trt__SetAudioDecoderConfiguration, "Media");
}

int OnvifMediaBindingService::GetVideoSourceConfigurationOptions(
    _trt__GetVideoSourceConfigurationOptions*
        trt__GetVideoSourceConfigurationOptions,
    _trt__GetVideoSourceConfigurationOptionsResponse&
        trt__GetVideoSourceConfigurationOptionsResponse) {
  LOG(LS_INFO) << "Media: " << __FUNCTION__;

  std::string token;
  int width;
  int height;
  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->mServicesInfo.get();

  auto profiles = ctx->get_profiles();

  if (trt__GetVideoSourceConfigurationOptions->ConfigurationToken != NULL) {
    for (auto it = profiles.cbegin(); it != profiles.cend(); ++it) {
      if (*(trt__GetVideoSourceConfigurationOptions->ConfigurationToken) ==
          it->second.get_video_enc_cfg(this->soap)->token) {
        token.assign(it->second.get_video_enc_cfg(this->soap)->token);
      }
    }
  } else if (trt__GetVideoSourceConfigurationOptions->ProfileToken != NULL) {
    for (auto it = profiles.cbegin(); it != profiles.cend(); ++it) {
      if (*(trt__GetVideoSourceConfigurationOptions->ProfileToken) ==
          it->second.get_video_src_cnf(this->soap)->token) {
        token.assign(it->second.get_video_src_cnf(this->soap)->token);
      }
    }
  }

  for (auto it = profiles.cbegin(); it != profiles.cend(); ++it) {
    if ((token.empty()) ||
        (token == it->second.get_video_src_cnf(this->soap)->token)) {
      width = it->second.get_width();
      height = it->second.get_height();

      trt__GetVideoSourceConfigurationOptionsResponse.Options =
          soap_new_tt__VideoSourceConfigurationOptions(soap);

      trt__GetVideoSourceConfigurationOptionsResponse.Options->BoundsRange =
          soap_new_tt__IntRectangleRange(soap);
      trt__GetVideoSourceConfigurationOptionsResponse.Options->BoundsRange
          ->XRange = soap_new_tt__IntRange(soap);
      trt__GetVideoSourceConfigurationOptionsResponse.Options->BoundsRange
          ->XRange->Min = 0;  // dummy
      trt__GetVideoSourceConfigurationOptionsResponse.Options->BoundsRange
          ->XRange->Max = width;  // dummy
      trt__GetVideoSourceConfigurationOptionsResponse.Options->BoundsRange
          ->YRange = soap_new_tt__IntRange(soap);
      trt__GetVideoSourceConfigurationOptionsResponse.Options->BoundsRange
          ->YRange->Min = 0;  // dummy
      trt__GetVideoSourceConfigurationOptionsResponse.Options->BoundsRange
          ->YRange->Max = height;  // dummy
      trt__GetVideoSourceConfigurationOptionsResponse.Options->BoundsRange
          ->WidthRange = soap_new_tt__IntRange(soap);
      trt__GetVideoSourceConfigurationOptionsResponse.Options->BoundsRange
          ->WidthRange->Min = 0;  // dummy
      trt__GetVideoSourceConfigurationOptionsResponse.Options->BoundsRange
          ->WidthRange->Max = width;  // dummy
      trt__GetVideoSourceConfigurationOptionsResponse.Options->BoundsRange
          ->HeightRange = soap_new_tt__IntRange(soap);
      trt__GetVideoSourceConfigurationOptionsResponse.Options->BoundsRange
          ->HeightRange->Min = 0;  // dummy
      trt__GetVideoSourceConfigurationOptionsResponse.Options->BoundsRange
          ->HeightRange->Max = height;  // dummy

      trt__GetVideoSourceConfigurationOptionsResponse.Options
          ->VideoSourceTokensAvailable.push_back(
              it->second.get_video_src_cnf(this->soap)->token);

      break;
    }
  }

  return SOAP_OK;
}

int OnvifMediaBindingService::GetVideoEncoderConfigurationOptions(
    _trt__GetVideoEncoderConfigurationOptions*
        trt__GetVideoEncoderConfigurationOptions,
    _trt__GetVideoEncoderConfigurationOptionsResponse&
        trt__GetVideoEncoderConfigurationOptionsResponse) {
  LOG(LS_INFO) << "Media: " << __FUNCTION__;

  std::string token;
  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->mServicesInfo.get();

  auto profiles = ctx->get_profiles();

  if (trt__GetVideoEncoderConfigurationOptions->ConfigurationToken != NULL) {
    for (auto it = profiles.cbegin(); it != profiles.cend(); ++it) {
      if (*(trt__GetVideoEncoderConfigurationOptions->ConfigurationToken) ==
          it->second.get_video_enc_cfg(this->soap)->token) {
        token.assign(it->second.get_video_enc_cfg(this->soap)->token);
      }
    }
  } else if (trt__GetVideoEncoderConfigurationOptions->ProfileToken != NULL) {
    for (auto it = profiles.cbegin(); it != profiles.cend(); ++it) {
      if (*(trt__GetVideoEncoderConfigurationOptions->ProfileToken) ==
          it->second.get_video_src_cnf(this->soap)->token) {
        token.assign(it->second.get_video_src_cnf(this->soap)->token);
      }
    }
  }

  trt__GetVideoEncoderConfigurationOptionsResponse.Options =
      soap_new_tt__VideoEncoderConfigurationOptions(soap);
  trt__GetVideoEncoderConfigurationOptionsResponse.Options->JPEG = NULL;
  trt__GetVideoEncoderConfigurationOptionsResponse.Options->MPEG4 = NULL;
  trt__GetVideoEncoderConfigurationOptionsResponse.Options->H264 = NULL;
  trt__GetVideoEncoderConfigurationOptionsResponse.Options->QualityRange =
      soap_new_tt__IntRange(soap);
  trt__GetVideoEncoderConfigurationOptionsResponse.Options->QualityRange->Min =
      0;  // dummy
  trt__GetVideoEncoderConfigurationOptionsResponse.Options->QualityRange->Max =
      100;  // dummy
  trt__GetVideoEncoderConfigurationOptionsResponse.Options->Extension = NULL;

  for (auto it = profiles.cbegin(); it != profiles.cend(); ++it) {
    if ((token.empty()) ||
        (token == it->second.get_video_src_cnf(this->soap)->token)) {
      if (it->second.get_type() == tt__VideoEncoding__JPEG) {
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->JPEG =
            soap_new_tt__JpegOptions(soap);

        tt__VideoResolution* vr = soap_new_tt__VideoResolution(soap);
        vr->Width = it->second.get_width();
        vr->Height = it->second.get_height();
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->JPEG
            ->ResolutionsAvailable.push_back(vr);

        trt__GetVideoEncoderConfigurationOptionsResponse.Options->JPEG
            ->FrameRateRange = soap_new_tt__IntRange(soap);
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->JPEG
            ->FrameRateRange->Min = 0;  // dummy
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->JPEG
            ->FrameRateRange->Max = 20;  // dummy
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->JPEG
            ->EncodingIntervalRange = soap_new_tt__IntRange(soap);
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->JPEG
            ->EncodingIntervalRange->Min = 0;  // dummy
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->JPEG
            ->EncodingIntervalRange->Max = 3;  // dummy
      } else if (it->second.get_type() == tt__VideoEncoding__MPEG4) {
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->MPEG4 =
            soap_new_tt__Mpeg4Options(soap);

        tt__VideoResolution* vr = soap_new_tt__VideoResolution(soap);
        vr->Width = it->second.get_width();
        vr->Height = it->second.get_height();
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->MPEG4
            ->ResolutionsAvailable.push_back(vr);

        trt__GetVideoEncoderConfigurationOptionsResponse.Options->MPEG4
            ->GovLengthRange = soap_new_tt__IntRange(soap);
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->MPEG4
            ->GovLengthRange->Min = 0;  // dummy
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->MPEG4
            ->GovLengthRange->Max = 40;  // dummy
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->MPEG4
            ->FrameRateRange = soap_new_tt__IntRange(soap);
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->MPEG4
            ->FrameRateRange->Min = 0;  // dummy
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->MPEG4
            ->FrameRateRange->Max = 20;  // dummy
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->MPEG4
            ->EncodingIntervalRange = soap_new_tt__IntRange(soap);
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->MPEG4
            ->EncodingIntervalRange->Min = 0;  // dummy
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->MPEG4
            ->EncodingIntervalRange->Max = 3;  // dummy
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->MPEG4
            ->Mpeg4ProfilesSupported.push_back(tt__Mpeg4Profile__SP);
      } else if (it->second.get_type() == tt__VideoEncoding__H264) {
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->H264 =
            soap_new_tt__H264Options(soap);

        tt__VideoResolution* vr = soap_new_tt__VideoResolution(soap);
        vr->Width = it->second.get_width();
        vr->Height = it->second.get_height();
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->H264
            ->ResolutionsAvailable.push_back(vr);

        trt__GetVideoEncoderConfigurationOptionsResponse.Options->H264
            ->GovLengthRange = soap_new_tt__IntRange(soap);
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->H264
            ->GovLengthRange->Min = 0;  // dummy
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->H264
            ->GovLengthRange->Max = 40;  // dummy
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->H264
            ->FrameRateRange = soap_new_tt__IntRange(soap);
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->H264
            ->FrameRateRange->Min = 0;  // dummy
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->H264
            ->FrameRateRange->Max = 20;  // dummy
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->H264
            ->EncodingIntervalRange = soap_new_tt__IntRange(soap);
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->H264
            ->EncodingIntervalRange->Min = 0;  // dummy
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->H264
            ->EncodingIntervalRange->Max = 3;  // dummy
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->H264
            ->H264ProfilesSupported.push_back(tt__H264Profile__Main);
        trt__GetVideoEncoderConfigurationOptionsResponse.Options->H264
            ->H264ProfilesSupported.push_back(tt__H264Profile__Main);
      }
    }
  }

  return SOAP_OK;
}

int OnvifMediaBindingService::GetAudioSourceConfigurationOptions(
    _trt__GetAudioSourceConfigurationOptions*
        trt__GetAudioSourceConfigurationOptions,
    _trt__GetAudioSourceConfigurationOptionsResponse&
        trt__GetAudioSourceConfigurationOptionsResponse) {
  SOAP_EMPTY_HANDLER(trt__GetAudioSourceConfigurationOptions, "Media");
}

int OnvifMediaBindingService::GetAudioEncoderConfigurationOptions(
    _trt__GetAudioEncoderConfigurationOptions*
        trt__GetAudioEncoderConfigurationOptions,
    _trt__GetAudioEncoderConfigurationOptionsResponse&
        trt__GetAudioEncoderConfigurationOptionsResponse) {
  SOAP_EMPTY_HANDLER(trt__GetAudioEncoderConfigurationOptions, "Media");
}

int OnvifMediaBindingService::GetMetadataConfigurationOptions(
    _trt__GetMetadataConfigurationOptions* trt__GetMetadataConfigurationOptions,
    _trt__GetMetadataConfigurationOptionsResponse&
        trt__GetMetadataConfigurationOptionsResponse) {
  SOAP_EMPTY_HANDLER(trt__GetMetadataConfigurationOptions, "Media");
}

int OnvifMediaBindingService::GetAudioOutputConfigurationOptions(
    _trt__GetAudioOutputConfigurationOptions*
        trt__GetAudioOutputConfigurationOptions,
    _trt__GetAudioOutputConfigurationOptionsResponse&
        trt__GetAudioOutputConfigurationOptionsResponse) {
  SOAP_EMPTY_HANDLER(trt__GetAudioOutputConfigurationOptions, "Media");
}

int OnvifMediaBindingService::GetAudioDecoderConfigurationOptions(
    _trt__GetAudioDecoderConfigurationOptions*
        trt__GetAudioDecoderConfigurationOptions,
    _trt__GetAudioDecoderConfigurationOptionsResponse&
        trt__GetAudioDecoderConfigurationOptionsResponse) {
  SOAP_EMPTY_HANDLER(trt__GetAudioDecoderConfigurationOptions, "Media");
}

int OnvifMediaBindingService::GetGuaranteedNumberOfVideoEncoderInstances(
    _trt__GetGuaranteedNumberOfVideoEncoderInstances*
        trt__GetGuaranteedNumberOfVideoEncoderInstances,
    _trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse&
        trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse) {
  UNUSED(trt__GetGuaranteedNumberOfVideoEncoderInstances);
  LOG(LS_INFO) << "Media: " << __FUNCTION__;

  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->mServicesInfo.get();

  auto profiles = ctx->get_profiles();

  int instancesNumber = 3;
  int jpegInstancesNumber = 0;
  int mpeg4InstancesNumber = 0;
  int h264InstancesNumber = 0;

  for (auto it = profiles.cbegin(); it != profiles.cend(); ++it) {
    if (it->second.get_type() == tt__VideoEncoding__JPEG) {
      jpegInstancesNumber += instancesNumber;
    } else if (it->second.get_type() == tt__VideoEncoding__MPEG4) {
      mpeg4InstancesNumber += instancesNumber;
    } else if (it->second.get_type() == tt__VideoEncoding__H264) {
      h264InstancesNumber += instancesNumber;
    }
  }

  trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse.TotalNumber =
      jpegInstancesNumber + mpeg4InstancesNumber + h264InstancesNumber;
  trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse.JPEG =
      soap_new_ptr(soap, jpegInstancesNumber);
  trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse.MPEG4 =
      soap_new_ptr(soap, mpeg4InstancesNumber);
  trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse.H264 =
      soap_new_ptr(soap, h264InstancesNumber);

  return SOAP_OK;
}

int OnvifMediaBindingService::GetStreamUri(
    _trt__GetStreamUri* trt__GetStreamUri,
    _trt__GetStreamUriResponse& trt__GetStreamUriResponse) {
  LOG(LS_INFO) << "Media: " << __FUNCTION__
               << "   for profile:" << trt__GetStreamUri->ProfileToken.c_str();

  int ret = SOAP_FAULT;

  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->mServicesInfo.get();
  auto profiles = ctx->get_profiles();
  auto it = profiles.find(trt__GetStreamUri->ProfileToken);

  if (it != profiles.end()) {
    trt__GetStreamUriResponse.MediaUri = soap_new_tt__MediaUri(this->soap);
    trt__GetStreamUriResponse.MediaUri->Uri =
        ctx->get_stream_uri(it->second.get_url(), htonl(this->soap->ip));
    ret = SOAP_OK;
  }

  return ret;
}

int OnvifMediaBindingService::StartMulticastStreaming(
    _trt__StartMulticastStreaming* trt__StartMulticastStreaming,
    _trt__StartMulticastStreamingResponse&
        trt__StartMulticastStreamingResponse) {
  SOAP_EMPTY_HANDLER(trt__StartMulticastStreaming, "Media");
}

int OnvifMediaBindingService::StopMulticastStreaming(
    _trt__StopMulticastStreaming* trt__StopMulticastStreaming,
    _trt__StopMulticastStreamingResponse& trt__StopMulticastStreamingResponse) {
  SOAP_EMPTY_HANDLER(trt__StopMulticastStreaming, "Media");
}

int OnvifMediaBindingService::SetSynchronizationPoint(
    _trt__SetSynchronizationPoint* trt__SetSynchronizationPoint,
    _trt__SetSynchronizationPointResponse&
        trt__SetSynchronizationPointResponse) {
  SOAP_EMPTY_HANDLER(trt__SetSynchronizationPoint, "Media");
}

int OnvifMediaBindingService::GetSnapshotUri(
    _trt__GetSnapshotUri* trt__GetSnapshotUri,
    _trt__GetSnapshotUriResponse& trt__GetSnapshotUriResponse) {
  LOG(LS_INFO) << "Media: " << __FUNCTION__ << "   for profile:"
               << trt__GetSnapshotUri->ProfileToken.c_str();

  int ret = SOAP_FAULT;

  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->mServicesInfo.get();
  auto profiles = ctx->get_profiles();
  auto it = profiles.find(trt__GetSnapshotUri->ProfileToken);

  if (it != profiles.end()) {
    trt__GetSnapshotUriResponse.MediaUri = soap_new_tt__MediaUri(this->soap);
    trt__GetSnapshotUriResponse.MediaUri->Uri =
        ctx->get_snapshot_uri(it->second.get_snapurl(), htonl(this->soap->ip));
    ret = SOAP_OK;
  }

  return ret;
}

int OnvifMediaBindingService::GetVideoSourceModes(
    _trt__GetVideoSourceModes* trt__GetVideoSourceModes,
    _trt__GetVideoSourceModesResponse& trt__GetVideoSourceModesResponse) {
  SOAP_EMPTY_HANDLER(trt__GetVideoSourceModes, "Media");
}

int OnvifMediaBindingService::SetVideoSourceMode(
    _trt__SetVideoSourceMode* trt__SetVideoSourceMode,
    _trt__SetVideoSourceModeResponse& trt__SetVideoSourceModeResponse) {
  SOAP_EMPTY_HANDLER(trt__SetVideoSourceMode, "Media");
}

int OnvifMediaBindingService::GetOSDs(
    _trt__GetOSDs* trt__GetOSDs,
    _trt__GetOSDsResponse& trt__GetOSDsResponse) {
  SOAP_EMPTY_HANDLER(trt__GetOSDs, "Media");
}

int OnvifMediaBindingService::GetOSD(
    _trt__GetOSD* trt__GetOSD,
    _trt__GetOSDResponse& trt__GetOSDResponse) {
  SOAP_EMPTY_HANDLER(trt__GetOSD, "Media");
}

int OnvifMediaBindingService::GetOSDOptions(
    _trt__GetOSDOptions* trt__GetOSDOptions,
    _trt__GetOSDOptionsResponse& trt__GetOSDOptionsResponse) {
  SOAP_EMPTY_HANDLER(trt__GetOSDOptions, "Media");
}

int OnvifMediaBindingService::SetOSD(
    _trt__SetOSD* trt__SetOSD,
    _trt__SetOSDResponse& trt__SetOSDResponse) {
  SOAP_EMPTY_HANDLER(trt__SetOSD, "Media");
}

int OnvifMediaBindingService::CreateOSD(
    _trt__CreateOSD* trt__CreateOSD,
    _trt__CreateOSDResponse& trt__CreateOSDResponse) {
  SOAP_EMPTY_HANDLER(trt__CreateOSD, "Media");
}

int OnvifMediaBindingService::DeleteOSD(
    _trt__DeleteOSD* trt__DeleteOSD,
    _trt__DeleteOSDResponse& trt__DeleteOSDResponse) {
  SOAP_EMPTY_HANDLER(trt__DeleteOSD, "Media");
}
}  // namespace avp
