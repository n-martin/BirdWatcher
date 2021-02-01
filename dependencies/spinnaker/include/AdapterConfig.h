//=============================================================================
// Copyright (c) 2001-2019 FLIR Systems, Inc. All Rights Reserved.
//
// This software is the confidential and proprietary information of FLIR
// Integrated Imaging Solutions, Inc. ("Confidential Information"). You
// shall not disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with FLIR Integrated Imaging Solutions, Inc. (FLIR).
//
// FLIR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. FLIR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================

#ifndef FLIR_SPINNAKER_ADAPTER_CONFIG_H
#define FLIR_SPINNAKER_ADAPTER_CONFIG_H

#include <string>
#include <vector>

#ifdef ADAPTERCONFIG_EXPORTS
#define ADAPTERCONFIG_API __declspec(dllexport)
#else
#define ADAPTERCONFIG_API __declspec(dllimport)
#endif
namespace AdapterConfig
{
    struct IpInfo
    {
        std::string ipAddress;
        std::string subnetMask;
        std::string gateway;
        unsigned int subnetLength;

        IpInfo() : subnetLength(0)
        {
        }
    };

    struct AdapterInfo
    {
        std::string adapterName;
        std::string adapterGUID;
        std::string adapterMACAddress;
        std::string adapterDescription;
        bool dhcpEnabled;

        IpInfo ipInfo;

        std::string receiveBuffersRegKey;
        std::string transmitBuffersRegKey;
        std::string jumboPacketsRegKey;

        unsigned int transmitBuffers;
        unsigned int receiveBuffers;
        unsigned int jumboPackets;

        unsigned int receiveBuffersMin;
        unsigned int receiveBuffersMax;
        unsigned int receiveBuffersStep;
        unsigned int transmitBuffersMin;
        unsigned int transmitBuffersMax;
        unsigned int transmitBuffersStep;
        std::vector<unsigned int> jumboPacketValidValues;

        AdapterInfo()
            : transmitBuffers(0), receiveBuffers(0), jumboPackets(0), receiveBuffersMin(0), receiveBuffersMax(0),
              receiveBuffersStep(0), transmitBuffersMin(0), transmitBuffersMax(0), transmitBuffersStep(0)
        {
        }
    };

    enum AdapterConfigErr
    {
        IP_ADDRESS_INVALID,
        IP_ADDRESS_IS_NOT_V4,
        IP_ADDRESS_TOO_LARGE,
        IP_ADDRESS_TOO_SMALL,
        HOST_ADDRESS_ZERO,
        SUBNET_MASK_INVALID,
        VALID_SUBNET_NOT_FOUND
    };

    ADAPTERCONFIG_API std::vector<AdapterInfo> RetrieveAllAdapters();

    ADAPTERCONFIG_API void AutoPopulateAdapterInfo(
        std::vector<AdapterInfo>& adaptersToConfigure,
        const std::vector<AdapterInfo>& allAdapters);

    ADAPTERCONFIG_API void AutoPopulateAdvancedProperties(std::vector<AdapterInfo>& adaptersToConfigure);

    ADAPTERCONFIG_API void PopulateAdapterIpInfo(
        IpInfo startingIpInfo,
        std::vector<AdapterInfo>& adaptersToConfigure,
        const std::vector<AdapterInfo>& allAdapters);

    ADAPTERCONFIG_API void ValidateIpAddress(const std::string& ipAddr, unsigned int subnetMaskLength);

    ADAPTERCONFIG_API bool IsValidIpAddress(const std::string& ipAddr);
    ADAPTERCONFIG_API bool IsValidSubnetMask(const std::string& subnetMask);
    ADAPTERCONFIG_API bool IsOnSameSubnet(
        const std::string& ipAddrStr1,
        const std::string& ipAddrStr2,
        const unsigned int subnetMaskLength);

    ADAPTERCONFIG_API unsigned int GetSubnetMaskLength(const std::string& subnetMask);

    ADAPTERCONFIG_API std::string GetEnumerationLogFileName();
    ADAPTERCONFIG_API std::string GetConfigLogFileName();

    ADAPTERCONFIG_API void ConfigureAdapter(AdapterInfo& adapter, bool configureIP, bool configureAdvancedProperties);

    ADAPTERCONFIG_API unsigned int GetAutoSubnetMaskLength();
    ADAPTERCONFIG_API std::string GetAutoSubnetMask();

    ADAPTERCONFIG_API std::string GetMaxIpAddress();
    ADAPTERCONFIG_API std::string GetMinIpAddress();

    ADAPTERCONFIG_API std::string GetAutoGigabitDesc();
    ADAPTERCONFIG_API std::string GetAuto10GDesc();
    ADAPTERCONFIG_API std::string GetAutoStartIp();
} // namespace AdapterConfig

class AdapterConfigException : public std::exception
{
    AdapterConfig::AdapterConfigErr errCode;
    std::string param;

  public:
    AdapterConfigException(const AdapterConfig::AdapterConfigErr errCode) : errCode(errCode), param("")
    {
    }

    AdapterConfigException(const AdapterConfig::AdapterConfigErr errCode, std::string param)
        : errCode(errCode), param(std::move(param))
    {
    }

    AdapterConfig::AdapterConfigErr ErrCode() const
    {
        return errCode;
    }

    std::string GetParamStr() const
    {
        return param;
    }
};

#endif // FLIR_SPINNAKER_ADAPTER_CONFIG_H