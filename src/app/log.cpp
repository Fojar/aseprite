// Aseprite
// Copyright (C) 2001-2015  David Capello
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "app/log.h"

#include "app/app.h"
#include "app/resource_finder.h"
#include "ui/base.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>

namespace app {

static FILE* log_fileptr = NULL;
static LoggerModule* logger_instance = NULL;

LoggerModule::LoggerModule(bool verbose)
  : m_verbose(verbose)
{
  logger_instance = this;
}

LoggerModule::~LoggerModule()
{
  LOG("Logger module: shutting down (this is the last line)\n");

  if (log_fileptr) {
    fclose(log_fileptr);
    log_fileptr = NULL;
  }

  logger_instance = NULL;
}

} // namespace app

void verbose_log(const char* format, ...)
{
  if (app::logger_instance && !app::logger_instance->isVerbose())
    return;

  if (!app::log_fileptr) {
    std::string filename;
    app::ResourceFinder rf(false);
    rf.includeUserDir("aseprite.log");
    filename = rf.defaultFilename();

    if (filename.size() > 0)
      app::log_fileptr = fopen(filename.c_str(), "w");
  }

  if (app::log_fileptr) {
    va_list ap;
    va_start(ap, format);

    vfprintf(app::log_fileptr, format, ap);
    fflush(app::log_fileptr);

#ifdef _DEBUG
    vfprintf(stdout, format, ap);
    fflush(stdout);
#endif

    va_end(ap);
  }

}

#include <Windows.h>
void PRINT_TO_VS_OUTPUT_WINDOW(const char* format, ...)
{
  const int BUFFER_SIZE = 1000;
  char buffer[BUFFER_SIZE];
  va_list arg;
  va_start(arg, format);
  _vsnprintf_s(buffer, _countof(buffer), _TRUNCATE, format, arg);
  va_end(arg);

  // Convert to a wchar_t*
  size_t origsize = strlen(buffer) + 1;
  const size_t newsize = BUFFER_SIZE;
  size_t convertedChars = 0;
  wchar_t wcstring[newsize];
  mbstowcs_s(&convertedChars, wcstring, origsize, buffer, _TRUNCATE);

  OutputDebugString(wcstring);
}
