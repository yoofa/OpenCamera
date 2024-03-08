/*
 * onvif_device_binding_service.h
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef ONVIF_DEVICE_BINDING_SERVICE_H
#define ONVIF_DEVICE_BINDING_SERVICE_H

#include "third_party/onvif_srvd/src/generated/soapDeviceBindingService.h"

namespace ave {

class OnvifDeviceBindingService : public DeviceBindingService {
 public:
  OnvifDeviceBindingService(struct soap* soap);
  virtual ~OnvifDeviceBindingService();

  // Service operations are listed below: you should define these
  // Note: compile with -DWITH_PURE_VIRTUAL to declare pure virtual methods
  //
  /// Web service operation 'GetServices' implementation, should return SOAP_OK
  /// or error code
  int GetServices(_tds__GetServices* tds__GetServices,
                  _tds__GetServicesResponse& tds__GetServicesResponse) override;
  //
  /// Web service operation 'GetServiceCapabilities' implementation, should
  /// return SOAP_OK or error code
  int GetServiceCapabilities(
      _tds__GetServiceCapabilities* tds__GetServiceCapabilities,
      _tds__GetServiceCapabilitiesResponse& tds__GetServiceCapabilitiesResponse)
      override;
  //
  /// Web service operation 'GetDeviceInformation' implementation, should return
  /// SOAP_OK or error code
  int GetDeviceInformation(
      _tds__GetDeviceInformation* tds__GetDeviceInformation,
      _tds__GetDeviceInformationResponse& tds__GetDeviceInformationResponse)
      override;
  //
  /// Web service operation 'SetSystemDateAndTime' implementation, should return
  /// SOAP_OK or error code
  int SetSystemDateAndTime(
      _tds__SetSystemDateAndTime* tds__SetSystemDateAndTime,
      _tds__SetSystemDateAndTimeResponse& tds__SetSystemDateAndTimeResponse)
      override;
  //
  /// Web service operation 'GetSystemDateAndTime' implementation, should return
  /// SOAP_OK or error code
  int GetSystemDateAndTime(
      _tds__GetSystemDateAndTime* tds__GetSystemDateAndTime,
      _tds__GetSystemDateAndTimeResponse& tds__GetSystemDateAndTimeResponse)
      override;
  //
  /// Web service operation 'SetSystemFactoryDefault' implementation, should
  /// return SOAP_OK or error code
  int SetSystemFactoryDefault(
      _tds__SetSystemFactoryDefault* tds__SetSystemFactoryDefault,
      _tds__SetSystemFactoryDefaultResponse&
          tds__SetSystemFactoryDefaultResponse) override;
  //
  /// Web service operation 'UpgradeSystemFirmware' implementation, should
  /// return SOAP_OK or error code
  int UpgradeSystemFirmware(
      _tds__UpgradeSystemFirmware* tds__UpgradeSystemFirmware,
      _tds__UpgradeSystemFirmwareResponse& tds__UpgradeSystemFirmwareResponse)
      override;
  //
  /// Web service operation 'SystemReboot' implementation, should return SOAP_OK
  /// or error code
  int SystemReboot(
      _tds__SystemReboot* tds__SystemReboot,
      _tds__SystemRebootResponse& tds__SystemRebootResponse) override;
  //
  /// Web service operation 'RestoreSystem' implementation, should return
  /// SOAP_OK or error code
  int RestoreSystem(
      _tds__RestoreSystem* tds__RestoreSystem,
      _tds__RestoreSystemResponse& tds__RestoreSystemResponse) override;
  //
  /// Web service operation 'GetSystemBackup' implementation, should return
  /// SOAP_OK or error code
  int GetSystemBackup(
      _tds__GetSystemBackup* tds__GetSystemBackup,
      _tds__GetSystemBackupResponse& tds__GetSystemBackupResponse) override;
  //
  /// Web service operation 'GetSystemLog' implementation, should return SOAP_OK
  /// or error code
  int GetSystemLog(
      _tds__GetSystemLog* tds__GetSystemLog,
      _tds__GetSystemLogResponse& tds__GetSystemLogResponse) override;
  //
  /// Web service operation 'GetSystemSupportInformation' implementation, should
  /// return SOAP_OK or error code
  int GetSystemSupportInformation(
      _tds__GetSystemSupportInformation* tds__GetSystemSupportInformation,
      _tds__GetSystemSupportInformationResponse&
          tds__GetSystemSupportInformationResponse) override;
  //
  /// Web service operation 'GetScopes' implementation, should return SOAP_OK or
  /// error code
  int GetScopes(_tds__GetScopes* tds__GetScopes,
                _tds__GetScopesResponse& tds__GetScopesResponse) override;
  //
  /// Web service operation 'SetScopes' implementation, should return SOAP_OK or
  /// error code
  int SetScopes(_tds__SetScopes* tds__SetScopes,
                _tds__SetScopesResponse& tds__SetScopesResponse) override;
  //
  /// Web service operation 'AddScopes' implementation, should return SOAP_OK or
  /// error code
  int AddScopes(_tds__AddScopes* tds__AddScopes,
                _tds__AddScopesResponse& tds__AddScopesResponse) override;
  //
  /// Web service operation 'RemoveScopes' implementation, should return SOAP_OK
  /// or error code
  int RemoveScopes(
      _tds__RemoveScopes* tds__RemoveScopes,
      _tds__RemoveScopesResponse& tds__RemoveScopesResponse) override;
  //
  /// Web service operation 'GetDiscoveryMode' implementation, should return
  /// SOAP_OK or error code
  int GetDiscoveryMode(
      _tds__GetDiscoveryMode* tds__GetDiscoveryMode,
      _tds__GetDiscoveryModeResponse& tds__GetDiscoveryModeResponse) override;
  //
  /// Web service operation 'SetDiscoveryMode' implementation, should return
  /// SOAP_OK or error code
  int SetDiscoveryMode(
      _tds__SetDiscoveryMode* tds__SetDiscoveryMode,
      _tds__SetDiscoveryModeResponse& tds__SetDiscoveryModeResponse) override;
  //
  /// Web service operation 'GetRemoteDiscoveryMode' implementation, should
  /// return SOAP_OK or error code
  int GetRemoteDiscoveryMode(
      _tds__GetRemoteDiscoveryMode* tds__GetRemoteDiscoveryMode,
      _tds__GetRemoteDiscoveryModeResponse& tds__GetRemoteDiscoveryModeResponse)
      override;
  //
  /// Web service operation 'SetRemoteDiscoveryMode' implementation, should
  /// return SOAP_OK or error code
  int SetRemoteDiscoveryMode(
      _tds__SetRemoteDiscoveryMode* tds__SetRemoteDiscoveryMode,
      _tds__SetRemoteDiscoveryModeResponse& tds__SetRemoteDiscoveryModeResponse)
      override;
  //
  /// Web service operation 'GetDPAddresses' implementation, should return
  /// SOAP_OK or error code
  int GetDPAddresses(
      _tds__GetDPAddresses* tds__GetDPAddresses,
      _tds__GetDPAddressesResponse& tds__GetDPAddressesResponse) override;
  //
  /// Web service operation 'GetEndpointReference' implementation, should return
  /// SOAP_OK or error code
  int GetEndpointReference(
      _tds__GetEndpointReference* tds__GetEndpointReference,
      _tds__GetEndpointReferenceResponse& tds__GetEndpointReferenceResponse)
      override;
  //
  /// Web service operation 'GetRemoteUser' implementation, should return
  /// SOAP_OK or error code
  int GetRemoteUser(
      _tds__GetRemoteUser* tds__GetRemoteUser,
      _tds__GetRemoteUserResponse& tds__GetRemoteUserResponse) override;
  //
  /// Web service operation 'SetRemoteUser' implementation, should return
  /// SOAP_OK or error code
  int SetRemoteUser(
      _tds__SetRemoteUser* tds__SetRemoteUser,
      _tds__SetRemoteUserResponse& tds__SetRemoteUserResponse) override;
  //
  /// Web service operation 'GetUsers' implementation, should return SOAP_OK or
  /// error code
  int GetUsers(_tds__GetUsers* tds__GetUsers,
               _tds__GetUsersResponse& tds__GetUsersResponse) override;
  //
  /// Web service operation 'CreateUsers' implementation, should return SOAP_OK
  /// or error code
  int CreateUsers(_tds__CreateUsers* tds__CreateUsers,
                  _tds__CreateUsersResponse& tds__CreateUsersResponse) override;
  //
  /// Web service operation 'DeleteUsers' implementation, should return SOAP_OK
  /// or error code
  int DeleteUsers(_tds__DeleteUsers* tds__DeleteUsers,
                  _tds__DeleteUsersResponse& tds__DeleteUsersResponse) override;
  //
  /// Web service operation 'SetUser' implementation, should return SOAP_OK or
  /// error code
  int SetUser(_tds__SetUser* tds__SetUser,
              _tds__SetUserResponse& tds__SetUserResponse) override;
  //
  /// Web service operation 'GetWsdlUrl' implementation, should return SOAP_OK
  /// or error code
  int GetWsdlUrl(_tds__GetWsdlUrl* tds__GetWsdlUrl,
                 _tds__GetWsdlUrlResponse& tds__GetWsdlUrlResponse) override;
  //
  /// Web service operation 'GetCapabilities' implementation, should return
  /// SOAP_OK or error code
  int GetCapabilities(
      _tds__GetCapabilities* tds__GetCapabilities,
      _tds__GetCapabilitiesResponse& tds__GetCapabilitiesResponse) override;
  //
  /// Web service operation 'SetDPAddresses' implementation, should return
  /// SOAP_OK or error code
  int SetDPAddresses(
      _tds__SetDPAddresses* tds__SetDPAddresses,
      _tds__SetDPAddressesResponse& tds__SetDPAddressesResponse) override;
  //
  /// Web service operation 'GetHostname' implementation, should return SOAP_OK
  /// or error code
  int GetHostname(_tds__GetHostname* tds__GetHostname,
                  _tds__GetHostnameResponse& tds__GetHostnameResponse) override;
  //
  /// Web service operation 'SetHostname' implementation, should return SOAP_OK
  /// or error code
  int SetHostname(_tds__SetHostname* tds__SetHostname,
                  _tds__SetHostnameResponse& tds__SetHostnameResponse) override;
  //
  /// Web service operation 'SetHostnameFromDHCP' implementation, should return
  /// SOAP_OK or error code
  int SetHostnameFromDHCP(_tds__SetHostnameFromDHCP* tds__SetHostnameFromDHCP,
                          _tds__SetHostnameFromDHCPResponse&
                              tds__SetHostnameFromDHCPResponse) override;
  //
  /// Web service operation 'GetDNS' implementation, should return SOAP_OK or
  /// error code
  int GetDNS(_tds__GetDNS* tds__GetDNS,
             _tds__GetDNSResponse& tds__GetDNSResponse) override;
  //
  /// Web service operation 'SetDNS' implementation, should return SOAP_OK or
  /// error code
  int SetDNS(_tds__SetDNS* tds__SetDNS,
             _tds__SetDNSResponse& tds__SetDNSResponse) override;
  //
  /// Web service operation 'GetNTP' implementation, should return SOAP_OK or
  /// error code
  int GetNTP(_tds__GetNTP* tds__GetNTP,
             _tds__GetNTPResponse& tds__GetNTPResponse) override;
  //
  /// Web service operation 'SetNTP' implementation, should return SOAP_OK or
  /// error code
  int SetNTP(_tds__SetNTP* tds__SetNTP,
             _tds__SetNTPResponse& tds__SetNTPResponse) override;
  //
  /// Web service operation 'GetDynamicDNS' implementation, should return
  /// SOAP_OK or error code
  int GetDynamicDNS(
      _tds__GetDynamicDNS* tds__GetDynamicDNS,
      _tds__GetDynamicDNSResponse& tds__GetDynamicDNSResponse) override;
  //
  /// Web service operation 'SetDynamicDNS' implementation, should return
  /// SOAP_OK or error code
  int SetDynamicDNS(
      _tds__SetDynamicDNS* tds__SetDynamicDNS,
      _tds__SetDynamicDNSResponse& tds__SetDynamicDNSResponse) override;
  //
  /// Web service operation 'GetNetworkInterfaces' implementation, should return
  /// SOAP_OK or error code
  int GetNetworkInterfaces(
      _tds__GetNetworkInterfaces* tds__GetNetworkInterfaces,
      _tds__GetNetworkInterfacesResponse& tds__GetNetworkInterfacesResponse)
      override;
  //
  /// Web service operation 'SetNetworkInterfaces' implementation, should return
  /// SOAP_OK or error code
  int SetNetworkInterfaces(
      _tds__SetNetworkInterfaces* tds__SetNetworkInterfaces,
      _tds__SetNetworkInterfacesResponse& tds__SetNetworkInterfacesResponse)
      override;
  //
  /// Web service operation 'GetNetworkProtocols' implementation, should return
  /// SOAP_OK or error code
  int GetNetworkProtocols(_tds__GetNetworkProtocols* tds__GetNetworkProtocols,
                          _tds__GetNetworkProtocolsResponse&
                              tds__GetNetworkProtocolsResponse) override;
  //
  /// Web service operation 'SetNetworkProtocols' implementation, should return
  /// SOAP_OK or error code
  int SetNetworkProtocols(_tds__SetNetworkProtocols* tds__SetNetworkProtocols,
                          _tds__SetNetworkProtocolsResponse&
                              tds__SetNetworkProtocolsResponse) override;
  //
  /// Web service operation 'GetNetworkDefaultGateway' implementation, should
  /// return SOAP_OK or error code
  int GetNetworkDefaultGateway(
      _tds__GetNetworkDefaultGateway* tds__GetNetworkDefaultGateway,
      _tds__GetNetworkDefaultGatewayResponse&
          tds__GetNetworkDefaultGatewayResponse) override;
  //
  /// Web service operation 'SetNetworkDefaultGateway' implementation, should
  /// return SOAP_OK or error code
  int SetNetworkDefaultGateway(
      _tds__SetNetworkDefaultGateway* tds__SetNetworkDefaultGateway,
      _tds__SetNetworkDefaultGatewayResponse&
          tds__SetNetworkDefaultGatewayResponse) override;
  //
  /// Web service operation 'GetZeroConfiguration' implementation, should return
  /// SOAP_OK or error code
  int GetZeroConfiguration(
      _tds__GetZeroConfiguration* tds__GetZeroConfiguration,
      _tds__GetZeroConfigurationResponse& tds__GetZeroConfigurationResponse)
      override;
  //
  /// Web service operation 'SetZeroConfiguration' implementation, should return
  /// SOAP_OK or error code
  int SetZeroConfiguration(
      _tds__SetZeroConfiguration* tds__SetZeroConfiguration,
      _tds__SetZeroConfigurationResponse& tds__SetZeroConfigurationResponse)
      override;
  //
  /// Web service operation 'GetIPAddressFilter' implementation, should return
  /// SOAP_OK or error code
  int GetIPAddressFilter(_tds__GetIPAddressFilter* tds__GetIPAddressFilter,
                         _tds__GetIPAddressFilterResponse&
                             tds__GetIPAddressFilterResponse) override;
  //
  /// Web service operation 'SetIPAddressFilter' implementation, should return
  /// SOAP_OK or error code
  int SetIPAddressFilter(_tds__SetIPAddressFilter* tds__SetIPAddressFilter,
                         _tds__SetIPAddressFilterResponse&
                             tds__SetIPAddressFilterResponse) override;
  //
  /// Web service operation 'AddIPAddressFilter' implementation, should return
  /// SOAP_OK or error code
  int AddIPAddressFilter(_tds__AddIPAddressFilter* tds__AddIPAddressFilter,
                         _tds__AddIPAddressFilterResponse&
                             tds__AddIPAddressFilterResponse) override;
  //
  /// Web service operation 'RemoveIPAddressFilter' implementation, should
  /// return SOAP_OK or error code
  int RemoveIPAddressFilter(
      _tds__RemoveIPAddressFilter* tds__RemoveIPAddressFilter,
      _tds__RemoveIPAddressFilterResponse& tds__RemoveIPAddressFilterResponse)
      override;
  //
  /// Web service operation 'GetAccessPolicy' implementation, should return
  /// SOAP_OK or error code
  int GetAccessPolicy(
      _tds__GetAccessPolicy* tds__GetAccessPolicy,
      _tds__GetAccessPolicyResponse& tds__GetAccessPolicyResponse) override;
  //
  /// Web service operation 'SetAccessPolicy' implementation, should return
  /// SOAP_OK or error code
  int SetAccessPolicy(
      _tds__SetAccessPolicy* tds__SetAccessPolicy,
      _tds__SetAccessPolicyResponse& tds__SetAccessPolicyResponse) override;
  //
  /// Web service operation 'CreateCertificate' implementation, should return
  /// SOAP_OK or error code
  int CreateCertificate(
      _tds__CreateCertificate* tds__CreateCertificate,
      _tds__CreateCertificateResponse& tds__CreateCertificateResponse) override;
  //
  /// Web service operation 'GetCertificates' implementation, should return
  /// SOAP_OK or error code
  int GetCertificates(
      _tds__GetCertificates* tds__GetCertificates,
      _tds__GetCertificatesResponse& tds__GetCertificatesResponse) override;
  //
  /// Web service operation 'GetCertificatesStatus' implementation, should
  /// return SOAP_OK or error code
  int GetCertificatesStatus(
      _tds__GetCertificatesStatus* tds__GetCertificatesStatus,
      _tds__GetCertificatesStatusResponse& tds__GetCertificatesStatusResponse)
      override;
  //
  /// Web service operation 'SetCertificatesStatus' implementation, should
  /// return SOAP_OK or error code
  int SetCertificatesStatus(
      _tds__SetCertificatesStatus* tds__SetCertificatesStatus,
      _tds__SetCertificatesStatusResponse& tds__SetCertificatesStatusResponse)
      override;
  //
  /// Web service operation 'DeleteCertificates' implementation, should return
  /// SOAP_OK or error code
  int DeleteCertificates(_tds__DeleteCertificates* tds__DeleteCertificates,
                         _tds__DeleteCertificatesResponse&
                             tds__DeleteCertificatesResponse) override;
  //
  /// Web service operation 'GetPkcs10Request' implementation, should return
  /// SOAP_OK or error code
  int GetPkcs10Request(
      _tds__GetPkcs10Request* tds__GetPkcs10Request,
      _tds__GetPkcs10RequestResponse& tds__GetPkcs10RequestResponse) override;
  //
  /// Web service operation 'LoadCertificates' implementation, should return
  /// SOAP_OK or error code
  int LoadCertificates(
      _tds__LoadCertificates* tds__LoadCertificates,
      _tds__LoadCertificatesResponse& tds__LoadCertificatesResponse) override;
  //
  /// Web service operation 'GetClientCertificateMode' implementation, should
  /// return SOAP_OK or error code
  int GetClientCertificateMode(
      _tds__GetClientCertificateMode* tds__GetClientCertificateMode,
      _tds__GetClientCertificateModeResponse&
          tds__GetClientCertificateModeResponse) override;
  //
  /// Web service operation 'SetClientCertificateMode' implementation, should
  /// return SOAP_OK or error code
  int SetClientCertificateMode(
      _tds__SetClientCertificateMode* tds__SetClientCertificateMode,
      _tds__SetClientCertificateModeResponse&
          tds__SetClientCertificateModeResponse) override;
  //
  /// Web service operation 'GetRelayOutputs' implementation, should return
  /// SOAP_OK or error code
  int GetRelayOutputs(
      _tds__GetRelayOutputs* tds__GetRelayOutputs,
      _tds__GetRelayOutputsResponse& tds__GetRelayOutputsResponse) override;
  //
  /// Web service operation 'SetRelayOutputSettings' implementation, should
  /// return SOAP_OK or error code
  int SetRelayOutputSettings(
      _tds__SetRelayOutputSettings* tds__SetRelayOutputSettings,
      _tds__SetRelayOutputSettingsResponse& tds__SetRelayOutputSettingsResponse)
      override;
  //
  /// Web service operation 'SetRelayOutputState' implementation, should return
  /// SOAP_OK or error code
  int SetRelayOutputState(_tds__SetRelayOutputState* tds__SetRelayOutputState,
                          _tds__SetRelayOutputStateResponse&
                              tds__SetRelayOutputStateResponse) override;
  //
  /// Web service operation 'SendAuxiliaryCommand' implementation, should return
  /// SOAP_OK or error code
  int SendAuxiliaryCommand(
      _tds__SendAuxiliaryCommand* tds__SendAuxiliaryCommand,
      _tds__SendAuxiliaryCommandResponse& tds__SendAuxiliaryCommandResponse)
      override;
  //
  /// Web service operation 'GetCACertificates' implementation, should return
  /// SOAP_OK or error code
  int GetCACertificates(
      _tds__GetCACertificates* tds__GetCACertificates,
      _tds__GetCACertificatesResponse& tds__GetCACertificatesResponse) override;
  //
  /// Web service operation 'LoadCertificateWithPrivateKey' implementation,
  /// should return SOAP_OK or error code
  int LoadCertificateWithPrivateKey(
      _tds__LoadCertificateWithPrivateKey* tds__LoadCertificateWithPrivateKey,
      _tds__LoadCertificateWithPrivateKeyResponse&
          tds__LoadCertificateWithPrivateKeyResponse) override;
  //
  /// Web service operation 'GetCertificateInformation' implementation, should
  /// return SOAP_OK or error code
  int GetCertificateInformation(
      _tds__GetCertificateInformation* tds__GetCertificateInformation,
      _tds__GetCertificateInformationResponse&
          tds__GetCertificateInformationResponse) override;
  //
  /// Web service operation 'LoadCACertificates' implementation, should return
  /// SOAP_OK or error code
  int LoadCACertificates(_tds__LoadCACertificates* tds__LoadCACertificates,
                         _tds__LoadCACertificatesResponse&
                             tds__LoadCACertificatesResponse) override;
  //
  /// Web service operation 'CreateDot1XConfiguration' implementation, should
  /// return SOAP_OK or error code
  int CreateDot1XConfiguration(
      _tds__CreateDot1XConfiguration* tds__CreateDot1XConfiguration,
      _tds__CreateDot1XConfigurationResponse&
          tds__CreateDot1XConfigurationResponse) override;
  //
  /// Web service operation 'SetDot1XConfiguration' implementation, should
  /// return SOAP_OK or error code
  int SetDot1XConfiguration(
      _tds__SetDot1XConfiguration* tds__SetDot1XConfiguration,
      _tds__SetDot1XConfigurationResponse& tds__SetDot1XConfigurationResponse)
      override;
  //
  /// Web service operation 'GetDot1XConfiguration' implementation, should
  /// return SOAP_OK or error code
  int GetDot1XConfiguration(
      _tds__GetDot1XConfiguration* tds__GetDot1XConfiguration,
      _tds__GetDot1XConfigurationResponse& tds__GetDot1XConfigurationResponse)
      override;
  //
  /// Web service operation 'GetDot1XConfigurations' implementation, should
  /// return SOAP_OK or error code
  int GetDot1XConfigurations(
      _tds__GetDot1XConfigurations* tds__GetDot1XConfigurations,
      _tds__GetDot1XConfigurationsResponse& tds__GetDot1XConfigurationsResponse)
      override;
  //
  /// Web service operation 'DeleteDot1XConfiguration' implementation, should
  /// return SOAP_OK or error code
  int DeleteDot1XConfiguration(
      _tds__DeleteDot1XConfiguration* tds__DeleteDot1XConfiguration,
      _tds__DeleteDot1XConfigurationResponse&
          tds__DeleteDot1XConfigurationResponse) override;
  //
  /// Web service operation 'GetDot11Capabilities' implementation, should return
  /// SOAP_OK or error code
  int GetDot11Capabilities(
      _tds__GetDot11Capabilities* tds__GetDot11Capabilities,
      _tds__GetDot11CapabilitiesResponse& tds__GetDot11CapabilitiesResponse)
      override;
  //
  /// Web service operation 'GetDot11Status' implementation, should return
  /// SOAP_OK or error code
  int GetDot11Status(
      _tds__GetDot11Status* tds__GetDot11Status,
      _tds__GetDot11StatusResponse& tds__GetDot11StatusResponse) override;
  //
  /// Web service operation 'ScanAvailableDot11Networks' implementation, should
  /// return SOAP_OK or error code
  int ScanAvailableDot11Networks(
      _tds__ScanAvailableDot11Networks* tds__ScanAvailableDot11Networks,
      _tds__ScanAvailableDot11NetworksResponse&
          tds__ScanAvailableDot11NetworksResponse) override;
  //
  /// Web service operation 'GetSystemUris' implementation, should return
  /// SOAP_OK or error code
  int GetSystemUris(
      _tds__GetSystemUris* tds__GetSystemUris,
      _tds__GetSystemUrisResponse& tds__GetSystemUrisResponse) override;
  //
  /// Web service operation 'StartFirmwareUpgrade' implementation, should return
  /// SOAP_OK or error code
  int StartFirmwareUpgrade(
      _tds__StartFirmwareUpgrade* tds__StartFirmwareUpgrade,
      _tds__StartFirmwareUpgradeResponse& tds__StartFirmwareUpgradeResponse)
      override;
  //
  /// Web service operation 'StartSystemRestore' implementation, should return
  /// SOAP_OK or error code
  int StartSystemRestore(_tds__StartSystemRestore* tds__StartSystemRestore,
                         _tds__StartSystemRestoreResponse&
                             tds__StartSystemRestoreResponse) override;
  //
  /// Web service operation 'GetStorageConfigurations' implementation, should
  /// return SOAP_OK or error code
  int GetStorageConfigurations(
      _tds__GetStorageConfigurations* tds__GetStorageConfigurations,
      _tds__GetStorageConfigurationsResponse&
          tds__GetStorageConfigurationsResponse) override;
  //
  /// Web service operation 'CreateStorageConfiguration' implementation, should
  /// return SOAP_OK or error code
  int CreateStorageConfiguration(
      _tds__CreateStorageConfiguration* tds__CreateStorageConfiguration,
      _tds__CreateStorageConfigurationResponse&
          tds__CreateStorageConfigurationResponse) override;
  //
  /// Web service operation 'GetStorageConfiguration' implementation, should
  /// return SOAP_OK or error code
  int GetStorageConfiguration(
      _tds__GetStorageConfiguration* tds__GetStorageConfiguration,
      _tds__GetStorageConfigurationResponse&
          tds__GetStorageConfigurationResponse) override;
  //
  /// Web service operation 'SetStorageConfiguration' implementation, should
  /// return SOAP_OK or error code
  int SetStorageConfiguration(
      _tds__SetStorageConfiguration* tds__SetStorageConfiguration,
      _tds__SetStorageConfigurationResponse&
          tds__SetStorageConfigurationResponse) override;
  //
  /// Web service operation 'DeleteStorageConfiguration' implementation, should
  /// return SOAP_OK or error code
  int DeleteStorageConfiguration(
      _tds__DeleteStorageConfiguration* tds__DeleteStorageConfiguration,
      _tds__DeleteStorageConfigurationResponse&
          tds__DeleteStorageConfigurationResponse) override;
  //
  /// Web service operation 'GetGeoLocation' implementation, should return
  /// SOAP_OK or error code
  int GetGeoLocation(
      _tds__GetGeoLocation* tds__GetGeoLocation,
      _tds__GetGeoLocationResponse& tds__GetGeoLocationResponse) override;
  //
  /// Web service operation 'SetGeoLocation' implementation, should return
  /// SOAP_OK or error code
  int SetGeoLocation(
      _tds__SetGeoLocation* tds__SetGeoLocation,
      _tds__SetGeoLocationResponse& tds__SetGeoLocationResponse) override;
  //
  /// Web service operation 'DeleteGeoLocation' implementation, should return
  /// SOAP_OK or error code
  int DeleteGeoLocation(
      _tds__DeleteGeoLocation* tds__DeleteGeoLocation,
      _tds__DeleteGeoLocationResponse& tds__DeleteGeoLocationResponse) override;

 private:
  /* data */
};
}  // namespace ave

#endif /* !ONVIF_DEVICE_BINDING_SERVICE_H */
