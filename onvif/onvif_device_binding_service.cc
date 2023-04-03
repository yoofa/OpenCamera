/*
 * onvif_device_binding_service.cc
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "onvif_device_binding_service.h"

#include <sys/reboot.h>
#include <thread>

#include "base/logging.h"
#include "onvif_server.h"
#include "third_party/onvif_srvd/src/src/ServiceContext.h"
#include "third_party/onvif_srvd/src/src/smacros.h"
#include "third_party/onvif_srvd/src/src/stools.h"

namespace avp {

OnvifDeviceBindingService::OnvifDeviceBindingService(struct soap* soap)
    : DeviceBindingService(soap) {}

OnvifDeviceBindingService::~OnvifDeviceBindingService() {}

int OnvifDeviceBindingService::GetServices(
    _tds__GetServices* tds__GetServices,
    _tds__GetServicesResponse& tds__GetServicesResponse) {
  LOG(LS_INFO) << "Device: " << __FUNCTION__;

  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->service_info_.get();

  std::string XAddr = ctx->getXAddr(this->soap);

  // Device Service
  tds__GetServicesResponse.Service.push_back(soap_new_tds__Service(this->soap));
  tds__GetServicesResponse.Service.back()->Namespace =
      "http://www.onvif.org/ver10/device/wsdl";
  tds__GetServicesResponse.Service.back()->XAddr =
      XAddr + "/onvif/device_service";
  //    tds__GetServicesResponse.Service.back()->Version    =
  //    soap_new_req_tt__OnvifVersion(this->soap, 2, 5);
  tds__GetServicesResponse.Service.back()->Version =
      soap_new_req_tt__OnvifVersion(this->soap, 19, 12);
  if (tds__GetServices->IncludeCapability) {
    tds__GetServicesResponse.Service.back()->Capabilities =
        soap_new__tds__Service_Capabilities(this->soap);
    tds__DeviceServiceCapabilities* capabilities =
        ctx->getDeviceServiceCapabilities(this->soap);
    tds__GetServicesResponse.Service.back()->Capabilities->__any =
        soap_dom_element(this->soap, NULL, "tds:Capabilities", capabilities,
                         capabilities->soap_type());
  }

  tds__GetServicesResponse.Service.push_back(soap_new_tds__Service(this->soap));
  tds__GetServicesResponse.Service.back()->Namespace =
      "http://www.onvif.org/ver10/media/wsdl";
  tds__GetServicesResponse.Service.back()->XAddr =
      XAddr + "/onvif/media_service";
  //    tds__GetServicesResponse.Service.back()->Version    =
  //    soap_new_req_tt__OnvifVersion(this->soap, 2, 6);
  tds__GetServicesResponse.Service.back()->Version =
      soap_new_req_tt__OnvifVersion(this->soap, 19, 6);
  if (tds__GetServices->IncludeCapability) {
    tds__GetServicesResponse.Service.back()->Capabilities =
        soap_new__tds__Service_Capabilities(this->soap);
    trt__Capabilities* capabilities =
        ctx->getMediaServiceCapabilities(this->soap);
    tds__GetServicesResponse.Service.back()->Capabilities->__any =
        soap_dom_element(this->soap, NULL, "trt:Capabilities", capabilities,
                         capabilities->soap_type());
  }

  if (ctx->get_ptz_node()->enable) {
    tds__GetServicesResponse.Service.push_back(
        soap_new_tds__Service(this->soap));
    tds__GetServicesResponse.Service.back()->Namespace =
        "http://www.onvif.org/ver20/ptz/wsdl";
    tds__GetServicesResponse.Service.back()->XAddr =
        XAddr + "/onvif/ptz_service";
    //        tds__GetServicesResponse.Service.back()->Version    =
    //        soap_new_req_tt__OnvifVersion(this->soap, 2, 4);
    tds__GetServicesResponse.Service.back()->Version =
        soap_new_req_tt__OnvifVersion(this->soap, 20, 12);
    if (tds__GetServices->IncludeCapability) {
      tds__GetServicesResponse.Service.back()->Capabilities =
          soap_new__tds__Service_Capabilities(this->soap);
      tptz__Capabilities* capabilities =
          ctx->getPTZServiceCapabilities(this->soap);
      tds__GetServicesResponse.Service.back()->Capabilities->__any =
          soap_dom_element(this->soap, NULL, "tptz:Capabilities", capabilities,
                           capabilities->soap_type());
    }
  }

  return SOAP_OK;
}

int OnvifDeviceBindingService::GetServiceCapabilities(
    _tds__GetServiceCapabilities* tds__GetServiceCapabilities,
    _tds__GetServiceCapabilitiesResponse& tds__GetServiceCapabilitiesResponse) {
  UNUSED(tds__GetServiceCapabilities);
  LOG(LS_INFO) << "Device: " << __FUNCTION__;

  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->service_info_.get();
  tds__GetServiceCapabilitiesResponse.Capabilities =
      ctx->getDeviceServiceCapabilities(this->soap);

  return SOAP_OK;
}

int OnvifDeviceBindingService::GetDeviceInformation(
    _tds__GetDeviceInformation* tds__GetDeviceInformation,
    _tds__GetDeviceInformationResponse& tds__GetDeviceInformationResponse) {
  UNUSED(tds__GetDeviceInformation);
  LOG(LS_INFO) << "Device: " << __FUNCTION__;

  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->service_info_.get();
  tds__GetDeviceInformationResponse.Manufacturer = ctx->manufacturer;
  tds__GetDeviceInformationResponse.Model = ctx->model;
  tds__GetDeviceInformationResponse.FirmwareVersion = ctx->firmware_version;
  tds__GetDeviceInformationResponse.SerialNumber = ctx->serial_number;
  tds__GetDeviceInformationResponse.HardwareId = ctx->hardware_id;

  return SOAP_OK;
}

int OnvifDeviceBindingService::SetSystemDateAndTime(
    _tds__SetSystemDateAndTime* tds__SetSystemDateAndTime,
    _tds__SetSystemDateAndTimeResponse& tds__SetSystemDateAndTimeResponse) {
  SOAP_EMPTY_HANDLER(tds__SetSystemDateAndTime, "Device");
}

int OnvifDeviceBindingService::GetSystemDateAndTime(
    _tds__GetSystemDateAndTime* tds__GetSystemDateAndTime,
    _tds__GetSystemDateAndTimeResponse& tds__GetSystemDateAndTimeResponse) {
  UNUSED(tds__GetSystemDateAndTime);
  LOG(LS_INFO) << "Device: " << __FUNCTION__;

  const time_t timestamp = time(NULL);
  struct tm* tm = gmtime(&timestamp);

  tds__GetSystemDateAndTimeResponse.SystemDateAndTime =
      soap_new_tt__SystemDateTime(this->soap);
  tds__GetSystemDateAndTimeResponse.SystemDateAndTime->DateTimeType =
      tt__SetDateTimeType__Manual;
  tds__GetSystemDateAndTimeResponse.SystemDateAndTime->DaylightSavings =
      tm->tm_isdst;
  tds__GetSystemDateAndTimeResponse.SystemDateAndTime->TimeZone =
      soap_new_tt__TimeZone(this->soap);
  tds__GetSystemDateAndTimeResponse.SystemDateAndTime->TimeZone->TZ = "GMT0";
  tds__GetSystemDateAndTimeResponse.SystemDateAndTime->UTCDateTime =
      soap_new_tt__DateTime(this->soap);
  tds__GetSystemDateAndTimeResponse.SystemDateAndTime->UTCDateTime->Time =
      soap_new_req_tt__Time(this->soap, tm->tm_hour, tm->tm_min, tm->tm_sec);
  tds__GetSystemDateAndTimeResponse.SystemDateAndTime->UTCDateTime->Date =
      soap_new_req_tt__Date(this->soap, tm->tm_year + 1900, tm->tm_mon + 1,
                            tm->tm_mday);

  return SOAP_OK;
}

int OnvifDeviceBindingService::SetSystemFactoryDefault(
    _tds__SetSystemFactoryDefault* tds__SetSystemFactoryDefault,
    _tds__SetSystemFactoryDefaultResponse&
        tds__SetSystemFactoryDefaultResponse) {
  SOAP_EMPTY_HANDLER(tds__SetSystemFactoryDefault, "Device");
}

int OnvifDeviceBindingService::UpgradeSystemFirmware(
    _tds__UpgradeSystemFirmware* tds__UpgradeSystemFirmware,
    _tds__UpgradeSystemFirmwareResponse& tds__UpgradeSystemFirmwareResponse) {
  SOAP_EMPTY_HANDLER(tds__UpgradeSystemFirmware, "Device");
}
namespace {

void reboot_task() {
  std::this_thread::sleep_for(std::chrono::seconds(3));
  sync();
  setuid(0);
  reboot(RB_AUTOBOOT);
}
}  // namespace

int OnvifDeviceBindingService::SystemReboot(
    _tds__SystemReboot* tds__SystemReboot,
    _tds__SystemRebootResponse& tds__SystemRebootResponse) {
  std::thread tr(reboot_task);
  tr.detach();
  return SOAP_OK;
}

int OnvifDeviceBindingService::RestoreSystem(
    _tds__RestoreSystem* tds__RestoreSystem,
    _tds__RestoreSystemResponse& tds__RestoreSystemResponse) {
  SOAP_EMPTY_HANDLER(tds__RestoreSystem, "Device");
}

int OnvifDeviceBindingService::GetSystemBackup(
    _tds__GetSystemBackup* tds__GetSystemBackup,
    _tds__GetSystemBackupResponse& tds__GetSystemBackupResponse) {
  SOAP_EMPTY_HANDLER(tds__GetSystemBackup, "Device");
}

int OnvifDeviceBindingService::GetSystemLog(
    _tds__GetSystemLog* tds__GetSystemLog,
    _tds__GetSystemLogResponse& tds__GetSystemLogResponse) {
  SOAP_EMPTY_HANDLER(tds__GetSystemLog, "Device");
}

int OnvifDeviceBindingService::GetSystemSupportInformation(
    _tds__GetSystemSupportInformation* tds__GetSystemSupportInformation,
    _tds__GetSystemSupportInformationResponse&
        tds__GetSystemSupportInformationResponse) {
  SOAP_EMPTY_HANDLER(tds__GetSystemSupportInformation, "Device");
}

int OnvifDeviceBindingService::GetScopes(
    _tds__GetScopes* tds__GetScopes,
    _tds__GetScopesResponse& tds__GetScopesResponse) {
  UNUSED(tds__GetScopes);
  LOG(LS_INFO) << "Device: " << __FUNCTION__;

  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->service_info_.get();

  for (size_t i = 0; i < ctx->scopes.size(); ++i) {
    tds__GetScopesResponse.Scopes.push_back(soap_new_req_tt__Scope(
        soap, tt__ScopeDefinition__Fixed, ctx->scopes[i]));
  }

  return SOAP_OK;
}

int OnvifDeviceBindingService::SetScopes(
    _tds__SetScopes* tds__SetScopes,
    _tds__SetScopesResponse& tds__SetScopesResponse) {
  SOAP_EMPTY_HANDLER(tds__SetScopes, "Device");
}

int OnvifDeviceBindingService::AddScopes(
    _tds__AddScopes* tds__AddScopes,
    _tds__AddScopesResponse& tds__AddScopesResponse) {
  SOAP_EMPTY_HANDLER(tds__AddScopes, "Device");
}

int OnvifDeviceBindingService::RemoveScopes(
    _tds__RemoveScopes* tds__RemoveScopes,
    _tds__RemoveScopesResponse& tds__RemoveScopesResponse) {
  SOAP_EMPTY_HANDLER(tds__RemoveScopes, "Device");
}

int OnvifDeviceBindingService::GetDiscoveryMode(
    _tds__GetDiscoveryMode* tds__GetDiscoveryMode,
    _tds__GetDiscoveryModeResponse& tds__GetDiscoveryModeResponse) {
  SOAP_EMPTY_HANDLER(tds__GetDiscoveryMode, "Device");
}

int OnvifDeviceBindingService::SetDiscoveryMode(
    _tds__SetDiscoveryMode* tds__SetDiscoveryMode,
    _tds__SetDiscoveryModeResponse& tds__SetDiscoveryModeResponse) {
  SOAP_EMPTY_HANDLER(tds__SetDiscoveryMode, "Device");
}

int OnvifDeviceBindingService::GetRemoteDiscoveryMode(
    _tds__GetRemoteDiscoveryMode* tds__GetRemoteDiscoveryMode,
    _tds__GetRemoteDiscoveryModeResponse& tds__GetRemoteDiscoveryModeResponse) {
  SOAP_EMPTY_HANDLER(tds__GetRemoteDiscoveryMode, "Device");
}

int OnvifDeviceBindingService::SetRemoteDiscoveryMode(
    _tds__SetRemoteDiscoveryMode* tds__SetRemoteDiscoveryMode,
    _tds__SetRemoteDiscoveryModeResponse& tds__SetRemoteDiscoveryModeResponse) {
  SOAP_EMPTY_HANDLER(tds__SetRemoteDiscoveryMode, "Device");
}

int OnvifDeviceBindingService::GetDPAddresses(
    _tds__GetDPAddresses* tds__GetDPAddresses,
    _tds__GetDPAddressesResponse& tds__GetDPAddressesResponse) {
  SOAP_EMPTY_HANDLER(tds__GetDPAddresses, "Device");
}

int OnvifDeviceBindingService::GetEndpointReference(
    _tds__GetEndpointReference* tds__GetEndpointReference,
    _tds__GetEndpointReferenceResponse& tds__GetEndpointReferenceResponse) {
  SOAP_EMPTY_HANDLER(tds__GetEndpointReference, "Device");
}

int OnvifDeviceBindingService::GetRemoteUser(
    _tds__GetRemoteUser* tds__GetRemoteUser,
    _tds__GetRemoteUserResponse& tds__GetRemoteUserResponse) {
  SOAP_EMPTY_HANDLER(tds__GetRemoteUser, "Device");
}

int OnvifDeviceBindingService::SetRemoteUser(
    _tds__SetRemoteUser* tds__SetRemoteUser,
    _tds__SetRemoteUserResponse& tds__SetRemoteUserResponse) {
  SOAP_EMPTY_HANDLER(tds__SetRemoteUser, "Device");
}

int OnvifDeviceBindingService::GetUsers(
    _tds__GetUsers* tds__GetUsers,
    _tds__GetUsersResponse& tds__GetUsersResponse) {
  UNUSED(tds__GetUsers);
  LOG(LS_INFO) << "Device: " << __FUNCTION__;

  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->service_info_.get();

  if (!ctx->user.empty()) {
    tds__GetUsersResponse.User.push_back(soap_new_tt__User(this->soap));
    tds__GetUsersResponse.User.back()->Username = ctx->user;
  }

  return SOAP_OK;
}

int OnvifDeviceBindingService::CreateUsers(
    _tds__CreateUsers* tds__CreateUsers,
    _tds__CreateUsersResponse& tds__CreateUsersResponse) {
  SOAP_EMPTY_HANDLER(tds__CreateUsers, "Device");
}

int OnvifDeviceBindingService::DeleteUsers(
    _tds__DeleteUsers* tds__DeleteUsers,
    _tds__DeleteUsersResponse& tds__DeleteUsersResponse) {
  SOAP_EMPTY_HANDLER(tds__DeleteUsers, "Device");
}

int OnvifDeviceBindingService::SetUser(
    _tds__SetUser* tds__SetUser,
    _tds__SetUserResponse& tds__SetUserResponse) {
  SOAP_EMPTY_HANDLER(tds__SetUser, "Device");
}

int OnvifDeviceBindingService::GetWsdlUrl(
    _tds__GetWsdlUrl* tds__GetWsdlUrl,
    _tds__GetWsdlUrlResponse& tds__GetWsdlUrlResponse) {
  UNUSED(tds__GetWsdlUrl);
  LOG(LS_INFO) << "Device: " << __FUNCTION__;

  std::string url = soap->endpoint;
  tds__GetWsdlUrlResponse.WsdlUrl = url.c_str();

  return SOAP_OK;
}

int OnvifDeviceBindingService::GetCapabilities(
    _tds__GetCapabilities* tds__GetCapabilities,
    _tds__GetCapabilitiesResponse& tds__GetCapabilitiesResponse) {
  LOG(LS_INFO) << "Device: " << __FUNCTION__;

  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->service_info_.get();

  std::string XAddr = ctx->getXAddr(this->soap);

  tds__GetCapabilitiesResponse.Capabilities =
      soap_new_tt__Capabilities(this->soap);
  std::vector<tt__CapabilityCategory>& categories(
      tds__GetCapabilities->Category);
  if (categories.empty()) {
    categories.push_back(tt__CapabilityCategory__All);
  }

  for (tt__CapabilityCategory category : categories) {
    if (!tds__GetCapabilitiesResponse.Capabilities->Device &&
        ((category == tt__CapabilityCategory__All) ||
         (category == tt__CapabilityCategory__Device))) {
      tds__GetCapabilitiesResponse.Capabilities->Device =
          soap_new_tt__DeviceCapabilities(this->soap);
      tds__GetCapabilitiesResponse.Capabilities->Device->XAddr =
          XAddr + "/onvif/device_service";
      tds__GetCapabilitiesResponse.Capabilities->Device->System =
          soap_new_tt__SystemCapabilities(this->soap);
      tds__GetCapabilitiesResponse.Capabilities->Device->System
          ->SupportedVersions.push_back(
              soap_new_req_tt__OnvifVersion(this->soap, 20, 12));
      tds__GetCapabilitiesResponse.Capabilities->Device->System
          ->SupportedVersions.push_back(
              soap_new_req_tt__OnvifVersion(this->soap, 17, 6));
      tds__GetCapabilitiesResponse.Capabilities->Device->System
          ->SupportedVersions.push_back(
              soap_new_req_tt__OnvifVersion(this->soap, 2, 6));
      tds__GetCapabilitiesResponse.Capabilities->Device->System
          ->SupportedVersions.push_back(
              soap_new_req_tt__OnvifVersion(this->soap, 2, 4));
      tds__GetCapabilitiesResponse.Capabilities->Device->System
          ->SupportedVersions.push_back(
              soap_new_req_tt__OnvifVersion(this->soap, 2, 0));
      tds__GetCapabilitiesResponse.Capabilities->Device->Network =
          soap_new_tt__NetworkCapabilities(this->soap);
      tds__GetCapabilitiesResponse.Capabilities->Device->Security =
          soap_new_tt__SecurityCapabilities(this->soap);
      tds__GetCapabilitiesResponse.Capabilities->Device->IO =
          soap_new_tt__IOCapabilities(this->soap);
    }

    if (!tds__GetCapabilitiesResponse.Capabilities->Media &&
        ((category == tt__CapabilityCategory__All) ||
         (category == tt__CapabilityCategory__Media))) {
      tds__GetCapabilitiesResponse.Capabilities->Media =
          soap_new_tt__MediaCapabilities(this->soap);
      tds__GetCapabilitiesResponse.Capabilities->Media->XAddr =
          XAddr + "/onvif/media_service";
      tds__GetCapabilitiesResponse.Capabilities->Media->StreamingCapabilities =
          soap_new_tt__RealTimeStreamingCapabilities(this->soap);
      tds__GetCapabilitiesResponse.Capabilities->Media->StreamingCapabilities
          ->RTPMulticast = soap_new_ptr(soap, false);
      tds__GetCapabilitiesResponse.Capabilities->Media->StreamingCapabilities
          ->RTP_USCORETCP = soap_new_ptr(soap, false);
      tds__GetCapabilitiesResponse.Capabilities->Media->StreamingCapabilities
          ->RTP_USCORERTSP_USCORETCP = soap_new_ptr(soap, true);
    }

    if (ctx->get_ptz_node()->enable) {
      if (!tds__GetCapabilitiesResponse.Capabilities->PTZ &&
          ((category == tt__CapabilityCategory__All) ||
           (category == tt__CapabilityCategory__PTZ))) {
        tds__GetCapabilitiesResponse.Capabilities->PTZ =
            soap_new_tt__PTZCapabilities(this->soap);
        tds__GetCapabilitiesResponse.Capabilities->PTZ->XAddr =
            XAddr + "/onvif/ptz_service";
      }
    }
  }

  return SOAP_OK;
}

int OnvifDeviceBindingService::SetDPAddresses(
    _tds__SetDPAddresses* tds__SetDPAddresses,
    _tds__SetDPAddressesResponse& tds__SetDPAddressesResponse) {
  SOAP_EMPTY_HANDLER(tds__SetDPAddresses, "Device");
}

int OnvifDeviceBindingService::GetHostname(
    _tds__GetHostname* tds__GetHostname,
    _tds__GetHostnameResponse& tds__GetHostnameResponse) {
  SOAP_EMPTY_HANDLER(tds__GetHostname, "Device");
}

int OnvifDeviceBindingService::SetHostname(
    _tds__SetHostname* tds__SetHostname,
    _tds__SetHostnameResponse& tds__SetHostnameResponse) {
  SOAP_EMPTY_HANDLER(tds__SetHostname, "Device");
}

int OnvifDeviceBindingService::SetHostnameFromDHCP(
    _tds__SetHostnameFromDHCP* tds__SetHostnameFromDHCP,
    _tds__SetHostnameFromDHCPResponse& tds__SetHostnameFromDHCPResponse) {
  SOAP_EMPTY_HANDLER(tds__SetHostnameFromDHCP, "Device");
}

int OnvifDeviceBindingService::GetDNS(
    _tds__GetDNS* tds__GetDNS,
    _tds__GetDNSResponse& tds__GetDNSResponse) {
  SOAP_EMPTY_HANDLER(tds__GetDNS, "Device");
}

int OnvifDeviceBindingService::SetDNS(
    _tds__SetDNS* tds__SetDNS,
    _tds__SetDNSResponse& tds__SetDNSResponse) {
  SOAP_EMPTY_HANDLER(tds__SetDNS, "Device");
}

int OnvifDeviceBindingService::GetNTP(
    _tds__GetNTP* tds__GetNTP,
    _tds__GetNTPResponse& tds__GetNTPResponse) {
  SOAP_EMPTY_HANDLER(tds__GetNTP, "Device");
}

int OnvifDeviceBindingService::SetNTP(
    _tds__SetNTP* tds__SetNTP,
    _tds__SetNTPResponse& tds__SetNTPResponse) {
  SOAP_EMPTY_HANDLER(tds__SetNTP, "Device");
}

int OnvifDeviceBindingService::GetDynamicDNS(
    _tds__GetDynamicDNS* tds__GetDynamicDNS,
    _tds__GetDynamicDNSResponse& tds__GetDynamicDNSResponse) {
  SOAP_EMPTY_HANDLER(tds__GetDynamicDNS, "Device");
}

int OnvifDeviceBindingService::SetDynamicDNS(
    _tds__SetDynamicDNS* tds__SetDynamicDNS,
    _tds__SetDynamicDNSResponse& tds__SetDynamicDNSResponse) {
  SOAP_EMPTY_HANDLER(tds__SetDynamicDNS, "Device");
}

int OnvifDeviceBindingService::GetNetworkInterfaces(
    _tds__GetNetworkInterfaces* tds__GetNetworkInterfaces,
    _tds__GetNetworkInterfacesResponse& tds__GetNetworkInterfacesResponse) {
  UNUSED(tds__GetNetworkInterfaces);
  LOG(LS_INFO) << "Device: " << __FUNCTION__;

  OnvifServer* server = (OnvifServer*)this->soap->user;
  ServiceContext* ctx = (ServiceContext*)server->service_info_.get();

  for (size_t i = 0; i < ctx->eth_ifs.size(); ++i) {
    char tmp_buf[20];

    tds__GetNetworkInterfacesResponse.NetworkInterfaces.push_back(
        soap_new_tt__NetworkInterface(this->soap));
    tds__GetNetworkInterfacesResponse.NetworkInterfaces.back()->Enabled = true;
    tds__GetNetworkInterfacesResponse.NetworkInterfaces.back()->Info =
        soap_new_tt__NetworkInterfaceInfo(this->soap);
    tds__GetNetworkInterfacesResponse.NetworkInterfaces.back()->Info->Name =
        soap_new_std__string(this->soap);
    tds__GetNetworkInterfacesResponse.NetworkInterfaces.back()
        ->Info->Name->assign(ctx->eth_ifs[i].dev_name());

    ctx->eth_ifs[i].get_hwaddr(tmp_buf);
    tds__GetNetworkInterfacesResponse.NetworkInterfaces.back()
        ->Info->HwAddress = tmp_buf;

    tds__GetNetworkInterfacesResponse.NetworkInterfaces.back()->IPv4 =
        soap_new_tt__IPv4NetworkInterface(this->soap);
    tds__GetNetworkInterfacesResponse.NetworkInterfaces.back()->IPv4->Config =
        soap_new_tt__IPv4Configuration(this->soap);
    tds__GetNetworkInterfacesResponse.NetworkInterfaces.back()
        ->IPv4->Config->Manual.push_back(
            soap_new_tt__PrefixedIPv4Address(this->soap));

    ctx->eth_ifs[i].get_ip(tmp_buf);

    tds__GetNetworkInterfacesResponse.NetworkInterfaces.back()
        ->IPv4->Config->Manual.back()
        ->Address = tmp_buf;
    tds__GetNetworkInterfacesResponse.NetworkInterfaces.back()
        ->IPv4->Config->Manual.back()
        ->PrefixLength = ctx->eth_ifs[i].get_mask_prefix();
  }

  return SOAP_OK;
}

int OnvifDeviceBindingService::SetNetworkInterfaces(
    _tds__SetNetworkInterfaces* tds__SetNetworkInterfaces,
    _tds__SetNetworkInterfacesResponse& tds__SetNetworkInterfacesResponse) {
  SOAP_EMPTY_HANDLER(tds__SetNetworkInterfaces, "Device");
}

int OnvifDeviceBindingService::GetNetworkProtocols(
    _tds__GetNetworkProtocols* tds__GetNetworkProtocols,
    _tds__GetNetworkProtocolsResponse& tds__GetNetworkProtocolsResponse) {
  SOAP_EMPTY_HANDLER(tds__GetNetworkProtocols, "Device");
}

int OnvifDeviceBindingService::SetNetworkProtocols(
    _tds__SetNetworkProtocols* tds__SetNetworkProtocols,
    _tds__SetNetworkProtocolsResponse& tds__SetNetworkProtocolsResponse) {
  SOAP_EMPTY_HANDLER(tds__SetNetworkProtocols, "Device");
}

int OnvifDeviceBindingService::GetNetworkDefaultGateway(
    _tds__GetNetworkDefaultGateway* tds__GetNetworkDefaultGateway,
    _tds__GetNetworkDefaultGatewayResponse&
        tds__GetNetworkDefaultGatewayResponse) {
  SOAP_EMPTY_HANDLER(tds__GetNetworkDefaultGateway, "Device");
}

int OnvifDeviceBindingService::SetNetworkDefaultGateway(
    _tds__SetNetworkDefaultGateway* tds__SetNetworkDefaultGateway,
    _tds__SetNetworkDefaultGatewayResponse&
        tds__SetNetworkDefaultGatewayResponse) {
  SOAP_EMPTY_HANDLER(tds__SetNetworkDefaultGateway, "Device");
}

int OnvifDeviceBindingService::GetZeroConfiguration(
    _tds__GetZeroConfiguration* tds__GetZeroConfiguration,
    _tds__GetZeroConfigurationResponse& tds__GetZeroConfigurationResponse) {
  SOAP_EMPTY_HANDLER(tds__GetZeroConfiguration, "Device");
}

int OnvifDeviceBindingService::SetZeroConfiguration(
    _tds__SetZeroConfiguration* tds__SetZeroConfiguration,
    _tds__SetZeroConfigurationResponse& tds__SetZeroConfigurationResponse) {
  SOAP_EMPTY_HANDLER(tds__SetZeroConfiguration, "Device");
}

int OnvifDeviceBindingService::GetIPAddressFilter(
    _tds__GetIPAddressFilter* tds__GetIPAddressFilter,
    _tds__GetIPAddressFilterResponse& tds__GetIPAddressFilterResponse) {
  SOAP_EMPTY_HANDLER(tds__GetIPAddressFilter, "Device");
}

int OnvifDeviceBindingService::SetIPAddressFilter(
    _tds__SetIPAddressFilter* tds__SetIPAddressFilter,
    _tds__SetIPAddressFilterResponse& tds__SetIPAddressFilterResponse) {
  SOAP_EMPTY_HANDLER(tds__SetIPAddressFilter, "Device");
}

int OnvifDeviceBindingService::AddIPAddressFilter(
    _tds__AddIPAddressFilter* tds__AddIPAddressFilter,
    _tds__AddIPAddressFilterResponse& tds__AddIPAddressFilterResponse) {
  SOAP_EMPTY_HANDLER(tds__AddIPAddressFilter, "Device");
}

int OnvifDeviceBindingService::RemoveIPAddressFilter(
    _tds__RemoveIPAddressFilter* tds__RemoveIPAddressFilter,
    _tds__RemoveIPAddressFilterResponse& tds__RemoveIPAddressFilterResponse) {
  SOAP_EMPTY_HANDLER(tds__RemoveIPAddressFilter, "Device");
}

int OnvifDeviceBindingService::GetAccessPolicy(
    _tds__GetAccessPolicy* tds__GetAccessPolicy,
    _tds__GetAccessPolicyResponse& tds__GetAccessPolicyResponse) {
  SOAP_EMPTY_HANDLER(tds__GetAccessPolicy, "Device");
}

int OnvifDeviceBindingService::SetAccessPolicy(
    _tds__SetAccessPolicy* tds__SetAccessPolicy,
    _tds__SetAccessPolicyResponse& tds__SetAccessPolicyResponse) {
  SOAP_EMPTY_HANDLER(tds__SetAccessPolicy, "Device");
}

int OnvifDeviceBindingService::CreateCertificate(
    _tds__CreateCertificate* tds__CreateCertificate,
    _tds__CreateCertificateResponse& tds__CreateCertificateResponse) {
  SOAP_EMPTY_HANDLER(tds__CreateCertificate, "Device");
}

int OnvifDeviceBindingService::GetCertificates(
    _tds__GetCertificates* tds__GetCertificates,
    _tds__GetCertificatesResponse& tds__GetCertificatesResponse) {
  SOAP_EMPTY_HANDLER(tds__GetCertificates, "Device");
}

int OnvifDeviceBindingService::GetCertificatesStatus(
    _tds__GetCertificatesStatus* tds__GetCertificatesStatus,
    _tds__GetCertificatesStatusResponse& tds__GetCertificatesStatusResponse) {
  SOAP_EMPTY_HANDLER(tds__GetCertificatesStatus, "Device");
}

int OnvifDeviceBindingService::SetCertificatesStatus(
    _tds__SetCertificatesStatus* tds__SetCertificatesStatus,
    _tds__SetCertificatesStatusResponse& tds__SetCertificatesStatusResponse) {
  SOAP_EMPTY_HANDLER(tds__SetCertificatesStatus, "Device");
}

int OnvifDeviceBindingService::DeleteCertificates(
    _tds__DeleteCertificates* tds__DeleteCertificates,
    _tds__DeleteCertificatesResponse& tds__DeleteCertificatesResponse) {
  SOAP_EMPTY_HANDLER(tds__DeleteCertificates, "Device");
}

int OnvifDeviceBindingService::GetPkcs10Request(
    _tds__GetPkcs10Request* tds__GetPkcs10Request,
    _tds__GetPkcs10RequestResponse& tds__GetPkcs10RequestResponse) {
  SOAP_EMPTY_HANDLER(tds__GetPkcs10Request, "Device");
}

int OnvifDeviceBindingService::LoadCertificates(
    _tds__LoadCertificates* tds__LoadCertificates,
    _tds__LoadCertificatesResponse& tds__LoadCertificatesResponse) {
  SOAP_EMPTY_HANDLER(tds__LoadCertificates, "Device");
}

int OnvifDeviceBindingService::GetClientCertificateMode(
    _tds__GetClientCertificateMode* tds__GetClientCertificateMode,
    _tds__GetClientCertificateModeResponse&
        tds__GetClientCertificateModeResponse) {
  SOAP_EMPTY_HANDLER(tds__GetClientCertificateMode, "Device");
}

int OnvifDeviceBindingService::SetClientCertificateMode(
    _tds__SetClientCertificateMode* tds__SetClientCertificateMode,
    _tds__SetClientCertificateModeResponse&
        tds__SetClientCertificateModeResponse) {
  SOAP_EMPTY_HANDLER(tds__SetClientCertificateMode, "Device");
}

int OnvifDeviceBindingService::GetRelayOutputs(
    _tds__GetRelayOutputs* tds__GetRelayOutputs,
    _tds__GetRelayOutputsResponse& tds__GetRelayOutputsResponse) {
  SOAP_EMPTY_HANDLER(tds__GetRelayOutputs, "Device");
}

int OnvifDeviceBindingService::SetRelayOutputSettings(
    _tds__SetRelayOutputSettings* tds__SetRelayOutputSettings,
    _tds__SetRelayOutputSettingsResponse& tds__SetRelayOutputSettingsResponse) {
  SOAP_EMPTY_HANDLER(tds__SetRelayOutputSettings, "Device");
}

int OnvifDeviceBindingService::SetRelayOutputState(
    _tds__SetRelayOutputState* tds__SetRelayOutputState,
    _tds__SetRelayOutputStateResponse& tds__SetRelayOutputStateResponse) {
  SOAP_EMPTY_HANDLER(tds__SetRelayOutputState, "Device");
}

int OnvifDeviceBindingService::SendAuxiliaryCommand(
    _tds__SendAuxiliaryCommand* tds__SendAuxiliaryCommand,
    _tds__SendAuxiliaryCommandResponse& tds__SendAuxiliaryCommandResponse) {
  SOAP_EMPTY_HANDLER(tds__SendAuxiliaryCommand, "Device");
}

int OnvifDeviceBindingService::GetCACertificates(
    _tds__GetCACertificates* tds__GetCACertificates,
    _tds__GetCACertificatesResponse& tds__GetCACertificatesResponse) {
  SOAP_EMPTY_HANDLER(tds__GetCACertificates, "Device");
}

int OnvifDeviceBindingService::LoadCertificateWithPrivateKey(
    _tds__LoadCertificateWithPrivateKey* tds__LoadCertificateWithPrivateKey,
    _tds__LoadCertificateWithPrivateKeyResponse&
        tds__LoadCertificateWithPrivateKeyResponse) {
  SOAP_EMPTY_HANDLER(tds__LoadCertificateWithPrivateKey, "Device");
}

int OnvifDeviceBindingService::GetCertificateInformation(
    _tds__GetCertificateInformation* tds__GetCertificateInformation,
    _tds__GetCertificateInformationResponse&
        tds__GetCertificateInformationResponse) {
  SOAP_EMPTY_HANDLER(tds__GetCertificateInformation, "Device");
}

int OnvifDeviceBindingService::LoadCACertificates(
    _tds__LoadCACertificates* tds__LoadCACertificates,
    _tds__LoadCACertificatesResponse& tds__LoadCACertificatesResponse) {
  SOAP_EMPTY_HANDLER(tds__LoadCACertificates, "Device");
}

int OnvifDeviceBindingService::CreateDot1XConfiguration(
    _tds__CreateDot1XConfiguration* tds__CreateDot1XConfiguration,
    _tds__CreateDot1XConfigurationResponse&
        tds__CreateDot1XConfigurationResponse) {
  SOAP_EMPTY_HANDLER(tds__CreateDot1XConfiguration, "Device");
}

int OnvifDeviceBindingService::SetDot1XConfiguration(
    _tds__SetDot1XConfiguration* tds__SetDot1XConfiguration,
    _tds__SetDot1XConfigurationResponse& tds__SetDot1XConfigurationResponse) {
  SOAP_EMPTY_HANDLER(tds__SetDot1XConfiguration, "Device");
}

int OnvifDeviceBindingService::GetDot1XConfiguration(
    _tds__GetDot1XConfiguration* tds__GetDot1XConfiguration,
    _tds__GetDot1XConfigurationResponse& tds__GetDot1XConfigurationResponse) {
  SOAP_EMPTY_HANDLER(tds__GetDot1XConfiguration, "Device");
}

int OnvifDeviceBindingService::GetDot1XConfigurations(
    _tds__GetDot1XConfigurations* tds__GetDot1XConfigurations,
    _tds__GetDot1XConfigurationsResponse& tds__GetDot1XConfigurationsResponse) {
  SOAP_EMPTY_HANDLER(tds__GetDot1XConfigurations, "Device");
}

int OnvifDeviceBindingService::DeleteDot1XConfiguration(
    _tds__DeleteDot1XConfiguration* tds__DeleteDot1XConfiguration,
    _tds__DeleteDot1XConfigurationResponse&
        tds__DeleteDot1XConfigurationResponse) {
  SOAP_EMPTY_HANDLER(tds__DeleteDot1XConfiguration, "Device");
}

int OnvifDeviceBindingService::GetDot11Capabilities(
    _tds__GetDot11Capabilities* tds__GetDot11Capabilities,
    _tds__GetDot11CapabilitiesResponse& tds__GetDot11CapabilitiesResponse) {
  SOAP_EMPTY_HANDLER(tds__GetDot11Capabilities, "Device");
}

int OnvifDeviceBindingService::GetDot11Status(
    _tds__GetDot11Status* tds__GetDot11Status,
    _tds__GetDot11StatusResponse& tds__GetDot11StatusResponse) {
  SOAP_EMPTY_HANDLER(tds__GetDot11Status, "Device");
}

int OnvifDeviceBindingService::ScanAvailableDot11Networks(
    _tds__ScanAvailableDot11Networks* tds__ScanAvailableDot11Networks,
    _tds__ScanAvailableDot11NetworksResponse&
        tds__ScanAvailableDot11NetworksResponse) {
  SOAP_EMPTY_HANDLER(tds__ScanAvailableDot11Networks, "Device");
}

int OnvifDeviceBindingService::GetSystemUris(
    _tds__GetSystemUris* tds__GetSystemUris,
    _tds__GetSystemUrisResponse& tds__GetSystemUrisResponse) {
  SOAP_EMPTY_HANDLER(tds__GetSystemUris, "Device");
}

int OnvifDeviceBindingService::StartFirmwareUpgrade(
    _tds__StartFirmwareUpgrade* tds__StartFirmwareUpgrade,
    _tds__StartFirmwareUpgradeResponse& tds__StartFirmwareUpgradeResponse) {
  SOAP_EMPTY_HANDLER(tds__StartFirmwareUpgrade, "Device");
}

int OnvifDeviceBindingService::StartSystemRestore(
    _tds__StartSystemRestore* tds__StartSystemRestore,
    _tds__StartSystemRestoreResponse& tds__StartSystemRestoreResponse) {
  SOAP_EMPTY_HANDLER(tds__StartSystemRestore, "Device");
}

int OnvifDeviceBindingService::GetStorageConfigurations(
    _tds__GetStorageConfigurations* tds__GetStorageConfigurations,
    _tds__GetStorageConfigurationsResponse&
        tds__GetStorageConfigurationsResponse) {
  SOAP_EMPTY_HANDLER(tds__GetStorageConfigurations, "Device");
}

int OnvifDeviceBindingService::CreateStorageConfiguration(
    _tds__CreateStorageConfiguration* tds__CreateStorageConfiguration,
    _tds__CreateStorageConfigurationResponse&
        tds__CreateStorageConfigurationResponse) {
  SOAP_EMPTY_HANDLER(tds__CreateStorageConfiguration, "Device");
}

int OnvifDeviceBindingService::GetStorageConfiguration(
    _tds__GetStorageConfiguration* tds__GetStorageConfiguration,
    _tds__GetStorageConfigurationResponse&
        tds__GetStorageConfigurationResponse) {
  SOAP_EMPTY_HANDLER(tds__GetStorageConfiguration, "Device");
}

int OnvifDeviceBindingService::SetStorageConfiguration(
    _tds__SetStorageConfiguration* tds__SetStorageConfiguration,
    _tds__SetStorageConfigurationResponse&
        tds__SetStorageConfigurationResponse) {
  SOAP_EMPTY_HANDLER(tds__SetStorageConfiguration, "Device");
}

int OnvifDeviceBindingService::DeleteStorageConfiguration(
    _tds__DeleteStorageConfiguration* tds__DeleteStorageConfiguration,
    _tds__DeleteStorageConfigurationResponse&
        tds__DeleteStorageConfigurationResponse) {
  SOAP_EMPTY_HANDLER(tds__DeleteStorageConfiguration, "Device");
}

int OnvifDeviceBindingService::GetGeoLocation(
    _tds__GetGeoLocation* tds__GetGeoLocation,
    _tds__GetGeoLocationResponse& tds__GetGeoLocationResponse) {
  SOAP_EMPTY_HANDLER(tds__GetGeoLocation, "Device");
}

int OnvifDeviceBindingService::SetGeoLocation(
    _tds__SetGeoLocation* tds__SetGeoLocation,
    _tds__SetGeoLocationResponse& tds__SetGeoLocationResponse) {
  SOAP_EMPTY_HANDLER(tds__SetGeoLocation, "Device");
}

int OnvifDeviceBindingService::DeleteGeoLocation(
    _tds__DeleteGeoLocation* tds__DeleteGeoLocation,
    _tds__DeleteGeoLocationResponse& tds__DeleteGeoLocationResponse) {
  SOAP_EMPTY_HANDLER(tds__DeleteGeoLocation, "Device");
}
}  // namespace avp
