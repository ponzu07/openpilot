//=============================================================================
//
<<<<<<< HEAD
//  Copyright (c) 2015 Qualcomm Technologies, Inc.
=======
//  Copyright (c) 2015, 2020 Qualcomm Technologies, Inc.
>>>>>>> origin/ci-clean
//  All Rights Reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================
#ifndef __DIAGLOG_OPTIONS_HPP_
#define __DIAGLOG_OPTIONS_HPP_

<<<<<<< HEAD
#ifndef ZDL_LOGGING_EXPORT
#define ZDL_LOGGING_EXPORT __attribute__((visibility("default")))
#endif

#include <string>
#include <set>
=======
#include <string>
#include "DlSystem/ZdlExportDefine.hpp"
>>>>>>> origin/ci-clean

namespace zdl
{
namespace DiagLog
{
/** @addtogroup c_plus_plus_apis C++
@{ */

/// @brief .
///
/// Options for setting up diagnostic logging for zdl components.
<<<<<<< HEAD
class ZDL_LOGGING_EXPORT Options
=======
class ZDL_EXPORT Options
>>>>>>> origin/ci-clean
{
public:
   Options() :
      DiagLogMask(""),
      LogFileDirectory("diaglogs"),
      LogFileName("DiagLog"),
      LogFileRotateCount(20),
      LogFileReplace(true)
   {
      // Solves the empty string problem with multiple std libs
      DiagLogMask.reserve(1);
   }

   /// @brief .
<<<<<<< HEAD
   /// 
=======
   ///
>>>>>>> origin/ci-clean
   /// Enables diag logging only on the specified area mask (DNN_RUNTIME=ON | OFF)
   std::string DiagLogMask;

   /// @brief .
<<<<<<< HEAD
   /// 
=======
   ///
>>>>>>> origin/ci-clean
   /// The path to the directory where log files will be written.
   /// The path may be relative or absolute. Relative paths are interpreted
   /// from the current working directory.
   /// Default value is "diaglogs"
   std::string LogFileDirectory;

   /// @brief .
<<<<<<< HEAD
   /// 
=======
   ///
>>>>>>> origin/ci-clean
   //// The name used for log files. If this value is empty then BaseName will be
   /// used as the default file name.
   /// Default value is "DiagLog"
   std::string LogFileName;

   /// @brief .
<<<<<<< HEAD
   /// 
   /// The maximum number of log files to create. If set to 0 no log rotation 
=======
   ///
   /// The maximum number of log files to create. If set to 0 no log rotation
>>>>>>> origin/ci-clean
   /// will be used and the log file name specified will be used each time, overwriting
   /// any existing log file that may exist.
   /// Default value is 20
   uint32_t LogFileRotateCount;

   /// @brief
   ///
   /// If the log file already exists, control whether it will be replaced
   /// (existing contents truncated), or appended.
   /// Default value is true
   bool LogFileReplace;
};
/** @} */ /* end_addtogroup c_plus_plus_apis C++ */

} // DiagLog namespace
} // zdl namespace


#endif
