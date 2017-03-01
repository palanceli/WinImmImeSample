// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <tchar.h>
#include "boost/program_options.hpp"
#include "boost/log/trivial.hpp"
#include "boost/log/core.hpp"
#include "boost/log/expressions.hpp"

int _tmain(int argc, TCHAR* argv[])
{
  namespace po = boost::program_options;
  po::options_description desc;
  desc.add_options()
    ("imePath", po::wvalue<std::wstring>(), "ime path")
    ("imeName", po::wvalue<std::wstring>(), "ime name");
  po::variables_map vm;
  po::basic_parsed_options<wchar_t> parsed = po::wcommand_line_parser(argc, argv).options(desc).run();
  po::store(parsed, vm);

  std::wstring imePath = vm["imePath"].as<std::wstring>();
  std::wstring imeName = vm["imeName"].as<std::wstring>();

  TCHAR szPath[MAX_PATH] = { 0 };
  if (!GetSystemDirectory(szPath, MAX_PATH)) {
    BOOST_LOG_TRIVIAL(error) << _T("System dir not found!");
    return -1;
  }
  std::wstring systemDir = szPath;
  BOOST_LOG_TRIVIAL(trace) << _T("System dir :") << systemDir;

  size_t pos = imePath.rfind('\\');
  std::wstring imeFileName = imePath.substr(pos);

  std::wstring imeFilePathInSys = systemDir;
  imeFilePathInSys += imeFileName;

  DWORD dwRetVal = GetTempFileName(systemDir.c_str(), _T("tempime"), 0, szPath);
  if (dwRetVal == 0) {
    BOOST_LOG_TRIVIAL(error)<<_T("Failed to generate temp file name.\n");
    return -1;
  }
  std::wstring tempImeFilePath = szPath;
  BOOST_LOG_TRIVIAL(trace) << _T("temp file name:") << tempImeFilePath;

  MoveFile(imeFilePathInSys.c_str(), tempImeFilePath.c_str());
  MoveFileEx(tempImeFilePath.c_str(), NULL, MOVEFILE_COPY_ALLOWED);

  BOOST_LOG_TRIVIAL(trace) << _T("[move file]") << imeFilePathInSys << _T("=>") << tempImeFilePath;
  CopyFile(imePath.c_str(), imeFilePathInSys.c_str(), FALSE);
  BOOST_LOG_TRIVIAL(trace) << _T("[move file]") << imePath << _T("=>") << imeFilePathInSys;

  ImmInstallIME(imeFilePathInSys.c_str(), imeName.c_str());

  return 0;
}