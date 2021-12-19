To meet our needs, these source files are extracted from the original sdk by the script tool of config.bat and extract.bat.

There is a project called Ali_Linkit_Lib.uvprojx which use these source files to generate ali_linkit_sdk.lib.

If you want to get more feature from the original sdk, please refer to the current config file of make.settings before extract again.

Reference flows are as follow:
1. In the iotkit-embedded-3.2.0:
    cp ./lib_iot_sdk_src/make.settings .config
    config.bat
2. Enable or disable feature interaction.
3. Extract src files:
    extract.bat
  All your need files are put in the folder ./output
5. Compare ./output and ./lib_iot_sdk_src, add files to ./lib_iot_sdk_src or remove files from it.
   Some files as infra_config.h, ali_linkit_sdk_include.h, wrappers.c/h also need to modify
Note: ali_linkit_sdk_include.h is corresponded to the sdk_include.h
6. Open Ali_Linkit_Lib.uvprojx, add or remove file from project, and build the project
7. Copy the new ali_linkit_sdk.lib to the ../lib/ to update and save lib file.
8. Copy the make.settings to the lib_iot_sdk_src to save config file.