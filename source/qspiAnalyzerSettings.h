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
	AnalyzerEnums::ShiftOrder mShiftOrder;
	U32 mBitsPerTransfer;
	BitState mClockInactiveState;
	AnalyzerEnums::Edge mDataValidEdge;
	BitState mEnableActiveState;

protected:
	std::unique_ptr< AnalyzerSettingInterfaceChannel >	mData0ChannelInterface;
	std::unique_ptr< AnalyzerSettingInterfaceChannel >	mData1ChannelInterface;
	std::unique_ptr< AnalyzerSettingInterfaceChannel >	mData2ChannelInterface;
	std::unique_ptr< AnalyzerSettingInterfaceChannel >	mData3ChannelInterface;
	std::unique_ptr< AnalyzerSettingInterfaceChannel >	mClockChannelInterface;
	std::unique_ptr< AnalyzerSettingInterfaceChannel >	mEnableChannelInterface;
	std::unique_ptr< AnalyzerSettingInterfaceNumberList > mShiftOrderInterface;
	std::unique_ptr< AnalyzerSettingInterfaceNumberList > mBitsPerTransferInterface;
	std::unique_ptr< AnalyzerSettingInterfaceNumberList > mClockInactiveStateInterface;
	std::unique_ptr< AnalyzerSettingInterfaceNumberList > mDataValidEdgeInterface;
	std::unique_ptr< AnalyzerSettingInterfaceNumberList > mEnableActiveStateInterface;
};

#endif //QSPI_ANALYZER_SETTINGS
