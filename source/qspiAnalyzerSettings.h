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
	void UpdateInterfacesFromSettings();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	
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
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mData0ChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mData1ChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mData2ChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mData3ChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mClockChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mEnableChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList > mShiftOrderInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList > mBitsPerTransferInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList > mClockInactiveStateInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList > mDataValidEdgeInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList > mEnableActiveStateInterface;
};


#endif //QSPI_ANALYZER_SETTINGS
