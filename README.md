# Project

## Usage

* Build the plugin dll via Visual Studio or msbuild
* Open a visual studio developer command prompt as administrator and sign the plugin via:
`cd path\to\sample-wsl-plugin && powershell .\sign-plugin.ps1 -PluginPath .\x64\Debug\sample-wsl-plugin.dll -Trust`

* Register the plugin with WSL via:
` reg.exe add "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Lxss\Plugins" /v sample-plugin /d path\to\sample-wsl-plugin\x64\Release\sample-wsl-plugin.dll  /t reg_sz

* Restart wslservice to load the plugin:
`sc.exe stop wslservice`

* Once loaded, open `C:\wsl-plugin-demo.txt` to see the plugin output

## Troubleshooting

Common error codes:
* `Wsl/Service/CreateInstance/CreateVm/Plugin/ERROR_MOD_NOT_FOUND` -> The plugin DLL could not be loaded. Check that the plugin registration path is correct
* `Wsl/Service/CreateInstance/CreateVm/Plugin/*` -> The plugin DLL returned an error in WSLPLUGINAPI_ENTRYPOINTV1 or OnVmStarted()
* `Wsl/Service/CreateInstance/CreateVm/Plugin/TRUST_E_NOSIGNATURE` -> The plugin DLL is not signed, or its signature is not trusted by the computer. Validate that you ran `sign-plugin.ps1`
* `Wsl/Service/CreateInstance/Plugin/*` -> The plugin DLL returned an error in OnDistributionStarted()

## Contributing

This project welcomes contributions and suggestions.  Most contributions require you to agree to a
Contributor License Agreement (CLA) declaring that you have the right to, and actually do, grant us
the rights to use your contribution. For details, visit https://cla.opensource.microsoft.com.

When you submit a pull request, a CLA bot will automatically determine whether you need to provide
a CLA and decorate the PR appropriately (e.g., status check, comment). Simply follow the instructions
provided by the bot. You will only need to do this once across all repos using our CLA.

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).
For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or
contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.

## Trademarks

This project may contain trademarks or logos for projects, products, or services. Authorized use of Microsoft 
trademarks or logos is subject to and must follow 
[Microsoft's Trademark & Brand Guidelines](https://www.microsoft.com/en-us/legal/intellectualproperty/trademarks/usage/general).
Use of Microsoft trademarks or logos in modified versions of this project must not cause confusion or imply Microsoft sponsorship.
Any use of third-party trademarks or logos are subject to those third-party's policies.
