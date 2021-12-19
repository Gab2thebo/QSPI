#include <AnalyzerHelpers.h>
#include <cstring>
#include <sstream>
#include "qspiAnalyzerSettings.h"

qspiAnalyzerSettings::qspiAnalyzerSettings()
:	mData0Channel( UNDEFINED_CHANNEL ),
	mData1Channel( UNDEFINED_CHANNEL ),
	mData2Channel( UNDEFINED_CHANNEL ),
	mData3Channel( UNDEFINED_CHANNEL ),
	mClockChannel( UNDEFINED_CHANNEL ),
	mEnableChannel( UNDEFINED_CHANNEL ),
	mClockInactiveState(BIT_LOW),
	mDummyCycles(0),
	mAddressSize(0)
{
	mData0ChannelInterface.reset( new AnalyzerSettingInterfaceChannel());
	mData0ChannelInterface->SetTitleAndTooltip( "Data 0", "Data line 0" );
	mData0ChannelInterface->SetChannel( mData0Channel );
	mData0ChannelInterface->SetSelectionOfNoneIsAllowed ( true );

	mData1ChannelInterface.reset( new AnalyzerSettingInterfaceChannel());
	mData1ChannelInterface->SetTitleAndTooltip( "Data 1", "Data line 1" );
	mData1ChannelInterface->SetChannel( mData1Channel );
	mData1ChannelInterface->SetSelectionOfNoneIsAllowed ( true );

	mData2ChannelInterface.reset( new AnalyzerSettingInterfaceChannel());
	mData2ChannelInterface->SetTitleAndTooltip( "Data 2", "Data line 2" );
	mData2ChannelInterface->SetChannel( mData2Channel );
	mData2ChannelInterface->SetSelectionOfNoneIsAllowed ( true );

	mData3ChannelInterface.reset( new AnalyzerSettingInterfaceChannel());
	mData3ChannelInterface->SetTitleAndTooltip( "Data 3", "Data line 3" );
	mData3ChannelInterface->SetChannel( mData3Channel );
	mData3ChannelInterface->SetSelectionOfNoneIsAllowed ( true );

	mClockChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mClockChannelInterface->SetTitleAndTooltip( "Clock", "Clock (CLK)" );
	mClockChannelInterface->SetChannel( mClockChannel );

	mEnableChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mEnableChannelInterface->SetTitleAndTooltip( "Enable", "Enable (SS, Slave Select)" );
	mEnableChannelInterface->SetChannel( mEnableChannel );
	mEnableChannelInterface->SetSelectionOfNoneIsAllowed( true );

	mClockInactiveStateInterface.reset(new AnalyzerSettingInterfaceNumberList());
	mClockInactiveStateInterface->SetTitleAndTooltip("Clock Polarity", "");
	mClockInactiveStateInterface->AddNumber(BIT_LOW, "Clock is Low when inactive (CPOL = 0, CPHA = 0)", "CPOL = 0 (Clock Polarity), CPHA = 0 (Clock Phase)");
	mClockInactiveStateInterface->AddNumber(BIT_HIGH, "Clock is High when inactive (CPOL = 1, CPHA = 1)", "CPOL = 1 (Clock Polarity), CPHA = 1 (Clock Phase)");
	mClockInactiveStateInterface->SetNumber(mClockInactiveState);

	mDummyCyclesInterface.reset(new AnalyzerSettingInterfaceNumberList());
	mDummyCyclesInterface->SetTitleAndTooltip("# Dummy Clock Cycles", "");
	for (U32 i = 0; i <= 7; i++)
	{
		std::stringstream ss;

		ss << i;

		mDummyCyclesInterface->AddNumber(i, ss.str().c_str(), "");
	}
	mDummyCyclesInterface->SetNumber(mDummyCycles);

	mAddressSizeInterface.reset(new AnalyzerSettingInterfaceNumberList());
	mAddressSizeInterface->SetTitleAndTooltip("Address Size (bytes)", "");
	mAddressSizeInterface->AddNumber(0, "None", "no address phase");
	mAddressSizeInterface->AddNumber(1, "One", "one byte addresses");
	mAddressSizeInterface->AddNumber(2, "Two", "two byte addresses");
	mAddressSizeInterface->AddNumber(3, "Three", "three byte addresses");
	mAddressSizeInterface->AddNumber(4, "Four", "four byte addresses");
	mAddressSizeInterface->SetNumber(mAddressSize);


	AddInterface(mEnableChannelInterface.get());
	AddInterface(mClockChannelInterface.get());
	AddInterface(mData0ChannelInterface.get());
	AddInterface(mData1ChannelInterface.get());
	AddInterface(mData2ChannelInterface.get());
	AddInterface(mData3ChannelInterface.get());
	AddInterface(mClockInactiveStateInterface.get());
	AddInterface(mDummyCyclesInterface.get());
	AddInterface(mAddressSizeInterface.get());


	AddExportOption( 0, "Export as text/csv file" );
	AddExportExtension( 0, "text", "txt" );
	AddExportExtension( 0, "csv", "csv" );

	ClearChannels();
	AddChannel(mEnableChannel, "CS", false);
	AddChannel(mClockChannel, "CLK", false);
	AddChannel(mData0Channel, "D0", false);
	AddChannel(mData1Channel, "D1", false);
	AddChannel(mData2Channel, "D2", false);
	AddChannel(mData3Channel, "D3", false);
}

qspiAnalyzerSettings::~qspiAnalyzerSettings()
{
}

bool qspiAnalyzerSettings::SetSettingsFromInterfaces()
{
	Channel data0 = mData0ChannelInterface->GetChannel();
	Channel data1 = mData1ChannelInterface->GetChannel();
	Channel data2 = mData2ChannelInterface->GetChannel();
	Channel data3 = mData3ChannelInterface->GetChannel();
	Channel clock = mClockChannelInterface->GetChannel();
	Channel enable = mEnableChannelInterface->GetChannel();

	std::vector<Channel> channels;
	channels.push_back( data0 );
	channels.push_back( data1 );
	channels.push_back( data2 );
	channels.push_back( data3 );
	channels.push_back( clock );
	channels.push_back( enable );

	if( AnalyzerHelpers::DoChannelsOverlap( &channels[0], channels.size() ) == true )
	{
		SetErrorText( "Please select different channels for each input." );
		return false;
	}
	
	if( ( data0 == UNDEFINED_CHANNEL ) && ( data1 == UNDEFINED_CHANNEL ) && ( data2 == UNDEFINED_CHANNEL ) && ( data3 == UNDEFINED_CHANNEL ) )
	{
		SetErrorText( "Please select at least one input for either Data 0, Data 1 , Data 2 or Data 3." );
		return false;
	}

	mData0Channel = mData0ChannelInterface->GetChannel();
	mData1Channel = mData1ChannelInterface->GetChannel();
	mData2Channel = mData2ChannelInterface->GetChannel();
	mData3Channel = mData3ChannelInterface->GetChannel();
	mClockChannel = mClockChannelInterface->GetChannel();
	mEnableChannel = mEnableChannelInterface->GetChannel();

	mClockInactiveState = (BitState) U32(mClockInactiveStateInterface->GetNumber());
	mDummyCycles = U32(mDummyCyclesInterface->GetNumber());
	mAddressSize = U32(mAddressSizeInterface->GetNumber());

	ClearChannels();
	AddChannel( mData0Channel, "Data 0", mData0Channel != UNDEFINED_CHANNEL );
	AddChannel( mData1Channel, "Data 1", mData1Channel != UNDEFINED_CHANNEL );
	AddChannel( mData2Channel, "Data 2", mData2Channel != UNDEFINED_CHANNEL );
	AddChannel( mData3Channel, "Data 3", mData3Channel != UNDEFINED_CHANNEL );
	AddChannel( mClockChannel, "CLOCK", mClockChannel != UNDEFINED_CHANNEL );
	AddChannel( mEnableChannel, "ENABLE", mEnableChannel != UNDEFINED_CHANNEL );

	return true;
}

void qspiAnalyzerSettings::UpdateInterfacesFromSettings()
{
	mData0ChannelInterface->SetChannel( mData0Channel );
	mData1ChannelInterface->SetChannel( mData1Channel );
	mData2ChannelInterface->SetChannel( mData2Channel );
	mData3ChannelInterface->SetChannel( mData3Channel );
	mClockChannelInterface->SetChannel( mClockChannel );
	mEnableChannelInterface->SetChannel( mEnableChannel );
	mClockInactiveStateInterface->SetNumber(mClockInactiveState);
	mDummyCyclesInterface->SetNumber(mDummyCycles);
	mAddressSizeInterface->SetNumber(mAddressSize);
}

void qspiAnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString( settings );

		const char* name_string;	//the first thing in the archive is the name of the protocol analyzer that the data belongs to.
	text_archive >> &name_string;
	if( strcmp( name_string, "SaleaeQuadSpiAnalyzer" ) != 0 )
		AnalyzerHelpers::Assert( "SaleaeQuadSpiAnalyzer: Provided with a settings string that doesn't belong to us;" );

	text_archive >>  mData0Channel;
	text_archive >>  mData1Channel;
	text_archive >>  mData2Channel;
	text_archive >>  mData3Channel;
	text_archive >>  mClockChannel;
	text_archive >>  mEnableChannel;
	text_archive >> *(U32*)&mClockInactiveState;
	text_archive >> *(U32*)&mDummyCycles;
	text_archive >> *(U32*)&mAddressSize;

	//bool success = text_archive >> mUsePackets;  //new paramater added -- do this for backwards compatibility
	//if( success == false )
	//	mUsePackets = false; //if the archive fails, set the default value

	ClearChannels();
	AddChannel( mData0Channel, "Data 0", mData0Channel != UNDEFINED_CHANNEL );
	AddChannel( mData1Channel, "Data 1", mData1Channel != UNDEFINED_CHANNEL );
	AddChannel( mData2Channel, "Data 2", mData2Channel != UNDEFINED_CHANNEL );
	AddChannel( mData3Channel, "Data 3", mData3Channel != UNDEFINED_CHANNEL );
	AddChannel( mClockChannel, "CLOCK", mClockChannel != UNDEFINED_CHANNEL );
	AddChannel( mEnableChannel, "ENABLE", mEnableChannel != UNDEFINED_CHANNEL );

	UpdateInterfacesFromSettings();
}

const char* qspiAnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	text_archive << "SaleaeQuadSpiAnalyzer";
	text_archive << mData0Channel;
	text_archive << mData1Channel;
	text_archive << mData2Channel;
	text_archive << mData3Channel;
	text_archive << mClockChannel;
	text_archive << mEnableChannel;
	text_archive << mClockInactiveState;
	text_archive << mDummyCycles;
	text_archive << mAddressSize;

	return SetReturnString( text_archive.GetString() );
}
