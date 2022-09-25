#pragma once

#include <json/json.h>

class JsonParser
{
public:
    JsonParser(std::string fileName);
    ~JsonParser();

public:
    Json::Value m_root;
};
