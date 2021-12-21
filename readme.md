# SR1120 QSPI Saleae analyzer

Custom Saleae Quad Serial Peripheral Interface (QSPI) Analyzer

## Getting Started

### Linux

Dependencies:

* CMake 3.11+
* gcc 9.3.0 x86-linux-gnu
* Visual Code (optional)

Build the analyser using VScode or run these commands in a terminal from within the tool root folder.

```
mkdir build
cd build
cmake ..
cmake --build .
```

The newly created analyser file will be located here: ``build/Analyzers/libqspi_analyzer.so``

#### Adding QuadSPI analyzer to Saleae Logic 2

1. Open the Saleae Logic 2.
2. Open the preference panel `Ctrl+,` .
3. Navigate to Custom Low Level analyzers and set the path to the `libqspi_analyser.so` file location.
4. Restart the Saleae Logic 2.



------

**Disclaimer**

This QSPI Saleae analyser is based of these three projects: 

1. Third party flash QSPI analyser https://github.com/dedicatedcomputing/saleae_qspi
2. Saleae Official Sample analyser https://github.com/saleae/SampleAnalyzer
3. Saleae Official SPI analyser https://github.com/saleae/spi-analyzer

