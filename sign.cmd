
:: First command line argument is the password for the code signing license

set pwd=%cd%

pushd \\lina\codelicense

"C:\Program Files (x86)\Windows Kits\10\bin\10.0.15063.0\x64\signtool.exe" sign /fd SHA512 /f Roboception_GmbH.p12 /p %1 /tr http://tsa.swisssign.net "%pwd%\rcdiscover.exe"" %pwd%\rcdiscover-gui.exe"

popd