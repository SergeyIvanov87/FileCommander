#ifndef COMMAND_IDENTIFIER_H
#define COMMAND_IDENTIFIER_H
#include <array>
#include <map>

#define STRINGIFY(str) #str


enum CommandIdentifier
{
    quit,
    compareDir,
    searchDupFilesMD5,
    searchDupFilesSizeBased,
    CommandIdentifiersMax
};


inline constexpr const char *CommandIdentifierToString(CommandIdentifier cmd)
{
    constexpr std::array<const char *, CommandIdentifier::CommandIdentifiersMax> str
    {
        STRINGIFY(quit),
        STRINGIFY(compareDir),
        STRINGIFY(searchDupFilesMD5),
        STRINGIFY(searchDupFilesSizeBased),
    };

    static_assert(str.size() == CommandIdentifiersMax, "CommandIdentifierToString: CommandIdentifier and its desription inconsistent");

    return cmd < str.size() ? str[cmd] : "INVALID_COMMAND";
}

CommandIdentifier stringToCommandIdentifier(const std::string &strCmd)
{
    using StrCmdMap = std::map<std::string, CommandIdentifier>;
    static const StrCmdMap strCmdMap
    {
        {STRINGIFY(quit), quit},
        {STRINGIFY(compareDir), compareDir},
        {STRINGIFY(searchDupFilesMD5), searchDupFilesMD5},
        {STRINGIFY(searchDupFilesSizeBased), searchDupFilesSizeBased}
    };

    auto it = strCmdMap.find(strCmd.c_str());
    return it == strCmdMap.end() ? CommandIdentifier::CommandIdentifiersMax : it->second;
}

#endif //COMMAND_IDENTIFIER_H
