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
	mShiftOrder( AnalyzerEnums::MsbFirst ),
	mBitsPerTransfer( 8 ),
	mClockInactiveState( BIT_LOW ),
	mDataValidEdge( AnalyzerEnums::LeadingEdge ), 
	mEnableActiveState( BIT_LOW )
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

	mShiftOrderInterface.reset( new AnalyzerSettingInterfaceNumberList() );
	mShiftOrderInterface->SetTitleAndTooltip( "Significant Bit", "" );
	mShiftOrderInterface->AddNumber( AnalyzerEnums::MsbFirst, "Most Significant Bit First (Standard)", "Select if the most significant bit or least significant bit is transmitted first" );
	mShiftOrderInterface->AddNumber( AnalyzerEnums::LsbFirst, "Least Significant Bit First", "" );
	mShiftOrderInterface->SetNumber( mShiftOrder );

	mBitsPerTransferInterface->SetTitleAndTooltip( "Bits per Transfer", "" );
	for( U32 i=1; i<=64; i++ )
	{
		std::stringstream ss;

		if( i == 8 )
			ss << "8 Bits per Transfer (Standard)";
		else
			ss << i << " Bits per Transfer";
		
		mBitsPerTransferInterface->AddNumber( i, ss.str().c_str(), "" );
	}
	mBitsPerTransferInterface->SetNumber( mBitsPerTransfer );

	mClockInactiveStateInterface.reset( new AnalyzerSettingInterfaceNumberList() );
	mClockInactiveStateInterface->SetTitleAndTooltip( "Clock State", "" );
	mClockInactiveStateInterface->AddNumber( BIT_LOW, "Clock is Low when inactive (CPOL = 0)", "CPOL = 0 (Clock Polarity)" );
	mClockInactiveStateInterface->AddNumber( BIT_HIGH, "Clock is High when inactive (CPOL = 1)", "CPOL = 1 (Clock Polarity)" );
	mClockInactiveStateInterface->SetNumber( mClockInactiveState );

	mDataValidEdgeInterface.reset( new AnalyzerSettingInterfaceNumberList() );
	mDataValidEdgeInterface->SetTitleAndTooltip( "Clock Phase", "" );
	mDataValidEdgeInterface->AddNumber( AnalyzerEnums::LeadingEdge, "Data is Valid on Clock Leading Edge (CPHA = 0)", "CPHA = 0 (Clock Phase)" );
	mDataValidEdgeInterface->AddNumber( AnalyzerEnums::TrailingEdge, "Data is Valid on Clock Trailing Edge (CPHA = 1)", "CPHA = 1 (Clock Phase)" );
	mDataValidEdgeInterface->SetNumber( mDataValidEdge );

	mEnableActiveStateInterface.reset( new AnalyzerSettingInterfaceNumberList() );
	mEnableActiveStateInterface->SetTitleAndTooltip( "Enable Line", "" );
	mEnableActiveStateInterface->AddNumber( BIT_LOW, "Enable line is Active Low (Standard)", "" );
	mEnableActiveStateInterface->AddNumber( BIT_HIGH, "Enable line is Active High", "" );
	mEnableActiveStateInterface->SetNumber( mEnableActiveState );

	AddInterface( mData0ChannelInterface.get() );
	AddInterface( mData1ChannelInterface.get() );
	AddInterface( mData2ChannelInterface.get() );
	AddInterface( mData3ChannelInterface.get() );
	AddInterface( mClockChannelInterface.get() );
	AddInterface( mEnableChannelInterface.get() );
	AddInterface( mShiftOrderInterface.get() );
	AddInterface( mBitsPerTransferInterface.get() );
	AddInterface( mClockInactiveStateInterface.get() );
	AddInterface( mDataValidEdgeInterface.get() );
	AddInterface( mEnableActiveStateInterface.get() );

	//AddExportOption( 0, "Export as text/csv file", "text (*.txt);;csv (*.csv)" );
	AddExportOption( 0, "Export as text/csv file" );
	AddExportExtension( 0, "text", "txt" );
	AddExportExtension( 0, "csv", "csv" );

	ClearChannels();
	AddChannel( mData0Channel, "Data 0", false );
	AddChannel( mData1Channel, "Data 1", false );
	AddChannel( mData2Channel, "Data 2", false );
	AddChannel( mData3Channel, "Data 3", false );
	AddChannel( mClockChannel, "CLOCK", false );
	AddChannel( mEnableChannel, "ENABLE", false );
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

	mShiftOrder =			(AnalyzerEnums::ShiftOrder) U32( mShiftOrderInterface->GetNumber() );
	mBitsPerTransfer =		U32( mBitsPerTransferInterface->GetNumber() );
	mClockInactiveState =	(BitState) U32( mClockInactiveStateInterface->GetNumber() );
	mDataValidEdge =		(AnalyzerEnums::Edge)  U32( mDataValidEdgeInterface->GetNumber() );
	mEnableActiveState =	(BitState) U32( mEnableActiveStateInterface->GetNumber() );

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
	mBitsPerTransferInterface->SetNumber( mBitsPerTransfer );
	mShiftOrderInterface->SetNumber( mShiftOrder );
	mDataValidEdgeInterface->SetNumber( mDataValidEdge );
	mClockInactiveStateInterface->SetNumber( mClockInactiveState );
	mDataValidEdgeInterface->SetNumber( mDataValidEdge );
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
	text_archive >>  *(U32*)&mShiftOrder;
	text_archive >>  mBitsPerTransfer;
	text_archive >>  *(U32*)&mClockInactiveState;
	text_archive >>  *(U32*)&mDataValidEdge;
	text_archive >>  *(U32*)&mEnableActiveState;

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
	text_archive <<  mData0Channel;
	text_archive <<  mData1Channel;
	text_archive <<  mData2Channel;
	text_archive <<  mData3Channel;
	text_archive <<  mClockChannel;
	text_archive <<  mEnableChannel;
	text_archive <<  mShiftOrder;
	text_archive <<  mBitsPerTransfer;
	text_archive <<  mClockInactiveState;
	text_archive <<  mDataValidEdge;
	text_archive <<  mEnableActiveState;

	return SetReturnString( text_archive.GetString() );
}
