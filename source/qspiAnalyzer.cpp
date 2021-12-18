#include <AnalyzerChannelData.h>
#include "qspiAnalyzer.h"
#include "qspiAnalyzerCommands.h"
#include "qspiAnalyzerSettings.h"
#include "AnalyzerHelpers.h"

//enum QuadSpiBubbleType { QuadSpiData, QuadSpiError };
qspiAnalyzer::qspiAnalyzer()
:	Analyzer2(),  
	mSettings( new qspiAnalyzerSettings() ),
	mSimulationInitilized( false ),
	mData0( NULL ),
	mData1( NULL ),
	mData2( NULL ),
	mData3( NULL ),
	mClock( NULL ),
	mEnable( NULL )
{
	SetAnalyzerSettings( mSettings.get() );

}

qspiAnalyzer::~qspiAnalyzer()
{
	KillThread();
}

void qspiAnalyzer::SetupResults()
{
	mResults.reset(new qspiAnalyzerResults(this, mSettings.get()));
	SetAnalyzerResults(mResults.get());

	if (mSettings->mClockChannel != UNDEFINED_CHANNEL)
		mResults->AddChannelBubblesWillAppearOn(mSettings->mClockChannel);
}

void qspiAnalyzer::WorkerThread()
{

		Setup();

	AdvanceToActiveEnableEdgeWithCorrectClockPolarity();

	for( ; ; )
	{
		GetFrame();
		CheckIfThreadShouldExit();
	}
}

void qspiAnalyzer::AdvanceToActiveEnableEdgeWithCorrectClockPolarity()
{
	mResults->CommitPacketAndStartNewPacket();
	mResults->CommitResults();

	AdvanceToActiveEnableEdge();

	for (; ; )
	{
		if (IsInitialClockPolarityCorrect() == true)  //if false, this function moves to the next active enable edge.
			break;
	}
}

void qspiAnalyzer::Setup()
{
	mArrowMarker = AnalyzerResults::UpArrow;

	if (mSettings->mData0Channel != UNDEFINED_CHANNEL)
		mData0 = GetAnalyzerChannelData(mSettings->mData0Channel);
	else
		mData0 = NULL;

	if (mSettings->mData1Channel != UNDEFINED_CHANNEL)
		mData1 = GetAnalyzerChannelData(mSettings->mData1Channel);
	else
		mData1 = NULL;

	if (mSettings->mData2Channel != UNDEFINED_CHANNEL)
		mData2 = GetAnalyzerChannelData(mSettings->mData2Channel);
	else
		mData2 = NULL;

	if (mSettings->mData3Channel != UNDEFINED_CHANNEL)
		mData3 = GetAnalyzerChannelData(mSettings->mData3Channel);
	else
		mData3 = NULL;

	mClock = GetAnalyzerChannelData(mSettings->mClockChannel);

	if (mSettings->mEnableChannel != UNDEFINED_CHANNEL)
		mEnable = GetAnalyzerChannelData(mSettings->mEnableChannel);
	else
		mEnable = NULL;

}

void qspiAnalyzer::AdvanceToActiveEnableEdge()
{
	if (mEnable != NULL)
	{
		if (mEnable->GetBitState() != BIT_LOW) // hard code to enable active low
		{
			mEnable->AdvanceToNextEdge();
		}
		else
		{
			mEnable->AdvanceToNextEdge();
			mEnable->AdvanceToNextEdge();
		}
		mCurrentSample = mEnable->GetSampleNumber();
		mClock->AdvanceToAbsPosition(mCurrentSample);
	}
	else
	{
		mCurrentSample = mClock->GetSampleNumber();
	}
}

bool qspiAnalyzer::IsInitialClockPolarityCorrect()
{
	if (mClock->GetBitState() == mSettings->mClockInactiveState)
		return true;

	mResults->AddMarker(mCurrentSample, AnalyzerResults::ErrorSquare, mSettings->mClockChannel);

	if (mEnable != NULL)
	{
		Frame error_frame;
		error_frame.mStartingSampleInclusive = mCurrentSample;

		mEnable->AdvanceToNextEdge();
		mCurrentSample = mEnable->GetSampleNumber();

		error_frame.mEndingSampleInclusive = mCurrentSample;
		error_frame.mFlags = DISPLAY_AS_ERROR_FLAG;
		mResults->AddFrame(error_frame);
		mResults->CommitResults();
		ReportProgress(error_frame.mEndingSampleInclusive);

		//move to the next active-going enable edge
		mEnable->AdvanceToNextEdge();
		mCurrentSample = mEnable->GetSampleNumber();
		mClock->AdvanceToAbsPosition(mCurrentSample);

		return false;
	}
	else
	{
		mClock->AdvanceToNextEdge();  //at least start with the clock in the idle state.
		mCurrentSample = mClock->GetSampleNumber();
		return true;
	}
}

bool qspiAnalyzer::IsParseResultError(qspiAnalyzer::ParseResult result)
{
	if (result.start >= 0 && result.end > result.start) {
        return false;
	}
	else {
        return true;
	}
}

bool qspiAnalyzer::WouldAdvancingTheClockToggleEnable()
{
	if (mEnable == NULL)
		return false;

	U64 next_edge = mClock->GetSampleOfNextEdge();
	bool enable_will_toggle = mEnable->WouldAdvancingToAbsPositionCauseTransition(next_edge);

	if (enable_will_toggle == false)
		return false;
	else
		return true;
}

void qspiAnalyzer::GetFrame()
{
	// ParseResult currentCommand;

	// CommandAttr currentCommandAttr;

	// // Get Command
	// switch (mSettings->mModeState) {
	// case 1 : currentCommand = GetCommand(0x01); //Extended mode
	// 	break;
	// case 2 : currentCommand = GetCommand(0x03); //Dual mode
	// 	break;
	// case 3 : currentCommand = GetCommand(0x0F); //Quad mode
	// 	break;
	// }

	// if(IsParseResultError(currentCommand)) {
    //     return;
    // }
    // else {
    //     SaveResults(currentCommand, FrameTypeCommand);

    //     if (IsCommandValid(currentCommand.data)==false) { //if command byte is not valid, skip forward to end of active edge
    //         AdvanceToActiveEnableEdgeWithCorrectClockPolarity();
    //         return;
    //     }
    // }

	// currentCommandAttr = GetQSPICommandAttr(currentCommand.data);


	// Get Address

	// if (currentCommandAttr.AcceptsAddr) {
	// 	ParseResult currentAddress;

	// 	switch (mSettings->mModeState) {
	// 	case 1: currentAddress = GetAddress(currentCommandAttr.AddressLineMask); //Extended mode
	// 		break;
	// 	case 2: currentAddress = GetAddress(0x03); //Dual mode
	// 		break;
	// 	case 3: currentAddress = GetAddress(0x0F); //Quad mode
	// 		break;
	// 	}
		

	// 	if(IsParseResultError(currentAddress)) {
    //         return;
	// 	}
    //     else {
    //         SaveResults(currentAddress, FrameTypeAddress);
    //     }

	// }

	// Get Dummy bits
	// if (mSettings->mDummyCycles > 0 ) {
	// 	if (currentCommandAttr.UsesDummyCycles) {
	// 		ParseResult currentDummy;
	// 		currentDummy = GetDummy();
	// 		if(IsParseResultError(currentDummy)) {
	// 			return;
	// 		}
	// 		else {
	// 			SaveResults(currentDummy, FrameTypeDummy);
	// 		}

	// 	}
	// }
	

	// Get Data
	// if (currentCommandAttr.HasData) {
	for (;;) {
		ParseResult currentData;

		// currentData = GetData(currentCommandAttr.DataLineMask);
		// switch (mSettings->mModeState) {
		// case 1: currentData = GetData(currentCommandAttr.DataLineMask); //Extended mode
		// 	break;
		// case 2: currentData = GetData(0x03); //Dual mode
		// 	break;
		// case 3: currentData = GetData(0x0F); //Quad mode
		// 	break;
		// }

		currentData = GetData(0x0F); //Quad mode

		if(IsParseResultError(currentData)) {
			return;
		}
		else {
			SaveResults(currentData, FrameTypeData);
		}
	}
	// }
}

qspiAnalyzer::ParseResult qspiAnalyzer::GetDummy()
{

	qspiAnalyzer::ParseResult return_value;


	mArrowLocations.clear();
	ReportProgress(mClock->GetSampleNumber());

	U64 first_sample = 0;

	for (U32 i = 0; i<mSettings->mDummyCycles; i++)
	{
		//on every single edge, we need to check that enable doesn't toggle.
		//note that we can't just advance the enable line to the next edge, becuase there may not be another edge

		if (WouldAdvancingTheClockToggleEnable() == true)
		{
			AdvanceToActiveEnableEdgeWithCorrectClockPolarity();  //ok, we pretty much need to reset everything and return.
			return{ -1,-1,0 }; // return values for error state
		}

		mClock->AdvanceToNextEdge(); // advance to rising edge
		if (i == 0)
			first_sample = mClock->GetSampleNumber();

		////data valid on AnalyzerEnums::LeadingEdge of clock
		mCurrentSample = mClock->GetSampleNumber();

		mArrowLocations.push_back(mCurrentSample);

		//this isn't the very last bit, etc, so proceed as normal
		if (WouldAdvancingTheClockToggleEnable() == true)
		{
			AdvanceToActiveEnableEdgeWithCorrectClockPolarity();  //ok, we pretty much need to reset everything and return.
			return{ -1,-1,0 }; // return values for error state
		}

		mClock->AdvanceToNextEdge(); // advance to falling edge
	}

	return_value.start = first_sample;
	return_value.end = mClock->GetSampleNumber();
	return_value.data = 0x00;

	return return_value;

}

qspiAnalyzer::ParseResult qspiAnalyzer::GetData(U64 DataLineMask)
{

	DataBuilder data_result;

	U64 data_word = 0;
	qspiAnalyzer::ParseResult return_value;

	data_result.Reset(&data_word, AnalyzerEnums::MsbFirst, 8); // todo: change 8 to bitspertransfer?


	mArrowLocations.clear();
	ReportProgress(mClock->GetSampleNumber());

	U64 first_sample = 0;
	//bool need_reset = false;


	// determine number of clock cycles needed

	int lines_used = 0;
	for (U32 i = 0; i < 4; i++) {
		if (DataLineMask >> i & 0x01) {
			lines_used++;
		}
	}

	for (U32 i = 0; i<(8/lines_used); i++)
	{
		//on every single edge, we need to check that enable doesn't toggle.
		//note that we can't just advance the enable line to the next edge, becuase there may not be another edge

		if (WouldAdvancingTheClockToggleEnable() == true)
		{
			AdvanceToActiveEnableEdgeWithCorrectClockPolarity();  //ok, we pretty much need to reset everything and return.
			return{ -1,-1,0 }; // return values for error state
		}

		mClock->AdvanceToNextEdge(); // advance to rising edge
		if (i == 0)
			first_sample = mClock->GetSampleNumber();

		//data valid on AnalyzerEnums::LeadingEdge of clock
		mCurrentSample = mClock->GetSampleNumber();

		if ((DataLineMask & 0x08) && (mData3 != NULL))
		{
			mData3->AdvanceToAbsPosition(mCurrentSample);
			data_result.AddBit(mData3->GetBitState());
		}
		if ((DataLineMask & 0x04) && (mData2 != NULL))
		{
			mData2->AdvanceToAbsPosition(mCurrentSample);
			data_result.AddBit(mData2->GetBitState());
		}
		if ((DataLineMask & 0x02) && (mData1 != NULL))
		{
			mData1->AdvanceToAbsPosition(mCurrentSample);
			data_result.AddBit(mData1->GetBitState());
		}
		if ((DataLineMask & 0x01) && (mData0 != NULL))
		{
			mData0->AdvanceToAbsPosition(mCurrentSample);
			data_result.AddBit(mData0->GetBitState());
		}

		mArrowLocations.push_back(mCurrentSample);


		//this isn't the very last bit, etc, so proceed as normal
		if (WouldAdvancingTheClockToggleEnable() == true)
		{
			AdvanceToActiveEnableEdgeWithCorrectClockPolarity();  //ok, we pretty much need to reset everything and return.
			return{ -1,-1,0 }; // return values for error state
		}

		mClock->AdvanceToNextEdge(); // advance to falling edge
	}

	return_value.start = first_sample;
	return_value.end = mClock->GetSampleNumber();
	return_value.data = data_word;

	return return_value;
}

qspiAnalyzer::ParseResult qspiAnalyzer::GetAddress(U64 AddressLineMask)
{

	DataBuilder address_result;

	U64 address_word = 0;
	qspiAnalyzer::ParseResult return_value;

	address_result.Reset(&address_word, AnalyzerEnums::MsbFirst, (mSettings->mAddressSize)*8); // todo: change 8 to bitspertransfer?


	mArrowLocations.clear();
	ReportProgress(mClock->GetSampleNumber());

	U64 first_sample = 0;
	//bool need_reset = false;


	// determine number of clock cycles needed
	int lines_used=0;
	for (U32 i = 0; i < 4; i++) {
		if (AddressLineMask >> i & 0x01) {
			lines_used++;
		}
	}

	for (U32 i = 0; i<((mSettings->mAddressSize) * 8/lines_used); i++)
	{
		//on every single edge, we need to check that enable doesn't toggle.
		//note that we can't just advance the enable line to the next edge, becuase there may not be another edge

		if (WouldAdvancingTheClockToggleEnable() == true)
		{
			AdvanceToActiveEnableEdgeWithCorrectClockPolarity();  //ok, we pretty much need to reset everything and return.
			return{ -1,-1,0 }; // return values for error state
		}

		mClock->AdvanceToNextEdge(); // advance to rising edge
		if (i == 0)
			first_sample = mClock->GetSampleNumber();

		//data valid on AnalyzerEnums::LeadingEdge of clock
		mCurrentSample = mClock->GetSampleNumber();

		if ((AddressLineMask & 0x08) && (mData3 != NULL))
		{
			mData3->AdvanceToAbsPosition(mCurrentSample);
			address_result.AddBit(mData3->GetBitState());
		}
		if ((AddressLineMask & 0x04) && (mData2 != NULL))
		{
			mData2->AdvanceToAbsPosition(mCurrentSample);
			address_result.AddBit(mData2->GetBitState());
		}
		if ((AddressLineMask & 0x02) && (mData1 != NULL))
		{
			mData1->AdvanceToAbsPosition(mCurrentSample);
			address_result.AddBit(mData1->GetBitState());
		}
		if ((AddressLineMask & 0x01) && (mData0 != NULL))
		{
			mData0->AdvanceToAbsPosition(mCurrentSample);
			address_result.AddBit(mData0->GetBitState());
		}

		mArrowLocations.push_back(mCurrentSample);


		//this isn't the very last bit, etc, so proceed as normal
		if (WouldAdvancingTheClockToggleEnable() == true)
		{
			AdvanceToActiveEnableEdgeWithCorrectClockPolarity();  //ok, we pretty much need to reset everything and return.
			return{ -1,-1,0 }; // return values for error state
		}

		mClock->AdvanceToNextEdge(); // advance to falling edge
	}

	return_value.start = first_sample;
	return_value.end = mClock->GetSampleNumber();
	return_value.data = address_word;

	return return_value;
}


qspiAnalyzer::ParseResult qspiAnalyzer::GetCommand(U64 CommandLineMask)
{

	DataBuilder command_result;

	U64 command_word = 0;
	qspiAnalyzer::ParseResult return_value;

	command_result.Reset(&command_word, AnalyzerEnums::MsbFirst, 8); // todo: change 8 to bitspertransfer?

	mArrowLocations.clear();
	ReportProgress(mClock->GetSampleNumber());

	U64 first_sample = 0;
	//bool need_reset = false;
	
	// determine number of clock cycles needed
	int lines_used = 0;
	for (U32 i = 0; i < 4; i++) {
		if (CommandLineMask >> i & 0x01) {
			lines_used++;
		}
	}

	
	for (U32 i = 0; i<(8 / lines_used); i++)
	{
		//on every single edge, we need to check that enable doesn't toggle.
		//note that we can't just advance the enable line to the next edge, becuase there may not be another edge

		if (WouldAdvancingTheClockToggleEnable() == true)
		{
			AdvanceToActiveEnableEdgeWithCorrectClockPolarity();  //ok, we pretty much need to reset everything and return.
			return {-1,-1,0}; // return values for error state
		}

		mClock->AdvanceToNextEdge(); // advance to rising edge
		if (i == 0)
			first_sample = mClock->GetSampleNumber();

		//data valid on AnalyzerEnums::LeadingEdge of clock
		mCurrentSample = mClock->GetSampleNumber();


		if ((CommandLineMask & 0x08) && (mData3 != NULL))
		{
			mData3->AdvanceToAbsPosition(mCurrentSample);
			command_result.AddBit(mData3->GetBitState());
		}
		if ((CommandLineMask & 0x04) && (mData2 != NULL))
		{
			mData2->AdvanceToAbsPosition(mCurrentSample);
			command_result.AddBit(mData2->GetBitState());
		}
		if ((CommandLineMask & 0x02) && (mData1 != NULL))
		{
			mData1->AdvanceToAbsPosition(mCurrentSample);
			command_result.AddBit(mData1->GetBitState());
		}
		if ((CommandLineMask & 0x01) && (mData0 != NULL))
		{
			mData0->AdvanceToAbsPosition(mCurrentSample);
			command_result.AddBit(mData0->GetBitState());
		}


		/*
		if (mData0 != NULL)
		{
			mData0->AdvanceToAbsPosition(mCurrentSample);
			mdq0_result.AddBit(mData0->GetBitState());
		}
		*/
		mArrowLocations.push_back(mCurrentSample);


		//this isn't the very last bit, etc, so proceed as normal
		if (WouldAdvancingTheClockToggleEnable() == true)
		{
			AdvanceToActiveEnableEdgeWithCorrectClockPolarity();  //ok, we pretty much need to reset everything and return.
			return {-1,-1,0 }; // return values for error state
		}

		mClock->AdvanceToNextEdge(); // advance to falling edge
	}

	return_value.start = first_sample;
	return_value.end = mClock->GetSampleNumber();
	return_value.data = command_word;

	return return_value;

}

void qspiAnalyzer::SaveResults(qspiAnalyzer::ParseResult return_value, QSPIFrameType frame_type) {
	//save the resuls:
	//U32 count = mArrowLocations.size();
	//for (U32 i = 0; i<count; i++)
	//	mResults->AddMarker(mArrowLocations[i], mArrowMarker, mSettings->mClockChannel);
    if(return_value.start > 0 && return_value.end > 0)
    {
        Frame result_frame;
        result_frame.mStartingSampleInclusive = return_value.start;
        result_frame.mEndingSampleInclusive = return_value.end;
        result_frame.mData1 = return_value.data;
        result_frame.mType = frame_type;
        result_frame.mFlags = 0;
        mResults->AddFrame(result_frame);

        mResults->CommitResults();
    }

}

bool qspiAnalyzer::NeedsRerun()
{
	return false;
}

U32 qspiAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if( mSimulationInitilized == false )
	{
		// mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), mSettings.get() );
		mSimulationInitilized = true;
	}

	// return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 qspiAnalyzer::GetMinimumSampleRateHz()
{
	return 10000; //we don't have any idea, depends on the SPI rate, etc.; return the lowest rate.
}

const char* qspiAnalyzer::GetAnalyzerName() const
{
	return "QSPI";
}

const char* GetAnalyzerName()
{
	return "QSPI";
}

Analyzer* CreateAnalyzer()
{
	return new qspiAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}
