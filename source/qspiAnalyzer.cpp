#include "qspiAnalyzer.h"
#include "qspiAnalyzerSettings.h"
#include <AnalyzerChannelData.h>

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
	mResults.reset( new qspiAnalyzerResults( this, mSettings.get() ) );
	SetAnalyzerResults( mResults.get() );
	mResults->AddChannelBubblesWillAppearOn( mSettings->mData0Channel );
}

void qspiAnalyzer::WorkerThread()
{

		Setup();

	AdvanceToActiveEnableEdgeWithCorrectClockPolarity();

	for( ; ; )
	{
		GetWord();
		CheckIfThreadShouldExit();
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
		mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), mSettings.get() );
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 qspiAnalyzer::GetMinimumSampleRateHz()
{
	return 10000; //we don't have any idea, depends on the SPI rate, etc.; return the lowest rate.
}

const char* qspiAnalyzer::GetAnalyzerName() const
{
	return "Quad SPI analyzer";
}

const char* GetAnalyzerName()
{
	return "Quad SPI analyzer";
}

Analyzer* CreateAnalyzer()
{
	return new qspiAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}

void qspiAnalyzer::GetWord()
{
	//we're assuming we come into this function with the clock in the idle state;

	U32 bits_per_transfer = mSettings->mBitsPerTransfer;
	U64 first_sample = 0;
	bool need_reset = false;
	U64 data_word = 0;
	U8 clk_per_transfer = bits_per_transfer / 4;
	
	mArrowLocations.clear();
	ReportProgress( mClock->GetSampleNumber() );

	for ( U8 i = 0; i < clk_per_transfer; i++ ) 
	{
		if( i == 0 )
			CheckIfThreadShouldExit();

		//on every single edge, we need to check that enable doesn't toggle.
		//note that we can't just advance the enable line to the next edge, becuase there may not be another edge

		if( WouldAdvancingTheClockToggleEnable() == true )
		{
			AdvanceToActiveEnableEdgeWithCorrectClockPolarity();  //ok, we pretty much need to reset everything and return.
			return;
		}

		mClock->AdvanceToNextEdge();
		if( i == 0 ) {
		
			if( mSettings->mDataValidEdge == AnalyzerEnums::LeadingEdge )
			{
				if( mData0 != NULL )
				{
					data_word |= (mData0->GetBitState()<<0);
				}
				if( mData1 != NULL )
				{
					data_word |= (mData1->GetBitState()<<1);
				}
				if( mData2 != NULL )
				{
					data_word |= (mData2->GetBitState()<<2);
				}
				if( mData3 != NULL )
				{
					data_word |= (mData3->GetBitState()<<3);
				}
				mArrowLocations.push_back( mCurrentSample );
			}
		}
		if( i == 1 )
		{
			if( mSettings->mDataValidEdge == AnalyzerEnums::LeadingEdge )
			{
				if( mData0 != NULL )
				{
					data_word |= (mData0->GetBitState()<<4);
				}
				if( mData1 != NULL )
				{
					data_word |= (mData1->GetBitState()<<5);
				}
				if( mData2 != NULL )
				{
					data_word |= (mData2->GetBitState()<<6);
				}
				if( mData3 != NULL )
				{
					data_word |= (mData3->GetBitState()<<7);
				}
				mArrowLocations.push_back( mCurrentSample );
			}
		}
	}

	Frame result_frame;
	// result_frame.mStartingSampleInclusive = first_sample;
	result_frame.mEndingSampleInclusive = mClock->GetSampleNumber();
	result_frame.mData1 = data_word;
	result_frame.mFlags = 0;
	mResults->AddFrame( result_frame );
	
	mResults->CommitResults();

	if( need_reset == true )
		AdvanceToActiveEnableEdgeWithCorrectClockPolarity();

}

void qspiAnalyzer::AdvanceToActiveEnableEdgeWithCorrectClockPolarity()
{
	mResults->CommitPacketAndStartNewPacket();
	mResults->CommitResults();
	
	AdvanceToActiveEnableEdge();

	for( ; ; )
	{
		if( IsInitialClockPolarityCorrect() == true )  //if false, this function moves to the next active enable edge.
			break;
	}
}

void qspiAnalyzer::Setup()
{
	bool allow_last_trailing_clock_edge_to_fall_outside_enable = false;
	if( mSettings->mDataValidEdge == AnalyzerEnums::LeadingEdge )
		allow_last_trailing_clock_edge_to_fall_outside_enable = true;

	if( mSettings->mClockInactiveState == BIT_LOW )
	{
		if( mSettings->mDataValidEdge == AnalyzerEnums::LeadingEdge )
			mArrowMarker = AnalyzerResults::UpArrow;
		else
			mArrowMarker = AnalyzerResults::DownArrow;

	}else
	{
		if( mSettings->mDataValidEdge == AnalyzerEnums::LeadingEdge )
			mArrowMarker = AnalyzerResults::DownArrow;
		else
			mArrowMarker = AnalyzerResults::UpArrow;
	}


	if( mSettings->mData0Channel != UNDEFINED_CHANNEL )
		mData0 = GetAnalyzerChannelData( mSettings->mData0Channel );
	else
		mData0 = NULL;

	if( mSettings->mData1Channel != UNDEFINED_CHANNEL )
		mData1 = GetAnalyzerChannelData( mSettings->mData1Channel );
	else
		mData1 = NULL;

	if( mSettings->mData2Channel != UNDEFINED_CHANNEL )
		mData2 = GetAnalyzerChannelData( mSettings->mData2Channel );
	else
		mData2 = NULL;

	if( mSettings->mData3Channel != UNDEFINED_CHANNEL )
		mData3 = GetAnalyzerChannelData( mSettings->mData3Channel );
	else
		mData3 = NULL;

	mClock = GetAnalyzerChannelData( mSettings->mClockChannel );

	if( mSettings->mEnableChannel != UNDEFINED_CHANNEL )
		mEnable = GetAnalyzerChannelData( mSettings->mEnableChannel );
	else
		mEnable = NULL;

}

void qspiAnalyzer::AdvanceToActiveEnableEdge()
{
	if( mEnable != NULL )
	{
		if( mEnable->GetBitState() != mSettings->mEnableActiveState )
		{
			mEnable->AdvanceToNextEdge();
		}else
		{
			mEnable->AdvanceToNextEdge();
			mEnable->AdvanceToNextEdge();
		}
		mCurrentSample = mEnable->GetSampleNumber();
		mClock->AdvanceToAbsPosition( mCurrentSample );
	}else
	{
		mCurrentSample = mClock->GetSampleNumber();
	}
}

bool qspiAnalyzer::IsInitialClockPolarityCorrect()
{
	if( mClock->GetBitState() == mSettings->mClockInactiveState )
		return true;

	mResults->AddMarker( mCurrentSample, AnalyzerResults::ErrorSquare, mSettings->mClockChannel );

	if( mEnable != NULL )
	{
		Frame error_frame;
		error_frame.mStartingSampleInclusive = mCurrentSample;

		mEnable->AdvanceToNextEdge();
		mCurrentSample = mEnable->GetSampleNumber();

		error_frame.mEndingSampleInclusive = mCurrentSample;
		error_frame.mFlags = QUADSPI_ERROR_FLAG | DISPLAY_AS_ERROR_FLAG;
		mResults->AddFrame( error_frame );
		mResults->CommitResults();
		ReportProgress( error_frame.mEndingSampleInclusive );

		//move to the next active-going enable edge
		mEnable->AdvanceToNextEdge();
		mCurrentSample = mEnable->GetSampleNumber();
		mClock->AdvanceToAbsPosition( mCurrentSample );

		return false;
	}else
	{
		mClock->AdvanceToNextEdge();  //at least start with the clock in the idle state.
		mCurrentSample = mClock->GetSampleNumber();
		return true;
	}
}

bool qspiAnalyzer::WouldAdvancingTheClockToggleEnable()
{
	if( mEnable == NULL )
		return false;

	U64 next_edge = mClock->GetSampleOfNextEdge();
	bool enable_will_toggle = mEnable->WouldAdvancingToAbsPositionCauseTransition( next_edge );

	if( enable_will_toggle == false )
		return false;
	else
		return true;
}