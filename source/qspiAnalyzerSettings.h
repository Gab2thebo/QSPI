#ifndef QSPI_ANALYZER_SETTINGS
#define QSPI_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class qspiAnalyzerSettings : public AnalyzerSettings
{
public:
	qspiAnalyzerSettings();
	virtual ~qspiAnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	void UpdateInterfacesFromSettings();

	Channel mData0Channel;
	Channel mData1Channel;
	Channel mData2Channel;
	Channel mData3Channel;
	Channel mClockChannel;
	Channel mEnableChannel;
	BitState mClockInactiveState;
	U32 mDummyCycles;
	U32 mAddressSize;


protected:
	std::unique_ptr< AnalyzerSettingInterfaceChannel >	mData0ChannelInterface;
	std::unique_ptr< AnalyzerSettingInterfaceChannel >	mData1ChannelInterface;
	std::unique_ptr< AnalyzerSettingInterfaceChannel >	mData2ChannelInterface;
	std::unique_ptr< AnalyzerSettingInterfaceChannel >	mData3ChannelInterface;
	std::unique_ptr< AnalyzerSettingInterfaceChannel >	mClockChannelInterface;
	std::unique_ptr< AnalyzerSettingInterfaceChannel >	mEnableChannelInterface;
	std::unique_ptr< AnalyzerSettingInterfaceNumberList >	mClockInactiveStateInterface;
	std::unique_ptr< AnalyzerSettingInterfaceNumberList >	mDummyCyclesInterface;
	std::unique_ptr< AnalyzerSettingInterfaceNumberList >	mAddressSizeInterface;
};

#endif //QSPI_ANALYZER_SETTINGS
