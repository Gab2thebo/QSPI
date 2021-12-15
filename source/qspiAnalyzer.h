#ifndef QSPI_ANALYZER_H
#define QSPI_ANALYZER_H

#include <Analyzer.h>
#include "qspiAnalyzerResults.h"
#include "qspiSimulationDataGenerator.h"

class qspiAnalyzerSettings;
class ANALYZER_EXPORT qspiAnalyzer : public Analyzer2
{
public:
	qspiAnalyzer();
	virtual ~qspiAnalyzer();

	virtual void SetupResults();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();

protected: //vars
	std::auto_ptr< qspiAnalyzerSettings > mSettings;
	std::auto_ptr< qspiAnalyzerResults > mResults;
	AnalyzerChannelData* mSerial;

	qspiSimulationDataGenerator mSimulationDataGenerator;
	bool mSimulationInitilized;

	//Serial analysis vars:
	U32 mSampleRateHz;
	U32 mStartOfStopBitOffset;
	U32 mEndOfStopBitOffset;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //QSPI_ANALYZER_H
