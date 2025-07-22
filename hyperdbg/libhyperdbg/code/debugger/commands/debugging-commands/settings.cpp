/**
 * @file settings.cpp
 * @author Sina Karvandi (sina@hyperdbg.org)
 * @brief settings command
 * @details
 * @version 0.1
 * @date 2020-08-18
 *
 * @copyright This project is released under the GNU Public License v3.
 *
 */
#include "pch.h"

//
// Global Variables
//
extern BOOLEAN g_AutoUnpause;
extern BOOLEAN g_AutoFlush;
extern BOOLEAN g_AddressConversion;
extern BOOLEAN g_IsConnectedToRemoteDebuggee;
extern UINT32  g_DisassemblerSyntax;

/**
 * @brief help of the settings command
 *
 * @return VOID
 */
VOID
CommandSettingsHelp()
{
    ShowMessages(
        "settings : queries, sets, or changes a value for a special settings option.\n\n");

    ShowMessages("syntax : \tsettings [OptionName (string)]\n");
    ShowMessages("syntax : \tsettings [OptionName (string)] [Value (hex)]\n");
    ShowMessages("syntax : \tsettings [OptionName (string)] [Value (string)]\n");
    ShowMessages("syntax : \tsettings [OptionName (string)] [on|off]\n");

    ShowMessages("\n");
    ShowMessages("\t\te.g : settings autounpause\n");
    ShowMessages("\t\te.g : settings autounpause on\n");
    ShowMessages("\t\te.g : settings autounpause off\n");
    ShowMessages("\t\te.g : settings addressconversion on\n");
    ShowMessages("\t\te.g : settings addressconversion off\n");
    ShowMessages("\t\te.g : settings autoflush on\n");
    ShowMessages("\t\te.g : settings autoflush off\n");
    ShowMessages("\t\te.g : settings syntax intel\n");
    ShowMessages("\t\te.g : settings syntax att\n");
    ShowMessages("\t\te.g : settings syntax masm\n");
}

/**
 * @brief Gets the setting values from config file
 *
 * @param OptionName
 * @param OptionValue
 * @return BOOLEAN Shows if the settings is available or not
 */
BOOLEAN
CommandSettingsGetValueFromConfigFile(std::string OptionName, std::string & OptionValue)
{
    inipp::Ini<char> Ini;
    WCHAR            ConfigPath[MAX_PATH] = {0};
    std::string      OptionValueFromFile;

    //
    // Get config file path
    //
    GetConfigFilePath(ConfigPath);

    if (!IsFileExistW(ConfigPath))
    {
        return FALSE;
    }

    //
    // Open the file
    //
    ifstream Is(ConfigPath);

    //
    // Read config file
    //
    Ini.parse(Is);

    //
    // Show config file
    //
    // Ini.generate(std::cout);

    inipp::get_value(Ini.sections["DEFAULT"], OptionName, OptionValueFromFile);

    Is.close();

    if (!OptionValueFromFile.empty())
    {
        OptionValue = OptionValueFromFile;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/**
 * @brief Sets the setting values from config file
 *
 * @param OptionName
 * @param OptionValue
 *
 * @return VOID
 */
VOID
CommandSettingsSetValueFromConfigFile(std::string OptionName, std::string OptionValue)
{
    inipp::Ini<char> Ini;
    WCHAR            ConfigPath[MAX_PATH] = {0};

    //
    // Get config file path
    //
    GetConfigFilePath(ConfigPath);

    ifstream Is(ConfigPath);

    //
    // Read config file
    //
    Ini.parse(Is);

    Is.close();

    //
    // Save the config
    //
    Ini.sections["DEFAULT"][OptionName] = OptionValue.c_str();
    Ini.interpolate();

    //
    // Test, show the config
    //
    // Ini.generate(std::cout);

    //
    // Save the config
    //
    ofstream Os(ConfigPath);

    Ini.generate(Os);

    Os.close();
}

/**
 * @brief Loads default settings values from config file
 *
 * @return VOID
 */
VOID
CommandSettingsLoadDefaultValuesFromConfigFile()
{
    string OptionValue;

    //
    // *** Set default configurations ***
    //

    //
    // Set the assembly syntax
    //
    if (CommandSettingsGetValueFromConfigFile("AsmSyntax", OptionValue))
    {
        //
        // The user tries to set a value as the syntax
        //
        if (!OptionValue.compare("intel"))
        {
            g_DisassemblerSyntax = 1;
        }
        else if (!OptionValue.compare("att") ||
                 !OptionValue.compare("at&t"))
        {
            g_DisassemblerSyntax = 2;
        }
        else if (!OptionValue.compare("masm"))
        {
            g_DisassemblerSyntax = 3;
        }
        else
        {
            //
            // Sth is incorrect
            //
            ShowMessages("err, incorrect assembly syntax settings\n");
        }
    }

    //
    // Set the auto unpause
    //
    if (CommandSettingsGetValueFromConfigFile("AutoUnpause", OptionValue))
    {
        if (!OptionValue.compare("on"))
        {
            g_AutoUnpause = TRUE;
        }
        else if (!OptionValue.compare("off"))
        {
            g_AutoUnpause = FALSE;
        }
        else
        {
            //
            // Sth is incorrect
            //
            ShowMessages("err, incorrect auto unpause settings\n");
        }
    }

    //
    // Set the auto flush
    //
    if (CommandSettingsGetValueFromConfigFile("AutoFlush", OptionValue))
    {
        if (!OptionValue.compare("on"))
        {
            g_AutoFlush = TRUE;
        }
        else if (!OptionValue.compare("off"))
        {
            g_AutoFlush = FALSE;
        }
        else
        {
            //
            // Sth is incorrect
            //
            ShowMessages("err, incorrect auto flush settings\n");
        }
    }

    //
    // Set the address conversion
    //
    if (CommandSettingsGetValueFromConfigFile("AddrConv", OptionValue))
    {
        if (!OptionValue.compare("on"))
        {
            g_AddressConversion = TRUE;
        }
        else if (!OptionValue.compare("off"))
        {
            g_AddressConversion = FALSE;
        }
        else
        {
            //
            // Sth is incorrect
            //
            ShowMessages("err, incorrect address conversion settings\n");
        }
    }
}

/**
 * @brief set the address conversion enabled and disabled
 * and query the status of this mode
 *
 * @param CommandTokens
 * @return VOID
 */
VOID
CommandSettingsAddressConversion(vector<CommandToken> CommandTokens)
{
    if (CommandTokens.size() == 2)
    {
        //
        // It's a query
        //
        if (g_AddressConversion)
        {
            ShowMessages("address conversion is enabled\n");
        }
        else
        {
            ShowMessages("address conversion is disabled\n");
        }
    }
    else if (CommandTokens.size() == 3)
    {
        //
        // The user tries to set a value as the autoflush
        //
        if (CompareLowerCaseStrings(CommandTokens.at(2), "on"))
        {
            g_AddressConversion = TRUE;
            CommandSettingsSetValueFromConfigFile("AddrConv", "on");

            ShowMessages("set address conversion to enabled\n");
        }
        else if (CompareLowerCaseStrings(CommandTokens.at(2), "off"))
        {
            g_AddressConversion = FALSE;
            CommandSettingsSetValueFromConfigFile("AddrConv", "off");

            ShowMessages("set address conversion to disabled\n");
        }
        else
        {
            //
            // Sth is incorrect
            //
            ShowMessages("incorrect use of the '%s', please use 'help %s' for more information\n",
                         GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str(),
                         GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());

            return;
        }
    }
    else
    {
        //
        // Sth is incorrect
        //
        ShowMessages("incorrect use of the '%s', please use 'help %s' for more information\n",
                     GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str(),
                     GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
        return;
    }
}

/**
 * @brief set the auto-flush mode to enabled and disabled
 * and query the status of this mode
 *
 * @param CommandTokens
 * @return VOID
 */
VOID
CommandSettingsAutoFlush(vector<CommandToken> CommandTokens)
{
    if (CommandTokens.size() == 2)
    {
        //
        // It's a query
        //
        if (g_AutoFlush)
        {
            ShowMessages("auto-flush is enabled\n");
        }
        else
        {
            ShowMessages("auto-flush is disabled\n");
        }
    }
    else if (CommandTokens.size() == 3)
    {
        //
        // The user tries to set a value as the autoflush
        //
        if (CompareLowerCaseStrings(CommandTokens.at(2), "on"))
        {
            g_AutoFlush = TRUE;
            CommandSettingsSetValueFromConfigFile("AutoFlush", "on");

            ShowMessages("set auto-flush to enabled\n");
        }
        else if (CompareLowerCaseStrings(CommandTokens.at(2), "off"))
        {
            g_AutoFlush = FALSE;
            CommandSettingsSetValueFromConfigFile("AutoFlush", "off");

            ShowMessages("set auto-flush to disabled\n");
        }
        else
        {
            //
            // Sth is incorrect
            //
            ShowMessages("incorrect use of the '%s', please use 'help %s' for more information\n",
                         GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str(),
                         GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
            return;
        }
    }
    else
    {
        //
        // Sth is incorrect
        //
        ShowMessages("incorrect use of the '%s', please use 'help %s' for more information\n",
                     GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str(),
                     GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
        return;
    }
}

/**
 * @brief set auto-unpause mode to enabled or disabled
 *
 * @param CommandTokens
 * @return VOID
 */
VOID
CommandSettingsAutoUpause(vector<CommandToken> CommandTokens)
{
    if (CommandTokens.size() == 2)
    {
        //
        // It's a query
        //
        if (g_AutoUnpause)
        {
            ShowMessages("auto-unpause is enabled\n");
        }
        else
        {
            ShowMessages("auto-unpause is disabled\n");
        }
    }
    else if (CommandTokens.size() == 3)
    {
        //
        // The user tries to set a value as the autounpause
        //
        if (CompareLowerCaseStrings(CommandTokens.at(2), "on"))
        {
            g_AutoUnpause = TRUE;
            CommandSettingsSetValueFromConfigFile("AutoUnpause", "on");

            ShowMessages("set auto-unpause to enabled\n");
        }
        else if (CompareLowerCaseStrings(CommandTokens.at(2), "off"))
        {
            g_AutoUnpause = FALSE;
            CommandSettingsSetValueFromConfigFile("AutoUnpause", "off");

            ShowMessages("set auto-unpause to disabled\n");
        }
        else
        {
            //
            // Sth is incorrect
            //
            ShowMessages("incorrect use of the '%s', please use 'help %s' for more information\n",
                         GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str(),
                         GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
            return;
        }
    }
    else
    {
        //
        // Sth is incorrect
        //
        ShowMessages("incorrect use of the '%s', please use 'help %s' for more information\n",
                     GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str(),
                     GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
        return;
    }
}

/**
 * @brief set the syntax of !u !u2 u u2 command
 *
 * @param CommandTokens
 * @return VOID
 */
VOID
CommandSettingsSyntax(vector<CommandToken> CommandTokens)
{
    if (CommandTokens.size() == 2)
    {
        //
        // It's a query
        //
        if (g_DisassemblerSyntax == 1)
        {
            ShowMessages("disassembler syntax is : intel\n");
        }
        else if (g_DisassemblerSyntax == 2)
        {
            ShowMessages("disassembler syntax is : at&t\n");
        }
        else if (g_DisassemblerSyntax == 3)
        {
            ShowMessages("disassembler syntax is : masm\n");
        }
        else
        {
            ShowMessages("unknown syntax\n");
        }
    }
    else if (CommandTokens.size() == 3)
    {
        //
        // The user tries to set a value as the syntax
        //
        if (CompareLowerCaseStrings(CommandTokens.at(2), "intel"))
        {
            g_DisassemblerSyntax = 1;
            CommandSettingsSetValueFromConfigFile("AsmSyntax", "intel");

            ShowMessages("set syntax to intel\n");
        }
        else if (CompareLowerCaseStrings(CommandTokens.at(2), "att") ||
                 CompareLowerCaseStrings(CommandTokens.at(2), "at&t"))
        {
            g_DisassemblerSyntax = 2;
            CommandSettingsSetValueFromConfigFile("AsmSyntax", "att");

            ShowMessages("set syntax to at&t\n");
        }
        else if (CompareLowerCaseStrings(CommandTokens.at(2), "masm"))
        {
            g_DisassemblerSyntax = 3;
            CommandSettingsSetValueFromConfigFile("AsmSyntax", "masm");

            ShowMessages("set syntax to masm\n");
        }
        else
        {
            //
            // Sth is incorrect
            //
            ShowMessages("incorrect use of the '%s', please use 'help %s' for more information\n",
                         GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str(),
                         GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
            return;
        }
    }
    else
    {
        //
        // Sth is incorrect
        //
        ShowMessages("incorrect use of the '%s', please use 'help %s' for more information\n",
                     GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str(),
                     GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
        return;
    }
}

/**
 * @brief settings command handler
 *
 * @param CommandTokens
 * @param Command
 * @return VOID
 */
VOID
CommandSettings(vector<CommandToken> CommandTokens, string Command)
{
    if (CommandTokens.size() <= 1)
    {
        ShowMessages("incorrect use of the '%s'\n\n",
                     GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
        CommandSettingsHelp();
        return;
    }

    //
    // Interpret the field name
    //
    if (CompareLowerCaseStrings(CommandTokens.at(1), "autounpause"))
    {
        //
        // Handle it locally
        //
        CommandSettingsAutoUpause(CommandTokens);
    }
    else if (CompareLowerCaseStrings(CommandTokens.at(1), "syntax"))
    {
        //
        // If it's a remote debugger then we send it to the remote debugger
        //
        if (g_IsConnectedToRemoteDebuggee)
        {
            RemoteConnectionSendCommand(Command.c_str(), (UINT32)Command.length() + 1);
        }
        else
        {
            //
            // If it's a connection over serial or a local debugging then
            // we handle it locally
            //
            CommandSettingsSyntax(CommandTokens);
        }
    }
    else if (CompareLowerCaseStrings(CommandTokens.at(1), "autoflush"))
    {
        //
        // If it's a remote debugger then we send it to the remote debugger
        //
        if (g_IsConnectedToRemoteDebuggee)
        {
            RemoteConnectionSendCommand(Command.c_str(), (UINT32)Command.length() + 1);
        }
        else
        {
            //
            // If it's a connection over serial or a local debugging then
            // we handle it locally
            //
            CommandSettingsAutoFlush(CommandTokens);
        }
    }
    else if (CompareLowerCaseStrings(CommandTokens.at(1), "addressconversion"))
    {
        //
        // If it's a remote debugger then we send it to the remote debugger
        //
        if (g_IsConnectedToRemoteDebuggee)
        {
            RemoteConnectionSendCommand(Command.c_str(), (UINT32)Command.length() + 1);
        }
        else
        {
            //
            // If it's a connection over serial or a local debugging then
            // we handle it locally
            //
            CommandSettingsAddressConversion(CommandTokens);
        }
    }
    else
    {
        //
        // option not found
        //
        ShowMessages("incorrect use of the '%s', please use 'help %s' for more information\n",
                     GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str(),
                     GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
        return;
    }
}
