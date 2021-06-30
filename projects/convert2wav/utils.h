#include <iostream>
#include <vector>
#pragma once

// STRING END WITH
//------------------------------------------------------------------------------
static bool EndsWithString(std::string const & value, 
  std::string const & ending)
{
  if (ending.size() > value.size()) return false;
  return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

// STRING START WITH
//------------------------------------------------------------------------------
static bool StartsWithString(std::string const & value, 
  std::string const & starting)
{
  if (starting.size() > value.size()) return false;
  return std::equal(starting.begin(), starting.end(), value.begin());
}

// SPLIT STRING
//------------------------------------------------------------------------------
static std::vector<std::string> SplitString(const std::string& s, 
  const std::string& delim)
{
  std::vector<std::string> tokens;
  
  std::size_t start = s.find_first_not_of(delim), end = 0;
  if(start == 1)tokens.push_back("");

  while((end = s.find_first_of(delim, start)) != std::string::npos)
  {
    if(end-start == 1)tokens.push_back("");
    else tokens.push_back(s.substr(start, end - start));
    start = s.find_first_not_of(delim, end);
  }
  if(start != std::string::npos)
  tokens.push_back(s.substr(start));
  
  return tokens;
}

// GET EXTENSION
//------------------------------------------------------------------------------
static std::string GetExtension(const std::string& filename)
{
  const std::vector<std::string> tokens = SplitString(filename, ".");
  if(tokens.size() > 1) {
    return std::string(".")+tokens.back();
  } else {
    return "";
  }
}