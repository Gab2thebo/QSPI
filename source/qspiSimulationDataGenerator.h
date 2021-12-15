#ifndef QSPI_SIMULATION_DATA_GENERATOR
#define QSPI_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include <string>
class qspiAnalyzerSettings;

class qspiSimulationDataGenerator
{
public:
	qspiSimulationDataGenerator();
	~qspiSimulationDataGenerator();

	void Initialize( U32 simulation_sample_rate, qspiAnalyzerSettings* settings );
	U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel );

protected:
	qspiAnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;

protected:
	void CreateSerialByte();
	std::string mSerialText;
	U32 mStringIndex;

	SimulationChannelDescriptor mSerialSimulationData;

};
#endif //QSPI_SIMULATION_DATA_GENERATOR